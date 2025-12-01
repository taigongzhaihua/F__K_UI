# INotifyDataErrorInfo

## 概览

| 属性 | 值 |
|------|-----|
| **命名空间** | `fk::binding` |
| **头文件** | `fk/binding/INotifyDataErrorInfo.h` |
| **类型** | 纯虚接口 |

## 描述

`INotifyDataErrorInfo` 是数据验证错误通知的核心接口。实现此接口的类可以报告验证错误并在错误状态变更时通知绑定系统，从而实现数据验证与 UI 的自动同步。

**WPF 对应**：`System.ComponentModel.INotifyDataErrorInfo`

主要用途：
- 为数据绑定源提供验证错误通知机制
- 使 `BindingExpression` 能够订阅并响应验证错误变更
- 实现 MVVM 模式中的数据验证反馈
- 支持属性级别和对象级别的错误报告

---

## 接口定义

```cpp
namespace fk::binding {

class INotifyDataErrorInfo {
public:
    using ErrorsChangedEvent = core::Event<std::string_view>;

    virtual ~INotifyDataErrorInfo() = default;

    [[nodiscard]] virtual bool HasErrors() const = 0;
    [[nodiscard]] virtual std::vector<std::string> GetErrors(std::string_view propertyName) const = 0;
    virtual ErrorsChangedEvent& ErrorsChanged() = 0;
};

} // namespace fk::binding

namespace fk {
    using binding::INotifyDataErrorInfo;
}
```

---

## 类型别名

### ErrorsChangedEvent

```cpp
using ErrorsChangedEvent = core::Event<std::string_view>;
```

错误状态变更事件类型。

**参数类型**：`std::string_view` - 错误状态变更的属性名称（空字符串表示对象级别错误）

**说明**：
- 基于 `fk::core::Event` 模板
- 事件参数为属性名称，允许订阅者根据名称过滤感兴趣的属性
- 传递空字符串表示对象级别的验证错误

---

## 公共方法

### HasErrors

```cpp
[[nodiscard]] virtual bool HasErrors() const = 0;
```

检查对象是否存在任何验证错误。

**返回值**：`bool`
- `true` - 存在验证错误
- `false` - 没有验证错误

**说明**：
- 纯虚函数，必须由派生类实现
- 检查所有属性的错误状态
- 用于快速判断对象整体验证状态

**示例**：
```cpp
if (viewModel->HasErrors()) {
    std::cout << "存在验证错误，无法提交" << std::endl;
}
```

---

### GetErrors

```cpp
[[nodiscard]] virtual std::vector<std::string> GetErrors(std::string_view propertyName) const = 0;
```

获取指定属性的验证错误列表。

**参数**：
| 参数 | 类型 | 描述 |
|------|------|------|
| `propertyName` | `std::string_view` | 属性名称，空字符串获取对象级别错误 |

**返回值**：`std::vector<std::string>` - 错误消息列表，无错误时返回空向量

**说明**：
- 纯虚函数，必须由派生类实现
- 传递空字符串获取对象级别的验证错误
- 每个属性可以有多个错误消息

**示例**：
```cpp
// 获取特定属性的错误
auto errors = viewModel->GetErrors("Email");
for (const auto& error : errors) {
    std::cout << "错误: " << error << std::endl;
}

// 获取对象级别的错误
auto objectErrors = viewModel->GetErrors("");
```

---

### ErrorsChanged

```cpp
virtual ErrorsChangedEvent& ErrorsChanged() = 0;
```

获取错误状态变更事件。

**返回值**：`ErrorsChangedEvent&` - 错误变更事件的引用

**说明**：
- 纯虚函数，必须由派生类实现
- 当属性的错误状态变更时触发此事件
- 绑定系统订阅此事件以更新 UI 的验证状态显示

**示例**：
```cpp
// 订阅错误变更
auto connection = viewModel->ErrorsChanged().Connect([](std::string_view propName) {
    if (propName.empty()) {
        std::cout << "对象级别错误已变更" << std::endl;
    } else {
        std::cout << "属性 " << propName << " 的错误已变更" << std::endl;
    }
});
```

---

## 实现指南

### 基础实现

```cpp
class ValidatableViewModel : public fk::ObservableObject, 
                              public fk::INotifyDataErrorInfo {
public:
    // INotifyDataErrorInfo 实现
    bool HasErrors() const override {
        return !errors_.empty();
    }

    std::vector<std::string> GetErrors(std::string_view propertyName) const override {
        auto it = errors_.find(std::string(propertyName));
        if (it != errors_.end()) {
            return it->second;
        }
        return {};
    }

    ErrorsChangedEvent& ErrorsChanged() override {
        return errorsChanged_;
    }

protected:
    // 设置属性错误
    void SetErrors(const std::string& propertyName, 
                   const std::vector<std::string>& errors) {
        if (errors.empty()) {
            errors_.erase(propertyName);
        } else {
            errors_[propertyName] = errors;
        }
        errorsChanged_(propertyName);
    }

    // 清除属性错误
    void ClearErrors(const std::string& propertyName) {
        if (errors_.erase(propertyName) > 0) {
            errorsChanged_(propertyName);
        }
    }

    // 清除所有错误
    void ClearAllErrors() {
        auto keys = std::vector<std::string>();
        for (const auto& [key, _] : errors_) {
            keys.push_back(key);
        }
        errors_.clear();
        for (const auto& key : keys) {
            errorsChanged_(key);
        }
    }

private:
    std::map<std::string, std::vector<std::string>> errors_;
    ErrorsChangedEvent errorsChanged_;
};
```

