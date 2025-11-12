# INotifyDataErrorInfo 设计文档

## 类概述

INotifyDataErrorInfo 提供数据验证错误通知功能，用于显示验证错误信息。

## 继承关系

```
无基类（纯接口）
```

## 核心职责

1. 定义ErrorsChanged事件
2. 提供错误信息查询
3. 支持异步验证

## 实现状态

### 已实现功能 ✅

- ✅ 接口定义
- ✅ 基本错误存储

### 简单实现须扩充 ⚠️

- ⚠️ 错误信息管理较简单

### 未实现功能 ❌

- ❌ 异步验证支持
- ❌ 多级错误严重性

## 扩展方向

参见完整的API文档以了解详细的扩展建议。

## 相关文档

- [API 文档](../../API/Binding/INotifyDataErrorInfo.md)
- [DependencyObject 设计文档](./DependencyObject.md)
- [Binding 设计文档](./Binding.md)
