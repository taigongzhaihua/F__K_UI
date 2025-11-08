# INotifyPropertyChanged - 属性变更通知接口

## 概述

`INotifyPropertyChanged` 是 MVVM 模式中 ViewModel 必须实现的核心接口。它允许 ViewModel 向绑定系统通知属性值的变更，驱动 UI 的自动更新。

## 核心 API

### 接口定义

```cpp
class INotifyPropertyChanged {
public:
    using PropertyChangedEvent = core::Event<std::string_view>;
    
    virtual ~INotifyPropertyChanged() = default;
    virtual PropertyChangedEvent& PropertyChanged() = 0;
};
```

### PropertyChanged 事件

```cpp
PropertyChangedEvent PropertyChanged;
```

当属性值发生改变时触发。

**事件签名：** `void(std::string_view propertyName)`

**参数：** 属性名称（作为 `std::string_view`）

## 常见模式

### 1. 基本 ViewModel 实现

```cpp
#include "fk/binding/INotifyPropertyChanged.h"

class LoginViewModel : public fk::binding::INotifyPropertyChanged {
public:
    std::string GetUsername() const { return username_; }
    void SetUsername(const std::string& value) {
        if (username_ != value) {
            username_ = value;
            OnPropertyChanged("Username");
        }
    }
    
    std::string GetPassword() const { return password_; }
    void SetPassword(const std::string& value) {
        if (password_ != value) {
            password_ = value;
            OnPropertyChanged("Password");
        }
    }
    
    fk::core::Event<std::string_view>& PropertyChanged() override {
        return propertyChanged_;
    }
    
protected:
    void OnPropertyChanged(std::string_view propertyName) {
        propertyChanged_.Raise(propertyName);
    }
    
private:
    std::string username_;
    std::string password_;
    fk::core::Event<std::string_view> propertyChanged_;
};
```

### 2. 订阅属性变更

```cpp
auto viewModel = std::make_shared<LoginViewModel>();

// 订阅所有属性变更
viewModel->PropertyChanged().Subscribe(
    [](std::string_view propertyName) {
        std::cout << "属性已更改: " << propertyName << "\n";
    }
);

// 用户修改了 Username
viewModel->SetUsername("Alice");  // 输出: "属性已更改: Username"
```

### 3. 观察特定属性

```cpp
class FormValidator {
public:
    void WatchViewModel(std::shared_ptr<LoginViewModel> vm) {
        vm->PropertyChanged().Subscribe(
            [this](std::string_view propName) {
                if (propName == "Email") {
                    ValidateEmail();
                } else if (propName == "Password") {
                    ValidatePassword();
                }
            }
        );
    }
};
```

### 4. 使用 ObservableObject 基类

```cpp
#include "fk/binding/ObservableObject.h"

// ObservableObject 已经实现了 INotifyPropertyChanged
class UserViewModel : public fk::binding::ObservableObject {
public:
    std::string GetName() const { return name_; }
    void SetName(const std::string& value) {
        if (name_ != value) {
            name_ = value;
            RaisePropertyChanged("Name");  // 方便的方法
        }
    }
    
private:
    std::string name_;
};
```

### 5. 双向绑定流程

```cpp
// 绑定系统流程
auto viewModel = std::make_shared<MyViewModel>();
auto textBox = fk::ui::TextBox::Create();
textBox->SetDataContext(viewModel);

// 创建绑定
auto binding = std::make_shared<fk::binding::Binding>();
binding->SetPath("Name");
binding->SetMode(fk::binding::BindingMode::TwoWay);

auto expr = std::make_shared<fk::binding::BindingExpression>(
    *binding, textBox.get(), fk::ui::TextBox::TextProperty
);

expr->Activate();

// 现在：
// 1. ViewModel.Name 改变 → PropertyChanged 事件
//    → 绑定系统监听到 → 更新 TextBox.Text
// 2. TextBox.Text 改变 → TextBox 属性事件
//    → 绑定系统监听到 → 更新 ViewModel.Name
```

### 6. 链式属性通知

