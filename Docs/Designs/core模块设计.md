# fk::core 模块设计

## 1. 职责与边界
- 提供框架级通用基础设施：事件机制、调度器、时间源、线程上下文等。
- 不依赖 UI、绑定、渲染等上层模块；允许被其他模块直接引用。
- 致力于线程安全与跨平台实现，封装平台特性（例如工作线程、互斥、条件变量）。

## 2. 关键组件

### 2.1 事件系统
- `Event<T...>`：泛型事件类型，支持任意签名；现有实现作为基线。
- `Event::Connection`：RAII 连接，支持移动语义、`Disconnect()`。
- `EventBus`（可选）：支持基于字符串或类型的全局事件分发，便于跨模块通信。

### 2.2 调度与上下文
- `Dispatcher`
  - 负责维护任务队列、执行循环。
  - 接口：`Post(Action)`, `Send(Action)`（同步执行），`HasPending()`。
  - 持有线程 ID，用于断言 UI 线程访问。
  - 底层使用平台事件源（GLFW、Win32 消息等）唤醒。
- `DispatcherOperation`
  - 表示一个排队任务，可取消、等待完成。
  - 提供状态（Pending/Running/Completed/Canceled）。
- `SynchronizationContext`
  - 与 `Dispatcher` 对应，用于协程或跨线程执行。

### 2.3 时间与动画基础
- `Clock`
  - 提供当前时间戳 (`std::chrono::steady_clock::time_point`)。
  - 支持订阅每帧回调（与渲染循环对齐）。
- `Timer`
  - 基于 `Dispatcher` 的定时器；可单次或重复触发。
  - 接口：`Start(interval)`, `Stop()`, `Tick` 事件。

### 2.4 资源生命周期
- `LifetimeScope`
  - 管理对象生命周期，类似 DI 容器中 scope 概念。
  - 提供 `AddDisposable(IDisposable*)` 用于批量释放。
- `IDisposable`
  - 接口：`Dispose()`，供需要手动释放资源的对象实现。

### 2.5 基础类型与工具
- `Result<T,E>` / `Expected<T,E>`：统一错误处理接口。
- `NonCopyable`, `NonMovable` 辅助结构。
- 日志接口：`Logger`（接口） + 默认实现（可配置输出）。

## 3. 线程模型
- 框架启动时创建主 `Dispatcher`（UI 线程）。
- 其他模块可创建后台 `Dispatcher` 用于 I/O、动画等。
- 为确保线程安全，`DispatcherObject`（在 ui 模块定义）将引用 `core::Dispatcher`，通过 `VerifyAccess()` 检查访问线程。

## 4. API 草图
```cpp
namespace fk::core {

class Dispatcher {
public:
    using Task = std::function<void()>;

    explicit Dispatcher(std::string name);
    ~Dispatcher();

    void Post(Task task);              // 异步入队
    void Send(Task task);              // 同步执行
    DispatcherOperation BeginInvoke(Task task, Priority priority = Priority::Normal);

    bool HasThreadAccess() const;
    void VerifyAccess() const;

    void Run();                        // 消息循环
    void Shutdown();                   // 请求退出

private:
    void ProcessQueue();
};

class DispatcherOperation {
public:
    enum class Status { Pending, Running, Completed, Canceled, Faulted };

    bool Cancel();
    void Wait();
    Status GetStatus() const;
};

class Timer : public std::enable_shared_from_this<Timer> {
public:
    core::Event<> Tick;

    void Start(std::chrono::milliseconds interval, bool repeat = true);
    void Stop();
    bool IsRunning() const;
};

} // namespace fk::core
```

## 5. 平台抽象
- `DispatcherBackend`：策略类，封装平台消息循环（GLFW、Win32）。
- 通过 `std::unique_ptr<IDispatcherBackend>` 注入，实现依赖倒置。
- 单元测试可提供 `TestDispatcherBackend` 模拟任务执行。

## 6. 错误处理与日志
- `core::Logger` 提供接口 `Log(LogLevel level, std::string_view message)`。
- 使用 `Event` 或回调输出到控制台/文件；允许 UI 模块订阅日志变化。
- 对于致命错误，抛出自定义异常 `FrameworkException`。

## 7. 单元测试建议
- 事件系统：连接/断开、嵌套调用、异常传播。
- Dispatcher：Post/Send 顺序、跨线程验证、Shutdown 行为。
- Timer：精度、重复运行、停止后的状态。
- Expected：错误传播、与异常混用场景。

## 8. 依赖与引用
- 引入 `<thread>`, `<mutex>`, `<condition_variable>` 等标准库组件。
- 不依赖第三方库；日志可选集成 `spdlog` 等（作为可插拔适配器）。
- 向上层暴露头文件路径：`include/fk/core/*`。

## 9. 实现路线
1. 整理现有 `Event` 代码至 `fk/core/Event.h` 并补齐单元测试。
2. 完成 `Dispatcher` 基础实现，利用 GLFW `glfwPostEmptyEvent` 或平台特定 API 唤醒。
3. 引入 `Timer`、`Clock`，整合进应用主循环。
4. 提供 `Logger` 接口及默认实现。
5. 针对核心模块补充 GoogleTest/ Catch2 测试工程（可选）。

---
> 本设计文档聚焦 `fk::core` 模块，后续模块设计应在此基础上协调接口与线程策略。