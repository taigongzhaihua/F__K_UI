# BindingMacros - 绑定宏

## 概述

`BindingMacros.h` 提供了一套宏来简化依赖属性的绑定支持,使属性能够支持链式语法和数据绑定。

**命名空间**: `fk::ui`  
**头文件**: `fk/ui/BindingMacros.h`

## 核心功能

- **链式语法**: 支持 `control->Property(value)->Property2(value2)` 链式调用
- **绑定支持**: 支持 `control->Property(binding)` 数据绑定语法
- **多种场景**: 适配不同的类继承结构
- **类型统一**: 所有类型(值类型、引用类型、枚举)使用相同宏

## 宏列表

### FK_BINDING_PROPERTY
```cpp
FK_BINDING_PROPERTY(PropertyName, ValueType)
```
为依赖属性添加绑定支持(通用版本)。

**适用于**: 直接继承 `enable_shared_from_this` 的类  
**支持类型**: 所有类型(int, float, bool, enum, std::string 等)

**生成的代码**:
```cpp
[[nodiscard]] const ValueType& PropertyName() const {
    return GetPropertyName();
}

Ptr PropertyName(const ValueType& value) {
    SetPropertyName(value);
    return Self();
}

Ptr PropertyName(binding::Binding binding) {
    SetBinding(PropertyNameProperty(), std::move(binding));
    return Self();
}
```

### FK_BINDING_PROPERTY_THIS
```cpp
FK_BINDING_PROPERTY_THIS(PropertyName, ValueType, BaseClass)
```
为依赖属性添加绑定支持(使用 this-> 访问基类)。

**适用于**: View 派生类,通过 `this->` 访问基类方法  
**需要参数**: `BaseClass` - 定义 Property() 方法的基类

### FK_BINDING_PROPERTY_BASE
```cpp
FK_BINDING_PROPERTY_BASE(PropertyName, ValueType, PropertyOwner)
```
为依赖属性添加绑定支持(使用 Base:: 访问基类)。

**适用于**: `View<Derived, Base>` 模板类  
**需要参数**: `PropertyOwner` - 定义 Property() 方法的类

### 向后兼容别名

```cpp
// FK_BINDING_PROPERTY 的别名
FK_BINDING_PROPERTY_VALUE      // 已废弃,使用 FK_BINDING_PROPERTY
FK_BINDING_PROPERTY_ENUM       // 已废弃,使用 FK_BINDING_PROPERTY

// FK_BINDING_PROPERTY_THIS 的别名
FK_BINDING_PROPERTY_VALUE_BASE // 已废弃,使用 FK_BINDING_PROPERTY_THIS

// FK_BINDING_PROPERTY_BASE 的别名
FK_BINDING_PROPERTY_VIEW       // 已废弃,使用 FK_BINDING_PROPERTY_BASE
FK_BINDING_PROPERTY_VIEW_VALUE // 已废弃,使用 FK_BINDING_PROPERTY_BASE
FK_BINDING_PROPERTY_VIEW_ENUM  // 已废弃,使用 FK_BINDING_PROPERTY_BASE
```

## 使用示例

### 1. 基础使用 - FK_BINDING_PROPERTY

```cpp
#include <fk/ui/Control.h>
#include <fk/ui/DependencyPropertyMacros.h>
#include <fk/ui/BindingMacros.h>

// 在类模板中使用
template <typename Derived = void>
class MyControl : public View<std::conditional_t<std::is_void_v<Derived>, MyControl<>, Derived>, detail::MyControlBase> {
public:
    using Base = View<...>;
    using Ptr = typename Base::Ptr;
    
    // 添加绑定支持
    FK_BINDING_PROPERTY(Width, float)
    FK_BINDING_PROPERTY(Height, float)
    FK_BINDING_PROPERTY(Background, std::string)
    FK_BINDING_PROPERTY(IsEnabled, bool)
    FK_BINDING_PROPERTY(Visibility, fk::ui::Visibility)  // enum
    
    static Ptr Create() {
        return std::make_shared<std::conditional_t<...>>();
    }
};

// 使用链式语法
auto control = MyControl<>::Create()
    ->Width(200.0f)
    ->Height(100.0f)
    ->Background("#FFFFFF")
    ->IsEnabled(true)
    ->Visibility(fk::ui::Visibility::Visible);

// 使用数据绑定
auto binding = fk::binding::Binding("Width");
binding.Source = viewModel;
control->Width(binding);
```

### 2. 派生类使用 - FK_BINDING_PROPERTY_THIS

```cpp
class ButtonBase : public ControlBase {
public:
    FK_DEPENDENCY_PROPERTY_DECLARE_REF(Background, std::string)
    FK_DEPENDENCY_PROPERTY_DECLARE(IsPressed, bool)
};

template <typename Derived = void>
class Button : public View<...> {
public:
    // 使用 this-> 访问基类方法
    FK_BINDING_PROPERTY_THIS(Background, std::string, ButtonBase)
    FK_BINDING_PROPERTY_THIS(IsPressed, bool, ButtonBase)
};

// 使用
auto button = fk::ui::button()
    ->Background("#0078D4")
    ->IsPressed(false);
```

