# INotifyPropertyChanged 设计文档

## 类概述

INotifyPropertyChanged 是属性变更通知的核心接口，所有支持数据绑定的类都应实现此接口。

## 继承关系

```
无基类（纯接口）
```

## 核心职责

1. 定义PropertyChanged事件
2. 提供属性变更通知机制

## 实现状态

### 已实现功能 ✅

- ✅ 接口定义
- ✅ 事件声明

### 未实现功能 ❌

- ❌ 批量通知支持（PropertyChanged("*"）

## 扩展方向

参见完整的API文档以了解详细的扩展建议。

## 相关文档

- [API 文档](../../API/Binding/INotifyPropertyChanged.md)
- [DependencyObject 设计文档](./DependencyObject.md)
- [Binding 设计文档](./Binding.md)
