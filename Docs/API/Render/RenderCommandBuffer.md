# RenderCommandBuffer - 渲染命令缓冲

## 概述

`RenderCommandBuffer` 是积累一帧内所有渲染命令的容器，提供了向缓冲中添加命令的接口。

## 主要功能

- 收集渲染命令
- 维护命令顺序
- 提供批处理优化机会

## 基本操作

```cpp
RenderCommandBuffer buffer;

// 添加命令
buffer.AddCommand(command1);
buffer.AddCommand(command2);

// 清空
buffer.Reset();
```

## 相关文档

- [RenderCommand.md](./RenderCommand.md) - 渲染命令
- [RenderScene.md](./RenderScene.md) - 渲染场景
