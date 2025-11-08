# GlRenderer - OpenGL 渲染器

## 概述

`GlRenderer` 是 `IRenderer` 接口的 OpenGL 实现,负责使用 OpenGL 图形 API 执行实际的渲染工作。

**命名空间**: `fk::render`  
**头文件**: `fk/render/GlRenderer.h`  
**继承**: `IRenderer`

## 核心功能

- **OpenGL 渲染**: 使用 OpenGL 3.3+ Core Profile
- **着色器管理**: 加载和编译着色器程序
- **命令执行**: 将渲染命令转换为 OpenGL 调用
- **文本渲染**: 集成 FreeType 文本渲染
- **图层支持**: 透明度和混合模式
- **裁剪区域**: 剪裁框实现

## API 参考

### 构造/析构

```cpp
GlRenderer();
~GlRenderer() override;
```

### IRenderer 接口实现

```cpp
void Initialize(const RendererInitParams& params) override;
```
初始化 OpenGL 渲染器。
- 创建 OpenGL 上下文
- 加载 GLAD
- 编译着色器
- 初始化缓冲区
- 创建 TextRenderer

```cpp
void Resize(const Extent2D& size) override;
```
调整视口大小。

```cpp
void BeginFrame(const FrameContext& ctx) override;
```
开始一帧渲染。
- 清屏
- 重置状态
- 更新投影矩阵

```cpp
void Draw(const RenderList& list) override;
```
绘制渲染列表中的所有命令。

```cpp
void EndFrame() override;
```
结束一帧渲染。
- 交换缓冲区

```cpp
void Shutdown() override;
```
关闭渲染器,释放 OpenGL 资源。

```cpp
TextRenderer* GetTextRenderer() const override;
```
获取文本渲染器实例。

## 使用示例

### 1. 创建和初始化

```cpp
#include <fk/render/GlRenderer.h>

// 创建渲染器
auto renderer = std::make_unique<fk::render::GlRenderer>();

// 初始化参数
fk::render::RendererInitParams params{
    .nativeSurfaceHandle = GetWindowHandle(),  // 窗口句柄
    .initialSize = {1920, 1080},
    .pixelRatio = 1.0f,
    .enableDebugLayer = true,  // 开发时启用
    .rendererName = "OpenGL 3.3"
};

// 初始化
try {
    renderer->Initialize(params);
} catch (const std::exception& e) {
    std::cerr << "渲染器初始化失败: " << e.what() << "\n";
    return false;
}
```

### 2. 渲染循环

```cpp
#include <fk/render/GlRenderer.h>
#include <fk/render/RenderList.h>

void RenderLoop(fk::render::GlRenderer& renderer) {
    while (isRunning) {
        // 构建帧上下文
        fk::render::FrameContext frameCtx{
            .elapsedSeconds = GetElapsedTime(),
            .deltaSeconds = GetDeltaTime(),
            .clearColor = {0.1f, 0.1f, 0.15f, 1.0f},
            .frameIndex = frameIndex++
        };
        
        // 开始一帧
        renderer.BeginFrame(frameCtx);
        
        // 获取渲染列表
        auto renderList = BuildRenderList();
        
        // 绘制
        renderer.Draw(renderList);
        
        // 结束一帧(交换缓冲区)
        renderer.EndFrame();
    }
    
    // 关闭渲染器
    renderer.Shutdown();
}
```

### 3. 文本渲染

```cpp
#include <fk/render/GlRenderer.h>
#include <fk/render/TextRenderer.h>

void SetupTextRendering(fk::render::GlRenderer& renderer) {
    // 获取 TextRenderer
    auto* textRenderer = renderer.GetTextRenderer();
    
    if (textRenderer) {
        // 加载字体
        int fontId = textRenderer->LoadFont("Arial.ttf", 16);
        
        if (fontId >= 0) {
            // 测量文本
            int width, height;
            textRenderer->MeasureText("Hello", fontId, width, height);
            
            std::cout << "文本大小: " << width << "x" << height << "\n";
        }
    }
}
```

