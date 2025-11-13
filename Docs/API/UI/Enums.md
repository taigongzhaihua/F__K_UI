# Enums

## 概览

**目的**：UI常用枚举类型

**命名空间**：`fk::ui`

**头文件**：`fk/ui/Enums.h`

## 枚举类型

### Visibility
元素可见性。
```cpp
enum class Visibility {
    Visible,    // 可见
    Hidden,     // 隐藏但占用空间
    Collapsed   // 隐藏且不占用空间
};
```

### Orientation
方向。
```cpp
enum class Orientation {
    Horizontal,  // 水平
    Vertical     // 垂直
};
```

### HorizontalAlignment
水平对齐。
```cpp
enum class HorizontalAlignment {
    Left,
    Center,
    Right,
    Stretch
};
```

### VerticalAlignment
垂直对齐。
```cpp
enum class VerticalAlignment {
    Top,
    Center,
    Bottom,
    Stretch
};
```

### FontWeight
字体粗细。
```cpp
enum class FontWeight {
    Thin,
    Light,
    Normal,
    Bold,
    Black
};
```

### TextAlignment
文本对齐。
```cpp
enum class TextAlignment {
    Left,
    Center,
    Right,
    Justify
};
```

## 使用示例

```cpp
element->SetValue(UIElement::VisibilityProperty(), Visibility::Visible);
panel->Orientation(Orientation::Horizontal);
element->HorizontalAlignment(HorizontalAlignment::Center);
```
