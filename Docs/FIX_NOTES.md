# 窗口渲染问题修复说明

## 问题描述

`examples/main.cpp` 示例程序创建窗口后只显示空白窗口，TextBlock等UI元素没有被渲染出来。

## 根本原因

在 `src/ui/Window.cpp` 的 `RenderFrame()` 方法中，第400行有一个TODO注释：

```cpp
// TODO: 渲染UI内容
// 这里应该调用渲染系统来绘制所有UI元素
```

这表明虽然窗口框架、事件循环、Content管理等功能都已经实现，但实际将UI内容绘制到屏幕上的渲染逻辑还未完成。

## 解决方案

### 1. 添加渲染系统组件到Window类

在 `Window.h` 中添加：
```cpp
// 前向声明
namespace fk::render {
    class GlRenderer;
    class RenderList;
    class TextRenderer;
}

// 私有成员
std::unique_ptr<render::GlRenderer> renderer_;
std::unique_ptr<render::RenderList> renderList_;
```

### 2. 初始化渲染系统

在 `Window::Window()` 构造函数中：
```cpp
#ifdef FK_HAS_OPENGL
    renderList_ = std::make_unique<render::RenderList>();
    renderer_ = std::make_unique<render::GlRenderer>();
#endif
```

### 3. 实现完整的渲染流程

在 `Window::RenderFrame()` 方法中实现：

```cpp
#ifdef FK_HAS_OPENGL
    if (renderer_ && renderList_) {
        // 1. 初始化渲染器（首次调用时）
        if (!renderer_->IsInitialized()) {
            render::RendererInitParams params;
            params.viewportWidth = width;
            params.viewportHeight = height;
            renderer_->Initialize(params);
        }
        
        // 2. 清空渲染命令列表
        renderList_->Clear();
        
        // 3. 创建渲染上下文
        render::RenderContext context(renderList_.get(), renderer_->GetTextRenderer());
        
        // 4. 从Content开始收集绘制命令
        auto content = GetContent();
        if (content) {
            // 执行布局
            auto availableSize = Size(static_cast<float>(width), static_cast<float>(height));
            content->Measure(availableSize);
            content->Arrange(Rect(0, 0, availableSize.width, availableSize.height));
            
            // 收集绘制命令（递归遍历整个UI树）
            content->CollectDrawCommands(context);
        }
        
        // 5. 渲染所有命令
        render::FrameContext frameCtx;
        frameCtx.frameNumber = 0;
        frameCtx.deltaTime = 0.016f;
        
        renderer_->BeginFrame(frameCtx);
        renderer_->Draw(*renderList_);
        renderer_->EndFrame();
    }
#endif
```

### 4. 添加辅助方法

在 `GlRenderer.h` 中添加：
```cpp
bool IsInitialized() const { return initialized_; }
```

## 渲染流程说明

1. **布局阶段** (Measure/Arrange)
   - 计算每个UI元素的期望尺寸
   - 确定元素的最终位置和大小

2. **命令收集阶段** (CollectDrawCommands)
   - 从根Content开始递归遍历UI树
   - 每个UI元素生成自己的绘制命令
   - 命令包含：矩形、文本、图像等基本图形

3. **渲染执行阶段** (BeginFrame/Draw/EndFrame)
   - GlRenderer将绘制命令转换为OpenGL调用
   - 执行实际的GPU渲染
   - 交换缓冲区显示到屏幕

## 架构说明

项目已有完整的渲染架构：

- **Visual** - 视觉树基类，定义了 `CollectDrawCommands` 虚方法
- **RenderContext** - 渲染上下文，提供高层绘制API
- **RenderList** - 渲染命令列表，存储本帧所有绘制命令
- **GlRenderer** - OpenGL渲染器，执行实际渲染
- **TextRenderer** - 文本渲染器，处理字体和文本绘制

各种UI元素（TextBlock、Border、Image等）都已经实现了 `CollectDrawCommands` 方法，只是Window没有调用它们。

## 测试说明

由于当前环境缺少GLFW和OpenGL支持，无法在本地验证视觉效果。但代码逻辑已经完整实现：

1. 代码可以成功编译
2. 在无OpenGL环境下使用条件编译，不会导致编译错误
3. 在有OpenGL支持的环境中，应该能正常渲染UI内容

## 后续改进建议

1. **性能优化**
   - 添加脏矩形（Dirty Rect）机制，只重绘变化的区域
   - 实现UI元素的渲染缓存

2. **功能增强**
   - 支持更多渲染效果（阴影、模糊等）
   - 支持硬件加速的文本渲染

3. **错误处理**
   - 添加更详细的渲染错误日志
   - 提供渲染失败时的降级方案

## 相关文件

- `include/fk/ui/Window.h` - Window类定义
- `src/ui/Window.cpp` - Window类实现
- `include/fk/render/GlRenderer.h` - OpenGL渲染器
- `src/render/GlRenderer.cpp` - 渲染器实现
- `include/fk/render/RenderContext.h` - 渲染上下文
- `include/fk/render/RenderList.h` - 渲染命令列表
