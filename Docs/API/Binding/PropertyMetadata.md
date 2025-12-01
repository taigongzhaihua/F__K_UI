# PropertyMetadata

## 概览

| 属性 | 值 |
|------|-----|
| **命名空间** | `fk::binding` |
| **头文件** | `fk/binding/DependencyProperty.h` |
| **类型** | 结构体 |

## 描述

`PropertyMetadata` 是一个结构体，用于存储依赖属性的元数据信息。它定义了属性的默认值、变更回调、验证回调和绑定选项，是创建依赖属性时的核心配置对象。

## 结构体定义

```cpp
namespace fk::binding {

// 回调类型别名
using PropertyChangedCallback = std::function<void(
    DependencyObject&,
    const DependencyProperty&,
    const std::any& oldValue,
    const std::any& newValue)>;

using ValidateValueCallback = std::function<bool(const std::any& value)>;

struct PropertyMetadata {
    std::any defaultValue{};
    PropertyChangedCallback propertyChangedCallback{};
    ValidateValueCallback validateCallback{};
    BindingOptions bindingOptions{};
};

} // namespace fk::binding
```

---

## 成员说明

### defaultValue

```cpp
std::any defaultValue{};
```

属性的默认值。当属性没有被显式设置时使用此值。

**说明**：
- 类型为 `std::any`，可存储任意类型的值
- 默认为空（`std::any{}`）
- 应与属性的声明类型匹配

---

### propertyChangedCallback

```cpp
PropertyChangedCallback propertyChangedCallback{};
```

属性值变更时的回调函数。

**回调签名**：
```cpp
void callback(DependencyObject& obj,
              const DependencyProperty& prop,
              const std::any& oldValue,
              const std::any& newValue);
```

**参数**：
| 参数 | 类型 | 描述 |
|------|------|------|
| `obj` | `DependencyObject&` | 属性所属的依赖对象 |
| `prop` | `const DependencyProperty&` | 变更的属性 |
| `oldValue` | `const std::any&` | 旧值 |
| `newValue` | `const std::any&` | 新值 |

---

### validateCallback

```cpp
ValidateValueCallback validateCallback{};
```

属性值验证回调函数。在设置属性值之前调用，用于验证值是否有效。

**回调签名**：
```cpp
bool callback(const std::any& value);
```

**参数**：
| 参数 | 类型 | 描述 |
|------|------|------|
| `value` | `const std::any&` | 要验证的值 |

**返回值**：`true` 表示值有效，`false` 表示值无效（将抛出异常）

---

### bindingOptions

```cpp
BindingOptions bindingOptions{};
```

属性的绑定配置选项。详见 [BindingOptions](./BindingOptions.md)。

**默认值**：
```cpp
BindingOptions {
    .defaultMode = BindingMode::OneWay,
    .updateSourceTrigger = UpdateSourceTrigger::PropertyChanged,
    .inheritsDataContext = false
};
```

---

## 回调类型详解

### PropertyChangedCallback

属性变更回调，在属性值成功变更后触发。

```cpp
using PropertyChangedCallback = std::function<void(
    DependencyObject&,
    const DependencyProperty&,
    const std::any& oldValue,
    const std::any& newValue)>;
```

**典型用途**：
- 触发 UI 更新
- 更新相关属性（级联更新）
- 记录属性变更日志
- 发送属性变更通知

---

### ValidateValueCallback

属性值验证回调，在设置属性值之前调用。

```cpp
using ValidateValueCallback = std::function<bool(const std::any& value)>;
```

**典型用途**：
- 范围检查（如年龄 0-150）
- 格式验证（如邮箱格式）
- 业务规则验证
- 空值检查

---

## 使用示例

### 示例 1：基本属性（仅默认值）

```cpp
// 简单的字符串属性
const auto& NameProperty = DependencyProperty::Register(
    "Name",
    typeid(std::string),
    typeid(Person),
    PropertyMetadata{
        .defaultValue = std::string{"Unknown"}
    }
);
```

### 示例 2：带变更回调的属性

```cpp
// 带变更回调的整数属性
const auto& AgeProperty = DependencyProperty::Register(
    "Age",
    typeid(int),
    typeid(Person),
    PropertyMetadata{
        .defaultValue = 0,
        .propertyChangedCallback = [](DependencyObject& obj,
                                      const DependencyProperty& prop,
                                      const std::any& oldValue,
                                      const std::any& newValue) {
            auto& person = static_cast<Person&>(obj);
            int oldAge = std::any_cast<int>(oldValue);
            int newAge = std::any_cast<int>(newValue);
            std::cout << "Age changed from " << oldAge << " to " << newAge << std::endl;
            
            // 可以在这里更新相关属性或触发其他逻辑
            person.UpdateAgeCategory();
        }
    }
);
```

### 示例 3：带验证回调的属性

```cpp
// 带范围验证的属性
const auto& ScoreProperty = DependencyProperty::Register(
    "Score",
    typeid(int),
    typeid(Student),
    PropertyMetadata{
        .defaultValue = 0,
        .validateCallback = [](const std::any& value) -> bool {
            try {
                int score = std::any_cast<int>(value);
                return score >= 0 && score <= 100;  // 分数范围 0-100
            } catch (...) {
                return false;
            }
        }
    }
);
```

### 示例 4：完整配置示例

