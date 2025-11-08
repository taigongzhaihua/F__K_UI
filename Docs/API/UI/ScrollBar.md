# ScrollBar - 滚动条控件

## 概述

`ScrollBar` 提供了滚动条的完整实现，支持水平和竖直两种方向、鼠标拖动、轨道点击等交互，通常与 `ScrollViewer` 配合使用。

## 核心属性

### 数值范围属性

```cpp
FK_DEPENDENCY_PROPERTY_DECLARE(Minimum, double)
FK_DEPENDENCY_PROPERTY_DECLARE(Maximum, double)
FK_DEPENDENCY_PROPERTY_DECLARE(Value, double)
FK_DEPENDENCY_PROPERTY_DECLARE(ViewportSize, double)
```

- `Minimum`: 最小值（默认 0）
- `Maximum`: 最大值（默认 100）
- `Value`: 当前值（必须在 [Minimum, Maximum] 范围）
- `ViewportSize`: 可见区域大小

### 方向和外观

```cpp
FK_DEPENDENCY_PROPERTY_DECLARE(Orientation, ui::Orientation)
FK_DEPENDENCY_PROPERTY_DECLARE_REF(ThumbBrush, std::string)
FK_DEPENDENCY_PROPERTY_DECLARE_REF(TrackBrush, std::string)
FK_DEPENDENCY_PROPERTY_DECLARE(Thickness, float)
```

## 事件

```cpp
core::Event<double> ValueChanged;
```

当 Value 属性变更时触发，传递新值。

## 常见用法

### 基本创建

```cpp
auto scrollBar = fk::ui::ScrollBar::Create()
    ->Orientation(fk::ui::Orientation::Vertical)
    ->Minimum(0)
    ->Maximum(100)
    ->Value(50)
    ->ViewportSize(10);

// 订阅值变更
scrollBar->OnValueChanged([](double newValue) {
    std::cout << "滚动条值: " << newValue << "\n";
});
```

### 与 ScrollViewer 配合

```cpp
auto scrollViewer = fk::ui::ScrollViewer::Create()
    ->HorizontalScrollBar(hScrollBar)
    ->VerticalScrollBar(vScrollBar);
```

### 手动更新视图

```cpp
void UpdateScrollBar(
    std::shared_ptr<fk::ui::ScrollBar> scrollBar,
    float contentHeight,
    float viewportHeight) {
    
    scrollBar->SetMaximum(contentHeight - viewportHeight);
    scrollBar->SetViewportSize(viewportHeight);
}
```

## 最佳实践

- 始终设置 Minimum 和 Maximum
- ViewportSize 应小于等于 Maximum - Minimum
- 订阅 ValueChanged 事件处理用户交互

## 相关文档

- [ScrollViewer.md](./ScrollViewer.md) - 滚动查看器
- [Orientation 枚举](./Enums.md) - 方向枚举
