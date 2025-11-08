# PropertyStore - 属性值存储

## 概述

`PropertyStore` 管理依赖属性的值存储和优先级系统。它支持多层值来源（Local、Binding、Style、Inherited、Default），自动维护有效值和优先级。

## 核心概念

### 值优先级层级

属性值可以来自多个来源，框架使用以下优先级（从高到低）：

```
Local (最高)
  ↓
Binding
  ↓
Style
  ↓
Inherited
  ↓
Default (最低)
```

当更高优先级的值存在时，它会覆盖所有较低优先级的值。

**示例：**
```
Value = Default(0)
→ Value = Inherited(5)           // 当前值: 5
→ Value = Style(10)              // 当前值: 10
→ Value = Local(20)              // 当前值: 20 (覆盖所有)
→ Value = Binding(15)            // 当前值: 20 (Local 优先级更高)
```

## 核心 API

### 构造函数

```cpp
PropertyStore();
explicit PropertyStore(ValueChangedCallback callback);
```

**参数：**
- `callback`: 可选的值变更回调函数

**示例：**
```cpp
auto store = std::make_unique<fk::binding::PropertyStore>(
    [](const fk::binding::DependencyProperty& prop,
       const std::any& oldValue,
       const std::any& newValue,
       fk::binding::ValueSource oldSource,
       fk::binding::ValueSource newSource) {
        std::cout << "属性变更: " << prop.Name() << "\n";
    }
);
```

### 获取属性值

```cpp
const std::any& GetValue(const DependencyProperty& property) const;
ValueSource GetValueSource(const DependencyProperty& property) const;
```

- `GetValue()`: 返回有效值（最高优先级来源）
- `GetValueSource()`: 返回值的来源

**示例：**
```cpp
auto& value = store.GetValue(property);
auto source = store.GetValueSource(property);

if (source == fk::binding::ValueSource::Binding) {
    std::cout << "值来自绑定\n";
}
```

### 设置属性值

```cpp
void SetValue(
    const DependencyProperty& property,
    std::any value,
    ValueSource source
);
```

设置特定来源的属性值，自动计算有效值。

**参数：**
- `property`: 要设置的属性
- `value`: 新值
- `source`: 值的来源

**示例：**
```cpp
// 设置本地值
store.SetValue(widthProperty, 100.0f, fk::binding::ValueSource::Local);

// 设置绑定值（会被本地值覆盖）
store.SetValue(heightProperty, 50.0f, fk::binding::ValueSource::Binding);

// 设置样式值
store.SetValue(colorProperty, std::string("Red"), fk::binding::ValueSource::Style);
```

### 清除属性值

```cpp
void ClearValue(const DependencyProperty& property, ValueSource source);
void ClearAll();
```

- `ClearValue()`: 清除特定来源的值，自动使用下一个优先级的值
- `ClearAll()`: 清除所有值

**示例：**
```cpp
// 清除本地值，让绑定值或其他值生效
store.ClearValue(property, fk::binding::ValueSource::Local);

// 清除所有值
store.ClearAll();
```

### 绑定管理

```cpp
void SetBinding(
    const DependencyProperty& property,
    std::shared_ptr<BindingExpression> binding
);

std::shared_ptr<BindingExpression> GetBinding(
    const DependencyProperty& property
) const;

void ClearBinding(const DependencyProperty& property);
void ApplyBindingValue(const DependencyProperty& property, std::any value);
```

管理属性的绑定表达式和值。

**示例：**
```cpp
// 设置绑定
auto binding = std::make_shared<fk::binding::BindingExpression>(...);
store.SetBinding(property, binding);

// 绑定更新值时调用
store.ApplyBindingValue(property, newValue);

// 清除绑定
store.ClearBinding(property);
```

### 检查和管理存储

```cpp
bool HasValue(const DependencyProperty& property) const;
void SetValueChangedCallback(ValueChangedCallback callback);
```

**示例：**
```cpp
if (store.HasValue(property)) {
    // 属性有值（来自任何来源）
}

// 动态改变回调
store.SetValueChangedCallback(
    [](const auto& prop, const auto& old, const auto& neu, auto oldSrc, auto newSrc) {
        // 新的回调处理
    }
);
```

## 常见模式

### 1. 基本属性管理

```cpp
class DependencyObject {
private:
    fk::binding::PropertyStore store_;
    
public:
    void SetValue(const DependencyProperty& prop, std::any value) {
        store_.SetValue(prop, value, fk::binding::ValueSource::Local);
    }
    
    const std::any& GetValue(const DependencyProperty& prop) const {
        return store_.GetValue(prop);
    }
};
```

