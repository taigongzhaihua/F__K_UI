# ValidationRule

数据验证规则基类和内置验证规则集合。

## 概览

| 项目 | 信息 |
|------|------|
| 命名空间 | `fk::binding`（别名 `fk`） |
| 头文件 | `fk/binding/ValidationRule.h` |
| 类型 | 抽象基类 + 具体实现类 |

---

## ValidationResult 结构体

验证操作的结果。

### 定义

```cpp
struct ValidationResult {
    bool isValid{true};
    std::string errorMessage{};

    static ValidationResult Success();
    static ValidationResult Error(std::string message);
};
```

### 成员

| 成员 | 类型 | 说明 |
|------|------|------|
| `isValid` | `bool` | 验证是否通过，默认为 `true` |
| `errorMessage` | `std::string` | 错误消息，验证失败时包含描述信息 |

### 静态方法

#### Success

```cpp
static ValidationResult Success();
```

创建成功的验证结果。

**返回值**：`isValid = true`，`errorMessage` 为空的 `ValidationResult`

#### Error

```cpp
static ValidationResult Error(std::string message);
```

创建失败的验证结果。

**参数**：
- `message`：错误描述消息

**返回值**：`isValid = false`，包含错误消息的 `ValidationResult`

---

## ValidationRule 基类

所有验证规则的抽象基类。

### 定义

```cpp
class ValidationRule {
public:
    virtual ~ValidationRule() = default;

    virtual ValidationResult Validate(const std::any& value) const = 0;
};
```

### 公共方法

#### Validate（纯虚方法）

```cpp
virtual ValidationResult Validate(const std::any& value) const = 0;
```

验证给定的值。

**参数**：
- `value`：要验证的值（`std::any` 类型）

**返回值**：`ValidationResult` 表示验证成功或失败

---

## 内置验证规则

### FunctionValidationRule

使用自定义函数的验证规则。

```cpp
class FunctionValidationRule : public ValidationRule {
public:
    using ValidatorFunc = std::function<ValidationResult(const std::any&)>;

    explicit FunctionValidationRule(ValidatorFunc validator);
    ValidationResult Validate(const std::any& value) const override;
};
```

**特点**：
- 接受 lambda 或函数对象作为验证器
- 灵活的内联验证逻辑

**示例**：
```cpp
auto rule = std::make_shared<FunctionValidationRule>(
    [](const std::any& value) -> ValidationResult {
        auto* str = std::any_cast<std::string>(&value);
        if (str && str->starts_with("FK_")) {
            return ValidationResult::Success();
        }
        return ValidationResult::Error("必须以 FK_ 开头");
    }
);
```

---

### NotEmptyValidationRule

验证值非空。

```cpp
class NotEmptyValidationRule : public ValidationRule {
public:
    explicit NotEmptyValidationRule(std::string errorMessage = "Value cannot be empty");
    ValidationResult Validate(const std::any& value) const override;
};
```

**构造函数参数**：
| 参数 | 类型 | 默认值 | 说明 |
|------|------|--------|------|
| `errorMessage` | `std::string` | `"Value cannot be empty"` | 验证失败时的错误消息 |

**验证逻辑**：
- `std::any` 没有值时返回错误
- 对于 `std::string` 类型，空字符串返回错误

**示例**：
```cpp
auto rule = std::make_shared<NotEmptyValidationRule>("用户名不能为空");

Binding binding("Username");
binding.ValidationRules({ rule });
```

---

### RangeValidationRule<T>

验证数值在指定范围内（模板类）。

```cpp
template<typename T>
class RangeValidationRule : public ValidationRule {
public:
    RangeValidationRule(T min, T max, std::string errorMessage = "");
    ValidationResult Validate(const std::any& value) const override;
};
```

**模板参数**：
- `T`：数值类型（如 `int`、`double`、`float` 等）

**构造函数参数**：
| 参数 | 类型 | 说明 |
|------|------|------|
| `min` | `T` | 最小允许值（包含） |
| `max` | `T` | 最大允许值（包含） |
| `errorMessage` | `std::string` | 可选的自定义错误消息 |

**验证逻辑**：
- 值为空时返回错误
- 类型不匹配时返回错误
- 值不在 `[min, max]` 范围内时返回错误

