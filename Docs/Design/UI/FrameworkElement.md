# FrameworkElement - 设计文档

## 概览

**目的**：添加布局系统支持和数据上下文

## 设计目标

1. **布局系统** - 提供对齐、边距、大小约束
2. **数据绑定** - 支持DataContext继承
3. **CRTP模式** - 类型安全的方法链
4. **性能优化** - 缓存布局计算

## CRTP设计

### 动机

使用CRTP实现类型安全的流畅API：

```cpp
template<typename Derived>
class FrameworkElement : public UIElement {
public:
    Derived* Width(double width) {
        SetValue(WidthProperty(), width);
        return static_cast<Derived*>(this);
    }
};

// 使用
button->Width(100)->Height(50)->Background(Colors::Blue);
```

**优势**：
- 返回正确的派生类型
- 无虚函数开销
- 编译时多态

## 布局算法

### Measure Pass

```cpp
Size FrameworkElement::MeasureOverride(const Size& availableSize) {
    // 1. 应用大小约束
    Size constraint = ApplyConstraints(availableSize);
    
    // 2. 测量子元素（如果有）
    Size desiredSize = MeasureChild(constraint);
    
    // 3. 应用边距
    desiredSize += margin;
    
    return desiredSize;
}
```

### Arrange Pass

```cpp
void FrameworkElement::ArrangeOverride(const Rect& finalRect) {
    // 1. 应用对齐
    Rect arrangeRect = ApplyAlignment(finalRect);
    
    // 2. 应用边距
    arrangeRect = ApplyMargin(arrangeRect);
    
    // 3. 排列子元素
    ArrangeChild(arrangeRect);
}
```

## DataContext继承

**实现**：沿逻辑树向下继承

```cpp
std::any FrameworkElement::GetDataContext() const {
    // 检查本地值
    auto local = GetLocalValue(DataContextProperty());
    if (local.has_value()) return local;
    
    // 从父元素继承
    auto parent = GetLogicalParent();
    if (parent) return parent->GetDataContext();
    
    return nullptr;
}
```

## 另请参阅

- [API文档](../../API/UI/FrameworkElement.md)
- [UIElement设计](UIElement.md)
