# Timer - 定时器

基于 Dispatcher 的定时器，支持单次/重复执行。

## 概述

`Timer` 是框架的定时器实现，提供：

- 单次或重复触发
- 精确的时间间隔控制
- 基于 Dispatcher 执行（线程安全）
- 简单的 Start/Stop 控制
- Tick 事件通知

## 基本用法

### 创建和启动定时器

```cpp
#include <fk/core/Timer.h>
#include <fk/core/Dispatcher.h>

auto dispatcher = std::make_shared<Dispatcher>();
auto timer = std::make_shared<Timer>(dispatcher);

// 订阅 Tick 事件
timer->Tick.Subscribe([]() {
    std::cout << "Timer tick!" << std::endl;
});

// 启动定时器（1000ms 间隔，重复）
timer->Start(std::chrono::milliseconds(1000), true);

// 停止
timer->Stop();
```

### 单次触发

```cpp
auto timer = std::make_shared<Timer>(dispatcher);

timer->Tick.Subscribe([]() {
    std::cout << "One-time callback" << std::endl;
});

// 单次触发（3 秒后执行一次）
timer->Start(std::chrono::milliseconds(3000), false);
```

## 构造函数

```cpp
explicit Timer(std::shared_ptr<Dispatcher> dispatcher);
```

创建关联到指定 Dispatcher 的定时器。

**参数：**
- `dispatcher` - 定时器任务执行的调度器

**示例：**

```cpp
auto dispatcher = std::make_shared<Dispatcher>();
auto timer = std::make_shared<Timer>(dispatcher);
```

## 主要方法

### Start - 启动定时器

```cpp
void Start(std::chrono::milliseconds interval, bool repeat = true);
```

启动定时器，指定时间间隔和是否重复。

**参数：**
- `interval` - 触发间隔（毫秒）
- `repeat` - 是否重复触发（默认 `true`）

**示例：**

```cpp
// 每秒触发一次
timer->Start(std::chrono::milliseconds(1000), true);

// 5 秒后触发一次
timer->Start(std::chrono::milliseconds(5000), false);
```

### Stop - 停止定时器

```cpp
void Stop();
```

停止定时器，不再触发 Tick 事件。

```cpp
timer->Stop();
```

### IsRunning - 检查运行状态

```cpp
bool IsRunning() const;
```

检查定时器是否正在运行。

**返回：** `true` 表示运行中

**示例：**

```cpp
if (timer->IsRunning()) {
    std::cout << "Timer is active" << std::endl;
    timer->Stop();
}
```

## 事件

### Tick - 定时触发事件

```cpp
core::Event<> Tick;
```

定时器触发时调用的事件（无参数）。

**示例：**

```cpp
int count = 0;
timer->Tick.Subscribe([&count]() {
    count++;
    std::cout << "Tick " << count << std::endl;
    
    if (count >= 10) {
        timer->Stop();  // 10 次后停止
    }
});

timer->Start(std::chrono::milliseconds(500), true);
```

## 完整示例

### 示例 1：简单倒计时

```cpp
#include <fk/core/Timer.h>
#include <fk/core/Dispatcher.h>
#include <iostream>

int main() {
    auto dispatcher = std::make_shared<Dispatcher>();
    
    std::thread dispatcherThread([dispatcher]() {
        dispatcher->Run();
    });
    
    auto timer = std::make_shared<Timer>(dispatcher);
    
    int countdown = 10;
    timer->Tick.Subscribe([&countdown, timer]() {
        std::cout << countdown << "..." << std::endl;
        countdown--;
        
        if (countdown <= 0) {
            std::cout << "Time's up!" << std::endl;
            timer->Stop();
        }
    });
    
    timer->Start(std::chrono::milliseconds(1000), true);
    
    std::this_thread::sleep_for(std::chrono::seconds(11));
    
    dispatcher->Shutdown();
    dispatcherThread.join();
}
```

### 示例 2：UI 动画定时器

```cpp
class AnimatedWindow {
public:
    AnimatedWindow(std::shared_ptr<Dispatcher> dispatcher) {
        timer_ = std::make_shared<Timer>(dispatcher);
        
        timer_->Tick.Subscribe([this]() {
            UpdateAnimation();
        });
    }
    
    void StartAnimation() {
        frame_ = 0;
        timer_->Start(std::chrono::milliseconds(16), true);  // ~60 FPS
    }
    
    void StopAnimation() {
        timer_->Stop();
    }
    
private:
    void UpdateAnimation() {
        frame_++;
        // 更新 UI 元素位置/颜色等
        std::cout << "Frame " << frame_ << std::endl;
        
        if (frame_ >= 120) {  // 2 秒动画
            StopAnimation();
        }
    }
    
    std::shared_ptr<Timer> timer_;
    int frame_{0};
};
```

