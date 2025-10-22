# fk::core 模块类设计

本文档在整体模块设计的基础上，细化 core 子模块中主要类与接口之间的关系、属性与方法。

## 1. 类关系概览
```
DispatcherObject (定义于 fk::ui，但引用 core::Dispatcher)
    ↑
core::Dispatcher
    ├─ Dispatcher::Task (std::function<void()>)
    ├─ DispatcherOperation
    └─ DispatcherBackend (接口)

core::Timer  ──→ core::Dispatcher

core::Event<T...>
    └─ Event::Connection

core::Logger (接口)
    └─ ConsoleLogger / FileLogger (示例实现)

core::Clock
```

## 2. Dispatcher 系列

### 2.1 Dispatcher
- **职责**：单线程任务队列，驱动 UI 消息循环。
- **成员**：
  - `std::thread::id threadId_`：所属线程。
  - `std::mutex queueMutex_`，`std::condition_variable cv_`：任务同步。
  - `std::deque<Task> pendingTasks_`：等待执行的任务。
  - `std::atomic<bool> running_{false}`：循环状态。
  - `std::unique_ptr<IDispatcherBackend> backend_`：平台唤醒实现。
- **接口**：
  - `Dispatcher(std::string name, std::unique_ptr<IDispatcherBackend>)`。
  - `void Post(Task task)`：异步入队。
  - `void PostDelayed(Task task, std::chrono::milliseconds delay)`：延迟执行。
  - `void Send(Task task)`：若在同一线程立即执行，否则入队并等待完成。
  - `DispatcherOperation BeginInvoke(Task task, Priority priority = Priority::Normal)`。
  - `void Run()`：进入消息循环。
  - `void Shutdown()`：请求退出。
  - `bool HasThreadAccess() const`、`void VerifyAccess() const`。
- **内部逻辑**：
  - `Run()` 中循环 `ProcessQueue()`，直到 `running_ == false`。
  - `ProcessQueue()`：取出当前批次任务执行；若有延迟任务，使用最小堆管理。
  - 通过 `backend_->NotifyWork()` 唤醒消息源（如 `glfwPostEmptyEvent`）。

### 2.2 DispatcherOperation
- **职责**：表示异步任务句柄，可用于取消或等待完成。
- **成员**：
  - `Dispatcher* dispatcher_`。
  - `std::shared_ptr<OperationState> state_`，其中包含 `Status` 与 `std::promise<void>`。
- **接口**：
  - `bool Cancel()`：标记任务已取消，若未开始执行则从队列移除。
  - `void Wait()` / `bool WaitFor(duration)`：等待任务结束。
  - `Status GetStatus() const`：返回 `Pending/Running/Completed/Canceled/Faulted`。

### 2.3 IDispatcherBackend
- **目的**：封装平台相关唤醒与事件处理。
- **接口示例**：
  ```cpp
  class IDispatcherBackend {
  public:
      virtual ~IDispatcherBackend() = default;
      virtual void NotifyWorkPending() = 0;
      virtual void WaitForEvents(std::chrono::milliseconds timeout) = 0;
  };
  ```
- **实现**：
  - `GlfwDispatcherBackend`：调用 `glfwPostEmptyEvent` 唤醒，`glfwWaitEventsTimeout` 等待。
  - `Win32MessageLoopBackend`（未来可选）。

## 3. Timer
- **职责**：基于 Dispatcher 执行的定时器。
- **成员**：
  - `core::Event<> Tick`：定时到期事件。
  - `std::shared_ptr<DispatcherOperation> operation_`：当前排程任务。
  - `std::chrono::milliseconds interval_`，`bool repeat_`。
  - `std::weak_ptr<Dispatcher> dispatcher_`。
- **接口**：
  - `void Start(std::chrono::milliseconds interval, bool repeat = true)`。
  - `void Stop()`。
  - `bool IsRunning() const`。
- **逻辑**：使用 `Dispatcher::PostDelayed()` 安排下次 Tick，执行后若 `repeat_` 为 true 则重新排程。

## 4. Event
- 已有实现为模板类，核心结构：
  - `std::shared_ptr<std::vector<std::pair<std::size_t, Handler>>> listeners_`。
  - `std::size_t nextId_` 作为连接 ID。
- `Connection` 提供 RAII 断开。
- **增强点**：
  - 用 `std::shared_mutex` 支持多线程读写（可选）。
  - 为 `Connect` 支持优先级或一次性事件（`ConnectOnce`）。

## 5. Clock
- **职责**：提供框架统一的时间源（steady clock）。
- **接口**：
  - `static Clock& Instance()`。
  - `std::chrono::steady_clock::time_point Now() const`。
  - `double GetDeltaSeconds()`：计算帧间隔，可与渲染帧同步。
- **实现**：内部维护上一帧时间戳，供动画/Timer 使用。

## 6. Logger
- **抽象接口**：
  ```cpp
  class Logger {
  public:
      enum class Level { Trace, Debug, Info, Warn, Error, Fatal };
      virtual ~Logger() = default;
      virtual void Log(Level level, std::string_view message) = 0;
  };
  ```
- **默认实现**：
  - `ConsoleLogger`：使用 `std::cout` 输出，支持时间戳。
  - `NullLogger`：不输出。
- **互操作**：Application 可注册全局 `Logger`，其他模块通过 `core::Logger::Current()` 访问。

## 7. Result / Expected
- 基于 `std::expected<T, Error>` (C++23) 或自定义。
- 用于 API 返回值，避免异常。`Error` 可包含错误码、消息。

## 8. 辅助类型
- `NonCopyable`, `NonMovable`：防止对象被复制。
- `ScopeGuard`：简单的 RAII 退出动作封装。
- `ThreadId`：包装 `std::thread::id`，提供比较/打印。

## 9. 与其他模块的接口
- `fk::ui::DispatcherObject` 将持有 `std::shared_ptr<core::Dispatcher>`。
- `fk::app::Application` 在初始化时创建主线程 Dispatcher，并暴露给 UI。
- `fk::binding` 使用 `Event` 和 `Result`，但不直接依赖 Dispatcher。

## 10. UML 粗略示意
```
+---------------------+      +----------------------+      +----------------------+
|      Dispatcher     |<>----|  IDispatcherBackend  |      |    DispatcherOperation |
|---------------------|      |----------------------|      |----------------------|
| + Post(task)        |      | + NotifyWorkPending()|      | + Cancel()            |
| + Send(task)        |      | + WaitForEvents()    |      | + Wait()              |
| + Run()             |      +----------------------+      +----------------------+
| + Shutdown()        |
+---------------------+
       ^
       |
+---------------------+
|       Timer         |
|---------------------|
| + Start(interval)   |
| + Stop()            |
| + Tick (Event<>)    |
+---------------------+
```

## 11. 实现优先级建议
1. Dispatcher + Backend（GLFW）
2. Timer + 基本测试
3. Logger + Clock
4. Event 增强（若需要）

---
> 以上设计为核心模块的类级细节，可在实现前进一步转化为头文件草稿。