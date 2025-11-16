# TemplateBinding 和链式绑定 API 实现说明

## 概述

本次实现完成了以下三个主要功能：
1. TemplateBinding 自动绑定到 TemplatedParent
2. 控件工厂函数
3. 链式绑定语法 (property(bind("xxx")))

## 1. TemplateBinding 自动绑定

### 实现原理

TemplateBinding 是一种特殊的 Binding，用于在 ControlTemplate 中绑定模板内元素的属性到 TemplatedParent（应用模板的控件）的属性。

#### 代码更改

**include/fk/binding/Binding.h**
```cpp
class Binding {
public:
    virtual ~Binding() = default;  // 添加虚析构函数
    
    // 添加虚函数用于识别 TemplateBinding
    [[nodiscard]] virtual bool IsTemplateBinding() const noexcept { return false; }
    
    // ... 其他代码
};
```

**include/fk/binding/TemplateBinding.h**
```cpp
class TemplateBinding : public Binding {
public:
    // 覆盖 IsTemplateBinding 方法
    [[nodiscard]] bool IsTemplateBinding() const noexcept override { return true; }
};
```

**src/binding/BindingExpression.cpp**
```cpp
std::any BindingExpression::ResolveSourceRoot() const {
    // 检测是否为 TemplateBinding
    if (definition_.IsTemplateBinding()) {
        // TemplateBinding 应该绑定到 TemplatedParent
        if (target_) {
            if (auto* uiElement = dynamic_cast<ui::UIElement*>(target_)) {
                auto* templatedParent = uiElement->GetTemplatedParent();
                if (templatedParent) {
                    return std::any(templatedParent);
                }
            }
        }
        return std::any{};
    }
    
    // ... 其他 source 解析逻辑
}
```

### 使用示例

```cpp
// 在 Button 的 ControlTemplate 中使用
auto* border = new Border();
border->SetTemplatedParent(button);  // 设置 TemplatedParent

// 使用 TemplateBinding 绑定 Button 的 Background 到 Border 的 Background
border->SetBinding(
    Border::BackgroundProperty(),
    TemplateBinding(Control<Button>::BackgroundProperty())
);

// 现在 Border 的 Background 会自动跟随 Button 的 Background
```

## 2. 控件工厂函数

### 实现原理

为了简化控件创建，提供与控件类同名的工厂函数。由于函数名与类名相同会产生冲突，工厂函数被放置在单独的 `fk::factory` 命名空间中。

#### 代码更改

**include/fk/ui/ControlFactory.h** (新文件)
```cpp
namespace fk::factory {

// Window 工厂函数
inline ui::Window* Window() {
    return new ui::Window();
}

// Button 工厂函数
inline ui::Button* Button() {
    return new ui::Button();
}

// 其他控件的工厂函数...
// TextBlock, Border, StackPanel, Grid, etc.

} // namespace fk::factory
```

### 使用示例

```cpp
#include "fk/ui/ControlFactory.h"

using namespace fk::factory;

// 使用工厂函数创建控件
auto* window = Window();
auto* button = Button();
auto* textBlock = TextBlock();
auto* border = Border();

// 也可以直接使用命名空间前缀
auto* panel = fk::factory::StackPanel();
```

## 3. 链式绑定语法

### 实现原理

为控件的属性方法添加 `Binding` 参数的重载版本，允许通过链式调用来设置绑定。

#### 代码更改

**include/fk/ui/Window.h**
```cpp
class Window : public ContentControl<Window> {
public:
    // 原有的字符串参数版本
    Window* Title(const std::string& value) {
        SetTitle(value);
        return this;
    }
    
    // 新增：Binding 参数版本
    Window* Title(binding::Binding binding) {
        SetBinding(TitleProperty(), std::move(binding));
        return this;
    }
    
    // 类似地为 Left 和 Top 添加 Binding 重载
    Window* Left(binding::Binding binding) {
        SetBinding(LeftProperty(), std::move(binding));
        return this;
    }
    
    Window* Top(binding::Binding binding) {
        SetBinding(TopProperty(), std::move(binding));
        return this;
    }
};
```

