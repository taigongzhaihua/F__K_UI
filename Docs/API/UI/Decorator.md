# Decorator - 装饰器容器

## 概述

`Decorator` 是一个容器控件，用于在内容周围添加装饰（如边框、背景、阴影等），而不改变内容本身的布局和功能。它是实现高级样式效果的基础。

## 核心特性

- 单子元素容器
- 灵活的装饰能力
- 完全的布局传递

## 使用示例

```cpp
auto decorator = fk::ui::Decorator::Create()
    ->Child(contentElement)
    ->Background("lightgray")
    ->BorderThickness(fk::Thickness(2.0f));
```

## 相关文档

- [ContentControl.md](./ContentControl.md) - 单内容容器基类
- [Panel.md](./Panel.md) - 容器基类
