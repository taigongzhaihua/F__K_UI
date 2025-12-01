# MultiBinding

## 概览

| 属性 | 值 |
|------|-----|
| **命名空间** | `fk::binding` |
| **头文件** | `fk/binding/MultiBinding.h` |
| **源文件** | `src/binding/MultiBinding.cpp` |

## 描述

`MultiBinding` 允许将多个源属性绑定到单个目标属性，负责：

- 配置多个子绑定（`Binding` 对象）
- 设置多值转换器（`IMultiValueConverter`）将多个值合并为一个
- 配置绑定模式和更新触发器
- 创建 `MultiBindingExpression` 活动实例

## 类定义

```cpp
namespace fk::binding {

class MultiBinding {
public:
    MultiBinding() = default;

    // 配置方法（链式调用）
    MultiBinding& AddBinding(Binding binding);
    MultiBinding& Converter(std::shared_ptr<IMultiValueConverter> converter);
    MultiBinding& ConverterParameter(std::any parameter);
    MultiBinding& Mode(BindingMode mode);
    MultiBinding& SetUpdateSourceTrigger(UpdateSourceTrigger trigger);

    // Getter 方法
    [[nodiscard]] const std::vector<Binding>& GetBindings() const noexcept;
    [[nodiscard]] const std::shared_ptr<IMultiValueConverter>& GetConverter() const noexcept;
    [[nodiscard]] const std::any& GetConverterParameter() const noexcept;
    [[nodiscard]] bool HasConverterParameter() const noexcept;
    [[nodiscard]] BindingMode GetMode() const noexcept;
    [[nodiscard]] UpdateSourceTrigger GetUpdateSourceTrigger() const noexcept;
    [[nodiscard]] bool HasExplicitMode() const noexcept;
    [[nodiscard]] bool HasExplicitUpdateSourceTrigger() const noexcept;

    // 创建表达式
    std::shared_ptr<MultiBindingExpression> CreateExpression(
        DependencyObject* target,
        const DependencyProperty& targetProperty) const;

private:
    std::vector<Binding> bindings_;
    std::shared_ptr<IMultiValueConverter> converter_;
    std::any converterParameter_;
    BindingMode mode_{BindingMode::OneWay};
    bool hasExplicitMode_{false};
    UpdateSourceTrigger updateSourceTrigger_{UpdateSourceTrigger::Default};
    bool hasExplicitUpdateSourceTrigger_{false};
};

} // namespace fk::binding
```

---

## 配置方法

### AddBinding

```cpp
MultiBinding& AddBinding(Binding binding);
```

添加一个子绑定。

**参数**：
| 参数 | 类型 | 描述 |
|------|------|------|
| `binding` | `Binding` | 要添加的绑定配置 |

**返回值**：`*this`（支持链式调用）

**示例**：
```cpp
fk::MultiBinding multi;
multi.AddBinding(fk::bind("FirstName"))
     .AddBinding(fk::bind("LastName"));
```

### Converter

```cpp
MultiBinding& Converter(std::shared_ptr<IMultiValueConverter> converter);
```

设置多值转换器。

**参数**：
| 参数 | 类型 | 描述 |
|------|------|------|
| `converter` | `std::shared_ptr<IMultiValueConverter>` | 多值转换器 |

**返回值**：`*this`（支持链式调用）

**说明**：
- 转换器负责将多个源值合并为单个目标值
- 如果不设置转换器，默认使用第一个绑定的值

**示例**：
```cpp
multi.Converter(std::make_shared<FullNameConverter>());
```

### ConverterParameter

```cpp
MultiBinding& ConverterParameter(std::any parameter);
```

设置转换器参数。

**参数**：
| 参数 | 类型 | 描述 |
|------|------|------|
| `parameter` | `std::any` | 传递给转换器的参数 |

**返回值**：`*this`（支持链式调用）

**示例**：
```cpp
multi.ConverterParameter(std::string(" - "));  // 自定义分隔符
```

### Mode

```cpp
MultiBinding& Mode(BindingMode mode);
```

设置绑定模式。

**参数**：
| 参数 | 类型 | 描述 |
|------|------|------|
| `mode` | `BindingMode` | 绑定模式 |

