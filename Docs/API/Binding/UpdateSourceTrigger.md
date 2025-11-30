# UpdateSourceTrigger

## 概览

| 属性 | 值 |
|------|-----|
| **命名空间** | `fk::binding` |
| **头文件** | `fk/binding/DependencyProperty.h` |
| **类型** | 枚举类 (`enum class`) |

## 描述

`UpdateSourceTrigger` 枚举定义在双向绑定（`TwoWay`）或反向绑定（`OneWayToSource`）模式下，何时将目标属性的变更同步回源属性。

## 枚举定义

```cpp
namespace fk::binding {

enum class UpdateSourceTrigger {
    Default,          // 使用属性的默认触发器
    PropertyChanged,  // 属性值变更时立即更新源
    LostFocus,        // 控件失去焦点时更新源
    Explicit          // 仅在显式调用时更新源
};

} // namespace fk::binding
```

---

## 枚举值

### Default

```cpp
Default
```

**使用默认触发器**：使用目标 `DependencyProperty` 在注册时配置的默认 `UpdateSourceTrigger` 值。

**特点**：
- 遵循属性定义时的默认行为
- 通常解析为 `PropertyChanged`
- 允许属性定义者控制更新时机

**使用场景**：
- 不需要覆盖属性默认行为时
- 希望使用控件推荐的更新策略

---

### PropertyChanged

```cpp
PropertyChanged
```

**属性变更时立即更新**：当目标属性值发生变化时，立即将新值同步到源属性。

**特点**：
- 即时同步，响应最快
- 每次值变更都会触发更新
- 适合需要实时反馈的场景
- 可能产生较多更新调用

**使用场景**：
- 滑块、进度条等需要实时反馈的控件
- 实时搜索过滤
- 实时计算显示

---

### LostFocus

```cpp
LostFocus
```

**失去焦点时更新**：当目标控件失去输入焦点时，才将值同步到源属性。

**特点**：
- 延迟更新，减少更新频率
- 用户完成输入后才同步
- 适合文本输入场景
- 减少不必要的中间状态更新

**使用场景**：
- 文本输入框
- 需要验证完整输入的表单字段
- 避免每次按键都触发更新的场景

---

### Explicit

```cpp
Explicit
```

**显式调用时更新**：仅当代码显式调用 `BindingExpression::UpdateSource()` 或 `UpdateSourceExplicitly()` 方法时，才将值同步到源属性。

**特点**：
- 完全由代码控制更新时机
- 最大程度的更新控制
- 适合需要批量提交或验证的场景
- 需要手动触发同步

**使用场景**：
- 表单提交按钮点击时批量更新
- 需要先验证再提交的场景
- 撤销/重做功能实现
- 事务性更新

---

## 更新时机对比图

```
┌──────────────────────────────────────────────────────────────┐
│              UpdateSourceTrigger 更新时机                    │
├──────────────────────────────────────────────────────────────┤
│                                                              │
│  PropertyChanged:                                            │
│  ┌────────┐  值变更  ┌────────┐  立即  ┌────────┐           │
│  │ 用户输入│ ───────→ │ 目标属性│ ─────→ │ 源属性 │           │
│  └────────┘          └────────┘        └────────┘           │
│       ↑                                                      │
│    每次变更都同步                                             │
│                                                              │
│  LostFocus:                                                  │
│  ┌────────┐  多次输入 ┌────────┐ 失焦时 ┌────────┐           │
│  │ 用户输入│ ───────→ │ 目标属性│ ─────→ │ 源属性 │           │
│  └────────┘          └────────┘        └────────┘           │
│       ↑                  ↑                                   │
│    连续输入          焦点移开时同步                            │
│                                                              │
│  Explicit:                                                   │
│  ┌────────┐  多次输入 ┌────────┐ 手动调用 ┌────────┐          │
│  │ 用户输入│ ───────→ │ 目标属性│ ──────→ │ 源属性 │          │
│  └────────┘          └────────┘         └────────┘          │
│       ↑                  ↑                                   │
│    任意输入        调用 UpdateSource() 时同步                  │
│                                                              │
└──────────────────────────────────────────────────────────────┘
```

---

## 触发器对比表

| 触发器 | 更新时机 | 更新频率 | 延迟 | 适用场景 |
|--------|----------|----------|------|----------|
| `Default` | 属性默认 | 取决于属性 | - | 通用 |
| `PropertyChanged` | 值变更时 | 高 | 无 | 实时反馈 |
| `LostFocus` | 失焦时 | 中 | 有 | 文本输入 |
| `Explicit` | 手动调用 | 低 | 可控 | 批量提交 |

---

## 使用示例

### 示例 1：PropertyChanged 实时搜索

```cpp
#include <fk/binding/Binding.h>
#include <fk/binding/DependencyProperty.h>

using namespace fk::binding;

// 搜索框实时过滤：每次输入都触发搜索
auto searchBinding = Binding()
    .SetPath("SearchQuery")
    .SetMode(BindingMode::TwoWay)
    .SetUpdateSourceTrigger(UpdateSourceTrigger::PropertyChanged);

// 用户每输入一个字符，SearchQuery 立即更新
// ViewModel 可以实时执行搜索过滤
```

