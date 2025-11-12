# GlRenderer

## 概览

**目的**：OpenGL渲染器实现

**命名空间**：`fk::render`

**继承**：`IRenderer`

**头文件**：`fk/render/GlRenderer.h`

## 描述

`GlRenderer` 是使用OpenGL的渲染器实现。

## 公共接口

### 初始化

#### Initialize
```cpp
void Initialize();
```

初始化OpenGL上下文。

### 渲染命令

#### DrawRectangle / FillRectangle
```cpp
void DrawRectangle(const Rect& rect, const Color& color, double thickness);
void FillRectangle(const Rect& rect, const Color& color);
```

绘制矩形。

#### DrawText
```cpp
void DrawText(const std::string& text, const Point& position, const TextStyle& style);
```

绘制文本。

## 使用示例

### 创建渲染器
```cpp
auto glRenderer = std::make_shared<GlRenderer>();
glRenderer->Initialize();
```

## 相关类

- [IRenderer](IRenderer.md)
- [Renderer](Renderer.md)
