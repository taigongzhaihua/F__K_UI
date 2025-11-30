# BindingOptions

## 概览

| 属性 | 值 |
|------|-----|
| **命名空间** | `fk::binding` |
| **头文件** | `fk/binding/DependencyProperty.h` |
| **类型** | 结构体 |

## 描述

`BindingOptions` 是一个结构体，用于配置依赖属性的绑定行为。它定义了属性的默认绑定模式、源更新触发器以及是否继承数据上下文。

## 结构体定义

```cpp
namespace fk::binding {

struct BindingOptions {
    BindingMode defaultMode{BindingMode::OneWay};
    UpdateSourceTrigger updateSourceTrigger{UpdateSourceTrigger::PropertyChanged};
    bool inheritsDataContext{false};
};

} // namespace fk::binding
```

---

## 成员说明

### defaultMode

```cpp
BindingMode defaultMode{BindingMode::OneWay};
```

属性的默认绑定模式。当绑定未显式指定模式时使用此值。

**默认值**：`BindingMode::OneWay`

**可选值**：
| 值 | 描述 |
|-----|------|
| `OneTime` | 一次性绑定，仅在绑定建立时更新目标 |
| `OneWay` | 单向绑定，源变化时更新目标（默认） |
| `TwoWay` | 双向绑定，源和目标相互更新 |
| `OneWayToSource` | 反向单向绑定，目标变化时更新源 |

---

### updateSourceTrigger

```cpp
UpdateSourceTrigger updateSourceTrigger{UpdateSourceTrigger::PropertyChanged};
```

控制何时将目标值更新回源（仅对 TwoWay 和 OneWayToSource 模式有效）。

**默认值**：`UpdateSourceTrigger::PropertyChanged`

**可选值**：
| 值 | 描述 |
|-----|------|
| `Default` | 使用属性定义的默认触发器 |
| `PropertyChanged` | 属性值变化时立即更新（默认） |
| `LostFocus` | 控件失去焦点时更新 |
| `Explicit` | 仅在显式调用 UpdateSource 时更新 |

---

### inheritsDataContext

```cpp
bool inheritsDataContext{false};
```

指定属性是否参与数据上下文的继承机制。

**默认值**：`false`

**说明**：
- 当设置为 `true` 时，该属性会从父元素继承数据上下文
- 主要用于容器控件和布局控件
- 大多数普通属性不需要启用此选项

---

## 使用示例

### 示例 1：默认配置（单向绑定）

```cpp
// 使用默认配置
const auto& TextProperty = DependencyProperty::Register(
    "Text",
    typeid(std::string),
    typeid(Label),
    PropertyMetadata{
        .defaultValue = std::string{""},
        .bindingOptions = BindingOptions{}  // 使用所有默认值
    }
);
```

### 示例 2：双向绑定（输入控件）

```cpp
// 文本输入框，需要双向绑定
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

### 示例 3：失去焦点时更新（表单输入）

```cpp
// 表单输入，失去焦点时才更新源
const auto& ValueProperty = DependencyProperty::Register(
    "Value",
    typeid(std::string),
    typeid(FormInput),
    PropertyMetadata{
        .defaultValue = std::string{""},
        .bindingOptions = BindingOptions{
            .defaultMode = BindingMode::TwoWay,
            .updateSourceTrigger = UpdateSourceTrigger::LostFocus
        }
    }
);
```

### 示例 4：显式更新（批量提交）

```cpp
// 需要手动触发更新的属性
const auto& DraftProperty = DependencyProperty::Register(
    "Draft",
    typeid(std::string),
    typeid(Editor),
    PropertyMetadata{
        .defaultValue = std::string{""},
        .bindingOptions = BindingOptions{
            .defaultMode = BindingMode::TwoWay,
            .updateSourceTrigger = UpdateSourceTrigger::Explicit
        }
    }
);

