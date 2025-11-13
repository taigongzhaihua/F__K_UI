# BindingPath

## 概览

**目的**：解析和遍历属性路径

**命名空间**：`fk::binding`

**头文件**：`fk/binding/BindingPath.h`

## 描述

`BindingPath` 解析属性路径字符串并提供遍历功能。

## 支持的路径语法

- 简单属性：`"PropertyName"`
- 嵌套属性：`"Object.Property.SubProperty"`
- 索引器：`"Collection[0]"`
- 附加属性：`"(OwnerType.PropertyName)"`

## 使用示例

### 路径解析
```cpp
BindingPath path("User.Address.City");

// 遍历路径
auto value = path.GetValue(sourceObject);
```

## 相关类

- [Binding](Binding.md)
- [BindingExpression](BindingExpression.md)
