# IRenderer

## 概览

**目的**：渲染器接口

**命名空间**：`fk::render`

**头文件**：`fk/render/IRenderer.h`

## 描述

`IRenderer` 定义渲染器的接口。

## 公共接口

### 绘图命令

```cpp
virtual void DrawRectangle(const Rect& rect, const Color& color, double thickness) = 0;
virtual void FillRectangle(const Rect& rect, const Color& color) = 0;
virtual void DrawEllipse(const Point& center, double rx, double ry, const Color& color) = 0;
virtual void FillEllipse(const Point& center, double rx, double ry, const Color& color) = 0;
virtual void DrawText(const std::string& text, const Rect& bounds, const TextStyle& style) = 0;
```

### 状态管理

```cpp
virtual void PushTransform(const Matrix3x2& transform) = 0;
virtual void PopTransform() = 0;
virtual void PushClip(const Rect& clipRect) = 0;
virtual void PopClip() = 0;
```

## 实现

- [GlRenderer](GlRenderer.md) - OpenGL实现

## 相关类

- [Renderer](Renderer.md)