### 3. View 模板使用 - FK_BINDING_PROPERTY_BASE

```cpp
template <typename Derived = void>
class TextBlock : public View<std::conditional_t<...>, detail::TextBlockBase> {
public:
    using Base = View<...>;
    
    // 使用 Base:: 访问继承的方法
    FK_BINDING_PROPERTY_BASE(Text, std::string, TextBlockBase)
    FK_BINDING_PROPERTY_BASE(FontSize, float, TextBlockBase)
    FK_BINDING_PROPERTY_BASE(Foreground, std::string, TextBlockBase)
};

// 使用
auto textBlock = fk::ui::textBlock()
    ->Text("Hello World")
    ->FontSize(16.0f)
    ->Foreground("#000000");
```

### 4. 所有类型的统一使用

```cpp
template <typename Derived = void>
class MyControl : public View<...> {
public:
    // 所有类型使用同一个宏
    FK_BINDING_PROPERTY(Width, float)              // 值类型
    FK_BINDING_PROPERTY(IsEnabled, bool)           // 布尔类型
    FK_BINDING_PROPERTY(Text, std::string)         // 引用类型
    FK_BINDING_PROPERTY(Orientation, Orientation)  // 枚举类型
    FK_BINDING_PROPERTY(Items, std::vector<int>)   // 容器类型
};
```

### 5. 数据绑定示例

```cpp
#include <fk/binding/ObservableObject.h>
#include <fk/binding/Binding.h>

// ViewModel
class ViewModel : public fk::binding::ObservableObject {
    FK_PROPERTY(std::string, Username)
    FK_PROPERTY(float, Progress)
    FK_PROPERTY(bool, IsLoading)
};

// View
auto viewModel = std::make_shared<ViewModel>();

auto textBox = fk::ui::textBox()
    ->Text(fk::binding::Binding("Username").Source(viewModel));

auto progressBar = fk::ui::progressBar()
    ->Value(fk::binding::Binding("Progress").Source(viewModel));

auto loadingIndicator = fk::ui::control()
    ->Visibility(fk::binding::Binding("IsLoading")
        .Source(viewModel)
        .Converter([](const std::any& value) -> std::any {
            bool isLoading = std::any_cast<bool>(value);
            return isLoading ? fk::ui::Visibility::Visible 
                            : fk::ui::Visibility::Collapsed;
        }));
```

### 6. 完整控件示例

```cpp
// MyButton.h
class MyButtonBase : public fk::ui::ControlBase {
public:
    FK_DEPENDENCY_PROPERTY_DECLARE_REF(Content, std::string)
    FK_DEPENDENCY_PROPERTY_DECLARE_REF(Background, std::string)
    FK_DEPENDENCY_PROPERTY_DECLARE(CornerRadius, float)
    FK_DEPENDENCY_PROPERTY_DECLARE(IsPressed, bool)
};

template <typename Derived = void>
class MyButton : public View<std::conditional_t<...>, MyButtonBase> {
public:
    using Base = View<...>;
    using Ptr = typename Base::Ptr;
    
    // 添加绑定支持
    FK_BINDING_PROPERTY_BASE(Content, std::string, MyButtonBase)
    FK_BINDING_PROPERTY_BASE(Background, std::string, MyButtonBase)
    FK_BINDING_PROPERTY_BASE(CornerRadius, float, MyButtonBase)
    FK_BINDING_PROPERTY_BASE(IsPressed, bool, MyButtonBase)
    
    static Ptr Create() {
        return std::make_shared<std::conditional_t<...>>();
    }
};

// 使用
auto button = MyButton<>::Create()
    ->Content("Click Me")
    ->Background("#0078D4")
    ->CornerRadius(4.0f)
    ->IsPressed(false);

// 链式调用
button->Background("#005A9E")
      ->CornerRadius(8.0f);

// 数据绑定
auto binding = fk::binding::Binding("ButtonText");
binding.Source = viewModel;
button->Content(binding);
```

### 7. 混合使用

```cpp
template <typename Derived = void>
class ComplexControl : public View<...> {
public:
    // 直接访问的属性
    FK_BINDING_PROPERTY(LocalProperty, float)
    
    // 从基类继承的属性
    FK_BINDING_PROPERTY_BASE(Width, float, FrameworkElement)
    FK_BINDING_PROPERTY_BASE(Height, float, FrameworkElement)
    FK_BINDING_PROPERTY_BASE(Visibility, Visibility, UIElement)
};

auto control = ComplexControl<>::Create()
    ->LocalProperty(42.0f)
    ->Width(200.0f)
    ->Height(100.0f)
    ->Visibility(fk::ui::Visibility::Visible);
```

## 最佳实践

### 1. 选择正确的宏

