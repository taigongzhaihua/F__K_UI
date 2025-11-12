# Binding - 设计文档

## 概览

**目的**：配置数据绑定的所有方面

## 设计目标

1. **声明式** - 简洁的绑定配置
2. **灵活性** - 支持多种绑定模式
3. **可扩展** - 自定义转换器和验证
4. **性能** - 延迟绑定解析

## 绑定解析流程

```
1. 创建Binding配置
   ↓
2. SetValue(property, binding)
   ↓
3. 创建BindingExpression
   ↓
4. 解析源对象（延迟）
   ↓
5. 订阅PropertyChanged
   ↓
6. 初始UpdateTarget()
   ↓
7. 源变更时自动更新
```

## 绑定模式

### OneWay
源 → 目标

```cpp
Binding("Username").Mode(BindingMode::OneWay)
```

### TwoWay
源 ⇄ 目标

```cpp
Binding("Username").Mode(BindingMode::TwoWay)
```

### OneTime
源 → 目标（一次）

### OneWayToSource
源 ← 目标

## 值转换

```
源值 → Converter.Convert() → 目标值
源值 ← Converter.ConvertBack() ← 目标值
```

## 另请参阅

- [API文档](../../API/Binding/Binding.md)
- [BindingExpression设计](BindingExpression.md)