### 示例 3：多个定时器

```cpp
auto dispatcher = std::make_shared<Dispatcher>();

// 快速定时器（每 100ms）
auto fastTimer = std::make_shared<Timer>(dispatcher);
fastTimer->Tick.Subscribe([]() {
    std::cout << "[Fast] Tick" << std::endl;
});
fastTimer->Start(std::chrono::milliseconds(100), true);

// 慢速定时器（每 1000ms）
auto slowTimer = std::make_shared<Timer>(dispatcher);
slowTimer->Tick.Subscribe([]() {
    std::cout << "[Slow] Tick" << std::endl;
});
slowTimer->Start(std::chrono::milliseconds(1000), true);

// 单次定时器（5 秒后停止所有）
auto stopTimer = std::make_shared<Timer>(dispatcher);
stopTimer->Tick.Subscribe([fastTimer, slowTimer]() {
    std::cout << "Stopping all timers..." << std::endl;
    fastTimer->Stop();
    slowTimer->Stop();
});
stopTimer->Start(std::chrono::milliseconds(5000), false);
```

### 示例 4：自动重启定时器

```cpp
class AutoRestartTimer {
public:
    AutoRestartTimer(std::shared_ptr<Dispatcher> dispatcher) 
        : timer_(std::make_shared<Timer>(dispatcher)) {
        
        timer_->Tick.Subscribe([this]() {
            OnTick();
        });
    }
    
    void Start() {
        timer_->Start(std::chrono::milliseconds(1000), false);  // 单次
    }
    
private:
    void OnTick() {
        std::cout << "Processing..." << std::endl;
        
        // 执行任务
        DoWork();
        
        // 自动重启（实现可变间隔）
        auto nextInterval = CalculateNextInterval();
        timer_->Start(nextInterval, false);
    }
    
    void DoWork() {
        // 实际工作
    }
    
    std::chrono::milliseconds CalculateNextInterval() {
        // 根据负载动态调整间隔
        return std::chrono::milliseconds(500 + rand() % 1000);
    }
    
    std::shared_ptr<Timer> timer_;
};
```

### 示例 5：周期性数据刷新

```cpp
class DataRefresher {
public:
    DataRefresher(std::shared_ptr<Dispatcher> dispatcher) {
        timer_ = std::make_shared<Timer>(dispatcher);
        
        timer_->Tick.Subscribe([this]() {
            RefreshData();
        });
        
        // 每 5 秒刷新一次
        timer_->Start(std::chrono::seconds(5), true);
    }
    
    ~DataRefresher() {
        timer_->Stop();
    }
    
    void Pause() {
        if (timer_->IsRunning()) {
            timer_->Stop();
            std::cout << "Refresh paused" << std::endl;
        }
    }
    
    void Resume() {
        if (!timer_->IsRunning()) {
            timer_->Start(std::chrono::seconds(5), true);
            std::cout << "Refresh resumed" << std::endl;
        }
    }
    
private:
    void RefreshData() {
        std::cout << "Fetching latest data..." << std::endl;
        // 网络请求、数据更新等
    }
    
    std::shared_ptr<Timer> timer_;
};
```

### 示例 6：带状态的定时器

```cpp
class StatefulTimer {
public:
    enum class State {
        Idle,
        Warming,
        Running,
        CoolingDown
    };
    
    StatefulTimer(std::shared_ptr<Dispatcher> dispatcher) 
        : timer_(std::make_shared<Timer>(dispatcher)) {
        
        timer_->Tick.Subscribe([this]() {
            OnTick();
        });
    }
    
    void Start() {
        state_ = State::Warming;
        tickCount_ = 0;
        timer_->Start(std::chrono::milliseconds(100), true);
    }
    
private:
    void OnTick() {
        tickCount_++;
        
        switch (state_) {
            case State::Warming:
                std::cout << "Warming up... " << tickCount_ << std::endl;
                if (tickCount_ >= 10) {  // 1 秒预热
                    state_ = State::Running;
                    tickCount_ = 0;
                }
                break;
                
            case State::Running:
                std::cout << "Running... " << tickCount_ << std::endl;
                if (tickCount_ >= 50) {  // 5 秒运行
                    state_ = State::CoolingDown;
                    tickCount_ = 0;
                }
                break;
                
            case State::CoolingDown:
                std::cout << "Cooling down... " << tickCount_ << std::endl;
                if (tickCount_ >= 10) {  // 1 秒冷却
                    timer_->Stop();
                    state_ = State::Idle;
                }
                break;
                
            default:
                break;
        }
    }
    
    std::shared_ptr<Timer> timer_;
    State state_{State::Idle};
    int tickCount_{0};
};
```

