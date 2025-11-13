# Primitives

## 概览

**目的**：基本几何和图形类型

**命名空间**：`fk::ui`

**头文件**：`fk/ui/Primitives.h`

## 类型

### Point
表示2D点。
```cpp
struct Point {
    double X, Y;
};
```

### Size
表示大小。
```cpp
struct Size {
    double Width, Height;
};
```

### Rect
表示矩形。
```cpp
struct Rect {
    double X, Y, Width, Height;
};
```

### Color
表示颜色（RGBA）。
```cpp
struct Color {
    uint8_t R, G, B, A;
};
```

### Matrix3x2
表示3x2变换矩阵。

## 使用示例

### 基本几何
```cpp
Point p(10, 20);
Size s(100, 50);
Rect r(0, 0, 200, 100);
Color c(255, 0, 0, 255);  // 红色
```

### 预定义颜色
```cpp
Colors::Red
Colors::Blue
Colors::Green
Colors::White
Colors::Black
Colors::Transparent
```
