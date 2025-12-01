# ObservableObject

## 概览

| 属性 | 值 |
|------|-----|
| **命名空间** | `fk::binding` |
| **头文件** | `fk/binding/ObservableObject.h` |
| **继承自** | `INotifyPropertyChanged` |

## 描述

`ObservableObject` 是 `INotifyPropertyChanged` 接口的基础实现类，提供属性变更通知的核心功能。它是创建 ViewModel 和其他可观察数据模型的推荐基类。

**WPF 对应**：类似于 `ObservableObject` 或 MVVM 框架中的 `ViewModelBase`

主要特点：
- 实现 `INotifyPropertyChanged` 接口
- 提供受保护的 `RaisePropertyChanged()` 辅助方法
- 支持与 `ObservableProperty<T, Owner>` 配合使用
- 简化 MVVM 模式中 ViewModel 的实现

## 类定义

```cpp
namespace fk::binding {

// Forward declaration for friend access
template<typename T, typename Owner>
class ObservableProperty;

class ObservableObject : public INotifyPropertyChanged {
public:
    ObservableObject() = default;
    ~ObservableObject() override = default;

    PropertyChangedEvent& PropertyChanged() override;

protected:
    void RaisePropertyChanged(std::string_view propertyName);
    void RaisePropertyChanged(const std::string& propertyName);

    // Allow ObservableProperty to call RaisePropertyChanged
    template<typename T, typename Owner>
    friend class ObservableProperty;

private:
    PropertyChangedEvent propertyChanged_;
};

} // namespace fk::binding
```

---

## 公共方法

### PropertyChanged

```cpp
PropertyChangedEvent& PropertyChanged() override;
```

获取属性变更事件。

**返回值**：`PropertyChangedEvent&` - 属性变更事件的引用

**说明**：
- 实现自 `INotifyPropertyChanged` 接口
- 返回内部的 `propertyChanged_` 事件
- 用于外部订阅属性变更通知

**示例**：
```cpp
class Person : public fk::ObservableObject {};

auto person = std::make_shared<Person>();
auto connection = person->PropertyChanged().Connect([](std::string_view propName) {
    std::cout << "属性 " << propName << " 已变更" << std::endl;
});
```

---

## 受保护方法

### RaisePropertyChanged

```cpp
void RaisePropertyChanged(std::string_view propertyName);
void RaisePropertyChanged(const std::string& propertyName);
```

触发属性变更事件。

**参数**：
| 参数 | 类型 | 描述 |
|------|------|------|
| `propertyName` | `std::string_view` 或 `const std::string&` | 变更的属性名称 |

**说明**：
- 受保护方法，只能在派生类中调用
- 触发 `PropertyChanged` 事件，通知所有订阅者
- 提供两个重载，支持不同的字符串类型
- `ObservableProperty` 作为友元类，可以直接调用此方法

**示例**：
```cpp
class Person : public fk::ObservableObject {
public:
    const std::string& Name() const { return name_; }
    
    void SetName(const std::string& value) {
        if (name_ != value) {
            name_ = value;
            RaisePropertyChanged("Name");  // 触发变更通知
        }
    }

private:
    std::string name_;
};
```

---

## 私有成员

| 成员 | 类型 | 描述 |
|------|------|------|
| `propertyChanged_` | `PropertyChangedEvent` | 属性变更事件对象 |

---

## 使用方式

### 方式一：手动属性（完全控制）

```cpp
class PersonViewModel : public fk::ObservableObject {
public:
    // Getter
    const std::string& FirstName() const { return firstName_; }
    const std::string& LastName() const { return lastName_; }
    int Age() const { return age_; }
    
    // Setter（带变更通知）
    void SetFirstName(const std::string& value) {
        if (firstName_ != value) {
            firstName_ = value;
            RaisePropertyChanged("FirstName");
            RaisePropertyChanged("FullName");  // 计算属性也需通知
        }
    }
    
    void SetLastName(const std::string& value) {
        if (lastName_ != value) {
            lastName_ = value;
            RaisePropertyChanged("LastName");
            RaisePropertyChanged("FullName");
        }
    }
    
    void SetAge(int value) {
        if (age_ != value) {
            age_ = value;
            RaisePropertyChanged("Age");
        }
    }
    
    // 计算属性（只读）
    std::string FullName() const {
        return firstName_ + " " + lastName_;
    }

private:
    std::string firstName_;
    std::string lastName_;
    int age_ = 0;
};
```

