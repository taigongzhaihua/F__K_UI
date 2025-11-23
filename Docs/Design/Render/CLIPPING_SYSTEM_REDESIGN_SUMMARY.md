# 裁剪系统重构 - 快速摘要

> 完整设计文档：[CLIPPING_SYSTEM_REDESIGN.md](./CLIPPING_SYSTEM_REDESIGN.md)

---

## 🎯 核心问题

当前裁剪系统存在四大问题：

1. **裁剪边界传递不完整** - ScrollViewer等控件未实现裁剪
2. **子元素失管** - 深层嵌套的裁剪不正确
3. **功能混乱** - 职责不清，行为不一致
4. **性能不足** - 未提前剔除不可见元素

## 💡 解决方案

### 三大核心概念

#### 1. 裁剪策略 (ClippingStrategy)
```cpp
enum class ClippingStrategy {
    None,              // 不裁剪
    ToBounds,          // 裁剪到自身边界
    ToContentArea,     // 裁剪到内容区域（考虑Padding）
    ToViewport,        // 裁剪到视口（ScrollViewer）
    Custom             // 自定义裁剪（ClipProperty）
};
```

#### 2. 裁剪能力接口 (IClippable)
```cpp
class IClippable {
    virtual ClippingStrategy GetClippingStrategy() const = 0;
    virtual ui::Rect CalculateClipRect() const = 0;
    virtual bool ShouldClipChildren() const = 0;
};
```

#### 3. 自动裁剪机制
- UIElement自动判断裁剪策略
- 容器控件自动裁剪子元素
- 提前剔除不可见元素

### 控件实现示例

#### Border - 自动裁剪到内容区域
```cpp
class Border : public IClippable {
    ClippingStrategy GetClippingStrategy() const override {
        return ClippingStrategy::ToContentArea;
    }
    
    ui::Rect CalculateClipRect() const override {
        // 返回内容区域（排除Border和Padding）
        return CalculateContentRect();
    }
};
```

#### ScrollViewer - 自动裁剪到视口
```cpp
class ScrollViewer : public IClippable {
    ClippingStrategy GetClippingStrategy() const override {
        return ClippingStrategy::ToViewport;
    }
    
    ui::Rect CalculateClipRect() const override {
        // 返回视口区域（排除滚动条）
        return CalculateViewportRect();
    }
};
```

#### Panel - 可选边界裁剪
```cpp
auto panel = std::make_shared<StackPanel>();
panel->SetClipToBounds(true);  // 启用边界裁剪
```

## 🏗️ 新架构

```
┌─────────────────────┐
│   UI层 - 控件策略    │  Border→ToContentArea, ScrollViewer→ToViewport
└─────────────────────┘
          ↓
┌─────────────────────┐
│  逻辑层 - 裁剪管理   │  自动交集计算，提前剔除
└─────────────────────┘
          ↓
┌─────────────────────┐
│  渲染层 - 命令生成   │  生成SetClip命令
└─────────────────────┘
          ↓
┌─────────────────────┐
│  硬件层 - OpenGL     │  glScissor实现
└─────────────────────┘
```

## 🎨 高级裁剪几何体（扩展）

设计支持多种裁剪几何体类型：

### 支持的几何体类型
- ✅ **矩形裁剪** - 基础实现（硬件加速）
- 🔲 **圆角矩形** - Border圆角支持（模板缓冲区）
- 🔲 **椭圆/圆形** - 圆形头像、按钮（模板缓冲区）
- 🔲 **多边形** - 不规则形状（模板缓冲区）
- 🔲 **路径裁剪** - 自定义形状、SVG（模板缓冲区）
- 🔲 **变换裁剪** - 支持旋转、缩放等变换

### 裁剪几何体系统
```cpp
enum class ClipGeometryType {
    Rectangle,          // 矩形（默认，最快）
    RoundedRectangle,   // 圆角矩形
    Ellipse,            // 椭圆
    Polygon,            // 多边形
    Path                // 自定义路径
};

class ClipGeometry {
    virtual ClipGeometryType GetType() const = 0;
    virtual ui::Rect GetBounds() const = 0;
    virtual bool Contains(const ui::Point& point) const = 0;
    virtual std::unique_ptr<ClipGeometry> Transform(const Matrix3x2&) const = 0;
    virtual std::unique_ptr<ClipGeometry> Intersect(const ClipGeometry*) const = 0;
};
```

