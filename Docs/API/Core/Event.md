# Event

## 概览

**目的**：类型安全的事件系统

**命名空间**：`fk::core`

**头文件**：`fk/core/Event.h`

**模板参数**：`Event<Args...>` - 事件参数类型

## 描述

`Event` 提供类型安全的事件订阅和触发机制。支持多个订阅者，自动管理连接生命周期。

## 公共接口

### 订阅事件

#### operator+=
```cpp
Connection operator+=(std::function<void(Args...)> handler);
```

订阅事件，返回连接对象。

**示例**：
```cpp
Event<int, std::string> myEvent;

auto connection = myEvent += [](int value, const std::string& text) {
    std::cout << "事件触发: " << value << ", " << text << std::endl;
};
```

### 触发事件

#### Invoke
```cpp
void Invoke(Args... args);
```

触发事件，通知所有订阅者。

**示例**：
```cpp
myEvent.Invoke(42, "Hello");
```

### Connection 类

#### Disconnect
```cpp
void Connection::Disconnect();
```

断开事件订阅。

**示例**：
```cpp
auto connection = myEvent += handler;
// 稍后...
connection.Disconnect();
```

#### IsConnected
```cpp
bool Connection::IsConnected() const;
```

检查连接是否仍然活动。

## 使用示例

### 基本事件订阅
```cpp
class Button {
public:
    core::Event<> Click;  // 无参数事件
};

auto button = std::make_shared<Button>();
button->Click += []() {
    std::cout << "按钮被点击" << std::endl;
};

// 触发事件
button->Click.Invoke();
```

### 带参数的事件
```cpp
class TextField {
public:
    core::Event<const std::string&> TextChanged;
};

auto textField = std::make_shared<TextField>();
textField->TextChanged += [](const std::string& newText) {
    std::cout << "文本变更为: " << newText << std::endl;
};

textField->TextChanged.Invoke("新文本");
```

### 管理连接
```cpp
class MyClass {
private:
    core::Connection connection_;
    
public:
    void Subscribe(Event<int>& event) {
        connection_ = event += [this](int value) {
            OnEventFired(value);
        };
    }
    
    void Unsubscribe() {
        if (connection_.IsConnected()) {
            connection_.Disconnect();
        }
    }
    
    void OnEventFired(int value) {
        // 处理事件
    }
};
```

### 多个订阅者
```cpp
Event<std::string> messageEvent;

// 订阅者1
messageEvent += [](const std::string& msg) {
    std::cout << "订阅者1收到: " << msg << std::endl;
};

// 订阅者2
messageEvent += [](const std::string& msg) {
    std::cout << "订阅者2收到: " << msg << std::endl;
};

// 触发 - 两个订阅者都会收到通知
messageEvent.Invoke("Hello");
```

### RAII连接管理
```cpp
{
    auto connection = button->Click += []() {
        std::cout << "点击" << std::endl;
    };
    
    // connection超出作用域时自动断开
}
```

## 线程安全

`Event` 类不是线程安全的。如果需要从多个线程访问，请使用 `Dispatcher`：

```cpp
dispatcher->InvokeAsync([&event]() {
    event.Invoke(args);
});
```

## 相关类

- [Dispatcher](Dispatcher.md) - 线程安全的事件调度

## 另请参阅

- [设计文档](../../Design/Core/Event.md)
