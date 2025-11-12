# RenderContext

## 概览

**目的**：渲染上下文，提供绘图API

**命名空间**：`fk::render`

**头文件**：`fk/render/RenderContext.h`

## 描述

`RenderContext` 封装当前渲染状态和绘图命令。

## 公共接口

### 绘图

#### DrawRectangle / FillRectangle
```cpp
void DrawRectangle(const Rect& rect, const Color& color, double thickness);
void FillRectangle(const Rect& rect, const Color& color);
```

#### DrawEllipse / FillEllipse
```cpp
void DrawEllipse(const Point& center, double radiusX, double radiusY, const Color& color);
void FillEllipse(const Point& center, double radiusX, double radiusY, const Color& color);
```

#### DrawText
```cpp
void DrawText(const std::string& text, const Rect& bounds, const TextStyle& style);
```

### 状态管理

#### PushTransform / PopTransform
```cpp
void PushTransform(const Matrix3x2& transform);
void PopTransform();
```

#### PushOpacity / PopOpacity
```cpp
void PushOpacity(double opacity);
void PopOpacity();
```

#### PushClip / PopClip
```cpp
void PushClip(const Rect& clipRect);
void PopClip();
```

## 使用示例

### 自定义渲染
```cpp
void MyElement::OnRender(const RenderContext& context) {
    // 绘制背景
    context.FillRectangle(GetBounds(), Colors::LightGray);
    
    // 绘制边框
    context.DrawRectangle(GetBounds(), Colors::Black, 2);
    
    // 绘制文本
    TextStyle style;
    style.FontSize = 14;
    style.Color = Colors::Black;
    context.DrawText("Hello", GetBounds(), style);
}
```

### 使用变换
```cpp
context.PushTransform(Matrix3x2::Rotation(45));
context.FillRectangle(rect, Colors::Blue);
context.PopTransform();
```
