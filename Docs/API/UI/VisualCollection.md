# VisualCollection

## 概览

**目的**：视觉子元素集合

**命名空间**：`fk::ui`

**头文件**：`fk/ui/VisualCollection.h`

## 描述

`VisualCollection` 管理视觉树的子元素集合。

## 公共接口

### 元素管理

#### Add / Remove / Clear
```cpp
void Add(Visual* visual);
void Remove(Visual* visual);
void Clear();
```

### 访问

#### Count / operator[]
```cpp
size_t Count() const;
Visual* operator[](size_t index) const;
```

## 使用示例

内部由Visual类使用。

## 相关类

- [Visual](Visual.md)
