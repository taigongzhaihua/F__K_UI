# Dispatcher - 任务调度器

线程安全的任务调度器，支持优先级队列、延迟执行、同步/异步调用。

## 概述

`Dispatcher` 是框架的任务调度核心，负责：

- 任务队列管理（优先级队列）
- 延迟任务调度（定时器）
- 线程访问检查
- 同步/异步任务执行
- 操作取消和状态跟踪

典型用于 UI 线程调度、后台任务协调等场景。

## 基本用法

### 创建 Dispatcher

```cpp
#include <fk/core/Dispatcher.h>

// 创建命名的 Dispatcher
auto dispatcher = std::make_shared<Dispatcher>("UI Thread");

// 在后台线程运行消息循环
std::thread dispatcherThread([dispatcher]() {
    dispatcher->Run();  // 阻塞，直到 Shutdown
});
```

### Post - 异步执行

```cpp
// 投递任务到 Dispatcher 队列
dispatcher->Post([]() {
    std::cout << "Task executed on dispatcher thread" << std::endl;
});

// 带优先级
dispatcher->Post([]() {
    std::cout << "High priority task" << std::endl;
}, Dispatcher::Priority::High);
```

### Send - 同步执行

```cpp
// 在 Dispatcher 线程同步执行任务（阻塞调用线程）
dispatcher->Send([]() {
    std::cout << "Synchronous task" << std::endl;
});
// 任务执行完成后才返回
```

## 核心类型

### Priority - 优先级

```cpp
enum class Priority {
    High = 0,     // 高优先级
    Normal = 1,   // 普通优先级（默认）
    Low = 2       // 低优先级
};
```

任务按优先级顺序执行：High → Normal → Low。

### DispatcherOperation - 操作句柄

```cpp
class DispatcherOperation {
public:
    enum class Status {
        Pending,    // 等待执行
        Running,    // 正在执行
        Completed,  // 已完成
        Canceled,   // 已取消
        Faulted     // 执行出错
    };
    
    bool Cancel();              // 取消操作
    void Wait() const;          // 等待完成
    Status GetStatus() const;   // 获取状态
    std::exception_ptr GetException() const;  // 获取异常
};
```

## 主要方法

### Post - 异步投递

```cpp
void Post(Task task, Priority priority = Priority::Normal);
```

将任务投递到队列，立即返回。

**参数：**
- `task` - 任务函数 `std::function<void()>`
- `priority` - 优先级（默认 `Normal`）

**示例：**

```cpp
dispatcher->Post([]() {
    std::cout << "Background task" << std::endl;
});

// 高优先级任务
dispatcher->Post([]() {
    UpdateUI();
}, Dispatcher::Priority::High);
```

### BeginInvoke - 异步执行（带操作句柄）

```cpp
DispatcherOperation BeginInvoke(Task task, Priority priority = Priority::Normal);
```

异步执行任务，返回操作句柄用于跟踪状态。

**返回：** `DispatcherOperation` 对象

**示例：**

```cpp
auto op = dispatcher->BeginInvoke([]() {
    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::cout << "Long task done" << std::endl;
});

// 可以取消
if (needCancel) {
    op.Cancel();
}

// 或等待完成
op.Wait();
std::cout << "Status: " << static_cast<int>(op.GetStatus()) << std::endl;
```

### PostDelayed - 延迟执行

```cpp
DispatcherOperation PostDelayed(
    Task task, 
    std::chrono::milliseconds delay, 
    Priority priority = Priority::Normal
);
```

延迟指定时间后执行任务。

**参数：**
- `task` - 任务函数
- `delay` - 延迟时间
- `priority` - 优先级

**示例：**

```cpp
// 1 秒后执行
auto op = dispatcher->PostDelayed([]() {
    std::cout << "Delayed task" << std::endl;
}, std::chrono::milliseconds(1000));

// 可以在执行前取消
op.Cancel();
```

### Send - 同步执行

```cpp
void Send(Task task);
```

在 Dispatcher 线程同步执行任务，阻塞调用线程直到完成。

**注意：** 如果在 Dispatcher 线程内调用 `Send`，任务会立即执行。

**示例：**

```cpp
int result = 0;
dispatcher->Send([&result]() {
    result = HeavyComputation();
});
// 此处 result 已经计算完成
std::cout << "Result: " << result << std::endl;
```

### Run - 运行消息循环

```cpp
void Run();
```

启动 Dispatcher 消息循环，阻塞当前线程直到 `Shutdown()` 被调用。

**示例：**

```cpp
auto dispatcher = std::make_shared<Dispatcher>("Worker");

std::thread worker([dispatcher]() {
    dispatcher->Run();  // 消息循环
});

// 投递任务...

dispatcher->Shutdown();  // 停止循环
worker.join();
```

### Shutdown - 停止调度器

```cpp
void Shutdown();
```

停止消息循环，取消所有待执行任务。

### HasThreadAccess / VerifyAccess

```cpp
bool HasThreadAccess() const;      // 检查是否在 Dispatcher 线程
void VerifyAccess() const;         // 断言在 Dispatcher 线程（否则抛异常）
```

