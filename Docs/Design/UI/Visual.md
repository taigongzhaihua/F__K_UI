# Visual - 设计文档

## 概览

**目的**：提供视觉树系统的基础，所有可见UI元素的根基类

**命名空间**：`fk::ui`

**头文件**：`fk/ui/Visual.h`

## 设计目标

1. **视觉树管理** - 高效的父子关系管理
2. **渲染支持** - 为派生类提供渲染基础设施
3. **变换系统** - 支持2D变换（旋转、缩放、平移等）
4. **命中测试** - 支持鼠标输入的命中测试
5. **性能** - 最小化内存占用和计算开销

## 架构

### 类设计

```cpp
class Visual : public DependencyObject {
public:
    // 视觉树管理
    void AddChild(Visual* child);
    void RemoveChild(Visual* child);
    Visual* GetParent() const;
    const std::vector<Visual*>& GetChildren() const;
    
    // 渲染
    virtual void OnRender(const RenderContext& context);
    void InvalidateVisual();
    
    // 依赖属性
    static const DependencyProperty& OpacityProperty();
    static const DependencyProperty& VisibilityProperty();
    static const DependencyProperty& RenderTransformProperty();
    
protected:
    // 内部渲染方法
    void Render(const RenderContext& context);
    void RenderChildren(const RenderContext& context);
    
private:
    Visual* parent_;
    std::vector<Visual*> children_;
    bool isVisualDirty_;
};
```

## 设计决策

### 1. 父子关系使用原始指针

**决策**：使用原始指针而非 shared_ptr

**理由**：
- 避免循环引用（父-子相互引用会导致内存泄漏）
- 公共API使用shared_ptr，内部树关系使用原始指针
- 生命周期由外部shared_ptr管理

**实现**：
```cpp
// 公共API
auto parent = std::make_shared<Visual>();
auto child = std::make_shared<Visual>();

// 内部存储原始指针
parent->AddChild(child.get());  // 弱引用
```

### 2. 延迟渲染

**决策**：使用脏标记延迟渲染

**理由**：
- 性能优化 - 避免不必要的重绘
- 批量更新 - 多次属性变更只触发一次渲染

**实现**：
```cpp
void Visual::SetOpacity(double opacity) {
    SetValue(OpacityProperty(), opacity);
    InvalidateVisual();  // 标记为脏
}

void Visual::Render(const RenderContext& context) {
    if (!isVisualDirty_) return;  // 跳过干净的元素
    
    OnRender(context);
    RenderChildren(context);
    isVisualDirty_ = false;
}
```

### 3. 变换累积

**决策**：变换从根到叶累积

**理由**：
- 符合视觉树层次结构
- 子元素自动继承父元素变换
- 支持复杂的组合变换

**实现**：
```cpp
void Visual::Render(const RenderContext& context) {
    // 保存当前变换
    auto savedTransform = context.GetTransform();
    
    // 应用本元素的变换
    if (auto transform = GetRenderTransform()) {
        context.PushTransform(transform->GetMatrix());
    }
    
    // 渲染自身和子元素
    OnRender(context);
    RenderChildren(context);
    
    // 恢复变换
    context.SetTransform(savedTransform);
}
```

### 4. 可见性和不透明度

**决策**：在渲染时检查可见性和不透明度

**理由**：
- 性能优化 - 跳过不可见元素
- 正确的渲染顺序
- 支持半透明效果

**实现**：
```cpp
void Visual::Render(const RenderContext& context) {
    // 检查可见性
    auto visibility = GetValue<Visibility>(VisibilityProperty());
    if (visibility == Visibility::Collapsed) {
        return;  // 完全跳过
    }
    
    // 应用不透明度
    auto opacity = GetValue<double>(OpacityProperty());
    if (opacity < 0.01) {
        return;  // 几乎透明，跳过
    }
    
    context.PushOpacity(opacity);
    OnRender(context);
    RenderChildren(context);
    context.PopOpacity();
}
```

## 内存管理

### 所有权模型

```
Window (shared_ptr)
    └─ Content (shared_ptr)
        └─ Children (shared_ptr)
            └─ Visual树关系（原始指针）
```

### 生命周期

1. 创建：使用shared_ptr创建
2. 添加到树：AddChild建立弱引用
3. 从树移除：RemoveChild清除弱引用
4. 销毁：最后一个shared_ptr销毁时自动清理

### 避免悬空指针

```cpp
void Visual::RemoveChild(Visual* child) {
    // 移除前检查
    if (!child) return;
    
    auto it = std::find(children_.begin(), children_.end(), child);
    if (it != children_.end()) {
        child->parent_ = nullptr;  // 清除父引用
        children_.erase(it);
    }
}
```

## 性能优化

### 1. 脏标记系统

```cpp
class Visual {
    bool isVisualDirty_;      // 渲染脏标记
    bool isMeasureDirty_;     // 布局测量脏标记
    bool isArrangeDirty_;     // 布局排列脏标记
};
```

### 2. 空间数据结构

考虑使用四叉树或R树优化命中测试（未来）：

```
┌─────────────────┐
│     Quad 1      │ Quad 2 │
│   ┌─────┐       │        │
│   │ Obj │       │  Obj   │
├───┴─────┴───────┼────────┤
│   Quad 3        │ Quad 4 │
│                 │        │
│      Obj        │        │
└─────────────────┴────────┘
```

### 3. 渲染批处理

将相似的绘制命令批处理（未来优化）：

```cpp
// 收集所有矩形
std::vector<Rect> rectangles;
// 一次性绘制
context.DrawRectangles(rectangles);
```

## 测试策略

### 单元测试

```cpp
TEST(VisualTest, ParentChild) {
    auto parent = std::make_shared<Visual>();
    auto child = std::make_shared<Visual>();
    
    parent->AddChild(child.get());
    
    EXPECT_EQ(child->GetParent(), parent.get());
    EXPECT_EQ(parent->GetChildren().size(), 1);
}

TEST(VisualTest, Opacity) {
    auto visual = std::make_shared<Visual>();
    visual->SetValue(Visual::OpacityProperty(), 0.5);
    
    EXPECT_DOUBLE_EQ(visual->GetValue<double>(Visual::OpacityProperty()), 0.5);
}

TEST(VisualTest, InvalidateVisual) {
    auto visual = std::make_shared<Visual>();
    EXPECT_FALSE(visual->IsVisualDirty());
    
    visual->InvalidateVisual();
    EXPECT_TRUE(visual->IsVisualDirty());
}
```

## 未来增强

1. **3D支持** - 扩展到3D变换和渲染
2. **合成层** - 离屏渲染和硬件加速
3. **效果系统** - 模糊、阴影等视觉效果
4. **缓存位图** - 缓存复杂视觉树的渲染结果

## 另请参阅

- [API文档](../../API/UI/Visual.md)
- [UIElement设计](UIElement.md)
- [渲染系统设计](../../Designs/RenderSystem.md)
