# DependencyProperty

## 概览

| 属性 | 值 |
|------|-----|
| **命名空间** | `fk::binding` |
| **头文件** | `fk/binding/DependencyProperty.h` |
| **源文件** | `src/binding/DependencyProperty.cpp` |

## 描述

`DependencyProperty` 定义依赖属性的元数据和行为，是依赖属性系统的核心，负责：

- 注册普通依赖属性和附加属性
- 存储属性元数据（默认值、变更回调、验证回调）
- 配置绑定选项（绑定模式、更新触发器）
- 提供属性的唯一标识和类型信息

## 类定义

```cpp
namespace fk::binding {

class DependencyProperty {
public:
    // 禁止复制和移动
    DependencyProperty(const DependencyProperty&) = delete;
    DependencyProperty& operator=(const DependencyProperty&) = delete;
    DependencyProperty(DependencyProperty&&) = delete;
    DependencyProperty& operator=(DependencyProperty&&) = delete;

    // 注册方法
    static const DependencyProperty& Register(std::string name,
        std::type_index propertyType,
        std::type_index ownerType,
        PropertyMetadata metadata = {});

    static const DependencyProperty& RegisterAttached(std::string name,
        std::type_index propertyType,
        std::type_index ownerType,
        PropertyMetadata metadata = {});

    // 属性信息
    [[nodiscard]] const std::string& Name() const noexcept;
    [[nodiscard]] std::type_index PropertyType() const noexcept;
    [[nodiscard]] std::type_index OwnerType() const noexcept;
    [[nodiscard]] const PropertyMetadata& Metadata() const noexcept;
    [[nodiscard]] std::size_t Id() const noexcept;
    [[nodiscard]] bool IsAttached() const noexcept;

private:
    DependencyProperty(std::string name,
        std::type_index propertyType,
        std::type_index ownerType,
        PropertyMetadata metadata,
        bool attached);

    std::string name_;
    std::type_index propertyType_;
    std::type_index ownerType_;
    PropertyMetadata metadata_;
    std::size_t id_{0};
    bool isAttached_{false};

    friend class DependencyPropertyRegistry;
};

} // namespace fk::binding
```

---

## 注册方法

### Register

```cpp
static const DependencyProperty& Register(std::string name,
    std::type_index propertyType,
    std::type_index ownerType,
    PropertyMetadata metadata = {});
```

注册一个普通依赖属性。

**参数**：
| 参数 | 类型 | 描述 |
|------|------|------|
| `name` | `std::string` | 属性名称 |
| `propertyType` | `std::type_index` | 属性值类型 |
| `ownerType` | `std::type_index` | 属性所属类类型 |
| `metadata` | `PropertyMetadata` | 属性元数据（可选） |

**返回值**：已注册属性的常量引用

**异常**：
- 如果 `name` 为空，抛出 `std::invalid_argument`
- 如果属性已注册，抛出 `std::logic_error`
- 如果默认值验证失败，抛出 `std::invalid_argument`

**示例**：
```cpp
class MyElement : public fk::DependencyObject {
public:
    static const fk::DependencyProperty& WidthProperty() {
        static const auto& prop = fk::DependencyProperty::Register(
            "Width",
            typeid(double),
            typeid(MyElement),
            fk::PropertyMetadata{
                .defaultValue = 0.0
            }
        );
        return prop;
    }
};
```

### RegisterAttached

```cpp
static const DependencyProperty& RegisterAttached(std::string name,
    std::type_index propertyType,
    std::type_index ownerType,
    PropertyMetadata metadata = {});
```

注册一个附加属性。

**参数**：
| 参数 | 类型 | 描述 |
|------|------|------|
| `name` | `std::string` | 属性名称 |
| `propertyType` | `std::type_index` | 属性值类型 |
| `ownerType` | `std::type_index` | 属性定义类类型 |
| `metadata` | `PropertyMetadata` | 属性元数据（可选） |

**返回值**：已注册属性的常量引用

**说明**：
- 附加属性可以设置到任何 `DependencyObject` 上，不限于所有者类型
- 常用于布局系统（如 `Grid.Row`、`Canvas.Left`）

