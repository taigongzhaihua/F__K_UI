# F__K_UI 线程安全指南

**日期**: 2025年10月26日  
**状态**: ✅ 已完成

---

## 一、线程安全原则

### 核心规则

**UI 对象必须在创建它们的线程上访问！**

```cpp
// ❌ 错误：在后台线程修改 UI
std::thread([window]() {
    window->Title("New Title");  // 会抛出异常！
}).detach();

// ✅ 正确：使用 Dispatcher 调度到 UI 线程
std::thread([window]() {
    window->GetDispatcher()->Invoke([window]() {
        window->Title("New Title");  // 安全！
    });
}).detach();
```

---

## 二、ThreadCheckMode 配置

### 可用模式

| 模式 | 行为 | 使用场景 | 性能 |
|------|------|----------|------|
| `Disabled` | 禁用检查 | 发布版本（确认无问题后） | ⚡ 最快 |
| `WarnOnly` | 只警告 | 开发阶段，希望看到问题但不中断 | 🟡 轻微影响 |
| `ThrowException` | 抛异常 | 调试阶段（推荐） | 🟡 轻微影响 |

### 配置方法

```cpp
#include "fk/ui/ThreadingConfig.h"

// 获取全局配置
auto& config = fk::ui::ThreadingConfig::Instance();

// 设置模式
config.SetThreadCheckMode(fk::ui::ThreadCheckMode::ThrowException);

// 查询当前模式
auto mode = config.GetThreadCheckMode();

// 便捷检查
if (config.IsThreadCheckEnabled()) {
    // 线程检查已启用
}
```

### 推荐配置

```cpp
// 开发/调试阶段
#ifdef _DEBUG
    ThreadingConfig::Instance().SetThreadCheckMode(ThreadCheckMode::ThrowException);
#else
    ThreadingConfig::Instance().SetThreadCheckMode(ThreadCheckMode::Disabled);
#endif
```

---

## 三、跨线程调用方法

### 3.1 Dispatcher::Invoke (同步)

阻塞当前线程，等待 UI 线程执行完成。

```cpp
#include "fk/core/Dispatcher.h"

void UpdateUIFromBackgroundThread(std::shared_ptr<fk::ui::Window> window) {
    // 在后台线程中
    std::string result = DoSomeWork();  // 耗时操作
    
    // 同步调用：等待 UI 更新完成
    window->GetDispatcher()->Invoke([window, result]() {
        window->Title(result);  // 在 UI 线程上执行
    });
    
    // 这里 Title 已经更新完成
}
```

**使用场景：**
- 需要等待 UI 更新完成
- 需要获取 UI 对象的返回值

**注意事项：**
- ⚠️ **不要在 UI 线程调用 Invoke**，会导致死锁
- ⚠️ 会阻塞当前线程

### 3.2 Dispatcher::InvokeAsync (异步)

不阻塞当前线程，立即返回。

```cpp
void UpdateUIFromBackgroundThread(std::shared_ptr<fk::ui::Window> window) {
    // 在后台线程中
    std::string result = DoSomeWork();  // 耗时操作
    
    // 异步调用：立即返回，不等待
    window->GetDispatcher()->InvokeAsync([window, result]() {
        window->Title(result);  // 稍后在 UI 线程上执行
    });
    
    // 这里 Title 可能还未更新
}
```

**使用场景：**
- 只需要触发 UI 更新，不需要等待完成
- "发射后不管" (Fire-and-forget)

**优点：**
- ✅ 不阻塞后台线程
- ✅ 性能更好

### 3.3 检查是否在 UI 线程

```cpp
void SomeMethod(fk::ui::UIElement* element) {
    if (element->HasThreadAccess()) {
        // 已在 UI 线程，直接操作
        element->SetOpacity(0.5f);
    } else {
        // 在后台线程，需要调度
        element->GetDispatcher()->InvokeAsync([element]() {
            element->SetOpacity(0.5f);
        });
    }
}
```

---

## 四、常见场景

### 4.1 网络请求后更新 UI

```cpp
void LoadDataAndUpdateUI(std::shared_ptr<fk::ui::Window> window) {
    // 启动后台线程进行网络请求
    std::thread([window]() {
        try {
            // 耗时网络操作
            std::string data = FetchDataFromServer();
            
            // 更新 UI（异步）
            window->GetDispatcher()->InvokeAsync([window, data]() {
                // 解析并显示数据
                auto content = ParseAndCreateUI(data);
                window->Content(content);
            });
        } catch (const std::exception& e) {
            // 错误处理（也要在 UI 线程）
            window->GetDispatcher()->InvokeAsync([window, error = e.what()]() {
                ShowErrorDialog(window, error);
            });
        }
    }).detach();
}
```

### 4.2 定时器更新 UI

```cpp
class MyWindow : public fk::ui::Window {
    std::unique_ptr<std::thread> timerThread_;
    std::atomic<bool> running_{false};
    
public:
    void StartTimer() {
        running_ = true;
        timerThread_ = std::make_unique<std::thread>([this]() {
            while (running_) {
                std::this_thread::sleep_for(std::chrono::seconds(1));
                
                // 更新时间显示
                auto now = std::chrono::system_clock::now();
                GetDispatcher()->InvokeAsync([this, now]() {
                    UpdateTimeDisplay(now);
                });
            }
        });
    }
    
    void StopTimer() {
        running_ = false;
        if (timerThread_ && timerThread_->joinable()) {
            timerThread_->join();
        }
    }
};
```

### 4.3 进度更新

