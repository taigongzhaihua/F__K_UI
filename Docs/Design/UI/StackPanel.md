# StackPanel - 设计文档

## 概览

**目的**：简单的顺序布局容器

## 布局算法

### 垂直堆栈

```cpp
Size StackPanel::MeasureOverride(const Size& availableSize) {
    double totalHeight = 0;
    double maxWidth = 0;
    
    for (auto& child : Children()) {
        child->Measure(Size(availableSize.Width, INFINITY));
        auto desired = child->DesiredSize();
        
        totalHeight += desired.Height + spacing_;
        maxWidth = std::max(maxWidth, desired.Width);
    }
    
    return Size(maxWidth, totalHeight);
}

void StackPanel::ArrangeOverride(const Size& finalSize) {
    double y = 0;
    
    for (auto& child : Children()) {
        double height = child->DesiredSize().Height;
        child->Arrange(Rect(0, y, finalSize.Width, height));
        y += height + spacing_;
    }
}
```

## 性能

- O(n) 测量和排列
- 无需复杂计算
- 适合简单列表

## 另请参阅

- [API文档](../../API/UI/StackPanel.md)
- [Panel设计](Panel.md)
