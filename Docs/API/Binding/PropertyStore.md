# PropertyStore

## 概览

| 属性 | 值 |
|------|-----|
| **命名空间** | `fk::binding` |
| **头文件** | `fk/binding/PropertyStore.h` |
| **源文件** | `src/binding/PropertyStore.cpp` |

## 描述

`PropertyStore` 是 `DependencyObject` 的内部属性值存储系统，负责：

- 管理多层级属性值（本地、绑定、样式、继承）
- 根据优先级计算有效属性值
- 存储和管理绑定表达式
- 触发值变更通知
- 跟踪属性值来源

## 类定义

```cpp
namespace fk::binding {

class PropertyStore {
public:
    using ValueChangedCallback = std::function<void(
        const DependencyProperty&,
        const std::any& oldValue,
        const std::any& newValue,
        ValueSource oldSource,
        ValueSource newSource
    )>;

    PropertyStore();
    explicit PropertyStore(ValueChangedCallback callback);

    // 值访问
    const std::any& GetValue(const DependencyProperty& property) const;
    ValueSource GetValueSource(const DependencyProperty& property) const;

    // 值设置
    void SetValue(const DependencyProperty& property, std::any value, ValueSource source);
    void ClearValue(const DependencyProperty& property, ValueSource source);

    // 绑定管理
    void SetBinding(const DependencyProperty& property, std::shared_ptr<BindingExpression> binding);
    std::shared_ptr<BindingExpression> GetBinding(const DependencyProperty& property) const;
    void ClearBinding(const DependencyProperty& property);
    void ApplyBindingValue(const DependencyProperty& property, std::any value);

    // 查询和清理
    bool HasValue(const DependencyProperty& property) const;
    void ClearAll();

    // 回调设置
    void SetValueChangedCallback(ValueChangedCallback callback);

private:
    struct Layer {
        bool hasValue{false};
        std::any value;
    };

    struct PropertyEntry {
        Layer local;
        Layer binding;
        Layer style;
        Layer inherited;
        std::shared_ptr<BindingExpression> bindingExpression;
        std::any effectiveValue;
        bool hasEffective{false};
        ValueSource effectiveSource{ValueSource::Default};
    };

    ValueChangedCallback valueChangedCallback_{};
    std::unordered_map<std::size_t, PropertyEntry> entries_;
};

} // namespace fk::binding
```

---

## 构造函数

### PropertyStore()

```cpp
PropertyStore();
```

默认构造函数，创建一个空的属性存储。

### PropertyStore(ValueChangedCallback)

```cpp
explicit PropertyStore(ValueChangedCallback callback);
```

构造一个带有值变更回调的属性存储。

**参数**：
| 参数 | 类型 | 描述 |
|------|------|------|
| `callback` | `ValueChangedCallback` | 属性值变更时的回调函数 |

**示例**：
```cpp
fk::PropertyStore store([](const fk::DependencyProperty& prop,
                           const std::any& oldVal,
                           const std::any& newVal,
                           fk::ValueSource oldSource,
                           fk::ValueSource newSource) {
    std::cout << "属性 " << prop.Name() << " 值已变更" << std::endl;
});
```

---

## 值访问方法

### GetValue

```cpp
const std::any& GetValue(const DependencyProperty& property) const;
```

获取属性的当前有效值。

**参数**：
| 参数 | 类型 | 描述 |
|------|------|------|
| `property` | `const DependencyProperty&` | 要查询的依赖属性 |

**返回值**：属性的有效值，如果没有设置则返回属性元数据中的默认值

**示例**：
```cpp
const std::any& value = store.GetValue(MyElement::WidthProperty());
double width = std::any_cast<double>(value);
```

### GetValueSource

```cpp
ValueSource GetValueSource(const DependencyProperty& property) const;
```

获取属性当前有效值的来源。

**参数**：
| 参数 | 类型 | 描述 |
|------|------|------|
| `property` | `const DependencyProperty&` | 要查询的依赖属性 |

**返回值**：`ValueSource` 枚举值，表示值的来源

**示例**：
```cpp
auto source = store.GetValueSource(MyElement::WidthProperty());
if (source == fk::ValueSource::Local) {
    std::cout << "值是本地设置的" << std::endl;
}
```

---

## 值设置方法

### SetValue

```cpp
void SetValue(const DependencyProperty& property, std::any value, ValueSource source);
```

在指定层级设置属性值。

**参数**：
| 参数 | 类型 | 描述 |
|------|------|------|
| `property` | `const DependencyProperty&` | 要设置的依赖属性 |
| `value` | `std::any` | 要设置的值 |
| `source` | `ValueSource` | 值的来源层级 |

**异常**：如果 `source` 为 `ValueSource::Binding`，抛出 `std::invalid_argument`（应使用 `SetBinding`）

