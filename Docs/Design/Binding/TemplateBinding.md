# TemplateBinding 设计文档

## 类概述

TemplateBinding 是模板内使用的特殊绑定，将模板化控件的属性绑定到模板内元素。

## 继承关系

```
BindingBase (基类)
```

## 核心职责

1. 连接控件属性到模板元素
2. 支持单向绑定
3. 优化模板绑定性能

## 实现状态

### 已实现功能 ✅

- ✅ 基本的模板绑定
- ✅ 单向数据流

### 简单实现须扩充 ⚠️

- ⚠️ 性能可进一步优化

### 未实现功能 ❌

- ❌ 双向模板绑定
- ❌ 模板绑定调试支持

## 扩展方向

参见完整的API文档以了解详细的扩展建议。

## 相关文档

- [API 文档](../../API/Binding/TemplateBinding.md)
- [DependencyObject 设计文档](./DependencyObject.md)
- [Binding 设计文档](./Binding.md)