### 带验证的 ViewModel

```cpp
class UserViewModel : public ValidatableViewModel {
public:
    fk::ObservableProperty<std::string, UserViewModel> email{this, "Email"};
    fk::ObservableProperty<int, UserViewModel> age{this, "Age", 0};

    UserViewModel() {
        // 订阅属性变更以触发验证
        PropertyChanged().Connect([this](std::string_view propName) {
            ValidateProperty(std::string(propName));
        });
    }

    void ValidateProperty(const std::string& propertyName) {
        std::vector<std::string> errors;

        if (propertyName == "Email") {
            const auto& value = email.Get();
            if (value.empty()) {
                errors.push_back("邮箱不能为空");
            } else if (value.find('@') == std::string::npos) {
                errors.push_back("请输入有效的邮箱地址");
            }
        }
        else if (propertyName == "Age") {
            int value = age.Get();
            if (value < 0) {
                errors.push_back("年龄不能为负数");
            } else if (value > 150) {
                errors.push_back("请输入有效的年龄");
            }
        }

        SetErrors(propertyName, errors);
    }

    bool ValidateAll() {
        ValidateProperty("Email");
        ValidateProperty("Age");
        return !HasErrors();
    }
};
```

---

## 验证流程

### 工作原理

```
┌─────────────────────────────────────────────────────────────┐
│                    用户输入 / 数据变更                        │
└─────────────────────────────────────────────────────────────┘
                               │
                               ▼
┌─────────────────────────────────────────────────────────────┐
│                  PropertyChanged 触发                        │
│              ViewModel 执行验证逻辑                          │
└─────────────────────────────────────────────────────────────┘
                               │
              ┌────────────────┴────────────────┐
              ▼                                 ▼
     ┌─────────────────┐               ┌─────────────────┐
     │   验证通过       │               │   验证失败       │
     │  ClearErrors()   │               │  SetErrors()    │
     └─────────────────┘               └─────────────────┘
              │                                 │
              └────────────────┬────────────────┘
                               ▼
┌─────────────────────────────────────────────────────────────┐
│                  ErrorsChanged 事件触发                       │
└─────────────────────────────────────────────────────────────┘
                               │
                               ▼
┌─────────────────────────────────────────────────────────────┐
│                    BindingExpression                         │
│              订阅 ErrorsChanged 事件                         │
│              更新验证错误显示                                │
└─────────────────────────────────────────────────────────────┘
                               │
                               ▼
┌─────────────────────────────────────────────────────────────┐
│                       UI 更新                                │
│            显示/隐藏错误提示、边框变红等                     │
└─────────────────────────────────────────────────────────────┘
```

### 与绑定系统的集成

```cpp
// BindingExpression 内部会订阅 ErrorsChanged 事件
void BindingExpression::SubscribeToValidation() {
    // 检查源是否实现 INotifyDataErrorInfo
    if (auto* errorInfo = GetSourceAs<INotifyDataErrorInfo>()) {
        validationConnection_ = errorInfo->ErrorsChanged().Connect(
            [this](std::string_view propName) {
                if (propName == path_ || propName.empty()) {
                    // 更新验证状态
                    UpdateValidationState();
                }
            });
    }
}
```

---

## 使用示例

### 基本验证绑定

```cpp
// ViewModel 定义
class LoginViewModel : public ValidatableViewModel {
public:
    fk::ObservableProperty<std::string, LoginViewModel> username{this, "Username"};
    fk::ObservableProperty<std::string, LoginViewModel> password{this, "Password"};

    LoginViewModel() {
        PropertyChanged().Connect([this](std::string_view propName) {
            Validate(std::string(propName));
        });
    }

    void Validate(const std::string& propName) {
        std::vector<std::string> errors;

        if (propName == "Username") {
            if (username.Get().empty()) {
                errors.push_back("用户名不能为空");
            } else if (username.Get().length() < 3) {
                errors.push_back("用户名至少3个字符");
            }
        }
        else if (propName == "Password") {
            if (password.Get().empty()) {
                errors.push_back("密码不能为空");
            } else if (password.Get().length() < 6) {
                errors.push_back("密码至少6个字符");
            }
        }

        SetErrors(propName, errors);
    }
};

// 创建和使用
auto viewModel = std::make_shared<LoginViewModel>();

// 监听验证错误
viewModel->ErrorsChanged().Connect([&viewModel](std::string_view propName) {
    auto errors = viewModel->GetErrors(propName);
    for (const auto& err : errors) {
        std::cout << propName << ": " << err << std::endl;
    }
});

// 触发验证
viewModel->username = "ab";  // 输出: Username: 用户名至少3个字符
viewModel->password = "123"; // 输出: Password: 密码至少6个字符
```

