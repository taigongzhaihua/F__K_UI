# fk::app 模块设计

## 1. 模块职责
- 负责应用程序级生命周期管理（初始化、消息循环、退出）。
- 管理顶层窗口对象、全局资源字典，协调 ui/render/core 等模块。
- 提供应用范围的服务访问点（如调度器、主题、配置）。

## 2. 关键组件

### 2.1 Application
- 单例语义：`Application::Current()` 返回当前实例。
- 生命周期方法：
  - `Initialize(const AppInitParams&)`：配置日志、渲染后端、资源路径等。
  - `Run()`：进入主消息循环，调用 `core::Dispatcher::Run()`。
  - `Shutdown()`：请求退出，触发 `Exit` 事件。
- 窗口管理：
  - `AddWindow(WindowPtr window, std::string name)`
  - `RemoveWindow(...)`
  - `MainWindow()` / `SetMainWindow()`
- 事件：`Startup`, `Exit`, `Activated`, `Deactivated`, `UnhandledException`。
- 全局资源：`ResourceDictionary& Resources()`。

### 2.2 WindowManager
- 持有 `std::unordered_map<std::string, WindowPtr>`。
- 负责窗口激活、切换、关闭流程。
- 管理窗口与底层平台窗口的映射（委托给 render 平台层）。
- 提供 `EnumerateWindows`, `CloseAll()` 等方法，供 Shutdown 阶段清理。

### 2.3 AppServices / ServiceLocator（可选）
- 装载可插拔服务（Logger、ThemeManager、NavigationService 等）。
- 支持注册/解析接口：
  ```cpp
  template <typename T>
  void RegisterService(std::shared_ptr<T> service);

  template <typename T>
  std::shared_ptr<T> GetService() const;
  ```
- 可根据项目需求决定是否引入，初期可简化为包装器。

### 2.4 Navigation（可选扩展）
- `NavigationHost`、`Frame` 控件与 `NavigationService` 协作，管理页面堆栈。
- 暂留接口，后续结合 UI 模块实现。

## 3. 与其他模块的交互
- **core**：
  - 使用 `core::Dispatcher` 作为应用的消息循环。
  - 利用事件系统分发生命周期事件。
- **ui**：
  - `Application` 管理 `Window` 实例（继承自 `fk::ui::Window`）。
  - 通过 `Window::Show()`/`Hide()` 控制顶层视觉树。
- **render**：
  - 初始化渲染后端（`IRenderer`），在创建窗口时注入。
  - 在 Run 循环中驱动渲染帧（调用 `Window::Render()` 或 `RenderHost`）。
- **binding/theme**：
  - 全局资源和主题可通过 `Application::Resources()`、`ThemeManager` 访问。

## 4. API 草图
```cpp
namespace fk::app {

struct AppInitParams {
    std::string appName;
    bool enableHighDpi = true;
    std::filesystem::path assetRoot;
    std::shared_ptr<render::IRenderer> renderer;
    std::shared_ptr<core::Logger> logger;
};

class Application {
public:
    Application();
    virtual ~Application();

    static Application* Current();

    void Initialize(const AppInitParams& params);
    int Run();
    void Shutdown();

    void AddWindow(const ui::WindowPtr& window, std::string name);
    void RemoveWindow(const std::string& name);
    ui::WindowPtr GetWindow(const std::string& name) const;

    void SetMainWindow(const ui::WindowPtr& window);
    ui::WindowPtr MainWindow() const;

    core::Event<> Startup;
    core::Event<> Exit;
    core::Event<> Activated;
    core::Event<> Deactivated;

    ResourceDictionary& Resources();

private:
    void OnWindowClosed(const ui::WindowPtr& window);
    void PumpEvents();

    static Application* instance_;
    std::unique_ptr<WindowManager> windowManager_;
    std::shared_ptr<core::Dispatcher> dispatcher_;
    ui::WindowPtr mainWindow_;
    ResourceDictionary resources_;
    bool running_ = false;
};

} // namespace fk::app
```

## 5. 消息循环设计
1. 调用 `Application::Run()`：
   - 设置 `running_ = true`
   - 触发 `Startup` 事件
   - 进入循环：
     ```cpp
     while (running_) {
         dispatcher_->ProcessEvents();
         windowManager_->RenderAll();
     }
     ```
2. 当所有窗口关闭或 `Shutdown()` 被调用时退出循环，触发 `Exit`。
3. 返回退出码。

## 6. 窗口生命周期
- 创建窗口：
  1. `Application::AddWindow()` 将窗口加入管理器。
  2. 调用 `window->Initialize(render::IRenderer*)` 创建平台句柄。
  3. 调用 `window->Show()`，并订阅 `Closed` 事件。
- 窗口关闭：
  - `Window::Closed` 事件触发 `Application::OnWindowClosed()`。
  - 从 `WindowManager` 中移除；若无窗口，调用 `Shutdown()`。

## 7. 资源与主题
- `Application::Resources()` 返回全局资源字典，与 UI 控件合并。
- 支持加载外部资源字典：
  ```cpp
  resources_.Merge(LoadResourceDictionary("themes/dark.json"));
  ```
- 主题切换可通过发布订阅或监听 ResourceDictionary 变化通知。

## 8. 异常与错误处理
- `UnhandledException` 事件：在消息循环中捕获异常后触发，允许应用层记录日志或显示提示。
- 可集成 `core::Logger` 输出栈信息。

## 9. 测试策略
- 单元测试：
  - 单例行为 (`Current`)。
  - 窗口添加/移除、MainWindow 设置。
  - 生命周期事件触发顺序。
  - Shutdown 条件（所有窗口关闭）。
- 集成测试：
  - 运行最小应用，确保 `Run()` 返回。
  - 多窗口场景，验证渲染和关闭逻辑。

## 10. 实现路线
1. 整理现有 `Application`, `Window` 代码，抽象出窗口管理器。
2. 接入 `core::Dispatcher` 与新渲染管线。
3. 实现生命周期事件与异常处理。
4. 引入资源字典与主题接口。
5. 扩展服务注册/导航等高级功能（可选）。

---
> 该设计为 app 模块提供框架，具体实现应与 ui/render/core 的接口保持同步，尤其是窗口与渲染器之间的依赖关系。