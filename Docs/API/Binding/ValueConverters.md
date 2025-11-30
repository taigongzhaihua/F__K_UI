# ValueConverters（值转换器）

值转换器模块提供了在数据绑定过程中进行值类型转换的接口和实现。

---

## 概览

| 属性 | 值 |
|------|-----|
| **命名空间** | `fk::binding` |
| **头文件** | `fk/binding/Binding.h`（接口）<br>`fk/binding/MultiBinding.h`（多值接口）<br>`fk/binding/ValueConverters.h`（实现） |
| **源文件** | `src/binding/ValueConverters.cpp` |

---

## 类和接口列表

本文档包含以下类型：

| 类型 | 描述 |
|------|------|
| `IValueConverter` | 单值转换器接口 |
| `IMultiValueConverter` | 多值转换器接口 |
| `DefaultValueConverter` | 默认类型转换器实现 |
| `BooleanToStringConverter` | 布尔值到字符串转换器 |

---

# IValueConverter

单值转换器接口，用于在绑定过程中转换单个源值到目标值。

## 接口定义

```cpp
class IValueConverter {
public:
    virtual ~IValueConverter() = default;
    
    virtual std::any Convert(
        const std::any& value, 
        std::type_index targetType, 
        const std::any* parameter) const = 0;
    
    virtual std::any ConvertBack(
        const std::any& value, 
        std::type_index sourceType, 
        const std::any* parameter) const = 0;
};
```

## 方法

### Convert

将源值转换为目标类型。

```cpp
virtual std::any Convert(
    const std::any& value, 
    std::type_index targetType, 
    const std::any* parameter) const = 0;
```

**参数：**

| 参数 | 类型 | 描述 |
|------|------|------|
| `value` | `const std::any&` | 源属性的值 |
| `targetType` | `std::type_index` | 目标属性的类型 |
| `parameter` | `const std::any*` | 可选的转换器参数，可能为 `nullptr` |

**返回值：** `std::any` - 转换后的值，用于设置到目标属性

---

### ConvertBack

将目标值转换回源类型（用于双向绑定）。

```cpp
virtual std::any ConvertBack(
    const std::any& value, 
    std::type_index sourceType, 
    const std::any* parameter) const = 0;
```

**参数：**

| 参数 | 类型 | 描述 |
|------|------|------|
| `value` | `const std::any&` | 目标属性的值 |
| `sourceType` | `std::type_index` | 源属性的类型 |
| `parameter` | `const std::any*` | 可选的转换器参数，可能为 `nullptr` |

**返回值：** `std::any` - 转换后的值，用于写回源属性

---

## 实现指南

要创建自定义转换器，继承 `IValueConverter` 并实现两个纯虚函数：

```cpp
class MyCustomConverter : public IValueConverter {
public:
    std::any Convert(const std::any& value, std::type_index targetType, 
                     const std::any* parameter) const override {
        // 从 value 提取实际值
        if (value.type() == typeid(int)) {
            int intValue = std::any_cast<int>(value);
            // 执行转换逻辑
            return std::to_string(intValue);
        }
        return value; // 无法转换时返回原值
    }
    
    std::any ConvertBack(const std::any& value, std::type_index sourceType,
                         const std::any* parameter) const override {
        // 反向转换（双向绑定需要）
        if (value.type() == typeid(std::string)) {
            std::string strValue = std::any_cast<std::string>(value);
            return std::stoi(strValue);
        }
        return value;
    }
};
```

---

# IMultiValueConverter

多值转换器接口，用于将多个源值组合转换为单个目标值。

## 接口定义

```cpp
class IMultiValueConverter {
public:
    virtual ~IMultiValueConverter() = default;

    virtual std::any Convert(
        const std::vector<std::any>& values,
        std::type_index targetType,
        const std::any* parameter) const = 0;

    virtual std::vector<std::any> ConvertBack(
        const std::any& value,
        const std::vector<std::type_index>& sourceTypes,
        const std::any* parameter) const;
};
```

## 方法

### Convert

将多个源值组合转换为单个目标值。

```cpp
virtual std::any Convert(
    const std::vector<std::any>& values,
    std::type_index targetType,
    const std::any* parameter) const = 0;
```

**参数：**

| 参数 | 类型 | 描述 |
|------|------|------|
| `values` | `const std::vector<std::any>&` | 多个源属性的值 |
| `targetType` | `std::type_index` | 目标属性的类型 |
| `parameter` | `const std::any*` | 可选的转换器参数 |

