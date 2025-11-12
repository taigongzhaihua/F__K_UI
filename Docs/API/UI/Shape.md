# Shape

## 概览

**目的**：矢量图形形状的基类

**命名空间**：`fk::ui`

**头文件**：`fk/ui/Shape.h`

## 描述

`Shape` 是所有矢量图形形状的基类。

## 公共接口

### 外观属性

#### Fill / Stroke
```cpp
static const DependencyProperty& FillProperty();
static const DependencyProperty& StrokeProperty();

Shape* Fill(const Color& color);
Shape* Stroke(const Color& color);
```

设置填充色和描边色。

#### StrokeThickness
```cpp
static const DependencyProperty& StrokeThicknessProperty();
Shape* StrokeThickness(double thickness);
```

设置描边粗细。

## 派生类

- [Rectangle](#) - 矩形
- [Ellipse](#) - 椭圆
- [Polygon](#) - 多边形
- [Path](#) - 路径

## 使用示例

### 矩形
```cpp
auto rect = std::make_shared<Rectangle>();
rect->Width(100)
    ->Height(50)
    ->Fill(Colors::Red)
    ->Stroke(Colors::Black)
    ->StrokeThickness(2);
```

### 椭圆
```cpp
auto ellipse = std::make_shared<Ellipse>();
ellipse->Width(80)
       ->Height(80)
       ->Fill(Colors::Blue);
```

## 相关类

- [FrameworkElement](FrameworkElement.md)
