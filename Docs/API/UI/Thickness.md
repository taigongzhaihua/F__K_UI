# Thickness

## 概览

**目的**：表示四边厚度（边距、内边距、边框等）

**命名空间**：`fk::ui`

**头文件**：`fk/ui/Thickness.h`

## 描述

`Thickness` 结构表示矩形四边的厚度值。

## 构造函数

```cpp
Thickness();                          // 全0
Thickness(double uniform);            // 四边相同
Thickness(double horizontal, double vertical);
Thickness(double left, double top, double right, double bottom);
```

## 使用示例

### 统一厚度
```cpp
Thickness margin(10);  // 四边都是10
element->Margin(margin);
```

### 不同厚度
```cpp
Thickness padding(5, 10, 5, 10);  // 左右5，上下10
element->Padding(padding);
```

### 边框
```cpp
border->BorderThickness(Thickness(2, 0, 2, 0));  // 仅左右边框
```