**示例：**

```cpp
if (dispatcher->HasThreadAccess()) {
    // 直接执行
    UpdateUI();
} else {
    // 投递到 Dispatcher 线程
    dispatcher->Post([this]() { UpdateUI(); });
}

// 或断言检查
dispatcher->VerifyAccess();  // 不在正确线程会抛异常
```

## 完整示例

### 示例 1：UI 线程调度器

```cpp
#include <fk/core/Dispatcher.h>

class Application {
public:
    Application() : uiDispatcher_(std::make_shared<Dispatcher>("UI Thread")) {
        uiThread_ = std::thread([this]() {
            uiDispatcher_->Run();
        });
    }
    
    ~Application() {
        uiDispatcher_->Shutdown();
        if (uiThread_.joinable()) {
            uiThread_.join();
        }
    }
    
    void UpdateUIFromBackground(const std::string& data) {
        // 从后台线程更新 UI
        uiDispatcher_->Post([data]() {
            std::cout << "[UI Thread] Data: " << data << std::endl;
        });
    }
    
private:
    std::shared_ptr<Dispatcher> uiDispatcher_;
    std::thread uiThread_;
};

int main() {
    Application app;
    
    // 后台线程
    std::thread worker([&app]() {
        for (int i = 0; i < 5; ++i) {
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            app.UpdateUIFromBackground("Data " + std::to_string(i));
        }
    });
    
    worker.join();
    std::this_thread::sleep_for(std::chrono::seconds(1));
}
```

### 示例 2：优先级队列

```cpp
auto dispatcher = std::make_shared<Dispatcher>();

std::thread worker([dispatcher]() {
    dispatcher->Run();
});

// 投递不同优先级的任务
dispatcher->Post([]() {
    std::cout << "Low priority task" << std::endl;
}, Dispatcher::Priority::Low);

dispatcher->Post([]() {
    std::cout << "Normal priority task" << std::endl;
}, Dispatcher::Priority::Normal);

dispatcher->Post([]() {
    std::cout << "High priority task" << std::endl;
}, Dispatcher::Priority::High);

// 输出顺序：High → Normal → Low

std::this_thread::sleep_for(std::chrono::milliseconds(100));
dispatcher->Shutdown();
worker.join();
```

### 示例 3：延迟任务和取消

```cpp
auto dispatcher = std::make_shared<Dispatcher>();

std::thread worker([dispatcher]() {
    dispatcher->Run();
});

// 3 秒后执行
auto op = dispatcher->PostDelayed([]() {
    std::cout << "This should not print" << std::endl;
}, std::chrono::milliseconds(3000));

// 1 秒后取消
std::this_thread::sleep_for(std::chrono::seconds(1));
if (op.Cancel()) {
    std::cout << "Task canceled successfully" << std::endl;
}

std::this_thread::sleep_for(std::chrono::seconds(3));
dispatcher->Shutdown();
worker.join();
```

### 示例 4：同步等待结果

```cpp
auto dispatcher = std::make_shared<Dispatcher>();

std::thread worker([dispatcher]() {
    dispatcher->Run();
});

// 异步计算
auto op = dispatcher->BeginInvoke([]() {
    std::this_thread::sleep_for(std::chrono::seconds(2));
    std::cout << "Computation done" << std::endl;
});

std::cout << "Waiting for result..." << std::endl;
op.Wait();  // 阻塞直到完成
std::cout << "Operation status: " << static_cast<int>(op.GetStatus()) << std::endl;

dispatcher->Shutdown();
worker.join();
```

### 示例 5：线程访问检查

```cpp
class DataModel {
public:
    DataModel(std::shared_ptr<Dispatcher> dispatcher) 
        : dispatcher_(dispatcher) {}
    
    void SetValue(int value) {
        if (dispatcher_->HasThreadAccess()) {
            // 已在正确线程
            value_ = value;
            NotifyChanged();
        } else {
            // 切换到正确线程
            dispatcher_->Post([this, value]() {
                value_ = value;
                NotifyChanged();
            });
        }
    }
    
    int GetValue() const {
        dispatcher_->VerifyAccess();  // 断言检查
        return value_;
    }
    
private:
    void NotifyChanged() {
        dispatcher_->VerifyAccess();
        std::cout << "Value changed to: " << value_ << std::endl;
    }
    
    std::shared_ptr<Dispatcher> dispatcher_;
    int value_{0};
};
```

### 示例 6：多线程协调

```cpp
auto mainDispatcher = std::make_shared<Dispatcher>("Main");
auto workerDispatcher = std::make_shared<Dispatcher>("Worker");

std::thread mainThread([mainDispatcher]() {
    mainDispatcher->Run();
});

std::thread workerThread([workerDispatcher]() {
    workerDispatcher->Run();
});

// 主线程投递任务到工作线程
mainDispatcher->Post([workerDispatcher]() {
    std::cout << "[Main] Posting to worker..." << std::endl;
    
    workerDispatcher->Post([]() {
        std::cout << "[Worker] Heavy computation" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
    });
});

std::this_thread::sleep_for(std::chrono::seconds(2));

mainDispatcher->Shutdown();
workerDispatcher->Shutdown();
mainThread.join();
workerThread.join();
```

