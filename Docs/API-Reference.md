# F__K_UI API 参考文档

> 版本：1.0.0  
> 最后更新：2025-10-25

## 目录

- [核心模块 (fk::core)](#核心模块-fkcore)
- [应用模块 (fk::app)](#应用模块-fkapp)
- [数据绑定 (fk::binding)](#数据绑定-fkbinding)
- [UI 元素 (fk::ui)](#ui-元素-fkui)
- [渲染系统 (fk::render)](#渲染系统-fkrender)

---

## 核心模块 (fk::core)

### Event\<Args...\>

**用途**：轻量级事件系统，支持多播委托

**头文件**：`fk/core/Event.h`

**模板参数**：
- `Args...` - 事件参数类型

**主要方法**：
```cpp
// 订阅事件
void operator+=(std::function<void(Args...)> handler);

// 取消订阅
void operator-=(const std::function<void(Args...)>& handler);

// 触发事件
void operator()(Args... args);

// 清除所有订阅
void Clear();
```

**示例**：
```cpp
Event<int, std::string> myEvent;

// 订阅
myEvent += [](int id, std::string msg) {
    std::cout << "Event: " << id << " - " << msg << std::endl;
};

// 触发
myEvent(42, "Hello");
```

---

### Dispatcher

**用途**：线程调度器，管理UI线程和工作线程任务

**头文件**：`fk/core/Dispatcher.h`

**主要方法**：
```cpp
// 获取当前线程的调度器
static std::shared_ptr<Dispatcher> CurrentDispatcher();

// 检查是否在调度器线程
bool CheckAccess() const;

// 同步调用（阻塞）
void Invoke(std::function<void()> callback, DispatcherPriority priority = Normal);

// 异步调用（非阻塞）
std::shared_ptr<DispatcherOperation> InvokeAsync(
    std::function<void()> callback, 
    DispatcherPriority priority = Normal);

// 处理待处理任务
void ProcessQueue();

// 运行消息循环
void Run();

// 退出消息循环
void ExitAllFrames();
```

**优先级**：
- `DispatcherPriority::Idle` - 空闲时执行
- `DispatcherPriority::Background` - 后台任务
- `DispatcherPriority::Normal` - 普通优先级（默认）
- `DispatcherPriority::Send` - 高优先级，立即执行

**示例**：
```cpp
auto dispatcher = Dispatcher::CurrentDispatcher();

// 在UI线程执行
dispatcher->InvokeAsync([]() {
    // 更新UI
});
```

---

### Timer

**用途**：基于调度器的定时器

**头文件**：`fk/core/Timer.h`

**主要方法**：
```cpp
// 创建定时器
static std::shared_ptr<Timer> Create(
    std::chrono::milliseconds interval,
    std::function<void()> callback,
    std::shared_ptr<Dispatcher> dispatcher = nullptr);

// 启动定时器
void Start();

// 停止定时器
void Stop();

// 检查是否运行
bool IsRunning() const;

// 设置间隔
void SetInterval(std::chrono::milliseconds interval);
```

**示例**：
```cpp
auto timer = Timer::Create(std::chrono::milliseconds(1000), []() {
    std::cout << "Tick!" << std::endl;
});
timer->Start();
```

---

### Clock

**用途**：高精度时钟工具

**头文件**：`fk/core/Clock.h`

**主要方法**：
```cpp
// 获取当前时间（毫秒）
static int64_t Now();

// 获取当前时间（秒）
static double NowSeconds();
```

---

## 应用模块 (fk::app)

### Application

**用途**：应用程序入口，管理窗口和生命周期

**头文件**：`fk/app/Application.h`

**主要方法**：
```cpp
// 获取当前应用实例
static Application* Current();

// 运行应用
void Run();

// 关闭应用
void Shutdown();

// 窗口管理
void AddWindow(const ui::WindowPtr& window, const std::string& name);
void RemoveWindow(const ui::WindowPtr& window);
ui::WindowPtr GetWindow(const std::string& name) const;
const std::unordered_map<std::string, ui::WindowPtr>& Windows() const;

// 检查运行状态
bool IsRunning() const;
```

**事件**：
```cpp
Event<> Startup;      // 应用启动
Event<> Exit;         // 应用退出
Event<> Activated;    // 应用激活
Event<> Deactivated;  // 应用失活
```

**示例**：
```cpp
Application app;

auto window = ui::window()->Title("Main Window");
app.AddWindow(window, "main");

app.Run();
```

---

## 数据绑定 (fk::binding)

### DependencyProperty

**用途**：依赖属性系统核心，支持继承、变更通知、默认值

**头文件**：`fk/binding/DependencyProperty.h`

**注册属性**：
```cpp
// 注册依赖属性
static std::shared_ptr<DependencyProperty> Register(
    std::string_view name,
    std::type_index ownerType,
    std::type_index propertyType,
    std::any defaultValue = {},
    std::shared_ptr<PropertyMetadata> metadata = nullptr);
```

**PropertyMetadata**：
```cpp
PropertyMetadata metadata;
metadata.defaultValue = 100;
metadata.inherits = true;
metadata.propertyChangedCallback = [](DependencyObject* obj, const PropertyChangedEventArgs& e) {
    // 属性变更回调
};
```

**示例**：
```cpp
// 注册属性
inline static auto WidthProperty = DependencyProperty::Register(
    "Width",
    typeid(MyControl),
    typeid(double),
    100.0);

// 使用
obj->SetValue(WidthProperty, 200.0);
double width = std::any_cast<double>(obj->GetValue(WidthProperty));
```

---

### DependencyObject

**用途**：依赖属性宿主，支持属性值存储和绑定

**头文件**：`fk/binding/DependencyObject.h`

**主要方法**：
```cpp
// 设置属性值
void SetValue(std::shared_ptr<DependencyProperty> dp, std::any value);

// 获取属性值
std::any GetValue(std::shared_ptr<DependencyProperty> dp) const;

// 清除属性值
void ClearValue(std::shared_ptr<DependencyProperty> dp);

// 读取本地值
std::any ReadLocalValue(std::shared_ptr<DependencyProperty> dp) const;

// 设置绑定
void SetBinding(std::shared_ptr<DependencyProperty> dp, std::shared_ptr<Binding> binding);

// 获取数据上下文
std::shared_ptr<BindingContext> GetDataContext() const;
void SetDataContext(std::shared_ptr<BindingContext> context);
```

---

### Binding

**用途**：数据绑定配置，支持单向/双向绑定、值转换、相对源

**头文件**：`fk/binding/Binding.h`

**绑定模式**：
```cpp
enum class BindingMode {
    OneWay,        // 源 -> 目标
    TwoWay,        // 源 <-> 目标
    OneTime,       // 仅初始化时
    OneWayToSource // 目标 -> 源
};
```

**主要属性**：
```cpp
std::string Path;                              // 属性路径
std::shared_ptr<std::any> Source;              // 绑定源
std::shared_ptr<RelativeSource> RelativeSource;// 相对源
std::shared_ptr<IValueConverter> Converter;    // 值转换器
BindingMode Mode;                              // 绑定模式
```

**示例**：
```cpp
auto binding = std::make_shared<Binding>();
binding->Path = "Width";
binding->Mode = BindingMode::TwoWay;

element->SetBinding(Element::WidthProperty, binding);
```

---

### INotifyPropertyChanged

**用途**：属性变更通知接口

**头文件**：`fk/binding/INotifyPropertyChanged.h`

**接口**：
```cpp
class INotifyPropertyChanged {
public:
    Event<const std::string&> PropertyChanged;
    virtual ~INotifyPropertyChanged() = default;
};
```

---

### ObservableObject

**用途**：可观察对象基类，自动触发属性变更通知

**头文件**：`fk/binding/ObservableObject.h`

**使用**：
```cpp
class MyModel : public ObservableObject {
    int value_;
public:
    int GetValue() const { return value_; }
    void SetValue(int v) {
        if (value_ != v) {
            value_ = v;
            OnPropertyChanged("Value");
        }
    }
};
```

---

## UI 元素 (fk::ui)

### 类继承层次

```
std::enable_shared_from_this
└── DispatcherObject
    └── DependencyObject (ui)
        └── UIElement (implements Visual)
            └── FrameworkElement
                ├── Panel (abstract)
                │   └── StackPanel
                ├── Control<T> (CRTP)
                │   ├── ContentControl
                │   └── ItemsControl
                └── Decorator
```

---

### UIElement

**用途**：UI元素基类，提供布局、事件、渲染集成

**头文件**：`fk/ui/UIElement.h`

**布局相关**：
```cpp
// 测量期望大小
virtual Size MeasureCore(const Size& availableSize);
void Measure(const Size& availableSize);

// 排列元素
virtual void ArrangeCore(const Rect& finalRect);
void Arrange(const Rect& finalRect);

// 布局信息
Size DesiredSize() const;
Rect LayoutSlot() const;

// 失效布局
void InvalidateMeasure();
void InvalidateArrange();
```

**依赖属性**：
```cpp
static std::shared_ptr<DependencyProperty> VisibilityProperty;
static std::shared_ptr<DependencyProperty> IsEnabledProperty;
static std::shared_ptr<DependencyProperty> OpacityProperty;
```

**Visual 接口实现**：
```cpp
Rect GetRenderBounds() const override;
float GetOpacity() const override;
Visibility GetVisibility() const override;
std::vector<Visual*> GetVisualChildren() const override;
bool HasRenderContent() const override;
```

---

### FrameworkElement

**用途**：框架级元素，添加宽高、边距、对齐等

**头文件**：`fk/ui/FrameworkElement.h`

**依赖属性**：
```cpp
static std::shared_ptr<DependencyProperty> WidthProperty;
static std::shared_ptr<DependencyProperty> HeightProperty;
static std::shared_ptr<DependencyProperty> MinWidthProperty;
static std::shared_ptr<DependencyProperty> MinHeightProperty;
static std::shared_ptr<DependencyProperty> MaxWidthProperty;
static std::shared_ptr<DependencyProperty> MaxHeightProperty;
static std::shared_ptr<DependencyProperty> MarginProperty;
static std::shared_ptr<DependencyProperty> HorizontalAlignmentProperty;
static std::shared_ptr<DependencyProperty> VerticalAlignmentProperty;
static std::shared_ptr<DependencyProperty> DataContextProperty;
```

**主要方法**：
```cpp
// 尺寸约束
double Width() const;
void Width(double value);
double Height() const;
void Height(double value);

// 对齐
HorizontalAlignment GetHorizontalAlignment() const;
void SetHorizontalAlignment(HorizontalAlignment value);

// 边距
Thickness Margin() const;
void Margin(const Thickness& value);
```

---

### Panel

**用途**：容器基类，管理子元素集合

**头文件**：`fk/ui/Panel.h`

**主要方法**：
```cpp
// 子元素管理
void AddChild(std::shared_ptr<UIElement> child);
void RemoveChild(std::shared_ptr<UIElement> child);
void ClearChildren();
const std::vector<std::shared_ptr<UIElement>>& Children() const;

// 布局虚函数
virtual Size MeasureOverride(const Size& availableSize) = 0;
virtual Size ArrangeOverride(const Size& finalSize) = 0;
```

---

### StackPanel

**用途**：堆栈布局面板，水平或垂直排列子元素

**头文件**：`fk/ui/StackPanel.h`

**依赖属性**：
```cpp
static std::shared_ptr<DependencyProperty> OrientationProperty;
```

**主要方法**：
```cpp
// 方向
Orientation GetOrientation() const;
void SetOrientation(Orientation value);
```

**Orientation 枚举**：
```cpp
enum class Orientation {
    Horizontal,
    Vertical
};
```

**示例**：
```cpp
auto panel = std::make_shared<StackPanel>();
panel->SetOrientation(Orientation::Vertical);

panel->AddChild(child1);
panel->AddChild(child2);
```

---

### Control\<T\>

**用途**：控件基类（CRTP模式），提供模板、样式支持

**头文件**：`fk/ui/Control.h`

**主要特性**：
- CRTP 继承模式
- 流式 API（返回 `shared_ptr<Derived>`）

**示例**：
```cpp
class MyControl : public Control<MyControl> {
public:
    auto MyProperty(int value) -> shared_ptr() {
        // 设置属性
        return shared_from_this();
    }
};

// 使用
auto ctrl = MyControl::Create()
    ->Width(100)
    ->Height(50)
    ->MyProperty(42);
```

---

### ContentControl

**用途**：单内容控件，显示一个子元素

**头文件**：`fk/ui/ContentControl.h`

**依赖属性**：
```cpp
static std::shared_ptr<DependencyProperty> ContentProperty;
```

**主要方法**：
```cpp
std::shared_ptr<UIElement> GetContent() const;
void SetContent(std::shared_ptr<UIElement> content);
```

---

### ItemsControl

**用途**：列表控件，显示项集合

**头文件**：`fk/ui/ItemsControl.h`

**依赖属性**：
```cpp
static std::shared_ptr<DependencyProperty> ItemsSourceProperty;
static std::shared_ptr<DependencyProperty> ItemTemplateProperty;
```

---

### Decorator

**用途**：装饰器基类，为单个子元素添加视觉效果

**头文件**：`fk/ui/Decorator.h`

**主要方法**：
```cpp
std::shared_ptr<UIElement> Child() const;
void Child(std::shared_ptr<UIElement> value);
```

---

### Window

**用途**：顶级窗口类，管理 GLFW 窗口、内容、渲染

**头文件**：`fk/ui/Window.h`

**主要方法**：
```cpp
// Getter/Setter 重载（流式API）
Ptr Title(const std::string& t);
std::string Title() const;

Ptr Width(int w);
int Width() const;

Ptr Height(int h);
int Height() const;

bool IsVisible() const;

// 内容管理
void SetContent(std::shared_ptr<UIElement> content);
std::shared_ptr<UIElement> GetContent() const;

// 窗口操作
void Show();
void Hide();
void Close();

// 消息循环
void Run(std::function<bool()> onFrame = nullptr);
bool ProcessEvents();
void RenderFrame();

// 渲染集成
void SetRenderHost(std::shared_ptr<render::RenderHost> renderHost);
std::shared_ptr<render::RenderHost> GetRenderHost() const;

// 本地句柄（尽量避免使用）
GLFWwindow* GetNativeHandle() const;
```

**事件**：
```cpp
Event<> Opened;
Event<> Closed;
Event<int, int> Resized;
Event<> Activated;
Event<> Deactivated;
```

**示例**：
```cpp
auto window = ui::window()
    ->Title("My App")
    ->Width(1024)
    ->Height(768);

window->SetContent(myPanel);

window->Opened += []() {
    std::cout << "Window opened!" << std::endl;
};

window->Show();

// 消息循环（带自定义逻辑）
window->Run([&window]() -> bool {
    if (glfwGetKey(window->GetNativeHandle(), GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        window->Close();
        return false;
    }
    return true;
});
```

---

## 渲染系统 (fk::render)

### Visual

**用途**：可渲染元素接口

**头文件**：`fk/ui/Visual.h`

**纯虚接口**：
```cpp
virtual Rect GetRenderBounds() const = 0;
virtual float GetOpacity() const = 0;
virtual Visibility GetVisibility() const = 0;
virtual std::vector<Visual*> GetVisualChildren() const = 0;
virtual bool HasRenderContent() const = 0;
```

---

### RenderHost

**用途**：渲染主机，协调渲染流程

**头文件**：`fk/render/RenderHost.h`

**配置**：
```cpp
struct RenderHostConfig {
    std::shared_ptr<ui::Window> window;
    std::unique_ptr<IRenderer> renderer;
};
```

**主要方法**：
```cpp
// 构造
RenderHost(RenderHostConfig config);

// 初始化
void Initialize(const RendererInitParams& params);

// 渲染控制
void RequestRender();
void RenderFrame(const FrameContext& frameContext, const ui::Visual& visualRoot);
void Resize(const Extent2D& newSize);
void Shutdown();

// 状态查询
bool IsRenderPending() const;
```

---

### IRenderer

**用途**：渲染器抽象接口

**头文件**：`fk/render/IRenderer.h`

**虚接口**：
```cpp
virtual bool Initialize(const RendererInitParams& params) = 0;
virtual void BeginFrame(const FrameContext& frameContext) = 0;
virtual void Draw(const RenderList& renderList) = 0;
virtual void EndFrame() = 0;
virtual void Resize(const Extent2D& newSize) = 0;
virtual void Shutdown() = 0;
```

**RendererInitParams**：
```cpp
struct RendererInitParams {
    void* nativeSurfaceHandle;  // GLFWwindow* 或其他平台句柄
    Extent2D initialSize;
    float pixelRatio;
    bool enableDebugLayer;
    std::string rendererName;
};
```

**FrameContext**：
```cpp
struct FrameContext {
    double elapsedSeconds;
    double deltaSeconds;
    Color clearColor;
    uint32_t frameIndex;
};
```

---

### GlRenderer

**用途**：OpenGL 3.3 Core 渲染器实现

**头文件**：`fk/render/GlRenderer.h`

**特性**：
- OpenGL 3.3 Core Profile
- 顶点着色器：位置变换到 NDC
- 片段着色器：颜色 + 不透明度混合
- VAO/VBO 管理
- 命令执行器（variant visitor 模式）
- Transform 栈和 Layer 栈

**支持的渲染命令**：
- `SetClip` - 设置裁剪区域
- `SetTransform` - 设置变换偏移
- `DrawRectangle` - 绘制矩形（2个三角形）
- `PushLayer` / `PopLayer` - 图层不透明度
- `DrawText` - 文本渲染（TODO）
- `DrawImage` - 图像渲染（TODO）

---

### RenderCommand

**用途**：渲染命令类型系统

**头文件**：`fk/render/RenderCommand.h`

**命令类型**：
```cpp
enum class CommandType {
    SetClip,
    SetTransform,
    DrawRectangle,
    DrawText,
    DrawImage,
    PushLayer,
    PopLayer
};
```

**Payload 结构**：
```cpp
struct ClipPayload { Rect clipRect; };
struct TransformPayload { float offsetX, offsetY; };
struct RectanglePayload {
    Rect rect;
    Color color;
    float cornerRadius;
};
struct TextPayload {
    Rect bounds;
    std::string text;
    Color foreground;
    std::string fontFamily;
    float fontSize;
};
struct ImagePayload {
    Rect bounds;
    uint32_t textureId;
};
struct LayerPayload { float opacity; };
```

---

### RenderTreeBuilder

**用途**：遍历 Visual 树生成渲染命令

**头文件**：`fk/render/RenderTreeBuilder.h`

**主要方法**：
```cpp
void Rebuild(const Visual& root, RenderScene& scene);
```

**工作流程**：
1. 递归遍历 Visual 树
2. 检查可见性（Collapsed 跳过）
3. 计算有效不透明度（父级累积）
4. 应用 Transform 偏移
5. 为 opacity < 1 的元素 Push/Pop Layer
6. 调用 `GenerateRenderContent` 生成元素渲染命令
7. 递归处理子元素

---

### RenderScene

**用途**：渲染场景，持有命令缓冲区

**头文件**：`fk/render/RenderScene.h`

**主要方法**：
```cpp
RenderCommandBuffer& GetCommandBuffer();
void Reset();
std::unique_ptr<RenderList> GenerateRenderList() const;
```

---

### RenderCommandBuffer

**用途**：渲染命令缓冲区，存储命令序列

**头文件**：`fk/render/RenderCommandBuffer.h`

**主要方法**：
```cpp
void AddCommand(RenderCommand command);
void Clear();
bool IsEmpty() const;
const std::vector<RenderCommand>& GetCommands() const;
size_t GetCommandCount() const;
```

---

### RenderList

**用途**：最终渲染列表，传递给渲染器

**头文件**：`fk/render/RenderList.h`

**主要方法**：
```cpp
void SetCommandBuffer(std::unique_ptr<RenderCommandBuffer> buffer);
const RenderCommandBuffer* CommandBuffer() const;
bool IsEmpty() const;
```

---

## 常用类型和枚举

### Size

```cpp
struct Size {
    float width;
    float height;
};
```

### Rect

```cpp
struct Rect {
    float x, y;
    float width, height;
};
```

### Thickness

```cpp
struct Thickness {
    float left, top, right, bottom;
};
```

### Color

```cpp
struct Color {
    float r, g, b, a;
};
```

### Visibility

```cpp
enum class Visibility {
    Visible,
    Hidden,
    Collapsed
};
```

### HorizontalAlignment

```cpp
enum class HorizontalAlignment {
    Left,
    Center,
    Right,
    Stretch
};
```

### VerticalAlignment

```cpp
enum class VerticalAlignment {
    Top,
    Center,
    Bottom,
    Stretch
};
```

---

## 快速开始示例

### 完整示例：创建窗口和UI

```cpp
#include "fk/ui/Window.h"
#include "fk/ui/StackPanel.h"
#include "fk/ui/ContentControl.h"
#include <GLFW/glfw3.h>

using namespace fk;

int main() {
    try {
        // 1. 创建 UI 树
        auto panel = std::make_shared<ui::StackPanel>();
        panel->SetOrientation(ui::Orientation::Vertical);
        
        for (int i = 0; i < 3; ++i) {
            auto child = std::make_shared<ui::ContentControl>();
            panel->AddChild(child);
        }
        
        // 2. 创建窗口
        auto window = ui::window()
            ->Title("F__K_UI Demo")
            ->Width(800)
            ->Height(600);
        
        window->SetContent(panel);
        
        // 3. 订阅事件
        window->Opened += []() {
            std::cout << "Window opened!" << std::endl;
        };
        
        window->Resized += [](int w, int h) {
            std::cout << "Resized to " << w << "x" << h << std::endl;
        };
        
        // 4. 显示窗口
        window->Show();
        
        // 5. 运行消息循环
        window->Run([&window]() -> bool {
            if (glfwGetKey(window->GetNativeHandle(), GLFW_KEY_ESCAPE) == GLFW_PRESS) {
                window->Close();
                return false;
            }
            return true;
        });
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return -1;
    }
}
```

---

## 架构概览

```
Application
    └── Window
        ├── Content (UIElement树)
        │   ├── StackPanel
        │   │   ├── ContentControl
        │   │   └── ContentControl
        │   └── ...
        │
        └── RenderHost
            ├── RenderTreeBuilder (遍历Visual树)
            ├── RenderScene (生成命令)
            └── GlRenderer (OpenGL执行)
                └── GLFW Window Context
```

### 渲染流程

1. **布局阶段**：
   - `Measure()` - 计算期望大小
   - `Arrange()` - 分配最终位置

2. **渲染构建**：
   - `RenderTreeBuilder` 遍历 Visual 树
   - 生成 `RenderCommand` 序列
   - 存储到 `RenderCommandBuffer`

3. **渲染执行**：
   - `GlRenderer::BeginFrame()`
   - `GlRenderer::Draw()` - 执行命令
   - `GlRenderer::EndFrame()`
   - `glfwSwapBuffers()`

---

## 注意事项

### 线程安全

- **UI元素**：仅限UI线程访问
- **Dispatcher**：跨线程调用使用 `InvokeAsync`
- **数据绑定**：自动处理线程同步

### 内存管理

- 所有UI对象使用 `shared_ptr` 管理
- 避免循环引用（父子关系使用弱引用）
- `Window` 自动管理 GLFW 窗口生命周期

### 性能优化

- 尽量减少布局失效（`InvalidateMeasure/Arrange`）
- 使用 `Visibility::Collapsed` 跳过不可见元素渲染
- 批量更新属性后再触发布局

---

## 未来计划

### 待实现功能

- [ ] 文本渲染（FreeType 集成）
- [ ] 图像渲染（纹理管理）
- [ ] 输入事件（鼠标、键盘）
- [ ] 更多控件（Button, TextBox, Label）
- [ ] 样式和模板系统
- [ ] 动画系统
- [ ] 拖放支持

---

## 版本历史

### v1.0.0 (2025-10-25)

**核心功能**：
- ✅ 完整的依赖属性系统
- ✅ 数据绑定（单向/双向）
- ✅ UI元素基础架构（UIElement, FrameworkElement）
- ✅ 布局系统（Measure/Arrange）
- ✅ 容器控件（Panel, StackPanel）
- ✅ 基础控件（Control, ContentControl, ItemsControl）
- ✅ 窗口管理（Window with GLFW）
- ✅ 渲染系统（RenderHost, GlRenderer）
- ✅ 事件系统
- ✅ 调度器和定时器

---

## 联系方式

- **项目**：F__K_UI
- **仓库**：taigongzhaihua/F__K_UI
- **文档位置**：`Docs/API-Reference.md`
