# Style - 设计文档

## 概览

**目的**：实现样式系统以统一控件外观

## 设计目标

1. **属性批量设置** - 一次设置多个属性
2. **样式继承** - 基于其他样式
3. **类型安全** - 仅应用于匹配的类型
4. **性能** - 高效的样式应用

## 样式应用流程

```
1. 检查TargetType
   ↓
2. 应用BasedOn样式（如果有）
   ↓
3. 应用本样式的Setters
   ↓
4. 存储为非本地值
```

## 值优先级

```
本地值（最高）
  ↓
样式值
  ↓
继承值
  ↓
默认值（最低）
```

## 隐式样式

**ResourceDictionary中的类型键**：

```cpp
// 定义隐式样式
resources->Add(typeid(Button), buttonStyle);

// 自动应用到所有Button
// 除非Button有显式Style
```

## 另请参阅

- [API文档](../../API/UI/Style.md)
- [ResourceDictionary设计](ResourceDictionary.md)