## 线程安全

`Timer` 是线程安全的：

- `Start` / `Stop` / `IsRunning` 可从任何线程调用
- Tick 事件在关联的 `Dispatcher` 线程执行
- 内部使用 `std::mutex` 保护状态

**跨线程示例：**

```cpp
auto timer = std::make_shared<Timer>(uiDispatcher);

// 后台线程控制定时器
std::thread worker([timer]() {
    std::this_thread::sleep_for(std::chrono::seconds(2));
    timer->Start(std::chrono::milliseconds(500), true);
    
    std::this_thread::sleep_for(std::chrono::seconds(5));
    timer->Stop();
});

worker.join();
```

## 最佳实践

### ✅ 推荐做法

**1. 使用智能指针管理生命周期**
```cpp
class MyClass {
    std::shared_ptr<Timer> timer_;
    
public:
    MyClass(std::shared_ptr<Dispatcher> dispatcher) {
        timer_ = std::make_shared<Timer>(dispatcher);
        // 自动管理生命周期
    }
};
```

**2. Stop 前检查 IsRunning**
```cpp
if (timer->IsRunning()) {
    timer->Stop();
}
```

**3. 在析构函数中停止定时器**
```cpp
~MyClass() {
    if (timer_) {
        timer_->Stop();
    }
}
```

**4. 合理选择间隔时间**
```cpp
// ✅ 根据实际需求
timer->Start(std::chrono::milliseconds(16), true);   // 动画：~60 FPS
timer->Start(std::chrono::seconds(1), true);         // 时钟：每秒更新
timer->Start(std::chrono::minutes(5), true);         // 数据刷新：5 分钟
```

### ❌ 避免的做法

**1. 在 Tick 回调中执行长时间操作**
```cpp
// ❌ 阻塞 Dispatcher 线程
timer->Tick.Subscribe([]() {
    HeavyComputation();  // 长时间阻塞
});

// ✅ 异步处理
timer->Tick.Subscribe([dispatcher]() {
    workerDispatcher->Post([]() {
        HeavyComputation();
    });
});
```

**2. 忘记停止定时器（资源泄漏）**
```cpp
// ❌ 定时器一直运行
void CreateTimer() {
    auto timer = std::make_shared<Timer>(dispatcher);
    timer->Start(std::chrono::seconds(1), true);
    // timer 离开作用域，但仍在运行！
}
```

**3. 过短的间隔（性能问题）**
```cpp
// ❌ 过于频繁
timer->Start(std::chrono::milliseconds(1), true);  // 1ms = 1000 Hz

// ✅ 合理间隔
timer->Start(std::chrono::milliseconds(16), true);  // ~60 Hz
```

## 常见问题

**Q: Timer 和 Dispatcher::PostDelayed 有什么区别？**

A: `Timer` 支持重复触发和运行时控制（Start/Stop），`PostDelayed` 是单次延迟执行。

**Q: 定时器精度如何？**

A: 取决于 Dispatcher 的调度精度和系统负载，通常误差在几毫秒内。

**Q: 可以在 Tick 回调中修改定时器吗？**

A: 可以，包括调用 `Stop()` 或重新 `Start()`：
```cpp
timer->Tick.Subscribe([timer]() {
    timer->Stop();  // 停止自己
    // 或
    timer->Start(newInterval, true);  // 修改间隔
});
```

**Q: 如何实现精确的倒计时？**

A: 使用系统时钟而非计数：
```cpp
auto startTime = std::chrono::steady_clock::now();
timer->Tick.Subscribe([startTime]() {
    auto elapsed = std::chrono::steady_clock::now() - startTime;
    auto remaining = std::chrono::seconds(10) - elapsed;
    std::cout << "Remaining: " << remaining.count() << "s" << std::endl;
});
```

**Q: Timer 析构时会自动停止吗？**

A: 是的，析构函数会自动调用 `Stop()`。

## 性能考虑

- **内存开销**：~128 字节（智能指针 + 状态）
- **CPU 开销**：取决于 Tick 回调的复杂度
- **最小间隔**：建议不小于 10ms（取决于平台）

**优化建议：**
- 避免过于频繁的定时器（<10ms）
- Tick 回调保持轻量级
- 考虑合并多个定时器

## 相关类型

- **Dispatcher** - 定时器任务的执行调度器
- **Event** - Tick 事件实现
- **DispatcherOperation** - 内部使用的操作句柄

## 总结

`Timer` 是基于 Dispatcher 的定时器，提供：

✅ 单次/重复触发  
✅ 精确的时间间隔控制  
✅ 线程安全的 Start/Stop  
✅ 简单的 Tick 事件  
✅ 自动生命周期管理

适用于动画、定时刷新、倒计时等场景。