### 多错误支持

```cpp
class RegistrationViewModel : public ValidatableViewModel {
public:
    fk::ObservableProperty<std::string, RegistrationViewModel> password{this, "Password"};

    void ValidatePassword() {
        std::vector<std::string> errors;
        const auto& pwd = password.Get();

        if (pwd.length() < 8) {
            errors.push_back("密码至少8个字符");
        }
        if (pwd.find_first_of("0123456789") == std::string::npos) {
            errors.push_back("密码必须包含数字");
        }
        if (pwd.find_first_of("ABCDEFGHIJKLMNOPQRSTUVWXYZ") == std::string::npos) {
            errors.push_back("密码必须包含大写字母");
        }
        if (pwd.find_first_of("!@#$%^&*") == std::string::npos) {
            errors.push_back("密码必须包含特殊字符");
        }

        SetErrors("Password", errors);
    }
};

// 使用
auto vm = std::make_shared<RegistrationViewModel>();
vm->password = "abc";
auto errors = vm->GetErrors("Password");
// errors 包含多条错误消息
```

### 对象级别验证

```cpp
class OrderViewModel : public ValidatableViewModel {
public:
    fk::ObservableProperty<std::string, OrderViewModel> startDate{this, "StartDate"};
    fk::ObservableProperty<std::string, OrderViewModel> endDate{this, "EndDate"};

    void ValidateDateRange() {
        std::vector<std::string> objectErrors;

        // 对象级别验证：开始日期必须早于结束日期
        if (startDate.Get() > endDate.Get()) {
            objectErrors.push_back("开始日期必须早于结束日期");
        }

        // 使用空字符串表示对象级别错误
        SetErrors("", objectErrors);
    }
};

// 获取对象级别错误
auto objectErrors = viewModel->GetErrors("");
```

### 表单提交前验证

```cpp
class FormViewModel : public ValidatableViewModel {
public:
    fk::ObservableProperty<std::string, FormViewModel> name{this, "Name"};
    fk::ObservableProperty<std::string, FormViewModel> email{this, "Email"};

    bool CanSubmit() const {
        return !HasErrors() && !name.Get().empty() && !email.Get().empty();
    }

    void Submit() {
        // 验证所有字段
        ValidateProperty("Name");
        ValidateProperty("Email");

        if (HasErrors()) {
            std::cout << "请修正验证错误后再提交" << std::endl;
            return;
        }

        // 执行提交逻辑
        std::cout << "表单提交成功！" << std::endl;
    }

private:
    void ValidateProperty(const std::string& propName) {
        // 验证逻辑...
    }
};
```

---

## 与 ValidationRule 的区别

| 特性 | INotifyDataErrorInfo | ValidationRule |
|------|---------------------|----------------|
| **验证时机** | 属性变更后立即验证 | 绑定更新源时验证 |
| **验证位置** | ViewModel 内部 | Binding 配置中 |
| **适用场景** | 复杂业务规则验证 | 简单格式验证 |
| **多错误支持** | 原生支持多条错误 | 单条错误 |
| **对象级别验证** | 支持 | 不支持 |
| **复用性** | 在 ViewModel 中实现 | 可跨绑定复用 |

### 组合使用

```cpp
// 使用 ValidationRule 进行格式验证
auto binding = fk::bind("Email")
    .AddValidationRule(std::make_shared<fk::EmailValidationRule>());

// 使用 INotifyDataErrorInfo 进行业务验证
class UserViewModel : public ValidatableViewModel {
    void ValidateEmail() {
        std::vector<std::string> errors;
        
        // 业务规则：邮箱不能重复
        if (IsEmailRegistered(email.Get())) {
            errors.push_back("该邮箱已被注册");
        }
        
        SetErrors("Email", errors);
    }
};
```

---

## 最佳实践

1. **继承基类**：创建一个实现了 `INotifyDataErrorInfo` 的基类（如 `ValidatableViewModel`），避免重复代码

2. **属性变更时验证**：在 `PropertyChanged` 事件中自动触发验证，提供即时反馈

3. **支持多条错误**：每个属性可能有多种验证失败原因，返回完整的错误列表

4. **分离验证逻辑**：将验证规则提取到单独的方法或类中，提高可测试性

5. **对象级别验证**：对于跨属性的验证规则，使用空字符串作为属性名

6. **提交前全量验证**：在表单提交前调用完整验证，确保所有字段都通过验证

7. **清晰的错误消息**：提供用户友好的错误描述，帮助用户快速修正问题

---

## 相关类

- [ValidationRule](ValidationRule.md) - 绑定级别的验证规则
- [BindingExpression](BindingExpression.md) - 订阅并响应验证错误
- [ObservableObject](ObservableObject.md) - 属性变更通知基类
- [INotifyPropertyChanged](INotifyPropertyChanged.md) - 属性变更通知接口

## 另请参阅

- [数据绑定概述](BINDING_MODULE_OVERVIEW.md)
- [值转换器](ValueConverters.md)
