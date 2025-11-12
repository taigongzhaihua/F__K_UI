# Dispatcher

## 概览

**状态**：✅ 已完全实现

**目的**：线程安全的消息调度和任务执行，具有优先级队列

**命名空间**：`fk::core`

**继承**：`std::enable_shared_from_this<Dispatcher>`

**头文件**：`fk/core/Dispatcher.h`

## 描述

`Dispatcher` 提供了一种线程安全的机制来在特定线程上排队和执行任务。它是 F__K_UI 中 UI 线程模型的基础，确保所有 UI 操作都在正确的线程上发生。

## 公共接口

### 任务投递

```cpp
void Post(Task task, Priority priority = Priority::Normal);
DispatcherOperation BeginInvoke(Task task, Priority priority = Priority::Normal);
DispatcherOperation PostDelayed(Task task, std::chrono::milliseconds delay, Priority priority = Priority::Normal);
void Send(Task task);  // 同步
```

### 消息循环

```cpp
void Run();  // 启动消息循环
void Shutdown();  // 停止消息循环
```

### 线程关联

```cpp
bool HasThreadAccess() const;
void VerifyAccess() const;
```

## 使用示例

```cpp
auto dispatcher = std::make_shared<Dispatcher>("UI");

// 投递异步任务
dispatcher->Post([]() {
    std::cout << "在 UI 线程上" << std::endl;
});

// 检查线程
if (!dispatcher->HasThreadAccess()) {
    dispatcher->Post([this]() { UpdateUI(); });
}
```

## 另请参阅

- [设计文档](../../Design/Core/Dispatcher.md)
