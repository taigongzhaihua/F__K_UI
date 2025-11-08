# RenderTreeBuilder - 渲染树构建器

## 概述

`RenderTreeBuilder` 负责遍历 UI 的可视树，提取视觉信息，并生成相应的渲染命令。它是连接 UI 层和渲染层的关键组件。

## 主要责任

- 遍历可视树
- 提取视觉属性（位置、尺寸、颜色、不透明度等）
- 生成优化的渲染命令
- 处理剪裁和变换

## 工作流程

```
可视树 → 遍历 → 收集视觉信息 → 生成命令 → 渲染列表
```

## 相关文档

- [Visual.md](../UI/Visual.md) - 可视元素接口
- [RenderScene.md](./RenderScene.md) - 渲染场景
- [RenderCommand.md](./RenderCommand.md) - 渲染命令
