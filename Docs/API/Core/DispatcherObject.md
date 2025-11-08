# DispatcherObject - 线程调度对象

提供线程访问检查和 Dispatcher 关联的基类。

## 概述

`DispatcherObject` 是 UI 线程模型的基础，提供：

- **Dispatcher 关联**：每个对象关联到一个 Dispatcher
- **线程访问检查**：HasThreadAccess / VerifyAccess
- **线程安全保证**：确保 UI 操作在正确线程执行

## 类层次结构

```
DispatcherObject    ← 你在这里
  └─ DependencyObject
      └─ UIElement
          └─ ...
```

## 基本用法

### 创建 DispatcherObject

```cpp
#include <fk/ui/DispatcherObject.h>

// 使用默认 Dispatcher
auto obj = std::make_shared<DispatcherObject>();

// 使用指定 Dispatcher
auto dispatcher = std::make_shared<Dispatcher>("UI");
auto obj = std::make_shared<DispatcherObject>(dispatcher);
```

### 线程访问检查

```cpp
// 检查是否在正确线程
if (obj->HasThreadAccess()) {
    // 可以安全访问
    obj->DoSomething();
} else {
    // 投递到正确线程
    obj->GetDispatcher()->Post([obj]() {
        obj->DoSomething();
    });
}
```

## 主要方法

### GetDispatcher / Dispatcher - 获取调度器

```cpp
std::shared_ptr<core::Dispatcher> GetDispatcher() const;
core::Dispatcher* Dispatcher() const noexcept;
```

获取关联的 Dispatcher。

**示例：**

```cpp
auto dispatcher = element->GetDispatcher();
if (dispatcher) {
    dispatcher->Post([element]() {
        element->SetValue(Property(), value);
    });
}

// 或使用原始指针
auto* disp = element->Dispatcher();
```

### SetDispatcher - 设置调度器

```cpp
void SetDispatcher(std::shared_ptr<core::Dispatcher> dispatcher);
```

设置关联的 Dispatcher（通常在对象创建时）。

**示例：**

```cpp
auto uiDispatcher = std::make_shared<Dispatcher>("UI");
element->SetDispatcher(uiDispatcher);
```

### HasThreadAccess - 检查线程访问

```cpp
bool HasThreadAccess() const;
```

检查当前线程是否可以访问此对象。

**返回：** `true` 表示当前线程是对象的 Dispatcher 线程

**示例：**

```cpp
void UpdateUI() {
    if (element->HasThreadAccess()) {
        // 当前线程正确，直接更新
        element->SetContent(newContent);
    } else {
        // 切换到 UI 线程
        element->GetDispatcher()->Post([this]() {
            element->SetContent(newContent);
        });
    }
}
```

### VerifyAccess - 断言线程访问

```cpp
void VerifyAccess() const;
```

断言当前线程可以访问对象，否则抛出异常。

**示例：**

```cpp
void UpdateElement() {
    element->VerifyAccess();  // 如果不在正确线程，抛异常
    
    // 安全访问
    element->SetValue(Property(), value);
}
```

## 完整示例

### 示例 1：跨线程更新 UI

```cpp
#include <fk/ui/DispatcherObject.h>
#include <fk/core/Dispatcher.h>
#include <thread>

class UIController {
public:
    UIController(std::shared_ptr<Button> button) 
        : button_(button) {}
    
    void UpdateFromBackground(const std::string& data) {
        // 后台线程调用
        if (button_->HasThreadAccess()) {
            // 已在 UI 线程
            UpdateUI(data);
        } else {
            // 投递到 UI 线程
            button_->GetDispatcher()->Post([this, data]() {
                UpdateUI(data);
            });
        }
    }
    
private:
    void UpdateUI(const std::string& data) {
        button_->VerifyAccess();  // 断言在 UI 线程
        button_->SetContent(textBlock().Text(data));
    }
    
    std::shared_ptr<Button> button_;
};

int main() {
    auto uiDispatcher = std::make_shared<Dispatcher>("UI");
    
    std::thread uiThread([uiDispatcher]() {
        uiDispatcher->Run();
    });
    
    auto button = std::make_shared<Button>(uiDispatcher);
    auto controller = std::make_shared<UIController>(button);
    
    // 后台线程更新 UI
    std::thread worker([controller]() {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        controller->UpdateFromBackground("Updated from worker");
    });
    
    worker.join();
    uiDispatcher->Shutdown();
    uiThread.join();
}
```

