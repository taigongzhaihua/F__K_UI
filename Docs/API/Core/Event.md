# Event<Args...> - 事件系统

线程安全的事件订阅/发布机制，支持优先级、一次性监听、RAII 连接管理。

## 概述

`Event` 是框架的事件系统核心，提供类型安全的观察者模式实现：

- 类型安全的参数传递
- 线程安全的订阅/取消订阅
- 优先级控制
- 一次性监听（Once）
- RAII 风格的连接管理
- 自动断开连接

## 基本用法

### 简单事件订阅

```cpp
#include <fk/core/Event.h>

// 定义事件
Event<int, std::string> onDataReceived;

// 订阅事件
auto conn = onDataReceived.Subscribe([](int code, const std::string& msg) {
    std::cout << "Received: " << code << " - " << msg << std::endl;
});

// 触发事件
onDataReceived(200, "OK");

// 自动断开（conn 析构时）
```

### 手动断开连接

```cpp
auto conn = myEvent.Subscribe([](int x) {
    std::cout << "Value: " << x << std::endl;
});

// 手动断开
conn.Disconnect();

// 或检查连接状态
if (conn.IsConnected()) {
    conn.Disconnect();
}
```

## 核心类型

### Event::Connection

RAII 风格的连接对象，负责管理订阅生命周期。

```cpp
class Connection {
public:
    void Disconnect();              // 断开连接
    bool IsConnected() const;       // 检查是否仍然连接
    
    // 移动语义（禁止拷贝）
    Connection(Connection&&) noexcept;
    Connection& operator=(Connection&&) noexcept;
};
```

**关键特性：**
- 析构时自动断开连接
- 支持移动，禁止拷贝
- 线程安全的断开操作

## 主要方法

### Subscribe - 订阅事件

```cpp
Connection Subscribe(Handler handler, int priority = 0);
```

订阅事件并返回连接对象。

**参数：**
- `handler` - 事件处理函数 `std::function<void(Args...)>`
- `priority` - 优先级（数值越小优先级越高，默认 0）

**返回：** `Connection` 对象，用于管理订阅

**示例：**

```cpp
Event<std::string> onMessage;

// 基本订阅
auto conn1 = onMessage.Subscribe([](const std::string& msg) {
    std::cout << "Handler 1: " << msg << std::endl;
});

// 高优先级订阅（先执行）
auto conn2 = onMessage.Subscribe([](const std::string& msg) {
    std::cout << "High priority: " << msg << std::endl;
}, -10);

onMessage("Hello");
// 输出顺序：
// High priority: Hello
// Handler 1: Hello
```

### SubscribeOnce - 一次性订阅

```cpp
Connection SubscribeOnce(Handler handler, int priority = 0);
```

订阅事件，但只触发一次后自动断开。

**示例：**

```cpp
Event<int> onReady;

onReady.SubscribeOnce([](int code) {
    std::cout << "Ready (once): " << code << std::endl;
});

onReady(1);  // 输出：Ready (once): 1
onReady(2);  // 不输出（已自动断开）
```

### operator() - 触发事件

```cpp
void operator()(Args... args);
```

触发事件，按优先级顺序调用所有订阅者。

**示例：**

```cpp
Event<int, double> onCalculate;

onCalculate.Subscribe([](int x, double y) {
    std::cout << "Result: " << x * y << std::endl;
});

onCalculate(5, 3.14);  // 输出：Result: 15.7
```

### Clear - 清除所有订阅

```cpp
void Clear();
```

移除所有订阅者，通常用于清理。

```cpp
event.Clear();  // 所有订阅者被移除
```

## 完整示例

### 示例 1：按钮点击事件

