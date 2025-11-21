# Shape 图形控件使用指南

## 概述

Shape 是所有 2D 图形的基类，提供填充（Fill）和描边（Stroke）功能。F__K_UI 提供了以下 Shape 派生类：

- **Rectangle** - 矩形（支持圆角）
- **Ellipse** - 椭圆/圆形
- **Line** - 直线
- **Polygon** - 多边形
- **Path** - 复杂路径（支持贝塞尔曲线、弧线等）

## 通用属性

所有 Shape 都支持以下属性：

```cpp
// 填充画刷
shape->Fill(new SolidColorBrush(Color::FromRGB(255, 100, 100, 255)));

// 描边画刷
shape->Stroke(new SolidColorBrush(Color::FromRGB(200, 0, 0, 255)));

// 描边厚度
shape->StrokeThickness(2.0f);
```

## 1. Rectangle - 矩形

### 基本用法

```cpp
// 普通矩形
auto* rect = new Rectangle();
rect->Width(100.0f)->Height(60.0f)
    ->Fill(new SolidColorBrush(Color::FromRGB(255, 100, 100, 255)))
    ->Stroke(new SolidColorBrush(Color::FromRGB(200, 0, 0, 255)))
    ->StrokeThickness(2.0f);
```

### 圆角矩形

```cpp
// 圆角矩形
auto* roundedRect = new Rectangle();
roundedRect->Width(100.0f)->Height(60.0f)
           ->RadiusX(15.0f)->RadiusY(15.0f)  // 圆角半径
           ->Fill(new SolidColorBrush(Color::FromRGB(100, 255, 100, 255)))
           ->Stroke(new SolidColorBrush(Color::FromRGB(0, 200, 0, 255)))
           ->StrokeThickness(2.0f);
```

### 完全圆角（胶囊形）

```cpp
// 设置圆角半径为宽度/高度的一半
auto* capsule = new Rectangle();
capsule->Width(120.0f)->Height(60.0f)
       ->RadiusX(30.0f)->RadiusY(30.0f)  // 高度的一半
       ->Fill(new SolidColorBrush(Color::FromRGB(100, 100, 255, 255)));
```

## 2. Ellipse - 椭圆/圆形

### 圆形

```cpp
// 宽高相等即为圆形
auto* circle = new Ellipse();
circle->Width(60.0f)->Height(60.0f)
      ->Fill(new SolidColorBrush(Color::FromRGB(255, 200, 0, 255)))
      ->Stroke(new SolidColorBrush(Color::FromRGB(200, 150, 0, 255)))
      ->StrokeThickness(2.0f);
```

### 椭圆

```cpp
// 宽高不等为椭圆
auto* ellipse = new Ellipse();
ellipse->Width(120.0f)->Height(60.0f)
       ->Fill(new SolidColorBrush(Color::FromRGB(255, 150, 200, 255)))
       ->Stroke(new SolidColorBrush(Color::FromRGB(200, 100, 150, 255)))
       ->StrokeThickness(2.0f);
```

### 仅描边（空心圆）

```cpp
auto* hollow = new Ellipse();
hollow->Width(60.0f)->Height(60.0f)
      ->Stroke(new SolidColorBrush(Color::FromRGB(0, 200, 200, 255)))
      ->StrokeThickness(3.0f);
// 不设置 Fill，默认为透明
```

## 3. Line - 直线

### 基本用法

```cpp
// 从 (x1, y1) 到 (x2, y2)
auto* line = new Line();
line->X1(0.0f)->Y1(0.0f)
    ->X2(100.0f)->Y2(50.0f)
    ->Stroke(new SolidColorBrush(Color::FromRGB(255, 0, 0, 255)))
    ->StrokeThickness(2.0f);
```

### 常用线条

```cpp
// 水平线
auto* hLine = new Line();
hLine->X1(0.0f)->Y1(0.0f)->X2(100.0f)->Y2(0.0f)
     ->Stroke(new SolidColorBrush(Color::FromRGB(255, 0, 0, 255)))
     ->StrokeThickness(2.0f);

// 垂直线
auto* vLine = new Line();
vLine->X1(0.0f)->Y1(0.0f)->X2(0.0f)->Y2(60.0f)
     ->Stroke(new SolidColorBrush(Color::FromRGB(0, 255, 0, 255)))
     ->StrokeThickness(2.0f);

// 对角线
auto* dLine = new Line();
dLine->X1(0.0f)->Y1(0.0f)->X2(60.0f)->Y2(60.0f)
     ->Stroke(new SolidColorBrush(Color::FromRGB(0, 0, 255, 255)))
     ->StrokeThickness(3.0f);
```

## 4. Polygon - 多边形

### 三角形

```cpp
auto* triangle = new Polygon();
triangle->AddPoint(Point(30, 0))    // 顶点
        ->AddPoint(Point(60, 60))   // 右下
        ->AddPoint(Point(0, 60))    // 左下
        ->Fill(new SolidColorBrush(Color::FromRGB(255, 100, 255, 255)))
        ->Stroke(new SolidColorBrush(Color::FromRGB(200, 0, 200, 255)))
        ->StrokeThickness(2.0f);
```

### 菱形

```cpp
auto* diamond = new Polygon();
diamond->AddPoint(Point(30, 0))     // 上
       ->AddPoint(Point(60, 30))    // 右
       ->AddPoint(Point(30, 60))    // 下
       ->AddPoint(Point(0, 30))     // 左
       ->Fill(new SolidColorBrush(Color::FromRGB(100, 255, 255, 255)))
       ->Stroke(new SolidColorBrush(Color::FromRGB(0, 200, 200, 255)))
       ->StrokeThickness(2.0f);
```

