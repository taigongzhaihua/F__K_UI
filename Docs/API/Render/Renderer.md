# Renderer

## 概览

**目的**：主渲染器，管理渲染管线

**命名空间**：`fk::render`

**头文件**：`fk/render/Renderer.h`

## 描述

`Renderer` 是主渲染器，负责将视觉树渲染到屏幕。

## 公共接口

### 渲染

#### Render
```cpp
void Render(Visual* root);
```

渲染视觉树。

#### SetBackend
```cpp
void SetBackend(std::shared_ptr<IRenderer> backend);
```

设置渲染后端。

## 使用示例

### 基本渲染
```cpp
auto renderer = std::make_shared<Renderer>();
auto glBackend = std::make_shared<GlRenderer>();
renderer->SetBackend(glBackend);

// 渲染循环
while (running) {
    renderer->Render(rootVisual);
}
```

## 相关类

- [IRenderer](IRenderer.md)
- [GlRenderer](GlRenderer.md)
- [RenderBackend](RenderBackend.md)