**示例**：
```cpp
class Grid : public fk::Panel {
public:
    static const fk::DependencyProperty& RowProperty() {
        static const auto& prop = fk::DependencyProperty::RegisterAttached(
            "Row",
            typeid(int),
            typeid(Grid),
            fk::PropertyMetadata{
                .defaultValue = 0
            }
        );
        return prop;
    }

    // 附加属性的 getter/setter
    static int GetRow(fk::DependencyObject* obj) {
        return obj->GetValue<int>(RowProperty());
    }

    static void SetRow(fk::DependencyObject* obj, int value) {
        obj->SetValue(RowProperty(), value);
    }
};
```

---

## 属性信息方法

### Name

```cpp
[[nodiscard]] const std::string& Name() const noexcept;
```

获取属性名称。

**返回值**：属性名称的常量引用

**示例**：
```cpp
std::cout << "属性名: " << property.Name() << std::endl;
```

### PropertyType

```cpp
[[nodiscard]] std::type_index PropertyType() const noexcept;
```

获取属性值类型。

**返回值**：属性值的 `std::type_index`

**示例**：
```cpp
if (property.PropertyType() == typeid(double)) {
    std::cout << "这是一个 double 类型的属性" << std::endl;
}
```

### OwnerType

```cpp
[[nodiscard]] std::type_index OwnerType() const noexcept;
```

获取属性所有者类型。

**返回值**：所有者类的 `std::type_index`

### Metadata

```cpp
[[nodiscard]] const PropertyMetadata& Metadata() const noexcept;
```

获取属性元数据。

**返回值**：`PropertyMetadata` 的常量引用

**示例**：
```cpp
const auto& metadata = property.Metadata();
if (metadata.defaultValue.has_value()) {
    double defaultVal = std::any_cast<double>(metadata.defaultValue);
    std::cout << "默认值: " << defaultVal << std::endl;
}
```

### Id

```cpp
[[nodiscard]] std::size_t Id() const noexcept;
```

获取属性的唯一标识符。

**返回值**：属性的唯一 ID（全局递增）

**说明**：ID 在运行时分配，用于快速比较和哈希

### IsAttached

```cpp
[[nodiscard]] bool IsAttached() const noexcept;
```

检查是否为附加属性。

**返回值**：如果是附加属性返回 `true`

**示例**：
```cpp
if (property.IsAttached()) {
    std::cout << "这是一个附加属性" << std::endl;
}
```

---

# PropertyMetadata

## 描述

`PropertyMetadata` 结构体存储依赖属性的元数据配置。

## 结构定义

```cpp
struct PropertyMetadata {
    std::any defaultValue{};                           // 默认值
    PropertyChangedCallback propertyChangedCallback{}; // 属性变更回调
    ValidateValueCallback validateCallback{};          // 值验证回调
    BindingOptions bindingOptions{};                   // 绑定选项
};
```

## 成员

| 成员 | 类型 | 描述 |
|------|------|------|
| `defaultValue` | `std::any` | 属性的默认值 |
| `propertyChangedCallback` | `PropertyChangedCallback` | 属性值变更时的回调函数 |
| `validateCallback` | `ValidateValueCallback` | 设置值前的验证函数 |
| `bindingOptions` | `BindingOptions` | 绑定相关选项 |

## 回调类型

### PropertyChangedCallback

```cpp
using PropertyChangedCallback = std::function<void(
    DependencyObject& sender,
    const DependencyProperty& property,
    const std::any& oldValue,
    const std::any& newValue
)>;
```

属性值变更时的回调函数。

**示例**：
```cpp
fk::PropertyMetadata metadata{
    .defaultValue = 0.0,
    .propertyChangedCallback = [](fk::DependencyObject& obj,
                                   const fk::DependencyProperty& prop,
                                   const std::any& oldVal,
                                   const std::any& newVal) {
        double oldWidth = std::any_cast<double>(oldVal);
        double newWidth = std::any_cast<double>(newVal);
        std::cout << "宽度从 " << oldWidth << " 变为 " << newWidth << std::endl;
    }
};
```

### ValidateValueCallback

