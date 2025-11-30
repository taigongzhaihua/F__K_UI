# RelativeSource 类

## 概览

| 项目 | 信息 |
|------|------|
| **命名空间** | `fk::binding` |
| **头文件** | `#include <fk/binding/Binding.h>` |
| **类型** | 类 + 枚举 |

`RelativeSource` 类用于指定绑定源的相对位置，允许绑定到元素自身或在逻辑树中查找祖先元素作为绑定源。

---

## RelativeSourceMode 枚举

```cpp
enum class RelativeSourceMode {
    Self,           // 绑定到元素自身
    FindAncestor    // 在逻辑树中查找祖先元素
};
```

### 枚举值说明

| 值 | 描述 |
|----|------|
| `Self` | 绑定到当前元素自身，用于同一元素上不同属性之间的绑定 |
| `FindAncestor` | 在逻辑树中向上查找指定类型的祖先元素作为绑定源 |

---

## RelativeSource 类定义

```cpp
class RelativeSource {
public:
    // 构造函数
    RelativeSource() = default;
    explicit RelativeSource(RelativeSourceMode mode);

    // 静态工厂方法
    static RelativeSource Self();
    static RelativeSource FindAncestor(std::type_index type, int level = 1);

    // 属性访问
    RelativeSourceMode Mode() const noexcept;
    bool HasAncestorType() const noexcept;
    std::type_index AncestorType() const noexcept;
    int AncestorLevel() const noexcept;

    // 配置方法
    void SetAncestorType(std::type_index type);
    void SetAncestorLevel(int level);

private:
    RelativeSourceMode mode_{RelativeSourceMode::Self};
    std::type_index ancestorType_{typeid(void)};
    int ancestorLevel_{1};
    bool hasAncestorType_{false};
};
```

---

## 构造函数

### 默认构造函数

```cpp
RelativeSource() = default;
```

创建默认的 RelativeSource，模式为 `Self`。

### 带模式构造函数

```cpp
explicit RelativeSource(RelativeSourceMode mode);
```

**参数：**
- `mode` - 相对源模式

---

## 静态工厂方法

### Self()

```cpp
static RelativeSource Self();
```

创建绑定到元素自身的相对源。

**返回值：** 配置为 `Self` 模式的 `RelativeSource` 对象

**示例：**
```cpp
// 绑定到元素自身的另一个属性
auto binding = Binding()
    .SetRelativeSource(RelativeSource::Self())
    .SetPath("Width");
```

---

### FindAncestor()

```cpp
static RelativeSource FindAncestor(std::type_index type, int level = 1);
```

创建在逻辑树中查找指定类型祖先的相对源。

**参数：**
- `type` - 要查找的祖先元素类型
- `level` - 要跳过的匹配祖先数量（默认为 1，即第一个匹配的祖先）

**返回值：** 配置为 `FindAncestor` 模式的 `RelativeSource` 对象

**示例：**
```cpp
// 绑定到最近的 Panel 类型祖先
auto binding = Binding()
    .SetRelativeSource(RelativeSource::FindAncestor(typeid(Panel)))
    .SetPath("Background");

// 绑定到第二个 Panel 类型祖先（跳过第一个）
auto binding2 = Binding()
    .SetRelativeSource(RelativeSource::FindAncestor(typeid(Panel), 2))
    .SetPath("Name");
```

---

## 属性访问方法

### Mode()

```cpp
RelativeSourceMode Mode() const noexcept;
```

获取相对源模式。

**返回值：** 当前设置的 `RelativeSourceMode`

---

### HasAncestorType()

```cpp
bool HasAncestorType() const noexcept;
```

检查是否设置了祖先类型。

**返回值：** 如果已设置祖先类型返回 `true`

---

### AncestorType()

```cpp
std::type_index AncestorType() const noexcept;
```

获取要查找的祖先类型。

**返回值：** 祖先类型的 `std::type_index`

---

### AncestorLevel()

```cpp
int AncestorLevel() const noexcept;
```

获取祖先级别（要跳过的匹配数量）。

**返回值：** 祖先级别，最小为 1

---

## 配置方法

### SetAncestorType()

```cpp
void SetAncestorType(std::type_index type);
```

设置要查找的祖先类型。

**参数：**
- `type` - 祖先元素的类型索引

