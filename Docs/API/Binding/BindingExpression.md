# BindingExpression

## 概览

| 属性 | 值 |
|------|-----|
| **命名空间** | `fk::binding` |
| **头文件** | `fk/binding/BindingExpression.h` |
| **源文件** | `src/binding/BindingExpression.cpp` |

## 描述

`BindingExpression` 是 `Binding` 配置的活动实例，负责：

- 解析数据源对象并订阅变更通知
- 在源和目标之间实际传输数据
- 应用值转换器进行类型转换
- 执行验证规则并管理验证错误
- 支持异步更新和显式更新模式
- 支持模板绑定（TemplateBinding）

## 类定义

```cpp
namespace fk::binding {

class BindingExpression : public std::enable_shared_from_this<BindingExpression> {
public:
    BindingExpression(const Binding& definition, DependencyObject* target, const DependencyProperty& property);

    // 生命周期控制
    void Activate();
    void Detach();

    // 更新方法
    void UpdateTarget();
    void UpdateSource();
    void UpdateSourceExplicitly();
    void UpdateTargetAsync();
    void UpdateSourceAsync();

    // 状态查询
    [[nodiscard]] bool IsActive() const noexcept;
    [[nodiscard]] DependencyObject* Target() const noexcept;
    [[nodiscard]] const DependencyProperty& Property() const noexcept;
    [[nodiscard]] const Binding& Definition() const noexcept;
    [[nodiscard]] bool HasValidationErrors() const noexcept;
    [[nodiscard]] const std::vector<ValidationResult>& GetValidationErrors() const noexcept;
    [[nodiscard]] UpdateSourceTrigger GetEffectiveUpdateSourceTrigger() const noexcept;

    // 值应用
    void ApplyTargetValue(std::any value);

    // 事件
    using ValidationErrorsChangedEvent = core::Event<const std::vector<ValidationResult>&>;
    ValidationErrorsChangedEvent ValidationErrorsChanged;

private:
    Binding definition_;
    BindingPath path_;
    DependencyObject* target_{nullptr};
    const DependencyProperty* property_{nullptr};
    bool isActive_{false};
    bool isTemplateBinding_{false};
    const DependencyProperty* templateBindingSourceProperty_{nullptr};
    bool isUpdatingTarget_{false};
    bool isUpdatingSource_{false};
    BindingMode effectiveMode_{BindingMode::OneWay};
    UpdateSourceTrigger effectiveUpdateSourceTrigger_{UpdateSourceTrigger::PropertyChanged};
    // ... 连接和订阅相关成员
    std::vector<ValidationResult> validationErrors_{};
    bool hasPendingTargetUpdate_{false};
    bool hasPendingSourceUpdate_{false};
};

} // namespace fk::binding
```

## 构造函数

### BindingExpression()

```cpp
BindingExpression(const Binding& definition, DependencyObject* target, const DependencyProperty& property);
```

构造一个绑定表达式实例。

**参数**：
| 参数 | 类型 | 描述 |
|------|------|------|
| `definition` | `const Binding&` | 绑定配置对象 |
| `target` | `DependencyObject*` | 绑定目标对象 |
| `property` | `const DependencyProperty&` | 目标属性 |

**异常**：如果 `target` 为 `nullptr`，抛出 `std::invalid_argument`

**说明**：
- 通常不直接调用，而是通过 `Binding::CreateExpression()` 或 `DependencyObject::SetBinding()` 创建
- 构造时会初始化有效设置（绑定模式和更新触发器）

**示例**：
```cpp
// 通常通过 SetBinding 自动创建
element->SetBinding(TextBox::TextProperty(), binding);

// 或手动创建（不推荐）
auto expr = std::make_shared<BindingExpression>(binding, target, property);
expr->Activate();
```

---

## 生命周期方法

### Activate

```cpp
void Activate();
```

激活绑定表达式，开始监听源变更并执行初始更新。

