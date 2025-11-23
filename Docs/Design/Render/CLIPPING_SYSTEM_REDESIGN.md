# 裁剪系统重构设计文档

**版本**: 1.1  
**日期**: 2025-11-23  
**状态**: 设计中  
**优先级**: P0（最高）  
**更新**: 添加高级裁剪几何体支持（圆角、椭圆、多边形、路径、变换）

---

## 📋 目录

1. [背景与动机](#背景与动机)
2. [现有系统分析](#现有系统分析)
3. [问题诊断](#问题诊断)
4. [新系统设计](#新系统设计)
   - [高级裁剪几何体支持](#高级裁剪几何体支持)
5. [实施计划](#实施计划)
6. [API 设计](#api-设计)
7. [测试策略](#测试策略)
8. [风险评估](#风险评估)

---

## 背景与动机

### 问题概述

当前项目中，控件的裁剪系统存在以下严重问题：

1. **功能混乱不成体系**: 裁剪功能散落在多个层次，缺乏统一的管理和协调机制
2. **裁剪边界传递几乎不存在**: 父子元素之间的裁剪边界信息传递不完整，导致子元素可能绘制在父元素边界之外
3. **子元素的子元素失管状态**: 深层嵌套的控件裁剪管理缺失，导致渲染错误和性能问题
4. **ScrollViewer等控件未正确裁剪**: 需要内容裁剪的控件（如ScrollViewer、Border等）没有正确实现裁剪逻辑

### 重构目标

1. 建立**统一的、成体系的裁剪架构**
2. 实现**完整的裁剪边界传递机制**
3. 提供**自动化的子元素裁剪管理**
4. 确保**性能优化**（裁剪剔除、减少绘制调用）
5. 支持**多种裁剪模式**（矩形裁剪、圆角裁剪、路径裁剪等）

---

## 现有系统分析

### 当前架构

#### 1. 裁剪相关的类和接口

```
RenderContext (fk::render)
├── PushClip(const ui::Rect& clipRect)
├── PopClip()
├── GetCurrentClip() const
└── IsClipped(const ui::Rect& rect) const

UIElement (fk::ui)
├── ClipProperty() - 依赖属性
├── SetClip(const Rect& value)
├── GetClip() const
├── HasClip() const
└── CollectDrawCommands(RenderContext& context)

GlRenderer (fk::render)
└── ApplyClip(const ClipPayload& payload)
    └── glScissor(...) - OpenGL裁剪实现
```

#### 2. 当前裁剪流程

```
UIElement::CollectDrawCommands(RenderContext& context)
  ↓
1. PushTransform(layoutRect_.x, layoutRect_.y)
  ↓
2. if (hasClip) { context.PushClip(GetClip()); }
  ↓
3. OnRender(context)
  ↓
4. Visual::CollectDrawCommands(context) // 递归子元素
  ↓
5. if (hasClip) { context.PopClip(); }
  ↓
6. PopTransform()
```

#### 3. RenderContext裁剪管理

```cpp
// RenderContext.cpp
void RenderContext::PushClip(const ui::Rect& clipRect) {
    // 1. 保存当前裁剪状态到栈
    clipStack_.push(currentClip_);
    
    // 2. 变换裁剪矩形到全局坐标
    ui::Rect globalClip = TransformRect(clipRect);
    
    // 3. 与当前裁剪求交集
    if (currentClip_.enabled) {
        // 计算交集矩形
        float x1 = std::max(currentClip_.clipRect.x, globalClip.x);
        float y1 = std::max(currentClip_.clipRect.y, globalClip.y);
        float x2 = std::min(...);
        float y2 = std::min(...);
        currentClip_.clipRect = ui::Rect{x1, y1, ...};
    } else {
        currentClip_.clipRect = globalClip;
    }
    
    currentClip_.enabled = true;
    
    // 4. 生成裁剪命令
    ApplyCurrentClip();
}
```

### 现有实现的优点

1. ✅ **基础架构清晰**: RenderContext作为中心管理裁剪栈
2. ✅ **交集计算正确**: 父子裁剪区域能够正确求交集
3. ✅ **OpenGL集成良好**: 使用glScissor实现硬件加速裁剪
4. ✅ **坐标变换处理**: 裁剪矩形能正确转换到全局坐标

### 现有实现的缺点

1. ❌ **ClipProperty使用率极低**: 仅在UIElement.cpp中手动调用，大部分控件未使用
2. ❌ **ScrollViewer未实现裁剪**: 标记了`// TODO: 实现裁剪`但未实际实现
3. ❌ **Border未实现内容裁剪**: Border控件没有裁剪子元素到Padding区域
4. ❌ **缺乏自动裁剪机制**: 需要控件主动调用PushClip/PopClip
5. ❌ **性能优化不足**: IsClipped()仅在绘制时检查，未进行提前剔除
6. ❌ **缺乏裁剪策略**: 没有定义哪些控件应该裁剪、如何裁剪

---

## 问题诊断

### 问题1: 裁剪边界传递不完整

**症状**:
- ScrollViewer的内容可以绘制到视口外部
- Border的子元素可以绘制到Padding区域外部
- Panel容器不会自动裁剪溢出的子元素

**根本原因**:
```cpp
// ScrollViewer.cpp:298
void ScrollViewer::OnRender(render::RenderContext& context) {
    // ...
    Rect viewportRect = CalculateViewportRect();
    // TODO: 实现裁剪  ← 仅有注释，未实现
    
    // 内容渲染会由视觉树系统自动处理
}
```

**影响**:
- 内容超出容器边界仍然可见
- 无法正确实现滚动效果
- 用户界面显示错误

### 问题2: 子元素的子元素失管

**症状**:
- 深层嵌套的控件树裁剪不正确
- 裁剪区域叠加计算错误
- 某些子元素未被裁剪

**根本原因**:
```cpp
// UIElement.cpp:370-409
void UIElement::CollectDrawCommands(render::RenderContext& context) {
    // ...
    bool hasClip = HasClip();  // ← 仅检查自身的ClipProperty
    if (hasClip) {
        context.PushClip(GetClip());
    }
    // ...
}
```

只有显式设置了ClipProperty的元素才会裁剪，大部分容器控件不会自动裁剪。

**影响**:
- Grid、StackPanel等容器的子元素可以溢出
- 嵌套容器的裁剪累积失效
- 无法实现复杂的UI布局

### 问题3: 功能混乱不成体系

**症状**:
- 裁剪功能分散在多个地方
- 没有统一的裁剪策略
- 不同控件的裁剪行为不一致

**问题点**:

1. **裁剪职责不清**:
   - RenderContext: 管理裁剪栈
   - UIElement: 提供ClipProperty
   - 各控件: 自行决定是否裁剪

2. **裁剪时机不一致**:
   - 有些在OnRender前裁剪
   - 有些在OnRender后裁剪
   - 有些完全不裁剪

3. **缺乏裁剪策略**:
   - 不知道哪些控件应该自动裁剪
   - 不知道应该裁剪到哪个区域
   - 不知道何时需要裁剪

### 问题4: 性能问题

**症状**:
- 绘制了不可见的元素
- 裁剪检查不充分

**问题点**:
```cpp
// RenderContext.cpp:110-127
bool RenderContext::IsClipped(const ui::Rect& rect) const {
    if (!currentClip_.enabled) {
        return false;
    }
    // 仅检查是否完全在裁剪区外，但不阻止绘制调用
    // ...
}
```

**影响**:
- 浪费GPU资源绘制被裁剪的内容
- 深层元素树性能差

---

## 新系统设计

### 设计原则

1. **自动化优先**: 容器控件应该自动裁剪子元素，减少手动调用
2. **策略驱动**: 通过裁剪策略定义控件的裁剪行为
3. **性能导向**: 尽早剔除不可见元素，减少绘制调用
4. **向后兼容**: 保留ClipProperty用于显式裁剪
5. **分层清晰**: 逻辑裁剪、渲染裁剪、硬件裁剪分层处理

### 核心概念

#### 1. 裁剪策略 (ClippingStrategy)

定义控件如何进行裁剪：

```cpp
enum class ClippingStrategy {
    None,               // 不裁剪（默认）
    ToBounds,           // 裁剪到自身边界
    ToContentArea,      // 裁剪到内容区域（考虑Padding）
    ToViewport,         // 裁剪到视口（用于ScrollViewer）
    Custom              // 自定义裁剪（使用ClipProperty）
};
```

#### 2. 裁剪上下文 (ClippingContext)

扩展RenderContext，提供更丰富的裁剪管理：

```cpp
class ClippingContext {
public:
    // 推入裁剪区域（自动与父裁剪求交集）
    void PushClip(const ui::Rect& clipRect, ClippingStrategy strategy);
    
    // 弹出裁剪区域
    void PopClip();
    
    // 获取当前有效裁剪区域
    ui::Rect GetEffectiveClipRect() const;
    
    // 判断元素是否被完全裁剪（优化用）
    bool IsCompletelyClipped(const ui::Rect& bounds) const;
    
    // 判断元素是否部分可见
    bool IsPartiallyVisible(const ui::Rect& bounds) const;
    
    // 获取可见区域（用于优化绘制）
    ui::Rect GetVisibleRect(const ui::Rect& bounds) const;

private:
    struct ClipState {
        ui::Rect clipRect;
        ClippingStrategy strategy;
        bool enabled;
        int depth;  // 裁剪深度（用于调试）
    };
    
    std::stack<ClipState> clipStack_;
    ClipState currentClip_;
};
```

#### 3. 裁剪能力接口 (IClippable)

为需要裁剪的控件提供统一接口：

```cpp
class IClippable {
public:
    virtual ~IClippable() = default;
    
    // 获取裁剪策略
    virtual ClippingStrategy GetClippingStrategy() const = 0;
    
    // 计算裁剪区域（局部坐标）
    virtual ui::Rect CalculateClipRect() const = 0;
    
    // 是否应该裁剪子元素
    virtual bool ShouldClipChildren() const = 0;
};
```

### 新架构设计

```
┌─────────────────────────────────────────────────────────┐
│  UI层 - 控件裁剪策略                                      │
│  - UIElement: ClipProperty (显式裁剪)                     │
│  - Border: ToContentArea (裁剪到Padding内)                │
│  - ScrollViewer: ToViewport (裁剪到视口)                  │
│  - Panel: 可选 ToBounds                                   │
│  - Control: 继承父类策略                                  │
└─────────────────────────────────────────────────────────┘
                         ↓
┌─────────────────────────────────────────────────────────┐
│  逻辑层 - ClippingContext                                 │
│  - 管理裁剪栈                                            │
│  - 自动计算交集                                          │
│  - 提前剔除不可见元素                                     │
│  - 生成裁剪命令                                          │
└─────────────────────────────────────────────────────────┘
                         ↓
┌─────────────────────────────────────────────────────────┐
│  渲染层 - RenderContext                                   │
│  - 维护变换栈、裁剪栈、图层栈                              │
│  - 生成RenderCommand                                      │
└─────────────────────────────────────────────────────────┘
                         ↓
┌─────────────────────────────────────────────────────────┐
│  命令层 - RenderCommand                                   │
│  - SetClip命令                                           │
│  - 携带裁剪区域信息                                       │
└─────────────────────────────────────────────────────────┘
                         ↓
┌─────────────────────────────────────────────────────────┐
│  硬件层 - GlRenderer                                      │
│  - glEnable(GL_SCISSOR_TEST)                             │
│  - glScissor(x, y, width, height)                        │
└─────────────────────────────────────────────────────────┘
```

### 裁剪流程重新设计

#### 阶段1: 元素准备阶段

```cpp
void UIElement::CollectDrawCommands(RenderContext& context) {
    // 1. 可见性检查
    if (GetVisibility() != Visibility::Visible) {
        return;
    }
    
    // 2. 推入变换
    context.PushTransform(layoutRect_.x, layoutRect_.y);
    
    // 3. **新增**: 确定裁剪策略
    ClippingStrategy strategy = DetermineClippingStrategy();
    bool shouldClip = (strategy != ClippingStrategy::None);
    
    // 4. **新增**: 计算裁剪区域
    ui::Rect clipRect;
    if (shouldClip) {
        clipRect = CalculateClipRectForStrategy(strategy);
        
        // **新增**: 提前剔除
        if (context.IsCompletelyClipped(clipRect)) {
            context.PopTransform();
            return; // 完全被裁剪，跳过绘制
        }
        
        context.PushClip(clipRect, strategy);
    }
    
    // 5. 应用不透明度
    bool hasOpacity = (GetOpacity() < 1.0f);
    if (hasOpacity) {
        context.PushLayer(GetOpacity());
    }
    
    // 6. 绘制自身
    OnRender(context);
    
    // 7. 递归子元素
    Visual::CollectDrawCommands(context);
    
    // 8. 恢复状态（LIFO顺序）
    if (hasOpacity) {
        context.PopLayer();
    }
    if (shouldClip) {
        context.PopClip();
    }
    context.PopTransform();
}
```

#### 阶段2: 策略确定

```cpp
ClippingStrategy UIElement::DetermineClippingStrategy() const {
    // 1. 优先使用显式设置的ClipProperty
    if (HasClip()) {
        return ClippingStrategy::Custom;
    }
    
    // 2. 检查控件特定策略
    if (auto* clippable = dynamic_cast<const IClippable*>(this)) {
        return clippable->GetClippingStrategy();
    }
    
    // 3. 默认不裁剪
    return ClippingStrategy::None;
}
```

#### 阶段3: 裁剪区域计算

```cpp
ui::Rect UIElement::CalculateClipRectForStrategy(ClippingStrategy strategy) const {
    switch (strategy) {
        case ClippingStrategy::Custom:
            return GetClip(); // 使用ClipProperty
            
        case ClippingStrategy::ToBounds:
            return ui::Rect{0, 0, GetRenderSize().width, GetRenderSize().height};
            
        case ClippingStrategy::ToContentArea:
            // 由派生类实现（如Border）
            if (auto* clippable = dynamic_cast<const IClippable*>(this)) {
                return clippable->CalculateClipRect();
            }
            return ui::Rect{0, 0, GetRenderSize().width, GetRenderSize().height};
            
        case ClippingStrategy::ToViewport:
            // 由派生类实现（如ScrollViewer）
            if (auto* clippable = dynamic_cast<const IClippable*>(this)) {
                return clippable->CalculateClipRect();
            }
            return ui::Rect{0, 0, GetRenderSize().width, GetRenderSize().height};
            
        default:
            return ui::Rect{0, 0, 0, 0};
    }
}
```

### 控件特定实现

#### Border控件裁剪

```cpp
class Border : public FrameworkElement<Border>, public IClippable {
public:
    // IClippable接口实现
    ClippingStrategy GetClippingStrategy() const override {
        // Border应该裁剪子元素到Padding内部
        return ClippingStrategy::ToContentArea;
    }
    
    ui::Rect CalculateClipRect() const override {
        auto borderThickness = GetBorderThickness();
        auto padding = GetPadding();
        auto size = GetRenderSize();
        
        // 计算内容区域
        float left = borderThickness.left + padding.left;
        float top = borderThickness.top + padding.top;
        float right = borderThickness.right + padding.right;
        float bottom = borderThickness.bottom + padding.bottom;
        
        return ui::Rect{
            left,
            top,
            size.width - left - right,
            size.height - top - bottom
        };
    }
    
    bool ShouldClipChildren() const override {
        return true;
    }
};
```

#### ScrollViewer控件裁剪

```cpp
class ScrollViewer : public ContentControl<ScrollViewer>, public IClippable {
public:
    // IClippable接口实现
    ClippingStrategy GetClippingStrategy() const override {
        // ScrollViewer必须裁剪到视口
        return ClippingStrategy::ToViewport;
    }
    
    ui::Rect CalculateClipRect() const override {
        // 计算视口区域（排除滚动条）
        Rect viewportRect = CalculateViewportRect();
        return viewportRect;
    }
    
    bool ShouldClipChildren() const override {
        return true;
    }

private:
    Rect CalculateViewportRect() const {
        float width = viewportWidth_;
        float height = viewportHeight_;
        
        // 减去滚动条占用的空间
        if (verticalScrollBar_ && 
            verticalScrollBar_->GetVisibility() == Visibility::Visible) {
            width -= 20; // 滚动条宽度
        }
        
        if (horizontalScrollBar_ && 
            horizontalScrollBar_->GetVisibility() == Visibility::Visible) {
            height -= 20; // 滚动条高度
        }
        
        return Rect{0, 0, width, height};
    }
};
```

#### Panel容器裁剪（可选）

```cpp
template<typename Derived>
class Panel : public FrameworkElement<Derived> {
public:
    // 添加裁剪控制属性
    static const binding::DependencyProperty& ClipToBoundsProperty() {
        static auto& property = binding::DependencyProperty::Register(
            "ClipToBounds",
            typeid(bool),
            typeid(Panel<Derived>),
            binding::PropertyMetadata{false}  // 默认不裁剪
        );
        return property;
    }
    
    bool GetClipToBounds() const { 
        return this->template GetValue<bool>(ClipToBoundsProperty()); 
    }
    
    void SetClipToBounds(bool value) { 
        this->SetValue(ClipToBoundsProperty(), value); 
    }
    
    Derived* ClipToBounds(bool value) {
        SetClipToBounds(value);
        return static_cast<Derived*>(this);
    }
    
protected:
    ClippingStrategy DetermineClippingStrategy() const {
        if (GetClipToBounds()) {
            return ClippingStrategy::ToBounds;
        }
        return ClippingStrategy::None;
    }
};
```

### 性能优化

#### 1. 提前剔除

```cpp
bool RenderContext::IsCompletelyClipped(const ui::Rect& rect) const {
    if (!currentClip_.enabled) {
        return false;
    }
    
    // 变换到全局坐标
    ui::Rect globalRect = TransformRect(rect);
    
    // 检查是否完全在裁剪区域外
    if (globalRect.x + globalRect.width <= currentClip_.clipRect.x ||
        globalRect.x >= currentClip_.clipRect.x + currentClip_.clipRect.width ||
        globalRect.y + globalRect.height <= currentClip_.clipRect.y ||
        globalRect.y >= currentClip_.clipRect.y + currentClip_.clipRect.height) {
        return true;
    }
    
    return false;
}
```

#### 2. 可见性检查

```cpp
bool RenderContext::IsPartiallyVisible(const ui::Rect& rect) const {
    return !IsCompletelyClipped(rect);
}

ui::Rect RenderContext::GetVisibleRect(const ui::Rect& rect) const {
    if (!currentClip_.enabled) {
        return rect;
    }
    
    ui::Rect globalRect = TransformRect(rect);
    
    // 计算可见区域（交集）
    float x1 = std::max(currentClip_.clipRect.x, globalRect.x);
    float y1 = std::max(currentClip_.clipRect.y, globalRect.y);
    float x2 = std::min(
        currentClip_.clipRect.x + currentClip_.clipRect.width,
        globalRect.x + globalRect.width
    );
    float y2 = std::min(
        currentClip_.clipRect.y + currentClip_.clipRect.height,
        globalRect.y + globalRect.height
    );
    
    return ui::Rect{x1, y1, std::max(0.0f, x2 - x1), std::max(0.0f, y2 - y1)};
}
```

#### 3. 跳过完全不可见的子树

```cpp
void Visual::CollectDrawCommands(render::RenderContext& context) {
    for (auto* child : visualChildren_) {
        if (!child) continue;
        
        // **新增**: 检查子元素边界
        if (auto* uiChild = dynamic_cast<UIElement*>(child)) {
            auto childBounds = uiChild->GetLayoutRect();
            
            // 如果子元素完全不可见，跳过整个子树
            if (context.IsCompletelyClipped(childBounds)) {
                continue;
            }
        }
        
        child->CollectDrawCommands(context);
    }
}
```

### 调试与诊断

#### 裁剪调试模式

```cpp
class RenderContext {
public:
    // 启用裁剪调试模式
    void SetClippingDebugMode(bool enabled) {
        clippingDebugMode_ = enabled;
    }
    
    // 获取裁剪统计信息
    struct ClippingStats {
        int totalElements;           // 总元素数
        int clippedElements;         // 被裁剪的元素数
        int partiallyVisible;        // 部分可见的元素数
        int maxClipDepth;            // 最大裁剪深度
        std::vector<std::string> clipStack;  // 裁剪栈信息
    };
    
    ClippingStats GetClippingStats() const;
    
private:
    bool clippingDebugMode_{false};
    ClippingStats stats_;
};
```

#### 可视化裁剪区域

```cpp
void RenderContext::DrawClipDebugOverlay() {
    if (!clippingDebugMode_) return;
    
    // 绘制当前裁剪区域的边界框（红色虚线）
    DrawRectangle(
        currentClip_.clipRect,
        {1.0f, 0.0f, 0.0f, 0.3f},  // 半透明红色填充
        {1.0f, 0.0f, 0.0f, 1.0f},  // 红色边框
        2.0f  // 边框宽度
    );
}
```

### 高级裁剪几何体支持

#### 设计目标

当前基础设计主要关注**矩形裁剪**以解决最常见的容器边界管理问题。但完整的UI框架还需要支持更复杂的裁剪几何体：

1. **圆角矩形裁剪** - Border的CornerRadius属性
2. **椭圆/圆形裁剪** - 圆形头像、圆形按钮等
3. **多边形裁剪** - 不规则形状的容器
4. **路径裁剪** - 自定义形状（SVG路径、贝塞尔曲线等）
5. **变换裁剪** - 支持旋转、缩放、倾斜等变换后的裁剪

#### 裁剪几何体类型系统

```cpp
/**
 * @brief 裁剪几何体类型
 */
enum class ClipGeometryType {
    Rectangle,          // 矩形（默认，最快）
    RoundedRectangle,   // 圆角矩形
    Ellipse,            // 椭圆
    Polygon,            // 多边形
    Path                // 自定义路径
};

/**
 * @brief 裁剪几何体基类
 */
class ClipGeometry {
public:
    virtual ~ClipGeometry() = default;
    
    virtual ClipGeometryType GetType() const = 0;
    
    // 获取边界矩形（用于快速剔除）
    virtual ui::Rect GetBounds() const = 0;
    
    // 判断点是否在裁剪区域内
    virtual bool Contains(const ui::Point& point) const = 0;
    
    // 应用变换
    virtual std::unique_ptr<ClipGeometry> Transform(const Matrix3x2& matrix) const = 0;
    
    // 与另一个几何体求交集
    virtual std::unique_ptr<ClipGeometry> Intersect(const ClipGeometry* other) const = 0;
};

/**
 * @brief 矩形裁剪几何体（硬件加速）
 */
class RectangleClipGeometry : public ClipGeometry {
public:
    explicit RectangleClipGeometry(const ui::Rect& rect) : rect_(rect) {}
    
    ClipGeometryType GetType() const override { return ClipGeometryType::Rectangle; }
    ui::Rect GetBounds() const override { return rect_; }
    bool Contains(const ui::Point& point) const override;
    
    std::unique_ptr<ClipGeometry> Transform(const Matrix3x2& matrix) const override;
    std::unique_ptr<ClipGeometry> Intersect(const ClipGeometry* other) const override;
    
    const ui::Rect& GetRect() const { return rect_; }

private:
    ui::Rect rect_;
};

/**
 * @brief 圆角矩形裁剪几何体
 */
class RoundedRectangleClipGeometry : public ClipGeometry {
public:
    RoundedRectangleClipGeometry(const ui::Rect& rect, const ui::CornerRadius& radius)
        : rect_(rect), cornerRadius_(radius) {}
    
    ClipGeometryType GetType() const override { return ClipGeometryType::RoundedRectangle; }
    ui::Rect GetBounds() const override { return rect_; }
    bool Contains(const ui::Point& point) const override;
    
    std::unique_ptr<ClipGeometry> Transform(const Matrix3x2& matrix) const override;
    std::unique_ptr<ClipGeometry> Intersect(const ClipGeometry* other) const override;
    
    const ui::Rect& GetRect() const { return rect_; }
    const ui::CornerRadius& GetCornerRadius() const { return cornerRadius_; }

private:
    ui::Rect rect_;
    ui::CornerRadius cornerRadius_;
};

/**
 * @brief 椭圆裁剪几何体
 */
class EllipseClipGeometry : public ClipGeometry {
public:
    EllipseClipGeometry(const ui::Point& center, float radiusX, float radiusY)
        : center_(center), radiusX_(radiusX), radiusY_(radiusY) {}
    
    ClipGeometryType GetType() const override { return ClipGeometryType::Ellipse; }
    ui::Rect GetBounds() const override;
    bool Contains(const ui::Point& point) const override;
    
    std::unique_ptr<ClipGeometry> Transform(const Matrix3x2& matrix) const override;
    std::unique_ptr<ClipGeometry> Intersect(const ClipGeometry* other) const override;

private:
    ui::Point center_;
    float radiusX_;
    float radiusY_;
};

/**
 * @brief 多边形裁剪几何体
 */
class PolygonClipGeometry : public ClipGeometry {
public:
    explicit PolygonClipGeometry(const std::vector<ui::Point>& points)
        : points_(points) {}
    
    ClipGeometryType GetType() const override { return ClipGeometryType::Polygon; }
    ui::Rect GetBounds() const override;
    bool Contains(const ui::Point& point) const override;
    
    std::unique_ptr<ClipGeometry> Transform(const Matrix3x2& matrix) const override;
    std::unique_ptr<ClipGeometry> Intersect(const ClipGeometry* other) const override;
    
    const std::vector<ui::Point>& GetPoints() const { return points_; }

private:
    std::vector<ui::Point> points_;
};

/**
 * @brief 路径裁剪几何体
 */
class PathClipGeometry : public ClipGeometry {
public:
    explicit PathClipGeometry(const PathGeometry& path)
        : path_(path) {}
    
    ClipGeometryType GetType() const override { return ClipGeometryType::Path; }
    ui::Rect GetBounds() const override;
    bool Contains(const ui::Point& point) const override;
    
    std::unique_ptr<ClipGeometry> Transform(const Matrix3x2& matrix) const override;
    std::unique_ptr<ClipGeometry> Intersect(const ClipGeometry* other) const override;
    
    const PathGeometry& GetPath() const { return path_; }

private:
    PathGeometry path_;
};
```

#### 几何体操作算法

裁剪几何体之间的交集、并集等布尔运算是裁剪系统的核心。以下详细说明不同几何体类型之间的操作算法。

##### 操作类型定义

```cpp
/**
 * @brief 几何体布尔操作类型
 */
enum class GeometryOperation {
    Intersect,    // 交集（裁剪系统主要使用）
    Union,        // 并集（用于复合裁剪区域）
    Subtract,     // 差集（挖空效果）
    Xor           // 异或（用于特殊效果）
};

/**
 * @brief 几何体操作接口扩展
 */
class ClipGeometry {
public:
    // 交集（裁剪系统核心操作）
    virtual std::unique_ptr<ClipGeometry> Intersect(const ClipGeometry* other) const = 0;
    
    // 并集（可选，用于复合区域）
    virtual std::unique_ptr<ClipGeometry> Union(const ClipGeometry* other) const = 0;
    
    // 差集（可选，用于挖空）
    virtual std::unique_ptr<ClipGeometry> Subtract(const ClipGeometry* other) const = 0;
    
    // 通用布尔操作
    virtual std::unique_ptr<ClipGeometry> BooleanOp(
        const ClipGeometry* other,
        GeometryOperation op
    ) const = 0;
};
```

##### 交集算法实现策略

**1. 矩形 ∩ 矩形**（最简单，最快）

```cpp
std::unique_ptr<ClipGeometry> RectangleClipGeometry::Intersect(
    const ClipGeometry* other) const 
{
    if (other->GetType() == ClipGeometryType::Rectangle) {
        auto* otherRect = static_cast<const RectangleClipGeometry*>(other);
        
        // 直接计算矩形交集
        float x1 = std::max(rect_.x, otherRect->rect_.x);
        float y1 = std::max(rect_.y, otherRect->rect_.y);
        float x2 = std::min(rect_.x + rect_.width, 
                           otherRect->rect_.x + otherRect->rect_.width);
        float y2 = std::min(rect_.y + rect_.height,
                           otherRect->rect_.y + otherRect->rect_.height);
        
        if (x2 <= x1 || y2 <= y1) {
            // 无交集，返回空矩形
            return std::make_unique<RectangleClipGeometry>(
                ui::Rect{0, 0, 0, 0}
            );
        }
        
        return std::make_unique<RectangleClipGeometry>(
            ui::Rect{x1, y1, x2 - x1, y2 - y1}
        );
    }
    
    // 矩形与其他类型：降级到多边形操作
    return IntersectAsPolygon(other);
}
```

**2. 圆角矩形 ∩ 矩形**（保持圆角矩形）

```cpp
std::unique_ptr<ClipGeometry> RoundedRectangleClipGeometry::Intersect(
    const ClipGeometry* other) const 
{
    if (other->GetType() == ClipGeometryType::Rectangle) {
        auto* otherRect = static_cast<const RectangleClipGeometry*>(other);
        
        // 先计算边界矩形的交集
        auto bounds = GetBounds();
        float x1 = std::max(bounds.x, otherRect->GetRect().x);
        float y1 = std::max(bounds.y, otherRect->GetRect().y);
        float x2 = std::min(bounds.x + bounds.width,
                           otherRect->GetRect().x + otherRect->GetRect().width);
        float y2 = std::min(bounds.y + bounds.height,
                           otherRect->GetRect().y + otherRect->GetRect().height);
        
        if (x2 <= x1 || y2 <= y1) {
            return std::make_unique<RectangleClipGeometry>(ui::Rect{0, 0, 0, 0});
        }
        
        ui::Rect intersectRect{x1, y1, x2 - x1, y2 - y1};
        
        // 调整圆角：如果交集裁剪掉了某些角，需要相应调整圆角半径
        ui::CornerRadius adjustedRadius = AdjustCornerRadiusForIntersection(
            intersectRect, rect_, cornerRadius_
        );
        
        return std::make_unique<RoundedRectangleClipGeometry>(
            intersectRect, adjustedRadius
        );
    }
    
    if (other->GetType() == ClipGeometryType::RoundedRectangle) {
        // 两个圆角矩形的交集：降级到路径操作
        return IntersectAsPath(other);
    }
    
    // 与其他类型：路径操作
    return IntersectAsPath(other);
}
```

**3. 椭圆 ∩ 矩形**（转换为多边形近似）

```cpp
std::unique_ptr<ClipGeometry> EllipseClipGeometry::Intersect(
    const ClipGeometry* other) const 
{
    if (other->GetType() == ClipGeometryType::Rectangle) {
        auto* otherRect = static_cast<const RectangleClipGeometry*>(other);
        
        // 快速边界检查
        auto bounds = GetBounds();
        if (!bounds.Intersects(otherRect->GetRect())) {
            return std::make_unique<RectangleClipGeometry>(ui::Rect{0, 0, 0, 0});
        }
        
        // 椭圆与矩形的交集：
        // 方案1：如果矩形完全包含椭圆，返回椭圆本身
        if (otherRect->GetRect().Contains(bounds)) {
            return std::make_unique<EllipseClipGeometry>(
                center_, radiusX_, radiusY_
            );
        }
        
        // 方案2：转换为多边形近似（64边）
        auto polygonPoints = EllipseToPolygon(center_, radiusX_, radiusY_, 64);
        auto ellipsePolygon = std::make_unique<PolygonClipGeometry>(polygonPoints);
        return ellipsePolygon->Intersect(other);
    }
    
    // 椭圆与其他类型：多边形近似
    return IntersectAsPolygon(other);
}

std::vector<ui::Point> EllipseToPolygon(
    const ui::Point& center, float radiusX, float radiusY, int segments) 
{
    std::vector<ui::Point> points;
    points.reserve(segments);
    
    for (int i = 0; i < segments; ++i) {
        float angle = (2.0f * M_PI * i) / segments;
        points.push_back(ui::Point{
            center.x + radiusX * std::cos(angle),
            center.y + radiusY * std::sin(angle)
        });
    }
    
    return points;
}
```

**4. 多边形 ∩ 多边形**（Sutherland-Hodgman算法）

```cpp
std::unique_ptr<ClipGeometry> PolygonClipGeometry::Intersect(
    const ClipGeometry* other) const 
{
    if (other->GetType() == ClipGeometryType::Rectangle) {
        // 使用Sutherland-Hodgman算法裁剪多边形到矩形
        auto* otherRect = static_cast<const RectangleClipGeometry*>(other);
        auto clippedPoints = SutherlandHodgmanClip(points_, otherRect->GetRect());
        
        if (clippedPoints.empty()) {
            return std::make_unique<RectangleClipGeometry>(ui::Rect{0, 0, 0, 0});
        }
        
        return std::make_unique<PolygonClipGeometry>(clippedPoints);
    }
    
    if (other->GetType() == ClipGeometryType::Polygon) {
        // 使用Weiler-Atherton算法或Clipper2库
        auto* otherPoly = static_cast<const PolygonClipGeometry*>(other);
        auto result = PolygonIntersection(points_, otherPoly->GetPoints());
        
        return std::make_unique<PolygonClipGeometry>(result);
    }
    
    // 与其他类型：先转换为多边形
    auto otherPolygon = other->ToPolygon();
    return Intersect(otherPolygon.get());
}

// Sutherland-Hodgman算法实现（多边形裁剪到矩形）
std::vector<ui::Point> SutherlandHodgmanClip(
    const std::vector<ui::Point>& polygon,
    const ui::Rect& clipRect)
{
    auto output = polygon;
    
    // 四条边依次裁剪：左、右、上、下
    struct Edge {
        float value;
        bool (*inside)(const ui::Point&, float);
        ui::Point (*intersect)(const ui::Point&, const ui::Point&, float);
    };
    
    Edge edges[] = {
        {clipRect.x, [](const ui::Point& p, float v) { return p.x >= v; },
         [](const ui::Point& p1, const ui::Point& p2, float x) {
             float t = (x - p1.x) / (p2.x - p1.x);
             return ui::Point{x, p1.y + t * (p2.y - p1.y)};
         }},
        {clipRect.x + clipRect.width, [](const ui::Point& p, float v) { return p.x <= v; },
         [](const ui::Point& p1, const ui::Point& p2, float x) {
             float t = (x - p1.x) / (p2.x - p1.x);
             return ui::Point{x, p1.y + t * (p2.y - p1.y)};
         }},
        {clipRect.y, [](const ui::Point& p, float v) { return p.y >= v; },
         [](const ui::Point& p1, const ui::Point& p2, float y) {
             float t = (y - p1.y) / (p2.y - p1.y);
             return ui::Point{p1.x + t * (p2.x - p1.x), y};
         }},
        {clipRect.y + clipRect.height, [](const ui::Point& p, float v) { return p.y <= v; },
         [](const ui::Point& p1, const ui::Point& p2, float y) {
             float t = (y - p1.y) / (p2.y - p1.y);
             return ui::Point{p1.x + t * (p2.x - p1.x), y};
         }}
    };
    
    for (const auto& edge : edges) {
        if (output.empty()) break;
        
        std::vector<ui::Point> input = output;
        output.clear();
        
        for (size_t i = 0; i < input.size(); ++i) {
            const auto& current = input[i];
            const auto& next = input[(i + 1) % input.size()];
            
            bool currentInside = edge.inside(current, edge.value);
            bool nextInside = edge.inside(next, edge.value);
            
            if (currentInside) {
                output.push_back(current);
                if (!nextInside) {
                    // 离开裁剪区域：添加交点
                    output.push_back(edge.intersect(current, next, edge.value));
                }
            } else if (nextInside) {
                // 进入裁剪区域：添加交点
                output.push_back(edge.intersect(current, next, edge.value));
            }
        }
    }
    
    return output;
}
```

**5. 路径 ∩ 路径**（使用第三方库）

```cpp
std::unique_ptr<ClipGeometry> PathClipGeometry::Intersect(
    const ClipGeometry* other) const 
{
    // 路径操作复杂，推荐使用成熟的几何库
    // 选项1：Clipper2（C++，高性能）
    // 选项2：CGAL（强大但较重）
    // 选项3：自定义简化实现（仅支持简单情况）
    
    if (other->GetType() == ClipGeometryType::Rectangle) {
        // 路径裁剪到矩形：使用Clipper2
        return ClipPathToRect(path_, 
            static_cast<const RectangleClipGeometry*>(other)->GetRect());
    }
    
    // 通用路径操作：使用Clipper2库
    auto otherPath = other->ToPath();
    return ClipperIntersect(path_, otherPath->GetPath());
}

// 使用Clipper2库的示例
std::unique_ptr<ClipGeometry> ClipperIntersect(
    const PathGeometry& path1,
    const PathGeometry& path2)
{
    // 转换为Clipper2格式
    Clipper2Lib::PathsD subject = ToClipperPaths(path1);
    Clipper2Lib::PathsD clip = ToClipperPaths(path2);
    
    // 执行交集操作
    Clipper2Lib::PathsD solution = Clipper2Lib::Intersect(
        subject, clip, Clipper2Lib::FillRule::NonZero
    );
    
    // 转换回PathGeometry
    if (solution.empty()) {
        return std::make_unique<RectangleClipGeometry>(ui::Rect{0, 0, 0, 0});
    }
    
    PathGeometry resultPath = FromClipperPaths(solution);
    return std::make_unique<PathClipGeometry>(resultPath);
}
```

##### 并集算法（可选功能）

并集主要用于创建复合裁剪区域，例如"裁剪到区域A或区域B"：

```cpp
std::unique_ptr<ClipGeometry> ClipGeometry::Union(
    const ClipGeometry* other) const 
{
    // 矩形 ∪ 矩形：可能返回矩形或多边形
    if (GetType() == ClipGeometryType::Rectangle && 
        other->GetType() == ClipGeometryType::Rectangle) {
        
        auto* rect1 = static_cast<const RectangleClipGeometry*>(this);
        auto* rect2 = static_cast<const RectangleClipGeometry*>(other);
        
        // 检查是否可以合并为单个矩形
        if (CanMergeRects(rect1->GetRect(), rect2->GetRect())) {
            return std::make_unique<RectangleClipGeometry>(
                MergeRects(rect1->GetRect(), rect2->GetRect())
            );
        }
        
        // 否则创建复合几何体
        return std::make_unique<CompoundClipGeometry>(
            GeometryOperation::Union,
            std::make_unique<RectangleClipGeometry>(rect1->GetRect()),
            std::make_unique<RectangleClipGeometry>(rect2->GetRect())
        );
    }
    
    // 通用情况：使用Clipper2或转换为多边形
    return PolygonUnion(ToPolygon(), other->ToPolygon());
}
```

##### 性能优化策略

**1. 分层处理**

```cpp
// 优先级：简单类型 > 复杂类型
enum class GeometryComplexity {
    Simple,      // 矩形（直接计算）
    Moderate,    // 圆角矩形、椭圆（近似或简化）
    Complex      // 多边形、路径（完整算法）
};

std::unique_ptr<ClipGeometry> OptimizedIntersect(
    const ClipGeometry* a, const ClipGeometry* b) 
{
    auto complexityA = GetComplexity(a->GetType());
    auto complexityB = GetComplexity(b->GetType());
    
    // 优先使用简单算法
    if (complexityA == GeometryComplexity::Simple && 
        complexityB == GeometryComplexity::Simple) {
        return FastRectIntersect(a, b);
    }
    
    // 中等复杂度：边界快速检查
    if (!a->GetBounds().Intersects(b->GetBounds())) {
        return std::make_unique<RectangleClipGeometry>(ui::Rect{0, 0, 0, 0});
    }
    
    // 完整算法
    return a->Intersect(b);
}
```

**2. 缓存交集结果**

```cpp
class GeometryOperationCache {
public:
    std::unique_ptr<ClipGeometry> GetCachedIntersection(
        const ClipGeometry* a,
        const ClipGeometry* b)
    {
        auto key = MakeCacheKey(a, b);
        auto it = cache_.find(key);
        
        if (it != cache_.end()) {
            return it->second->Clone();
        }
        
        auto result = a->Intersect(b);
        cache_[key] = result->Clone();
        return result;
    }

private:
    struct CacheKey {
        size_t hashA;
        size_t hashB;
        // 比较运算符...
    };
    
    std::unordered_map<CacheKey, std::unique_ptr<ClipGeometry>> cache_;
};
```

**3. 近似处理**

对于不影响视觉效果的情况，使用边界矩形近似：

```cpp
std::unique_ptr<ClipGeometry> ApproximateIntersect(
    const ClipGeometry* a, const ClipGeometry* b,
    float tolerance = 1.0f)  // 1像素误差
{
    // 如果边界矩形足够接近，直接使用边界矩形交集
    auto boundsA = a->GetBounds();
    auto boundsB = b->GetBounds();
    
    if (ShouldApproximate(a, b, tolerance)) {
        return RectIntersect(boundsA, boundsB);
    }
    
    // 否则使用精确算法
    return a->Intersect(b);
}
```

##### 复合几何体（高级）

对于无法简化的复杂操作结果，使用复合几何体：

```cpp
/**
 * @brief 复合裁剪几何体（表示多个几何体的布尔运算）
 */
class CompoundClipGeometry : public ClipGeometry {
public:
    CompoundClipGeometry(
        GeometryOperation op,
        std::unique_ptr<ClipGeometry> left,
        std::unique_ptr<ClipGeometry> right)
        : operation_(op)
        , left_(std::move(left))
        , right_(std::move(right)) {}
    
    ClipGeometryType GetType() const override { 
        return ClipGeometryType::Compound; 
    }
    
    ui::Rect GetBounds() const override {
        auto boundsA = left_->GetBounds();
        auto boundsB = right_->GetBounds();
        
        switch (operation_) {
            case GeometryOperation::Intersect:
                return RectIntersect(boundsA, boundsB);
            case GeometryOperation::Union:
                return RectUnion(boundsA, boundsB);
            default:
                return boundsA;
        }
    }
    
    bool Contains(const ui::Point& point) const override {
        bool inLeft = left_->Contains(point);
        bool inRight = right_->Contains(point);
        
        switch (operation_) {
            case GeometryOperation::Intersect:
                return inLeft && inRight;
            case GeometryOperation::Union:
                return inLeft || inRight;
            case GeometryOperation::Subtract:
                return inLeft && !inRight;
            case GeometryOperation::Xor:
                return inLeft != inRight;
            default:
                return false;
        }
    }

private:
    GeometryOperation operation_;
    std::unique_ptr<ClipGeometry> left_;
    std::unique_ptr<ClipGeometry> right_;
};
```

##### 第三方库集成建议

| 库 | 优势 | 适用场景 |
|----|------|---------|
| **Clipper2** | 快速、轻量、许可友好 | 多边形和路径的布尔运算 |
| **CGAL** | 功能强大、精确 | 需要高精度几何计算 |
| **libtess2** | OpenGL集成、已使用 | 多边形三角化（已在项目中） |
| **Boost.Geometry** | 功能全面 | 复杂几何操作 |

推荐选择：**Clipper2**（高性能，MIT许可，专注于2D布尔运算）

##### 交集操作总结表

| 类型A | 类型B | 算法 | 复杂度 | 结果类型 |
|-------|-------|------|--------|---------|
| 矩形 | 矩形 | 直接计算 | O(1) | 矩形 |
| 矩形 | 圆角矩形 | 边界+圆角调整 | O(1) | 圆角矩形 |
| 矩形 | 椭圆 | 多边形近似 | O(n) | 多边形 |
| 矩形 | 多边形 | Sutherland-Hodgman | O(n) | 多边形 |
| 矩形 | 路径 | Clipper2 | O(n log n) | 路径 |
| 圆角矩形 | 圆角矩形 | 路径操作 | O(n log n) | 路径 |
| 椭圆 | 椭圆 | 多边形近似 | O(n²) | 多边形 |
| 多边形 | 多边形 | Clipper2 | O(n log n) | 多边形 |
| 路径 | 路径 | Clipper2 | O(n log n) | 路径 |

**n**: 顶点/段数量

#### 扩展的裁剪上下文

```cpp
class RenderContext {
public:
    // === 基础矩形裁剪（阶段1实施） ===
    void PushClip(const ui::Rect& clipRect);
    void PopClip();
    
    // === 高级几何体裁剪（阶段2+扩展） ===
    void PushClip(std::unique_ptr<ClipGeometry> geometry);
    
    // 便捷方法
    void PushRoundedRectClip(const ui::Rect& rect, const ui::CornerRadius& radius);
    void PushEllipseClip(const ui::Point& center, float radiusX, float radiusY);
    void PushPolygonClip(const std::vector<ui::Point>& points);
    void PushPathClip(const PathGeometry& path);
    
    // 获取当前裁剪几何体
    const ClipGeometry* GetCurrentClipGeometry() const;
    
    // 快速矩形裁剪检查（向后兼容）
    bool IsCompletelyClipped(const ui::Rect& rect) const;

private:
    struct ClipState {
        std::unique_ptr<ClipGeometry> geometry;
        ClippingStrategy strategy;
        bool enabled;
    };
    
    std::stack<ClipState> clipStack_;
};
```

#### 渲染实现策略

##### 1. 硬件加速裁剪（优先）

```cpp
// GlRenderer.cpp
void GlRenderer::ApplyClip(const ClipPayload& payload) {
    if (!payload.geometry) {
        glDisable(GL_SCISSOR_TEST);
        return;
    }
    
    switch (payload.geometry->GetType()) {
        case ClipGeometryType::Rectangle: {
            // 使用 glScissor（硬件加速，最快）
            auto* rectGeom = static_cast<const RectangleClipGeometry*>(payload.geometry.get());
            auto bounds = rectGeom->GetBounds();
            
            glEnable(GL_SCISSOR_TEST);
            glScissor(bounds.x, bounds.y, bounds.width, bounds.height);
            break;
        }
        
        case ClipGeometryType::RoundedRectangle:
        case ClipGeometryType::Ellipse:
        case ClipGeometryType::Polygon:
        case ClipGeometryType::Path: {
            // 使用模板缓冲区（Stencil Buffer）
            ApplyStencilClip(payload.geometry.get());
            break;
        }
    }
}
```

##### 2. 模板缓冲区裁剪

对于复杂几何体，使用OpenGL的Stencil Buffer：

```cpp
void GlRenderer::ApplyStencilClip(const ClipGeometry* geometry) {
    // 1. 清除模板缓冲区
    glClearStencil(0);
    glClear(GL_STENCIL_BUFFER_BIT);
    
    // 2. 设置模板测试
    glEnable(GL_STENCIL_TEST);
    glStencilFunc(GL_ALWAYS, 1, 0xFF);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    
    // 3. 禁用颜色写入，只写入模板
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    
    // 4. 绘制裁剪几何体到模板缓冲区
    RenderClipGeometryToStencil(geometry);
    
    // 5. 启用颜色写入，配置模板测试
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glStencilFunc(GL_EQUAL, 1, 0xFF);
    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
    
    // 后续绘制将只在模板值为1的区域显示
}

void GlRenderer::RenderClipGeometryToStencil(const ClipGeometry* geometry) {
    switch (geometry->GetType()) {
        case ClipGeometryType::RoundedRectangle: {
            auto* rrGeom = static_cast<const RoundedRectangleClipGeometry*>(geometry);
            // 使用SDF着色器绘制圆角矩形
            DrawRoundedRectToStencil(rrGeom->GetRect(), rrGeom->GetCornerRadius());
            break;
        }
        
        case ClipGeometryType::Ellipse: {
            auto* ellipseGeom = static_cast<const EllipseClipGeometry*>(geometry);
            // 绘制椭圆到模板
            DrawEllipseToStencil(ellipseGeom);
            break;
        }
        
        case ClipGeometryType::Polygon: {
            auto* polyGeom = static_cast<const PolygonClipGeometry*>(geometry);
            // 使用tessellation绘制多边形
            DrawPolygonToStencil(polyGeom->GetPoints());
            break;
        }
        
        case ClipGeometryType::Path: {
            auto* pathGeom = static_cast<const PathClipGeometry*>(geometry);
            // 使用tessellation绘制路径
            DrawPathToStencil(pathGeom->GetPath());
            break;
        }
        
        default:
            break;
    }
}
```

##### 3. 软件裁剪（后备方案）

对于不支持的硬件或特殊情况，提供软件实现：

```cpp
class SoftwareClipper {
public:
    // 软件光栅化裁剪
    static std::vector<ui::Point> ClipPolygon(
        const std::vector<ui::Point>& polygon,
        const ClipGeometry* clipGeometry
    );
    
    // Sutherland-Hodgman算法（多边形裁剪）
    static std::vector<ui::Point> SutherlandHodgmanClip(
        const std::vector<ui::Point>& subject,
        const std::vector<ui::Point>& clipPoly
    );
};
```

#### 变换支持

##### 变换矩阵管理

```cpp
class RenderContext {
public:
    // 推入仿射变换
    void PushTransform(const Matrix3x2& matrix);
    
    // 便捷方法
    void PushTranslation(float x, float y);
    void PushRotation(float angle, const ui::Point& center);
    void PushScale(float scaleX, float scaleY, const ui::Point& center);
    void PushSkew(float angleX, float angleY, const ui::Point& center);
    
    void PopTransform();
    
    // 获取当前累积变换
    Matrix3x2 GetCurrentTransform() const;

private:
    std::stack<Matrix3x2> transformStack_;
    Matrix3x2 currentTransform_;
};
```

##### 变换后的裁剪

当裁剪区域或元素应用变换时：

```cpp
void RenderContext::PushClip(std::unique_ptr<ClipGeometry> geometry) {
    // 1. 保存当前状态
    clipStack_.push(std::move(currentClip_));
    
    // 2. 应用当前变换到裁剪几何体
    auto transformedGeometry = geometry->Transform(currentTransform_);
    
    // 3. 与父裁剪求交集
    if (currentClip_.geometry) {
        transformedGeometry = transformedGeometry->Intersect(currentClip_.geometry.get());
    }
    
    // 4. 更新当前裁剪
    currentClip_.geometry = std::move(transformedGeometry);
    currentClip_.enabled = true;
    
    // 5. 应用到渲染器
    ApplyCurrentClip();
}
```

#### 控件API扩展

##### Border - 圆角裁剪

```cpp
class Border : public FrameworkElement<Border>, public IClippable {
public:
    ClippingStrategy GetClippingStrategy() const override {
        // 如果有圆角，使用圆角矩形裁剪
        auto cornerRadius = GetCornerRadius();
        if (cornerRadius.IsUniform() && cornerRadius.topLeft > 0) {
            return ClippingStrategy::Custom;
        }
        return ClippingStrategy::ToContentArea;
    }
    
    std::unique_ptr<ClipGeometry> CalculateClipGeometry() const override {
        auto contentRect = CalculateContentRect();
        auto cornerRadius = GetCornerRadius();
        
        if (cornerRadius.topLeft > 0 || cornerRadius.topRight > 0 ||
            cornerRadius.bottomRight > 0 || cornerRadius.bottomLeft > 0) {
            // 圆角矩形裁剪
            return std::make_unique<RoundedRectangleClipGeometry>(
                contentRect, cornerRadius
            );
        }
        
        // 普通矩形裁剪
        return std::make_unique<RectangleClipGeometry>(contentRect);
    }
};
```

##### UIElement - 自定义几何体裁剪

```cpp
class UIElement : public Visual {
public:
    // 现有矩形裁剪API（向后兼容）
    void SetClip(const Rect& rect);
    Rect GetClip() const;
    
    // 新增：几何体裁剪API
    void SetClipGeometry(std::unique_ptr<ClipGeometry> geometry);
    const ClipGeometry* GetClipGeometry() const;
    
    // 便捷方法
    void SetRoundedRectClip(const Rect& rect, const CornerRadius& radius);
    void SetEllipseClip(const Point& center, float radiusX, float radiusY);
    void SetPolygonClip(const std::vector<Point>& points);
    void SetPathClip(const PathGeometry& path);
    
private:
    std::unique_ptr<ClipGeometry> clipGeometry_;
};
```

#### 性能优化策略

##### 1. 分层优化

```cpp
// 优先级：硬件裁剪 > 模板裁剪 > 软件裁剪
enum class ClipImplementation {
    Hardware,    // glScissor（仅矩形）
    Stencil,     // 模板缓冲区（复杂几何体）
    Software     // CPU裁剪（后备）
};

ClipImplementation ChooseImplementation(const ClipGeometry* geometry) {
    // 1. 矩形优先使用硬件裁剪
    if (geometry->GetType() == ClipGeometryType::Rectangle) {
        return ClipImplementation::Hardware;
    }
    
    // 2. 检查是否支持模板缓冲区
    if (stencilBufferAvailable_) {
        return ClipImplementation::Stencil;
    }
    
    // 3. 后备到软件裁剪
    return ClipImplementation::Software;
}
```

##### 2. 边界矩形快速剔除

```cpp
bool RenderContext::IsCompletelyClipped(const ui::Rect& bounds) const {
    if (!currentClip_.geometry) {
        return false;
    }
    
    // 第一步：快速边界检查
    auto clipBounds = currentClip_.geometry->GetBounds();
    if (!bounds.Intersects(clipBounds)) {
        return true;  // 边界不相交，完全被裁剪
    }
    
    // 第二步：精确几何检查（仅在必要时）
    // 对于简单几何体可以跳过
    if (currentClip_.geometry->GetType() == ClipGeometryType::Rectangle) {
        return false;  // 已经通过边界检查
    }
    
    // 对于复杂几何体，检查四个角点
    return CheckComplexClipping(bounds, currentClip_.geometry.get());
}
```

##### 3. 缓存和复用

```cpp
class ClipGeometryCache {
public:
    // 缓存变换后的几何体
    const ClipGeometry* GetTransformed(
        const ClipGeometry* original,
        const Matrix3x2& transform
    );
    
    // 缓存交集结果
    const ClipGeometry* GetIntersection(
        const ClipGeometry* a,
        const ClipGeometry* b
    );
    
private:
    std::unordered_map<CacheKey, std::unique_ptr<ClipGeometry>> cache_;
};
```

#### 实施路线图扩展

现有的6周基础实施计划完成后，可以按以下顺序添加高级几何体支持：

**阶段5: 圆角矩形裁剪（第7-8周）**
- [ ] 实现RoundedRectangleClipGeometry
- [ ] Border控件集成圆角裁剪
- [ ] 模板缓冲区渲染实现
- [ ] 测试和性能优化

**阶段6: 椭圆和多边形裁剪（第9-10周）**
- [ ] 实现EllipseClipGeometry
- [ ] 实现PolygonClipGeometry
- [ ] Stencil渲染扩展
- [ ] API文档和示例

**阶段7: 路径裁剪和变换（第11-12周）**
- [ ] 实现PathClipGeometry
- [ ] 完整的变换矩阵支持
- [ ] 变换裁剪交集算法
- [ ] 性能基准和优化

**阶段8: 优化和完善（第13-14周）**
- [ ] 缓存机制
- [ ] 软件后备实现
- [ ] 全面性能测试
- [ ] 完整文档和示例

#### 使用示例

##### 示例1: Border圆角裁剪
```cpp
auto border = std::make_shared<Border>();
border->SetCornerRadius(CornerRadius(10)); // 圆角10px
border->SetChild(content);
// content会自动裁剪到圆角矩形内 ✅
```

##### 示例2: 圆形头像裁剪
```cpp
auto image = std::make_shared<Image>();
image->SetSource("avatar.png");
image->SetWidth(100);
image->SetHeight(100);

// 圆形裁剪
image->SetEllipseClip(Point(50, 50), 50, 50);
```

##### 示例3: 自定义路径裁剪
```cpp
auto element = std::make_shared<UIElement>();

// 星形裁剪
PathGeometry starPath;
starPath.MoveTo(Point(50, 0));
starPath.LineTo(Point(60, 35));
starPath.LineTo(Point(95, 35));
starPath.LineTo(Point(65, 55));
starPath.LineTo(Point(75, 90));
starPath.LineTo(Point(50, 70));
starPath.LineTo(Point(25, 90));
starPath.LineTo(Point(35, 55));
starPath.LineTo(Point(5, 35));
starPath.LineTo(Point(40, 35));
starPath.Close();

element->SetPathClip(starPath);
```

##### 示例4: 旋转裁剪
```cpp
auto container = std::make_shared<Panel>();
container->SetRenderTransform(
    RotateTransform(45, Point(100, 100))
);
container->SetClipToBounds(true);
// 子元素会被裁剪到旋转后的边界 ✅
```

#### 技术挑战和解决方案

| 挑战 | 解决方案 |
|------|---------|
| 复杂几何体交集计算 | 使用成熟的几何库（如Clipper2）或分层近似 |
| 变换后的裁剪性能 | 缓存变换结果，边界矩形快速剔除 |
| 模板缓冲区限制 | 嵌套深度限制（通常8层），超出回退到软件 |
| 抗锯齿边缘 | 使用多重采样或SDF着色器 |
| 跨平台兼容性 | 提供软件后备实现，检测硬件能力 |

#### 向后兼容性

所有高级几何体功能都是**可选的扩展**：
- 基础矩形裁剪保持不变
- 现有API继续工作
- 高级功能通过新API访问
- 自动降级到支持的实现

---

## 实施计划

### 阶段1: 基础架构重构（第1-2周）

#### 任务1.1: 扩展ClippingStrategy枚举
- [ ] 定义ClippingStrategy枚举类型
- [ ] 添加到RenderCommand.h
- [ ] 文档说明各策略用途

#### 任务1.2: 创建IClippable接口
- [ ] 定义IClippable接口
- [ ] 添加到include/fk/ui/IClippable.h
- [ ] 编写接口文档

#### 任务1.3: 增强RenderContext
- [ ] 添加IsCompletelyClipped()方法
- [ ] 添加IsPartiallyVisible()方法
- [ ] 添加GetVisibleRect()方法
- [ ] 修改PushClip()接受ClippingStrategy参数
- [ ] 添加裁剪调试模式

#### 任务1.4: 修改UIElement基类
- [ ] 添加DetermineClippingStrategy()方法
- [ ] 添加CalculateClipRectForStrategy()方法
- [ ] 重构CollectDrawCommands()实现提前剔除
- [ ] 保持向后兼容性

### 阶段2: 控件适配（第3-4周）

#### 任务2.1: 实现Border裁剪
- [ ] Border继承IClippable
- [ ] 实现GetClippingStrategy() → ToContentArea
- [ ] 实现CalculateClipRect()（考虑BorderThickness和Padding）
- [ ] 测试嵌套Border裁剪

#### 任务2.2: 实现ScrollViewer裁剪
- [ ] ScrollViewer继承IClippable
- [ ] 实现GetClippingStrategy() → ToViewport
- [ ] 实现CalculateClipRect()（考虑滚动条）
- [ ] 测试滚动内容裁剪
- [ ] 测试滚动条可见性变化

#### 任务2.3: 实现Panel容器可选裁剪
- [ ] 添加ClipToBoundsProperty
- [ ] 实现DetermineClippingStrategy()
- [ ] Grid、StackPanel继承该机制
- [ ] 测试容器边界裁剪

#### 任务2.4: 适配其他控件
- [ ] TextBox: 裁剪到边界
- [ ] ComboBox: 下拉部分不裁剪
- [ ] ListBox: 裁剪到视口
- [ ] 逐一测试和验证

### 阶段3: 性能优化（第5周）

#### 任务3.1: 实现提前剔除
- [ ] 在CollectDrawCommands中添加剔除逻辑
- [ ] 测量性能提升
- [ ] 优化剔除判断算法

#### 任务3.2: 优化子树遍历
- [ ] 修改Visual::CollectDrawCommands
- [ ] 跳过完全不可见的子树
- [ ] 性能基准测试

#### 任务3.3: 缓存优化
- [ ] 缓存裁剪区域计算结果
- [ ] 裁剪状态变化时失效缓存
- [ ] 测量内存开销

### 阶段4: 测试与文档（第6周）

#### 任务4.1: 单元测试
- [ ] RenderContext裁剪逻辑测试
- [ ] ClippingStrategy测试
- [ ] 交集计算测试
- [ ] 边界情况测试

#### 任务4.2: 集成测试
- [ ] 嵌套容器裁剪测试
- [ ] ScrollViewer滚动测试
- [ ] Border+Padding裁剪测试
- [ ] 复杂布局裁剪测试

#### 任务4.3: 性能测试
- [ ] 深层元素树渲染性能
- [ ] 大量元素剔除效率
- [ ] 内存占用测试

#### 任务4.4: 文档编写
- [ ] API文档更新
- [ ] 使用指南
- [ ] 最佳实践文档
- [ ] 迁移指南（如果需要）

---

## API 设计

### 公共API

#### UIElement

```cpp
class UIElement : public Visual {
public:
    // 现有API（保持不变）
    static const binding::DependencyProperty& ClipProperty();
    void SetClip(const Rect& value);
    Rect GetClip() const;
    bool HasClip() const;

protected:
    // **新增**: 裁剪策略相关
    virtual ClippingStrategy DetermineClippingStrategy() const;
    virtual ui::Rect CalculateClipRectForStrategy(ClippingStrategy strategy) const;
};
```

#### IClippable接口

```cpp
class IClippable {
public:
    virtual ~IClippable() = default;
    
    // 获取裁剪策略
    virtual ClippingStrategy GetClippingStrategy() const = 0;
    
    // 计算裁剪区域（局部坐标）
    virtual ui::Rect CalculateClipRect() const = 0;
    
    // 是否应该裁剪子元素
    virtual bool ShouldClipChildren() const { return true; }
};
```

#### Panel容器

```cpp
template<typename Derived>
class Panel : public FrameworkElement<Derived> {
public:
    // **新增**: ClipToBounds属性
    static const binding::DependencyProperty& ClipToBoundsProperty();
    bool GetClipToBounds() const;
    void SetClipToBounds(bool value);
    
    // 链式调用
    Derived* ClipToBounds(bool value);
};
```

#### RenderContext

```cpp
class RenderContext {
public:
    // 现有API（保持不变）
    void PushClip(const ui::Rect& clipRect);
    void PopClip();
    ui::Rect GetCurrentClip() const;
    bool IsClipped(const ui::Rect& rect) const;
    
    // **新增**: 增强的裁剪API
    void PushClip(const ui::Rect& clipRect, ClippingStrategy strategy);
    bool IsCompletelyClipped(const ui::Rect& rect) const;
    bool IsPartiallyVisible(const ui::Rect& rect) const;
    ui::Rect GetVisibleRect(const ui::Rect& rect) const;
    
    // **新增**: 调试API
    void SetClippingDebugMode(bool enabled);
    ClippingStats GetClippingStats() const;
};
```

### 使用示例

#### 示例1: Border自动裁剪

```cpp
auto border = std::make_shared<Border>();
border->SetBackground(new SolidColorBrush(Colors::LightGray));
border->SetBorderBrush(new SolidColorBrush(Colors::Gray));
border->SetBorderThickness(Thickness(2));
border->SetPadding(Thickness(10));

// Border会自动裁剪子元素到Padding内部
auto textBlock = std::make_shared<TextBlock>();
textBlock->SetText("这段很长的文本会被裁剪到Border的内容区域内...");
border->SetChild(textBlock);
```

#### 示例2: ScrollViewer自动裁剪

```cpp
auto scrollViewer = std::make_shared<ScrollViewer>();
scrollViewer->SetWidth(300);
scrollViewer->SetHeight(200);

// ScrollViewer会自动裁剪内容到视口
auto largeContent = std::make_shared<StackPanel>();
// ... 添加大量内容
scrollViewer->SetContent(largeContent);

// 内容超出视口的部分会被自动裁剪，不会显示
```

#### 示例3: Panel可选裁剪

```cpp
auto stackPanel = std::make_shared<StackPanel>();
stackPanel->SetClipToBounds(true);  // 启用边界裁剪
stackPanel->SetWidth(200);
stackPanel->SetHeight(200);

// 子元素超出200x200的部分会被裁剪
auto largeChild = std::make_shared<Rectangle>();
largeChild->SetWidth(300);  // 超出父容器
largeChild->SetHeight(300);
stackPanel->AddChild(largeChild);
```

#### 示例4: 显式裁剪（保留兼容性）

```cpp
auto element = std::make_shared<UIElement>();
element->SetWidth(200);
element->SetHeight(200);

// 显式设置裁剪区域（圆形裁剪效果）
element->SetClip(Rect(50, 50, 100, 100));

// 只有中间100x100的区域会显示
```

---

## 测试策略

### 单元测试

#### 测试1: 裁剪交集计算

```cpp
TEST(RenderContextTest, ClipIntersection) {
    RenderList renderList;
    RenderContext context(&renderList);
    
    // 推入第一个裁剪区域
    context.PushClip(Rect(0, 0, 100, 100));
    auto clip1 = context.GetCurrentClip();
    EXPECT_EQ(clip1, Rect(0, 0, 100, 100));
    
    // 推入第二个裁剪区域（部分重叠）
    context.PushClip(Rect(50, 50, 100, 100));
    auto clip2 = context.GetCurrentClip();
    EXPECT_EQ(clip2, Rect(50, 50, 50, 50));  // 交集
    
    // 弹出裁剪
    context.PopClip();
    auto clip3 = context.GetCurrentClip();
    EXPECT_EQ(clip3, Rect(0, 0, 100, 100));  // 恢复
}
```

#### 测试2: 完全裁剪检测

```cpp
TEST(RenderContextTest, CompletelyClipped) {
    RenderList renderList;
    RenderContext context(&renderList);
    
    context.PushClip(Rect(0, 0, 100, 100));
    
    // 完全在裁剪区域内
    EXPECT_FALSE(context.IsCompletelyClipped(Rect(10, 10, 50, 50)));
    
    // 完全在裁剪区域外
    EXPECT_TRUE(context.IsCompletelyClipped(Rect(200, 200, 50, 50)));
    
    // 部分重叠
    EXPECT_FALSE(context.IsCompletelyClipped(Rect(80, 80, 50, 50)));
}
```

#### 测试3: 裁剪策略

```cpp
TEST(UIElementTest, ClippingStrategy) {
    auto element = std::make_shared<UIElement>();
    
    // 默认不裁剪
    EXPECT_EQ(element->DetermineClippingStrategy(), ClippingStrategy::None);
    
    // 设置ClipProperty后使用Custom策略
    element->SetClip(Rect(0, 0, 100, 100));
    EXPECT_EQ(element->DetermineClippingStrategy(), ClippingStrategy::Custom);
}
```

### 集成测试

#### 测试4: Border裁剪

```cpp
TEST(BorderTest, ClipsToContentArea) {
    auto border = std::make_shared<Border>();
    border->SetBorderThickness(Thickness(5));
    border->SetPadding(Thickness(10));
    border->Arrange(Rect(0, 0, 200, 200));
    
    // 验证裁剪区域
    EXPECT_EQ(border->GetClippingStrategy(), ClippingStrategy::ToContentArea);
    
    auto clipRect = border->CalculateClipRect();
    EXPECT_EQ(clipRect, Rect(15, 15, 170, 170));  // 5+10边距
}
```

#### 测试5: ScrollViewer裁剪

```cpp
TEST(ScrollViewerTest, ClipsToViewport) {
    auto scrollViewer = std::make_shared<ScrollViewer>();
    scrollViewer->Arrange(Rect(0, 0, 300, 200));
    
    // 验证裁剪区域（考虑滚动条）
    EXPECT_EQ(scrollViewer->GetClippingStrategy(), ClippingStrategy::ToViewport);
    
    // 内容超出视口
    auto largeContent = std::make_shared<StackPanel>();
    largeContent->SetWidth(500);
    largeContent->SetHeight(400);
    scrollViewer->SetContent(largeContent);
    
    // 渲染后验证裁剪生效
    // ...
}
```

### 性能测试

#### 测试6: 深层嵌套性能

```cpp
TEST(PerformanceTest, DeepNesting) {
    // 创建100层嵌套的Border
    auto root = CreateDeeplyNestedBorders(100);
    
    // 测量渲染时间
    auto start = std::chrono::high_resolution_clock::now();
    
    RenderList renderList;
    RenderContext context(&renderList);
    root->CollectDrawCommands(context);
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    // 应该在合理时间内完成（例如<100ms）
    EXPECT_LT(duration.count(), 100);
}
```

#### 测试7: 大量元素剔除

```cpp
TEST(PerformanceTest, MassiveElementCulling) {
    auto scrollViewer = std::make_shared<ScrollViewer>();
    scrollViewer->SetWidth(300);
    scrollViewer->SetHeight(200);
    
    auto container = std::make_shared<StackPanel>();
    
    // 添加1000个元素，但只有少数可见
    for (int i = 0; i < 1000; ++i) {
        auto item = std::make_shared<Rectangle>();
        item->SetWidth(280);
        item->SetHeight(50);
        container->AddChild(item);
    }
    
    scrollViewer->SetContent(container);
    
    // 测量渲染命令数量
    RenderList renderList;
    RenderContext context(&renderList);
    scrollViewer->CollectDrawCommands(context);
    
    // 应该只生成可见元素的命令（约4-5个）
    EXPECT_LT(renderList.GetCommandCount(), 20);
}
```

---

## 风险评估

### 高风险项

#### 风险1: 性能回退
**描述**: 新的裁剪检查可能增加CPU开销  
**影响**: 渲染性能下降  
**缓解措施**:
- 进行详细的性能基准测试
- 使用缓存减少重复计算
- 提供性能分析工具
- 可选的裁剪优化级别

#### 风险2: 向后兼容性
**描述**: 修改UIElement::CollectDrawCommands可能影响现有代码  
**影响**: 现有控件行为改变  
**缓解措施**:
- 保留ClipProperty的现有行为
- 默认策略为None，不影响未适配的控件
- 提供迁移指南
- 充分测试现有示例

### 中风险项

#### 风险3: 复杂度增加
**描述**: 引入新概念（策略、接口）增加学习成本  
**影响**: 开发者难以理解和使用  
**缓解措施**:
- 编写详细文档和示例
- 提供调试工具
- 默认行为尽可能简单

#### 风险4: 边界情况
**描述**: 特殊布局可能出现裁剪错误  
**影响**: UI显示异常  
**缓解措施**:
- 广泛的单元测试
- 集成测试覆盖复杂场景
- 提供可视化调试工具

### 低风险项

#### 风险5: 内存开销
**描述**: 裁剪栈可能增加内存使用  
**影响**: 内存占用略微增加  
**缓解措施**:
- 使用轻量级的ClipState结构
- 限制最大裁剪深度
- 监控内存使用

---

## 附录

### A. 术语表

| 术语 | 定义 |
|------|------|
| **裁剪 (Clipping)** | 限制渲染输出到特定区域，区域外的内容不显示 |
| **裁剪区域 (Clip Region)** | 定义可见内容的矩形边界 |
| **裁剪策略 (Clipping Strategy)** | 定义控件如何确定裁剪区域的规则 |
| **裁剪交集 (Clip Intersection)** | 多个裁剪区域的重叠部分 |
| **提前剔除 (Early Culling)** | 在绘制前判断元素是否可见，跳过不可见元素 |
| **硬件裁剪 (Hardware Clipping)** | 使用GPU功能（如glScissor）进行裁剪 |

### B. 参考资料

1. **WPF裁剪系统**: 
   - [UIElement.Clip Property](https://docs.microsoft.com/en-us/dotnet/api/system.windows.uielement.clip)
   - [ClipToBounds Property](https://docs.microsoft.com/en-us/dotnet/api/system.windows.uielement.cliptobounds)

2. **OpenGL裁剪**:
   - [glScissor Documentation](https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glScissor.xhtml)
   - [GL_SCISSOR_TEST](https://www.khronos.org/opengl/wiki/Scissor_Test)

3. **现有代码**:
   - `include/fk/render/RenderContext.h`
   - `src/render/RenderContext.cpp`
   - `src/ui/UIElement.cpp`
   - `src/render/GlRenderer.cpp`

### C. 变更记录

| 日期 | 版本 | 作者 | 变更说明 |
|------|------|------|----------|
| 2025-11-23 | 1.0 | AI | 初始版本，完整的重构设计 |
| 2025-11-23 | 1.1 | AI | 新增高级裁剪几何体支持章节（圆角矩形、椭圆、多边形、路径裁剪、变换支持） |

---

## 总结

本设计文档提出了一套**完整的、成体系的裁剪系统重构方案**，核心要点包括：

1. **策略驱动**: 通过ClippingStrategy枚举定义控件的裁剪行为
2. **自动化**: 容器控件自动裁剪子元素，无需手动调用
3. **分层清晰**: UI层策略 → 逻辑层管理 → 渲染层执行 → 硬件层实现
4. **性能优化**: 提前剔除不可见元素，减少绘制调用
5. **向后兼容**: 保留ClipProperty，支持显式裁剪

该设计解决了现有系统的所有主要问题：
- ✅ 裁剪边界完整传递（自动交集计算）
- ✅ 子元素裁剪统一管理（递归应用策略）
- ✅ 功能成体系（清晰的架构和接口）
- ✅ ScrollViewer等控件正确裁剪（实现IClippable）

实施该方案预计需要**6周时间**，分为基础架构、控件适配、性能优化、测试文档四个阶段。

---

**文档状态**: ✅ 完成  
**下一步**: 提交审核，开始实施阶段1