```cpp
#include <fk/core/Event.h>

class Button {
public:
    // 点击事件（无参数）
    Event<> onClick;
    
    // 鼠标事件（带坐标）
    Event<int, int> onMouseMove;
    
    void SimulateClick() {
        onClick();  // 触发点击
    }
    
    void SimulateMouseMove(int x, int y) {
        onMouseMove(x, y);
    }
};

int main() {
    Button btn;
    
    // 订阅点击
    auto clickConn = btn.onClick.Subscribe([]() {
        std::cout << "Button clicked!" << std::endl;
    });
    
    // 订阅鼠标移动
    auto moveConn = btn.onMouseMove.Subscribe([](int x, int y) {
        std::cout << "Mouse at (" << x << ", " << y << ")" << std::endl;
    });
    
    btn.SimulateClick();          // 输出：Button clicked!
    btn.SimulateMouseMove(100, 200);  // 输出：Mouse at (100, 200)
    
    // Connection 析构时自动断开
}
```

### 示例 2：数据变化通知

```cpp
class DataModel {
public:
    Event<const std::string&, int> onPropertyChanged;
    
    void SetValue(const std::string& prop, int value) {
        properties_[prop] = value;
        onPropertyChanged(prop, value);  // 通知变化
    }
    
private:
    std::map<std::string, int> properties_;
};

int main() {
    DataModel model;
    
    // 订阅属性变化
    auto conn = model.onPropertyChanged.Subscribe(
        [](const std::string& prop, int value) {
            std::cout << prop << " changed to " << value << std::endl;
        }
    );
    
    model.SetValue("age", 25);     // 输出：age changed to 25
    model.SetValue("score", 100);  // 输出：score changed to 100
}
```

### 示例 3：优先级控制

```cpp
Event<std::string> onLog;

// 高优先级：日志过滤器（先执行）
auto filter = onLog.Subscribe([](const std::string& msg) {
    if (msg.find("ERROR") != std::string::npos) {
        std::cerr << "[FILTERED] " << msg << std::endl;
    }
}, -100);

// 普通优先级：普通日志
auto normal = onLog.Subscribe([](const std::string& msg) {
    std::cout << "[LOG] " << msg << std::endl;
}, 0);

// 低优先级：统计（最后执行）
int count = 0;
auto stats = onLog.Subscribe([&count](const std::string& msg) {
    count++;
    std::cout << "[Total logs: " << count << "]" << std::endl;
}, 100);

onLog("Normal message");
onLog("ERROR: Something wrong");
// 输出顺序：过滤器 -> 普通日志 -> 统计
```

### 示例 4：多个订阅者

```cpp
Event<int> onScoreChanged;

// 多个系统订阅同一事件
auto uiUpdate = onScoreChanged.Subscribe([](int score) {
    std::cout << "[UI] Score: " << score << std::endl;
});

auto achievement = onScoreChanged.Subscribe([](int score) {
    if (score >= 100) {
        std::cout << "[Achievement] High score!" << std::endl;
    }
});

auto analytics = onScoreChanged.Subscribe([](int score) {
    // 发送统计数据
    std::cout << "[Analytics] Tracking score: " << score << std::endl;
});

onScoreChanged(50);   // 三个订阅者都收到通知
onScoreChanged(150);  // 同上，achievement 触发特殊逻辑
```

### 示例 5：RAII 作用域管理

```cpp
Event<std::string> onMessage;

{
    // Connection 作用域内有效
    auto conn = onMessage.Subscribe([](const std::string& msg) {
        std::cout << "Received: " << msg << std::endl;
    });
    
    onMessage("Hello");  // 输出：Received: Hello
    
}  // conn 析构，自动断开

onMessage("World");  // 不输出（订阅已断开）
```

## 线程安全

`Event` 是完全线程安全的：

```cpp
Event<int> sharedEvent;

// 线程 1：订阅
std::thread t1([&]() {
    auto conn = sharedEvent.Subscribe([](int x) {
        std::cout << "Thread 1: " << x << std::endl;
    });
    std::this_thread::sleep_for(std::chrono::seconds(1));
});

// 线程 2：触发
std::thread t2([&]() {
    for (int i = 0; i < 10; ++i) {
        sharedEvent(i);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
});

t1.join();
t2.join();
```

**保证：**
- 订阅/取消订阅是线程安全的
- 事件触发期间添加/删除订阅者是安全的
- 使用 `shared_mutex` 实现读写锁

## 最佳实践

### ✅ 推荐做法