### 使用示例

```cpp
#include "fk/binding/Binding.h"

// 创建 ViewModel
auto viewModel = std::make_shared<MyViewModel>();

// 创建 Window 并设置 DataContext
auto* window = new Window();
window->SetDataContext(viewModel);

// 使用链式绑定语法
window->Title(bind("WindowTitle"))     // 绑定到 ViewModel.WindowTitle
      ->Width(800)                      // 直接设置值
      ->Height(600)                     // 直接设置值
      ->Left(bind("WindowX"))           // 绑定到 ViewModel.WindowX
      ->Top(bind("WindowY"));           // 绑定到 ViewModel.WindowY
```

## ViewModel 属性注册

为了让绑定系统能够访问 ViewModel 的属性，需要注册属性访问器。

### 使用宏注册

```cpp
#include "fk/binding/ViewModelMacros.h"
#include "fk/binding/ObservableObject.h"

class MyViewModel : public ObservableObject {
public:
    const std::string& GetWindowTitle() const { return windowTitle_; }
    void SetWindowTitle(const std::string& value) {
        if (windowTitle_ != value) {
            windowTitle_ = value;
            RaisePropertyChanged(std::string("WindowTitle"));
        }
    }
    
private:
    std::string windowTitle_{"My Window"};
};

// 使用宏自动注册属性
FK_VIEWMODEL_AUTO(MyViewModel, WindowTitle)
```

### 手动注册（高级用法）

```cpp
// 使用手动注册方式
FK_VIEWMODEL_BEGIN(MyViewModel)
    FK_VIEWMODEL_PROPERTY_RW("WindowTitle", 
                             &MyViewModel::GetWindowTitle, 
                             &MyViewModel::SetWindowTitle)
FK_VIEWMODEL_END(MyViewModel)
```

## 测试

### template_binding_test.cpp

测试以下功能：
1. TemplateBinding 自动绑定到 TemplatedParent
2. 控件创建
3. 链式绑定语法
4. 组合使用

### binding_debug_test.cpp

调试绑定系统，验证：
1. DataContext 设置
2. Binding 创建和激活
3. 属性更新传播

## 注意事项

1. **TemplateBinding 要求**：使用 TemplateBinding 的元素必须已经设置了 TemplatedParent

2. **工厂函数命名空间**：工厂函数在 `fk::factory` 命名空间中，使用时需要 `using namespace fk::factory;` 或使用完整限定名

3. **链式绑定语法**：需要使用 `bind()` 辅助函数创建 Binding 对象

4. **ViewModel 属性注册**：ViewModel 的属性必须通过宏或手动方式注册到 PropertyAccessorRegistry，否则绑定系统无法访问

5. **类型安全**：Binding 参数重载与原有的值参数重载共存，编译器会根据参数类型自动选择正确的版本

## 扩展其他控件

为其他控件添加链式绑定语法支持的模式：

```cpp
// 在控件类中为需要支持绑定的属性添加重载
class MyControl : public Control<MyControl> {
public:
    // 原有方法
    MyControl* MyProperty(const MyType& value) {
        SetMyProperty(value);
        return this;
    }
    
    // 添加 Binding 重载
    MyControl* MyProperty(binding::Binding binding) {
        SetBinding(MyPropertyProperty(), std::move(binding));
        return this;
    }
};
```

## 总结

本次实现完成了用户提出的三个需求：
1. ✅ TemplateBinding 自动绑定到 TemplatedParent
2. ✅ 提供控件类同名的工厂函数（在 fk::factory 命名空间中）
3. ✅ 支持 `Window->Title(bind("xxx"))` 链式绑定语法

所有功能都已实现并通过编译。使用时需要注意 ViewModel 属性注册和 TemplatedParent 设置等前提条件。
