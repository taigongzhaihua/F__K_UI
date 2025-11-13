# Clock

## 概览

**目的**：时间和帧跟踪

**命名空间**：`fk::core`

**头文件**：`fk/core/Clock.h`

## 描述

`Clock` 提供高精度时间测量和帧时间跟踪，用于动画和性能分析。

## 公共接口

### 时间测量

#### GetElapsedTime
```cpp
std::chrono::milliseconds GetElapsedTime() const;
```

获取自Clock启动以来的经过时间。

#### GetDeltaTime
```cpp
std::chrono::milliseconds GetDeltaTime() const;
```

获取上一帧到当前帧的时间差。

### 帧统计

#### GetFrameCount
```cpp
uint64_t GetFrameCount() const;
```

获取总帧数。

#### GetFPS
```cpp
double GetFPS() const;
```

获取当前帧率（每秒帧数）。

## 使用示例

### 基本使用
```cpp
Clock clock;

// 游戏循环
while (running) {
    clock.Tick();  // 更新时钟
    
    auto deltaTime = clock.GetDeltaTime();
    auto fps = clock.GetFPS();
    
    Update(deltaTime);
    Render();
}
```

### 动画
```cpp
double animationTime = 0;

void Update(std::chrono::milliseconds deltaTime) {
    animationTime += deltaTime.count() / 1000.0;
    
    // 使用animationTime计算动画帧
}
```

## 相关类

- [Timer](Timer.md)