**说明**：
- 如果已激活，不执行任何操作
- 订阅数据源的变更通知
- 执行初始的 `UpdateTarget()` 同步目标值

**示例**：
```cpp
auto expr = binding.CreateExpression(target, property);
expr->Activate();  // 开始数据同步
```

### Detach

```cpp
void Detach();
```

分离绑定表达式，停止监听并清理资源。

**说明**：
- 如果未激活，不执行任何操作
- 断开所有事件订阅
- 清除缓存的源对象引用

**示例**：
```cpp
expr->Detach();  // 停止数据同步
```

---

## 更新方法

### UpdateTarget

```cpp
void UpdateTarget();
```

从源更新目标属性值。

**说明**：
- 如果绑定未激活或模式为 `OneWayToSource`，不执行任何操作
- 解析源对象并通过属性路径获取值
- 应用值转换器（如果设置）
- 将转换后的值应用到目标属性

**示例**：
```cpp
// 手动刷新目标值
if (expr->IsActive()) {
    expr->UpdateTarget();
}
```

### UpdateSource

```cpp
void UpdateSource();
```

从目标更新源属性值。

**说明**：
- 如果绑定未激活或模式为 `OneWay`/`OneTime`，不执行任何操作
- 获取目标属性的当前值
- 应用值转换器的 `ConvertBack`（如果设置）
- 执行验证规则
- 如果验证通过，将值写回源

**示例**：
```cpp
// 手动同步到源
if (expr->IsActive()) {
    expr->UpdateSource();
}
```

### UpdateSourceExplicitly

```cpp
void UpdateSourceExplicitly();
```

显式触发源更新，仅在 `UpdateSourceTrigger::Explicit` 模式下有效。

**说明**：
- 如果绑定未激活或触发器不是 `Explicit`，不执行任何操作
- 内部调用 `UpdateSource()`

**示例**：
```cpp
// 设置显式更新模式的绑定
fk::Binding binding;
binding.Path("Name")
       .Mode(fk::BindingMode::TwoWay)
       .SetUpdateSourceTrigger(fk::UpdateSourceTrigger::Explicit);

textBox->SetBinding(TextBox::TextProperty(), binding);

// 用户点击保存按钮时更新
saveButton->OnClick([&]() {
    auto expr = textBox->GetBinding(TextBox::TextProperty());
    if (expr) {
        expr->UpdateSourceExplicitly();
    }
});
```

### UpdateTargetAsync

```cpp
void UpdateTargetAsync();
```

异步更新目标属性值。

**说明**：
- 如果绑定未激活或未设置异步模式，回退到同步更新
- 避免重复的待处理更新
- 适用于耗时的数据获取操作

**示例**：
```cpp
// 设置异步绑定
fk::Binding binding;
binding.Path("LargeData")
       .IsAsync(true);

element->SetBinding(property, binding);
```

### UpdateSourceAsync

```cpp
void UpdateSourceAsync();
```

异步更新源属性值。

**说明**：
- 如果绑定未激活或未设置异步模式，回退到同步更新
- 避免重复的待处理更新

---

## 状态查询方法

### IsActive

```cpp
[[nodiscard]] bool IsActive() const noexcept;
```

检查绑定表达式是否已激活。

**返回值**：如果绑定已激活并正在同步数据，返回 `true`

**示例**：
```cpp
if (expr->IsActive()) {
    std::cout << "绑定正在活动" << std::endl;
}
```

### Target

```cpp
[[nodiscard]] DependencyObject* Target() const noexcept;
```

获取绑定目标对象。

**返回值**：目标对象指针

### Property

```cpp
[[nodiscard]] const DependencyProperty& Property() const noexcept;
```

获取绑定的目标属性。

**返回值**：目标属性的常量引用

### Definition

```cpp
[[nodiscard]] const Binding& Definition() const noexcept;
```

获取绑定配置定义。

**返回值**：`Binding` 对象的常量引用

### HasValidationErrors

```cpp
[[nodiscard]] bool HasValidationErrors() const noexcept;
```

