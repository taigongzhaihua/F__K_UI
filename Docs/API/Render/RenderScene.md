# RenderScene - 渲染场景

## 概述

`RenderScene` 管理一帧的渲染命令和数据，积累来自可视树的所有渲染指令，并生成最终的渲染列表供渲染器执行。

## 核心 API

### 重置场景

```cpp
void Reset();
```

清除所有渲染命令，准备新的一帧。

**调用时机：** 每帧开始时调用

### 访问命令缓冲

```cpp
RenderCommandBuffer& CommandBuffer() noexcept;
const RenderCommandBuffer& CommandBuffer() const noexcept;
```

获取当前帧的命令缓冲，用于添加渲染命令。

### 生成渲染列表

```cpp
std::unique_ptr<RenderList> GenerateRenderList();
```

将积累的命令转换为可被渲染器执行的渲染列表。

**返回值：** 优化后的渲染列表，可用于：
- 批处理优化
- 深度排序
- 渲染顺序调整

## 典型工作流程

```cpp
fk::render::RenderScene scene;

// 1. 重置场景（每帧开始）
scene.Reset();

// 2. 遍历可视树，添加渲染命令
void AddVisualCommands(const fk::ui::Visual* visual) {
    auto& buffer = scene.CommandBuffer();
    // 添加命令：背景、边框、文本等
    buffer.AddCommand(...);
}

// 3. 生成渲染列表
auto renderList = scene.GenerateRenderList();

// 4. 传递给渲染器
renderer->Render(*renderList);
```

## 相关文档

- [RenderCommandBuffer.md](./RenderCommandBuffer.md) - 命令缓冲
- [RenderList.md](./RenderList.md) - 渲染列表
- [RenderHost.md](./RenderHost.md) - 渲染主机