### 方式二：使用 ObservableProperty（推荐）

```cpp
class PersonViewModel : public fk::ObservableObject {
public:
    // 使用 ObservableProperty 自动处理变更通知
    fk::ObservableProperty<std::string, PersonViewModel> firstName{this, "FirstName"};
    fk::ObservableProperty<std::string, PersonViewModel> lastName{this, "LastName"};
    fk::ObservableProperty<int, PersonViewModel> age{this, "Age", 0};
    
    // 计算属性仍需手动实现
    std::string FullName() const {
        return firstName.Get() + " " + lastName.Get();
    }
    
    PersonViewModel() {
        // 监听 firstName 和 lastName 变更，通知 FullName
        PropertyChanged().Connect([this](std::string_view propName) {
            if (propName == "FirstName" || propName == "LastName") {
                RaisePropertyChanged("FullName");
            }
        });
    }
};

// 使用
PersonViewModel vm;
vm.firstName = "张";       // 自动触发 PropertyChanged("FirstName")
vm.lastName = "三";        // 自动触发 PropertyChanged("LastName")
vm.age = 25;               // 自动触发 PropertyChanged("Age")

std::cout << vm.firstName.Get() << std::endl;  // "张"
std::cout << vm.FullName() << std::endl;       // "张 三"
```

### 方式三：SetValue 辅助方法模式

```cpp
class PersonViewModel : public fk::ObservableObject {
public:
    const std::string& Name() const { return name_; }
    void SetName(const std::string& value) { SetValue(name_, value, "Name"); }
    
    int Age() const { return age_; }
    void SetAge(int value) { SetValue(age_, value, "Age"); }

protected:
    // 通用的值设置辅助方法
    template<typename T>
    void SetValue(T& field, const T& newValue, std::string_view propertyName) {
        if (field != newValue) {
            field = newValue;
            RaisePropertyChanged(propertyName);
        }
    }

private:
    std::string name_;
    int age_ = 0;
};
```

---

## 使用示例

### 基本 MVVM 绑定

```cpp
// ViewModel
class UserViewModel : public fk::ObservableObject {
public:
    fk::ObservableProperty<std::string, UserViewModel> username{this, "Username"};
    fk::ObservableProperty<std::string, UserViewModel> email{this, "Email"};
    fk::ObservableProperty<bool, UserViewModel> isActive{this, "IsActive", true};
};

// View 绑定
auto viewModel = std::make_shared<UserViewModel>();

auto usernameBox = std::make_shared<TextBox>();
usernameBox->SetBinding(
    TextBox::TextProperty(),
    fk::bind("Username").Mode(fk::BindingMode::TwoWay)
);
usernameBox->SetDataContext(viewModel);

auto emailBox = std::make_shared<TextBox>();
emailBox->SetBinding(
    TextBox::TextProperty(),
    fk::bind("Email").Mode(fk::BindingMode::TwoWay)
);
emailBox->SetDataContext(viewModel);

// 更新 ViewModel，UI 自动同步
viewModel->username = "admin";
viewModel->email = "admin@example.com";
```

### 监听属性变更

```cpp
auto viewModel = std::make_shared<UserViewModel>();

// 监听所有属性变更
auto connection = viewModel->PropertyChanged().Connect([](std::string_view propName) {
    std::cout << "属性变更: " << propName << std::endl;
});

viewModel->username = "user1";  // 输出: 属性变更: Username
viewModel->isActive = false;    // 输出: 属性变更: IsActive

// 断开连接
connection.Disconnect();
```

### 嵌套 ViewModel

```cpp
class AddressViewModel : public fk::ObservableObject {
public:
    fk::ObservableProperty<std::string, AddressViewModel> city{this, "City"};
    fk::ObservableProperty<std::string, AddressViewModel> street{this, "Street"};
};

class PersonViewModel : public fk::ObservableObject {
public:
    fk::ObservableProperty<std::string, PersonViewModel> name{this, "Name"};
    std::shared_ptr<AddressViewModel> address;
    
    PersonViewModel() : address(std::make_shared<AddressViewModel>()) {
        // 转发子对象的变更通知
        address->PropertyChanged().Connect([this](std::string_view propName) {
            RaisePropertyChanged("Address." + std::string(propName));
        });
    }
};

// 绑定到嵌套属性
textBox->SetBinding(
    TextBox::TextProperty(),
    fk::bind("Address.City")
);
```

