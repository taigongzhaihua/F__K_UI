# BindingExpression 设计文档

## 类概述

BindingExpression 是数据绑定的运行时实例，负责管理源属性和目标属性之间的数据流动。

## 继承关系

```
BindingExpressionBase (基类)
```

## 核心职责

1. 管理源到目标的数据传输
2. 处理绑定模式（OneWay/TwoWay）
3. 监听源属性变更
4. 触发目标属性更新
5. 处理值转换器

## 实现状态

### 已实现功能 ✅

- ✅ 基本的单向绑定（OneWay）
- ✅ 双向绑定（TwoWay）
- ✅ 值转换器应用
- ✅ 属性变更监听

### 简单实现须扩充 ⚠️

- ⚠️ 错误处理机制较简单
- ⚠️ 绑定失败的fallback值支持不完善

### 未实现功能 ❌

- ❌ 绑定优先级处理
- ❌ 异步绑定更新
- ❌ 绑定延迟评估

## 扩展方向

参见完整的API文档以了解详细的扩展建议。

## 相关文档

- [API 文档](../../API/Binding/BindingExpression.md)
- [DependencyObject 设计文档](./DependencyObject.md)
- [Binding 设计文档](./Binding.md)
