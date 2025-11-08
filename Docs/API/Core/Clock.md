# Clock - 时钟系统

## 概述

`Clock` 类提供框架级别的高精度时间测量功能，用于实现帧时间计算、动画和性能监测等需要精确时间的功能。

## 核心 API

### 单例获取

```cpp
static Clock& Instance();
```

获取全局唯一的时钟实例。

**示例：**
```cpp
auto& clock = fk::core::Clock::Instance();
```

### 获取当前时间点

```cpp
TimePoint Now() const;
```

获取当前的高精度时间点（基于 `std::chrono::steady_clock`）。

**特点：**
- 返回类型：`std::chrono::steady_clock::time_point`
- 线程安全：使用互斥锁保护
- 精度：与系统时钟相同（通常为纳秒级）

**示例：**
```cpp
auto& clock = fk::core::Clock::Instance();
auto t1 = clock.Now();
// ... 执行某些操作
auto t2 = clock.Now();
auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);
std::cout << "耗时: " << duration.count() << " ms" << std::endl;
```

### 获取帧时间差

```cpp
double GetDeltaSeconds();
```

获取自上次调用此方法以来的时间差（单位：秒），常用于计算帧时间（delta time）。

**特点：**
- 返回值：浮点数，单位为秒
- 首次调用返回 0
- 后续调用返回距离上一次调用的时间差
- 线程安全

**示例：**
```cpp
// 游戏主循环中
while (running) {
    double deltaTime = fk::core::Clock::Instance().GetDeltaSeconds();
    
    // 更新游戏逻辑
    Update(deltaTime);
    
    // 更新动画
    for (auto& animation : animations) {
        animation.Update(deltaTime);
    }
}
```

### 重置时钟

```cpp
void Reset();
```

重置时钟的参考时间点，下次调用 `GetDeltaSeconds()` 时将基于新的参考点计算。

**使用场景：**
- 暂停/恢复时需要重置
- 场景切换时可以重置
- 性能分析的特定时间段

**示例：**
```cpp
// 暂停游戏
bool paused = false;

void TogglePause() {
    if (!paused) {
        // 进入暂停状态
        paused = true;
    } else {
        // 退出暂停状态，重置时钟避免累积大的 deltaTime
        fk::core::Clock::Instance().Reset();
        paused = false;
    }
}
```

## 类型定义

```cpp
using TimePoint = std::chrono::steady_clock::time_point;
using Duration = std::chrono::steady_clock::duration;
```

- `TimePoint`: 时间点类型，表示某一时刻
- `Duration`: 时间段类型，表示两个时间点之间的差

## 实现细节

### 线程安全性

Clock 使用 `std::mutex` 保护内部状态，所有公开方法都是线程安全的。

```cpp
private:
    mutable std::mutex mutex_;
    TimePoint lastTime_;
```

### 为什么使用 steady_clock？

`steady_clock` 不受系统时钟调整影响，适合用于：
- 测量相对时间（而非绝对时间）
- 计算经过时间
- 实现超时机制

## 常见模式

### 1. 帧率独立的游戏循环

```cpp
class Game {
private:
    fk::core::Clock& clock_ = fk::core::Clock::Instance();
    
public:
    void Run() {
        while (IsRunning()) {
            double deltaTime = clock_.GetDeltaSeconds();
            
            Update(deltaTime);  // 参数化的游戏逻辑更新
            Render();
        }
    }
    
    void Update(double deltaTime) {
        // 所有运动计算都乘以 deltaTime
        position_ += velocity_ * deltaTime;
        rotation_ += angularVelocity_ * deltaTime;
    }
};
```

### 2. 性能分析

```cpp
void AnalyzePerformance() {
    auto& clock = fk::core::Clock::Instance();
    
    // 分析渲染性能
    clock.Reset();
    RenderFrame();
    double renderTime = clock.GetDeltaSeconds();
    
    // 分析更新性能
    clock.Reset();
    UpdateLogic();
    double logicTime = clock.GetDeltaSeconds();
    
    std::cout << "Render: " << renderTime * 1000 << " ms\n";
    std::cout << "Logic: " << logicTime * 1000 << " ms\n";
}
```