### 示例 2：安全的属性访问器

```cpp
class SafeElement : public DispatcherObject {
public:
    void SetValue(int value) {
        if (HasThreadAccess()) {
            value_ = value;
        } else {
            GetDispatcher()->Send([this, value]() {
                value_ = value;
            });
        }
    }
    
    int GetValue() const {
        if (HasThreadAccess()) {
            return value_;
        } else {
            int result = 0;
            GetDispatcher()->Send([this, &result]() {
                result = value_;
            });
            return result;
        }
    }
    
private:
    int value_{0};
};
```

### 示例 3：UI 更新助手类

```cpp
class UIUpdater {
public:
    UIUpdater(std::shared_ptr<DispatcherObject> target)
        : target_(target), dispatcher_(target->GetDispatcher()) {}
    
    template<typename Func>
    void Post(Func&& func) {
        if (target_->HasThreadAccess()) {
            func();
        } else {
            dispatcher_->Post(std::forward<Func>(func));
        }
    }
    
    template<typename Func>
    void Send(Func&& func) {
        if (target_->HasThreadAccess()) {
            func();
        } else {
            dispatcher_->Send(std::forward<Func>(func));
        }
    }
    
private:
    std::shared_ptr<DispatcherObject> target_;
    std::shared_ptr<Dispatcher> dispatcher_;
};

// 使用
auto updater = std::make_shared<UIUpdater>(element);
updater->Post([element]() {
    element->SetContent(newContent);
});
```

### 示例 4：多线程数据加载

```cpp
class DataLoader {
public:
    DataLoader(std::shared_ptr<TextBlock> statusText)
        : statusText_(statusText) {}
    
    void LoadData() {
        UpdateStatus("Loading...");
        
        // 后台加载
        std::thread([this]() {
            auto data = FetchDataFromNetwork();
            
            UpdateStatus("Processing...");
            auto processed = ProcessData(data);
            
            UpdateStatus("Done!");
            DisplayData(processed);
        }).detach();
    }
    
private:
    void UpdateStatus(const std::string& status) {
        if (statusText_->HasThreadAccess()) {
            statusText_->SetText(status);
        } else {
            statusText_->GetDispatcher()->Post([this, status]() {
                statusText_->SetText(status);
            });
        }
    }
    
    std::string FetchDataFromNetwork() {
        std::this_thread::sleep_for(std::chrono::seconds(2));
        return "Network data";
    }
    
    std::string ProcessData(const std::string& data) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        return "Processed: " + data;
    }
    
    void DisplayData(const std::string& data) {
        statusText_->GetDispatcher()->Post([this, data]() {
            statusText_->SetText(data);
        });
    }
    
    std::shared_ptr<TextBlock> statusText_;
};
```

### 示例 5：线程安全的集合更新

```cpp
class ThreadSafeListView : public DispatcherObject {
public:
    void AddItem(const std::string& item) {
        if (HasThreadAccess()) {
            AddItemInternal(item);
        } else {
            GetDispatcher()->Post([this, item]() {
                AddItemInternal(item);
            });
        }
    }
    
    void AddItems(const std::vector<std::string>& items) {
        if (HasThreadAccess()) {
            for (const auto& item : items) {
                AddItemInternal(item);
            }
        } else {
            GetDispatcher()->Post([this, items]() {
                for (const auto& item : items) {
                    AddItemInternal(item);
                }
            });
        }
    }
    
    void Clear() {
        if (HasThreadAccess()) {
            ClearInternal();
        } else {
            GetDispatcher()->Send([this]() {
                ClearInternal();
            });
        }
    }
    
private:
    void AddItemInternal(const std::string& item) {
        VerifyAccess();  // 确保在 UI 线程
        items_.push_back(item);
        // 更新 UI...
    }
    
    void ClearInternal() {
        VerifyAccess();
        items_.clear();
        // 更新 UI...
    }
    
    std::vector<std::string> items_;
};
```

### 示例 6：异步操作包装器