**返回值**：`*this`（支持链式调用）

**示例**：
```cpp
multi.Mode(fk::BindingMode::OneWay);
```

### SetUpdateSourceTrigger

```cpp
MultiBinding& SetUpdateSourceTrigger(UpdateSourceTrigger trigger);
```

设置更新源触发器。

**参数**：
| 参数 | 类型 | 描述 |
|------|------|------|
| `trigger` | `UpdateSourceTrigger` | 更新触发器 |

**返回值**：`*this`（支持链式调用）

---

## Getter 方法

### GetBindings

```cpp
[[nodiscard]] const std::vector<Binding>& GetBindings() const noexcept;
```

获取所有子绑定。

**返回值**：子绑定向量的常量引用

### GetConverter

```cpp
[[nodiscard]] const std::shared_ptr<IMultiValueConverter>& GetConverter() const noexcept;
```

获取多值转换器。

**返回值**：转换器智能指针的常量引用

### GetConverterParameter / HasConverterParameter

```cpp
[[nodiscard]] const std::any& GetConverterParameter() const noexcept;
[[nodiscard]] bool HasConverterParameter() const noexcept;
```

获取/检查转换器参数。

### GetMode / HasExplicitMode

```cpp
[[nodiscard]] BindingMode GetMode() const noexcept;
[[nodiscard]] bool HasExplicitMode() const noexcept;
```

获取绑定模式和是否显式设置。

### GetUpdateSourceTrigger / HasExplicitUpdateSourceTrigger

```cpp
[[nodiscard]] UpdateSourceTrigger GetUpdateSourceTrigger() const noexcept;
[[nodiscard]] bool HasExplicitUpdateSourceTrigger() const noexcept;
```

获取更新触发器和是否显式设置。

---

## 创建表达式方法

### CreateExpression

```cpp
std::shared_ptr<MultiBindingExpression> CreateExpression(
    DependencyObject* target,
    const DependencyProperty& targetProperty) const;
```

创建 `MultiBindingExpression` 活动实例。

**参数**：
| 参数 | 类型 | 描述 |
|------|------|------|
| `target` | `DependencyObject*` | 绑定目标对象 |
| `targetProperty` | `const DependencyProperty&` | 目标属性 |

**返回值**：`MultiBindingExpression` 智能指针

**示例**：
```cpp
auto expr = multi.CreateExpression(textBlock, TextBlock::TextProperty());
expr->Activate();
```

---

# IMultiValueConverter

## 描述

多值转换器接口，用于将多个源值转换为单个目标值。

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

```cpp
virtual std::any Convert(
    const std::vector<std::any>& values,
    std::type_index targetType,
    const std::any* parameter) const = 0;
```

将多个源值转换为单个目标值。

**参数**：
| 参数 | 类型 | 描述 |
|------|------|------|
| `values` | `const std::vector<std::any>&` | 源值向量 |
| `targetType` | `std::type_index` | 目标属性类型 |
| `parameter` | `const std::any*` | 可选的转换器参数 |

**返回值**：转换后的目标值

### ConvertBack

```cpp
virtual std::vector<std::any> ConvertBack(
    const std::any& value,
    const std::vector<std::type_index>& sourceTypes,
    const std::any* parameter) const;
```

将目标值转换回多个源值（可选实现）。

**参数**：
| 参数 | 类型 | 描述 |
|------|------|------|
| `value` | `const std::any&` | 目标值 |
| `sourceTypes` | `const std::vector<std::type_index>&` | 源属性类型向量 |
| `parameter` | `const std::any*` | 可选的转换器参数 |

**返回值**：转换后的源值向量（默认返回空向量）

---

# MultiBindingExpression

## 概览

| 属性 | 值 |
|------|-----|
| **命名空间** | `fk::binding` |
| **头文件** | `fk/binding/MultiBindingExpression.h` |
| **源文件** | `src/binding/MultiBindingExpression.cpp` |

## 描述

`MultiBindingExpression` 是 `MultiBinding` 的活动实例，负责：

- 管理多个子 `BindingExpression`
- 收集所有子绑定的源值
- 使用转换器合并值并更新目标
- 订阅源属性变更通知

## 类定义