### 命令模式配合

```cpp
class MainViewModel : public fk::ObservableObject {
public:
    fk::ObservableProperty<std::string, MainViewModel> status{this, "Status", "就绪"};
    fk::ObservableProperty<bool, MainViewModel> isBusy{this, "IsBusy", false};
    
    void ExecuteCommand() {
        if (isBusy.Get()) return;
        
        isBusy = true;
        status = "处理中...";
        
        // 模拟异步操作
        std::thread([this]() {
            std::this_thread::sleep_for(std::chrono::seconds(2));
            status = "完成";
            isBusy = false;
        }).detach();
    }
};
```

### 数据验证

```cpp
class RegistrationViewModel : public fk::ObservableObject {
public:
    fk::ObservableProperty<std::string, RegistrationViewModel> email{this, "Email"};
    fk::ObservableProperty<std::string, RegistrationViewModel> password{this, "Password"};
    fk::ObservableProperty<std::string, RegistrationViewModel> errorMessage{this, "ErrorMessage"};
    fk::ObservableProperty<bool, RegistrationViewModel> isValid{this, "IsValid", false};
    
    RegistrationViewModel() {
        // 监听输入变更，自动验证
        PropertyChanged().Connect([this](std::string_view propName) {
            if (propName == "Email" || propName == "Password") {
                Validate();
            }
        });
    }
    
private:
    void Validate() {
        std::string error;
        bool valid = true;
        
        if (email.Get().empty()) {
            error = "请输入邮箱";
            valid = false;
        } else if (password.Get().length() < 6) {
            error = "密码至少6位";
            valid = false;
        }
        
        errorMessage = error;
        isValid = valid;
    }
};
```

---

## 与 INotifyPropertyChanged 的关系

```
┌─────────────────────────────────────────────────────────────┐
│           INotifyPropertyChanged（接口）                     │
│                                                             │
│  + PropertyChanged() : PropertyChangedEvent&                │
└─────────────────────────────────────────────────────────────┘
                              △
                              │ 实现
                              │
┌─────────────────────────────────────────────────────────────┐
│              ObservableObject（基类）                        │
│                                                             │
│  + PropertyChanged() override                               │
│  # RaisePropertyChanged(propertyName)                       │
│  - propertyChanged_ : PropertyChangedEvent                  │
└─────────────────────────────────────────────────────────────┘
                              △
                              │ 继承
                              │
┌─────────────────────────────────────────────────────────────┐
│              用户 ViewModel（派生类）                        │
│                                                             │
│  使用 RaisePropertyChanged() 或 ObservableProperty          │
└─────────────────────────────────────────────────────────────┘
```

---

## 最佳实践

1. **使用 ObservableProperty**：优先使用 `ObservableProperty<T, Owner>` 替代手动属性，减少样板代码

2. **属性名一致性**：确保 `RaisePropertyChanged()` 传入的属性名与绑定路径完全匹配

3. **避免循环通知**：在监听自身属性变更时，注意避免无限循环
   ```cpp
   // 错误示例 - 可能导致无限循环
   PropertyChanged().Connect([this](std::string_view) {
       RaisePropertyChanged("SomeProperty");  // 危险！
   });
   ```

4. **计算属性通知**：当依赖属性变更时，记得通知计算属性
   ```cpp
   void SetFirstName(const std::string& value) {
       // ...
       RaisePropertyChanged("FirstName");
       RaisePropertyChanged("FullName");  // 计算属性
   }
   ```

5. **线程安全**：如果从非 UI 线程更新属性，确保通过调度器回到 UI 线程

---

## 相关类

- [INotifyPropertyChanged](INotifyPropertyChanged.md) - 实现的接口
- [ObservableProperty](ObservableProperty.md) - 自动触发变更通知的属性包装器
- [Binding](Binding.md) - 数据绑定配置
- [BindingExpression](BindingExpression.md) - 绑定表达式，订阅 PropertyChanged 事件

## 另请参阅

- [数据绑定概述](../BINDING_MODULE_OVERVIEW.md)
- [入门指南](../../Guides/GettingStarted.md)
