# BindingMode

## 概览

| 属性 | 值 |
|------|-----|
| **命名空间** | `fk::binding` |
| **头文件** | `fk/binding/DependencyProperty.h` |
| **类型** | 枚举类 (`enum class`) |

## 描述

`BindingMode` 枚举定义数据绑定的数据流方向，决定源对象和目标对象之间数据如何同步。

## 枚举定义

```cpp
namespace fk::binding {

enum class BindingMode {
    OneTime,        // 一次性绑定：仅在绑定建立时更新目标
    OneWay,         // 单向绑定：源变更时更新目标（默认）
    TwoWay,         // 双向绑定：源和目标相互同步
    OneWayToSource  // 反向单向绑定：目标变更时更新源
};

} // namespace fk::binding
```

---

## 枚举值

### OneTime

```cpp
OneTime
```

**一次性绑定**：仅在绑定激活时从源读取一次值并设置到目标，之后不再同步。

**特点**：
- 初始化时从源读取值
- 源变更后目标不会更新
- 目标变更后源不会更新
- 性能最佳，适合静态数据

**使用场景**：
- 显示不会变化的配置项
- 只读的静态标签
- 初始化后不需要更新的数据

---

### OneWay

```cpp
OneWay
```

**单向绑定**：当源属性变更时自动更新目标属性，但目标属性变更不影响源。

**特点**：
- 源 → 目标的单向数据流
- 源变更时目标自动更新
- 目标变更不会影响源
- 大多数绑定的默认模式

**使用场景**：
- 显示模型数据的只读 UI 控件
- 标签、文本显示等
- 状态指示器

---

### TwoWay

```cpp
TwoWay
```

**双向绑定**：源和目标属性相互同步，任一方变更都会更新另一方。

**特点**：
- 源 ↔ 目标的双向数据流
- 源变更时目标更新
- 目标变更时源更新
- 适合可编辑的输入控件

**使用场景**：
- 文本输入框
- 复选框、单选按钮
- 滑块、数值输入
- 任何需要用户编辑的控件

---

### OneWayToSource

```cpp
OneWayToSource
```

**反向单向绑定**：当目标属性变更时更新源属性，但源属性变更不影响目标。

**特点**：
- 目标 → 源的单向数据流
- 目标变更时源更新
- 源变更不会影响目标
- 较少使用的特殊模式

**使用场景**：
- 收集用户输入但不需要显示初始值
- 只写属性
- 特殊的数据采集场景

---

## 数据流方向图

```
┌──────────────────────────────────────────────────────────┐
│                     BindingMode 数据流                   │
├──────────────────────────────────────────────────────────┤
│                                                          │
│  OneTime:                                                │
│  ┌────────┐     初始化     ┌────────┐                   │
│  │  源    │ ─────────────→ │  目标  │                   │
│  └────────┘                └────────┘                   │
│                                                          │
│  OneWay:                                                 │
│  ┌────────┐     持续更新    ┌────────┐                   │
│  │  源    │ ─────────────→ │  目标  │                   │
│  └────────┘                └────────┘                   │
│                                                          │
│  TwoWay:                                                 │
│  ┌────────┐     双向同步    ┌────────┐                   │
│  │  源    │ ←───────────→ │  目标  │                   │
│  └────────┘                └────────┘                   │
│                                                          │
│  OneWayToSource:                                         │
│  ┌────────┐     反向更新    ┌────────┐                   │
│  │  源    │ ←──────────── │  目标  │                   │
│  └────────┘                └────────┘                   │
│                                                          │
└──────────────────────────────────────────────────────────┘
```

---

## 模式对比表

| 模式 | 源→目标 | 目标→源 | 性能 | 适用场景 |
|------|---------|---------|------|----------|
| `OneTime` | 仅初始化 | ✗ | ⭐⭐⭐ 最佳 | 静态数据显示 |
| `OneWay` | ✓ | ✗ | ⭐⭐ 良好 | 只读 UI 显示 |
| `TwoWay` | ✓ | ✓ | ⭐ 一般 | 可编辑输入控件 |
| `OneWayToSource` | ✗ | ✓ | ⭐⭐ 良好 | 数据采集 |

---

## 使用示例

### 示例 1：OneWay 单向绑定（显示用户名）

```cpp
#include <fk/binding/Binding.h>
#include <fk/binding/DependencyProperty.h>

using namespace fk::binding;

// 创建单向绑定显示用户名
auto binding = Binding()
    .SetPath("UserName")
    .SetMode(BindingMode::OneWay);  // 源变更时自动更新显示

// 当 ViewModel 的 UserName 变化时，UI 自动更新
```

### 示例 2：TwoWay 双向绑定（编辑文本）

```cpp
// 创建双向绑定用于文本输入
auto binding = Binding()
    .SetPath("InputText")
    .SetMode(BindingMode::TwoWay);  // 输入和显示双向同步

// 用户在输入框中输入时，ViewModel 的 InputText 属性同步更新
// ViewModel 的 InputText 变化时，输入框内容也同步更新
```

### 示例 3：OneTime 一次性绑定（显示版本号）

```cpp
// 版本号在应用运行期间不会变化
auto binding = Binding()
    .SetPath("AppVersion")
    .SetMode(BindingMode::OneTime);  // 只读取一次

// 性能最佳，因为不需要监听变更
```

### 示例 4：OneWayToSource 反向绑定（收集用户选择）

```cpp
// 只将用户选择同步回 ViewModel，不显示初始值
auto binding = Binding()
    .SetPath("SelectedOption")
    .SetMode(BindingMode::OneWayToSource);

// 用户选择时更新 ViewModel，但 ViewModel 的变化不会反映到 UI
```

### 示例 5：配置默认绑定模式

```cpp
#include <fk/binding/DependencyProperty.h>

// 注册属性时配置默认绑定模式
static const DependencyProperty& TextProperty = DependencyProperty::Register(
    "Text",
    typeid(std::string),
    typeid(MyTextBox),
    PropertyMetadata{
        .defaultValue = std::string{},
        .bindingOptions = BindingOptions{
            .defaultMode = BindingMode::TwoWay,  // 文本框默认使用双向绑定
            .updateSourceTrigger = UpdateSourceTrigger::PropertyChanged
        }
    }
);
```

---

## 性能考虑

1. **优先选择 OneTime**：如果数据不会变化，使用 `OneTime` 避免不必要的监听开销
2. **谨慎使用 TwoWay**：双向绑定需要监听两端变更，性能开销最大
3. **避免过度绑定**：只在需要动态更新的地方使用 `OneWay` 或 `TwoWay`

---

## 最佳实践

1. **根据数据特性选择模式**：
   - 静态数据用 `OneTime`
   - 只读显示用 `OneWay`
   - 可编辑用 `TwoWay`

2. **在属性注册时设置合理默认值**：
   - 文本输入属性默认 `TwoWay`
   - 显示属性默认 `OneWay`

3. **配合 UpdateSourceTrigger 使用**：
   - `TwoWay` + `PropertyChanged` = 即时同步
   - `TwoWay` + `LostFocus` = 失去焦点时同步
   - `TwoWay` + `Explicit` = 手动触发同步

---

## 相关类

- [Binding](Binding.md) - 使用 BindingMode 配置绑定
- [DependencyProperty](DependencyProperty.md) - 注册属性时配置默认绑定模式
- [UpdateSourceTrigger](UpdateSourceTrigger.md) - 控制何时更新源
- [BindingExpression](BindingExpression.md) - 运行时绑定实例