**示例**：
```cpp
// 年龄验证：0-150
auto ageRule = std::make_shared<RangeValidationRule<int>>(0, 150, "年龄必须在0-150之间");

// 价格验证：0.01-99999.99
auto priceRule = std::make_shared<RangeValidationRule<double>>(0.01, 99999.99);

Binding binding("Age");
binding.ValidationRules({ ageRule });
```

---

### StringLengthValidationRule

验证字符串长度。

```cpp
class StringLengthValidationRule : public ValidationRule {
public:
    StringLengthValidationRule(
        std::size_t minLength = 0,
        std::size_t maxLength = std::numeric_limits<std::size_t>::max(),
        std::string errorMessage = ""
    );
    ValidationResult Validate(const std::any& value) const override;
};
```

**构造函数参数**：
| 参数 | 类型 | 默认值 | 说明 |
|------|------|--------|------|
| `minLength` | `std::size_t` | `0` | 最小长度（包含） |
| `maxLength` | `std::size_t` | `max` | 最大长度（包含） |
| `errorMessage` | `std::string` | 自动生成 | 可选的自定义错误消息 |

**自动生成错误消息规则**：
- 同时指定最小和最大：`"String length must be between X and Y"`
- 仅指定最小：`"String length must be at least X"`
- 仅指定最大：`"String length must be at most Y"`

**示例**：
```cpp
// 密码长度：8-32 字符
auto passwordRule = std::make_shared<StringLengthValidationRule>(
    8, 32, "密码长度必须在8-32个字符之间"
);

// 用户名最少3个字符
auto usernameRule = std::make_shared<StringLengthValidationRule>(3);

Binding binding("Password");
binding.ValidationRules({ passwordRule });
```

---

### EmailValidationRule

验证电子邮件格式。

```cpp
class EmailValidationRule : public ValidationRule {
public:
    explicit EmailValidationRule(std::string errorMessage = "Invalid email format");
    ValidationResult Validate(const std::any& value) const override;
};
```

**构造函数参数**：
| 参数 | 类型 | 默认值 | 说明 |
|------|------|--------|------|
| `errorMessage` | `std::string` | `"Invalid email format"` | 验证失败时的错误消息 |

**验证逻辑**：
- 必须包含 `@` 符号
- `@` 不能在开头或结尾
- `@` 之后必须包含 `.`
- `.` 不能紧跟 `@` 或在结尾

**示例**：
```cpp
auto emailRule = std::make_shared<EmailValidationRule>("请输入有效的邮箱地址");

Binding binding("Email");
binding.ValidationRules({ emailRule });
```

---

## 验证流程图

```
┌─────────────────────────────────────────────────────────────┐
│                    绑定值更新                                │
└─────────────────────────────────────────────────────────────┘
                           │
                           ▼
              ┌────────────────────────┐
              │  遍历 ValidationRules  │
              └────────────────────────┘
                           │
           ┌───────────────┴───────────────┐
           ▼                               ▼
    ┌─────────────┐               ┌─────────────┐
    │  Rule 1     │               │  Rule N     │
    │  Validate() │      ...      │  Validate() │
    └─────────────┘               └─────────────┘
           │                               │
           ▼                               ▼
    ┌─────────────┐               ┌─────────────┐
    │ isValid?    │               │ isValid?    │
    └─────────────┘               └─────────────┘
           │                               │
    ┌──────┴──────┐                 ┌──────┴──────┐
    ▼             ▼                 ▼             ▼
  true         false              true         false
    │             │                 │             │
    │             ▼                 │             ▼
    │     收集错误消息              │      收集错误消息
    │             │                 │             │
    └──────┬──────┘                 └──────┬──────┘
           │                               │
           └───────────────┬───────────────┘
                           ▼
              ┌────────────────────────┐
              │  所有规则验证完成      │
              └────────────────────────┘
                           │
           ┌───────────────┴───────────────┐
           ▼                               ▼
    全部通过                           存在失败
        │                                  │
        ▼                                  ▼
    更新源属性                      触发 ValidationError 事件
                                   不更新源属性
```

---

## 使用示例

### 示例 1：组合多个验证规则

```cpp
Binding binding("Email");
binding.Source(viewModel)
       .ValidationRules({
           std::make_shared<NotEmptyValidationRule>("邮箱不能为空"),
           std::make_shared<EmailValidationRule>("请输入有效的邮箱格式"),
           std::make_shared<StringLengthValidationRule>(0, 100, "邮箱长度不能超过100字符")
       });

emailTextBox->SetValue(TextBox::TextProperty(), binding);
```

