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
- 使用多值转换器（`IMultiValueConverter`）合并值
- 将合并后的值更新到目标属性
- 订阅和响应源属性变更通知

## 类定义

```cpp
namespace fk::binding {

class MultiBindingExpression : public std::enable_shared_from_this<MultiBindingExpression> {
public:
    MultiBindingExpression(
        MultiBinding definition,
        DependencyObject* target,
        const DependencyProperty& property);

    // 生命周期方法
    void Activate();
    void Detach();
    void UpdateTarget();

    // 状态查询方法
    [[nodiscard]] bool IsActive() const noexcept;
    [[nodiscard]] DependencyObject* Target() const noexcept;
    [[nodiscard]] const DependencyProperty& Property() const noexcept;
    [[nodiscard]] const MultiBinding& Definition() const noexcept;

private:
    void EnsureTargetAlive() const;
    void InitializeEffectiveSettings();
    void SubscribeToChildren();
    void UnsubscribeFromChildren();
    void OnChildBindingUpdated();
    void ApplyTargetValue(std::any value);
    std::vector<std::any> CollectSourceValues() const;

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

**说明**：
- 构造函数会调用 `InitializeEffectiveSettings()` 初始化有效设置
- 不会自动激活，需要显式调用 `Activate()`

**示例**：
```cpp
fk::MultiBinding multi;
multi.AddBinding(fk::bind("FirstName"))
     .AddBinding(fk::bind("LastName"))
     .Converter(std::make_shared<FullNameConverter>());

auto expr = std::make_shared<fk::MultiBindingExpression>(
    multi, textBlock, TextBlock::TextProperty());