```cpp
// 场景 1: 直接继承 enable_shared_from_this
// 使用 FK_BINDING_PROPERTY
class MyControl : public std::enable_shared_from_this<MyControl> {
    FK_BINDING_PROPERTY(Width, float)
};

// 场景 2: 使用 this-> 访问基类
// 使用 FK_BINDING_PROPERTY_THIS
FK_BINDING_PROPERTY_THIS(Width, float, FrameworkElement)

// 场景 3: View 模板类使用 Base::
// 使用 FK_BINDING_PROPERTY_BASE
FK_BINDING_PROPERTY_BASE(Width, float, FrameworkElement)
```

### 2. 所有类型统一使用

```cpp
// 推荐: 统一使用 FK_BINDING_PROPERTY
FK_BINDING_PROPERTY(Width, float)
FK_BINDING_PROPERTY(Text, std::string)
FK_BINDING_PROPERTY(IsEnabled, bool)
FK_BINDING_PROPERTY(Orientation, Orientation)

// 避免: 使用废弃的别名
FK_BINDING_PROPERTY_VALUE(Width, float)        // 废弃
FK_BINDING_PROPERTY_ENUM(Orientation, Orientation)  // 废弃
```

### 3. 保持链式语法

```cpp
// 推荐: 利用链式语法
auto control = MyControl<>::Create()
    ->Width(200)
    ->Height(100)
    ->Background("#FFF");

// 避免: 多次分别调用
auto control = MyControl<>::Create();
control->Width(200);
control->Height(100);
control->Background("#FFF");
```

### 4. 数据绑定最佳实践

```cpp
// 推荐: 使用 Binding 对象
auto binding = fk::binding::Binding("PropertyName");
binding.Source = viewModel;
binding.Mode = fk::binding::BindingMode::TwoWay;
control->Property(binding);

// 避免: 手动同步
// 不要手动监听 PropertyChanged 然后调用 SetProperty
```

## 常见问题

### Q1: 三个宏有什么区别?

```cpp
// FK_BINDING_PROPERTY
// - 使用 GetPropertyName() / SetPropertyName()
// - 使用 PropertyNameProperty()
// - 适用于直接访问的情况

// FK_BINDING_PROPERTY_THIS
// - 使用 this->GetPropertyName() / this->SetPropertyName()
// - 使用 BaseClass::PropertyNameProperty()
// - 适用于需要显式 this 的情况

// FK_BINDING_PROPERTY_BASE
// - 使用 Base::GetPropertyName() / Base::SetPropertyName()
// - 使用 PropertyOwner::PropertyNameProperty()
// - 适用于 View 模板类
```

### Q2: 为什么废弃了 VALUE 和 ENUM 后缀?

现在所有类型(值类型、引用类型、枚举)都使用统一的宏,不需要区分。

### Q3: 如何支持双向绑定?

```cpp
auto binding = fk::binding::Binding("Text");
binding.Source = viewModel;
binding.Mode = fk::binding::BindingMode::TwoWay;  // 双向绑定

textBox->Text(binding);

// TextBox 文本改变时自动更新 ViewModel
// ViewModel 属性改变时自动更新 TextBox
```

### Q4: 链式语法的性能如何?

链式语法没有性能开销,它只是返回 `this` 指针,编译器会优化。

### Q5: 可以混用普通赋值和绑定吗?

```cpp
auto control = MyControl<>::Create();

// 普通赋值
control->Width(200.0f);

// 数据绑定
control->Height(fk::binding::Binding("Height").Source(vm));

// 可以混用,但绑定会覆盖之前的值
```

## 性能考虑

### 1. 链式调用无开销

```cpp
// 链式调用
control->Width(200)->Height(100)->Background("#FFF");

// 等价于
control->Width(200);
control->Height(100);
control->Background("#FFF");

// 无额外开销,只是返回 this
```

### 2. 绑定创建开销

```cpp
// 避免: 重复创建绑定
for (int i = 0; i < 1000; ++i) {
    control->Width(fk::binding::Binding("Width").Source(vm));
}

// 推荐: 一次性绑定
control->Width(fk::binding::Binding("Width").Source(vm));
```

### 3. Getter 性能

```cpp
// 高效: const 引用返回
const std::string& text = textBlock->Text();

// 避免复制
std::string text = textBlock->Text();  // 触发复制
```

## 内部机制

### 宏展开示例

```cpp
// 原始宏:
FK_BINDING_PROPERTY(Width, float)

// 展开为:
[[nodiscard]] const float& Width() const {
    return GetWidth();
}

Ptr Width(const float& value) {
    SetWidth(value);
    return Self();
}

Ptr Width(binding::Binding binding) {
    SetBinding(WidthProperty(), std::move(binding));
    return Self();
}
```

## 相关文档

- [DependencyPropertyMacros.md](DependencyPropertyMacros.md) - 依赖属性宏
- [Binding.md](../Binding/Binding.md) - 数据绑定
- [DependencyProperty.md](../Binding/DependencyProperty.md) - 依赖属性系统
- [ObservableObject.md](../Binding/ObservableObject.md) - 可观察对象
