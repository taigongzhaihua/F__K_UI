# Dispatcher

## Overview

**Status**: ✅ Fully implemented

**Purpose**: Thread-safe message dispatching and task execution with priority queuing

**Namespace**: `fk::core`

**Inheritance**: `std::enable_shared_from_this<Dispatcher>`

**Header**: `fk/core/Dispatcher.h`

## Description

`Dispatcher` provides a thread-safe mechanism for queuing and executing tasks on a specific thread. It's the foundation for the UI thread model in F__K_UI, ensuring all UI operations occur on the correct thread.

## Public Interface

### Task Posting

```cpp
void Post(Task task, Priority priority = Priority::Normal);
DispatcherOperation BeginInvoke(Task task, Priority priority = Priority::Normal);
DispatcherOperation PostDelayed(Task task, std::chrono::milliseconds delay, Priority priority = Priority::Normal);
void Send(Task task);  // Synchronous
```

### Message Loop

```cpp
void Run();  // Start message loop
void Shutdown();  // Stop message loop
```

### Thread Affinity

```cpp
bool HasThreadAccess() const;
void VerifyAccess() const;
```

## Usage Examples

```cpp
auto dispatcher = std::make_shared<Dispatcher>("UI");

// Post async task
dispatcher->Post([]() {
    std::cout << "On UI thread" << std::endl;
});

// Check thread
if (!dispatcher->HasThreadAccess()) {
    dispatcher->Post([this]() { UpdateUI(); });
}
```

## See Also

- [Design Document](../../Design/Core/Dispatcher.md)
