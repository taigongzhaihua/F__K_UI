# Transform

## 概览

**目的**：2D变换的基类

**命名空间**：`fk::ui`

**头文件**：`fk/ui/Transform.h`

## 描述

`Transform` 是所有2D变换的基类。

## 派生类

- **TranslateTransform** - 平移
- **ScaleTransform** - 缩放
- **RotateTransform** - 旋转
- **SkewTransform** - 倾斜
- **MatrixTransform** - 矩阵变换
- **TransformGroup** - 组合变换

## 使用示例

### 平移
```cpp
auto translate = std::make_shared<TranslateTransform>();
translate->X(50)->Y(100);
element->SetRenderTransform(translate.get());
```

### 旋转
```cpp
auto rotate = std::make_shared<RotateTransform>();
rotate->Angle(45)->CenterX(50)->CenterY(50);
element->SetRenderTransform(rotate.get());
```

### 缩放
```cpp
auto scale = std::make_shared<ScaleTransform>();
scale->ScaleX(2.0)->ScaleY(2.0);
element->SetRenderTransform(scale.get());
```

### 组合变换
```cpp
auto group = std::make_shared<TransformGroup>();
group->AddTransform(std::make_shared<ScaleTransform>(2.0, 2.0));
group->AddTransform(std::make_shared<RotateTransform>(45));
element->SetRenderTransform(group.get());
```

## 相关类

- [Visual](Visual.md)
- [UIElement](UIElement.md)
