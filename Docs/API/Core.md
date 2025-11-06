# Core 模块 API 文档

核心基础设施模块，提供线程调度、事件系统、日志和定时器等基础功能。

命名空间: `fk::core`

---

## 目录

- [Dispatcher](#dispatcher) - 线程调度器
- [Event](#event) - 事件系统
- [Logger](#logger) - 日志系统
- [Timer](#timer) - 定时器
- [Clock](#clock) - 时钟系统

---

## Dispatcher

**线程调度器** - 提供消息队列和任务调度功能，支持跨线程调用。

### 类定义

```cpp
class Dispatcher : public std::enable_shared_from_this<Dispatcher> {
public:
    using Task = std::function<void()>;
    
    enum class Priority {
        High = 0,
        Normal = 1,
        Low = 2
    };
    
    explicit Dispatcher(std::string name = {}, 
                       std::unique_ptr<IDispatcherBackend> backend = {});
    ~Dispatcher();
};
```

### 枚举

#### Priority

任务优先级

| 值 | 说明 |
|---|---|
| `High` | 高优先级，优先执行 |
| `Normal` | 普通优先级（默认） |
| `Low` | 低优先级，最后执行 |

### 构造函数

#### Dispatcher()

```cpp
explicit Dispatcher(std::string name = {}, 
                   std::unique_ptr<IDispatcherBackend> backend = {});
```

**参数:**
- `name` - 调度器名称（可选，用于调试）
- `backend` - 后端实现（可选，用于集成到事件循环）

**线程安全:** 可以在任意线程创建

### 核心方法

#### Post()

异步投递任务到调度器队列

```cpp
void Post(Task task, Priority priority = Priority::Normal);
```

**参数:**
- `task` - 要执行的任务函数
- `priority` - 任务优先级（默认为 Normal）

**线程安全:** 是

**示例:**
```cpp
dispatcher->Post([] {
    std::cout << "Task executed" << std::endl;
});

// 高优先级任务
dispatcher->Post([] {
    // Critical task
}, Dispatcher::Priority::High);
```

#### BeginInvoke()

异步投递任务并返回操作句柄

```cpp
DispatcherOperation BeginInvoke(Task task, Priority priority = Priority::Normal);
```

**参数:**
- `task` - 要执行的任务函数
- `priority` - 任务优先级

**返回:** `DispatcherOperation` - 可用于取消或等待任务完成

**线程安全:** 是

**示例:**
```cpp
auto op = dispatcher->BeginInvoke([] {
    // Long running task
});

// 稍后可以取消
op.Cancel();

// 或等待完成
op.Wait();
```

#### PostDelayed()

延迟投递任务

```cpp
DispatcherOperation PostDelayed(Task task, 
                                std::chrono::milliseconds delay,
                                Priority priority = Priority::Normal);
```

**参数:**
- `task` - 要执行的任务函数
- `delay` - 延迟时间
- `priority` - 任务优先级

**返回:** `DispatcherOperation` - 操作句柄

**线程安全:** 是

**示例:**
```cpp
// 1秒后执行
dispatcher->PostDelayed([] {
    std::cout << "Delayed task" << std::endl;
}, std::chrono::milliseconds(1000));
```

#### Send()

同步调用任务（阻塞直到任务执行完成）

```cpp
void Send(Task task);
```

**参数:**
- `task` - 要执行的任务函数

**线程安全:** 是（但从 Dispatcher 线程调用会死锁）

**注意:** 
- 如果在 Dispatcher 线程上调用会抛出异常
- 会阻塞调用线程直到任务完成

**示例:**
```cpp
int result = 0;
dispatcher->Send([&result] {
    result = 42;
});
// result 现在是 42
```

#### Run()

运行调度器消息循环

```cpp
void Run();
```

**线程安全:** 否（必须在创建 Dispatcher 的线程调用）

**注意:** 此方法会阻塞直到调用 `Shutdown()`

**示例:**
```cpp
auto dispatcher = std::make_shared<Dispatcher>("MainDispatcher");

std::thread dispatcherThread([dispatcher] {
    dispatcher->Run();  // 运行消息循环
});

// 投递任务...
dispatcher->Post([] { /* ... */ });

// 关闭
dispatcher->Shutdown();
dispatcherThread.join();
```

#### Shutdown()

关闭调度器，停止消息循环

```cpp
void Shutdown();
```

**线程安全:** 是

**注意:** 调用后 `Run()` 会退出，所有待处理任务会被取消

#### HasThreadAccess()

检查当前线程是否为 Dispatcher 线程

```cpp
bool HasThreadAccess() const;
```

**返回:** 如果当前线程是 Dispatcher 线程返回 true

**线程安全:** 是

**示例:**
```cpp
if (dispatcher->HasThreadAccess()) {
    // 直接执行
    UpdateUI();
} else {
    // 投递到 UI 线程
    dispatcher->Post([this] { UpdateUI(); });
}
```

#### VerifyAccess()

验证当前线程是否为 Dispatcher 线程，否则抛出异常

```cpp
void VerifyAccess() const;
```

**抛出:** `std::runtime_error` - 如果当前线程不是 Dispatcher 线程

**线程安全:** 是

**示例:**
```cpp
void UpdateUI() {
    dispatcher->VerifyAccess();  // 确保在正确的线程
    // UI 更新代码...
}
```

### 属性

#### Name()

获取调度器名称

```cpp
const std::string& Name() const;
```

**返回:** 调度器名称

---

## Event

**类型安全的事件系统** - 支持多播委托、优先级和自动连接管理。

### 类定义

```cpp
template<typename... Args>
class Event {
public:
    using Handler = std::function<void(Args...)>;
    
    class Connection {
    public:
        void Disconnect();
        bool IsConnected() const;
    };
};
```

### 核心方法

#### operator+=

添加事件处理器（推荐方式）

```cpp
void operator+=(const Handler& handler);
```

**参数:**
- `handler` - 事件处理函数

**线程安全:** 是

**示例:**
```cpp
Event<int, std::string> myEvent;

// 推荐: 使用 += 运算符
myEvent += [](int value, const std::string& text) {
    std::cout << "Event: " << value << ", " << text << std::endl;
};
```

#### Add()

添加事件处理器（支持优先级）

```cpp
void Add(const Handler& handler, int priority = 0);
```

**参数:**
- `handler` - 事件处理函数
- `priority` - 优先级（数字越小优先级越高）

**线程安全:** 是

**何时使用:** 只在需要指定优先级时使用，否则推荐使用 `+=` 运算符

**示例:**
```cpp
// 需要优先级时使用 Add()
myEvent.Add([](int value, const std::string& text) {
    std::cout << "High priority handler" << std::endl;
}, -10);  // 高优先级处理器
```

#### AddOnce()

添加一次性事件处理器（触发后自动移除）

```cpp
void AddOnce(const Handler& handler, int priority = 0);
```

**参数:**
- `handler` - 事件处理函数
- `priority` - 优先级

**线程安全:** 是

**示例:**
```cpp
myEvent.AddOnce([](int value, const std::string& text) {
    std::cout << "This will only fire once" << std::endl;
});
```

#### Connect()

添加处理器并返回连接对象

```cpp
Connection Connect(Handler handler, int priority = 0);
```

**参数:**
- `handler` - 事件处理函数
- `priority` - 优先级

**返回:** `Connection` - 可用于断开连接

**线程安全:** 是

**示例:**
```cpp
Event<>::Connection conn = myEvent.Connect([] {
    std::cout << "Connected handler" << std::endl;
});

// 稍后断开
conn.Disconnect();

// 或使用 RAII，Connection 析构时自动断开
{
    auto conn = myEvent.Connect([] { /* ... */ });
    // 离开作用域时自动断开
}
```

#### Invoke()

触发事件，调用所有处理器

```cpp
void Invoke(Args... args);
```

**参数:**
- `args` - 传递给处理器的参数

**线程安全:** 是

**示例:**
```cpp
Event<int, std::string> myEvent;

// 订阅事件
myEvent += [](int n, const std::string& s) {
    std::cout << n << ": " << s << std::endl;
};

// 触发事件
myEvent.Invoke(42, "Hello");  // 输出: 42: Hello
```

**简化语法:**
```cpp
myEvent(42, "Hello");  // 等价于 Invoke()
```

#### Clear()

移除所有事件处理器

```cpp
void Clear();
```

**线程安全:** 是

**示例:**
```cpp
myEvent.Clear();  // 移除所有处理器
```

#### IsEmpty()

检查是否有注册的处理器

```cpp
bool IsEmpty() const;
```

**返回:** 如果没有处理器返回 true

**线程安全:** 是

### Connection 类

事件连接的 RAII 包装器

#### Disconnect()

断开事件连接

```cpp
void Disconnect();
```

**线程安全:** 是

**注意:** 可以多次调用，后续调用无效果

#### IsConnected()

检查连接是否仍然有效

```cpp
bool IsConnected() const;
```

**返回:** 如果连接有效返回 true

**线程安全:** 是

### 使用示例

#### 基本用法

```cpp
#include "fk/core/Event.h"

class Button {
public:
    core::Event<Button&> Click;
    
    void OnClick() {
        Click.Invoke(*this);
    }
};

// 使用 (推荐 += 运算符)
Button btn;
btn.Click += [](Button& sender) {
    std::cout << "Button clicked!" << std::endl;
};
```

#### 带参数的事件

```cpp
core::Event<int, double, std::string> DataReceived;

// 订阅事件
DataReceived += [](int id, double value, const std::string& name) {
    std::cout << "Data: " << id << ", " << value << ", " << name << std::endl;
};

// 触发事件
DataReceived.Invoke(1, 3.14, "pi");
// 或使用简化语法
DataReceived(1, 3.14, "pi");
```

#### 使用 Connection 管理生命周期

```cpp
class EventListener {
    core::Event<>::Connection connection_;
    
public:
    void Subscribe(core::Event<>& event) {
        connection_ = event.Connect([this] {
            OnEvent();
        });
    }
    
    void Unsubscribe() {
        connection_.Disconnect();
    }
    
    void OnEvent() {
        std::cout << "Event received" << std::endl;
    }
    
    ~EventListener() {
        // Connection 自动断开
    }
};
```

#### 优先级示例

```cpp
core::Event<> myEvent;

// 只在需要优先级时使用 Add()
myEvent.Add([] { std::cout << "3rd" << std::endl; }, 10);   // 低优先级
myEvent.Add([] { std::cout << "1st" << std::endl; }, -10);  // 高优先级
myEvent.Add([] { std::cout << "2nd" << std::endl; }, 0);    // 默认优先级

myEvent.Invoke();
// 输出:
// 1st
// 2nd
// 3rd
```

### 最佳实践

1. **优先使用 `+=` 运算符**
   ```cpp
   // ✅ 推荐
   event += [](int value) { /* ... */ };
   
   // ❌ 避免 (除非需要优先级)
   event.Add([](int value) { /* ... */ });
   ```

2. **需要优先级时使用 `Add()`**
   ```cpp
   // ✅ 高优先级处理器
   event.Add([](int value) { /* critical */ }, -10);
   ```

3. **使用 Connection 管理生命周期**
   ```cpp
   // ✅ RAII 方式
   auto conn = event.Connect([](int value) { /* ... */ });
   // 析构时自动断开
   ```

4. **触发事件使用简化语法**
   ```cpp
   // ✅ 推荐
   event(42, "data");
   
   // ✅ 也可以
   event.Invoke(42, "data");
   ```

---

## Logger

**日志系统** - 提供分级日志记录功能。

### 类定义

```cpp
class Logger {
public:
    enum class Level {
        Trace,
        Debug,
        Info,
        Warning,
        Error,
        Critical
    };
    
    static Logger& Instance();
};
```

### 枚举

#### Level

日志级别

| 值 | 说明 |
|---|---|
| `Trace` | 跟踪信息（最详细） |
| `Debug` | 调试信息 |
| `Info` | 普通信息 |
| `Warning` | 警告 |
| `Error` | 错误 |
| `Critical` | 严重错误 |

### 核心方法

#### Instance()

获取全局 Logger 实例

```cpp
static Logger& Instance();
```

**返回:** Logger 单例引用

**线程安全:** 是

#### SetLevel()

设置日志级别（低于此级别的日志不会输出）

```cpp
void SetLevel(Level level);
```

**参数:**
- `level` - 最低日志级别

**线程安全:** 是

#### Log()

记录日志

```cpp
void Log(Level level, const std::string& message);
void Log(Level level, const std::string& category, const std::string& message);
```

**参数:**
- `level` - 日志级别
- `category` - 日志分类（可选）
- `message` - 日志消息

**线程安全:** 是

### 便利宏

```cpp
#define FK_LOG_TRACE(msg)     Logger::Instance().Log(Logger::Level::Trace, msg)
#define FK_LOG_DEBUG(msg)     Logger::Instance().Log(Logger::Level::Debug, msg)
#define FK_LOG_INFO(msg)      Logger::Instance().Log(Logger::Level::Info, msg)
#define FK_LOG_WARNING(msg)   Logger::Instance().Log(Logger::Level::Warning, msg)
#define FK_LOG_ERROR(msg)     Logger::Instance().Log(Logger::Level::Error, msg)
#define FK_LOG_CRITICAL(msg)  Logger::Instance().Log(Logger::Level::Critical, msg)
```

### 使用示例

```cpp
#include "fk/core/Logger.h"

// 设置日志级别
Logger::Instance().SetLevel(Logger::Level::Debug);

// 记录日志
FK_LOG_INFO("Application started");
FK_LOG_DEBUG("Loading configuration");
FK_LOG_WARNING("Configuration file not found, using defaults");
FK_LOG_ERROR("Failed to connect to server");

// 带分类的日志
Logger::Instance().Log(Logger::Level::Info, "Network", "Connected to server");
```

---

## Timer

**定时器** - 提供周期性或一次性的定时任务执行。

### 类定义

```cpp
class Timer {
public:
    using Callback = std::function<void()>;
    
    explicit Timer(std::shared_ptr<Dispatcher> dispatcher);
    ~Timer();
};
```

### 构造函数

```cpp
explicit Timer(std::shared_ptr<Dispatcher> dispatcher);
```

**参数:**
- `dispatcher` - 定时器回调将在此 Dispatcher 上执行

**线程安全:** 可以在任意线程创建

### 核心方法

#### Start()

启动定时器

```cpp
void Start(std::chrono::milliseconds interval, 
           Callback callback, 
           bool repeat = true);
```

**参数:**
- `interval` - 时间间隔
- `callback` - 定时器回调函数
- `repeat` - 是否重复执行（false 表示只执行一次）

**线程安全:** 是

**示例:**
```cpp
auto dispatcher = std::make_shared<Dispatcher>();
Timer timer(dispatcher);

// 每秒执行一次
timer.Start(std::chrono::milliseconds(1000), [] {
    std::cout << "Tick" << std::endl;
}, true);

// 3秒后执行一次
timer.Start(std::chrono::milliseconds(3000), [] {
    std::cout << "One-shot timer" << std::endl;
}, false);
```

#### Stop()

停止定时器

```cpp
void Stop();
```

**线程安全:** 是

#### IsRunning()

检查定时器是否正在运行

```cpp
bool IsRunning() const;
```

**返回:** 如果定时器运行中返回 true

**线程安全:** 是

### 使用示例

```cpp
class AnimationController {
    Timer animationTimer_;
    
public:
    AnimationController(std::shared_ptr<Dispatcher> dispatcher)
        : animationTimer_(dispatcher) {}
    
    void StartAnimation() {
        animationTimer_.Start(
            std::chrono::milliseconds(16),  // ~60 FPS
            [this] { UpdateFrame(); },
            true  // 重复
        );
    }
    
    void StopAnimation() {
        animationTimer_.Stop();
    }
    
    void UpdateFrame() {
        // 更新动画帧
    }
};
```

---

## Clock

**时钟系统** - 提供高精度时间测量。

### 类定义

```cpp
class Clock {
public:
    using TimePoint = std::chrono::steady_clock::time_point;
    using Duration = std::chrono::steady_clock::duration;
};
```

### 核心方法

#### Now()

获取当前时间点

```cpp
static TimePoint Now();
```

**返回:** 当前时间点

**线程安全:** 是

#### Elapsed()

计算经过的时间

```cpp
static Duration Elapsed(TimePoint start);
```

**参数:**
- `start` - 起始时间点

**返回:** 从起始时间到现在经过的时间

**线程安全:** 是

### 使用示例

```cpp
#include "fk/core/Clock.h"

// 性能测量
auto start = Clock::Now();

// 执行一些操作...
DoSomeWork();

auto duration = Clock::Elapsed(start);
auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(duration);
std::cout << "Operation took: " << ms.count() << " ms" << std::endl;
```

---

## 线程安全说明

| 类 | 线程安全性 |
|---|---|
| Dispatcher | 所有方法线程安全，但 `Run()` 必须在 Dispatcher 线程调用 |
| Event | 所有方法线程安全 |
| Logger | 所有方法线程安全 |
| Timer | 所有方法线程安全 |
| Clock | 所有方法线程安全 |

---

## 最佳实践

### Dispatcher

1. **为 UI 线程创建专用 Dispatcher**
   ```cpp
   auto uiDispatcher = std::make_shared<Dispatcher>("UI");
   ```

2. **使用 HasThreadAccess() 避免死锁**
   ```cpp
   void UpdateUI() {
       if (dispatcher->HasThreadAccess()) {
           DoUpdate();
       } else {
           dispatcher->Post([this] { DoUpdate(); });
       }
   }
   ```

3. **延迟任务优于 Timer（简单场景）**
   ```cpp
   dispatcher->PostDelayed([] { /* ... */ }, std::chrono::seconds(1));
   ```

### Event

1. **使用 Connection 管理订阅生命周期**
   ```cpp
   class MyClass {
       Event<>::Connection connection_;
   public:
       ~MyClass() {
           connection_.Disconnect();  // 显式或自动断开
       }
   };
   ```

2. **避免在事件处理器中修改事件订阅**
   ```cpp
   // 不推荐
   myEvent.Add([&myEvent] {
       myEvent.Clear();  // 可能导致迭代器失效
   });
   ```

3. **使用优先级控制执行顺序**
   - 验证器: -100
   - 核心逻辑: 0
   - UI 更新: 100

### Logger

1. **生产环境设置合适的日志级别**
   ```cpp
   #ifdef NDEBUG
       Logger::Instance().SetLevel(Logger::Level::Info);
   #else
       Logger::Instance().SetLevel(Logger::Level::Debug);
   #endif
   ```

2. **使用分类组织日志**
   ```cpp
   Logger::Instance().Log(Level::Info, "Render", "Frame rendered");
   Logger::Instance().Log(Level::Warning, "Network", "Slow connection");
   ```
