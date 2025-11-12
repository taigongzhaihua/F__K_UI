# BindingExpression

## 概览

**目的**：表示活动的数据绑定实例

**命名空间**：`fk::binding`

**头文件**：`fk/binding/BindingExpression.h`

## 描述

`BindingExpression` 是 `Binding` 的活动实例，负责在源和目标之间实际传输数据。它订阅源对象的变更通知并更新目标属性。

## 公共接口

### 创建

通常不直接创建，而是通过设置 `Binding` 自动创建：

```cpp
element->SetValue(property, binding);  // 自动创建BindingExpression
```

### 更新控制

#### UpdateTarget / UpdateSource
```cpp
void UpdateTarget();
void UpdateSource();
```

手动触发目标或源的更新。

**示例**：
```cpp
// 获取绑定表达式
auto expr = element->GetBindingExpression(TextBox::TextProperty());

// 手动更新
if (expr) {
    expr->UpdateTarget();  // 从源更新到目标
}
```

### 状态查询

#### GetValue
```cpp
std::any GetValue() const;
```

获取当前绑定值。

#### IsAttached
```cpp
bool IsAttached() const;
```

检查绑定是否已附加并活动。

## 绑定生命周期

1. **创建**：通过 `SetValue(property, Binding)` 创建
2. **附加**：解析源对象并订阅变更通知
3. **活动**：在源和目标之间传输数据
4. **分离**：当目标对象销毁时自动清理

## 使用示例

### 获取绑定表达式
```cpp
// 设置绑定
element->SetValue(TextBox::TextProperty(), Binding("Name").Source(vm));

// 稍后获取绑定表达式
auto expr = element->GetBindingExpression(TextBox::TextProperty());
if (expr) {
    // 可以控制绑定行为
    expr->UpdateSource();
}
```

### 手动更新
```cpp
// 对于UpdateSourceTrigger为Explicit的绑定
textBox->SetValue(TextBox::TextProperty(), 
                 Binding("Name")
                     .Source(viewModel)
                     .UpdateSourceTrigger(UpdateSourceTrigger::Explicit));

// 稍后显式更新源
auto expr = textBox->GetBindingExpression(TextBox::TextProperty());
if (expr) {
    expr->UpdateSource();
}
```

## 内部工作原理

```
1. BindingExpression创建
   ↓
2. 解析源对象（通过Path）
   ↓
3. 订阅INotifyPropertyChanged
   ↓
4. 初始UpdateTarget()
   ↓
5. 源变更 → PropertyChanged
   ↓
6. UpdateTarget() - 更新UI
   ↓
7. UI变更（TwoWay模式）
   ↓
8. UpdateSource() - 更新源
```

## 相关类

- [Binding](Binding.md) - 绑定配置
- [DependencyObject](DependencyObject.md) - 绑定目标
- [INotifyPropertyChanged](INotifyPropertyChanged.md) - 变更通知

## 另请参阅

- [设计文档](../../Design/Binding/BindingExpression.md)
