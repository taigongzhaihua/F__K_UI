# Renderer - 设计文档

## 概览

**目的**：主渲染管线和渲染树管理

## 渲染管线

```
视觉树
  ↓
RenderTreeBuilder
  ↓
渲染命令
  ↓
RenderCommandBuffer
  ↓
GlRenderer
  ↓
OpenGL
  ↓
屏幕
```

## 渲染流程

### 1. 构建阶段

```cpp
void Renderer::Render(Visual* root) {
    // 清空命令缓冲
    commandBuffer_.Clear();
    
    // 遍历视觉树生成命令
    RenderTreeBuilder builder;
    builder.Build(root, commandBuffer_);
    
    // 执行命令
    backend_->Execute(commandBuffer_);
}
```

### 2. 执行阶段

```cpp
void GlRenderer::Execute(const RenderCommandBuffer& buffer) {
    for (auto& command : buffer.GetCommands()) {
        switch (command.Type) {
            case CommandType::FillRect:
                DrawFilledRectangle(command);
                break;
            // ...
        }
    }
}
```

## 优化策略

1. **批处理** - 合并相似的绘制命令
2. **剔除** - 跳过不可见元素
3. **缓存** - 缓存不变的渲染树部分
4. **GPU上传** - 最小化CPU-GPU数据传输

## 另请参阅

- [API文档](../../API/Render/Renderer.md)
- [GlRenderer设计](GlRenderer.md)
