# ValidationResult

## 概览

| 属性 | 值 |
|------|-----|
| **命名空间** | `fk::binding` |
| **头文件** | `fk/binding/ValidationRule.h` |
| **类型** | 结构体 |

## 描述

`ValidationResult` 是一个轻量级结构体，表示验证操作的结果。它包含两个信息：

- **验证是否通过**：布尔值指示验证成功或失败
- **错误消息**：当验证失败时提供的描述性错误信息

该结构体被 `ValidationRule` 及其所有子类使用，作为 `Validate` 方法的返回值。

---

## 结构体定义

```cpp
namespace fk::binding {

struct ValidationResult {
    bool isValid{true};
    std::string errorMessage{};

    static ValidationResult Valid();
    static ValidationResult Invalid(std::string message);
};

} // namespace fk::binding
```

---

## 数据成员

### isValid

```cpp
bool isValid{true};
```

指示验证是否通过。

| 值 | 描述 |
|----|------|
| `true` | 验证通过，值有效 |
| `false` | 验证失败，值无效 |

**默认值**：`true`

**示例**：
```cpp
fk::binding::ValidationResult result;
if (result.isValid) {
    std::cout << "验证通过" << std::endl;
} else {
    std::cout << "验证失败: " << result.errorMessage << std::endl;
}
```

### errorMessage

```cpp
std::string errorMessage{};
```

验证失败时的错误描述信息。

**默认值**：空字符串

**说明**：
- 当 `isValid == true` 时，此字段通常为空
- 当 `isValid == false` 时，此字段应包含有意义的错误描述

**示例**：
```cpp
auto result = fk::binding::ValidationResult::Invalid("用户名不能为空");
std::cout << result.errorMessage << std::endl;  // "用户名不能为空"
```

---

## 静态工厂方法

### Valid

```cpp
static ValidationResult Valid();
```

创建表示验证通过的结果。

**返回值**：`isValid == true`，`errorMessage` 为空的 `ValidationResult`

**示例**：
```cpp
auto result = fk::binding::ValidationResult::Valid();
std::cout << result.isValid << std::endl;       // true
std::cout << result.errorMessage << std::endl;  // ""
```

### Invalid

```cpp
static ValidationResult Invalid(std::string message);
```

创建表示验证失败的结果。

**参数**：
| 参数 | 类型 | 描述 |
|------|------|------|
| `message` | `std::string` | 错误描述信息 |

**返回值**：`isValid == false`，`errorMessage` 为指定消息的 `ValidationResult`

**示例**：
```cpp
auto result = fk::binding::ValidationResult::Invalid("邮箱格式不正确");
std::cout << result.isValid << std::endl;       // false
std::cout << result.errorMessage << std::endl;  // "邮箱格式不正确"
```

---

## 使用示例

### 示例 1：在 ValidationRule 中使用

```cpp
class AgeValidationRule : public fk::binding::ValidationRule {
public:
    fk::binding::ValidationResult Validate(const std::any& value) override {
        try {
            int age = std::any_cast<int>(value);
            
            if (age < 0) {
                return fk::binding::ValidationResult::Invalid("年龄不能为负数");
            }
            if (age > 150) {
                return fk::binding::ValidationResult::Invalid("年龄超出合理范围");
            }
            
            return fk::binding::ValidationResult::Valid();
        } catch (const std::bad_any_cast&) {
            return fk::binding::ValidationResult::Invalid("无效的年龄类型");
        }
    }
};
```

### 示例 2：检查验证结果

```cpp
void ProcessInput(const std::string& input, fk::binding::ValidationRule& rule) {
    auto result = rule.Validate(std::any(input));
    
    if (result.isValid) {
        std::cout << "✓ 输入有效，继续处理..." << std::endl;
        // 处理有效输入
    } else {
        std::cout << "✗ 验证失败: " << result.errorMessage << std::endl;
        // 显示错误给用户
    }
}
```

### 示例 3：链式验证