```cpp
void ProcessFilesWithProgress(
    std::shared_ptr<fk::ui::Window> window,
    std::shared_ptr<fk::ui::ProgressBar> progressBar
) {
    std::thread([window, progressBar]() {
        const int totalFiles = 100;
        
        for (int i = 0; i < totalFiles; ++i) {
            ProcessFile(i);  // 耗时操作
            
            // 更新进度条
            int progress = (i + 1) * 100 / totalFiles;
            progressBar->GetDispatcher()->InvokeAsync([progressBar, progress]() {
                progressBar->Value(progress);
            });
        }
        
        // 完成
        window->GetDispatcher()->InvokeAsync([window]() {
            ShowCompletionMessage(window);
        });
    }).detach();
}
```

---

## 五、错误处理

### 5.1 捕获线程安全异常

```cpp
try {
    // 错误的跨线程调用
    std::thread([window]() {
        window->Title("New Title");  // 抛出异常
    }).detach();
} catch (const std::runtime_error& e) {
    std::cerr << "Thread safety violation: " << e.what() << std::endl;
    // 错误信息：
    // "Cross-thread operation not allowed on UIElement. 
    //  UI objects can only be accessed from the thread they were created on. 
    //  Use Dispatcher::Invoke() or Dispatcher::InvokeAsync() for cross-thread calls."
}
```

### 5.2 WarnOnly 模式调试

```cpp
// 开发阶段，希望看到所有跨线程访问但不中断
ThreadingConfig::Instance().SetThreadCheckMode(ThreadCheckMode::WarnOnly);

// 会输出警告到 stderr：
// [WARNING] Cross-thread access detected on UIElement. 
// Use Dispatcher::Invoke() or Dispatcher::InvokeAsync() for thread-safe calls.

// 程序继续运行（但可能导致崩溃或未定义行为！）
```

---

## 六、性能考虑

### 调度开销

| 操作 | 开销 | 说明 |
|------|------|------|
| 线程检查 | ~10ns | 检查线程 ID |
| Invoke | ~100μs | 同步等待，包含上下文切换 |
| InvokeAsync | ~1μs | 只是入队，立即返回 |

### 优化建议

1. **批量更新**

```cpp
// ❌ 不好：多次调度
for (int i = 0; i < 1000; ++i) {
    dispatcher->InvokeAsync([element, i]() {
        element->Update(i);
    });
}

// ✅ 更好：批量调度
std::vector<int> updates;
for (int i = 0; i < 1000; ++i) {
    updates.push_back(i);
}
dispatcher->InvokeAsync([element, updates]() {
    for (int i : updates) {
        element->Update(i);
    }
});
```

2. **减少跨线程调用**

```cpp
// ❌ 不好：频繁跨线程
void UpdateUI() {
    for (auto& item : items) {
        dispatcher->InvokeAsync([this, item]() {
            AddItem(item);  // 每次都跨线程
        });
    }
}

// ✅ 更好：在 UI 线程批量处理
void UpdateUI() {
    auto itemsCopy = items;
    dispatcher->InvokeAsync([this, itemsCopy]() {
        for (const auto& item : itemsCopy) {
            AddItem(item);  // 在 UI 线程批量处理
        }
    });
}
```

---

## 七、为什么移除 AutoDispatch？

### 原计划的 AutoDispatch 模式

```cpp
// 原计划：自动调度
ThreadingConfig::Instance().SetThreadCheckMode(ThreadCheckMode::AutoDispatch);

// 后台线程直接调用，框架自动调度到 UI 线程
std::thread([window]() {
    window->Title("New Title");  // 框架自动 Invoke
}).detach();
```

### 移除原因

1. **复杂度高** 🔴
   - 需要实现命令队列、参数捕获、异步执行框架
   - 大幅增加框架复杂度和维护成本

2. **性能开销大** ⚠️
   - 每次属性访问都要检查并可能入队
   - 无法批量优化

3. **隐藏问题** ❌
   - 自动调度会隐藏线程安全问题
   - 开发者不知道何时发生了跨线程调用
   - 难以调试和优化

4. **语义不清晰** ❌
   - 同步还是异步？
   - 返回值如何处理？
   - 异常如何传播？

5. **显式更好** ✅
   - 显式的 `Dispatcher::Invoke()` 更清晰
   - 开发者明确知道发生了什么
   - 更好的性能和可控性

### 最佳实践

```cpp
// ✅ 推荐：显式调度
if (element->HasThreadAccess()) {
    element->Update();
} else {
    element->GetDispatcher()->InvokeAsync([element]() {
        element->Update();
    });
}
```

---

## 八、总结

### ✅ 线程安全检查清单

- [x] 所有 UI 操作在 UI 线程
- [x] 后台线程使用 Dispatcher::Invoke/InvokeAsync
- [x] 调试时启用 ThrowException 模式
- [x] 发布时考虑 Disabled 模式（确认安全后）
- [x] 避免在 UI 线程调用 Invoke（死锁）
- [x] 优先使用 InvokeAsync（性能更好）

### 🎯 关键要点

1. **UI 对象只能在创建线程访问**
2. **使用 Dispatcher 进行跨线程调用**
3. **调试时启用线程检查**
4. **异步调用优于同步调用**
5. **批量更新减少调度开销**

### 📚 相关文档

- `include/fk/ui/ThreadingConfig.h` - 线程检查配置
- `include/fk/core/Dispatcher.h` - 线程调度器
- `include/fk/ui/DispatcherObject.h` - 线程关联对象

---

**编写时间**: 2025-10-26  
**版本**: v1.0  
**状态**: ✅ 已完成，AutoDispatch 已移除
