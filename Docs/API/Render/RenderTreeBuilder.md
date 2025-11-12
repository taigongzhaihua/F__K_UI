# RenderTreeBuilder

## 概览

**目的**：从视觉树构建渲染树

**命名空间**：`fk::render`

**头文件**：`fk/render/RenderTreeBuilder.h`

## 描述

`RenderTreeBuilder` 遍历视觉树并生成渲染命令。

## 公共接口

### 构建

#### Build
```cpp
void Build(Visual* root, RenderCommandBuffer& commandBuffer);
```

从视觉树构建渲染命令。

## 使用示例

### 构建渲染树
```cpp
auto builder = std::make_shared<RenderTreeBuilder>();
RenderCommandBuffer commandBuffer;

builder->Build(rootVisual, commandBuffer);

// 执行渲染命令
renderer->Execute(commandBuffer);
```

## 相关类

- [Renderer](Renderer.md)
- [RenderCommandBuffer](RenderCommandBuffer.md)
