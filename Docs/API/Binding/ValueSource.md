# ValueSource

## 概览

| 属性 | 值 |
|------|-----|
| 命名空间 | `fk::binding` |
| 头文件 | `<fk/binding/PropertyStore.h>` |
| 类型 | 枚举类 (`enum class`) |

`ValueSource` 枚举定义了依赖属性值的来源，用于确定属性值的优先级顺序。

---

## 定义

```cpp
namespace fk::binding {

enum class ValueSource {
    Default,
    Inherited,
    Style,
    Binding,
    Local
};

} // namespace fk::binding

// 便捷别名
namespace fk {
    using binding::ValueSource;
}
```

---

## 枚举值

### Default

```cpp
Default = 0
```

**默认值来源**。当没有设置任何其他值时，使用 `DependencyProperty` 注册时指定的默认值。

- **优先级**：最低（0）
- **设置方式**：在 `DependencyProperty::Register()` 中指定
- **特点**：永远可用作后备值

### Inherited

```cpp
Inherited = 1
```

**继承值来源**。从逻辑树中的父元素继承的值。

- **优先级**：低（1）
- **设置方式**：通过父元素的属性值自动传播
- **适用条件**：属性必须标记为可继承（Inherits）
- **典型用途**：字体、前景色等可继承属性

### Style

```cpp
Style = 2
```

**样式值来源**。通过样式系统设置的值。

- **优先级**：中（2）
- **设置方式**：通过 Style 或 ControlTemplate 设置
- **典型用途**：控件默认外观、主题值

### Binding

```cpp
Binding = 3
```

**绑定值来源**。通过数据绑定从数据源获取的值。

- **优先级**：较高（3）
- **设置方式**：通过 `SetBinding()` 建立绑定
- **特点**：值会随数据源变化自动更新

### Local

```cpp
Local = 4
```

**本地值来源**。通过代码直接设置的值。

- **优先级**：最高（4）
- **设置方式**：通过 `SetValue()` 直接设置
- **特点**：会覆盖所有其他来源的值

---

## 优先级表

值源按以下优先级排序，**数字越大优先级越高**：

| 优先级 | ValueSource | 描述 | 设置方式 |
|--------|-------------|------|----------|
| 4 | `Local` | 本地值 | `SetValue()` |
| 3 | `Binding` | 绑定值 | `SetBinding()` |
| 2 | `Style` | 样式值 | Style/Template |
| 1 | `Inherited` | 继承值 | 父元素传播 |
| 0 | `Default` | 默认值 | 属性注册时指定 |

---

## 值解析流程

```
┌─────────────────────────────────────────────────────────────┐
│                    GetValue(property)                        │
└─────────────────────────────────────────────────────────────┘
                            │
                            ▼
                ┌───────────────────────┐
                │  检查 Local 层        │ ──── 有值 ───▶ 返回 Local 值
                │  (优先级最高)          │
                └───────────────────────┘
                            │ 无值
                            ▼
                ┌───────────────────────┐
                │  检查 Binding 层      │ ──── 有值 ───▶ 返回 Binding 值
                │  (数据绑定)            │
                └───────────────────────┘
                            │ 无值
                            ▼
                ┌───────────────────────┐
                │  检查 Style 层        │ ──── 有值 ───▶ 返回 Style 值
                │  (样式/模板)           │
                └───────────────────────┘
                            │ 无值
                            ▼
                ┌───────────────────────┐
                │  检查 Inherited 层    │ ──── 有值 ───▶ 返回继承值
                │  (父元素继承)          │
                └───────────────────────┘
                            │ 无值
                            ▼
                ┌───────────────────────┐
                │  返回 Default 值      │
                │  (属性默认值)          │
                └───────────────────────┘
```

---

## 使用示例

### 基本使用：查询值来源

```cpp
#include <fk/binding/DependencyObject.h>
#include <fk/binding/PropertyStore.h>
#include <iostream>

void CheckValueSource(DependencyObject& obj, const DependencyProperty& property) {
    // 获取当前值来源
    ValueSource source = obj.GetPropertyStore().GetValueSource(property);
    
    switch (source) {
        case ValueSource::Default:
            std::cout << "使用默认值" << std::endl;
            break;
        case ValueSource::Inherited:
            std::cout << "从父元素继承" << std::endl;
            break;
        case ValueSource::Style:
            std::cout << "来自样式" << std::endl;
            break;
        case ValueSource::Binding:
            std::cout << "来自数据绑定" << std::endl;
            break;
        case ValueSource::Local:
            std::cout << "本地设置的值" << std::endl;
            break;
    }
}
```

