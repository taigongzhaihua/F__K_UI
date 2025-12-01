# Binding

## 概览

| 属性 | 值 |
|------|-----|
| **命名空间** | `fk::binding` |
| **头文件** | `fk/binding/Binding.h` |
| **源文件** | `src/binding/Binding.cpp` |

## 描述

`Binding` 类是数据绑定系统的核心配置对象，负责：

- 配置数据源对象和属性路径
- 设置绑定模式（单向、双向、一次性等）
- 配置值转换器和转换参数
- 管理验证规则
- 支持元素名称绑定和相对源绑定
- 创建活动的绑定表达式

## 类定义

```cpp
namespace fk::binding {

class Binding {
public:
    Binding() = default;
    virtual ~Binding() = default;

    // 链式配置方法
    Binding& Path(std::string path);
    Binding& Source(std::any source);
    Binding& Mode(BindingMode mode);
    Binding& SetUpdateSourceTrigger(UpdateSourceTrigger trigger);
    Binding& Converter(std::shared_ptr<IValueConverter> converter);
    Binding& ConverterParameter(std::any parameter);
    Binding& ValidatesOnDataErrors(bool enable);
    Binding& AddValidationRule(std::shared_ptr<ValidationRule> rule);
    Binding& ElementName(std::string name);
    Binding& SetRelativeSource(RelativeSource relativeSource);
    Binding& IsAsync(bool async);

    // Getter 方法
    [[nodiscard]] const std::string& GetPath() const noexcept;
    [[nodiscard]] const std::any& GetSource() const noexcept;
    [[nodiscard]] bool HasExplicitSource() const noexcept;
    [[nodiscard]] BindingMode GetMode() const noexcept;
    [[nodiscard]] UpdateSourceTrigger GetUpdateSourceTrigger() const noexcept;
    [[nodiscard]] const std::shared_ptr<IValueConverter>& GetConverter() const noexcept;
    [[nodiscard]] const std::any& GetConverterParameter() const noexcept;
    [[nodiscard]] bool HasConverterParameter() const noexcept;
    [[nodiscard]] bool ShouldValidateOnDataErrors() const noexcept;
    [[nodiscard]] const std::vector<std::shared_ptr<ValidationRule>>& GetValidationRules() const noexcept;
    [[nodiscard]] bool HasExplicitMode() const noexcept;
    [[nodiscard]] bool HasExplicitUpdateSourceTrigger() const noexcept;
    [[nodiscard]] bool HasElementName() const noexcept;
    [[nodiscard]] const std::string& GetElementName() const noexcept;
    [[nodiscard]] bool HasRelativeSource() const noexcept;
    [[nodiscard]] const RelativeSource& GetRelativeSource() const;
    [[nodiscard]] bool GetIsAsync() const noexcept;

    // 虚方法（支持派生类如 TemplateBinding）
    [[nodiscard]] virtual bool IsTemplateBinding() const noexcept;
    [[nodiscard]] virtual const DependencyProperty* GetTemplateBindingSourceProperty() const noexcept;
    [[nodiscard]] virtual std::unique_ptr<Binding> Clone() const;

    // 创建绑定表达式
    std::shared_ptr<BindingExpression> CreateExpression(DependencyObject* target, const DependencyProperty& targetProperty) const;

private:
    std::string path_{};
    std::any source_{};
    bool hasExplicitSource_{false};
    BindingMode mode_{BindingMode::OneWay};
    bool hasExplicitMode_{false};
    UpdateSourceTrigger updateSourceTrigger_{UpdateSourceTrigger::Default};
    bool hasExplicitUpdateSourceTrigger_{false};
    std::shared_ptr<IValueConverter> converter_{};
    std::any converterParameter_{};
    bool validatesOnDataErrors_{false};
    std::vector<std::shared_ptr<ValidationRule>> validationRules_{};
    bool hasElementName_{false};
    std::string elementName_{};
    std::optional<RelativeSource> relativeSource_{};
    bool isAsync_{false};
};

} // namespace fk::binding
```

## 构造函数与析构函数

### Binding()

```cpp
Binding() = default;
```

默认构造函数，创建一个空的绑定配置对象。

**示例**：
```cpp
fk::Binding binding;
binding.Path("Username").Mode(fk::BindingMode::TwoWay);
```

### ~Binding()

```cpp
virtual ~Binding() = default;
```

虚析构函数，支持派生类（如 `TemplateBinding`）。

---

## 配置方法

