# Control

## 概览

**目的**：可模板化控件的基类，提供样式、模板和外观支持

**命名空间**：`fk::ui`

**继承**：`FrameworkElement` → `UIElement` → `Visual` → `DependencyObject`

**头文件**：`fk/ui/Control.h`

**模板参数**：CRTP 模式，`Control<Derived>`

## 描述

`Control` 是所有可模板化控件的基类。它添加了背景、前景、字体、模板和样式支持。

## 公共接口

### 外观属性

#### Background / Foreground
```cpp
static const DependencyProperty& BackgroundProperty();
static const DependencyProperty& ForegroundProperty();

Derived* Background(const Color& color);
Derived* Foreground(const Color& color);
```

设置背景色和前景色。

**示例**：
```cpp
control->Background(Colors::Blue)
       ->Foreground(Colors::White);
```

#### BorderBrush / BorderThickness
```cpp
static const DependencyProperty& BorderBrushProperty();
static const DependencyProperty& BorderThicknessProperty();

Derived* BorderBrush(const Color& color);
Derived* BorderThickness(const Thickness& thickness);
```

设置边框颜色和厚度。

### 字体属性

#### FontSize / FontFamily / FontWeight
```cpp
static const DependencyProperty& FontSizeProperty();
static const DependencyProperty& FontFamilyProperty();
static const DependencyProperty& FontWeightProperty();

Derived* FontSize(double size);
Derived* FontFamily(const std::string& family);
Derived* FontWeight(FontWeight weight);
```

设置字体属性。

**示例**：
```cpp
control->FontSize(16)
       ->FontFamily("Arial")
       ->FontWeight(FontWeight::Bold);
```

### 模板和样式

#### Template
```cpp
static const DependencyProperty& TemplateProperty();
void SetTemplate(ControlTemplate* templ);
```

设置控件模板以自定义视觉外观。

**示例**：
```cpp
auto template = std::make_shared<ControlTemplate>();
// ... 配置模板
control->SetTemplate(template.get());
```

#### Style
```cpp
static const DependencyProperty& StyleProperty();
void SetStyle(Style* style);
```

应用样式来设置多个属性。

## 使用示例

### 完整外观配置
```cpp
auto control = std::make_shared<MyControl>();
control->Background(Colors::LightBlue)
       ->Foreground(Colors::Black)
       ->BorderBrush(Colors::Blue)
       ->BorderThickness(Thickness(2))
       ->FontSize(14)
       ->FontFamily("Microsoft YaHei")
       ->Padding(Thickness(10));
```

### 使用样式
```cpp
auto style = std::make_shared<Style>(typeid(Button));
style->AddSetter(Control::BackgroundProperty(), Colors::Blue);
style->AddSetter(Control::ForegroundProperty(), Colors::White);

button->SetStyle(style.get());
```

## 相关类

- [FrameworkElement](FrameworkElement.md) - 基类
- [ContentControl](ContentControl.md) - 派生类
- [ControlTemplate](ControlTemplate.md) - 模板系统
- [Style](Style.md) - 样式系统

## 另请参阅

- [设计文档](../../Design/UI/Control.md)
