#pragma once

#include "fk/ui/ContentControl.h"
#include "fk/ui/ScrollBar.h"
#include "fk/ui/Enums.h"
#include "fk/ui/BindingMacros.h"
#include "fk/ui/DependencyPropertyMacros.h"
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
class ScrollViewerBase : public ContentControl {
public:
    using ContentControl::ContentControl;

    ScrollViewerBase();
    ~ScrollViewerBase() override;

    // 依赖属性（使用宏）
    FK_DEPENDENCY_PROPERTY_DECLARE(HorizontalOffset, double)
    FK_DEPENDENCY_PROPERTY_DECLARE(VerticalOffset, double)
    FK_DEPENDENCY_PROPERTY_DECLARE(HorizontalScrollBarVisibility, ScrollBarVisibility)
    FK_DEPENDENCY_PROPERTY_DECLARE(VerticalScrollBarVisibility, ScrollBarVisibility)
    FK_DEPENDENCY_PROPERTY_DECLARE(ViewportWidth, double)
    FK_DEPENDENCY_PROPERTY_DECLARE(ViewportHeight, double)
    FK_DEPENDENCY_PROPERTY_DECLARE(ExtentWidth, double)
    FK_DEPENDENCY_PROPERTY_DECLARE(ExtentHeight, double)

public:

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
    bool OnMouseButtonDown(int button, double x, double y) override;
    bool OnMouseButtonUp(int button, double x, double y) override;
    bool OnMouseMove(double x, double y) override;
    
    // 鼠标滚轮事件处理
    bool OnMouseWheel(double xoffset, double yoffset, double mouseX, double mouseY) override;

    // 更新滚动条状态
    void UpdateScrollBars();
    void UpdateHorizontalScrollBar();
    void UpdateVerticalScrollBar();

private:
    // ScrollBar 实例
    std::shared_ptr<ScrollBarView> horizontalScrollBar_;
    std::shared_ptr<ScrollBarView> verticalScrollBar_;

    // 缓存值
    double lineSize_ = 16.0;  // 行/列大小 (用于 LineUp/LineDown)
};

} // namespace detail

// ScrollViewer 模板类 (支持链式调用)
template <typename Derived>
class ScrollViewer : public detail::ScrollViewerBase {
public:
    using BaseType = detail::ScrollViewerBase;
    using Ptr = std::shared_ptr<Derived>;

    // 🎯 Fluent API with Binding Support
    FK_BINDING_PROPERTY_VALUE(HorizontalOffset, double)
    FK_BINDING_PROPERTY_VALUE(VerticalOffset, double)
    FK_BINDING_PROPERTY_ENUM(HorizontalScrollBarVisibility, ScrollBarVisibility)
    FK_BINDING_PROPERTY_ENUM(VerticalScrollBarVisibility, ScrollBarVisibility)

    // Fluent API: OnScrollChanged (事件订阅)
    Ptr OnScrollChanged(std::function<void(double, double)> callback) {
        ScrollChanged += std::move(callback);
        return Self();
    }

    // Fluent API: 滚动到指定位置
    Ptr ScrollToVerticalOffset(double offset) {
        SetVerticalOffset(offset);
        return Self();
    }

    Ptr ScrollToHorizontalOffset(double offset) {
        SetHorizontalOffset(offset);
        return Self();
    }

    Ptr ScrollToTop() {
        BaseType::ScrollToTop();
        return Self();
    }

    Ptr ScrollToBottom() {
        BaseType::ScrollToBottom();
        return Self();
    }

    Ptr ScrollToLeft() {
        BaseType::ScrollToLeft();
        return Self();
    }

    Ptr ScrollToRight() {
        BaseType::ScrollToRight();
        return Self();
    }

protected:
    Ptr Self() {
        // 通过 ContentControl 基类获取 shared_from_this
        auto base = ContentControl::shared_from_this();
        return std::static_pointer_cast<Derived>(base);
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
