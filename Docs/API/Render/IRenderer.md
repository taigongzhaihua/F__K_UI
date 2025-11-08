# IRenderer - 渲染器接口

## 概述

`IRenderer` 是渲染器的抽象接口,定义了渲染器的生命周期和核心功能。所有具体渲染器(如 `GlRenderer`)都实现此接口。

**命名空间**: `fk::render`  
**头文件**: `fk/render/IRenderer.h`

## 核心功能

- **抽象接口**: 定义渲染器标准接口
- **生命周期管理**: Initialize、Shutdown
- **帧渲染**: BeginFrame、Draw、EndFrame
- **窗口管理**: Resize
- **文本支持**: GetTextRenderer

## 数据结构

### Extent2D
```cpp
struct Extent2D {
    std::uint32_t width{0};
    std::uint32_t height{0};
};
```
表示 2D 尺寸。

### RendererInitParams
```cpp
struct RendererInitParams {
    void* nativeSurfaceHandle{nullptr};  // 原生窗口句柄(HWND/Window*)
    Extent2D initialSize{};              // 初始大小
    float pixelRatio{1.0f};              // DPI 缩放比例
    bool enableDebugLayer{false};        // 启用调试层(用于开发)
    std::string rendererName;            // 渲染器名称
};
```
渲染器初始化参数。

### FrameContext
```cpp
struct FrameContext {
    double elapsedSeconds{0.0};               // 应用启动以来的总时间
    double deltaSeconds{0.0};                 // 距离上一帧的时间
    std::array<float, 4> clearColor{0,0,0,0}; // 清屏颜色(RGBA)
    std::uint64_t frameIndex{0};              // 帧索引
};
```
帧上下文信息。

## API 参考

### 纯虚方法

```cpp
virtual void Initialize(const RendererInitParams& params) = 0;
```
初始化渲染器。
- **参数**: `params` - 初始化参数
- **说明**: 创建渲染上下文、加载着色器、初始化资源

```cpp
virtual void Resize(const Extent2D& size) = 0;
```
调整渲染器大小(响应窗口大小改变)。
- **参数**: `size` - 新的窗口大小
- **说明**: 重新创建交换链、帧缓冲等

```cpp
virtual void BeginFrame(const FrameContext& ctx) = 0;
```
开始一帧渲染。
- **参数**: `ctx` - 帧上下文
- **说明**: 清屏、重置状态、开始记录命令

```cpp
virtual void Draw(const RenderList& list) = 0;
```
绘制渲染列表。
- **参数**: `list` - 包含所有渲染命令的列表
- **说明**: 遍历命令并执行绘制

```cpp
virtual void EndFrame() = 0;
```
结束一帧渲染。
- **说明**: 提交命令缓冲、交换缓冲区、显示帧

```cpp
virtual void Shutdown() = 0;
```
关闭渲染器。
- **说明**: 释放所有资源、销毁渲染上下文

### 可选方法

```cpp
virtual TextRenderer* GetTextRenderer() const { return nullptr; }
```
获取 TextRenderer 实例(用于文本测量)。
- **返回**: TextRenderer 指针,如果渲染器不支持返回 `nullptr`
- **说明**: 默认实现返回 `nullptr`,子类可覆盖

## 使用示例

### 1. 实现自定义渲染器

```cpp
#include <fk/render/IRenderer.h>

class MyRenderer : public fk::render::IRenderer {
public:
    void Initialize(const fk::render::RendererInitParams& params) override {
        // 保存参数
        surfaceHandle_ = params.nativeSurfaceHandle;
        width_ = params.initialSize.width;
        height_ = params.initialSize.height;
        
        // 创建渲染上下文
        CreateContext();
        
        // 加载着色器
        LoadShaders();
        
        // 初始化资源
        InitializeResources();
    }
    
    void Resize(const fk::render::Extent2D& size) override {
        width_ = size.width;
        height_ = size.height;
        
        // 重新创建帧缓冲
        RecreateFramebuffer();
    }
    
    void BeginFrame(const fk::render::FrameContext& ctx) override {
        // 清屏
        ClearScreen(ctx.clearColor);
        
        // 重置渲染状态
        ResetState();
    }
    
    void Draw(const fk::render::RenderList& list) override {
        // 遍历渲染命令
        for (const auto& cmd : list.Commands()) {
            ExecuteCommand(cmd);
        }
    }
    
    void EndFrame() override {
        // 交换缓冲区
        SwapBuffers();
    }
    
    void Shutdown() override {
        // 释放资源
        ReleaseResources();
        
        // 销毁上下文
        DestroyContext();
    }

private:
    void* surfaceHandle_{nullptr};
    std::uint32_t width_{0};
    std::uint32_t height_{0};
    
    void CreateContext() { /* ... */ }
    void LoadShaders() { /* ... */ }
    void InitializeResources() { /* ... */ }
    void RecreateFramebuffer() { /* ... */ }
    void ClearScreen(const std::array<float, 4>& color) { /* ... */ }
    void ResetState() { /* ... */ }
    void ExecuteCommand(const fk::render::RenderCommand& cmd) { /* ... */ }
    void SwapBuffers() { /* ... */ }
    void ReleaseResources() { /* ... */ }
    void DestroyContext() { /* ... */ }
};
```

