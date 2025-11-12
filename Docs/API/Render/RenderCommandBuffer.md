# RenderCommandBuffer

## 概览

**目的**：渲染命令缓冲区

**命名空间**：`fk::render`

**头文件**：`fk/render/RenderCommandBuffer.h`

## 描述

`RenderCommandBuffer` 存储渲染命令序列。

## 公共接口

### 命令管理

#### AddCommand
```cpp
void AddCommand(const RenderCommand& command);
```

#### Clear
```cpp
void Clear();
```

#### GetCommands
```cpp
const std::vector<RenderCommand>& GetCommands() const;
```

## 使用示例

```cpp
RenderCommandBuffer buffer;

buffer.AddCommand(FillRectCommand(rect, color));
buffer.AddCommand(DrawTextCommand(text, pos, style));

// 执行命令
renderer->Execute(buffer);
```

## 相关类

- [RenderCommand](RenderCommand.md)
- [Renderer](Renderer.md)
