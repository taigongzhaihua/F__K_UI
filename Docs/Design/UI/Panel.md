# Panel - 设计文档

## 概览

**目的**：为多子元素布局容器提供基类和共同功能

**命名空间**：`fk::ui`

**头文件**：`fk/ui/Panel.h`

## 设计目标

1. **通用容器** - 为所有布局容器提供共同基础
2. **灵活布局** - 支持派生类实现自定义布局逻辑
3. **性能** - 高效的子元素管理和布局计算
4. **易用性** - 简洁的API用于添加和管理子元素

## 架构

### 类层次结构

```
FrameworkElement<Panel>
    └── Panel<Derived> (CRTP)
        ├── StackPanel
        ├── Grid
        └── Canvas (未来)
```

### 类设计

```cpp
template<typename Derived>
class Panel : public FrameworkElement<Derived> {
public:
    // 子元素管理
    template<typename T>
    std::shared_ptr<T> AddChild();
    
    void AddChild(std::shared_ptr<UIElement> child);
    void RemoveChild(std::shared_ptr<UIElement> child);
    void ClearChildren();
    
    const std::vector<std::shared_ptr<UIElement>>& GetChildren() const;
    
    // 布局重写（派生类实现）
    virtual Size MeasureOverride(const Size& availableSize) = 0;
    virtual Size ArrangeOverride(const Size& finalSize) = 0;
    
protected:
    std::vector<std::shared_ptr<UIElement>> children_;
};
```

## 设计决策

### 1. CRTP 模式

**决策**：使用 CRTP (Curiously Recurring Template Pattern)

**理由**：
- 类型安全的方法链
- 编译时多态，无虚函数开销
- 返回正确的派生类型

**示例**：
```cpp
auto stack = std::make_shared<StackPanel>();
stack->Orientation(Orientation::Vertical)  // 返回 StackPanel*
     ->Spacing(10)                          // 返回 StackPanel*
     ->Background(Colors::White);           // 返回 StackPanel*
```

### 2. shared_ptr 管理子元素

**决策**：使用 shared_ptr 存储子元素

**理由**：
- 明确的所有权 - Panel拥有其子元素
- 自动内存管理
- 安全的生命周期

**实现**：
```cpp
std::vector<std::shared_ptr<UIElement>> children_;

template<typename T>
std::shared_ptr<T> Panel::AddChild() {
    auto child = std::make_shared<T>();
    children_.push_back(child);
    child->SetLogicalParent(this);
    return child;
}
```

### 3. 抽象布局方法

**决策**：MeasureOverride和ArrangeOverride为纯虚函数

**理由**：
- 强制派生类实现布局逻辑
- 每种Panel有独特的布局算法
- 保持基类简洁

**示例实现（StackPanel）**：
```cpp
Size StackPanel::MeasureOverride(const Size& availableSize) {
    Size totalSize;
    
    for (auto& child : GetChildren()) {
        child->Measure(availableSize);
        auto desired = child->GetDesiredSize();
        
        if (orientation_ == Orientation::Vertical) {
            totalSize.Width = std::max(totalSize.Width, desired.Width);
            totalSize.Height += desired.Height + spacing_;
        } else {
            totalSize.Width += desired.Width + spacing_;
            totalSize.Height = std::max(totalSize.Height, desired.Height);
        }
    }
    
    return totalSize;
}
```

### 4. 自动视觉树同步

**决策**：添加子元素时自动更新视觉树

**理由**：
- 保持逻辑树和视觉树同步
- 简化API - 用户不需要手动管理两个树

**实现**：
```cpp
void Panel::AddChild(std::shared_ptr<UIElement> child) {
    children_.push_back(child);
    
    // 更新逻辑树
    child->SetLogicalParent(this);
    AddLogicalChild(child.get());
    
    // 更新视觉树
    Visual::AddChild(child.get());
    
    // 触发布局
    InvalidateMeasure();
}
```

## 布局算法

### 两遍布局

Panel参与框架的两遍布局系统：

**测量遍（Measure Pass）**：
1. Panel调用每个子元素的Measure()
2. 子元素返回DesiredSize
3. Panel根据子元素的DesiredSize计算自己的DesiredSize