```cpp
namespace fk::binding {

class MultiBindingExpression : public std::enable_shared_from_this<MultiBindingExpression> {
public:
    MultiBindingExpression(
        MultiBinding definition,
        DependencyObject* target,
        const DependencyProperty& property);

    void Activate();
    void Detach();
    void UpdateTarget();

    [[nodiscard]] bool IsActive() const noexcept;
    [[nodiscard]] DependencyObject* Target() const noexcept;
    [[nodiscard]] const DependencyProperty& Property() const noexcept;
    [[nodiscard]] const MultiBinding& Definition() const noexcept;

private:
    MultiBinding definition_;
    DependencyObject* target_{nullptr};
    const DependencyProperty* property_{nullptr};
    bool isActive_{false};
    bool isUpdatingTarget_{false};
    BindingMode effectiveMode_{BindingMode::OneWay};
    UpdateSourceTrigger effectiveUpdateSourceTrigger_{UpdateSourceTrigger::PropertyChanged};
    std::vector<std::shared_ptr<BindingExpression>> childExpressions_;
    std::vector<INotifyPropertyChanged::PropertyChangedEvent::Connection> childConnections_;
};

} // namespace fk::binding
```

---

## 构造函数

### MultiBindingExpression()

```cpp
MultiBindingExpression(
    MultiBinding definition,
    DependencyObject* target,
    const DependencyProperty& property);
```

构造一个多绑定表达式实例。

**参数**：
| 参数 | 类型 | 描述 |
|------|------|------|
| `definition` | `MultiBinding` | 多绑定配置 |
| `target` | `DependencyObject*` | 绑定目标对象 |
| `property` | `const DependencyProperty&` | 目标属性 |

**异常**：如果 `target` 为 `nullptr`，抛出 `std::invalid_argument`

---

## 生命周期方法

### Activate

```cpp
void Activate();
```

激活多绑定表达式。

**说明**：
- 为每个子绑定创建并激活 `BindingExpression`
- 订阅子绑定源的变更通知
- 执行初始的 `UpdateTarget()`

### Detach

```cpp
void Detach();
```

分离多绑定表达式。

**说明**：
- 断开所有子绑定的事件订阅
- 分离并清除所有子 `BindingExpression`
- 设置 `isActive_ = false`

### UpdateTarget

```cpp
void UpdateTarget();
```

从所有源更新目标属性值。

**说明**：
- 收集所有子绑定的源值
- 使用转换器合并值（如果设置）
- 将合并后的值应用到目标属性

---

## 状态查询方法

### IsActive

```cpp
[[nodiscard]] bool IsActive() const noexcept;
```

检查多绑定表达式是否已激活。

### Target

```cpp
[[nodiscard]] DependencyObject* Target() const noexcept;
```

获取绑定目标对象。

### Property

```cpp
[[nodiscard]] const DependencyProperty& Property() const noexcept;
```

获取目标属性。

### Definition

```cpp
[[nodiscard]] const MultiBinding& Definition() const noexcept;
```

获取多绑定配置定义。

---

## 使用示例

### 组合姓名

```cpp
#include "fk/binding/MultiBinding.h"

// 自定义转换器：组合名字和姓氏
class FullNameConverter : public fk::IMultiValueConverter {
public:
    std::any Convert(
        const std::vector<std::any>& values,
        std::type_index,
        const std::any* parameter) const override {
        
        if (values.size() < 2) {
            return std::string{};
        }
        
        std::string firstName = values[0].has_value() 
            ? std::any_cast<std::string>(values[0]) 
            : "";
        std::string lastName = values[1].has_value() 
            ? std::any_cast<std::string>(values[1]) 
            : "";
        
        // 使用参数作为分隔符（如果提供）
        std::string separator = " ";
        if (parameter && parameter->has_value()) {
            separator = std::any_cast<std::string>(*parameter);
        }
        
        return firstName + separator + lastName;
    }
};

// 使用
fk::MultiBinding multi;
multi.AddBinding(fk::bind("FirstName"))
     .AddBinding(fk::bind("LastName"))
     .Converter(std::make_shared<FullNameConverter>());

textBlock->SetBinding(TextBlock::TextProperty(), multi);
```