```cpp
using ValidateValueCallback = std::function<bool(const std::any& value)>;
```

值验证回调，返回 `true` 表示值有效。

**示例**：
```cpp
fk::PropertyMetadata metadata{
    .defaultValue = 1.0,
    .validateCallback = [](const std::any& value) {
        double opacity = std::any_cast<double>(value);
        return opacity >= 0.0 && opacity <= 1.0;  // 值必须在 0-1 之间
    }
};
```

---

# BindingOptions

## 描述

`BindingOptions` 结构体配置属性的绑定行为。

## 结构定义

```cpp
struct BindingOptions {
    BindingMode defaultMode{BindingMode::OneWay};
    UpdateSourceTrigger updateSourceTrigger{UpdateSourceTrigger::PropertyChanged};
    bool inheritsDataContext{false};
};
```

## 成员

| 成员 | 类型 | 默认值 | 描述 |
|------|------|--------|------|
| `defaultMode` | `BindingMode` | `OneWay` | 默认绑定模式 |
| `updateSourceTrigger` | `UpdateSourceTrigger` | `PropertyChanged` | 默认更新触发器 |
| `inheritsDataContext` | `bool` | `false` | 是否继承数据上下文 |

---

# BindingMode 枚举

## 描述

定义绑定的数据流方向。

## 枚举值

```cpp
enum class BindingMode {
    OneTime,
    OneWay,
    TwoWay,
    OneWayToSource
};
```

| 值 | 描述 |
|----|------|
| `OneTime` | 一次性绑定，初始化后不再更新 |
| `OneWay` | 单向绑定，源变更时更新目标（默认） |
| `TwoWay` | 双向绑定，源和目标互相同步 |
| `OneWayToSource` | 反向单向绑定，目标变更时更新源 |

**使用示例**：
```cpp
fk::Binding binding;
binding.Path("Username")
       .Mode(fk::BindingMode::TwoWay);  // 双向绑定
```

---

# UpdateSourceTrigger 枚举

## 描述

定义何时将目标值更新回源（用于 `TwoWay` 和 `OneWayToSource` 模式）。

## 枚举值

```cpp
enum class UpdateSourceTrigger {
    Default,
    PropertyChanged,
    LostFocus,
    Explicit
};
```

| 值 | 描述 |
|----|------|
| `Default` | 使用属性元数据中的默认设置 |
| `PropertyChanged` | 属性变更时立即更新源 |
| `LostFocus` | 失去焦点时更新源 |
| `Explicit` | 显式调用 `UpdateSource()` 时更新 |

**使用示例**：
```cpp
fk::Binding binding;
binding.Path("Email")
       .Mode(fk::BindingMode::TwoWay)
       .SetUpdateSourceTrigger(fk::UpdateSourceTrigger::LostFocus);
```

---

## 使用示例

### 基本属性注册

```cpp
#include "fk/binding/DependencyProperty.h"
#include "fk/binding/DependencyObject.h"

class MyElement : public fk::DependencyObject {
public:
    // 注册简单属性
    static const fk::DependencyProperty& WidthProperty() {
        static const auto& prop = fk::DependencyProperty::Register(
            "Width",
            typeid(double),
            typeid(MyElement),
            fk::PropertyMetadata{
                .defaultValue = 0.0
            }
        );
        return prop;
    }

    // 便捷的 getter/setter
    double GetWidth() const {
        return GetValue<double>(WidthProperty());
    }

    void SetWidth(double value) {
        SetValue(WidthProperty(), value);
    }
};
```

### 带变更回调的属性

```cpp
class Control : public fk::DependencyObject {
public:
    static const fk::DependencyProperty& BackgroundProperty() {
        static const auto& prop = fk::DependencyProperty::Register(
            "Background",
            typeid(Color),
            typeid(Control),
            fk::PropertyMetadata{
                .defaultValue = Colors::Transparent,
                .propertyChangedCallback = [](fk::DependencyObject& obj,
                                              const fk::DependencyProperty&,
                                              const std::any&,
                                              const std::any&) {
                    // 触发重绘
                    static_cast<Control&>(obj).InvalidateVisual();
                }
            }
        );
        return prop;
    }
};
```

