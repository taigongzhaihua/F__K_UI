# BindingContext

## 概览

| 属性 | 值 |
|------|-----|
| **命名空间** | `fk::binding` |
| **头文件** | `fk/binding/BindingContext.h` |
| **源文件** | `src/binding/BindingContext.cpp` |

## 描述

`BindingContext` 负责管理 `DependencyObject` 的数据上下文（DataContext），实现：

- 本地数据上下文的设置和清除
- 从父元素继承数据上下文
- 有效数据上下文的计算（本地优先于继承）
- 数据上下文变更的通知

## 类定义

```cpp
namespace fk::binding {

class BindingContext {
public:
    using DataContextChangedEvent = core::Event<const std::any&, const std::any&>;

    explicit BindingContext(DependencyObject& owner);
    ~BindingContext();

    // 数据上下文获取
    const std::any& GetDataContext() const noexcept;
    bool HasDataContext() const noexcept;
    bool HasLocalDataContext() const noexcept;

    // 本地数据上下文管理
    void SetLocalDataContext(std::any value);
    void ClearLocalDataContext();

    // 父级上下文管理
    void SetParent(BindingContext* parent);
    BindingContext* Parent() const noexcept;

    // 事件
    DataContextChangedEvent DataContextChanged;

private:
    DependencyObject& owner_;
    BindingContext* parent_{nullptr};
    DataContextChangedEvent::Connection parentConnection_{};

    std::any localValue_;
    bool hasLocal_{false};

    std::any inheritedValue_;
    bool hasInherited_{false};

    std::any effectiveValue_;
    bool hasEffective_{false};
};

} // namespace fk::binding
```

## 构造函数与析构函数

### BindingContext()

```cpp
explicit BindingContext(DependencyObject& owner);
```

构造一个绑定上下文实例。

**参数**：
| 参数 | 类型 | 描述 |
|------|------|------|
| `owner` | `DependencyObject&` | 拥有此上下文的依赖对象 |

**说明**：
- 由 `DependencyObject` 内部创建和管理
- 通常不需要手动构造

### ~BindingContext()

```cpp
~BindingContext();
```

析构函数，断开与父上下文的连接。

---

## 数据上下文获取方法

### GetDataContext

```cpp
const std::any& GetDataContext() const noexcept;
```

获取当前有效的数据上下文。

**返回值**：有效数据上下文的常量引用，如果没有则返回空的 `std::any`

**说明**：
- 如果设置了本地数据上下文，返回本地值
- 否则返回从父级继承的值
- 如果都没有，返回空值

**示例**：
```cpp
const auto& dataContext = element->GetBindingContext().GetDataContext();
if (dataContext.has_value()) {
    auto* viewModel = std::any_cast<MyViewModel*>(dataContext);
    // 使用 viewModel
}
```

### HasDataContext

```cpp
bool HasDataContext() const noexcept;
```

检查是否有有效的数据上下文（本地或继承）。

**返回值**：如果有有效数据上下文返回 `true`

**示例**：
```cpp
if (element->GetBindingContext().HasDataContext()) {
    // 有数据上下文可用
}
```

### HasLocalDataContext

```cpp
bool HasLocalDataContext() const noexcept;
```

检查是否设置了本地数据上下文。

**返回值**：如果设置了本地数据上下文返回 `true`

**示例**：
```cpp
if (element->GetBindingContext().HasLocalDataContext()) {
    // 元素有自己的数据上下文，不是继承的
}
```

---

## 本地数据上下文管理方法

### SetLocalDataContext

```cpp
void SetLocalDataContext(std::any value);
```

设置本地数据上下文。

**参数**：
| 参数 | 类型 | 描述 |
|------|------|------|
| `value` | `std::any` | 数据上下文值 |

**说明**：
- 本地数据上下文优先于继承的数据上下文
- 设置后会触发 `DataContextChanged` 事件
- 子元素会自动继承新的数据上下文

**示例**：
```cpp
auto viewModel = std::make_shared<MyViewModel>();
element->GetBindingContext().SetLocalDataContext(viewModel);
```

### ClearLocalDataContext

```cpp
void ClearLocalDataContext();
```

清除本地数据上下文。

**说明**：
- 如果没有本地数据上下文，不执行任何操作
- 清除后，有效数据上下文将回退到继承的值
- 会触发 `DataContextChanged` 事件（如果有效值发生变化）

**示例**：
```cpp
element->GetBindingContext().ClearLocalDataContext();
// 现在元素使用继承的数据上下文
```

---

## 父级上下文管理方法

### SetParent

```cpp
void SetParent(BindingContext* parent);
```

设置父级绑定上下文，用于数据上下文继承。

**参数**：
| 参数 | 类型 | 描述 |
|------|------|------|
| `parent` | `BindingContext*` | 父级绑定上下文，可以为 `nullptr` |

**说明**：
- 断开与旧父级的连接
- 订阅新父级的 `DataContextChanged` 事件
- 更新继承的数据上下文
- 如果父级为 `nullptr`，清除继承的数据上下文

**示例**：
```cpp
// 通常由 DependencyObject 在设置逻辑父级时调用
child->GetBindingContext().SetParent(&parent->GetBindingContext());
```

### Parent

```cpp
BindingContext* Parent() const noexcept;
```