```cpp
class UserProfile : public fk::binding::INotifyPropertyChanged {
public:
    std::string GetFullName() const { 
        return firstName_ + " " + lastName_; 
    }
    
    void SetFirstName(const std::string& value) {
        if (firstName_ != value) {
            firstName_ = value;
            OnPropertyChanged("FirstName");
            OnPropertyChanged("FullName");  // 连锁通知
        }
    }
    
    void SetLastName(const std::string& value) {
        if (lastName_ != value) {
            lastName_ = value;
            OnPropertyChanged("LastName");
            OnPropertyChanged("FullName");  // 连锁通知
        }
    }
    
    fk::core::Event<std::string_view>& PropertyChanged() override {
        return propertyChanged_;
    }
    
protected:
    void OnPropertyChanged(std::string_view propName) {
        propertyChanged_.Raise(propName);
    }
    
private:
    std::string firstName_;
    std::string lastName_;
    fk::core::Event<std::string_view> propertyChanged_;
};

// 使用：改变 FirstName 会同时通知 FirstName 和 FullName 变更
profile->SetFirstName("John");
```

## 实现指南

### 步骤 1: 继承接口

```cpp
class MyViewModel : public fk::binding::INotifyPropertyChanged {
    // ...
};
```

### 步骤 2: 实现 PropertyChanged() 方法

```cpp
fk::core::Event<std::string_view>& PropertyChanged() override {
    return propertyChanged_;
}

private:
    fk::core::Event<std::string_view> propertyChanged_;
```

### 步骤 3: 在 setter 中触发事件

```cpp
void SetPropertyName(const std::string& value) {
    if (property_ != value) {
        property_ = value;
        propertyChanged_.Raise("PropertyName");
    }
}
```

### 步骤 4: （可选）使用 ObservableObject

```cpp
class MyViewModel : public fk::binding::ObservableObject {
    // ObservableObject 已实现 PropertyChanged
    // 只需在 setter 中调用 RaisePropertyChanged()
};
```

## 最佳实践

### 1. 只在值改变时触发

```cpp
// ✅ 推荐
void SetValue(int value) {
    if (value_ != value) {
        value_ = value;
        OnPropertyChanged("Value");
    }
}

// ❌ 避免：无谓触发事件
void SetValue(int value) {
    value_ = value;
    OnPropertyChanged("Value");  // 即使值未改变也触发
}
```

### 2. 属性名使用常量

```cpp
// ✅ 推荐
class MyViewModel : public fk::binding::INotifyPropertyChanged {
private:
    static constexpr std::string_view PropName = "Name";
    
    void SetName(const std::string& value) {
        if (name_ != value) {
            name_ = value;
            OnPropertyChanged(PropName);
        }
    }
};

// ❌ 避免：硬编码字符串
OnPropertyChanged("Name");  // 拼写错误难以发现
```

### 3. 对于计算属性，在依赖属性改变时通知

```cpp
void SetFirstName(const std::string& value) {
    if (firstName_ != value) {
        firstName_ = value;
        OnPropertyChanged("FirstName");
        OnPropertyChanged("FullName");  // 计算属性也要通知
    }
}
```

### 4. 使用 ObservableObject 简化代码

```cpp
// ✅ 更简洁
class UserViewModel : public fk::binding::ObservableObject {
    void SetEmail(const std::string& value) {
        if (email_ != value) {
            email_ = value;
            RaisePropertyChanged("Email");
        }
    }
};
```

## 常见问题

### Q: 必须手动实现 PropertyChanged() 吗？
**A:** 不必须。可以继承 `ObservableObject`，它已经提供了实现。

### Q: 属性名拼写错误会怎样？
**A:** 绑定系统无法找到对应的属性。建议使用常量或代码生成宏。

### Q: 一次可以通知多个属性吗？
**A:** 可以，多次调用 `OnPropertyChanged()`，但不能一次通知多个。

### Q: 绑定系统如何监听 PropertyChanged 事件？
**A:** BindingExpression 自动订阅源的 PropertyChanged 事件，更新目标。

## 相关文档

- [ObservableObject.md](./ObservableObject.md) - 可观察对象基类
- [BindingExpression.md](./BindingExpression.md) - 绑定表达式
- [Event.md](../Core/Event.md) - 事件系统