所有配置方法都返回 `Binding&` 以支持链式调用。

### Path

```cpp
Binding& Path(std::string path);
```

设置要绑定的属性路径。

**参数**：
| 参数 | 类型 | 描述 |
|------|------|------|
| `path` | `std::string` | 属性路径，支持嵌套属性（如 `"User.Address.City"`） |

**返回值**：当前 `Binding` 对象的引用

**示例**：
```cpp
binding.Path("Username");           // 简单属性
binding.Path("User.Name");          // 嵌套属性
binding.Path("Items[0].Name");      // 索引访问
```

### Source

```cpp
Binding& Source(std::any source);
```

设置数据源对象。如果不设置，将使用目标元素的 DataContext。

**参数**：
| 参数 | 类型 | 描述 |
|------|------|------|
| `source` | `std::any` | 数据源对象，可以是任意类型 |

**返回值**：当前 `Binding` 对象的引用

**示例**：
```cpp
auto viewModel = std::make_shared<MyViewModel>();
binding.Source(viewModel);
```

### Mode

```cpp
Binding& Mode(BindingMode mode);
```

设置绑定模式。

**参数**：
| 参数 | 类型 | 描述 |
|------|------|------|
| `mode` | `BindingMode` | 绑定模式 |

**绑定模式**：
| 模式 | 描述 |
|------|------|
| `OneTime` | 一次性绑定，初始化后不再更新 |
| `OneWay` | 单向绑定，源变更时更新目标（默认） |
| `TwoWay` | 双向绑定，源和目标互相同步 |
| `OneWayToSource` | 反向单向绑定，目标变更时更新源 |

**返回值**：当前 `Binding` 对象的引用

**示例**：
```cpp
binding.Mode(fk::BindingMode::TwoWay);
```

### SetUpdateSourceTrigger

```cpp
Binding& SetUpdateSourceTrigger(UpdateSourceTrigger trigger);
```

设置何时将目标值更新回源（仅对 `TwoWay` 和 `OneWayToSource` 有效）。

**参数**：
| 参数 | 类型 | 描述 |
|------|------|------|
| `trigger` | `UpdateSourceTrigger` | 更新触发器 |

**触发器类型**：
| 触发器 | 描述 |
|--------|------|
| `Default` | 使用属性默认设置 |
| `PropertyChanged` | 属性变更时立即更新 |
| `LostFocus` | 失去焦点时更新 |
| `Explicit` | 显式调用 `UpdateSource()` 时更新 |

**返回值**：当前 `Binding` 对象的引用

**示例**：
```cpp
binding.SetUpdateSourceTrigger(fk::UpdateSourceTrigger::LostFocus);
```

### Converter

```cpp
Binding& Converter(std::shared_ptr<IValueConverter> converter);
```

设置值转换器，在绑定时转换值。

**参数**：
| 参数 | 类型 | 描述 |
|------|------|------|
| `converter` | `std::shared_ptr<IValueConverter>` | 值转换器实例 |

**返回值**：当前 `Binding` 对象的引用

**示例**：
```cpp
binding.Converter(std::make_shared<BoolToStringConverter>());
```

### ConverterParameter

```cpp
Binding& ConverterParameter(std::any parameter);
```

设置传递给转换器的参数。

**参数**：
| 参数 | 类型 | 描述 |
|------|------|------|
| `parameter` | `std::any` | 转换器参数 |

**返回值**：当前 `Binding` 对象的引用

**示例**：
```cpp
binding.Converter(converter)
       .ConverterParameter(std::string("format:yyyy-MM-dd"));
```

### ValidatesOnDataErrors

```cpp
Binding& ValidatesOnDataErrors(bool enable);
```

启用或禁用数据错误验证。

**参数**：
| 参数 | 类型 | 描述 |
|------|------|------|
| `enable` | `bool` | 是否启用验证 |

**返回值**：当前 `Binding` 对象的引用

**示例**：
```cpp
binding.ValidatesOnDataErrors(true);
```

### AddValidationRule

```cpp
Binding& AddValidationRule(std::shared_ptr<ValidationRule> rule);
```

添加验证规则。

**参数**：
| 参数 | 类型 | 描述 |
|------|------|------|
| `rule` | `std::shared_ptr<ValidationRule>` | 验证规则实例 |

**返回值**：当前 `Binding` 对象的引用

**示例**：
```cpp
binding.AddValidationRule(std::make_shared<NotEmptyValidationRule>())
       .AddValidationRule(std::make_shared<EmailValidationRule>());
```