### 计算总价

```cpp
// 转换器：价格 × 数量
class TotalPriceConverter : public fk::IMultiValueConverter {
public:
    std::any Convert(
        const std::vector<std::any>& values,
        std::type_index,
        const std::any*) const override {
        
        if (values.size() < 2) {
            return 0.0;
        }
        
        double price = values[0].has_value() 
            ? std::any_cast<double>(values[0]) 
            : 0.0;
        int quantity = values[1].has_value() 
            ? std::any_cast<int>(values[1]) 
            : 0;
        
        return price * quantity;
    }
};

fk::MultiBinding totalBinding;
totalBinding.AddBinding(fk::bind("UnitPrice"))
            .AddBinding(fk::bind("Quantity"))
            .Converter(std::make_shared<TotalPriceConverter>());

totalLabel->SetBinding(Label::ContentProperty(), totalBinding);
```

### 条件显示

```cpp
// 转换器：根据多个条件决定可见性
class VisibilityConverter : public fk::IMultiValueConverter {
public:
    std::any Convert(
        const std::vector<std::any>& values,
        std::type_index,
        const std::any*) const override {
        
        // 所有条件都为 true 时显示
        for (const auto& value : values) {
            if (!value.has_value()) {
                return Visibility::Collapsed;
            }
            if (!std::any_cast<bool>(value)) {
                return Visibility::Collapsed;
            }
        }
        return Visibility::Visible;
    }
};

fk::MultiBinding visBinding;
visBinding.AddBinding(fk::bind("IsLoggedIn"))
          .AddBinding(fk::bind("HasPermission"))
          .AddBinding(fk::bind("IsEnabled"))
          .Converter(std::make_shared<VisibilityConverter>());

adminPanel->SetBinding(UIElement::VisibilityProperty(), visBinding);
```

### 带参数的转换器

```cpp
fk::MultiBinding multi;
multi.AddBinding(fk::bind("Value1"))
     .AddBinding(fk::bind("Value2"))
     .Converter(std::make_shared<SeparatorConverter>())
     .ConverterParameter(std::string(", "));  // 使用逗号分隔

textBlock->SetBinding(TextBlock::TextProperty(), multi);
```

---

## 私有成员

### MultiBinding

| 成员 | 类型 | 描述 |
|------|------|------|
| `bindings_` | `std::vector<Binding>` | 子绑定列表 |
| `converter_` | `std::shared_ptr<IMultiValueConverter>` | 多值转换器 |
| `converterParameter_` | `std::any` | 转换器参数 |
| `mode_` | `BindingMode` | 绑定模式 |
| `hasExplicitMode_` | `bool` | 是否显式设置了模式 |
| `updateSourceTrigger_` | `UpdateSourceTrigger` | 更新触发器 |
| `hasExplicitUpdateSourceTrigger_` | `bool` | 是否显式设置了触发器 |

### MultiBindingExpression

| 成员 | 类型 | 描述 |
|------|------|------|
| `definition_` | `MultiBinding` | 多绑定配置 |
| `target_` | `DependencyObject*` | 绑定目标对象 |
| `property_` | `const DependencyProperty*` | 目标属性 |
| `isActive_` | `bool` | 是否已激活 |
| `isUpdatingTarget_` | `bool` | 是否正在更新目标（防止递归） |
| `effectiveMode_` | `BindingMode` | 实际生效的绑定模式 |
| `effectiveUpdateSourceTrigger_` | `UpdateSourceTrigger` | 实际生效的更新触发器 |
| `childExpressions_` | `std::vector<std::shared_ptr<BindingExpression>>` | 子绑定表达式列表 |
| `childConnections_` | `std::vector<Connection>` | 子绑定事件连接列表 |

---

## 相关类

- [Binding](Binding.md) - 单一绑定配置
- [BindingExpression](BindingExpression.md) - 单一绑定的活动实例
- [DependencyObject](DependencyObject.md) - 绑定目标基类
- [IValueConverter](ValueConverters.md) - 单值转换器接口

## 另请参阅

- [数据绑定概述](../BINDING_MODULE_OVERVIEW.md)
- [入门指南](../../Guides/GettingStarted.md)
