# 裁剪系统重构设计文档

**版本**: 1.0  
**日期**: 2025-11-23  
**状态**: 设计中  
**优先级**: P0（最高）

---

## 📋 目录

1. [背景与动机](#背景与动机)
2. [现有系统分析](#现有系统分析)
3. [问题诊断](#问题诊断)
4. [新系统设计](#新系统设计)
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