### ElementName

```cpp
Binding& ElementName(std::string name);
```

设置元素名称，绑定到具有指定名称的元素。

**参数**：
| 参数 | 类型 | 描述 |
|------|------|------|
| `name` | `std::string` | 元素名称（通过 `SetElementName` 设置） |

**返回值**：当前 `Binding` 对象的引用

**示例**：
```cpp
// 绑定到名为 "slider1" 的元素的 Value 属性
binding.ElementName("slider1").Path("Value");
```

### SetRelativeSource

```cpp
Binding& SetRelativeSource(RelativeSource relativeSource);
```

设置相对源，绑定到相对于目标元素的元素。

**参数**：
| 参数 | 类型 | 描述 |
|------|------|------|
| `relativeSource` | `RelativeSource` | 相对源配置 |

**返回值**：当前 `Binding` 对象的引用

**示例**：
```cpp
// 绑定到自身
binding.SetRelativeSource(fk::RelativeSource::Self());

// 绑定到祖先元素
binding.SetRelativeSource(fk::RelativeSource::FindAncestor(typeid(Window), 1));
```

### IsAsync

```cpp
Binding& IsAsync(bool async);
```

设置是否异步获取绑定值。

**参数**：
| 参数 | 类型 | 描述 |
|------|------|------|
| `async` | `bool` | 是否异步 |

**返回值**：当前 `Binding` 对象的引用

**示例**：
```cpp
binding.IsAsync(true);
```

---

## Getter 方法

### GetPath

```cpp
[[nodiscard]] const std::string& GetPath() const noexcept;
```

获取属性路径。

**返回值**：属性路径字符串的常量引用

### GetSource

```cpp
[[nodiscard]] const std::any& GetSource() const noexcept;
```

获取数据源对象。

**返回值**：数据源的常量引用

### HasExplicitSource

```cpp
[[nodiscard]] bool HasExplicitSource() const noexcept;
```

检查是否显式设置了数据源。

**返回值**：如果显式设置了数据源返回 `true`

### GetMode

```cpp
[[nodiscard]] BindingMode GetMode() const noexcept;
```

获取绑定模式。

**返回值**：当前绑定模式

### GetConverter

```cpp
[[nodiscard]] const std::shared_ptr<IValueConverter>& GetConverter() const noexcept;
```

获取值转换器。

**返回值**：转换器智能指针的常量引用

### GetValidationRules

```cpp
[[nodiscard]] const std::vector<std::shared_ptr<ValidationRule>>& GetValidationRules() const noexcept;
```

获取所有验证规则。

**返回值**：验证规则向量的常量引用

---

## 虚方法

### IsTemplateBinding

```cpp
[[nodiscard]] virtual bool IsTemplateBinding() const noexcept;
```

检查是否为模板绑定。

**返回值**：基类返回 `false`，`TemplateBinding` 派生类返回 `true`

### Clone

```cpp
[[nodiscard]] virtual std::unique_ptr<Binding> Clone() const;
```

克隆绑定对象。

**返回值**：克隆的绑定对象唯一指针

---

## 创建绑定表达式

### CreateExpression

```cpp
std::shared_ptr<BindingExpression> CreateExpression(DependencyObject* target, const DependencyProperty& targetProperty) const;
```

创建活动的绑定表达式。

**参数**：
| 参数 | 类型 | 描述 |
|------|------|------|
| `target` | `DependencyObject*` | 绑定目标对象 |
| `targetProperty` | `const DependencyProperty&` | 目标属性 |

**返回值**：绑定表达式智能指针

**异常**：如果 `target` 为 `nullptr`，抛出 `std::invalid_argument`

**说明**：通常不需要直接调用此方法，`DependencyObject::SetBinding()` 会自动调用。

---

## 便捷函数

### bind()

```cpp
namespace fk {
    inline Binding bind(std::string path);
}
```

便捷的绑定创建函数。

**参数**：
| 参数 | 类型 | 描述 |
|------|------|------|
| `path` | `std::string` | 属性路径 |

**返回值**：配置了路径的 `Binding` 对象

**示例**：
```cpp
// 使用便捷函数
textBox->SetBinding(TextBox::TextProperty(), fk::bind("Username"));
```

---

## 使用示例

### 基本绑定

