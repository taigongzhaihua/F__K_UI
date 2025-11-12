# Event - 设计文档

## 概览

**目的**：类型安全的事件系统

## 设计目标

1. **类型安全** - 编译时类型检查
2. **简洁API** - 易于订阅和取消
3. **RAII** - 自动生命周期管理
4. **性能** - 最小开销

## 实现

### 基于std::function

```cpp
template<typename... Args>
class Event {
public:
    Connection operator+=(std::function<void(Args...)> handler) {
        auto connection = std::make_shared<ConnectionImpl>();
        connection->handler = handler;
        handlers_.push_back(connection);
        return Connection(connection);
    }
    
    void Invoke(Args... args) {
        for (auto& conn : handlers_) {
            if (auto ptr = conn.lock()) {
                ptr->handler(args...);
            }
        }
    }
    
private:
    std::vector<std::weak_ptr<ConnectionImpl>> handlers_;
};
```

## Connection管理

**RAII模式**：
```cpp
{
    auto conn = event += handler;
    // connection在作用域内有效
} // 超出作用域自动断开
```

**手动断开**：
```cpp
auto conn = event += handler;
conn.Disconnect();
```

## 性能考虑

- 使用weak_ptr避免循环引用
- 延迟清理失效的连接
- 小型内联优化

## 另请参阅

- [API文档](../../API/Core/Event.md)
