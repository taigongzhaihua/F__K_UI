# INotifyPropertyChanged

## 概览

| 属性 | 值 |
|------|-----|
| **命名空间** | `fk::binding` |
| **头文件** | `fk/binding/INotifyPropertyChanged.h` |
| **类型** | 纯虚接口 |

## 描述

`INotifyPropertyChanged` 是属性变更通知的核心接口。实现此接口的类可以在属性值变更时通知绑定系统，从而实现数据绑定的自动更新。

**WPF 对应**：`System.ComponentModel.INotifyPropertyChanged`

主要用途：
- 为数据绑定源提供属性变更通知机制
- 使 `BindingExpression` 能够订阅并响应源属性变更
- 实现 MVVM 模式中 ViewModel 与 View 的自动同步

## 接口定义

```cpp
namespace fk::binding {

class INotifyPropertyChanged {
public:
    using PropertyChangedEvent = core::Event<std::string_view>;

    virtual ~INotifyPropertyChanged() = default;

    virtual PropertyChangedEvent& PropertyChanged() = 0;
};

} // namespace fk::binding
```

---

## 类型别名

### PropertyChangedEvent

```cpp
using PropertyChangedEvent = core::Event<std::string_view>;
```

属性变更事件类型。

**参数类型**：`std::string_view` - 变更的属性名称

**说明**：
- 基于 `fk::core::Event` 模板
- 事件参数为属性名称，允许订阅者根据名称过滤感兴趣的属性

---

## 公共方法

### PropertyChanged

```cpp
virtual PropertyChangedEvent& PropertyChanged() = 0;
```

获取属性变更事件。

**返回值**：`PropertyChangedEvent&` - 属性变更事件的引用

**说明**：
- 纯虚函数，必须由派生类实现
- 返回的事件用于订阅属性变更通知
- 当属性值变更时，应触发此事件并传递属性名称

**示例**：
```cpp
// 订阅属性变更
auto connection = obj->PropertyChanged().Connect([](std::string_view propName) {
    std::cout << "属性 " << propName << " 已变更" << std::endl;
});
```

---

## 实现指南

### 手动实现

```cpp
class Person : public fk::INotifyPropertyChanged {
public:
    // 实现接口
    PropertyChangedEvent& PropertyChanged() override {
        return propertyChanged_;
    }

    // 属性访问器
    const std::string& Name() const { return name_; }
    
    void SetName(const std::string& value) {
        if (name_ != value) {
            name_ = value;
            propertyChanged_("Name");  // 触发事件
        }
    }

    int Age() const { return age_; }
    
    void SetAge(int value) {
        if (age_ != value) {
            age_ = value;
            propertyChanged_("Age");  // 触发事件
        }
    }

private:
    std::string name_;
    int age_ = 0;
    PropertyChangedEvent propertyChanged_;
};
```

### 继承 ObservableObject（推荐）

```cpp
class Person : public fk::ObservableObject {
public:
    const std::string& Name() const { return name_; }
    
    void SetName(const std::string& value) {
        if (name_ != value) {
            name_ = value;
            RaisePropertyChanged("Name");  // 使用辅助方法
        }
    }

private:
    std::string name_;
};
```

### 使用 ObservableProperty（最佳实践）

```cpp
class Person : public fk::ObservableObject {
public:
    // 使用 ObservableProperty 自动处理变更通知
    fk::ObservableProperty<std::string, Person> name{this, "Name"};
    fk::ObservableProperty<int, Person> age{this, "Age", 0};
};

// 使用
Person person;
person.name = "张三";  // 自动触发 PropertyChanged("Name")
person.age = 25;       // 自动触发 PropertyChanged("Age")
```

---

## 绑定系统集成

### 工作原理

```
┌─────────────────────────────────────────────────────────────┐
│                      数据源（ViewModel）                     │
│                INotifyPropertyChanged 实现                   │
└─────────────────────────────────────────────────────────────┘
                              │
                              │ PropertyChanged 事件
                              ▼
┌─────────────────────────────────────────────────────────────┐
│                    BindingExpression                         │
│              订阅 PropertyChanged 事件                       │
│              过滤匹配绑定 Path 的属性变更                    │
└─────────────────────────────────────────────────────────────┘
                              │
                              │ UpdateTarget()
                              ▼
┌─────────────────────────────────────────────────────────────┐
│                    目标 DependencyObject                     │
│                    属性值自动更新                            │
└─────────────────────────────────────────────────────────────┘
```