// 使用时手动更新
void Editor::Save() {
    auto expr = GetBindingExpression(DraftProperty);
    if (expr) {
        expr->UpdateSource();  // 显式更新源
    }
}
```

### 示例 5：继承数据上下文

```cpp
// 容器控件，需要将数据上下文传递给子元素
const auto& ItemsSourceProperty = DependencyProperty::Register(
    "ItemsSource",
    typeid(std::vector<std::any>),
    typeid(ItemsControl),
    PropertyMetadata{
        .defaultValue = std::vector<std::any>{},
        .bindingOptions = BindingOptions{
            .defaultMode = BindingMode::OneWay,
            .inheritsDataContext = true
        }
    }
);
```

### 示例 6：一次性绑定（静态内容）

```cpp
// 只需要初始化一次的属性
const auto& TitleProperty = DependencyProperty::Register(
    "Title",
    typeid(std::string),
    typeid(Window),
    PropertyMetadata{
        .defaultValue = std::string{"Untitled"},
        .bindingOptions = BindingOptions{
            .defaultMode = BindingMode::OneTime
        }
    }
);
```

---

## 常用配置模式

### 显示属性（只读）

```cpp
BindingOptions{
    .defaultMode = BindingMode::OneWay
}
```

适用于：Label、TextBlock 等显示控件的文本属性

### 输入属性（可编辑）

```cpp
BindingOptions{
    .defaultMode = BindingMode::TwoWay,
    .updateSourceTrigger = UpdateSourceTrigger::PropertyChanged
}
```

适用于：TextBox、CheckBox 等输入控件的值属性

### 表单属性（延迟更新）

```cpp
BindingOptions{
    .defaultMode = BindingMode::TwoWay,
    .updateSourceTrigger = UpdateSourceTrigger::LostFocus
}
```

适用于：需要验证的表单字段

### 批量提交属性

```cpp
BindingOptions{
    .defaultMode = BindingMode::TwoWay,
    .updateSourceTrigger = UpdateSourceTrigger::Explicit
}
```

适用于：需要统一保存的编辑器

---

## 配置选择指南

```
┌─────────────────────────────────────────────────────────┐
│                   属性用途是什么？                        │
└─────────────────────────────────────────────────────────┘
                           │
           ┌───────────────┼───────────────┐
           ▼               ▼               ▼
    ┌──────────┐    ┌──────────┐    ┌──────────┐
    │ 只显示    │    │ 可编辑    │    │ 容器属性  │
    └──────────┘    └──────────┘    └──────────┘
           │               │               │
           ▼               ▼               ▼
      OneWay          TwoWay         OneWay +
                          │          inheritsDataContext
                          │
           ┌──────────────┼──────────────┐
           ▼              ▼              ▼
    ┌──────────┐   ┌──────────┐   ┌──────────┐
    │ 实时更新  │   │ 延迟更新  │   │ 手动更新  │
    └──────────┘   └──────────┘   └──────────┘
           │              │              │
           ▼              ▼              ▼
    PropertyChanged   LostFocus     Explicit
```

---

## 最佳实践

### 1. 根据控件类型选择默认模式

```cpp
// ✓ 显示控件：单向绑定
// Label, TextBlock, Image 等
.defaultMode = BindingMode::OneWay

// ✓ 输入控件：双向绑定
// TextBox, CheckBox, Slider 等
.defaultMode = BindingMode::TwoWay
```

### 2. 根据更新频率选择触发器

```cpp
// ✓ 实时反馈（如搜索框）
.updateSourceTrigger = UpdateSourceTrigger::PropertyChanged

// ✓ 表单验证（如注册表单）
.updateSourceTrigger = UpdateSourceTrigger::LostFocus

// ✓ 批量保存（如设置页面）
.updateSourceTrigger = UpdateSourceTrigger::Explicit
```

### 3. 谨慎使用 inheritsDataContext

```cpp
// ✓ 容器控件需要传递上下文
.inheritsDataContext = true  // ItemsControl, ContentControl 等

// ✓ 普通属性通常不需要
.inheritsDataContext = false  // 默认值，大多数情况
```

---

## 相关类

- [PropertyMetadata](./PropertyMetadata.md) - 包含 BindingOptions 的属性元数据
- [DependencyProperty](./DependencyProperty.md) - 使用 BindingOptions 注册属性
- [BindingMode](./BindingMode.md) - 绑定模式枚举详解
- [UpdateSourceTrigger](./UpdateSourceTrigger.md) - 更新触发器枚举详解
- [Binding](./Binding.md) - 运行时绑定配置