### 示例 2：LostFocus 表单输入

```cpp
// 用户名输入框：完成输入后才验证
auto usernameBinding = Binding()
    .SetPath("Username")
    .SetMode(BindingMode::TwoWay)
    .SetUpdateSourceTrigger(UpdateSourceTrigger::LostFocus);

// 用户输入完成并点击其他地方后，Username 才更新
// 避免输入过程中频繁触发验证
```

### 示例 3：Explicit 表单提交

```cpp
// 表单字段：点击提交按钮时批量更新
auto emailBinding = Binding()
    .SetPath("Email")
    .SetMode(BindingMode::TwoWay)
    .SetUpdateSourceTrigger(UpdateSourceTrigger::Explicit);

// 绑定激活后获取表达式
auto expr = textBox->GetBindingExpression(TextBox::TextProperty);

// 点击提交按钮时手动同步所有字段
void OnSubmitClicked() {
    // 显式更新源
    expr->UpdateSourceExplicitly();
    
    // 或批量更新多个字段
    for (auto& binding : formBindings) {
        binding->UpdateSourceExplicitly();
    }
}
```

### 示例 4：配置属性默认触发器

```cpp
#include <fk/binding/DependencyProperty.h>

// 注册文本属性，默认使用 LostFocus
static const DependencyProperty& TextProperty = DependencyProperty::Register(
    "Text",
    typeid(std::string),
    typeid(TextBox),
    PropertyMetadata{
        .defaultValue = std::string{},
        .bindingOptions = BindingOptions{
            .defaultMode = BindingMode::TwoWay,
            .updateSourceTrigger = UpdateSourceTrigger::LostFocus  // 文本框默认失焦更新
        }
    }
);

// 注册滑块值属性，默认使用 PropertyChanged
static const DependencyProperty& ValueProperty = DependencyProperty::Register(
    "Value",
    typeid(double),
    typeid(Slider),
    PropertyMetadata{
        .defaultValue = 0.0,
        .bindingOptions = BindingOptions{
            .defaultMode = BindingMode::TwoWay,
            .updateSourceTrigger = UpdateSourceTrigger::PropertyChanged  // 滑块实时更新
        }
    }
);
```

### 示例 5：混合使用不同触发器

```cpp
class SettingsForm {
public:
    void SetupBindings() {
        // 用户名：失焦时更新（避免频繁验证）
        usernameBinding_ = Binding()
            .SetPath("Username")
            .SetMode(BindingMode::TwoWay)
            .SetUpdateSourceTrigger(UpdateSourceTrigger::LostFocus);
        
        // 音量滑块：实时更新（即时反馈）
        volumeBinding_ = Binding()
            .SetPath("Volume")
            .SetMode(BindingMode::TwoWay)
            .SetUpdateSourceTrigger(UpdateSourceTrigger::PropertyChanged);
        
        // 密码：显式更新（提交时验证）
        passwordBinding_ = Binding()
            .SetPath("Password")
            .SetMode(BindingMode::TwoWay)
            .SetUpdateSourceTrigger(UpdateSourceTrigger::Explicit);
    }
    
    void OnSaveClicked() {
        // 只需显式更新密码字段
        passwordExpr_->UpdateSourceExplicitly();
        // 其他字段已自动同步
    }
};
```

---

## 与 BindingMode 的配合

`UpdateSourceTrigger` 仅在以下绑定模式下有效：

| BindingMode | UpdateSourceTrigger 生效 |
|-------------|-------------------------|
| `OneTime` | ✗ 无效（不更新源） |
| `OneWay` | ✗ 无效（不更新源） |
| `TwoWay` | ✓ 有效 |
| `OneWayToSource` | ✓ 有效 |

---

## 性能考虑

1. **PropertyChanged 开销**：每次值变更都同步，频繁输入时可能产生大量更新调用
2. **LostFocus 平衡**：延迟更新减少调用次数，但用户体验略有延迟
3. **Explicit 最优控制**：完全控制更新时机，适合需要优化性能的场景
4. **验证成本**：如果源属性有验证逻辑，`PropertyChanged` 会频繁触发验证

---

## 最佳实践

1. **根据控件类型选择**：
   - 文本框：`LostFocus`（避免每次按键触发）
   - 滑块/进度条：`PropertyChanged`（需要实时反馈）
   - 表单提交：`Explicit`（批量验证和提交）

2. **考虑验证逻辑**：
   - 轻量验证：可用 `PropertyChanged`
   - 复杂验证（如网络请求）：使用 `LostFocus` 或 `Explicit`

3. **在属性注册时设置合理默认值**：
   - 让控件开发者决定最佳更新策略
   - 使用者可通过绑定配置覆盖

4. **Explicit 模式记得手动同步**：
   - 确保在适当时机调用 `UpdateSourceExplicitly()`
   - 考虑在窗口关闭前同步未保存的更改

---

## 相关类

- [Binding](Binding.md) - 使用 UpdateSourceTrigger 配置绑定
- [BindingExpression](BindingExpression.md) - UpdateSourceExplicitly() 方法
- [BindingMode](BindingMode.md) - 控制数据流方向
- [DependencyProperty](DependencyProperty.md) - 注册属性时配置默认触发器