### 4. 处理窗口大小改变

```cpp
#include <fk/render/GlRenderer.h>

void OnWindowResize(fk::render::GlRenderer& renderer, int width, int height) {
    // 调整渲染器大小
    fk::render::Extent2D newSize{
        static_cast<std::uint32_t>(width),
        static_cast<std::uint32_t>(height)
    };
    
    renderer.Resize(newSize);
}
```

### 5. 调试模式

```cpp
#include <fk/render/GlRenderer.h>

int main() {
    auto renderer = std::make_unique<fk::render::GlRenderer>();
    
    fk::render::RendererInitParams params;
    
    #ifdef _DEBUG
        // 开发模式:启用调试层
        params.enableDebugLayer = true;
    #else
        // 发布模式:禁用调试层
        params.enableDebugLayer = false;
    #endif
    
    renderer->Initialize(params);
    
    // ...
}
```

### 6. 高 DPI 支持

```cpp
#include <fk/render/GlRenderer.h>

void SetupHighDPI(fk::render::GlRenderer& renderer, float dpiScale) {
    fk::render::RendererInitParams params{
        .nativeSurfaceHandle = windowHandle,
        .initialSize = {1920, 1080},
        .pixelRatio = dpiScale,  // 1.0 (普通), 1.5 (150%), 2.0 (200%)
        .enableDebugLayer = false,
        .rendererName = "OpenGL"
    };
    
    renderer.Initialize(params);
}
```

## 内部机制

### 着色器

GlRenderer 使用两个着色器程序:

1. **矩形着色器**: 绘制实心矩形和圆角矩形
2. **文本着色器**: 渲染文本字形纹理

### 顶点缓冲区

- **矩形 VAO/VBO**: 用于绘制矩形
- **文本 VAO/VBO**: 用于绘制文本

### 状态管理

- **变换栈**: 管理坐标变换(偏移)
- **透明度栈**: 管理图层不透明度
- **裁剪区域**: 使用 OpenGL Scissor Test

### 命令执行流程

```
Draw(RenderList)
  └─> 遍历命令
      ├─> SetClip → glEnable/glDisable(GL_SCISSOR_TEST)
      ├─> SetTransform → 更新 currentOffsetX/Y
      ├─> DrawRectangle → 绑定 VAO, 设置 uniform, glDrawArrays
      ├─> DrawText → 使用 TextRenderer, 绘制字形纹理
      ├─> PushLayer → 压入透明度栈
      └─> PopLayer → 弹出透明度栈
```

## 最佳实践

### 1. 资源清理
```cpp
// 推荐:显式关闭
renderer->Shutdown();

// 析构函数也会清理,但显式调用更清晰
```

### 2. 错误处理
```cpp
// 推荐:捕获初始化异常
try {
    renderer->Initialize(params);
} catch (const std::runtime_error& e) {
    std::cerr << "OpenGL 初始化失败: " << e.what() << "\n";
    // 尝试回退到软件渲染或退出
}
```

### 3. 调试层
```cpp
// 推荐:仅在开发时启用
#ifdef _DEBUG
    params.enableDebugLayer = true;
#endif
```

### 4. 帧率限制
```cpp
// 推荐:控制帧率
auto frameStart = std::chrono::steady_clock::now();

renderer.BeginFrame(frameCtx);
renderer.Draw(list);
renderer.EndFrame();

auto elapsed = std::chrono::steady_clock::now() - frameStart;
auto targetFrameTime = std::chrono::milliseconds(16);  // ~60 FPS
if (elapsed < targetFrameTime) {
    std::this_thread::sleep_for(targetFrameTime - elapsed);
}
```

### 5. 文本缓存
```cpp
// 推荐:缓存字体和文本纹理
static int fontId = -1;
if (fontId < 0) {
    fontId = textRenderer->LoadFont("Arial.ttf", 16);
}
```

## 常见问题

### Q1: 支持哪些 OpenGL 版本?
OpenGL 3.3 Core Profile 或更高版本。