检查是否存在验证错误。

**返回值**：如果有验证错误返回 `true`

**示例**：
```cpp
if (expr->HasValidationErrors()) {
    for (const auto& error : expr->GetValidationErrors()) {
        std::cout << "验证错误: " << error.errorMessage << std::endl;
    }
}
```

### GetValidationErrors

```cpp
[[nodiscard]] const std::vector<ValidationResult>& GetValidationErrors() const noexcept;
```

获取所有验证错误。

**返回值**：验证结果向量的常量引用

### GetEffectiveUpdateSourceTrigger

```cpp
[[nodiscard]] UpdateSourceTrigger GetEffectiveUpdateSourceTrigger() const noexcept;
```

获取实际生效的更新源触发器。

**返回值**：实际使用的 `UpdateSourceTrigger` 值

**说明**：这是根据绑定配置和属性元数据计算出的有效值

---

## 值应用方法

### ApplyTargetValue

```cpp
void ApplyTargetValue(std::any value);
```

将值应用到目标属性。

**参数**：
| 参数 | 类型 | 描述 |
|------|------|------|
| `value` | `std::any` | 要应用的值 |

**说明**：
- 如果绑定未激活，不执行任何操作
- 内部调用 `DependencyObject::ApplyBindingValue()`
- 防止递归更新

---

## 事件

### ValidationErrorsChanged

```cpp
using ValidationErrorsChangedEvent = core::Event<const std::vector<ValidationResult>&>;
ValidationErrorsChangedEvent ValidationErrorsChanged;
```

验证错误变更时触发的事件。

**参数**：当前的验证错误列表

**示例**：
```cpp
expr->ValidationErrorsChanged += [](const std::vector<fk::ValidationResult>& errors) {
    if (errors.empty()) {
        std::cout << "验证通过" << std::endl;
    } else {
        for (const auto& error : errors) {
            std::cout << "错误: " << error.errorMessage << std::endl;
        }
    }
};
```

---

## 绑定生命周期

```
1. 创建 BindingExpression
   └─ 通过 Binding::CreateExpression() 或 DependencyObject::SetBinding()
   └─ 初始化有效设置（绑定模式、更新触发器）
   ↓
2. 激活 Activate()
   └─ 订阅 DataContext 变更（如果使用继承的数据上下文）
   └─ 订阅源对象的 PropertyChanged 事件
   └─ 订阅目标属性变更（对于 TwoWay/OneWayToSource）
   └─ 执行初始 UpdateTarget()
   ↓
3. 活动状态
   └─ 源变更 → PropertyChanged → UpdateTarget() → 更新 UI
   └─ UI 变更 → PropertyChanged → UpdateSource() → 更新源（TwoWay 模式）
   ↓
4. 分离 Detach()
   └─ 断开所有事件订阅
   └─ 清理缓存的源引用
   └─ 设置 isActive_ = false
```

---

## 使用示例

### 基本用法

```cpp
#include "fk/binding/Binding.h"
#include "fk/binding/BindingExpression.h"

// 设置绑定（自动创建并激活 BindingExpression）
fk::Binding binding;
binding.Path("Username").Source(viewModel);
textBox->SetBinding(TextBox::TextProperty(), binding);

// 获取绑定表达式
auto expr = textBox->GetBinding(TextBox::TextProperty());
if (expr && expr->IsActive()) {
    std::cout << "绑定已激活" << std::endl;
}
```

### 显式更新模式

```cpp
// 设置显式更新的绑定
fk::Binding binding;
binding.Path("Email")
       .Source(viewModel)
       .Mode(fk::BindingMode::TwoWay)
       .SetUpdateSourceTrigger(fk::UpdateSourceTrigger::Explicit);

textBox->SetBinding(TextBox::TextProperty(), binding);

// 保存按钮点击时手动更新源
saveButton->OnClick([textBox]() {
    auto expr = textBox->GetBinding(TextBox::TextProperty());
    if (expr) {
        expr->UpdateSourceExplicitly();
        
        // 检查验证结果
        if (expr->HasValidationErrors()) {
            std::cout << "保存失败，存在验证错误" << std::endl;
        } else {
            std::cout << "保存成功" << std::endl;
        }
    }
});
```

