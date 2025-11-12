# UIElement - 设计文档

## 概览

**目的**：为可交互UI元素提供输入处理和布局系统集成

## 设计目标

1. **输入抽象** - 统一处理鼠标、键盘、触摸输入
2. **布局参与** - 集成到布局系统
3. **焦点管理** - 支持键盘焦点
4. **事件路由** - 实现路由事件系统
5. **性能** - 高效的输入处理和布局

## 架构

### 类设计

```cpp
class UIElement : public Visual {
public:
    // 输入事件
    core::Event<const MouseEventArgs&> MouseEnter;
    core::Event<const MouseEventArgs&> MouseLeave;
    core::Event<const MouseButtonEventArgs&> MouseDown;
    core::Event<const KeyEventArgs&> KeyDown;
    
    // 布局
    void Measure(const Size& availableSize);
    void Arrange(const Rect& finalRect);
    
    // 焦点
    bool Focus();
    bool IsFocused() const;
    
protected:
    virtual void OnMouseEnter(const MouseEventArgs& e);
    virtual void OnKeyDown(const KeyEventArgs& e);
    
    virtual Size MeasureCore(const Size& availableSize);
    virtual void ArrangeCore(const Rect& finalRect);
};
```

## 设计决策

### 1. 路由事件vs直接事件

**决策**：使用直接事件（core::Event）而非完整的路由事件

**理由**：
- 简化实现
- 足够的灵活性
- 可以后期添加路由支持

### 2. 两遍布局

**决策**：采用Measure-Arrange两遍布局

**理由**：
- 符合WPF模型
- 灵活处理复杂布局
- 支持约束传播

**实现**：
```cpp
void UIElement::Measure(const Size& availableSize) {
    if (!isMeasureDirty_) return;
    
    desiredSize_ = MeasureCore(availableSize);
    isMeasureDirty_ = false;
}

void UIElement::Arrange(const Rect& finalRect) {
    if (!isArrangeDirty_) return;
    
    ArrangeCore(finalRect);
    renderSize_ = finalRect.Size();
    isArrangeDirty_ = false;
}
```

### 3. 焦点管理

**决策**：集中式焦点管理

**理由**：
- 全局只能有一个焦点元素
- 简化焦点跟踪
- 支持焦点导航

## 另请参阅

- [API文档](../../API/UI/UIElement.md)
- [Visual设计](Visual.md)