---

### SetAncestorLevel()

```cpp
void SetAncestorLevel(int level);
```

设置祖先级别。

**参数：**
- `level` - 级别值，小于 1 的值会被调整为 1

---

## 使用示例

### 示例 1：绑定到元素自身

```cpp
// 将元素的 Height 绑定到自身的 Width（创建正方形）
class SquarePanel : public Panel {
public:
    static inline auto HeightProperty = DependencyProperty::Register(
        "Height", typeid(SquarePanel),
        PropertyMetadata(100.0)
    );

    SquarePanel() {
        auto binding = Binding()
            .SetRelativeSource(RelativeSource::Self())
            .SetPath("Width");
        
        SetBinding(HeightProperty, binding);
    }
};
```

### 示例 2：绑定到祖先元素

```cpp
// 子项绑定到父容器的背景色
class ChildItem : public DependencyObject {
public:
    static inline auto BackgroundProperty = DependencyProperty::Register(
        "Background", typeid(ChildItem),
        PropertyMetadata(Color::White)
    );

    void BindToParentBackground() {
        auto binding = Binding()
            .SetRelativeSource(RelativeSource::FindAncestor(typeid(Container)))
            .SetPath("Background");
        
        SetBinding(BackgroundProperty, binding);
    }
};
```

### 示例 3：跳过多层祖先

```cpp
// 绑定到第二层 Grid 祖先的宽度
auto binding = Binding()
    .SetRelativeSource(RelativeSource::FindAncestor(typeid(Grid), 2))
    .SetPath("ActualWidth");

element->SetBinding(WidthProperty, binding);
```

### 示例 4：与 Binding 类配合使用

```cpp
// 完整的 RelativeSource 绑定配置
auto binding = Binding()
    .SetRelativeSource(RelativeSource::FindAncestor(typeid(Window)))
    .SetPath("Title")
    .SetMode(BindingMode::OneWay);

// 检查绑定是否使用了相对源
if (binding.HasRelativeSource()) {
    const auto& source = binding.GetRelativeSource();
    if (source.Mode() == RelativeSourceMode::FindAncestor) {
        // 处理祖先查找逻辑
    }
}
```

---

## 相对源查找流程

```
┌─────────────────────────────────────────────────────────┐
│                  RelativeSource 查找                     │
├─────────────────────────────────────────────────────────┤
│                                                         │
│  ┌─────────────┐                                        │
│  │ Self 模式   │ ──► 直接使用当前元素作为绑定源          │
│  └─────────────┘                                        │
│                                                         │
│  ┌─────────────┐    ┌──────────────────────────────┐   │
│  │FindAncestor │ ──►│ 在逻辑树中向上遍历            │   │
│  │    模式     │    │                              │   │
│  └─────────────┘    │  1. 获取当前元素的父元素      │   │
│                     │  2. 检查类型是否匹配          │   │
│                     │  3. 如果匹配，检查 level      │   │
│                     │  4. level 递减到 0 时返回     │   │
│                     │  5. 否则继续向上遍历          │   │
│                     └──────────────────────────────┘   │
│                                                         │
└─────────────────────────────────────────────────────────┘
```

---

## 私有成员

| 成员 | 类型 | 默认值 | 描述 |
|------|------|--------|------|
| `mode_` | `RelativeSourceMode` | `Self` | 相对源模式 |
| `ancestorType_` | `std::type_index` | `typeid(void)` | 祖先类型 |
| `ancestorLevel_` | `int` | `1` | 祖先级别 |
| `hasAncestorType_` | `bool` | `false` | 是否设置了祖先类型 |

---

## 最佳实践

1. **优先使用静态工厂方法** - 使用 `Self()` 和 `FindAncestor()` 而非直接构造
2. **合理设置 level** - level 为 1 表示第一个匹配的祖先，较大的值用于跳过近层祖先
3. **类型安全** - 使用 `typeid()` 获取正确的类型索引
4. **配合 Binding 使用** - 通过 `Binding::SetRelativeSource()` 设置相对源

---

## 相关类

- [Binding](Binding.md) - 使用 RelativeSource 配置绑定源
- [BindingExpression](BindingExpression.md) - 运行时解析 RelativeSource
- [DependencyObject](DependencyObject.md) - 逻辑树遍历的基础类