**返回值：** `std::any` - 组合转换后的单个值

---

### ConvertBack

将目标值分解转换回多个源值（可选实现）。

```cpp
virtual std::vector<std::any> ConvertBack(
    const std::any& value,
    const std::vector<std::type_index>& sourceTypes,
    const std::any* parameter) const;
```

**参数：**

| 参数 | 类型 | 描述 |
|------|------|------|
| `value` | `const std::any&` | 目标属性的值 |
| `sourceTypes` | `const std::vector<std::type_index>&` | 各源属性的类型列表 |
| `parameter` | `const std::any*` | 可选的转换器参数 |

**返回值：** `std::vector<std::any>` - 分解后的多个值，对应各源属性

**默认实现：** 返回空向量（表示不支持反向转换）

---

## 实现示例

```cpp
// 组合姓名的多值转换器
class FullNameConverter : public IMultiValueConverter {
public:
    std::any Convert(const std::vector<std::any>& values,
                     std::type_index targetType,
                     const std::any* parameter) const override {
        if (values.size() >= 2) {
            std::string firstName = std::any_cast<std::string>(values[0]);
            std::string lastName = std::any_cast<std::string>(values[1]);
            return firstName + " " + lastName;
        }
        return std::string{};
    }
    
    std::vector<std::any> ConvertBack(const std::any& value,
                                       const std::vector<std::type_index>& sourceTypes,
                                       const std::any* parameter) const override {
        // 可选：将全名拆分回姓和名
        std::string fullName = std::any_cast<std::string>(value);
        auto pos = fullName.find(' ');
        if (pos != std::string::npos) {
            return {
                fullName.substr(0, pos),
                fullName.substr(pos + 1)
            };
        }
        return {fullName, std::string{}};
    }
};
```

---

# DefaultValueConverter

默认的类型转换器实现，使用框架内置规则进行常见类型之间的自动转换。

## 类定义

```cpp
class DefaultValueConverter : public IValueConverter {
public:
    DefaultValueConverter() = default;
    ~DefaultValueConverter() override = default;

    std::any Convert(const std::any& value, std::type_index targetType, 
                     const std::any* parameter) const override;
    std::any ConvertBack(const std::any& value, std::type_index sourceType, 
                         const std::any* parameter) const override;

    static std::shared_ptr<DefaultValueConverter> Instance();
};
```

## 方法

### Instance

获取单例实例，方便重复使用。

```cpp
static std::shared_ptr<DefaultValueConverter> Instance();
```

**返回值：** `std::shared_ptr<DefaultValueConverter>` - 共享的默认转换器实例

---

### Convert / ConvertBack

使用内置规则进行类型转换。

**支持的转换：**

| 源类型 | 目标类型 | 说明 |
|--------|----------|------|
| `int` | `std::string` | 整数转字符串 |
| `double` | `std::string` | 浮点数转字符串 |
| `bool` | `std::string` | 布尔转 "true"/"false" |
| `std::string` | `int` | 字符串解析为整数 |
| `std::string` | `double` | 字符串解析为浮点数 |
| `std::string` | `bool` | "true"/"1" 转 true |
| `int` | `double` | 整数转浮点 |
| `double` | `int` | 浮点截断为整数 |

---

## 辅助函数

### TryDefaultConvert

底层转换函数，供内部使用。

```cpp
bool TryDefaultConvert(const std::any& value, std::type_index targetType, std::any& outResult);
```

**参数：**

| 参数 | 类型 | 描述 |
|------|------|------|
| `value` | `const std::any&` | 原始值 |
| `targetType` | `std::type_index` | 目标类型 |
| `outResult` | `std::any&` | 输出的转换结果 |

**返回值：** `bool` - `true` 转换成功，`false` 无法转换

---

## 使用示例

```cpp
// 使用单例实例
auto converter = DefaultValueConverter::Instance();

// 在绑定中使用（通常不需要显式指定，绑定系统自动使用）
Binding binding("Age");
binding.Converter(DefaultValueConverter::Instance());
```

---

# BooleanToStringConverter

将布尔值转换为自定义字符串的转换器。

## 类定义