**1. 使用 RAII 管理连接**
```cpp
class MyClass {
    Event<>::Connection conn_;
    
public:
    void Connect(Event<>& event) {
        conn_ = event.Subscribe([this]() {
            OnEvent();
        });
    }
    // 析构时自动断开
};
```

**2. 优先级合理分配**
```cpp
// 过滤器/验证器：-100 ~ -1
// 业务逻辑：0
// 日志/统计：1 ~ 100
event.Subscribe(validator, -10);
event.Subscribe(handler, 0);
event.Subscribe(logger, 10);
```

**3. 避免在事件处理中长时间阻塞**
```cpp
// ❌ 不好：阻塞所有订阅者
event.Subscribe([](int x) {
    HeavyComputation();  // 阻塞
});

// ✅ 好：异步处理
event.Subscribe([](int x) {
    std::async(std::launch::async, [x]() {
        HeavyComputation();
    });
});
```

### ❌ 避免的做法

**1. 在事件处理中触发同一事件（可能死锁）**
```cpp
// ❌ 危险：递归触发
event.Subscribe([&event](int x) {
    if (x > 0) {
        event(x - 1);  // 可能死锁
    }
});
```

**2. 忘记保存 Connection（立即断开）**
```cpp
// ❌ 错误：Connection 临时对象立即析构
myEvent.Subscribe([](int x) { /*...*/ });  // 立即断开！

// ✅ 正确：保存 Connection
auto conn = myEvent.Subscribe([](int x) { /*...*/ });
```

**3. 手动管理生命周期（容易泄漏）**
```cpp
// ❌ 不推荐：手动管理
Event<>::Connection* conn = new Event<>::Connection(/*...*/);
// ... 容易忘记 delete

// ✅ 推荐：自动管理
Event<>::Connection conn = event.Subscribe(/*...*/);
```

## 常见问题

**Q: Event 和 std::function 有什么区别？**

A: `Event` 支持多个订阅者、优先级、自动管理生命周期，而 `std::function` 只能存储单个回调。

**Q: 如何在事件处理中访问 `this`？**

A: 使用 lambda 捕获或 `std::bind`：
```cpp
class MyClass {
    void OnEvent() { /*...*/ }
    
    void Connect(Event<>& event) {
        // 方式 1：lambda
        conn_ = event.Subscribe([this]() { OnEvent(); });
        
        // 方式 2：bind
        conn_ = event.Subscribe(std::bind(&MyClass::OnEvent, this));
    }
    
    Event<>::Connection conn_;
};
```

**Q: 事件触发顺序是什么？**

A: 按优先级从小到大，相同优先级按订阅顺序。

**Q: 如何实现"事件只触发一次"？**

A: 使用 `SubscribeOnce`：
```cpp
event.SubscribeOnce([](int x) {
    std::cout << "Only once: " << x << std::endl;
});
```

**Q: Connection 可以拷贝吗？**

A: 不可以。`Connection` 禁止拷贝，只能移动，避免多次断开连接。

## 性能考虑

- **订阅/取消订阅**：O(n) 时间复杂度（n 为订阅者数量）
- **触发事件**：O(n) 遍历所有订阅者
- **内存开销**：每个订阅者 ~64 字节（函数对象 + 元数据）
- **线程同步**：使用 `shared_mutex`，读操作（触发）可并发

**优化建议：**
- 避免频繁订阅/取消订阅
- 对于高频事件，考虑批量处理
- 合理使用优先级，避免过多订阅者

## 相关类型

- **ICommand** - 命令系统，内部使用 `Event<>` 实现 `CanExecuteChanged`
- **DependencyProperty** - 属性变化通知可以用 `Event` 实现
- **ObservableObject** - 属性变化事件 `PropertyChanged`

## 总结

`Event` 是框架事件系统的核心，提供：

✅ 类型安全的多播事件  
✅ 线程安全的订阅管理  
✅ RAII 风格的连接对象  
✅ 优先级和一次性订阅  
✅ 简洁的 API 设计

适用于 UI 事件、数据绑定、组件通信等各种场景。