```cpp
template<typename T>
class AsyncResult {
public:
    AsyncResult(std::shared_ptr<DispatcherObject> context)
        : context_(context) {}
    
    void SetResult(T result) {
        if (context_->HasThreadAccess()) {
            result_ = std::move(result);
            OnCompleted();
        } else {
            context_->GetDispatcher()->Post([this, result = std::move(result)]() mutable {
                result_ = std::move(result);
                OnCompleted();
            });
        }
    }
    
    void OnCompleted(std::function<void(const T&)> callback) {
        callback_ = std::move(callback);
    }
    
private:
    void OnCompleted() {
        if (callback_) {
            callback_(result_);
        }
    }
    
    std::shared_ptr<DispatcherObject> context_;
    T result_;
    std::function<void(const T&)> callback_;
};

// 使用
auto result = std::make_shared<AsyncResult<std::string>>(uiElement);
result->OnCompleted([](const std::string& data) {
    std::cout << "Result: " << data << std::endl;
});

std::thread([result]() {
    auto data = DoHeavyWork();
    result->SetResult(data);  // 自动切换到 UI 线程
}).detach();
```

## 最佳实践

### ✅ 推荐做法

**1. 始终检查线程访问**
```cpp
void UpdateElement() {
    if (element->HasThreadAccess()) {
        DoUpdate();
    } else {
        element->GetDispatcher()->Post([this]() { DoUpdate(); });
    }
}
```

**2. 使用 VerifyAccess 保护内部方法**
```cpp
class MyControl : public DispatcherObject {
private:
    void UpdateInternal() {
        VerifyAccess();  // 确保在正确线程
        // 安全操作...
    }
};
```

**3. Post 用于异步，Send 用于同步**
```cpp
// ✅ 不阻塞当前线程
dispatcher->Post([element]() {
    element->Update();
});

// ✅ 需要立即完成
dispatcher->Send([element]() {
    element->Update();
});
// 此处已完成
```

**4. 缓存 Dispatcher 引用**
```cpp
class Worker {
    std::shared_ptr<Dispatcher> uiDispatcher_;
    
public:
    Worker(std::shared_ptr<DispatcherObject> uiElement)
        : uiDispatcher_(uiElement->GetDispatcher()) {}
    
    void Work() {
        uiDispatcher_->Post([]() { /*...*/ });
    }
};
```

### ❌ 避免的做法

**1. 从错误线程访问 UI（崩溃）**
```cpp
// ❌ 后台线程直接访问 UI
std::thread([element]() {
    element->SetContent(text);  // 崩溃！
}).detach();

// ✅ 切换到 UI 线程
std::thread([element]() {
    element->GetDispatcher()->Post([element]() {
        element->SetContent(text);
    });
}).detach();
```

**2. 在 Dispatcher 线程内调用 Send（死锁）**
```cpp
// ❌ 死锁！
dispatcher->Post([dispatcher]() {
    dispatcher->Send([]() {  // 等待自己完成
        // ...
    });
});
```

**3. 忘记检查 Dispatcher 是否为空**
```cpp
// ❌ 可能为 nullptr
element->GetDispatcher()->Post(...);

// ✅ 检查
if (auto disp = element->GetDispatcher()) {
    disp->Post(...);
}
```

## 常见问题

**Q: 所有 UI 对象都需要关联 Dispatcher 吗？**

A: 是的。所有派生自 `DispatcherObject` 的类都关联一个 Dispatcher，通常是 UI 线程的 Dispatcher。

**Q: 如何在创建时指定 Dispatcher？**

A: 通过构造函数：
```cpp
auto element = std::make_shared<Button>(uiDispatcher);
```

**Q: HasThreadAccess 的性能开销？**

A: 很小，只是比较线程 ID，可以频繁调用。

**Q: VerifyAccess 抛出什么异常？**

A: 通常是 `std::runtime_error`，消息为 "Invalid cross-thread access"。

**Q: 可以在运行时更改 Dispatcher 吗？**

A: 技术上可以，但通常不推荐。对象应该在整个生命周期内关联同一个 Dispatcher。

## 性能考虑

- **线程检查**：O(1) 操作（比较线程 ID）
- **内存开销**：~16 字节（智能指针 + mutex）
- **Post 开销**：约 1-10 微秒（取决于队列状态）

**优化建议：**
- 缓存 `HasThreadAccess()` 结果（如果连续多次访问）
- 批量操作在单个 Post 中完成
- 避免频繁的线程切换

## 相关类型

- **Dispatcher** - 任务调度器
- **DependencyObject** - 派生类，添加依赖属性支持
- **UIElement** - 派生类，UI 元素基类

## 总结

`DispatcherObject` 是 UI 线程模型的基础，提供：

✅ Dispatcher 关联  
✅ 线程访问检查（HasThreadAccess）  
✅ 线程安全断言（VerifyAccess）  
✅ 跨线程访问保护

确保 UI 操作在正确的线程执行，避免多线程问题。