### 示例 2：自定义验证规则

```cpp
class PhoneNumberValidationRule : public ValidationRule {
public:
    explicit PhoneNumberValidationRule(std::string errorMessage = "无效的手机号码")
        : errorMessage_(std::move(errorMessage)) {}

    ValidationResult Validate(const std::any& value) const override {
        if (!value.has_value()) {
            return ValidationResult::Error(errorMessage_);
        }

        const auto* str = std::any_cast<std::string>(&value);
        if (!str) {
            return ValidationResult::Error("值必须是字符串");
        }

        // 中国手机号：11位数字，以1开头
        if (str->length() != 11 || (*str)[0] != '1') {
            return ValidationResult::Error(errorMessage_);
        }

        for (char c : *str) {
            if (!std::isdigit(c)) {
                return ValidationResult::Error(errorMessage_);
            }
        }

        return ValidationResult::Success();
    }

private:
    std::string errorMessage_;
};
```

### 示例 3：使用 FunctionValidationRule

```cpp
// 验证用户名只包含字母、数字和下划线
auto usernameRule = std::make_shared<FunctionValidationRule>(
    [](const std::any& value) -> ValidationResult {
        const auto* str = std::any_cast<std::string>(&value);
        if (!str) {
            return ValidationResult::Error("用户名必须是字符串");
        }
        
        for (char c : *str) {
            if (!std::isalnum(c) && c != '_') {
                return ValidationResult::Error("用户名只能包含字母、数字和下划线");
            }
        }
        
        return ValidationResult::Success();
    }
);

Binding binding("Username");
binding.ValidationRules({
    std::make_shared<NotEmptyValidationRule>("用户名不能为空"),
    std::make_shared<StringLengthValidationRule>(3, 20),
    usernameRule
});
```

### 示例 4：监听验证错误

```cpp
auto expr = textBox->GetBindingExpression(TextBox::TextProperty());
if (expr) {
    expr->ValidationError().connect([](const std::vector<ValidationResult>& errors) {
        for (const auto& error : errors) {
            if (!error.isValid) {
                std::cout << "验证错误: " << error.errorMessage << std::endl;
            }
        }
    });
}
```

---

## 内置规则对比表

| 规则类 | 用途 | 支持类型 | 构造参数 |
|--------|------|----------|----------|
| `NotEmptyValidationRule` | 非空验证 | 任意类型，对 `string` 特殊处理 | 错误消息 |
| `RangeValidationRule<T>` | 数值范围 | 数值类型（int, double 等） | min, max, 错误消息 |
| `StringLengthValidationRule` | 字符串长度 | `std::string` | minLength, maxLength, 错误消息 |
| `EmailValidationRule` | 邮箱格式 | `std::string` | 错误消息 |
| `FunctionValidationRule` | 自定义逻辑 | 任意类型 | 验证函数 |

---

## 最佳实践

### 1. 合理的错误消息

```cpp
// ✓ 提供清晰的错误消息
std::make_shared<RangeValidationRule<int>>(18, 120, "年龄必须在18-120岁之间")

// ✗ 使用默认消息可能不够友好
std::make_shared<RangeValidationRule<int>>(18, 120)  // "Value must be between 18 and 120"
```

### 2. 验证规则顺序

```cpp
// ✓ 先验证必填，再验证格式
binding.ValidationRules({
    std::make_shared<NotEmptyValidationRule>("该字段为必填"),
    std::make_shared<EmailValidationRule>("邮箱格式不正确")
});

// ✗ 顺序不当可能导致不必要的验证
binding.ValidationRules({
    std::make_shared<EmailValidationRule>("邮箱格式不正确"),
    std::make_shared<NotEmptyValidationRule>("该字段为必填")
});
```

### 3. 复用验证规则

```cpp
// 创建共享的验证规则实例
auto emailRule = std::make_shared<EmailValidationRule>("请输入有效邮箱");

// 在多个绑定中复用
binding1.ValidationRules({ emailRule });
binding2.ValidationRules({ emailRule });
```

---

## 相关类

- [Binding](Binding.md) - 绑定类，配置验证规则
- [BindingExpression](BindingExpression.md) - 提供 ValidationError 事件
- [INotifyDataErrorInfo](INotifyDataErrorInfo.md) - 数据错误通知接口