expr->Activate();
```

---

## 生命周期方法

### Activate

```cpp
void Activate();
```

激活多绑定表达式。

**说明**：
- 如果已激活，不执行任何操作
- 为每个子绑定创建 `BindingExpression` 并激活
- 订阅所有子绑定源的 `PropertyChanged` 事件
- 执行初始的 `UpdateTarget()` 更新目标值

**示例**：
```cpp
auto expr = multi.CreateExpression(target, property);
expr->Activate();
// 现在多绑定已激活，源变更会自动更新目标
```

### Detach

```cpp
void Detach();
```

分离多绑定表达式。

**说明**：
- 如果未激活，不执行任何操作
- 断开所有子绑定的事件订阅
- 分离并清除所有子 `BindingExpression`
- 设置 `isActive_ = false`

**示例**：
```cpp
expr->Detach();
// 现在多绑定不再响应源变更
```

### UpdateTarget

```cpp
void UpdateTarget();
```

从所有源更新目标属性值。

**说明**：
- 如果绑定模式为 `OneWayToSource`，不执行更新
- 收集所有子绑定的源值（通过 `CollectSourceValues()`）
- 如果设置了转换器，调用 `IMultiValueConverter::Convert()` 合并值
- 如果没有转换器，使用第一个绑定的值
- 将合并后的值应用到目标属性

**示例**：
```cpp
// 手动触发更新（通常不需要，源变更会自动触发）
expr->UpdateTarget();
```

---

## 状态查询方法

### IsActive

```cpp
[[nodiscard]] bool IsActive() const noexcept;
```

检查多绑定表达式是否已激活。

**返回值**：如果已激活返回 `true`

**示例**：
```cpp
if (expr->IsActive()) {
    std::cout << "多绑定已激活" << std::endl;
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

获取目标属性。

**返回值**：目标依赖属性的常量引用

### Definition

```cpp
[[nodiscard]] const MultiBinding& Definition() const noexcept;
```

获取多绑定配置定义。

**返回值**：`MultiBinding` 的常量引用

---

## 生命周期流程

```
┌─────────────────────────────────────────────────────────────┐
│                    MultiBindingExpression                   │
└─────────────────────────────────────────────────────────────┘
                              │
                              ▼
┌─────────────────────────────────────────────────────────────┐
│  构造函数                                                    │
│  - 保存 definition, target, property                        │
│  - InitializeEffectiveSettings() 计算有效设置               │
└─────────────────────────────────────────────────────────────┘
                              │
                              ▼
┌─────────────────────────────────────────────────────────────┐
│  Activate()                                                  │
│  - 为每个子绑定创建 BindingExpression                        │
│  - 激活所有子绑定                                            │
│  - SubscribeToChildren() 订阅源变更                         │
│  - UpdateTarget() 初始更新                                   │
└─────────────────────────────────────────────────────────────┘
                              │
                              ▼
┌─────────────────────────────────────────────────────────────┐
│  源属性变更                                                  │
│  - 子绑定的源触发 PropertyChanged                           │
│  - OnChildBindingUpdated() 被调用                           │
│  - UpdateTarget() 重新收集并更新目标                        │
└─────────────────────────────────────────────────────────────┘
                              │
                              ▼
┌─────────────────────────────────────────────────────────────┐
│  UpdateTarget()                                              │
│  - CollectSourceValues() 收集所有源值                       │
│  - 转换器 Convert() 合并值                                  │
│  - ApplyTargetValue() 应用到目标                            │
└─────────────────────────────────────────────────────────────┘
                              │
                              ▼
┌─────────────────────────────────────────────────────────────┐
│  Detach()                                                    │
│  - UnsubscribeFromChildren() 断开订阅                       │
│  - 分离所有子 BindingExpression                             │
│  - isActive_ = false                                         │
└─────────────────────────────────────────────────────────────┘
```

---

## 使用示例

### 组合姓名

```cpp
#include "fk/binding/MultiBinding.h"
#include "fk/binding/MultiBindingExpression.h"

// 自定义转换器
class FullNameConverter : public fk::IMultiValueConverter {
public:
    std::any Convert(
        const std::vector<std::any>& values,
        std::type_index,
        const std::any*) const override {
        
        if (values.size() < 2) return std::string{};
        
        std::string first = values[0].has_value() 
            ? std::any_cast<std::string>(values[0]) : "";
        std::string last = values[1].has_value() 
            ? std::any_cast<std::string>(values[1]) : "";
        
        return first + " " + last;
    }
};

// 创建并使用
fk::MultiBinding multi;
multi.AddBinding(fk::bind("FirstName"))
     .AddBinding(fk::bind("LastName"))
     .Converter(std::make_shared<FullNameConverter>());

auto expr = multi.CreateExpression(textBlock, TextBlock::TextProperty());
expr->Activate();

// 设置数据上下文
textBlock->SetDataContext(viewModel);

// 现在当 FirstName 或 LastName 变更时，textBlock 会自动更新
viewModel->firstName = "张";  // 触发更新
viewModel->lastName = "三";   // 触发更新
// textBlock 显示 "张 三"
```

### 计算绑定

```cpp
class TotalPriceConverter : public fk::IMultiValueConverter {
public:
    std::any Convert(
        const std::vector<std::any>& values,
        std::type_index,
        const std::any*) const override {
        
        if (values.size() < 2) return 0.0;
        
        double price = values[0].has_value() 
            ? std::any_cast<double>(values[0]) : 0.0;
        int quantity = values[1].has_value() 
            ? std::any_cast<int>(values[1]) : 0;
        
        return price * quantity;
    }
};

fk::MultiBinding totalBinding;
totalBinding.AddBinding(fk::bind("UnitPrice"))
            .AddBinding(fk::bind("Quantity"))
            .Converter(std::make_shared<TotalPriceConverter>());

auto expr = totalBinding.CreateExpression(totalLabel, Label::ContentProperty());
expr->Activate();

// 价格或数量变化时自动计算总价
```

### 检查多绑定状态

```cpp
auto expr = multi.CreateExpression(target, property);

std::cout << "激活前: " << expr->IsActive() << std::endl;  // false

expr->Activate();
std::cout << "激活后: " << expr->IsActive() << std::endl;  // true

// 获取配置信息
const auto& def = expr->Definition();
std::cout << "子绑定数量: " << def.GetBindings().size() << std::endl;

expr->Detach();
std::cout << "分离后: " << expr->IsActive() << std::endl;  // false
```

---

## 私有方法

### EnsureTargetAlive

```cpp
void EnsureTargetAlive() const;
```

确保目标对象有效，如果 `target_` 为 `nullptr` 则抛出异常。

### InitializeEffectiveSettings

```cpp
void InitializeEffectiveSettings();
```

根据 `MultiBinding` 配置和属性元数据计算有效的绑定模式和更新触发器。

### SubscribeToChildren

```cpp
void SubscribeToChildren();
```

订阅所有子绑定源的 `PropertyChanged` 事件。

### UnsubscribeFromChildren

```cpp
void UnsubscribeFromChildren();
```

断开所有子绑定的事件订阅。

### OnChildBindingUpdated

```cpp
void OnChildBindingUpdated();
```

子绑定源变更时的回调，触发 `UpdateTarget()`。

### ApplyTargetValue

```cpp
void ApplyTargetValue(std::any value);
```

将值应用到目标属性，使用 `ScopedFlag` 防止递归更新。

### CollectSourceValues

```cpp
std::vector<std::any> CollectSourceValues() const;
```

从所有子绑定收集当前源值。

---

## 私有成员

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

## 性能考虑

1. **批量更新**：当多个源同时变更时，考虑使用去抖动或批量更新策略
2. **转换器缓存**：复杂转换器可以实现结果缓存
3. **弱引用**：使用 `weak_from_this()` 防止循环引用
4. **递归保护**：`isUpdatingTarget_` 标志防止递归更新

---

## 相关类

- [MultiBinding](MultiBinding.md) - 多绑定配置类
- [IMultiValueConverter](MultiBinding.md#imultivalueconverter) - 多值转换器接口
- [BindingExpression](BindingExpression.md) - 单一绑定表达式
- [DependencyObject](DependencyObject.md) - 绑定目标基类

## 另请参阅

- [数据绑定概述](../BINDING_MODULE_OVERVIEW.md)
- [入门指南](../../Guides/GettingStarted.md)
