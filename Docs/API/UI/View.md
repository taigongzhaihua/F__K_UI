# View - 视图模板基类

## 概述

`View<Derived, Base>` 是一个模板基类，为自定义 UI 视图提供链式 API 支持、自动的 `shared_from_this` 功能和便利的数据绑定方法。它简化了创建支持 MVVM 模式的 UI 元素。

## 设计目标

- 提供统一的链式 API 接口
- 支持自动的 `enable_shared_from_this` 功能
- 简化数据绑定相关方法
- 减少样板代码

## 核心 API

### 模板参数

```cpp
template <typename Derived, typename Base = FrameworkElement>
class View : public Base, public std::enable_shared_from_this<Derived>
```

- `Derived`: 派生类型（通常是自定义控件类名）
- `Base`: 基类（默认为 FrameworkElement）

### 静态工厂方法

```cpp
template <typename... Args>
static Ptr Create(Args&&... args);

using Ptr = std::shared_ptr<Derived>;
```

创建并返回派生类的共享指针。

**示例：**
```cpp
class MyButton : public fk::ui::View<MyButton, fk::ui::Button> {
public:
    // ... 自定义实现
};

auto button = MyButton::Create();  // 返回 std::shared_ptr<MyButton>
```

### 链式 API 方法

所有属性都有 getter 和 setter 两个版本：

```cpp
// Getter
const std::any& DataContext() const;

// Setter - 返回 Ptr 以支持链式
Ptr DataContext(std::any value);
Ptr DataContext(T&& value);
Ptr ClearDataContextValue();

// 其他属性
Ptr Width(float value);
Ptr Height(float value);
Ptr HorizontalAlignment(fk::ui::HorizontalAlignment value);
// ... 更多属性
```

**特点：**
- Setter 返回 `Ptr`（指向 Derived 的共享指针）
- 支持模板化的值传递
- 维护类型安全性

### 自动 Self() 方法

```cpp
Ptr Self();
std::shared_ptr<const Derived> Self() const;
```

获取指向当前对象的适当类型的智能指针。

## 常见模式

### 1. 创建自定义控件

```cpp
#include "fk/ui/View.h"
#include "fk/ui/Button.h"

class CustomButton : public fk::ui::View<CustomButton, fk::ui::Button> {
public:
    // 继承所有 Button 的链式方法
};

// 使用
auto btn = CustomButton::Create()
    ->Width(100)
    ->Height(50)
    ->Text("Click me");
```

### 2. 链式 API 构建

```cpp
auto panel = fk::ui::StackPanel::Create()
    ->Orientation(fk::ui::Orientation::Vertical)
    ->Width(300)
    ->Margin(fk::Thickness(10.0f))
    ->DataContext(viewModel);
```

### 3. MVVM 绑定

```cpp
auto textBox = fk::ui::TextBox::Create()
    ->DataContext(viewModel)
    ->HorizontalAlignment(fk::ui::HorizontalAlignment::Stretch);

// TextBox 自动继承 FrameworkElement 的对齐方法
```

## 实现示例

```cpp
// 自定义 Panel 视图
class CustomPanel : public fk::ui::View<CustomPanel, fk::ui::Panel> {
public:
    CustomPanel() {
        SetPadding(fk::Thickness(10.0f));
    }
    
    // 自定义方法也可以返回 Ptr
    Ptr SetTitle(const std::string& title) {
        title_ = title;
        return Self();
    }
    
private:
    std::string title_;
};

// 使用
auto panel = CustomPanel::Create()
    ->SetTitle("My Panel")
    ->Width(300)
    ->Height(200)
    ->DataContext(viewModel);
```

## 最佳实践

### 1. 为自定义控件使用 View

```cpp
// ✅ 推荐
class MyControl : public fk::ui::View<MyControl, fk::ui::Control> {
    // 继承链式 API
};

// ❌ 避免直接继承而失去链式支持
class MyControl : public fk::ui::Control {
    // 需要手动实现链式方法
};
```

### 2. 在构造函数中初始化

```cpp
class StyledButton : public fk::ui::View<StyledButton, fk::ui::Button> {
public:
    StyledButton() {
        // 设置默认值
        SetBackground("blue");
        SetForeground("white");
    }
};
```

### 3. 使用 Self() 在成员函数中

```cpp
class CustomPanel : public fk::ui::View<CustomPanel, fk::ui::Panel> {
private:
    Ptr SetupDefault() {
        SetMargin(fk::Thickness(10.0f));
        return Self();  // 返回正确的类型
    }
};
```

## 相关文档

- [FrameworkElement.md](./FrameworkElement.md) - 基类
- [Control.md](./Control.md) - 控件基类
- [BindingMacros.md](./BindingMacros.md) - 绑定宏
