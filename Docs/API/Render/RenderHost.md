# RenderHost - 渲染主机

## 概述

`RenderHost` 是连接 UI 树和渲染器的桥梁,负责将 UI 元素转换为渲染命令并提交给底层渲染器。它管理渲染循环、失效追踪和渲染优化。

**命名空间**: `fk::render`  
**头文件**: `fk/render/RenderHost.h`

## 核心功能

- **渲染管理**: 协调 UI 树和渲染器
- **失效追踪**: 记录需要重绘的元素,避免全屏重绘
- **渲染优化**: 仅在必要时触发渲染
- **场景构建**: 将 Visual 树转换为 RenderScene
- **文本测量**: 提供 TextRenderer 用于文本布局

## API 参考

### 构造/析构

```cpp
explicit RenderHost(RenderHostConfig config);
~RenderHost();

// 不可拷贝、不可移动
RenderHost(const RenderHost&) = delete;
RenderHost& operator=(const RenderHost&) = delete;
RenderHost(RenderHost&&) noexcept = delete;
RenderHost& operator=(RenderHost&&) noexcept = delete;
```

**RenderHostConfig**:
```cpp
struct RenderHostConfig {
    std::shared_ptr<ui::Window> window;      // 关联的窗口
    std::unique_ptr<IRenderer> renderer;     // 渲染器实例
};
```

### 核心方法

```cpp
void Initialize(const RendererInitParams& params);
```
初始化渲染器。
- **参数**: `params` - 渲染器初始化参数(窗口句柄、大小、DPI 等)

```cpp
void RequestRender();
```
请求在下一帧渲染。设置 `renderPending_` 标志。

```cpp
bool RenderFrame(const FrameContext& frameContext, const ui::Visual& visualRoot);
```
渲染一帧。
- **参数**: 
  - `frameContext` - 帧上下文(时间、帧索引等)
  - `visualRoot` - 可视树根节点
- **返回**: 如果渲染成功返回 `true`

```cpp
void Resize(const Extent2D& newSize);
```
调整渲染器大小(响应窗口大小改变)。

```cpp
void Shutdown();
```
关闭渲染器,释放资源。

### 失效追踪

```cpp
void InvalidateElement(ui::UIElement* element);
```
标记元素需要重绘。
- **说明**: 当元素调用 `InvalidateVisual()` 时调用此方法
- **优化**: RenderHost 记录所有失效元素,下一帧统一处理

```cpp
void ClearDirtyElements();
```
清除失效追踪(渲染完成后调用)。

```cpp
[[nodiscard]] bool HasDirtyElements() const noexcept;
```
检查是否有失效元素。

```cpp
[[nodiscard]] bool IsRenderPending() const noexcept;
```
检查是否有待处理的渲染请求。

### 文本测量

```cpp
TextRenderer* GetTextRenderer() const;
```
获取 TextRenderer 用于文本测量。
- **返回**: TextRenderer 指针,如果不可用返回 `nullptr`

## 使用示例

### 1. 创建 RenderHost

```cpp
#include <fk/render/RenderHost.h>
#include <fk/render/GlRenderer.h>
#include <fk/ui/Window.h>

// 创建窗口
auto window = fk::ui::Window::Create();
window->Width(800)->Height(600);

// 创建渲染器
auto renderer = std::make_unique<fk::render::GlRenderer>();

// 创建 RenderHost
fk::render::RenderHostConfig config{
    .window = window,
    .renderer = std::move(renderer)
};
auto renderHost = std::make_unique<fk::render::RenderHost>(std::move(config));

// 初始化渲染器
fk::render::RendererInitParams params{
    .nativeSurfaceHandle = window->GetNativeHandle(),
    .initialSize = {800, 600},
    .pixelRatio = 1.0f,
    .enableDebugLayer = true,
    .rendererName = "OpenGL"
};
renderHost->Initialize(params);
```

### 2. 渲染循环

