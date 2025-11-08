# ValueConverters - 值类型转换器

## 概述

`ValueConverters` 模块提供了值类型转换的接口和常用实现，支持绑定系统中源值和目标值的格式转换（如 bool 到 string、string 到 int 等）。

## 核心接口

### IValueConverter

```cpp
class IValueConverter {
public:
    virtual ~IValueConverter() = default;
    
    virtual std::any Convert(
        const std::any& value,
        std::type_index targetType,
        const std::any* parameter
    ) const = 0;
    
    virtual std::any ConvertBack(
        const std::any& value,
        std::type_index sourceType,
        const std::any* parameter
    ) const = 0;
};
```

**说明：**
- `Convert()`: 将源值转换为目标类型（绑定：源 → UI）
- `ConvertBack()`: 将目标值转换回源类型（绑定：UI → 源）
- `parameter`: 可选的转换参数

## 内置实现

### DefaultValueConverter - 默认转换器

```cpp
class DefaultValueConverter : public IValueConverter {
public:
    std::any Convert(const std::any& value, std::type_index targetType, const std::any* parameter) const override;
    std::any ConvertBack(const std::any& value, std::type_index sourceType, const std::any* parameter) const override;
    static std::shared_ptr<DefaultValueConverter> Instance();
};
```

**支持转换：**
- 字符串与数值类型互转
- 字符串与布尔值互转
- 类型之间的合理转换

**示例：**
```cpp
auto converter = fk::binding::DefaultValueConverter::Instance();

// 字符串 → int
std::any intValue = converter->Convert(
    std::string("123"),
    std::type_index(typeid(int)),
    nullptr
);
auto num = std::any_cast<int>(intValue);  // 123

// int → 字符串
std::any strValue = converter->ConvertBack(
    123,
    std::type_index(typeid(std::string)),
    nullptr
);
auto str = std::any_cast<std::string>(strValue);  // "123"
```

### BooleanToStringConverter - 布尔值转字符串

```cpp
class BooleanToStringConverter : public IValueConverter {
public:
    BooleanToStringConverter(
        std::string trueString = "True",
        std::string falseString = "False"
    );
    
    void SetTrueString(std::string value);
    void SetFalseString(std::string value);
    
    std::any Convert(const std::any& value, std::type_index targetType, const std::any* parameter) const override;
    std::any ConvertBack(const std::any& value, std::type_index sourceType, const std::any* parameter) const override;
};
```

**用途：** 将 bool 值显示为可读的文本（如 "是/否"、"启用/禁用"）

**示例：**
```cpp
auto converter = std::make_shared<fk::binding::BooleanToStringConverter>(
    "已启用",
    "已禁用"
);

// true → "已启用"
auto str = std::any_cast<std::string>(
    converter->Convert(true, std::type_index(typeid(std::string)), nullptr)
);

// "已启用" → true
auto flag = std::any_cast<bool>(
    converter->ConvertBack(str, std::type_index(typeid(bool)), nullptr)
);
```

## 常见模式

### 1. 整数到百分比字符串

```cpp
class PercentageConverter : public fk::binding::IValueConverter {
public:
    std::any Convert(const std::any& value, std::type_index targetType, const std::any* parameter) const override {
        if (value.type() != typeid(int)) return "";
        int percent = std::any_cast<int>(value);
        return std::string(std::to_string(percent) + "%");
    }
    
    std::any ConvertBack(const std::any& value, std::type_index sourceType, const std::any* parameter) const override {
        // 从字符串解析百分比
        if (value.type() != typeid(std::string)) return 0;
        std::string str = std::any_cast<std::string>(value);
        // 移除 % 并解析
        return std::stoi(str);
    }
};
```

### 2. 可见性转换

