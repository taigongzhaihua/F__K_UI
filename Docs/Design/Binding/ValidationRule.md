# ValidationRule 设计文档

## 类概述

ValidationRule 是数据验证规则的基类，用于定义自定义验证逻辑。

## 继承关系

```
无基类
```

## 核心职责

1. 定义验证接口
2. 执行验证逻辑
3. 返回验证结果

## 实现状态

### 已实现功能 ✅

- ✅ 基本验证框架
- ✅ 验证结果类型

### 简单实现须扩充 ⚠️

- ⚠️ 内置验证规则较少

### 未实现功能 ❌

- ❌ 异步验证规则
- ❌ 验证规则链
- ❌ 条件验证

## 扩展方向

参见完整的API文档以了解详细的扩展建议。

## 相关文档

- [API 文档](../../API/Binding/ValidationRule.md)
- [DependencyObject 设计文档](./DependencyObject.md)
- [Binding 设计文档](./Binding.md)