```cpp
#include "fk/binding/Binding.h"
#include "fk/binding/DependencyObject.h"

// 创建绑定并应用到 UI 元素
fk::Binding binding;
binding.Path("Username")
       .Source(viewModel);

textBox->SetBinding(TextBox::TextProperty(), binding);
```

### 双向绑定

```cpp
fk::Binding binding;
binding.Path("Age")
       .Source(viewModel)
       .Mode(fk::BindingMode::TwoWay)
       .SetUpdateSourceTrigger(fk::UpdateSourceTrigger::PropertyChanged);

textBox->SetBinding(TextBox::TextProperty(), binding);
```

### 带转换器的绑定

```cpp
// 定义转换器
class BoolToStringConverter : public fk::IValueConverter {
public:
    std::any Convert(const std::any& value, std::type_index targetType, const std::any* parameter) const override {
        bool boolValue = std::any_cast<bool>(value);
        return std::string(boolValue ? "是" : "否");
    }
    
    std::any ConvertBack(const std::any& value, std::type_index sourceType, const std::any* parameter) const override {
        std::string strValue = std::any_cast<std::string>(value);
        return strValue == "是";
    }
};

// 使用转换器
fk::Binding binding;
binding.Path("IsEnabled")
       .Source(viewModel)
       .Converter(std::make_shared<BoolToStringConverter>());

textBlock->SetBinding(TextBlock::TextProperty(), binding);
```

### 使用 DataContext

```cpp
// 设置父元素的 DataContext
window->SetDataContext(viewModel);

// 子元素自动使用继承的 DataContext（无需设置 Source）
textBox->SetBinding(TextBox::TextProperty(), fk::bind("Username"));
```

### 带验证的绑定

```cpp
fk::Binding binding;
binding.Path("Email")
       .Source(viewModel)
       .Mode(fk::BindingMode::TwoWay)
       .ValidatesOnDataErrors(true)
       .AddValidationRule(std::make_shared<fk::NotEmptyValidationRule>("邮箱不能为空"))
       .AddValidationRule(std::make_shared<fk::EmailValidationRule>("邮箱格式无效"));

textBox->SetBinding(TextBox::TextProperty(), binding);
```

### 元素间绑定

```cpp
// Slider 的值绑定到 TextBlock
slider->SetElementName("slider1");

fk::Binding binding;
binding.ElementName("slider1")
       .Path("Value");

textBlock->SetBinding(TextBlock::TextProperty(), binding);
```

### 相对源绑定

```cpp
// 绑定到自身的另一个属性
fk::Binding binding;
binding.SetRelativeSource(fk::RelativeSource::Self())
       .Path("Width");

element->SetBinding(Element::HeightProperty(), binding);  // 高度 = 宽度
```

---

## 私有成员

| 成员 | 类型 | 描述 |
|------|------|------|
| `path_` | `std::string` | 属性路径 |
| `source_` | `std::any` | 数据源对象 |
| `hasExplicitSource_` | `bool` | 是否显式设置了数据源 |
| `mode_` | `BindingMode` | 绑定模式，默认 `OneWay` |
| `hasExplicitMode_` | `bool` | 是否显式设置了绑定模式 |
| `updateSourceTrigger_` | `UpdateSourceTrigger` | 更新源触发器 |
| `hasExplicitUpdateSourceTrigger_` | `bool` | 是否显式设置了触发器 |
| `converter_` | `std::shared_ptr<IValueConverter>` | 值转换器 |
| `converterParameter_` | `std::any` | 转换器参数 |
| `validatesOnDataErrors_` | `bool` | 是否验证数据错误 |
| `validationRules_` | `std::vector<std::shared_ptr<ValidationRule>>` | 验证规则列表 |
| `hasElementName_` | `bool` | 是否设置了元素名称 |
| `elementName_` | `std::string` | 元素名称 |
| `relativeSource_` | `std::optional<RelativeSource>` | 相对源配置 |
| `isAsync_` | `bool` | 是否异步 |

---

## 相关类

- [BindingExpression](BindingExpression.md) - 活动绑定实例
- [IValueConverter](IValueConverter.md) - 值转换器接口
- [DependencyObject](DependencyObject.md) - 绑定目标基类
- [ValidationRule](ValidationRule.md) - 验证规则基类
- [RelativeSource](RelativeSource.md) - 相对源配置
- [TemplateBinding](TemplateBinding.md) - 模板绑定（派生类）

## 另请参阅

- [数据绑定概述](../BINDING_MODULE_OVERVIEW.md)
- [入门指南](../../Guides/GettingStarted.md)
