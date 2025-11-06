# 为依赖属性添加绑定支持指南

## 概述

F__K_UI 框架现在提供了简化的宏来为所有依赖属性快速添加绑定支持。

## 自动 DataContext 继承

✅ **DataContext 已自动继承！**

```cpp
// FrameworkElement.cpp
binding::PropertyMetadata FrameworkElement::BuildDataContextMetadata() {
    metadata.bindingOptions.inheritsDataContext = true;  // ← 已启用
    return metadata;
}
```

**这意味着：**
- 只需在父元素（如 Window）设置 DataContext
- 所有子元素自动继承
- 无需手动传播

```cpp
// ✅ 只需设置一次
window->SetDataContext(baseViewModel);

// ✅ 子元素自动继承
panel->Text(bind("Name"));    // 自动找到 DataContext
textBlock->Text(bind("City")); // 自动找到 DataContext
```

---

## 使用宏添加绑定支持

### 步骤 1：包含头文件

```cpp
#include "fk/ui/BindingMacros.h"
```

### 步骤 2：选择合适的宏

#### `FK_BINDING_PROPERTY` - 引用类型属性

适用于：`std::string`, 自定义类等

```cpp
FK_BINDING_PROPERTY(Text, std::string)
FK_BINDING_PROPERTY(FontFamily, std::string)
```

**展开为：**
```cpp
// Getter (返回引用)
[[nodiscard]] const std::string& Text() const {
    return GetText();
}

// Setter (普通值)
Ptr Text(const std::string& value) {
    SetText(value);
    return Self();
}

// Setter (绑定)
Ptr Text(binding::Binding binding) {
    SetBinding(TextProperty(), std::move(binding));
    return Self();
}
```

#### `FK_BINDING_PROPERTY_VALUE` - 值类型属性

适用于：`float`, `int`, `bool` 等

```cpp
FK_BINDING_PROPERTY_VALUE(FontSize, float)
FK_BINDING_PROPERTY_VALUE(Width, float)
FK_BINDING_PROPERTY_VALUE(IsEnabled, bool)
```

**展开为：**
```cpp
// Getter (返回值，不是引用)
[[nodiscard]] float FontSize() const {
    return GetFontSize();
}

// Setter (普通值)
Ptr FontSize(float value) {
    SetFontSize(value);
    return Self();
}

// Setter (绑定)
Ptr FontSize(binding::Binding binding) {
    SetBinding(FontSizeProperty(), std::move(binding));
    return Self();
}
```

#### `FK_BINDING_PROPERTY_ENUM` - 枚举类型属性

适用于：枚举类型

```cpp
FK_BINDING_PROPERTY_ENUM(TextWrapping, ui::TextWrapping)
FK_BINDING_PROPERTY_ENUM(Orientation, ui::Orientation)
FK_BINDING_PROPERTY_ENUM(HorizontalAlignment, ui::HorizontalAlignment)
```

**展开为：**
```cpp
// Getter
[[nodiscard]] ui::Orientation Orientation() const {
    return GetOrientation();
}

// Setter (普通值)
Ptr Orientation(ui::Orientation value) {
    SetOrientation(value);
    return Self();
}

// Setter (绑定)
Ptr Orientation(binding::Binding binding) {
    SetBinding(OrientationProperty(), std::move(binding));
    return Self();
}
```

---

## 完整示例

### TextBlock（已实现）

```cpp
// include/fk/ui/TextBlock.h
#include "fk/ui/BindingMacros.h"

template <typename Derived>
class TextBlock : public detail::TextBlockBase, public std::enable_shared_from_this<Derived> {
public:
    // ... 其他代码 ...
    
    // 🎯 使用宏快速添加绑定支持
    FK_BINDING_PROPERTY(Text, std::string)
    FK_BINDING_PROPERTY(Foreground, std::string)
    FK_BINDING_PROPERTY_VALUE(FontSize, float)
    FK_BINDING_PROPERTY(FontFamily, std::string)
    FK_BINDING_PROPERTY_ENUM(TextWrapping, ui::TextWrapping)
    FK_BINDING_PROPERTY_ENUM(TextTrimming, ui::TextTrimming)
    
protected:
    Ptr Self() {
        auto* derivedThis = static_cast<Derived*>(this);
        return std::static_pointer_cast<Derived>(derivedThis->shared_from_this());
    }
};
```

**使用效果：**
```cpp
auto text = ui::textBlock()
    ->Text(bind("Name"))              // ✅ 绑定文本
    ->FontSize(bind("TitleSize"))     // ✅ 绑定字体大小
    ->Foreground(bind("ThemeColor"))  // ✅ 绑定颜色
    ->FontFamily("Arial");            // ✅ 混用普通值
```

### StackPanel（已实现）

```cpp
// include/fk/ui/StackPanel.h
#include "fk/ui/BindingMacros.h"

class StackPanel : public View<StackPanel, Panel<StackPanel>> {
public:
    // Getter
    [[nodiscard]] ui::Orientation Orientation() const;
    
    // Setter (普通值)
    std::shared_ptr<StackPanel> Orientation(ui::Orientation orientation);
    
    // 🎯 Setter (绑定) - 手动添加
    std::shared_ptr<StackPanel> Orientation(binding::Binding binding) {
        SetBinding(OrientationProperty(), std::move(binding));
        return this->Self();
    }
    
    // Getter
    [[nodiscard]] float Spacing() const;
    
    // Setter (普通值)
    std::shared_ptr<StackPanel> Spacing(float spacing);
    
    // 🎯 Setter (绑定) - 手动添加
    std::shared_ptr<StackPanel> Spacing(binding::Binding binding) {
        SetBinding(SpacingProperty(), std::move(binding));
        return this->Self();
    }
};
```