### 带验证的属性

```cpp
class UIElement : public fk::DependencyObject {
public:
    static const fk::DependencyProperty& OpacityProperty() {
        static const auto& prop = fk::DependencyProperty::Register(
            "Opacity",
            typeid(double),
            typeid(UIElement),
            fk::PropertyMetadata{
                .defaultValue = 1.0,
                .validateCallback = [](const std::any& value) {
                    double opacity = std::any_cast<double>(value);
                    return opacity >= 0.0 && opacity <= 1.0;
                }
            }
        );
        return prop;
    }
};
```

### 配置绑定选项

```cpp
class TextBox : public fk::Control {
public:
    static const fk::DependencyProperty& TextProperty() {
        static const auto& prop = fk::DependencyProperty::Register(
            "Text",
            typeid(std::string),
            typeid(TextBox),
            fk::PropertyMetadata{
                .defaultValue = std::string{},
                .bindingOptions = fk::BindingOptions{
                    .defaultMode = fk::BindingMode::TwoWay,
                    .updateSourceTrigger = fk::UpdateSourceTrigger::LostFocus
                }
            }
        );
        return prop;
    }
};
```

### 附加属性

```cpp
class Canvas : public fk::Panel {
public:
    // 附加属性：Canvas.Left
    static const fk::DependencyProperty& LeftProperty() {
        static const auto& prop = fk::DependencyProperty::RegisterAttached(
            "Left",
            typeid(double),
            typeid(Canvas),
            fk::PropertyMetadata{
                .defaultValue = 0.0,
                .propertyChangedCallback = [](fk::DependencyObject& obj,
                                              const fk::DependencyProperty&,
                                              const std::any&,
                                              const std::any&) {
                    // 通知父 Canvas 重新布局
                    if (auto* parent = obj.GetLogicalParent()) {
                        if (auto* canvas = dynamic_cast<Canvas*>(parent)) {
                            canvas->InvalidateArrange();
                        }
                    }
                }
            }
        );
        return prop;
    }

    // 附加属性的静态 getter
    static double GetLeft(fk::DependencyObject* obj) {
        return obj->GetValue<double>(LeftProperty());
    }

    // 附加属性的静态 setter
    static void SetLeft(fk::DependencyObject* obj, double value) {
        obj->SetValue(LeftProperty(), value);
    }
};

// 使用附加属性
auto button = std::make_shared<Button>();
Canvas::SetLeft(button.get(), 100.0);
double left = Canvas::GetLeft(button.get());  // 100.0
```

---

## 私有成员

| 成员 | 类型 | 描述 |
|------|------|------|
| `name_` | `std::string` | 属性名称 |
| `propertyType_` | `std::type_index` | 属性值类型 |
| `ownerType_` | `std::type_index` | 所有者类类型 |
| `metadata_` | `PropertyMetadata` | 属性元数据 |
| `id_` | `std::size_t` | 唯一标识符 |
| `isAttached_` | `bool` | 是否为附加属性 |

---

## DependencyPropertyRegistry（内部类）

`DependencyPropertyRegistry` 是一个内部单例类，管理所有已注册的依赖属性。

### 主要功能
- 存储所有已注册的 `DependencyProperty` 实例
- 按所有者类型和属性名称索引
- 防止重复注册
- 提供按类型和名称查找属性的功能

### 全局查找函数

```cpp
const DependencyProperty* FindDependencyProperty(std::type_index ownerType, const std::string& name);
```

按所有者类型和属性名称查找依赖属性。

**返回值**：找到的属性指针，如果未注册返回 `nullptr`

---

## 相关类

- [DependencyObject](DependencyObject.md) - 使用依赖属性的基类
- [Binding](Binding.md) - 使用绑定模式和更新触发器
- [BindingExpression](BindingExpression.md) - 使用属性元数据中的绑定选项
- [PropertyStore](PropertyStore.md) - 属性值存储

## 另请参阅

- [数据绑定概述](../BINDING_MODULE_OVERVIEW.md)
- [入门指南](../../Guides/GettingStarted.md)
