# Grid - 设计文档

## 概览

**目的**：实现灵活的基于行列的布局系统

## 布局算法

### Measure Pass

```cpp
Size Grid::MeasureOverride(const Size& availableSize) {
    // 1. 解析行列定义
    ResolveRowDefinitions(availableSize.Height);
    ResolveColumnDefinitions(availableSize.Width);
    
    // 2. 测量每个单元格
    for (auto& child : Children()) {
        int row = GetRow(child);
        int col = GetColumn(child);
        
        Size cellConstraint = CalculateCellSize(row, col);
        child->Measure(cellConstraint);
    }
    
    // 3. 计算总大小
    return CalculateTotalSize();
}
```

### Star大小计算

```
总可用空间 = 固定大小之和 + Star空间

Star空间 / Star总数 = 每个Star的实际大小
```

## 性能优化

1. **缓存** - 缓存行列大小计算
2. **增量更新** - 只重新计算变更的单元格
3. **跨越优化** - 优化跨行列元素的处理

## 另请参阅

- [API文档](../../API/UI/Grid.md)
- [Panel设计](Panel.md)