获取父级绑定上下文。

**返回值**：父级绑定上下文指针，如果没有则返回 `nullptr`

**示例**：
```cpp
auto* parentContext = element->GetBindingContext().Parent();
if (parentContext && parentContext->HasDataContext()) {
    // 父级有数据上下文
}
```

---

## 事件

### DataContextChanged

```cpp
using DataContextChangedEvent = core::Event<const std::any&, const std::any&>;
DataContextChangedEvent DataContextChanged;
```

数据上下文变更时触发的事件。

**参数**：
| 参数 | 描述 |
|------|------|
| 第一个参数 | 旧的数据上下文值 |
| 第二个参数 | 新的数据上下文值 |

**示例**：
```cpp
element->GetBindingContext().DataContextChanged += [](const std::any& oldValue, const std::any& newValue) {
    if (newValue.has_value()) {
        std::cout << "数据上下文已设置" << std::endl;
    } else {
        std::cout << "数据上下文已清除" << std::endl;
    }
};
```

---

## 数据上下文继承机制

```
Window (DataContext = ViewModel)
│   └─ BindingContext: effectiveValue_ = ViewModel
│
├── StackPanel (无本地 DataContext)
│   │   └─ BindingContext: inheritedValue_ = ViewModel, effectiveValue_ = ViewModel
│   │
│   ├── TextBox (无本地 DataContext)
│   │       └─ BindingContext: inheritedValue_ = ViewModel, effectiveValue_ = ViewModel
│   │
│   └── Button (本地 DataContext = ButtonViewModel)
│           └─ BindingContext: localValue_ = ButtonViewModel, effectiveValue_ = ButtonViewModel
│
└── Grid (无本地 DataContext)
        └─ BindingContext: inheritedValue_ = ViewModel, effectiveValue_ = ViewModel
```

**优先级规则**：
1. 本地数据上下文（`localValue_`）优先级最高
2. 如果没有本地值，使用继承的数据上下文（`inheritedValue_`）
3. 有效值（`effectiveValue_`）是最终使用的值

---

## 使用示例

### 设置数据上下文

```cpp
#include "fk/binding/DependencyObject.h"

// 创建 ViewModel
auto viewModel = std::make_shared<MyViewModel>();

// 在根元素设置数据上下文
window->SetDataContext(viewModel);  // 内部调用 BindingContext::SetLocalDataContext

// 子元素自动继承
// textBox->GetBindingContext().GetDataContext() 返回 viewModel
```

### 监听数据上下文变更

```cpp
element->GetBindingContext().DataContextChanged += [](const std::any& oldValue, const std::any& newValue) {
    // 处理数据上下文变更
    if (newValue.has_value()) {
        // 重新绑定或更新 UI
    }
};
```

### 检查数据上下文来源

```cpp
auto& context = element->GetBindingContext();

if (context.HasLocalDataContext()) {
    std::cout << "使用本地数据上下文" << std::endl;
} else if (context.HasDataContext()) {
    std::cout << "使用继承的数据上下文" << std::endl;
} else {
    std::cout << "没有数据上下文" << std::endl;
}
```

### 覆盖继承的数据上下文

```cpp
// 窗口设置主 ViewModel
window->SetDataContext(mainViewModel);

// 特定区域使用不同的 ViewModel
settingsPanel->SetDataContext(settingsViewModel);
// settingsPanel 及其子元素现在使用 settingsViewModel

// 清除本地设置，回退到继承
settingsPanel->ClearDataContext();
// settingsPanel 现在使用 mainViewModel
```

---

## 私有成员

| 成员 | 类型 | 描述 |
|------|------|------|
| `owner_` | `DependencyObject&` | 拥有此上下文的依赖对象 |
| `parent_` | `BindingContext*` | 父级绑定上下文 |
| `parentConnection_` | `Connection` | 父级 DataContextChanged 事件连接 |
| `localValue_` | `std::any` | 本地设置的数据上下文值 |
| `hasLocal_` | `bool` | 是否设置了本地数据上下文 |
| `inheritedValue_` | `std::any` | 从父级继承的数据上下文值 |
| `hasInherited_` | `bool` | 是否有继承的数据上下文 |
| `effectiveValue_` | `std::any` | 最终有效的数据上下文值 |
| `hasEffective_` | `bool` | 是否有有效的数据上下文 |

---

## 私有方法

### UpdateEffective

```cpp
void UpdateEffective();
```

更新有效数据上下文值，根据优先级选择本地或继承的值，并在变更时触发事件。

### OnParentDataContextChanged

```cpp
void OnParentDataContextChanged(const std::any& oldValue, const std::any& newValue);
```

父级数据上下文变更时的回调，更新继承的值并重新计算有效值。

### AreEquivalent

```cpp
static bool AreEquivalent(const std::any& lhs, const std::any& rhs);
```

比较两个 `std::any` 值是否等价，用于确定是否需要触发变更事件。

---

## 相关类

- [DependencyObject](DependencyObject.md) - 拥有 BindingContext 的基类
- [BindingExpression](BindingExpression.md) - 使用数据上下文解析绑定源
- [Binding](Binding.md) - 绑定配置

## 另请参阅

- [数据绑定概述](../BINDING_MODULE_OVERVIEW.md)
- [入门指南](../../Guides/GettingStarted.md)