### 监听验证错误

```cpp
fk::Binding binding;
binding.Path("Age")
       .Source(viewModel)
       .Mode(fk::BindingMode::TwoWay)
       .AddValidationRule(std::make_shared<fk::RangeValidationRule<int>>(0, 150, "年龄必须在 0-150 之间"));

textBox->SetBinding(TextBox::TextProperty(), binding);

// 监听验证错误变更
auto expr = textBox->GetBinding(TextBox::TextProperty());
if (expr) {
    expr->ValidationErrorsChanged += [](const std::vector<fk::ValidationResult>& errors) {
        if (!errors.empty()) {
            // 显示错误提示
            for (const auto& error : errors) {
                showErrorTooltip(error.errorMessage);
            }
        } else {
            // 隐藏错误提示
            hideErrorTooltip();
        }
    };
}
```

### 手动刷新绑定

```cpp
// 当外部数据变更时手动刷新
void OnExternalDataChanged() {
    auto expr = textBlock->GetBinding(TextBlock::TextProperty());
    if (expr && expr->IsActive()) {
        expr->UpdateTarget();  // 强制从源刷新
    }
}
```

---

## 私有成员

| 成员 | 类型 | 描述 |
|------|------|------|
| `definition_` | `Binding` | 绑定配置定义 |
| `path_` | `BindingPath` | 解析后的属性路径 |
| `target_` | `DependencyObject*` | 绑定目标对象 |
| `property_` | `const DependencyProperty*` | 目标属性 |
| `isActive_` | `bool` | 是否已激活 |
| `isTemplateBinding_` | `bool` | 是否为模板绑定 |
| `templateBindingSourceProperty_` | `const DependencyProperty*` | 模板绑定的源属性 |
| `isUpdatingTarget_` | `bool` | 是否正在更新目标（防止递归） |
| `isUpdatingSource_` | `bool` | 是否正在更新源（防止递归） |
| `effectiveMode_` | `BindingMode` | 实际生效的绑定模式 |
| `effectiveUpdateSourceTrigger_` | `UpdateSourceTrigger` | 实际生效的更新触发器 |
| `dataContextConnection_` | `Connection` | DataContext 变更事件连接 |
| `targetPropertyConnection_` | `Connection` | 目标属性变更事件连接 |
| `sourcePropertyConnection_` | `Connection` | 源属性变更事件连接 |
| `sourceDependencyObjectConnection_` | `Connection` | 源 DependencyObject 变更事件连接 |
| `currentSource_` | `std::any` | 缓存的当前源对象 |
| `sharedNotifierHolder_` | `std::shared_ptr<INotifyPropertyChanged>` | 持有的通知器（防止释放） |
| `rawNotifier_` | `INotifyPropertyChanged*` | 原始通知器指针 |
| `validationErrors_` | `std::vector<ValidationResult>` | 当前验证错误列表 |
| `hasPendingTargetUpdate_` | `bool` | 是否有待处理的目标更新 |
| `hasPendingSourceUpdate_` | `bool` | 是否有待处理的源更新 |

---

## 相关类

- [Binding](Binding.md) - 绑定配置
- [DependencyObject](DependencyObject.md) - 绑定目标基类
- [DependencyProperty](DependencyProperty.md) - 依赖属性
- [INotifyPropertyChanged](INotifyPropertyChanged.md) - 属性变更通知接口
- [ValidationRule](ValidationRule.md) - 验证规则基类
- [BindingPath](BindingPath.md) - 属性路径解析

## 另请参阅

- [数据绑定概述](../BINDING_MODULE_OVERVIEW.md)
- [入门指南](../../Guides/GettingStarted.md)