**使用效果：**
```cpp
auto panel = ui::stackPanel()
    ->Orientation(bind("LayoutDirection")) // ✅ 绑定方向
    ->Spacing(bind("ItemSpacing"));        // ✅ 绑定间距
```

---

## 为新控件添加绑定支持

### 选项 1：使用宏（推荐）

**适用于：** 使用 CRTP 模板模式的控件

```cpp
template <typename Derived>
class MyControl : public SomeBase, public std::enable_shared_from_this<Derived> {
public:
    using Ptr = std::shared_ptr<Derived>;
    
    // 🎯 一行代码添加完整支持
    FK_BINDING_PROPERTY(Title, std::string)
    FK_BINDING_PROPERTY_VALUE(Opacity, float)
    FK_BINDING_PROPERTY_ENUM(Visibility, ui::Visibility)
    
protected:
    Ptr Self() {
        auto* derivedThis = static_cast<Derived*>(this);
        return std::static_pointer_cast<Derived>(derivedThis->shared_from_this());
    }
};
```

### 选项 2：手动实现

**适用于：** 不使用模板模式的控件

```cpp
class Button : public Control {
public:
    // Getter
    [[nodiscard]] const std::string& Content() const { return GetContent(); }
    
    // Setter (普通值)
    std::shared_ptr<Button> Content(const std::string& value) {
        SetContent(value);
        return shared_from_this();
    }
    
    // 🎯 Setter (绑定) - 手动添加
    std::shared_ptr<Button> Content(binding::Binding binding) {
        SetBinding(ContentProperty(), std::move(binding));
        return shared_from_this();
    }
};
```

---

## 需要实现的前提条件

### 1. 依赖属性已注册

```cpp
static const binding::DependencyProperty& MyProperty() {
    static const auto& property = binding::DependencyProperty::Register(
        "MyProperty",
        typeid(MyControl),
        BuildMyPropertyMetadata()
    );
    return property;
}
```

### 2. Getter/Setter 已实现

```cpp
void SetMyProperty(const ValueType& value) {
    SetValue(MyProperty(), value);
}

ValueType GetMyProperty() const {
    return std::any_cast<ValueType>(GetValue(MyProperty()));
}
```

### 3. 控件支持 shared_from_this

```cpp
class MyControl : public std::enable_shared_from_this<MyControl> {
    // ...
};
```

---

## 已支持绑定的控件

### ✅ TextBlock

- `Text(bind("..."))`
- `Foreground(bind("..."))`
- `FontSize(bind("..."))`
- `FontFamily(bind("..."))`
- `TextWrapping(bind("..."))`
- `TextTrimming(bind("..."))`

### ✅ StackPanel

- `Orientation(bind("..."))`
- `Spacing(bind("..."))`

### ✅ FrameworkElement (所有控件基类)

DataContext 自动继承，无需手动设置

---

## 下一步扩展

### 计划支持的控件

#### Button
```cpp
button->Content(bind("ButtonText"))
      ->Background(bind("ThemeColor"))
      ->IsEnabled(bind("CanExecute"));
```

#### TextBox
```cpp
textBox->Text(bind("InputValue"), BindingMode::TwoWay)
       ->IsReadOnly(bind("IsLocked"));
```

#### CheckBox
```cpp
checkBox->IsChecked(bind("IsSelected"))
        ->Content(bind("Label"));
```

#### ScrollViewer
```cpp
scrollViewer->HorizontalOffset(bind("ScrollX"))
            ->VerticalOffset(bind("ScrollY"));
```

---

## 注意事项

### ⚠️ 必须转换为基类指针

```cpp
// ❌ 错误
window->SetDataContext(viewModel);

// ✅ 正确
std::shared_ptr<binding::INotifyPropertyChanged> baseViewModel = viewModel;
window->SetDataContext(baseViewModel);
```

### ⚠️ 属性必须注册

```cpp
// 必须在 ViewModel 类定义后添加
FK_VIEWMODEL_AUTO(MyViewModel, Property1, Property2, ...)
```

### ⚠️ 绑定顺序

```cpp
// ✅ 正确：先添加到树，再绑定
panel->AddChild(textBlock);
window->SetContent(panel);
window->SetDataContext(baseViewModel);  // DataContext 自动继承
textBlock->Text(bind("Name"));          // 此时有 DataContext

// ❌ 错误：绑定时还没有 DataContext
textBlock->Text(bind("Name"));          // ❌ 无 DataContext
window->SetDataContext(baseViewModel);
```

---

## 总结

### 已实现功能

- ✅ **DataContext 自动继承** - 无需手动传播
- ✅ **绑定支持宏** - 快速添加 `Property(bind(...))` 支持
- ✅ **TextBlock 完整支持** - 所有属性都支持绑定
- ✅ **StackPanel 绑定支持** - Orientation 和 Spacing
- ✅ **类型安全** - 编译时检查
- ✅ **零开销** - 内联函数

### 优势

1. **一致性** - 所有控件使用相同的绑定语法
2. **简洁性** - 一行代码完成绑定
3. **可扩展性** - 宏让添加新支持变得简单
4. **向后兼容** - 不影响现有代码

现在，为依赖属性添加绑定支持只需一行宏！🎉