```cpp
fk::binding::ValidationResult ValidateUser(const User& user) {
    // 验证用户名
    if (user.name.empty()) {
        return fk::binding::ValidationResult::Invalid("用户名不能为空");
    }
    
    // 验证邮箱
    if (user.email.find('@') == std::string::npos) {
        return fk::binding::ValidationResult::Invalid("邮箱格式不正确");
    }
    
    // 验证年龄
    if (user.age < 18) {
        return fk::binding::ValidationResult::Invalid("用户必须年满18岁");
    }
    
    // 所有验证通过
    return fk::binding::ValidationResult::Valid();
}
```

### 示例 4：收集多个验证错误

```cpp
std::vector<std::string> ValidateForm(const FormData& form) {
    std::vector<std::string> errors;
    
    auto nameResult = nameRule.Validate(std::any(form.name));
    if (!nameResult.isValid) {
        errors.push_back(nameResult.errorMessage);
    }
    
    auto emailResult = emailRule.Validate(std::any(form.email));
    if (!emailResult.isValid) {
        errors.push_back(emailResult.errorMessage);
    }
    
    auto passwordResult = passwordRule.Validate(std::any(form.password));
    if (!passwordResult.isValid) {
        errors.push_back(passwordResult.errorMessage);
    }
    
    return errors;
}
```

### 示例 5：与数据绑定集成

```cpp
// 配置带验证的绑定
auto binding = fk::Binding()
    .Path("Email")
    .Mode(fk::BindingMode::TwoWay)
    .AddValidationRule(std::make_shared<fk::binding::EmailValidationRule>());

// 绑定表达式会自动执行验证
// 当验证失败时，触发 ValidationError 事件
bindingExpression.ValidationError += [](const fk::binding::ValidationResult& result) {
    if (!result.isValid) {
        ShowErrorToUser(result.errorMessage);
    }
};
```

---

## 验证流程

```
┌─────────────────────────────────┐
│      用户输入 / 数据变更         │
└─────────────────────────────────┘
                │
                ▼
┌─────────────────────────────────┐
│     ValidationRule.Validate()    │
│         验证逻辑执行             │
└─────────────────────────────────┘
                │
        ┌───────┴───────┐
        │               │
        ▼               ▼
   验证通过         验证失败
        │               │
        ▼               ▼
┌───────────────┐ ┌───────────────┐
│ Valid()       │ │ Invalid(msg)  │
│ isValid=true  │ │ isValid=false │
│ errorMessage=""│ │ errorMessage= │
│               │ │   错误信息    │
└───────────────┘ └───────────────┘
        │               │
        └───────┬───────┘
                │
                ▼
┌─────────────────────────────────┐
│   检查 result.isValid           │
│   处理验证结果                   │
└─────────────────────────────────┘
```

---

## 工厂方法 vs 直接构造

| 方式 | 代码 | 推荐度 |
|------|------|--------|
| 工厂方法（推荐） | `ValidationResult::Valid()` | ⭐⭐⭐ |
| 工厂方法（推荐） | `ValidationResult::Invalid("错误")` | ⭐⭐⭐ |
| 直接构造 | `ValidationResult{true, ""}` | ⭐ |
| 直接构造 | `ValidationResult{false, "错误"}` | ⭐ |

**推荐使用工厂方法的原因**：
- 代码更具可读性
- 意图更明确
- 减少错误（如忘记设置 `isValid`）

---

## 最佳实践

1. **使用工厂方法**：优先使用 `Valid()` 和 `Invalid()` 工厂方法

2. **提供有意义的错误消息**：错误消息应清晰描述问题
   ```cpp
   // 好
   Invalid("密码长度必须在 8-20 个字符之间")
   
   // 不好
   Invalid("无效")
   ```

3. **本地化考虑**：如需支持多语言，可使用本地化系统获取错误消息
   ```cpp
   Invalid(Localization::Get("error.password.length"))
   ```

4. **一致的错误格式**：在项目中保持错误消息格式一致

5. **及早验证**：在数据流的早期阶段进行验证，避免无效数据传播

---

## 相关类

- [ValidationRule](ValidationRule.md) - 验证规则基类，使用 ValidationResult 作为返回值
- [Binding](Binding.md) - 支持添加验证规则
- [BindingExpression](BindingExpression.md) - 执行验证并触发错误事件
- [INotifyDataErrorInfo](INotifyDataErrorInfo.md) - 异步验证接口

## 另请参阅

- [数据绑定概述](../BINDING_MODULE_OVERVIEW.md)