```cpp
#include <fk/render/RenderHost.h>
#include <fk/ui/UIElement.h>

// 主渲染循环
void RenderLoop(fk::render::RenderHost& renderHost, fk::ui::Visual& root) {
    std::uint64_t frameIndex = 0;
    auto startTime = std::chrono::steady_clock::now();
    
    while (true) {
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration<double>(now - startTime).count();
        
        // 构建帧上下文
        fk::render::FrameContext frameCtx{
            .elapsedSeconds = elapsed,
            .deltaSeconds = 1.0 / 60.0,  // 假设 60 FPS
            .clearColor = {0.1f, 0.1f, 0.1f, 1.0f},
            .frameIndex = frameIndex++
        };
        
        // 渲染一帧
        if (renderHost.IsRenderPending() || renderHost.HasDirtyElements()) {
            renderHost.RenderFrame(frameCtx, root);
        }
        
        // 限制帧率
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }
}
```

### 3. 失效追踪

```cpp
#include <fk/render/RenderHost.h>
#include <fk/ui/Button.h>

auto button = fk::ui::Button::Create();

// 当按钮属性改变时,自动标记为失效
button->Background(fk::ui::Colors::Red);  
// 内部调用: InvalidateVisual()
// 这会调用 renderHost->InvalidateElement(this)

// RenderHost 记录失效元素
// 下一帧统一重绘所有失效元素

// 渲染完成后清除
renderHost.ClearDirtyElements();
```

### 4. 响应窗口大小改变

```cpp
#include <fk/render/RenderHost.h>
#include <fk/ui/Window.h>

auto window = fk::ui::Window::Create();
auto renderHost = CreateRenderHost(window);

// 订阅窗口大小改变事件
window->SizeChanged.Subscribe([&renderHost](const fk::ui::SizeChangedEventArgs& e) {
    // 通知 RenderHost 调整大小
    fk::render::Extent2D newSize{
        static_cast<std::uint32_t>(e.NewSize.Width),
        static_cast<std::uint32_t>(e.NewSize.Height)
    };
    
    renderHost.Resize(newSize);
    renderHost.RequestRender();  // 请求重绘
});
```

### 5. 文本测量

```cpp
#include <fk/render/RenderHost.h>
#include <fk/render/TextRenderer.h>

auto renderHost = CreateRenderHost(window);

// 获取 TextRenderer
auto* textRenderer = renderHost.GetTextRenderer();
if (textRenderer) {
    // 测量文本尺寸
    auto size = textRenderer->MeasureText(
        "Hello World",
        "Arial",
        16.0f
    );
    
    std::cout << "文本宽度: " << size.width << "\n";
    std::cout << "文本高度: " << size.height << "\n";
}
```

### 6. 按需渲染

```cpp
#include <fk/render/RenderHost.h>

// 仅在必要时渲染(节省资源)
void OptimizedRenderLoop(fk::render::RenderHost& renderHost, fk::ui::Visual& root) {
    while (true) {
        // 检查是否需要渲染
        if (renderHost.IsRenderPending() || renderHost.HasDirtyElements()) {
            // 构建帧上下文
            fk::render::FrameContext frameCtx = BuildFrameContext();
            
            // 渲染
            renderHost.RenderFrame(frameCtx, root);
            
            // 清除失效标记
            renderHost.ClearDirtyElements();
        } else {
            // 没有更新,休眠节省 CPU
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }
}
```

## 最佳实践

### 1. 生命周期管理
```cpp
// 推荐:按顺序初始化和关闭
renderHost->Initialize(params);
// ... 使用 ...
renderHost->Shutdown();
```

### 2. 失效追踪
```cpp
// 推荐:利用自动失效追踪
button->Background(Colors::Red);  // 自动标记失效

// 避免:手动调用 RequestRender()
renderHost.RequestRender();  // 不必要,除非全屏更新
```

### 3. 帧率控制
```cpp
// 推荐:根据失效状态决定是否渲染
if (renderHost.IsRenderPending() || renderHost.HasDirtyElements()) {
    renderHost.RenderFrame(frameCtx, root);
}

// 避免:无条件渲染
renderHost.RenderFrame(frameCtx, root);  // 浪费资源
```

