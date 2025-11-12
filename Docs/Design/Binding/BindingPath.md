# BindingPath 设计文档

## 类概述

BindingPath 负责解析和导航属性路径，支持复杂的路径表达式如 'Parent.Child.Value'。

## 继承关系

```
无基类
```

## 核心职责

1. 解析属性路径字符串
2. 导航对象图
3. 支持索引器访问
4. 处理路径变更

## 实现状态

### 已实现功能 ✅

- ✅ 基本的点分隔路径解析
- ✅ 简单属性访问

### 简单实现须扩充 ⚠️

- ⚠️ 索引器支持不完整
- ⚠️ 路径缓存机制简单

### 未实现功能 ❌

- ❌ 集合索引器 [0]
- ❌ 字典键访问 ['key']
- ❌ 附加属性路径

## 扩展方向

参见完整的API文档以了解详细的扩展建议。

## 相关文档

- [API 文档](../../API/Binding/BindingPath.md)
- [DependencyObject 设计文档](./DependencyObject.md)
- [Binding 设计文档](./Binding.md)