### 渲染策略
1. **硬件裁剪** - glScissor（仅矩形，最快）
2. **模板缓冲区** - Stencil Buffer（复杂几何体）
3. **软件后备** - CPU裁剪（兼容性）

### 使用示例
```cpp
// 圆角Border自动裁剪
auto border = std::make_shared<Border>();
border->SetCornerRadius(CornerRadius(10));
// 子元素自动裁剪到圆角矩形 ✅

// 圆形头像
auto image = std::make_shared<Image>();
image->SetEllipseClip(Point(50, 50), 50, 50);

// 旋转裁剪
container->SetRenderTransform(RotateTransform(45));
container->SetClipToBounds(true);
// 子元素裁剪到旋转后的边界 ✅
```

## 📋 实施计划

### 基础实施（6周 - 矩形裁剪）

| 阶段 | 时间 | 任务 |
|------|------|------|
| **阶段1** | 第1-2周 | 基础架构重构（枚举、接口、RenderContext增强） |
| **阶段2** | 第3-4周 | 控件适配（Border、ScrollViewer、Panel等） |
| **阶段3** | 第5周 | 性能优化（提前剔除、缓存） |
| **阶段4** | 第6周 | 测试与文档（单元测试、集成测试、文档） |

### 扩展实施（可选，8周 - 高级几何体）

| 阶段 | 时间 | 任务 |
|------|------|------|
| **阶段5** | 第7-8周 | 圆角矩形裁剪（RoundedRectangle、模板缓冲区） |
| **阶段6** | 第9-10周 | 椭圆和多边形裁剪（Ellipse、Polygon） |
| **阶段7** | 第11-12周 | 路径裁剪和变换（Path、Matrix变换） |
| **阶段8** | 第13-14周 | 优化和完善（缓存、软件后备、性能测试） |

## ✨ 主要特性

- ✅ **自动化**：容器控件自动裁剪，无需手动调用
- ✅ **策略驱动**：清晰的裁剪策略定义
- ✅ **性能优化**：提前剔除不可见元素
- ✅ **向后兼容**：保留ClipProperty，不破坏现有API
- ✅ **易调试**：提供调试模式和可视化工具

## 📊 预期效果

### 功能改善
- ScrollViewer正确裁剪内容 ✅
- Border正确裁剪子元素 ✅
- 深层嵌套裁剪正确 ✅

### 性能提升
- 大量元素场景：减少60-80%的绘制调用
- 深层嵌套场景：减少50-70%的CPU开销
- 内存占用：基本持平（<5%增加）

### 代码质量
- 裁剪逻辑统一管理
- 控件职责清晰
- 易于维护和扩展

## 🔧 使用示例

### 示例1：Border自动裁剪
```cpp
auto border = std::make_shared<Border>();
border->SetPadding(Thickness(10));
border->SetChild(textBlock);
// textBlock会自动裁剪到Padding内部
```

### 示例2：ScrollViewer自动裁剪
```cpp
auto scrollViewer = std::make_shared<ScrollViewer>();
scrollViewer->SetContent(largeContent);
// largeContent超出视口的部分自动裁剪
```

### 示例3：Panel可选裁剪
```cpp
auto panel = std::make_shared<StackPanel>();
panel->SetClipToBounds(true);
panel->AddChild(largeChild);
// largeChild超出panel边界的部分会被裁剪
```

### 示例4：显式裁剪（兼容）
```cpp
auto element = std::make_shared<UIElement>();
element->SetClip(Rect(0, 0, 100, 100));
// 显式裁剪，保持向后兼容
```

## ⚠️ 风险与缓解

| 风险 | 影响 | 缓解措施 |
|------|------|----------|
| 性能回退 | 渲染变慢 | 性能基准测试，缓存优化 |
| 向后兼容 | 现有代码受影响 | 保留旧API，默认行为不变 |
| 复杂度增加 | 学习成本高 | 详细文档，使用示例 |

## 📖 相关文件

- **设计文档**: `Docs/Design/Render/CLIPPING_SYSTEM_REDESIGN.md`
- **现有实现**: 
  - `include/fk/render/RenderContext.h`
  - `src/render/RenderContext.cpp`
  - `src/ui/UIElement.cpp`
  - `src/render/GlRenderer.cpp`

## 📝 下一步

1. ✅ 完成设计文档
2. ⏳ 团队审核
3. ⏳ 开始阶段1实施
4. ⏳ 性能基准测试

---

**状态**: ✅ 设计完成  
**日期**: 2025-11-23  
**作者**: AI  
**版本**: 1.0