### 4. 窗口关联
```cpp
// 推荐:一个窗口一个 RenderHost
auto renderHost = std::make_unique<RenderHost>(config);

// 避免:多个窗口共享 RenderHost
```

### 5. 资源清理
```cpp
// 推荐:显式关闭
renderHost->Shutdown();

// 析构函数也会自动清理,但显式调用更清晰
```

## 常见问题

### Q1: 什么时候需要调用 RequestRender()?
通常不需要手动调用。UI 元素属性改变时会自动标记为失效。仅在需要强制全屏重绘时调用。

### Q2: 如何实现高效渲染?
```cpp
// 利用失效追踪,仅在有更新时渲染
if (renderHost.HasDirtyElements()) {
    renderHost.RenderFrame(frameCtx, root);
    renderHost.ClearDirtyElements();
}
```

### Q3: RenderHost 线程安全吗?
不是。RenderHost 必须在 UI 线程上使用。

### Q4: 如何处理渲染错误?
```cpp
try {
    bool success = renderHost.RenderFrame(frameCtx, root);
    if (!success) {
        std::cerr << "渲染失败\n";
    }
} catch (const std::exception& e) {
    std::cerr << "渲染异常: " << e.what() << "\n";
}
```

### Q5: 如何获取渲染统计信息?
FrameContext 包含帧索引和时间信息,可用于计算 FPS:
```cpp
static auto lastTime = std::chrono::steady_clock::now();
auto now = std::chrono::steady_clock::now();
auto delta = std::chrono::duration<double>(now - lastTime).count();
double fps = 1.0 / delta;
lastTime = now;
```

## 性能优化

### 1. 失效追踪
- 仅重绘失效元素,避免全屏重绘
- 渲染后及时调用 `ClearDirtyElements()`

### 2. 帧率控制
```cpp
// 动态帧率:有更新时高帧率,静止时低帧率
if (renderHost.HasDirtyElements()) {
    std::this_thread::sleep_for(std::chrono::milliseconds(16));  // 60 FPS
} else {
    std::this_thread::sleep_for(std::chrono::milliseconds(100)); // 10 FPS
}
```

### 3. 批量更新
```cpp
// 推荐:批量更新属性后统一渲染
button1->Background(Colors::Red);
button2->Background(Colors::Blue);
button3->Background(Colors::Green);
// 三个按钮在同一帧渲染

// 避免:每次更新后立即渲染
```

### 4. 避免频繁调整大小
```cpp
// 推荐:防抖处理
std::chrono::steady_clock::time_point lastResize;
window->SizeChanged.Subscribe([&](auto& e) {
    lastResize = std::chrono::steady_clock::now();
});

// 延迟 100ms 后才真正调整大小
if (now - lastResize > std::chrono::milliseconds(100)) {
    renderHost.Resize(newSize);
}
```

## 内部机制

### 渲染流程
1. **失效追踪**: UI 元素改变时标记为 dirty
2. **场景构建**: 遍历 Visual 树,生成 RenderScene
3. **命令生成**: 将 Visual 转换为 RenderCommand
4. **提交渲染**: 调用 `IRenderer::Draw()`
5. **清除标记**: 调用 `ClearDirtyElements()`

### 失效传播
```
UIElement::InvalidateVisual()
  └─> RenderHost::InvalidateElement(this)
      └─> dirtyElements_.insert(this)
          └─> 下一帧: RenderFrame() 处理所有失效元素
```

## 相关类

- **IRenderer**: 渲染器接口
- **GlRenderer**: OpenGL 渲染器实现
- **RenderScene**: 渲染场景
- **RenderTreeBuilder**: 场景构建器
- **TextRenderer**: 文本渲染器

## 参考

- [IRenderer.md](IRenderer.md) - 渲染器接口
- [RenderCommand.md](RenderCommand.md) - 渲染命令
- [Window.md](../UI/Window.md) - 窗口文档
