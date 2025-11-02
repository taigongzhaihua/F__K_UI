#pragma once

#include "fk/ui/ContentControl.h"
#include "fk/ui/ScrollBar.h"
#include "fk/ui/Enums.h"
#include "fk/core/Event.h"

namespace fk::ui {

/**
 * @brief ScrollBar 可见性策略
 */
enum class ScrollBarVisibility {
    Auto,      // 需要时自动显示
    Visible,   // 始终可见
    Hidden,    // 始终隐藏
    Disabled   // 禁用(隐藏且不占用空间)
};

namespace detail {

/**
 * @brief ScrollViewer 基类实现
 * 
 * 提供内容滚动功能:
 * - 垂直和水平滚动条
 * - 滚动条可见性控制
 * - 内容偏移管理
 * - 鼠标滚轮支持
 */
class ScrollViewerBase : public ControlBase {
public:
    using ControlBase::ControlBase;

    ScrollViewerBase();
    ~ScrollViewerBase() override;

    // 依赖属性
    static const binding::DependencyProperty& HorizontalOffsetProperty();
    static const binding::DependencyProperty& VerticalOffsetProperty();
    static const binding::DependencyProperty& HorizontalScrollBarVisibilityProperty();
    static const binding::DependencyProperty& VerticalScrollBarVisibilityProperty();
    static const binding::DependencyProperty& ViewportWidthProperty();
    static const binding::DependencyProperty& ViewportHeightProperty();
    static const binding::DependencyProperty& ExtentWidthProperty();
    static const binding::DependencyProperty& ExtentHeightProperty();

    // HorizontalOffset 属性 (水平滚动偏移)
    void SetHorizontalOffset(double offset);
    [[nodiscard]] double GetHorizontalOffset() const;

    // VerticalOffset 属性 (垂直滚动偏移)
    void SetVerticalOffset(double offset);
    [[nodiscard]] double GetVerticalOffset() const;

    // HorizontalScrollBarVisibility 属性
    void SetHorizontalScrollBarVisibility(ScrollBarVisibility visibility);
    [[nodiscard]] ScrollBarVisibility GetHorizontalScrollBarVisibility() const;

    // VerticalScrollBarVisibility 属性
    void SetVerticalScrollBarVisibility(ScrollBarVisibility visibility);
    [[nodiscard]] ScrollBarVisibility GetVerticalScrollBarVisibility() const;

    // ViewportWidth 属性 (只读,可见区域宽度)
    [[nodiscard]] double GetViewportWidth() const;

    // ViewportHeight 属性 (只读,可见区域高度)
    [[nodiscard]] double GetViewportHeight() const;

    // ExtentWidth 属性 (只读,内容实际宽度)
    [[nodiscard]] double GetExtentWidth() const;

    // ExtentHeight 属性 (只读,内容实际高度)
    [[nodiscard]] double GetExtentHeight() const;

    // 滚动方法
    void ScrollToTop();
    void ScrollToBottom();
    void ScrollToLeft();
    void ScrollToRight();
    void ScrollToVerticalOffset(double offset);
    void ScrollToHorizontalOffset(double offset);
    void LineUp();      // 向上滚动一行
    void LineDown();    // 向下滚动一行
    void LineLeft();    // 向左滚动一列
    void LineRight();   // 向右滚动一列
    void PageUp();      // 向上滚动一页
    void PageDown();    // 向下滚动一页
    void PageLeft();    // 向左滚动一页
    void PageRight();   // 向右滚动一页

    // 滚动事件
    core::Event<double, double> ScrollChanged;  // (horizontalOffset, verticalOffset)

    // 获取 ScrollBar (用于渲染和交互)
    [[nodiscard]] ScrollBarView* GetVerticalScrollBar() const { return verticalScrollBar_.get(); }
    [[nodiscard]] ScrollBarView* GetHorizontalScrollBar() const { return horizontalScrollBar_.get(); }

protected:
    Size MeasureOverride(const Size& availableSize) override;
    Size ArrangeOverride(const Size& finalSize) override;
    
    // 重写可视子元素,包含 ScrollBar
    std::vector<Visual*> GetVisualChildren() const override;
    
    // 重写命中测试,检查 ScrollBar
    UIElement* HitTestChildren(double x, double y) override;
    
    // 重写鼠标事件,传递给 ScrollBar
    void OnMouseButtonDown(int button, double x, double y) override;
    void OnMouseButtonUp(int button, double x, double y) override;
    void OnMouseMove(double x, double y) override;
    
