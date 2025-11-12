# Setter

## 概览

**目的**：样式中的属性设置器

**命名空间**：`fk::ui`

**头文件**：`fk/ui/Setter.h`

## 描述

`Setter` 在样式中设置单个属性值。

## 公共接口

### 创建设置器

```cpp
Setter(const DependencyProperty& property, const std::any& value);
```

## 使用示例

### 在样式中使用
```cpp
auto style = std::make_shared<Style>(typeid(Button));

style->Setters().Add(
    std::make_shared<Setter>(Control::BackgroundProperty(), Colors::Blue)
);
style->Setters().Add(
    std::make_shared<Setter>(Control::ForegroundProperty(), Colors::White)
);
style->Setters().Add(
    std::make_shared<Setter>(Control::FontSizeProperty(), 14.0)
);
```

## 相关类

- [Style](Style.md)
- [DependencyProperty](../Binding/DependencyProperty.md)