### 2. 使用 GlRenderer

```cpp
#include <fk/render/GlRenderer.h>
#include <fk/render/RenderHost.h>

// 创建 OpenGL 渲染器
auto renderer = std::make_unique<fk::render::GlRenderer>();

// 初始化参数
fk::render::RendererInitParams params{
    .nativeSurfaceHandle = windowHandle,
    .initialSize = {1920, 1080},
    .pixelRatio = 1.5f,  // 4K 显示器
    .enableDebugLayer = true,
    .rendererName = "OpenGL 4.5"
};

// 初始化
renderer->Initialize(params);

// 创建 RenderHost
fk::render::RenderHostConfig config{
    .window = window,
    .renderer = std::move(renderer)
};
auto renderHost = std::make_unique<fk::render::RenderHost>(std::move(config));
```

### 3. 渲染循环

```cpp
#include <fk/render/IRenderer.h>

void RenderLoop(fk::render::IRenderer& renderer, 
                fk::render::RenderHost& renderHost,
                fk::ui::Visual& root) {
    std::uint64_t frameIndex = 0;
    auto startTime = std::chrono::steady_clock::now();
    
    while (isRunning) {
        // 计算帧时间
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration<double>(now - startTime).count();
        static auto lastFrameTime = now;
        auto delta = std::chrono::duration<double>(now - lastFrameTime).count();
        lastFrameTime = now;
        
        // 构建帧上下文
        fk::render::FrameContext frameCtx{
            .elapsedSeconds = elapsed,
            .deltaSeconds = delta,
            .clearColor = {0.1f, 0.1f, 0.1f, 1.0f},
            .frameIndex = frameIndex++
        };
        
        // 开始一帧
        renderer.BeginFrame(frameCtx);
        
        // 构建并提交渲染列表
        renderHost.RenderFrame(frameCtx, root);
        
        // 结束一帧
        renderer.EndFrame();
    }
    
    // 关闭渲染器
    renderer.Shutdown();
}
```

### 4. 响应窗口大小改变

```cpp
#include <fk/render/IRenderer.h>
#include <fk/ui/Window.h>

void SetupRenderer(fk::render::IRenderer& renderer, 
                   fk::ui::Window& window) {
    // 订阅窗口大小改变事件
    window.SizeChanged.Subscribe([&renderer](const auto& e) {
        fk::render::Extent2D newSize{
            static_cast<std::uint32_t>(e.NewSize.Width),
            static_cast<std::uint32_t>(e.NewSize.Height)
        };
        
        // 通知渲染器调整大小
        renderer.Resize(newSize);
    });
}
```

### 5. 获取文本渲染器

```cpp
#include <fk/render/IRenderer.h>
#include <fk/render/TextRenderer.h>

void MeasureText(fk::render::IRenderer& renderer) {
    // 尝试获取 TextRenderer
    auto* textRenderer = renderer.GetTextRenderer();
    
    if (textRenderer) {
        // 测量文本
        auto size = textRenderer->MeasureText("Hello", "Arial", 16.0f);
        std::cout << "文本大小: " << size.width << "x" << size.height << "\n";
    } else {
        std::cerr << "渲染器不支持文本测量\n";
    }
}
```

### 6. 多后端支持

```cpp
#include <fk/render/IRenderer.h>
#include <fk/render/GlRenderer.h>
// #include <fk/render/VulkanRenderer.h>  // 未来支持

enum class RendererType {
    OpenGL,
    Vulkan,
    DirectX
};

std::unique_ptr<fk::render::IRenderer> CreateRenderer(RendererType type) {
    switch (type) {
        case RendererType::OpenGL:
            return std::make_unique<fk::render::GlRenderer>();
        
        // 未来扩展:
        // case RendererType::Vulkan:
        //     return std::make_unique<fk::render::VulkanRenderer>();
        
        default:
            throw std::runtime_error("不支持的渲染器类型");
    }
}

int main() {
    // 根据配置选择渲染器
    auto renderer = CreateRenderer(RendererType::OpenGL);
    
    // 使用统一接口
    renderer->Initialize(params);
    // ...
    renderer->Shutdown();
}
```