    // 鼠标滚轮事件处理
    void OnMouseWheel(double xoffset, double yoffset, double mouseX, double mouseY) override;

    // 属性变更回调
    virtual void OnHorizontalOffsetChanged(double oldValue, double newValue);
    virtual void OnVerticalOffsetChanged(double oldValue, double newValue);
    virtual void OnHorizontalScrollBarVisibilityChanged(ScrollBarVisibility oldValue, ScrollBarVisibility newValue);
    virtual void OnVerticalScrollBarVisibilityChanged(ScrollBarVisibility oldValue, ScrollBarVisibility newValue);

    // 更新滚动条状态
    void UpdateScrollBars();
    void UpdateHorizontalScrollBar();
    void UpdateVerticalScrollBar();

private:
    // 元数据构建
    static binding::PropertyMetadata BuildHorizontalOffsetMetadata();
    static binding::PropertyMetadata BuildVerticalOffsetMetadata();
    static binding::PropertyMetadata BuildHorizontalScrollBarVisibilityMetadata();
    static binding::PropertyMetadata BuildVerticalScrollBarVisibilityMetadata();
    static binding::PropertyMetadata BuildViewportWidthMetadata();
    static binding::PropertyMetadata BuildViewportHeightMetadata();
    static binding::PropertyMetadata BuildExtentWidthMetadata();
    static binding::PropertyMetadata BuildExtentHeightMetadata();

    // 属性变更回调 (静态)
    static void HorizontalOffsetPropertyChanged(
        binding::DependencyObject& sender,
        const binding::DependencyProperty& property,
        const std::any& oldValue,
        const std::any& newValue
    );

    static void VerticalOffsetPropertyChanged(
        binding::DependencyObject& sender,
        const binding::DependencyProperty& property,
        const std::any& oldValue,
        const std::any& newValue
    );

    static void HorizontalScrollBarVisibilityPropertyChanged(
        binding::DependencyObject& sender,
        const binding::DependencyProperty& property,
        const std::any& oldValue,
        const std::any& newValue
    );

    static void VerticalScrollBarVisibilityPropertyChanged(
        binding::DependencyObject& sender,
        const binding::DependencyProperty& property,
        const std::any& oldValue,
        const std::any& newValue
    );

    // 内部方法
    void SetViewportWidth(double width);
    void SetViewportHeight(double height);
    void SetExtentWidth(double width);
    void SetExtentHeight(double height);

    // ScrollBar 实例
    std::shared_ptr<ScrollBarView> horizontalScrollBar_;
    std::shared_ptr<ScrollBarView> verticalScrollBar_;

    // 缓存值
    double lineSize_ = 16.0;  // 行/列大小 (用于 LineUp/LineDown)
};

} // namespace detail

// ScrollViewer 模板类 (支持链式调用)
template <typename Derived>
class ScrollViewer : public detail::ScrollViewerBase, public std::enable_shared_from_this<Derived> {
public:
    using BaseType = detail::ScrollViewerBase;
    using Ptr = std::shared_ptr<Derived>;

    // Fluent API: HorizontalScrollBarVisibility
    [[nodiscard]] ScrollBarVisibility HorizontalScrollBarVisibility() const {
        return GetHorizontalScrollBarVisibility();
    }

    Ptr HorizontalScrollBarVisibility(ScrollBarVisibility visibility) {
        SetHorizontalScrollBarVisibility(visibility);
        return Self();
    }

    // Fluent API: VerticalScrollBarVisibility
    [[nodiscard]] ScrollBarVisibility VerticalScrollBarVisibility() const {
        return GetVerticalScrollBarVisibility();
    }

    Ptr VerticalScrollBarVisibility(ScrollBarVisibility visibility) {
        SetVerticalScrollBarVisibility(visibility);
        return Self();
    }

    // Fluent API: OnScrollChanged (事件订阅)
    Ptr OnScrollChanged(std::function<void(double, double)> callback) {
        ScrollChanged += std::move(callback);
        return Self();
    }

protected:
    Ptr Self() {
        auto* derivedThis = static_cast<Derived*>(this);
        return std::static_pointer_cast<Derived>(derivedThis->shared_from_this());
    }
};

// 标准 ScrollViewer 类
class ScrollViewerView : public ScrollViewer<ScrollViewerView> {
public:
    using ScrollViewer<ScrollViewerView>::ScrollViewer;
};

// 工厂函数
inline std::shared_ptr<ScrollViewerView> scrollViewer() {
    return std::make_shared<ScrollViewerView>();
}

} // namespace fk::ui
