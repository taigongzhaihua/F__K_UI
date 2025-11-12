# ValidationRule

## 概览

**目的**：数据验证规则基类

**命名空间**：`fk::binding`

**头文件**：`fk/binding/ValidationRule.h`

## 描述

`ValidationRule` 是创建自定义验证规则的基类，用于验证绑定数据的有效性。

## 公共接口

### 验证方法

#### Validate
```cpp
virtual ValidationResult Validate(const std::any& value) = 0;
```

验证给定的值，返回验证结果。

## 内置验证规则

### NotEmptyValidationRule
验证字符串非空。

### RangeValidationRule
验证数值在指定范围内。

### StringLengthValidationRule
验证字符串长度。

### EmailValidationRule
验证电子邮件格式。

## 使用示例

### 自定义验证规则
```cpp
class AgeValidationRule : public ValidationRule {
public:
    ValidationResult Validate(const std::any& value) override {
        try {
            int age = std::any_cast<int>(value);
            if (age < 0 || age > 150) {
                return ValidationResult::Error("年龄必须在0-150之间");
            }
            return ValidationResult::Valid();
        } catch (...) {
            return ValidationResult::Error("无效的年龄值");
        }
    }
};
```

### 在绑定中使用
```cpp
Binding binding("Age");
binding.Source(viewModel)
       .ValidationRules({
           std::make_shared<AgeValidationRule>()
       });

textBox->SetValue(TextBox::TextProperty(), binding);
```

### 使用内置规则
```cpp
Binding binding("Email");
binding.ValidationRules({
    std::make_shared<EmailValidationRule>()
});
```

## 相关类

- [Binding](Binding.md)
- [INotifyDataErrorInfo](INotifyDataErrorInfo.md)
