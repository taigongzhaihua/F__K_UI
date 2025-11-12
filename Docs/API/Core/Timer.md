# Timer

## 概览

**目的**：定时器实现

**命名空间**：`fk::core`

**头文件**：`fk/core/Timer.h`

## 描述

`Timer` 提供定时回调功能。

## 公共接口

### 创建定时器

#### Timer
```cpp
Timer(std::chrono::milliseconds interval, std::function<void()> callback);
```

创建定时器。

### 控制

#### Start / Stop
```cpp
void Start();
void Stop();
```

启动和停止定时器。

## 使用示例

### 周期性任务
```cpp
auto timer = std::make_shared<Timer>(
    std::chrono::seconds(1),
    []() {
        std::cout << "每秒执行" << std::endl;
    }
);

timer->Start();

// 稍后停止
timer->Stop();
```

### 一次性任务
```cpp
auto timer = std::make_shared<Timer>(
    std::chrono::seconds(5),
    []() {
        std::cout << "5秒后执行一次" << std::endl;
    }
);

timer->SetRepeating(false);
timer->Start();
```

## 相关类

- [Clock](Clock.md)
- [Dispatcher](Dispatcher.md)
