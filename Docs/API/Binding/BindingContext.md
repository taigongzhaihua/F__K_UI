# BindingContext

## 概览

**目的**：管理对象的所有绑定

**命名空间**：`fk::binding`

**头文件**：`fk/binding/BindingContext.h`

## 描述

`BindingContext` 管理单个DependencyObject上的所有活动绑定。

## 公共接口

### 绑定管理

#### SetBinding / GetBinding
```cpp
void SetBinding(const DependencyProperty& property, 
                std::shared_ptr<BindingExpression> expression);
std::shared_ptr<BindingExpression> GetBinding(const DependencyProperty& property);
```

#### ClearBinding
```cpp
void ClearBinding(const DependencyProperty& property);
```

## 使用示例

内部由DependencyObject使用，通常不需要直接操作。

## 相关类

- [DependencyObject](DependencyObject.md)
- [BindingExpression](BindingExpression.md)