### 示例：值优先级演示

```cpp
#include <fk/binding/DependencyObject.h>
#include <fk/binding/DependencyProperty.h>
#include <fk/binding/Binding.h>

class MyControl : public DependencyObject {
public:
    static const DependencyProperty& WidthProperty() {
        static auto property = DependencyProperty::Register<double, MyControl>(
            "Width",
            PropertyMetadata{100.0}  // 默认值 100
        );
        return property;
    }
    
    double GetWidth() const { return GetValue<double>(WidthProperty()); }
    void SetWidth(double value) { SetValue(WidthProperty(), value); }
};

void DemonstrateValuePriority() {
    MyControl control;
    
    // 初始状态：使用默认值
    // 值 = 100, 来源 = Default
    
    // 设置本地值（优先级最高）
    control.SetWidth(200);
    // 值 = 200, 来源 = Local
    
    // 设置绑定（但本地值优先级更高）
    auto binding = Binding::Create().Path("Width");
    control.SetBinding(MyControl::WidthProperty(), binding);
    // 值 = 200, 来源 = Local（本地值仍然覆盖绑定）
    
    // 清除本地值后，绑定生效
    control.ClearValue(MyControl::WidthProperty());
    // 值 = 绑定值, 来源 = Binding
}
```

### 示例：监听值来源变化

```cpp
#include <fk/binding/PropertyStore.h>
#include <iostream>

void SetupValueSourceMonitoring(PropertyStore& store) {
    store.SetValueChangedCallback(
        [](const DependencyProperty& property,
           const std::any& oldValue,
           const std::any& newValue,
           ValueSource oldSource,
           ValueSource newSource) {
            
            std::cout << "属性 " << property.GetName() << " 值来源变化：";
            std::cout << static_cast<int>(oldSource) << " -> " << static_cast<int>(newSource);
            std::cout << std::endl;
        }
    );
}
```

### 示例：条件性值设置

```cpp
// 只在没有本地值时设置样式值
void SetStyleValueIfNoLocal(PropertyStore& store, 
                            const DependencyProperty& property,
                            std::any value) {
    ValueSource currentSource = store.GetValueSource(property);
    
    // 只有当前值不是本地值时，才设置样式值
    if (currentSource != ValueSource::Local) {
        store.SetValue(property, std::move(value), ValueSource::Style);
    }
}

// 强制设置本地值（覆盖任何其他值）
void ForceLocalValue(PropertyStore& store,
                     const DependencyProperty& property,
                     std::any value) {
    store.SetValue(property, std::move(value), ValueSource::Local);
}
```

---

## 与 PropertyStore 的关系

`ValueSource` 与 [PropertyStore](PropertyStore.md) 紧密配合：

| PropertyStore 方法 | 使用的 ValueSource |
|-------------------|-------------------|
| `SetValue(property, value, source)` | 指定的 source |
| `ClearValue(property, source)` | 清除指定 source 的值 |
| `GetValueSource(property)` | 返回当前有效值的 source |
| `SetBinding()` | 自动使用 `Binding` |
| `ApplyBindingValue()` | 自动使用 `Binding` |

---

## 相关类

- [PropertyStore](PropertyStore.md) - 使用 ValueSource 管理属性值优先级
- [DependencyObject](DependencyObject.md) - 依赖对象基类
- [DependencyProperty](DependencyProperty.md) - 依赖属性定义
- [Binding](Binding.md) - 数据绑定配置
- [BindingMode](BindingMode.md) - 绑定模式枚举
- [UpdateSourceTrigger](UpdateSourceTrigger.md) - 更新源触发器枚举

---

## 最佳实践

### ✅ 推荐做法

1. **理解优先级机制**：Local > Binding > Style > Inherited > Default
2. **适当使用 ClearValue**：清除本地值以允许绑定或样式生效
3. **查询值来源进行调试**：使用 `GetValueSource()` 诊断值来源问题
4. **样式值用于默认外观**：让用户的本地设置能够覆盖样式

### ❌ 避免做法

1. **不要忽略优先级**：设置本地值会覆盖绑定
2. **不要混淆清除操作**：`ClearValue(Local)` 只清除本地值，不影响其他来源
3. **不要直接比较枚举整数值**：使用枚举值本身进行比较
