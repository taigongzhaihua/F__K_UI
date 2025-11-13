# Style

## 概览

**目的**：定义控件的样式和属性设置

**命名空间**：`fk::ui`

**头文件**：`fk/ui/Style.h`

## 描述

`Style` 用于定义一组属性设置，可应用于控件以统一外观。

## 公共接口

### 创建样式

```cpp
Style(const std::type_info& targetType);
```

为特定类型创建样式。

### 属性设置

#### AddSetter
```cpp
void AddSetter(const DependencyProperty& property, const std::any& value);
```

添加属性设置器。

### 应用样式

#### Apply / Unapply
```cpp
void Apply(DependencyObject* target);
void Unapply(DependencyObject* target);
```

应用或撤销样式。

## 使用示例

### 创建按钮样式
```cpp
auto buttonStyle = std::make_shared<Style>(typeid(Button));

buttonStyle->AddSetter(Control::BackgroundProperty(), Colors::Blue);
buttonStyle->AddSetter(Control::ForegroundProperty(), Colors::White);
buttonStyle->AddSetter(Control::FontSizeProperty(), 14.0);
buttonStyle->AddSetter(Control::PaddingProperty(), Thickness(10, 5));

button->SetStyle(buttonStyle.get());
```

### 基于其他样式
```cpp
auto derivedStyle = std::make_shared<Style>(typeid(Button));
derivedStyle->BasedOn(buttonStyle);
derivedStyle->AddSetter(Control::FontWeightProperty(), FontWeight::Bold);
```

### 隐式样式
```cpp
// 在ResourceDictionary中定义
resourceDictionary->Add(typeid(Button), buttonStyle);

// 自动应用到所有Button
```

## 相关类

- [Setter](Setter.md)
- [ResourceDictionary](ResourceDictionary.md)
- [Control](Control.md)