## 最佳实践

### 1. 资源管理
```cpp
// 推荐:显式生命周期管理
renderer->Initialize(params);
// ... 使用 ...
renderer->Shutdown();

// 避免:依赖析构函数
// 显式调用 Shutdown() 更清晰
```

### 2. 错误处理
```cpp
// 推荐:捕获初始化错误
try {
    renderer->Initialize(params);
} catch (const std::exception& e) {
    std::cerr << "渲染器初始化失败: " << e.what() << "\n";
    return false;
}
```

### 3. 调试支持
```cpp
// 推荐:开发时启用调试层
#ifdef _DEBUG
    params.enableDebugLayer = true;
#endif

renderer->Initialize(params);
```

### 4. DPI 感知
```cpp
// 推荐:根据显示器 DPI 设置 pixelRatio
float dpiScale = GetSystemDpiScale();
params.pixelRatio = dpiScale;
renderer->Initialize(params);
```

### 5. 帧率控制
```cpp
// 推荐:控制帧率避免过度渲染
auto frameStart = std::chrono::steady_clock::now();

renderer->BeginFrame(frameCtx);
// ... 渲染 ...
renderer->EndFrame();

// 限制为 60 FPS
auto frameEnd = std::chrono::steady_clock::now();
auto elapsed = std::chrono::duration<double>(frameEnd - frameStart).count();
auto targetFrameTime = 1.0 / 60.0;
if (elapsed < targetFrameTime) {
    auto sleepTime = targetFrameTime - elapsed;
    std::this_thread::sleep_for(std::chrono::duration<double>(sleepTime));
}
```

## 常见问题

### Q1: IRenderer 是线程安全的吗?
不是。所有渲染器方法必须在同一线程(渲染线程/UI 线程)上调用。

### Q2: 如何选择渲染器?
```cpp
// 当前仅支持 GlRenderer
auto renderer = std::make_unique<fk::render::GlRenderer>();

// 未来可能支持 Vulkan、DirectX 等
```

### Q3: 如何处理渲染失败?
```cpp
try {
    renderer->BeginFrame(frameCtx);
    renderer->Draw(list);
    renderer->EndFrame();
} catch (const std::exception& e) {
    std::cerr << "渲染失败: " << e.what() << "\n";
    // 尝试恢复或关闭应用
}
```

### Q4: 如何优化渲染性能?
- 使用失效追踪避免不必要的重绘
- 批量提交渲染命令
- 缓存静态资源(字体、纹理等)
- 控制帧率避免过度渲染

### Q5: 窗口大小改变时如何处理?
调用 `Resize()` 方法,渲染器会重新创建帧缓冲和交换链。

## 性能考虑

### 1. 初始化开销
```cpp
// Initialize() 开销较大(加载着色器、创建资源)
// 应在应用启动时调用,而非每帧调用
renderer->Initialize(params);
```

### 2. 调整大小
```cpp
// Resize() 开销中等(重建帧缓冲)
// 建议防抖处理,避免频繁调用
```

### 3. 帧渲染
```cpp
// BeginFrame/Draw/EndFrame 每帧调用
// 优化 Draw() 中的命令数量和复杂度
```

### 4. 关闭清理
```cpp
// Shutdown() 应在应用退出时调用一次
renderer->Shutdown();
```

## 实现指南

### 必须实现的方法
1. `Initialize()`: 创建渲染上下文
2. `Resize()`: 处理窗口大小改变
3. `BeginFrame()`: 清屏、重置状态
4. `Draw()`: 执行渲染命令
5. `EndFrame()`: 交换缓冲区
6. `Shutdown()`: 释放资源

### 可选实现
1. `GetTextRenderer()`: 支持文本测量

### 实现清单
- [ ] 平台相关的上下文创建(OpenGL/Vulkan/DirectX)
- [ ] 着色器加载和编译
- [ ] 资源管理(顶点缓冲、纹理等)
- [ ] 渲染命令执行
- [ ] 帧缓冲管理
- [ ] 文本渲染(可选)
- [ ] 调试层支持(可选)

## 相关类

- **GlRenderer**: OpenGL 渲染器实现
- **RenderHost**: 渲染主机
- **RenderList**: 渲染命令列表
- **RenderCommand**: 渲染命令
- **TextRenderer**: 文本渲染器

## 参考

- [GlRenderer.md](GlRenderer.md) - OpenGL 渲染器
- [RenderHost.md](RenderHost.md) - 渲染主机
- [RenderCommand.md](RenderCommand.md) - 渲染命令
