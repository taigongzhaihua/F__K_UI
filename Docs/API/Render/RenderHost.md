# RenderHost

## 概览

**目的**：管理渲染表面和窗口集成

**命名空间**：`fk::render`

**头文件**：`fk/render/RenderHost.h`

## 描述

`RenderHost` 管理渲染表面和与窗口系统的集成。

## 公共接口

### 表面管理

#### CreateSurface
```cpp
void CreateSurface(void* windowHandle, int width, int height);
```

### 渲染循环

#### BeginFrame / EndFrame
```cpp
void BeginFrame();
void EndFrame();
```

## 使用示例

```cpp
auto renderHost = std::make_shared<RenderHost>();
renderHost->CreateSurface(windowHandle, 800, 600);

while (running) {
    renderHost->BeginFrame();
    // 渲染...
    renderHost->EndFrame();
}
```

## 相关类

- [Renderer](Renderer.md)
