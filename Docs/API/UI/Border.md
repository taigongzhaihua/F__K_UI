# Border

## 概览

**目的**：为单个子元素提供边框和背景的装饰器

**命名空间**：`fk::ui`

**继承**：`FrameworkElement` → `UIElement` → `Visual`

**头文件**：`fk/ui/Border.h`

## 描述

`Border` 是一个装饰器元素，可以在其单个子元素周围绘制边框和背景。支持圆角和不同的边框厚度。

## 公共接口

### 外观属性

#### Background / BorderBrush
```cpp
static const DependencyProperty& BackgroundProperty();
static const DependencyProperty& BorderBrushProperty();

Border* Background(const Color& color);
Border* BorderBrush(const Color& color);
```

设置背景色和边框颜色。

**示例**：
```cpp
border->Background(Colors::LightGray)
      ->BorderBrush(Colors::Black);
```

#### BorderThickness
```cpp
static const DependencyProperty& BorderThicknessProperty();
Border* BorderThickness(const Thickness& thickness);
```

设置边框厚度（可以为每边设置不同值）。

**示例**：
```cpp
border->BorderThickness(Thickness(2));  // 所有边2像素
border->BorderThickness(Thickness(1, 2, 1, 2));  // 左右1像素，上下2像素
```

#### CornerRadius
```cpp
static const DependencyProperty& CornerRadiusProperty();
Border* CornerRadius(const CornerRadius& radius);
```

设置圆角半径。

**示例**：
```cpp
border->CornerRadius(CornerRadius(5));  // 所有角5像素圆角
border->CornerRadius(CornerRadius(10, 10, 0, 0));  // 仅顶部圆角
```

#### Padding
```cpp
static const DependencyProperty& PaddingProperty();
Border* Padding(const Thickness& padding);
```

设置内容与边框之间的内边距。

**示例**：
```cpp
border->Padding(Thickness(10));
```

### 子元素

#### SetChild
```cpp
template<typename T>
std::shared_ptr<T> SetChild();

void SetChild(std::shared_ptr<UIElement> child);
std::shared_ptr<UIElement> GetChild() const;
```

设置和获取子元素（只能有一个）。

**示例**：
```cpp
auto textBlock = border->SetChild<TextBlock>();
textBlock->Text("带边框的内容");
```

## 使用示例

### 基本边框
```cpp
auto border = std::make_shared<Border>();
border->Background(Colors::White)
      ->BorderBrush(Colors::Blue)
      ->BorderThickness(Thickness(2))
      ->CornerRadius(CornerRadius(5))
      ->Padding(Thickness(10));

auto content = border->SetChild<TextBlock>();
content->Text("带边框和圆角的文本");
```

### 作为容器装饰器
```cpp
auto border = std::make_shared<Border>();
border->BorderBrush(Colors::Gray)
      ->BorderThickness(Thickness(1))
      ->Padding(Thickness(5));

auto stack = border->SetChild<StackPanel>();
stack->AddChild<Button>()->Content("按钮1");
stack->AddChild<Button>()->Content("按钮2");
```

## 相关类

- [FrameworkElement](FrameworkElement.md) - 基类
- [Panel](Panel.md) - 容器类

## 另请参阅

- [设计文档](../../Design/UI/Border.md)