```cpp
class VisibilityConverter : public fk::binding::IValueConverter {
public:
    std::any Convert(const std::any& value, std::type_index targetType, const std::any* parameter) const override {
        if (value.type() != typeid(bool)) return fk::ui::Visibility::Collapsed;
        bool visible = std::any_cast<bool>(value);
        return visible ? fk::ui::Visibility::Visible : fk::ui::Visibility::Collapsed;
    }
    
    std::any ConvertBack(const std::any& value, std::type_index sourceType, const std::any* parameter) const override {
        if (value.type() != typeid(fk::ui::Visibility)) return false;
        auto vis = std::any_cast<fk::ui::Visibility>(value);
        return vis == fk::ui::Visibility::Visible;
    }
};
```

### 3. 参数化转换器

```cpp
class MultiplyConverter : public fk::binding::IValueConverter {
public:
    std::any Convert(const std::any& value, std::type_index targetType, const std::any* parameter) const override {
        if (!parameter || parameter->type() != typeid(float)) return value;
        
        float multiplier = std::any_cast<float>(*parameter);
        
        if (value.type() == typeid(int)) {
            return static_cast<int>(std::any_cast<int>(value) * multiplier);
        } else if (value.type() == typeid(float)) {
            return std::any_cast<float>(value) * multiplier;
        }
        return value;
    }
    
    std::any ConvertBack(const std::any& value, std::type_index sourceType, const std::any* parameter) const override {
        // 类似逻辑，用 1/multiplier
        return value;
    }
};
```

### 4. 在绑定中使用

```cpp
auto viewModel = std::make_shared<MyViewModel>();
auto label = fk::ui::TextBlock::Create();
label->SetDataContext(viewModel);

// 创建转换器
auto converter = std::make_shared<PercentageConverter>();

// 创建绑定并设置转换器
auto binding = std::make_shared<fk::binding::Binding>();
binding->SetPath("Progress");
binding->SetConverter(converter);

label->SetBinding(fk::ui::TextBlock::TextProperty, binding);
```

## 最佳实践

### 1. 实现 ConvertBack 的互逆性

```cpp
// ✅ 推荐：能够往返转换
std::any Convert(const std::any& value, ...){
    int n = std::any_cast<int>(value);
    return std::string(std::to_string(n));
}

std::any ConvertBack(const std::any& value, ...){
    std::string s = std::any_cast<std::string>(value);
    return std::stoi(s);
}

// ❌ 避免：转换不可逆
std::any Convert(const std::any& value, ...){
    return true;  // 信息丢失
}

std::any ConvertBack(const std::any& value, ...){
    return nullptr;  // 无法恢复
}
```

### 2. 处理类型不匹配

```cpp
// ✅ 推荐
std::any Convert(const std::any& value, ...) const override {
    if (value.type() != typeid(int)) {
        return DefaultValue;  // 返回默认值
    }
    // ... 实际转换
}

// ❌ 避免：直接 any_cast（可能抛异常）
std::any Convert(const std::any& value, ...) const override {
    return std::any_cast<int>(value);  // 可能抛异常
}
```

### 3. 参数验证

```cpp
// ✅ 推荐
std::any Convert(const std::any& value, ..., const std::any* parameter) const override {
    if (!parameter) {
        return DefaultValue;
    }
    if (parameter->type() != typeid(float)) {
        return DefaultValue;
    }
    // ... 使用参数
}
```

## 常见问题

### Q: 转换失败时应返回什么？
**A:** 建议返回空值或默认值，而不是抛异常，让绑定系统处理。

### Q: 何时使用 Convert vs ConvertBack？
**A:**
- **Convert**: 源 → 目标（正向，绑定源到 UI）
- **ConvertBack**: 目标 → 源（反向，UI 回写到源）

### Q: 如何传递参数给转换器？
**A:** 通过 Binding 的 ConverterParameter 属性或 Convert/ConvertBack 的 parameter 参数。

### Q: 可以链式使用多个转换器吗？
**A:** 不直接支持，但可以创建复合转换器实现。

## 相关文档

- [Binding.md](./Binding.md) - 绑定系统
- [BindingExpression.md](./BindingExpression.md) - 绑定表达式执行引擎