```cpp
// 完整配置的属性
const auto& EmailProperty = DependencyProperty::Register(
    "Email",
    typeid(std::string),
    typeid(User),
    PropertyMetadata{
        .defaultValue = std::string{""},
        .propertyChangedCallback = [](DependencyObject& obj,
                                      const DependencyProperty& prop,
                                      const std::any& oldValue,
                                      const std::any& newValue) {
            auto& user = static_cast<User&>(obj);
            user.OnEmailChanged();
        },
        .validateCallback = [](const std::any& value) -> bool {
            try {
                auto email = std::any_cast<std::string>(value);
                // 简单的邮箱格式验证
                return email.find('@') != std::string::npos;
            } catch (...) {
                return false;
            }
        },
        .bindingOptions = BindingOptions{
            .defaultMode = BindingMode::TwoWay,
            .updateSourceTrigger = UpdateSourceTrigger::LostFocus,
            .inheritsDataContext = false
        }
    }
);
```

### 示例 5：配置双向绑定默认模式

```cpp
// 文本输入属性，默认双向绑定
const auto& TextProperty = DependencyProperty::Register(
    "Text",
    typeid(std::string),
    typeid(TextBox),
    PropertyMetadata{
        .defaultValue = std::string{""},
        .bindingOptions = BindingOptions{
            .defaultMode = BindingMode::TwoWay,
            .updateSourceTrigger = UpdateSourceTrigger::PropertyChanged
        }
    }
);
```

### 示例 6：级联属性更新

```cpp
// 当宽度或高度变化时，自动更新面积
const auto& WidthProperty = DependencyProperty::Register(
    "Width",
    typeid(double),
    typeid(Rectangle),
    PropertyMetadata{
        .defaultValue = 0.0,
        .propertyChangedCallback = [](DependencyObject& obj,
                                      const DependencyProperty& prop,
                                      const std::any& oldValue,
                                      const std::any& newValue) {
            auto& rect = static_cast<Rectangle&>(obj);
            rect.UpdateArea();  // 重新计算面积
        },
        .validateCallback = [](const std::any& value) -> bool {
            try {
                return std::any_cast<double>(value) >= 0;  // 宽度不能为负
            } catch (...) {
                return false;
            }
        }
    }
);
```

---

## 回调执行顺序

```
┌─────────────────────────────────────────────────────────┐
│                    SetValue() 调用                       │
└─────────────────────────────────────────────────────────┘
                           │
                           ▼
┌─────────────────────────────────────────────────────────┐
│              1. 调用 validateCallback                    │
│                 (如果存在且返回 false，抛出异常)           │
└─────────────────────────────────────────────────────────┘
                           │
                           ▼
┌─────────────────────────────────────────────────────────┐
│              2. 存储新值到 PropertyStore                  │
└─────────────────────────────────────────────────────────┘
                           │
                           ▼
┌─────────────────────────────────────────────────────────┐
│              3. 调用 propertyChangedCallback             │
│                 (如果存在，传递旧值和新值)                 │
└─────────────────────────────────────────────────────────┘
                           │
                           ▼
┌─────────────────────────────────────────────────────────┐
│              4. 触发 PropertyChanged 事件                │
└─────────────────────────────────────────────────────────┘
```

---

## 最佳实践

### 1. 默认值类型匹配

```cpp
// ✓ 正确：默认值类型与属性类型匹配
PropertyMetadata{
    .defaultValue = std::string{""}  // std::string 属性
}

// ✗ 错误：类型不匹配
PropertyMetadata{
    .defaultValue = ""  // const char* 不是 std::string
}
```

### 2. 验证回调要处理异常

```cpp
// ✓ 正确：捕获 any_cast 异常
.validateCallback = [](const std::any& value) -> bool {
    try {
        auto v = std::any_cast<int>(value);
        return v >= 0;
    } catch (...) {
        return false;
    }
}

// ✗ 危险：未处理异常
.validateCallback = [](const std::any& value) -> bool {
    auto v = std::any_cast<int>(value);  // 可能抛出异常
    return v >= 0;
}
```

### 3. 变更回调避免递归

```cpp
// ✓ 正确：检查值是否真的变化了
.propertyChangedCallback = [](DependencyObject& obj,
                              const DependencyProperty& prop,
                              const std::any& oldValue,
                              const std::any& newValue) {
    auto oldV = std::any_cast<int>(oldValue);
    auto newV = std::any_cast<int>(newValue);
    if (oldV != newV) {  // 只有真正变化时才处理
        // 处理逻辑
    }
}
```

### 4. 根据用途选择绑定选项

```cpp
// 显示属性：单向绑定
.bindingOptions = { .defaultMode = BindingMode::OneWay }

// 输入属性：双向绑定
.bindingOptions = { .defaultMode = BindingMode::TwoWay }

// 一次性初始化：OneTime
.bindingOptions = { .defaultMode = BindingMode::OneTime }
```

---

## 相关类

- [DependencyProperty](./DependencyProperty.md) - 使用 PropertyMetadata 注册属性
- [DependencyObject](./DependencyObject.md) - 存储和管理属性值
- [BindingOptions](./BindingOptions.md) - 绑定配置选项
- [BindingMode](./BindingMode.md) - 绑定模式枚举
- [UpdateSourceTrigger](./UpdateSourceTrigger.md) - 更新触发器枚举
