# Render 模块 API 文档

渲染引擎模块,提供基于 OpenGL 的 2D 渲染能力。

命名空间: `fk::render`

---

## 目录

- [IRenderer](#irenderer) - 渲染器接口
- [GlRenderer](#glrenderer) - OpenGL 渲染器实现
- [RenderHost](#renderhost) - 渲染主机
- [RenderCommand](#rendercommand) - 渲染命令
- [RenderList](#renderlist) - 渲染列表
- [TextRenderer](#textrenderer) - 文本渲染器
- [ColorUtils](#colorutils) - 颜色工具

---

## IRenderer

**渲染器接口** - 定义渲染器的基本契约。

### 类定义

```cpp
class IRenderer {
public:
    virtual ~IRenderer() = default;
    
    virtual void Initialize(const RendererInitParams& params) = 0;
    virtual void Resize(const Extent2D& size) = 0;
    virtual void BeginFrame(const FrameContext& ctx) = 0;
    virtual void Draw(const RenderList& list) = 0;
    virtual void EndFrame() = 0;
    virtual void Shutdown() = 0;
};
```

### 数据结构

#### Extent2D

二维尺寸

```cpp
struct Extent2D {
    std::uint32_t width{0};
    std::uint32_t height{0};
};
```

#### RendererInitParams

渲染器初始化参数

```cpp
struct RendererInitParams {
    void* nativeSurfaceHandle{nullptr};  // 原生窗口句柄
    Extent2D initialSize{};              // 初始大小
    float pixelRatio{1.0f};              // 像素比率 (用于高DPI)
    bool enableDebugLayer{false};        // 启用调试层
    std::string rendererName;            // 渲染器名称
};
```

#### FrameContext

帧上下文

```cpp
struct FrameContext {
    double elapsedSeconds{0.0};         // 总运行时间(秒)
    double deltaSeconds{0.0};           // 距上一帧的时间(秒)
    std::array<float, 4> clearColor{0.0f, 0.0f, 0.0f, 0.0f}; // 清屏颜色(RGBA)
    std::uint64_t frameIndex{0};        // 帧索引
};
```

### 核心方法

#### Initialize()

初始化渲染器

```cpp
virtual void Initialize(const RendererInitParams& params) = 0;
```

**参数:**
- `params` - 初始化参数

**线程安全:** 否(必须在渲染线程调用)

#### Resize()

调整渲染器大小

```cpp
virtual void Resize(const Extent2D& size) = 0;
```

**参数:**
- `size` - 新的尺寸

**线程安全:** 否

**使用场景:** 窗口大小改变时调用

#### BeginFrame()

开始渲染新的一帧

```cpp
virtual void BeginFrame(const FrameContext& ctx) = 0;
```

**参数:**
- `ctx` - 帧上下文

**线程安全:** 否

**注意:** 必须在 EndFrame() 之前调用

#### Draw()

执行渲染命令列表

```cpp
virtual void Draw(const RenderList& list) = 0;
```

**参数:**
- `list` - 渲染命令列表

**线程安全:** 否

**注意:** 必须在 BeginFrame() 和 EndFrame() 之间调用

#### EndFrame()

结束当前帧的渲染

```cpp
virtual void EndFrame() = 0;
```

**线程安全:** 否

**作用:** 提交渲染命令,交换缓冲区

#### Shutdown()

关闭渲染器,释放资源

```cpp
virtual void Shutdown() = 0;
```

**线程安全:** 否

---

## GlRenderer

**OpenGL 渲染器** - IRenderer 的 OpenGL 实现。

### 类定义

```cpp
class GlRenderer : public IRenderer {
public:
    GlRenderer();
    ~GlRenderer() override;
    
    void Initialize(const RendererInitParams& params) override;
    void Resize(const Extent2D& size) override;
    void BeginFrame(const FrameContext& ctx) override;
    void Draw(const RenderList& list) override;
    void EndFrame() override;
    void Shutdown() override;
};
```

### 使用示例

```cpp
#include "fk/render/GlRenderer.h"

auto renderer = std::make_unique<GlRenderer>();

RendererInitParams params{};
params.nativeSurfaceHandle = glfwGetWin32Window(window);
params.initialSize = {800, 600};
params.pixelRatio = 1.0f;
params.enableDebugLayer = true;
params.rendererName = "MainRenderer";

renderer->Initialize(params);

// 主循环
while (running) {
    FrameContext ctx{};
    ctx.deltaSeconds = 0.016;  // ~60 FPS
    ctx.clearColor = {0.2f, 0.2f, 0.2f, 1.0f};
    
    renderer->BeginFrame(ctx);
    renderer->Draw(renderList);
    renderer->EndFrame();
}

renderer->Shutdown();
```

---

## RenderHost

**渲染主机** - 管理 UI 元素树的渲染。

### 类定义

```cpp
class RenderHost {
public:
    RenderHost(std::shared_ptr<IRenderer> renderer);
    ~RenderHost();
    
    void SetRoot(ui::UIElement* root);
    ui::UIElement* GetRoot() const;
    
    void Render();
    void InvalidateVisual(ui::UIElement* element);
};
```

### 核心方法

#### SetRoot()

设置根元素

```cpp
void SetRoot(ui::UIElement* root);
```

**参数:**
- `root` - 根 UI 元素指针

**线程安全:** 否

**示例:**
```cpp
auto renderHost = std::make_shared<RenderHost>(renderer);
renderHost->SetRoot(window.get());
```

#### GetRoot()

获取根元素

```cpp
ui::UIElement* GetRoot() const;
```

**返回:** 根元素指针

**线程安全:** 否

#### Render()

渲染当前帧

```cpp
void Render();
```

**线程安全:** 否

**作用:** 遍历视觉树并生成渲染命令

**示例:**
```cpp
// 主循环
while (running) {
    // 处理事件...
    
    renderHost->Render();  // 渲染
}
```

#### InvalidateVisual()

标记元素需要重新渲染

```cpp
void InvalidateVisual(ui::UIElement* element);
```

**参数:**
- `element` - 需要重绘的元素

**线程安全:** 是(内部会切换到渲染线程)

**注意:** 通常不需要手动调用,由 UIElement::InvalidateVisual() 内部调用

---

## RenderCommand

**渲染命令** - 表示单个渲染操作。

### 命令类型

```cpp
enum class RenderCommandType {
    DrawRectangle,      // 绘制矩形
    DrawRoundedRect,    // 绘制圆角矩形
    DrawText,           // 绘制文本
    DrawLine,           // 绘制线条
    SetClipRect,        // 设置裁剪区域
    ClearClipRect       // 清除裁剪区域
};
```

### 类定义

```cpp
class RenderCommand {
public:
    RenderCommandType type;
    
    // 几何参数
    Rect bounds;
    float cornerRadius{0.0f};
    float lineWidth{1.0f};
    
    // 颜色
    Color fillColor;
    Color strokeColor;
    
    // 文本
    std::string text;
    float fontSize{14.0f};
    std::string fontFamily;
    
    // 变换
    float opacity{1.0f};
    bool clipToBounds{false};
};
```

### 使用示例

```cpp
// 通常由渲染系统自动生成
RenderCommand cmd;
cmd.type = RenderCommandType::DrawRoundedRect;
cmd.bounds = Rect{100, 100, 200, 50};
cmd.cornerRadius = 5.0f;
cmd.fillColor = Color{0.3f, 0.7f, 0.3f, 1.0f};
cmd.strokeColor = Color{0.0f, 0.0f, 0.0f, 1.0f};
cmd.lineWidth = 2.0f;
```

---

## RenderList

**渲染列表** - 存储一帧的所有渲染命令。

### 类定义

```cpp
class RenderList {
public:
    void AddCommand(const RenderCommand& cmd);
    void Clear();
    
    const std::vector<RenderCommand>& Commands() const;
    std::size_t Size() const;
    bool IsEmpty() const;
};
```

### 核心方法

#### AddCommand()

添加渲染命令

```cpp
void AddCommand(const RenderCommand& cmd);
```

**参数:**
- `cmd` - 渲染命令

**线程安全:** 否

#### Clear()

清空所有命令

```cpp
void Clear();
```

**线程安全:** 否

**使用场景:** 每帧开始时调用

#### Commands()

获取所有命令

```cpp
const std::vector<RenderCommand>& Commands() const;
```

**返回:** 命令列表的常量引用

**线程安全:** 否

### 使用示例

```cpp
RenderList renderList;

// 构建渲染列表
RenderCommand cmd1{};
cmd1.type = RenderCommandType::DrawRectangle;
cmd1.bounds = {0, 0, 100, 100};
cmd1.fillColor = {1.0f, 0.0f, 0.0f, 1.0f};
renderList.AddCommand(cmd1);

RenderCommand cmd2{};
cmd2.type = RenderCommandType::DrawText;
cmd2.bounds = {10, 10, 80, 30};
cmd2.text = "Hello";
cmd2.fontSize = 16.0f;
renderList.AddCommand(cmd2);

// 渲染
renderer->Draw(renderList);

// 下一帧
renderList.Clear();
```

---

## TextRenderer

**文本渲染器** - 负责文本的光栅化和渲染。

### 类定义

```cpp
class TextRenderer {
public:
    TextRenderer();
    ~TextRenderer();
    
    void Initialize();
    void Shutdown();
    
    void DrawText(const std::string& text,
                 float x, float y,
                 float fontSize,
                 const Color& color,
                 const std::string& fontFamily = "");
    
    Size MeasureText(const std::string& text,
                    float fontSize,
                    const std::string& fontFamily = "");
};
```

### 核心方法

#### Initialize()

初始化文本渲染器

```cpp
void Initialize();
```

**线程安全:** 否

**注意:** 必须在渲染上下文中调用

#### DrawText()

绘制文本

```cpp
void DrawText(const std::string& text,
             float x, float y,
             float fontSize,
             const Color& color,
             const std::string& fontFamily = "");
```

**参数:**
- `text` - 文本内容
- `x`, `y` - 位置
- `fontSize` - 字体大小
- `color` - 文本颜色
- `fontFamily` - 字体族(可选)

**线程安全:** 否

#### MeasureText()

测量文本尺寸

```cpp
Size MeasureText(const std::string& text,
                float fontSize,
                const std::string& fontFamily = "");
```

**参数:**
- `text` - 文本内容
- `fontSize` - 字体大小
- `fontFamily` - 字体族(可选)

**返回:** 文本的边界框大小

**线程安全:** 否

**使用场景:** 布局计算时使用

---

## ColorUtils

**颜色工具** - 颜色转换和处理工具。

### 类定义

```cpp
class ColorUtils {
public:
    // 从十六进制字符串解析颜色
    static Color FromHex(const std::string& hex);
    
    // 颜色转十六进制字符串
    static std::string ToHex(const Color& color);
    
    // 颜色插值
    static Color Lerp(const Color& a, const Color& b, float t);
    
    // 调整亮度
    static Color AdjustBrightness(const Color& color, float factor);
    
    // 调整透明度
    static Color WithAlpha(const Color& color, float alpha);
};
```

### Color 结构

```cpp
struct Color {
    float r{0.0f};  // 红色 (0.0 - 1.0)
    float g{0.0f};  // 绿色 (0.0 - 1.0)
    float b{0.0f};  // 蓝色 (0.0 - 1.0)
    float a{1.0f};  // 透明度 (0.0 - 1.0)
    
    Color() = default;
    Color(float r, float g, float b, float a = 1.0f);
};
```

### 核心方法

#### FromHex()

从十六进制字符串解析颜色

```cpp
static Color FromHex(const std::string& hex);
```

**参数:**
- `hex` - 十六进制颜色字符串
  - `"#RGB"` - 短格式
  - `"#RRGGBB"` - 标准格式
  - `"#RRGGBBAA"` - 带透明度

**返回:** Color 对象

**示例:**
```cpp
auto red = ColorUtils::FromHex("#FF0000");
auto semiTransparent = ColorUtils::FromHex("#FF000080");
auto short = ColorUtils::FromHex("#F00");
```

#### ToHex()

将颜色转换为十六进制字符串

```cpp
static std::string ToHex(const Color& color);
```

**参数:**
- `color` - Color 对象

**返回:** 十六进制字符串 (格式: "#RRGGBBAA")

**示例:**
```cpp
Color color{1.0f, 0.0f, 0.0f, 1.0f};
std::string hex = ColorUtils::ToHex(color);  // "#FF0000FF"
```

#### Lerp()

颜色线性插值

```cpp
static Color Lerp(const Color& a, const Color& b, float t);
```

**参数:**
- `a` - 起始颜色
- `b` - 结束颜色
- `t` - 插值参数 (0.0 - 1.0)

**返回:** 插值后的颜色

**示例:**
```cpp
auto red = Color{1.0f, 0.0f, 0.0f, 1.0f};
auto blue = Color{0.0f, 0.0f, 1.0f, 1.0f};
auto purple = ColorUtils::Lerp(red, blue, 0.5f);  // 紫色
```

#### AdjustBrightness()

调整颜色亮度

```cpp
static Color AdjustBrightness(const Color& color, float factor);
```

**参数:**
- `color` - 原始颜色
- `factor` - 亮度系数 (< 1.0 变暗, > 1.0 变亮)

**返回:** 调整后的颜色

**示例:**
```cpp
auto color = ColorUtils::FromHex("#4CAF50");
auto darker = ColorUtils::AdjustBrightness(color, 0.7f);   // 变暗
auto lighter = ColorUtils::AdjustBrightness(color, 1.3f);  // 变亮
```

#### WithAlpha()

设置颜色的透明度

```cpp
static Color WithAlpha(const Color& color, float alpha);
```

**参数:**
- `color` - 原始颜色
- `alpha` - 新的透明度 (0.0 - 1.0)

**返回:** 设置透明度后的颜色

**示例:**
```cpp
auto color = ColorUtils::FromHex("#FF0000");
auto semiTransparent = ColorUtils::WithAlpha(color, 0.5f);
```

---

## 完整渲染示例

### 基本渲染循环

```cpp
#include "fk/render/GlRenderer.h"
#include "fk/render/RenderHost.h"
#include "fk/ui/Window.h"

int main() {
    // 创建窗口
    auto window = std::make_shared<Window>();
    window->Title("Render Example")->Width(800)->Height(600);
    
    // 创建渲染器
    auto renderer = std::make_shared<GlRenderer>();
    RendererInitParams params{};
    params.nativeSurfaceHandle = window->GetNativeHandle();
    params.initialSize = {800, 600};
    renderer->Initialize(params);
    
    // 创建渲染主机
    auto renderHost = std::make_shared<RenderHost>(renderer);
    window->SetRenderHost(renderHost);
    renderHost->SetRoot(window.get());
    
    // 主循环
    double lastTime = 0.0;
    std::uint64_t frameIndex = 0;
    
    while (window->ProcessEvents()) {
        double currentTime = GetCurrentTime();
        double deltaTime = currentTime - lastTime;
        lastTime = currentTime;
        
        // 开始帧
        FrameContext ctx{};
        ctx.elapsedSeconds = currentTime;
        ctx.deltaSeconds = deltaTime;
        ctx.clearColor = {0.2f, 0.2f, 0.2f, 1.0f};
        ctx.frameIndex = frameIndex++;
        
        renderer->BeginFrame(ctx);
        
        // 渲染
        renderHost->Render();
        
        // 结束帧
        renderer->EndFrame();
    }
    
    // 清理
    renderer->Shutdown();
    
    return 0;
}
```

### 自定义渲染

```cpp
class CustomRenderer : public IRenderer {
public:
    void Draw(const RenderList& list) override {
        for (const auto& cmd : list.Commands()) {
            switch (cmd.type) {
            case RenderCommandType::DrawRectangle:
                DrawRect(cmd.bounds, cmd.fillColor);
                break;
                
            case RenderCommandType::DrawRoundedRect:
                DrawRoundedRect(cmd.bounds, cmd.cornerRadius, 
                              cmd.fillColor, cmd.strokeColor);
                break;
                
            case RenderCommandType::DrawText:
                DrawText(cmd.text, cmd.bounds, cmd.fontSize, 
                        cmd.fillColor);
                break;
                
            // ... 其他命令类型
            }
        }
    }
    
private:
    void DrawRect(const Rect& bounds, const Color& color) {
        // OpenGL 绘制代码...
    }
    
    void DrawRoundedRect(const Rect& bounds, float radius,
                        const Color& fill, const Color& stroke) {
        // OpenGL 绘制代码...
    }
    
    void DrawText(const std::string& text, const Rect& bounds,
                 float fontSize, const Color& color) {
        // 文本渲染代码...
    }
};
```

---

## 线程安全说明

| 类 | 线程安全性 |
|---|---|
| IRenderer | 所有方法必须在渲染线程调用 |
| GlRenderer | 所有方法必须在渲染线程调用 |
| RenderHost | Render() 必须在渲染线程, InvalidateVisual() 线程安全 |
| RenderList | 非线程安全 |
| TextRenderer | 所有方法必须在渲染线程调用 |
| ColorUtils | 所有方法线程安全(静态方法) |

---

## 最佳实践

### 性能优化

1. **批量渲染命令**
   - 合并相同类型的渲染命令
   - 减少状态切换

2. **最小化 InvalidateVisual() 调用**
   - 只在必要时调用
   - 合并多个属性变更

3. **使用对象池**
   - 重用 RenderCommand 对象
   - 减少内存分配

4. **裁剪不可见元素**
   - 实现视锥剔除
   - 不渲染完全透明的元素

### 颜色管理

1. **使用 ColorUtils 进行颜色操作**
   ```cpp
   auto color = ColorUtils::FromHex("#4CAF50");
   auto hover = ColorUtils::AdjustBrightness(color, 1.2f);
   ```

2. **缓存解析后的颜色**
   ```cpp
   // 不推荐 - 每次都解析
   button->SetBackground("#4CAF50");
   
   // 推荐 - 解析一次,重用
   static const Color primaryColor = ColorUtils::FromHex("#4CAF50");
   ```

3. **使用预定义调色板**
   ```cpp
   namespace Colors {
       inline const Color Primary = ColorUtils::FromHex("#4CAF50");
       inline const Color Secondary = ColorUtils::FromHex("#2196F3");
       inline const Color Background = ColorUtils::FromHex("#FAFAFA");
   }
   ```

### 渲染架构

1. **分离渲染逻辑和业务逻辑**
   - UI 元素负责生成渲染命令
   - 渲染器负责执行命令

2. **使用渲染主机管理渲染**
   - 不要直接调用渲染器
   - 通过 RenderHost 协调

3. **正确的初始化顺序**
   ```cpp
   // 1. 创建窗口
   auto window = CreateWindow();
   
   // 2. 创建渲染器
   auto renderer = CreateRenderer();
   renderer->Initialize(params);
   
   // 3. 创建渲染主机
   auto renderHost = CreateRenderHost(renderer);
   
   // 4. 设置根元素
   renderHost->SetRoot(window.get());
   ```