**说明**：
- 设置值后会重新计算有效值
- 如果有效值发生变化，触发回调

**示例**：
```cpp
store.SetValue(MyElement::WidthProperty(), 200.0, fk::ValueSource::Local);
```

### ClearValue

```cpp
void ClearValue(const DependencyProperty& property, ValueSource source);
```

清除指定层级的属性值。

**参数**：
| 参数 | 类型 | 描述 |
|------|------|------|
| `property` | `const DependencyProperty&` | 要清除的依赖属性 |
| `source` | `ValueSource` | 要清除的层级 |

**说明**：
- 清除后会重新计算有效值
- 如果所有层级都没有值且没有绑定，条目会被完全移除

**示例**：
```cpp
store.ClearValue(MyElement::WidthProperty(), fk::ValueSource::Local);
```

---

## 绑定管理方法

### SetBinding

```cpp
void SetBinding(const DependencyProperty& property, std::shared_ptr<BindingExpression> binding);
```

设置属性的绑定表达式。

**参数**：
| 参数 | 类型 | 描述 |
|------|------|------|
| `property` | `const DependencyProperty&` | 要绑定的依赖属性 |
| `binding` | `std::shared_ptr<BindingExpression>` | 绑定表达式 |

**说明**：
- 设置绑定会清除绑定层的当前值
- 绑定值通过 `ApplyBindingValue()` 应用

**示例**：
```cpp
auto expr = binding.CreateExpression(target, property);
store.SetBinding(property, expr);
```

### GetBinding

```cpp
std::shared_ptr<BindingExpression> GetBinding(const DependencyProperty& property) const;
```

获取属性的绑定表达式。

**参数**：
| 参数 | 类型 | 描述 |
|------|------|------|
| `property` | `const DependencyProperty&` | 要查询的依赖属性 |

**返回值**：绑定表达式智能指针，如果没有绑定返回 `nullptr`

### ClearBinding

```cpp
void ClearBinding(const DependencyProperty& property);
```

清除属性的绑定。

**参数**：
| 参数 | 类型 | 描述 |
|------|------|------|
| `property` | `const DependencyProperty&` | 要清除绑定的依赖属性 |

**说明**：
- 清除绑定表达式和绑定层的值
- 重新计算有效值

### ApplyBindingValue

```cpp
void ApplyBindingValue(const DependencyProperty& property, std::any value);
```

应用绑定计算的值到绑定层。

**参数**：
| 参数 | 类型 | 描述 |
|------|------|------|
| `property` | `const DependencyProperty&` | 目标依赖属性 |
| `value` | `std::any` | 绑定计算的值 |

**说明**：
- 由 `BindingExpression` 在更新目标时调用
- 设置绑定层的值并重新计算有效值

---

## 查询和清理方法

### HasValue

```cpp
bool HasValue(const DependencyProperty& property) const;
```

检查属性是否有任何设置的值。

**参数**：
| 参数 | 类型 | 描述 |
|------|------|------|
| `property` | `const DependencyProperty&` | 要查询的依赖属性 |

**返回值**：如果有有效值或任何层级有值，返回 `true`

### ClearAll

```cpp
void ClearAll();
```

清除所有属性条目。

**说明**：通常在对象销毁时调用

---

## 回调设置方法

### SetValueChangedCallback

```cpp
void SetValueChangedCallback(ValueChangedCallback callback);
```

设置或更新值变更回调。

**参数**：
| 参数 | 类型 | 描述 |
|------|------|------|
| `callback` | `ValueChangedCallback` | 新的回调函数 |

---

# ValueSource 枚举

## 描述

定义属性值的来源层级，用于确定值的优先级。

## 枚举定义

```cpp
enum class ValueSource {
    Default,
    Inherited,
    Style,
    Binding,
    Local
};
```

## 枚举值（按优先级从低到高）

| 值 | 优先级 | 描述 |
|----|--------|------|
| `Default` | 1（最低） | 属性元数据中定义的默认值 |
| `Inherited` | 2 | 从父元素继承的值 |
| `Style` | 3 | 样式设置的值 |
| `Binding` | 4 | 绑定计算的值 |
| `Local` | 5（最高） | 直接设置的本地值 |

---

## 值优先级机制

`PropertyStore` 使用分层存储结构管理属性值：

```
PropertyEntry
├── local      (优先级最高)
├── binding
├── style
├── inherited
└── default    (来自 PropertyMetadata，优先级最低)
```

### 有效值计算规则

1. 检查 `local` 层，如果有值则使用
2. 检查 `binding` 层，如果有值则使用
3. 检查 `style` 层，如果有值则使用
4. 检查 `inherited` 层，如果有值则使用
5. 使用 `PropertyMetadata` 中的默认值

### 示例

