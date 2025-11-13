# CornerRadius

## 概览

**目的**：表示圆角半径

**命名空间**：`fk::ui`

**头文件**：`fk/ui/CornerRadius.h`

## 描述

`CornerRadius` 结构表示矩形四个角的圆角半径。

## 构造函数

```cpp
CornerRadius();
CornerRadius(double uniform);
CornerRadius(double topLeft, double topRight, double bottomRight, double bottomLeft);
```

## 使用示例

### 统一圆角
```cpp
border->CornerRadius(CornerRadius(5));  // 四角都是5
```

### 不同圆角
```cpp
border->CornerRadius(CornerRadius(10, 10, 0, 0));  // 仅顶部圆角
```

## 相关类

- [Border](Border.md)