```cpp
class BooleanToStringConverter : public IValueConverter {
public:
    BooleanToStringConverter(std::string trueString = "True", 
                             std::string falseString = "False");

    void SetTrueString(std::string value);
    void SetFalseString(std::string value);

    std::any Convert(const std::any& value, std::type_index targetType, 
                     const std::any* parameter) const override;
    std::any ConvertBack(const std::any& value, std::type_index sourceType, 
                         const std::any* parameter) const override;

private:
    std::string trueString_;
    std::string falseString_;
};
```

## 构造函数

```cpp
BooleanToStringConverter(std::string trueString = "True", 
                         std::string falseString = "False");
```

**参数：**

| 参数 | 类型 | 默认值 | 描述 |
|------|------|--------|------|
| `trueString` | `std::string` | `"True"` | `true` 对应的字符串 |
| `falseString` | `std::string` | `"False"` | `false` 对应的字符串 |

---

## 方法

### SetTrueString

设置 `true` 值对应的字符串。

```cpp
void SetTrueString(std::string value);
```

---

### SetFalseString

设置 `false` 值对应的字符串。

```cpp
void SetFalseString(std::string value);
```

---

### Convert

将布尔值转换为字符串。

```cpp
std::any Convert(const std::any& value, std::type_index targetType, 
                 const std::any* parameter) const override;
```

**转换规则：**
- `true` → `trueString_`
- `false` → `falseString_`

---

### ConvertBack

将字符串转换回布尔值。

```cpp
std::any ConvertBack(const std::any& value, std::type_index sourceType, 
                     const std::any* parameter) const override;
```

**转换规则：**
- 匹配 `trueString_` → `true`
- 其他 → `false`

---

## 私有成员

| 成员 | 类型 | 描述 |
|------|------|------|
| `trueString_` | `std::string` | true 对应的字符串 |
| `falseString_` | `std::string` | false 对应的字符串 |

---

# 使用示例

## 基本单值转换器

```cpp
#include <fk/binding/Binding.h>
#include <fk/binding/ValueConverters.h>

// 创建布尔到字符串转换器
auto converter = std::make_shared<BooleanToStringConverter>("是", "否");

// 设置绑定
Binding binding("IsActive");
binding.Source(viewModel)
       .Converter(converter);

label->SetBinding(Label::TextProperty(), binding);
// IsActive=true 显示 "是"
// IsActive=false 显示 "否"
```

## 带参数的转换器

```cpp
// 使用 ConverterParameter
class StatusConverter : public IValueConverter {
public:
    std::any Convert(const std::any& value, std::type_index targetType, 
                     const std::any* parameter) const override {
        bool status = std::any_cast<bool>(value);
        
        // 读取参数
        if (parameter && parameter->has_value()) {
            auto prefix = std::any_cast<std::string>(*parameter);
            return prefix + (status ? "在线" : "离线");
        }
        return status ? "在线" : "离线";
    }
    
    std::any ConvertBack(const std::any& value, std::type_index sourceType, 
                         const std::any* parameter) const override {
        std::string str = std::any_cast<std::string>(value);
        return str.find("在线") != std::string::npos;
    }
};

// 使用
Binding binding("IsOnline");
binding.Converter(std::make_shared<StatusConverter>())
       .ConverterParameter(std::string("状态: "));
```

## 多值转换器

```cpp
#include <fk/binding/MultiBinding.h>

// 计算总价转换器
class TotalPriceConverter : public IMultiValueConverter {
public:
    std::any Convert(const std::vector<std::any>& values,
                     std::type_index targetType,
                     const std::any* parameter) const override {
        if (values.size() >= 2) {
            double price = std::any_cast<double>(values[0]);
            int quantity = std::any_cast<int>(values[1]);
            double total = price * quantity;
            
            // 格式化为货币字符串
            std::ostringstream oss;
            oss << std::fixed << std::setprecision(2) << "¥" << total;
            return oss.str();
        }
        return std::string("¥0.00");
    }
};

// 使用多值绑定
MultiBinding multi;
multi.AddBinding(Binding().Path("UnitPrice").Source(product));
multi.AddBinding(Binding().Path("Quantity").Source(cart));
multi.Converter(std::make_shared<TotalPriceConverter>());

totalLabel->SetBinding(Label::TextProperty(), multi);
```

## 条件显示转换器