## 线程安全

`Dispatcher` 的所有公共方法都是线程安全的：

- `Post` / `BeginInvoke` / `PostDelayed` 可从任何线程调用
- `Send` 会在调用线程阻塞，目标线程执行
- `Shutdown` 可从任何线程安全调用

**内部同步机制：**
- 使用 `std::mutex` 保护任务队列
- 使用 `std::condition_variable` 实现等待/唤醒

## 最佳实践

### ✅ 推荐做法

**1. 为 UI 线程创建专用 Dispatcher**
```cpp
class UIApplication {
    std::shared_ptr<Dispatcher> uiDispatcher_;
    
public:
    UIApplication() 
        : uiDispatcher_(std::make_shared<Dispatcher>("UI")) {
        uiThread_ = std::thread([this]() {
            uiDispatcher_->Run();
        });
    }
};
```

**2. 使用 HasThreadAccess 避免不必要的投递**
```cpp
void UpdateData(int value) {
    if (dispatcher_->HasThreadAccess()) {
        data_ = value;  // 直接更新
    } else {
        dispatcher_->Post([this, value]() {
            data_ = value;  // 投递更新
        });
    }
}
```

**3. 长时间操作使用后台 Dispatcher**
```cpp
// UI Dispatcher
uiDispatcher->Post([workerDispatcher]() {
    // 投递到后台
    workerDispatcher->Post([]() {
        HeavyComputation();  // 不阻塞 UI
    });
});
```

**4. 使用 DispatcherOperation 跟踪异步操作**
```cpp
auto op = dispatcher->BeginInvoke([]() {
    LongTask();
});

// 可以在其他地方取消或等待
if (needCancel) {
    op.Cancel();
}
```

### ❌ 避免的做法

**1. 在 Dispatcher 线程内调用 Send（死锁）**
```cpp
// ❌ 死锁！
dispatcher->Post([dispatcher]() {
    dispatcher->Send([]() {  // 等待自己完成
        // ...
    });
});
```

**2. 忘记调用 Shutdown**
```cpp
// ❌ 线程永远不会退出
auto dispatcher = std::make_shared<Dispatcher>();
std::thread t([dispatcher]() {
    dispatcher->Run();  // 无限循环
});
// ... 忘记调用 dispatcher->Shutdown()
```

**3. 在任务中抛出未捕获异常**
```cpp
// ❌ 异常会终止 Dispatcher
dispatcher->Post([]() {
    throw std::runtime_error("Error");  // 未捕获
});

// ✅ 正确：捕获异常
dispatcher->Post([]() {
    try {
        MayThrow();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
});
```

## 常见问题

**Q: Dispatcher 和 std::thread 有什么区别？**

A: `Dispatcher` 提供任务队列、优先级、延迟执行等高级功能，而 `std::thread` 只是执行单个函数。

**Q: 如何实现定时器？**

A: 使用 `PostDelayed` 递归投递：
```cpp
void StartTimer() {
    dispatcher->PostDelayed([this]() {
        OnTimer();
        StartTimer();  // 递归
    }, std::chrono::milliseconds(1000));
}
```

**Q: Post 和 BeginInvoke 有什么区别？**

A: `Post` 无返回值，`BeginInvoke` 返回 `DispatcherOperation` 用于跟踪状态和取消。

**Q: Dispatcher 线程何时退出？**

A: 调用 `Shutdown()` 后，`Run()` 会退出。

**Q: 如何从 Dispatcher 线程获取返回值？**

A: 使用 `Send` 或 `std::future`：
```cpp
// 方式 1：Send（同步）
int result = 0;
dispatcher->Send([&result]() {
    result = Compute();
});

// 方式 2：future（异步）
std::promise<int> promise;
auto future = promise.get_future();
dispatcher->Post([&promise]() {
    promise.set_value(Compute());
});
int result = future.get();
```

## 性能考虑

- **任务投递**：O(log n) 插入优先级队列（n 为待执行任务数）
- **任务执行**：O(log n) 从队列取出
- **内存开销**：每个任务 ~128 字节（函数对象 + 状态）
- **线程同步**：mutex + condition_variable

**优化建议：**
- 批量投递任务时使用相同优先级
- 避免投递大量短时任务（合并处理）
- 长时间操作使用独立的后台 Dispatcher

## 相关类型

- **Timer** - 基于 Dispatcher 实现的定时器
- **Window** - 通常关联一个 UI Dispatcher
- **IDispatcherBackend** - 自定义事件循环后端（如 Win32 消息循环）

## 总结

`Dispatcher` 是框架的任务调度核心，提供：

✅ 优先级任务队列  
✅ 延迟/定时执行  
✅ 同步/异步调用  
✅ 线程访问检查  
✅ 操作取消和状态跟踪

适用于 UI 线程管理、后台任务协调、定时器等场景。