### 2. 优先级决议

```cpp
void ResolvePropertyPriority() {
    // 场景：Local、Style、Binding 都设置了同一属性
    
    store_.SetValue(prop, 10, fk::binding::ValueSource::Style);
    store_.SetValue(prop, 20, fk::binding::ValueSource::Binding);
    store_.SetValue(prop, 30, fk::binding::ValueSource::Local);
    
    // 有效值: 30 (Local 优先级最高)
    auto value = std::any_cast<int>(store_.GetValue(prop));  // 30
}
```

### 3. 清除本地值恢复继承

```cpp
class UIElement {
private:
    fk::binding::PropertyStore store_;
    
public:
    void ResetToInherited(const DependencyProperty& prop) {
        // 清除本地值，允许继承值显示
        store_.ClearValue(prop, fk::binding::ValueSource::Local);
        
        // 现在 GetValue 会返回继承或样式或绑定值
    }
};
```

### 4. 监听值变更

```cpp
class ReactivePropertyStore {
private:
    std::unordered_map<std::string, std::function<void(const std::any&)>> listeners_;
    
public:
    void WatchProperty(
        const std::string& propName,
        std::function<void(const std::any&)> callback) {
        
        listeners_[propName] = callback;
    }
    
    void SetupStore() {
        store_ = std::make_unique<fk::binding::PropertyStore>(
            [this](const auto& prop, const auto& oldVal, const auto& newVal, auto, auto) {
                auto it = listeners_.find(prop.Name());
                if (it != listeners_.end()) {
                    it->second(newVal);
                }
            }
        );
    }
};
```

### 5. 绑定值应用流程

```cpp
void ApplyBindingUpdate(
    const DependencyProperty& property,
    const std::any& newValue) {
    
    // 绑定系统将新值应用到存储
    store_.ApplyBindingValue(property, newValue);
    
    // PropertyStore 自动：
    // 1. 如果没有 Local 值，使用这个 Binding 值
    // 2. 如果有 Local 值，保留 Local（较高优先级）
    // 3. 触发 ValueChanged 回调
    
    // 获取最终有效值
    auto effectiveValue = store_.GetValue(property);
}
```

## 值优先级决议算法

```cpp
// 伪代码
EffectiveValue = 
    if (Local.HasValue) 
        return Local
    else if (Binding.HasValue)
        return Binding
    else if (Style.HasValue)
        return Style
    else if (Inherited.HasValue)
        return Inherited
    else
        return Default
```

## 最佳实践

### 1. 总是指定 ValueSource

```cpp
// ✅ 推荐：明确指定来源
store.SetValue(prop, value, fk::binding::ValueSource::Local);

// ❌ 避免：可能导致优先级混乱
store.SetValue(prop, value, some_unknown_source);
```

### 2. 在清除前检查

```cpp
// ✅ 推荐：检查后清除
if (store.HasValue(property)) {
    store.ClearValue(property, fk::binding::ValueSource::Local);
}

// ❌ 不必要的防护（但也不会出错）
// store.ClearValue(...) 会安全地处理
```

### 3. 使用回调进行反应式更新

```cpp
// ✅ 推荐：回调处理变更
auto store = std::make_unique<PropertyStore>(
    [this](const auto& prop, const auto& oldVal, const auto& newVal, auto, auto) {
        OnPropertyChanged(prop.Name(), newVal);
    }
);

// ❌ 避免：不必要的查询
while (true) {
    auto val = store.GetValue(property);  // 轮询不好
}
```

## 线程安全性

PropertyStore 不是线程安全的。应在单线程（UI 线程）中使用。

## 常见问题

### Q: 如果多个来源设置了值，哪个会被使用？
**A:** 优先级最高的那个。参考值优先级章节。

### Q: 清除一个来源的值会发生什么？
**A:** 自动使用下一个优先级的值（如果存在）并触发回调。

### Q: 绑定和 Local 值冲突时怎样？
**A:** Local 优先级更高，绑定值被忽视。

### Q: 如何调试属性值来源？
**A:** 使用 `GetValueSource()` 查询，并通过回调监听变更。

## 相关文档

- [DependencyProperty.md](./DependencyProperty.md) - 依赖属性定义
- [BindingExpression.md](./BindingExpression.md) - 绑定表达式
- [DependencyObject.md](./DependencyObject.md) - 依赖对象基类