```cpp
// 设置样式值
store.SetValue(WidthProperty, 100.0, fk::ValueSource::Style);
// 有效值 = 100.0, 来源 = Style

// 设置本地值（优先级更高）
store.SetValue(WidthProperty, 200.0, fk::ValueSource::Local);
// 有效值 = 200.0, 来源 = Local

// 清除本地值
store.ClearValue(WidthProperty, fk::ValueSource::Local);
// 有效值 = 100.0, 来源 = Style（回退到样式值）
```

---

## 内部结构

### Layer 结构体

```cpp
struct Layer {
    bool hasValue{false};
    std::any value;
};
```

表示单个值层级，包含是否有值的标志和实际值。

### PropertyEntry 结构体

```cpp
struct PropertyEntry {
    Layer local;                                      // 本地层
    Layer binding;                                    // 绑定层
    Layer style;                                      // 样式层
    Layer inherited;                                  // 继承层
    std::shared_ptr<BindingExpression> bindingExpression;  // 绑定表达式
    std::any effectiveValue;                          // 计算后的有效值
    bool hasEffective{false};                         // 是否有有效值
    ValueSource effectiveSource{ValueSource::Default}; // 有效值来源
};
```

---

## 使用示例

### DependencyObject 中的使用

```cpp
class DependencyObject {
public:
    DependencyObject()
        : propertyStore_([this](const DependencyProperty& property,
                                const std::any& oldValue,
                                const std::any& newValue,
                                ValueSource oldSource,
                                ValueSource newSource) {
              // 处理值变更
              OnPropertyChanged(property, oldValue, newValue, oldSource, newSource);
          }) {}

    const std::any& GetValue(const DependencyProperty& property) const {
        return propertyStore_.GetValue(property);
    }

    void SetValue(const DependencyProperty& property, std::any value) {
        propertyStore_.SetValue(property, std::move(value), ValueSource::Local);
    }

private:
    PropertyStore propertyStore_;
};
```

### 多层级值管理

```cpp
// 创建属性存储
fk::PropertyStore store;

// 设置继承值
store.SetValue(FontSizeProperty, 14.0, fk::ValueSource::Inherited);
// 有效值 = 14.0

// 设置样式值（优先级更高）
store.SetValue(FontSizeProperty, 16.0, fk::ValueSource::Style);
// 有效值 = 16.0

// 设置绑定
auto expr = binding.CreateExpression(target, FontSizeProperty);
store.SetBinding(FontSizeProperty, expr);
// 绑定激活后，绑定值将覆盖样式值

// 设置本地值（优先级最高）
store.SetValue(FontSizeProperty, 20.0, fk::ValueSource::Local);
// 有效值 = 20.0，忽略绑定和样式值
```

---

## 私有成员

| 成员 | 类型 | 描述 |
|------|------|------|
| `valueChangedCallback_` | `ValueChangedCallback` | 值变更回调函数 |
| `entries_` | `std::unordered_map<std::size_t, PropertyEntry>` | 属性条目映射（按属性 ID 索引） |

---

## 私有方法

### EnsureEntry

```cpp
PropertyEntry& EnsureEntry(const DependencyProperty& property);
```

确保属性条目存在，如果不存在则创建。

### FindEntry

```cpp
PropertyEntry* FindEntry(const DependencyProperty& property);
const PropertyEntry* FindEntry(const DependencyProperty& property) const;
```

查找属性条目，如果不存在返回 `nullptr`。

### UpdateEffectiveValue

```cpp
void UpdateEffectiveValue(const DependencyProperty& property, PropertyEntry& entry);
```

重新计算有效值并触发变更通知。

### SelectLayer

```cpp
static Layer& SelectLayer(PropertyEntry& entry, ValueSource source);
static const Layer& SelectLayer(const PropertyEntry& entry, ValueSource source);
```

根据 `ValueSource` 选择对应的层。

### DetermineEffectiveSource

```cpp
static ValueSource DetermineEffectiveSource(const PropertyEntry& entry);
```

根据各层是否有值确定有效值来源。

### TryGetLayerValue

```cpp
static const std::any* TryGetLayerValue(const PropertyEntry& entry, ValueSource source);
```

尝试获取指定层的值。

### AreEquivalent

```cpp
static bool AreEquivalent(const std::any& lhs, const std::any& rhs);
```

比较两个 `std::any` 值是否等价（支持常用类型）。

### EntryHasAnyValue

```cpp
static bool EntryHasAnyValue(const PropertyEntry& entry);
```

检查条目是否有任何层级的值。

---

## 相关类

- [DependencyObject](DependencyObject.md) - 使用 PropertyStore 存储属性值
- [DependencyProperty](DependencyProperty.md) - 定义属性元数据
- [BindingExpression](BindingExpression.md) - 通过 PropertyStore 应用绑定值

## 另请参阅

- [数据绑定概述](../BINDING_MODULE_OVERVIEW.md)
- [入门指南](../../Guides/GettingStarted.md)