### 五角星

```cpp
auto* star = new Polygon();
star->AddPoint(Point(30, 0))    // 顶点
    ->AddPoint(Point(37, 22))
    ->AddPoint(Point(60, 22))
    ->AddPoint(Point(42, 36))
    ->AddPoint(Point(49, 60))
    ->AddPoint(Point(30, 45))
    ->AddPoint(Point(11, 60))
    ->AddPoint(Point(18, 36))
    ->AddPoint(Point(0, 22))
    ->AddPoint(Point(23, 22))
    ->Fill(new SolidColorBrush(Color::FromRGB(255, 215, 0, 255)))
    ->Stroke(new SolidColorBrush(Color::FromRGB(200, 165, 0, 255)))
    ->StrokeThickness(2.0f);
```

### 批量设置点

```cpp
auto* polygon = new Polygon();
std::vector<Point> points = {
    Point(10, 10),
    Point(50, 10),
    Point(50, 50),
    Point(10, 50)
};
polygon->SetPoints(points);
```

## 5. Path - 复杂路径

### 基本绘制命令

```cpp
auto* path = new Path();
path->MoveTo(0, 30)              // 移动到起点
    ->LineTo(30, 0)              // 直线到
    ->LineTo(60, 30)             // 直线到
    ->Close()                    // 闭合路径
    ->Fill(new SolidColorBrush(Color::FromRGB(150, 200, 255, 255)))
    ->Stroke(new SolidColorBrush(Color::FromRGB(0, 100, 200, 255)))
    ->StrokeThickness(2.0f);
```

### 二次贝塞尔曲线

```cpp
auto* curve = new Path();
curve->MoveTo(0, 30)
     ->QuadraticTo(Point(30, 0), Point(60, 30))  // 控制点，终点
     ->Stroke(new SolidColorBrush(Color::FromRGB(255, 0, 0, 255)))
     ->StrokeThickness(2.0f);
```

### 三次贝塞尔曲线

```cpp
auto* bezier = new Path();
bezier->MoveTo(0, 30)
      ->CubicTo(Point(20, 0), Point(40, 60), Point(60, 30))  // 控制点1, 控制点2, 终点
      ->Stroke(new SolidColorBrush(Color::FromRGB(0, 0, 255, 255)))
      ->StrokeThickness(2.0f);
```

### 心形路径

```cpp
auto* heart = new Path();
heart->MoveTo(30, 15)
     ->CubicTo(30, 10, 25, 5, 20, 5)
     ->CubicTo(10, 5, 5, 15, 5, 25)
     ->CubicTo(5, 35, 15, 45, 30, 55)
     ->CubicTo(45, 45, 55, 35, 55, 25)
     ->CubicTo(55, 15, 50, 5, 40, 5)
     ->CubicTo(35, 5, 30, 10, 30, 15)
     ->Close()
     ->Fill(new SolidColorBrush(Color::FromRGB(255, 100, 120, 255)))
     ->Stroke(new SolidColorBrush(Color::FromRGB(200, 50, 70, 255)))
     ->StrokeThickness(2.0f);
```

### 圆弧

```cpp
auto* arc = new Path();
arc->MoveTo(10, 30)
   ->ArcTo(Point(50, 30), 20, 20, 0, false, true)  // 终点, 半径X, 半径Y, 旋转角, 大弧标志, 顺时针
   ->Stroke(new SolidColorBrush(Color::FromRGB(0, 200, 0, 255)))
   ->StrokeThickness(2.0f);
```

## 高级用法

### 渐变填充

```cpp
auto* rect = new Rectangle();
rect->Width(100.0f)->Height(60.0f);

// 线性渐变
auto* gradient = new LinearGradientBrush();
gradient->SetStartPoint(Point(0, 0));
gradient->SetEndPoint(Point(1, 1));
gradient->AddGradientStop(GradientStop(Color::FromRGB(255, 0, 0, 255), 0.0f));
gradient->AddGradientStop(GradientStop(Color::FromRGB(0, 0, 255, 255), 1.0f));

rect->Fill(gradient);
```

### 透明度

```cpp
// 半透明填充
auto* rect = new Rectangle();
rect->Width(100.0f)->Height(60.0f)
    ->Fill(new SolidColorBrush(Color::FromRGB(255, 100, 100, 128)))  // Alpha = 128
    ->Opacity(0.5f);  // 整体透明度
```

### 无填充/无描边

```cpp
// 仅填充，无描边
auto* shape = new Ellipse();
shape->Fill(new SolidColorBrush(Color::FromRGB(255, 100, 100, 255)));
// 不设置 Stroke

// 仅描边，无填充
auto* shape2 = new Ellipse();
shape2->Stroke(new SolidColorBrush(Color::FromRGB(255, 0, 0, 255)))
      ->StrokeThickness(2.0f);
// 不设置 Fill
```

## 运行演示

编译并运行 Shape 演示程序：

```bash
cd build
cmake --build . --target shape_demo
./shape_demo
```

## 注意事项

1. **坐标系统** - 左上角为原点 (0, 0)，X 向右增长，Y 向下增长
2. **内存管理** - Shape 对象由容器（如 StackPanel）自动管理，无需手动释放
3. **性能** - Path 和 Polygon 的复杂度影响渲染性能，避免过多的点和曲线
4. **布局** - Shape 遵循 WPF 的布局系统，可以使用 Width、Height、Margin 等属性
5. **动画** - 所有 Shape 属性都可以使用动画系统进行动画化

## 相关文档

- [Brush 画刷系统](./BRUSH_GUIDE.md)
- [动画系统使用指南](./ANIMATION_GUIDE.md)
- [布局系统](./LAYOUT_GUIDE.md)