**排列遍（Arrange Pass）**：
1. Panel调用每个子元素的Arrange()
2. 为每个子元素分配最终位置和大小
3. 子元素在其分配的矩形内渲染

```
Parent.Measure(availableSize)
    ↓
Panel.MeasureOverride(availableSize)
    ↓
foreach child: child.Measure(childConstraint)
    ↓
Panel returns desiredSize
    ↓
Parent.Arrange(finalRect)
    ↓
Panel.ArrangeOverride(finalSize)
    ↓
foreach child: child.Arrange(childRect)
```

## 性能考虑

### 1. 批量失效

```cpp
void Panel::BeginBatchUpdate() {
    batchUpdateInProgress_ = true;
}

void Panel::EndBatchUpdate() {
    batchUpdateInProgress_ = false;
    if (hasPendingInvalidation_) {
        InvalidateMeasure();
    }
}

void Panel::InvalidateMeasure() {
    if (batchUpdateInProgress_) {
        hasPendingInvalidation_ = true;
    } else {
        // 实际失效
    }
}
```

### 2. 增量布局

考虑只重新布局变更的子元素（未来优化）：

```cpp
void Panel::OnChildMeasureInvalidated(UIElement* child) {
    // 只重新测量受影响的子元素
    if (CanUseIncrementalLayout()) {
        RemeasureChild(child);
    } else {
        InvalidateMeasure();
    }
}
```

### 3. 虚拟化

对于大量子元素，考虑UI虚拟化（未来）：

```cpp
class VirtualizingPanel : public Panel<VirtualizingPanel> {
    // 只创建和布局可见的元素
    void Realize(int startIndex, int endIndex);
    void Virtualize(int startIndex, int endIndex);
};
```

## 测试策略

### 单元测试

```cpp
TEST(PanelTest, AddRemoveChildren) {
    auto panel = std::make_shared<TestPanel>();
    auto child1 = std::make_shared<UIElement>();
    auto child2 = std::make_shared<UIElement>();
    
    panel->AddChild(child1);
    panel->AddChild(child2);
    
    EXPECT_EQ(panel->GetChildren().size(), 2);
    
    panel->RemoveChild(child1);
    EXPECT_EQ(panel->GetChildren().size(), 1);
}

TEST(PanelTest, MeasureArrange) {
    auto panel = std::make_shared<StackPanel>();
    panel->AddChild<UIElement>()->Width(100)->Height(50);
    panel->AddChild<UIElement>()->Width(100)->Height(50);
    
    panel->Measure(Size(200, 200));
    auto desired = panel->GetDesiredSize();
    
    EXPECT_EQ(desired.Width, 100);
    EXPECT_EQ(desired.Height, 100);
}
```

## 派生类实现指南

### 创建自定义Panel

```cpp
class MyPanel : public Panel<MyPanel> {
public:
    MyPanel() {
        // 初始化
    }
    
protected:
    Size MeasureOverride(const Size& availableSize) override {
        Size totalSize;
        
        for (auto& child : GetChildren()) {
            // 为子元素提供约束
            Size childConstraint = /* 计算约束 */;
            child->Measure(childConstraint);
            
            // 累积大小
            auto desired = child->GetDesiredSize();
            // ... 更新totalSize
        }
        
        return totalSize;
    }
    
    Size ArrangeOverride(const Size& finalSize) override {
        Point currentPosition;
        
        for (auto& child : GetChildren()) {
            // 计算子元素的最终矩形
            auto childSize = child->GetDesiredSize();
            Rect childRect(currentPosition, childSize);
            
            // 排列子元素
            child->Arrange(childRect);
            
            // 更新位置
            // ... 移动currentPosition
        }
        
        return finalSize;
    }
};
```

## 未来增强

1. **附加属性** - 支持子元素的附加属性（如Grid.Row）
2. **Z-Order** - 子元素的Z顺序控制
3. **裁剪** - 自动裁剪超出边界的子元素
4. **滚动** - 内置滚动支持

## 另请参阅

- [API文档](../../API/UI/Panel.md)
- [StackPanel设计](StackPanel.md)
- [Grid设计](Grid.md)
- [布局系统](../../Designs/LayoutSystem.md)