### Q2: 如何处理 OpenGL 上下文丢失?
```cpp
// 需要重新初始化
renderer->Shutdown();
renderer->Initialize(params);
```

### Q3: 如何优化渲染性能?
- 减少状态切换(批量绘制相同类型的命令)
- 缓存文本纹理
- 使用失效追踪避免全屏重绘
- 启用 VSync 避免撕裂

### Q4: 支持多线程渲染吗?
不支持。OpenGL 上下文必须在单线程上使用。

### Q5: 如何绘制自定义形状?
当前版本仅支持矩形和文本。自定义形状需要扩展 `RenderCommand` 和着色器。

## 性能考虑

### 初始化开销
- 加载 GLAD: 中等
- 编译着色器: 中等
- 初始化 FreeType: 较大
- **建议**: 在应用启动时初始化一次

### 渲染开销
- 矩形绘制: 极快
- 文本绘制: 中等(取决于字符数)
- 状态切换: 较快
- **优化**: 批量提交相同类型的命令

### 内存占用
- 着色器程序: ~10 KB
- 顶点缓冲区: ~1 KB
- 字体缓存: 取决于字体大小和字符集
- **优化**: 按需加载字体,及时释放

## 故障排除

### 黑屏/无输出
```cpp
// 检查清单:
// 1. OpenGL 上下文是否成功创建?
// 2. 着色器是否编译成功?
// 3. 视口大小是否正确?
// 4. 渲染命令列表是否为空?
```

### 文本不显示
```cpp
// 检查清单:
// 1. 字体是否加载成功?
// 2. TextRenderer 是否初始化?
// 3. 文本颜色是否与背景色相同?
// 4. 文本是否在视口范围内?
```

### 性能问题
```cpp
// 分析方法:
// 1. 统计每帧命令数量
// 2. 测量各阶段耗时(BeginFrame/Draw/EndFrame)
// 3. 检查是否有不必要的全屏重绘
// 4. 使用 OpenGL 性能分析工具(如 RenderDoc)
```

## 扩展开发

### 添加新的绘制命令

1. **扩展 RenderCommand**:
```cpp
enum class CommandType {
    // ... 现有类型 ...
    DrawCircle,
};

struct CirclePayload {
    ui::Point center;
    float radius;
    std::array<float, 4> color;
};
```

2. **实现 GlRenderer 方法**:
```cpp
void GlRenderer::ExecuteCommand(const RenderCommand& cmd) {
    switch (cmd.type) {
        // ... 现有命令 ...
        case CommandType::DrawCircle:
            DrawCircle(std::get<CirclePayload>(cmd.payload));
            break;
    }
}

void GlRenderer::DrawCircle(const CirclePayload& payload) {
    // 1. 绑定着色器
    // 2. 设置 uniform
    // 3. 绘制圆形
}
```

3. **添加着色器**:
```glsl
// circle.vert
#version 330 core
layout (location = 0) in vec2 aPos;
// ...

// circle.frag
#version 330 core
out vec4 FragColor;
// 使用距离场渲染圆形
void main() {
    float dist = length(gl_PointCoord - vec2(0.5));
    if (dist > 0.5) discard;
    FragColor = uColor;
}
```

## 依赖项

- **GLAD**: OpenGL 函数加载器
- **FreeType**: 文本渲染
- **GLM**: 数学库(可选,用于矩阵运算)

## 相关类

- **IRenderer**: 渲染器接口
- **TextRenderer**: 文本渲染器
- **RenderCommand**: 渲染命令
- **RenderList**: 渲染命令列表
- **RenderHost**: 渲染主机

## 参考

- [IRenderer.md](IRenderer.md) - 渲染器接口
- [TextRenderer.md](TextRenderer.md) - 文本渲染
- [RenderCommand.md](RenderCommand.md) - 渲染命令
- [RenderHost.md](RenderHost.md) - 渲染主机
- [OpenGL 官方文档](https://www.opengl.org/documentation/)