### 3. 动画系统集成

```cpp
class Animation {
private:
    float currentTime_ = 0.0f;
    float duration_ = 1.0f;
    
public:
    void Update() {
        double deltaTime = fk::core::Clock::Instance().GetDeltaSeconds();
        currentTime_ += deltaTime;
        
        if (currentTime_ >= duration_) {
            currentTime_ = duration_;
            IsComplete = true;
        }
        
        // 计算动画进度 [0, 1]
        float progress = currentTime_ / duration_;
        OnProgress(progress);
    }
};
```

### 4. 暂停系统

```cpp
class GameState {
private:
    fk::core::Clock& clock_ = fk::core::Clock::Instance();
    bool paused_ = false;
    
public:
    void SetPaused(bool paused) {
        if (paused && !paused_) {
            // 进入暂停
            paused_ = true;
        } else if (!paused && paused_) {
            // 退出暂停，重置以避免大的 deltaTime
            paused_ = false;
            clock_.Reset();
        }
    }
    
    void Update() {
        if (paused_) return;
        
        double deltaTime = clock_.GetDeltaSeconds();
        // ... 正常更新
    }
};
```

## 最佳实践

### 1. 使用单一全局时钟

- 不要创建多个 Clock 实例
- 始终通过 `Clock::Instance()` 访问
- 避免在不同线程中混合使用 `Now()` 和 `GetDeltaSeconds()`

### 2. 正确处理时间差

```cpp
// ✅ 推荐：参数化时间差
void Update(double deltaTime) {
    velocity_ += acceleration_ * deltaTime;
    position_ += velocity_ * deltaTime;
}

// ❌ 避免：固定时间步长
void Update() {
    // 这会导致帧率波动时逻辑不稳定
    position_ += velocity_ * 0.016;  // 假设 60 FPS
}
```

### 3. 场景切换时重置

```cpp
void LoadScene(const std::string& sceneName) {
    // 重置时钟避免旧场景的时间流入新场景
    fk::core::Clock::Instance().Reset();
    
    // 加载新场景
    currentScene_ = CreateScene(sceneName);
}
```

### 4. 长运行应用中的时间溢出

对于长时间运行的应用，考虑定期重置时钟以避免浮点精度问题：

```cpp
void MainLoop() {
    int frameCount = 0;
    while (running_) {
        double deltaTime = clock_.GetDeltaSeconds();
        
        Update(deltaTime);
        Render();
        
        frameCount++;
        if (frameCount >= 3600) {  // 每小时重置
            clock_.Reset();
            frameCount = 0;
        }
    }
}
```

## 常见问题

### Q: GetDeltaSeconds() 首次调用返回 0 吗？
**A:** 是的。首次调用时没有"上一次"时间点，所以返回 0。第二次及后续调用才会返回真实的时间差。

### Q: 如何实现固定时间步长的物理模拟？
**A:** 虽然 Clock 提供可变时间步长，但可以累积时间并分割成固定步长：

```cpp
double accumulator = 0;
const double fixedStep = 1.0 / 60.0;  // 60 Hz

while (running_) {
    double deltaTime = clock_.GetDeltaSeconds();
    accumulator += deltaTime;
    
    while (accumulator >= fixedStep) {
        PhysicsUpdate(fixedStep);
        accumulator -= fixedStep;
    }
}
```

### Q: 在多线程环境中如何安全地使用 Clock？
**A:** Clock 的所有方法都是线程安全的，但建议在主线程中调用 `GetDeltaSeconds()`，在其他线程中仅使用 `Now()` 进行时间点查询。

## 相关文档

- [Timer.md](./Timer.md) - 定时器系统（基于 Clock）
- [Event.md](./Event.md) - 事件系统
- [Dispatcher.md](./Dispatcher.md) - 调度器