```cpp
// 根据多个条件决定显示内容
class ConditionalDisplayConverter : public IMultiValueConverter {
public:
    std::any Convert(const std::vector<std::any>& values,
                     std::type_index targetType,
                     const std::any* parameter) const override {
        // values[0]: 是否登录
        // values[1]: 用户名
        // values[2]: 是否VIP
        
        bool isLoggedIn = std::any_cast<bool>(values[0]);
        
        if (!isLoggedIn) {
            return std::string("请登录");
        }
        
        std::string username = std::any_cast<std::string>(values[1]);
        bool isVip = std::any_cast<bool>(values[2]);
        
        if (isVip) {
            return "VIP: " + username;
        }
        return "用户: " + username;
    }
};

// 配置多绑定
MultiBinding multi;
multi.AddBinding(Binding().Path("IsLoggedIn").Source(session));
multi.AddBinding(Binding().Path("Username").Source(user));
multi.AddBinding(Binding().Path("IsVip").Source(user));
multi.Converter(std::make_shared<ConditionalDisplayConverter>());

welcomeLabel->SetBinding(Label::TextProperty(), multi);
```

## 使用默认转换器

```cpp
// 默认转换器自动处理常见类型转换
// 通常不需要显式指定，绑定系统会自动应用

// 显式使用（当需要确保使用默认转换时）
Binding binding("Count");  // Count 是 int 类型
binding.Source(model)
       .Converter(DefaultValueConverter::Instance());

// 绑定到 Text 属性（string 类型）
// DefaultValueConverter 自动将 int 转换为 string
textBox->SetBinding(TextBox::TextProperty(), binding);
```

---

# 转换流程图

```
┌─────────────────────────────────────────────────────────────────┐
│                    单值转换器工作流程                              │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│   Source Property ──► Convert() ──► Target Property             │
│        (源值)           ↓              (目标值)                   │
│                    IValueConverter                              │
│                                                                 │
│   Source Property ◄── ConvertBack() ◄── Target Property         │
│        (源值)              ↑              (目标值)                │
│                    (双向绑定时)                                   │
│                                                                 │
└─────────────────────────────────────────────────────────────────┘

┌─────────────────────────────────────────────────────────────────┐
│                   多值转换器工作流程                               │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│   Source Property 1 ─┐                                          │
│   Source Property 2 ─┼─► Convert() ──► Target Property          │
│   Source Property N ─┘       ↓           (单个目标值)             │
│                      IMultiValueConverter                       │
│                                                                 │
│   Source Property 1 ◄─┐                                         │
│   Source Property 2 ◄─┼─ ConvertBack() ◄── Target Property      │
│   Source Property N ◄─┘        ↑            (单个目标值)          │
│                       (双向绑定时)                                │
│                                                                 │
└─────────────────────────────────────────────────────────────────┘
```

---

# 最佳实践

## 1. 转换器应该是无状态的

```cpp
// ✓ 好：无状态转换器
class GoodConverter : public IValueConverter {
public:
    std::any Convert(...) const override {
        // 只使用参数，不使用成员变量
    }
};

// ✗ 避免：有状态可能导致问题
class BadConverter : public IValueConverter {
    mutable int callCount_ = 0; // 避免这样的状态
};
```

## 2. 处理无效输入

```cpp
std::any Convert(const std::any& value, ...) const override {
    // 检查值是否有效
    if (!value.has_value()) {
        return std::any{}; // 返回空值
    }
    
    // 检查类型
    if (value.type() != typeid(ExpectedType)) {
        return value; // 无法转换时返回原值
    }
    
    // 执行转换
    // ...
}
```

## 3. 使用单例模式复用转换器

```cpp
// 全局静态实例
static auto s_boolConverter = std::make_shared<BooleanToStringConverter>("Yes", "No");

// 使用
binding1.Converter(s_boolConverter);
binding2.Converter(s_boolConverter);
```

## 4. 支持双向绑定

```cpp
// 如果不支持 ConvertBack，返回原值或抛出
std::any ConvertBack(const std::any& value, ...) const override {
    // 选项 1：返回原值表示不支持
    return value;
    
    // 选项 2：抛出异常
    // throw std::runtime_error("ConvertBack not supported");
}
```

---

# 相关类

- [Binding](Binding.md) - 使用 IValueConverter 进行值转换
- [MultiBinding](MultiBinding.md) - 使用 IMultiValueConverter 进行多值转换
- [BindingExpression](BindingExpression.md) - 执行实际转换的运行时对象
