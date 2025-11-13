# TextBlock

## 概览

**目的**：显示只读文本的轻量级元素

**命名空间**：`fk::ui`

**继承**：`FrameworkElement` → `UIElement` → `Visual`

**头文件**：`fk/ui/TextBlock.h`

## 描述

`TextBlock` 用于显示不可编辑的文本。它支持字体样式、颜色、对齐方式和文本换行。

## 公共接口

### 文本属性

#### Text
```cpp
static const DependencyProperty& TextProperty();
TextBlock* Text(const std::string& text);
```

设置要显示的文本。

**示例**：
```cpp
textBlock->Text("Hello, World!");
```

### 字体属性

#### FontSize / FontFamily / FontWeight
```cpp
TextBlock* FontSize(double size);
TextBlock* FontFamily(const std::string& family);
TextBlock* FontWeight(FontWeight weight);
```

设置字体属性。

**示例**：
```cpp
textBlock->FontSize(18)
         ->FontFamily("Microsoft YaHei")
         ->FontWeight(FontWeight::Bold);
```

### 颜色

#### Foreground
```cpp
static const DependencyProperty& ForegroundProperty();
TextBlock* Foreground(const Color& color);
```

设置文本颜色。

**示例**：
```cpp
textBlock->Foreground(Colors::Red);
```

### 对齐

#### TextAlignment
```cpp
static const DependencyProperty& TextAlignmentProperty();
TextBlock* TextAlignment(TextAlignment alignment);
```

设置文本对齐方式（Left、Center、Right、Justify）。

**示例**：
```cpp
textBlock->TextAlignment(TextAlignment::Center);
```

### 换行

#### TextWrapping
```cpp
static const DependencyProperty& TextWrappingProperty();
TextBlock* TextWrapping(TextWrapping wrapping);
```

控制文本换行行为。

**示例**：
```cpp
textBlock->TextWrapping(TextWrapping::Wrap);
```

## 使用示例

### 基本文本
```cpp
auto textBlock = std::make_shared<TextBlock>();
textBlock->Text("欢迎使用 F__K_UI")
         ->FontSize(24)
         ->Foreground(Colors::Blue)
         ->TextAlignment(TextAlignment::Center);
```

### 多行文本
```cpp
textBlock->Text("这是一段较长的文本...\n可以包含多行")
         ->TextWrapping(TextWrapping::Wrap)
         ->Width(300);
```

### 数据绑定
```cpp
textBlock->SetValue(TextBlock::TextProperty(), 
                   Binding("DisplayText").Source(viewModel));
```

## 相关类

- [FrameworkElement](FrameworkElement.md) - 基类
- [TextBox](TextBox.md) - 可编辑文本

## 另请参阅

- [设计文档](../../Design/UI/TextBlock.md)