### BindingExpression 订阅机制

```cpp
// BindingExpression 内部实现
void BindingExpression::SubscribeToSource() {
    // 尝试获取 INotifyPropertyChanged 接口
    if (auto* notifier = std::any_cast<std::shared_ptr<INotifyPropertyChanged>>(&source_)) {
        // 订阅 PropertyChanged 事件
        sourceConnection_ = (*notifier)->PropertyChanged().Connect(
            [this](std::string_view propName) {
                // 检查属性名是否匹配绑定路径
                if (propName == definition_.GetPath()) {
                    UpdateTarget();  // 更新目标值
                }
            });
    }
}
```

---

## 使用示例

### 基本数据绑定

```cpp
// ViewModel
class UserViewModel : public fk::ObservableObject {
public:
    fk::ObservableProperty<std::string, UserViewModel> username{this, "Username"};
    fk::ObservableProperty<bool, UserViewModel> isLoggedIn{this, "IsLoggedIn", false};
};

// 创建绑定
auto viewModel = std::make_shared<UserViewModel>();

auto textBox = std::make_shared<TextBox>();
textBox->SetBinding(TextBox::TextProperty(), fk::bind("Username"));
textBox->SetDataContext(viewModel);

// 更新 ViewModel，UI 自动同步
viewModel->username = "admin";  // TextBox 显示 "admin"
```

### 监听多个属性

```cpp
auto viewModel = std::make_shared<UserViewModel>();

// 监听所有属性变更
auto connection = viewModel->PropertyChanged().Connect([](std::string_view propName) {
    std::cout << "属性变更: " << propName << std::endl;
});

viewModel->username = "user1";    // 输出: 属性变更: Username
viewModel->isLoggedIn = true;     // 输出: 属性变更: IsLoggedIn
```

### 监听特定属性

```cpp
auto viewModel = std::make_shared<UserViewModel>();

// 只关心 Username 的变更
auto connection = viewModel->PropertyChanged().Connect([](std::string_view propName) {
    if (propName == "Username") {
        std::cout << "用户名已变更!" << std::endl;
    }
});
```

### 级联属性变更

```cpp
class OrderViewModel : public fk::ObservableObject {
public:
    fk::ObservableProperty<double, OrderViewModel> unitPrice{this, "UnitPrice", 0.0};
    fk::ObservableProperty<int, OrderViewModel> quantity{this, "Quantity", 0};
    
    double TotalPrice() const {
        return unitPrice.Get() * quantity.Get();
    }

    OrderViewModel() {
        // 当 UnitPrice 或 Quantity 变更时，通知 TotalPrice 也变更
        PropertyChanged().Connect([this](std::string_view propName) {
            if (propName == "UnitPrice" || propName == "Quantity") {
                RaisePropertyChanged("TotalPrice");
            }
        });
    }
};
```

### 断开连接

```cpp
auto viewModel = std::make_shared<UserViewModel>();

auto connection = viewModel->PropertyChanged().Connect([](std::string_view propName) {
    // 处理变更
});

// 稍后断开连接
connection.Disconnect();

// 或者让 connection 超出作用域自动断开
```

---

## 实现类

| 类 | 描述 |
|----|------|
| [ObservableObject](ObservableObject.md) | 基础实现，提供 `RaisePropertyChanged()` 辅助方法 |
| [ObservableProperty](ObservableProperty.md) | 属性包装器，自动触发变更通知 |

---

## 最佳实践

1. **优先使用 ObservableObject**：继承 `ObservableObject` 而非直接实现 `INotifyPropertyChanged`，获得辅助方法

2. **使用 ObservableProperty**：使用 `ObservableProperty<T, Owner>` 自动处理变更通知，减少样板代码

3. **属性名一致性**：确保触发事件时使用的属性名与绑定路径完全匹配

4. **避免不必要的通知**：在 setter 中先检查值是否真的变更，避免重复通知

5. **管理连接生命周期**：保存 `Connection` 对象，在不需要时断开，避免内存泄漏

---

## 相关类

- [ObservableObject](ObservableObject.md) - `INotifyPropertyChanged` 的基础实现
- [ObservableProperty](ObservableProperty.md) - 自动触发变更通知的属性包装器
- [Binding](Binding.md) - 数据绑定配置
- [BindingExpression](BindingExpression.md) - 订阅并响应属性变更

## 另请参阅

- [数据绑定概述](../BINDING_MODULE_OVERVIEW.md)
- [入门指南](../../Guides/GettingStarted.md)
