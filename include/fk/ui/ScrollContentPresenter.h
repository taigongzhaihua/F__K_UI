#pragma once

#include "fk/ui/ContentPresenter.h"
#include "fk/binding/DependencyProperty.h"
#include "fk/core/Event.h"

namespace fk::ui {

// 前向声明
class ScrollViewer;

/**
 * @brief 滚动内容展示器
 * 
 * 职责：
 * - 在 ScrollViewer 的模板中展示可滚动内容
 * - 管理内容的测量和排列（支持无限大小测量）
 * - 提供滚动偏移控制
 * - 维护 Extent（内容大小）和 Viewport（可视区域大小）信息
 * 
 * WPF 对应：ScrollContentPresenter
 * 
 * 核心机制：
 * 1. MeasureOverride：在滚动方向上给内容无限空间
 * 2. ArrangeOverride：根据偏移量定位内容
 * 3. 自动裁剪超出可视区域的内容
 * 
 * 使用场景：
 * ```cpp
 * // 在 ScrollViewer 的 ControlTemplate 中：
 * auto* template = new ControlTemplate();
 * template->SetFactory([]() -> UIElement* {
 *     auto* grid = new Grid();
 *     // ... 布局 ScrollBar 等
 *     auto* presenter = new ScrollContentPresenter();
 *     presenter->SetName("PART_ScrollContentPresenter");
 *     grid->AddChild(presenter);
 *     return grid;
 * });
 * ```
 */
template<typename Derived = void>
class ScrollContentPresenter : public ContentPresenter<
    std::conditional_t<std::is_void_v<Derived>, ScrollContentPresenter<>, Derived>
> {
private:
    using Base = ContentPresenter<
        std::conditional_t<std::is_void_v<Derived>, ScrollContentPresenter<>, Derived>
    >;
    using Self = std::conditional_t<std::is_void_v<Derived>, ScrollContentPresenter<>*, Derived*>;

public:
    ScrollContentPresenter() = default;
    virtual ~ScrollContentPresenter() = default;

    // ========== 依赖属性声明 ==========
    
    /** @brief 是否可水平滚动 */
    static const binding::DependencyProperty& CanHorizontallyScrollProperty();
    
    /** @brief 是否可垂直滚动 */
    static const binding::DependencyProperty& CanVerticallyScrollProperty();
    
    /** @brief 水平滚动偏移量 */
    static const binding::DependencyProperty& HorizontalOffsetProperty();
    
    /** @brief 垂直滚动偏移量 */
    static const binding::DependencyProperty& VerticalOffsetProperty();

    // ========== 事件 ==========
    
    /**
     * @brief 滚动信息变化事件
     * 
     * 当 Extent、Viewport 或 Offset 变化时触发
     * ScrollViewer 订阅此事件以更新 ScrollBar
     */
    core::Event<> ScrollInfoChanged;

    // ========== CanHorizontallyScroll 属性 ==========
    
    bool GetCanHorizontallyScroll() const {
        return this->template GetValue<bool>(CanHorizontallyScrollProperty());
    }
    
    void SetCanHorizontallyScroll(bool value) {
        this->SetValue(CanHorizontallyScrollProperty(), value);
        this->InvalidateMeasure();
    }
    
    Self CanHorizontallyScroll(bool value) {
        SetCanHorizontallyScroll(value);
        return static_cast<Self>(this);
    }
    
    bool CanHorizontallyScroll() const { return GetCanHorizontallyScroll(); }

    // ========== CanVerticallyScroll 属性 ==========
    
    bool GetCanVerticallyScroll() const {
        return this->template GetValue<bool>(CanVerticallyScrollProperty());
    }
    
    void SetCanVerticallyScroll(bool value) {
        this->SetValue(CanVerticallyScrollProperty(), value);
        this->InvalidateMeasure();
    }
    
    Self CanVerticallyScroll(bool value) {
        SetCanVerticallyScroll(value);
        return static_cast<Self>(this);
    }
    
    bool CanVerticallyScroll() const { return GetCanVerticallyScroll(); }

    // ========== HorizontalOffset 属性 ==========
    
    double GetHorizontalOffset() const {
        return this->template GetValue<double>(HorizontalOffsetProperty());
    }
    
    void SetHorizontalOffset(double value) {
        // 钳制到有效范围
        double clampedValue = std::clamp(value, 0.0, GetScrollableWidth());
        if (clampedValue != GetHorizontalOffset()) {
            this->SetValue(HorizontalOffsetProperty(), clampedValue);
            this->InvalidateArrange();
            ScrollInfoChanged();
        }
    }
    
    Self HorizontalOffset(double value) {
        SetHorizontalOffset(value);
        return static_cast<Self>(this);
    }
    
    double HorizontalOffset() const { return GetHorizontalOffset(); }

    // ========== VerticalOffset 属性 ==========
    
    double GetVerticalOffset() const {
        return this->template GetValue<double>(VerticalOffsetProperty());
    }
    
    void SetVerticalOffset(double value) {
        // 钳制到有效范围
        double clampedValue = std::clamp(value, 0.0, GetScrollableHeight());
        if (clampedValue != GetVerticalOffset()) {
            this->SetValue(VerticalOffsetProperty(), clampedValue);
            this->InvalidateArrange();
            ScrollInfoChanged();
        }
    }
    
    Self VerticalOffset(double value) {
        SetVerticalOffset(value);
        return static_cast<Self>(this);
    }
    
    double VerticalOffset() const { return GetVerticalOffset(); }

    // ========== 只读属性（计算得出）==========
    
    /** @brief 内容总宽度 */
    double GetExtentWidth() const { return extentWidth_; }
    
    /** @brief 内容总高度 */
    double GetExtentHeight() const { return extentHeight_; }
    
    /** @brief 可视区域宽度 */
    double GetViewportWidth() const { return viewportWidth_; }
    
    /** @brief 可视区域高度 */
    double GetViewportHeight() const { return viewportHeight_; }
    
    /** @brief 可水平滚动的距离 */
    double GetScrollableWidth() const {
        return std::max(0.0, extentWidth_ - viewportWidth_);
    }
    
    /** @brief 可垂直滚动的距离 */
    double GetScrollableHeight() const {
        return std::max(0.0, extentHeight_ - viewportHeight_);
    }

    // ========== 滚动方法 ==========
    
    /** @brief 向上滚动一行 */
    void LineUp() {
        SetVerticalOffset(GetVerticalOffset() - lineScrollAmount_);
    }
    
    /** @brief 向下滚动一行 */
    void LineDown() {
        SetVerticalOffset(GetVerticalOffset() + lineScrollAmount_);
    }
    
    /** @brief 向左滚动一列 */
    void LineLeft() {
        SetHorizontalOffset(GetHorizontalOffset() - lineScrollAmount_);
    }
    
    /** @brief 向右滚动一列 */
    void LineRight() {
        SetHorizontalOffset(GetHorizontalOffset() + lineScrollAmount_);
    }
    
    /** @brief 向上翻页 */
    void PageUp() {
        SetVerticalOffset(GetVerticalOffset() - viewportHeight_);
    }
    
    /** @brief 向下翻页 */
    void PageDown() {
        SetVerticalOffset(GetVerticalOffset() + viewportHeight_);
    }
    
    /** @brief 向左翻页 */
    void PageLeft() {
        SetHorizontalOffset(GetHorizontalOffset() - viewportWidth_);
    }
    
    /** @brief 向右翻页 */
    void PageRight() {
        SetHorizontalOffset(GetHorizontalOffset() + viewportWidth_);
    }
    
    /** @brief 滚动到顶部 */
    void ScrollToTop() {
        SetVerticalOffset(0);
    }
    
    /** @brief 滚动到底部 */
    void ScrollToBottom() {
        SetVerticalOffset(GetScrollableHeight());
    }
    
    /** @brief 滚动到最左边 */
    void ScrollToLeftEnd() {
        SetHorizontalOffset(0);
    }
    
    /** @brief 滚动到最右边 */
    void ScrollToRightEnd() {
        SetHorizontalOffset(GetScrollableWidth());
    }
    
    /** @brief 滚动到指定位置 */
    void ScrollToPosition(double horizontalOffset, double verticalOffset) {
        SetHorizontalOffset(horizontalOffset);
        SetVerticalOffset(verticalOffset);
    }

    /**
     * @brief 使指定矩形区域可见
     * 
     * @param rect 要显示的矩形区域（相对于内容坐标）
     * @return 实际滚动到的矩形区域
     */
    Rect MakeVisible(const Rect& rect) {
        double newHorizontalOffset = GetHorizontalOffset();
        double newVerticalOffset = GetVerticalOffset();
        
        // 水平方向
        if (rect.x < newHorizontalOffset) {
            newHorizontalOffset = rect.x;
        } else if (rect.x + rect.width > newHorizontalOffset + viewportWidth_) {
            newHorizontalOffset = rect.x + rect.width - viewportWidth_;
        }
        
        // 垂直方向
        if (rect.y < newVerticalOffset) {
            newVerticalOffset = rect.y;
        } else if (rect.y + rect.height > newVerticalOffset + viewportHeight_) {
            newVerticalOffset = rect.y + rect.height - viewportHeight_;
        }
        
        SetHorizontalOffset(newHorizontalOffset);
        SetVerticalOffset(newVerticalOffset);
        
        // 返回实际可见的区域
        return Rect(
            GetHorizontalOffset(),
            GetVerticalOffset(),
            viewportWidth_,
            viewportHeight_
        );
    }

    // ========== 配置 ==========
    
    /** @brief 设置单行滚动量 */
    Self LineScrollAmount(double amount) {
        lineScrollAmount_ = amount;
        return static_cast<Self>(this);
    }
    
    double GetLineScrollAmount() const { return lineScrollAmount_; }

    // ========== Binding 支持 ==========
    
    Self CanHorizontallyScroll(binding::Binding binding) {
        this->SetBinding(CanHorizontallyScrollProperty(), std::move(binding));
        return static_cast<Self>(this);
    }
    
    Self CanVerticallyScroll(binding::Binding binding) {
        this->SetBinding(CanVerticallyScrollProperty(), std::move(binding));
        return static_cast<Self>(this);
    }
    
    Self HorizontalOffset(binding::Binding binding) {
        this->SetBinding(HorizontalOffsetProperty(), std::move(binding));
        return static_cast<Self>(this);
    }
    
    Self VerticalOffset(binding::Binding binding) {
        this->SetBinding(VerticalOffsetProperty(), std::move(binding));
        return static_cast<Self>(this);
    }

protected:
    /**
     * @brief 测量内容
     * 
     * 核心逻辑：
     * 1. 先用有限空间测量，获取内容的"自然尺寸"
     * 2. 如果内容需要更多空间且允许滚动，再用无限空间测量
     * 
     * 这样可以正确处理"Auto"滚动模式：
     * - 内容小于视口时，不需要滚动，内容正常布局
     * - 内容大于视口时，允许滚动，给内容无限空间
     */
    Size MeasureOverride(const Size& availableSize) override {
        UIElement* child = this->GetVisualChild();
        Size childDesiredSize(0, 0);
        
        if (child) {
            // 构建测量约束
            // 关键：只在需要滚动的方向给无限空间
            // 对于典型的垂直滚动场景，水平方向应该使用可用宽度
            Size contentConstraint = availableSize;
            
            // 只有当确实需要水平滚动时才给无限宽度
            // 注意：CanHorizontallyScroll=true 只表示"允许"滚动，不表示"需要"滚动
            // 因此我们应该先用有限空间测量，然后根据结果决定
            if (GetCanVerticallyScroll()) {
                contentConstraint.height = std::numeric_limits<float>::infinity();
            }
            // 水平方向保持有限宽度，让 StackPanel 等容器正确计算宽度
            // 只有当子元素显式设置了更大的宽度时才会超出
            
            child->Measure(contentConstraint);
            childDesiredSize = child->GetDesiredSize();
            
            // 如果内容宽度超出可用宽度且允许水平滚动，需要重新测量
            if (GetCanHorizontallyScroll() && childDesiredSize.width > availableSize.width) {
                contentConstraint.width = std::numeric_limits<float>::infinity();
                child->InvalidateMeasure();
                child->Measure(contentConstraint);
                childDesiredSize = child->GetDesiredSize();
            }
        }
        
        // 更新 Extent（内容大小）
        bool extentChanged = false;
        if (extentWidth_ != childDesiredSize.width || extentHeight_ != childDesiredSize.height) {
            extentWidth_ = childDesiredSize.width;
            extentHeight_ = childDesiredSize.height;
            extentChanged = true;
        }
        
        // 计算期望大小（不超过可用空间）
        Size desiredSize(
            std::min(childDesiredSize.width, availableSize.width),
            std::min(childDesiredSize.height, availableSize.height)
        );
        
        // 如果 Extent 变化，触发事件
        if (extentChanged) {
            ScrollInfoChanged();
        }
        
        return desiredSize;
    }
    
    /**
     * @brief 排列内容
     * 
     * 核心逻辑：根据偏移量将内容定位在负坐标位置
     */
    Size ArrangeOverride(const Size& finalSize) override {
        UIElement* child = this->GetVisualChild();
        
        // 更新 Viewport（可视区域大小）
        bool viewportChanged = false;
        if (viewportWidth_ != finalSize.width || viewportHeight_ != finalSize.height) {
            viewportWidth_ = finalSize.width;
            viewportHeight_ = finalSize.height;
            viewportChanged = true;
            
            // Viewport 变化可能需要调整偏移量
            CoerceOffsets();
        }
        
        // 排列内容
        if (child) {
            // 内容大小：取 Extent 和 Viewport 中的较大值
            Size contentSize(
                std::max(extentWidth_, (double)finalSize.width),
                std::max(extentHeight_, (double)finalSize.height)
            );
            
            // 根据偏移量定位内容（负坐标实现滚动效果）
            double offsetX = GetCanHorizontallyScroll() ? -GetHorizontalOffset() : 0;
            double offsetY = GetCanVerticallyScroll() ? -GetVerticalOffset() : 0;
            
            Rect childRect(
                static_cast<float>(offsetX),
                static_cast<float>(offsetY),
                static_cast<float>(contentSize.width),
                static_cast<float>(contentSize.height)
            );
            
            // 强制子元素重新排列，确保它的子元素也被排列
            child->InvalidateArrange();
            child->Arrange(childRect);
        }
        
        // 如果 Viewport 变化，触发事件
        if (viewportChanged) {
            ScrollInfoChanged();
        }
        
        return finalSize;
    }
    
    /**
     * @brief 始终启用裁剪
     * 
     * ScrollContentPresenter 需要裁剪超出可视区域的内容
     */
    bool ShouldClipToBounds() const override { return true; }
    
    /**
     * @brief 计算裁剪边界
     * 
     * 返回整个可视区域作为裁剪边界
     * 如果 viewport 尚未设置，使用 renderSize 作为后备
     */
    Rect CalculateClipBounds() const override {
        // 如果 viewport 尚未初始化（首次布局前），使用 renderSize
        double width = viewportWidth_ > 0 ? viewportWidth_ : this->GetRenderSize().width;
        double height = viewportHeight_ > 0 ? viewportHeight_ : this->GetRenderSize().height;
        return Rect(0, 0, static_cast<float>(width), static_cast<float>(height));
    }

private:
    /**
     * @brief 调整偏移量到有效范围
     * 
     * 当 Viewport 或 Extent 变化时调用
     */
    void CoerceOffsets() {
        double maxHOffset = GetScrollableWidth();
        double maxVOffset = GetScrollableHeight();
        
        if (GetHorizontalOffset() > maxHOffset) {
            this->SetValue(HorizontalOffsetProperty(), maxHOffset);
        }
        if (GetVerticalOffset() > maxVOffset) {
            this->SetValue(VerticalOffsetProperty(), maxVOffset);
        }
    }

    // 内容大小
    double extentWidth_{0};
    double extentHeight_{0};
    
    // 可视区域大小
    double viewportWidth_{0};
    double viewportHeight_{0};
    
    // 单行滚动量（像素）
    double lineScrollAmount_{16.0};
};

// ========== 依赖属性实现 ==========

template<typename Derived>
const binding::DependencyProperty& ScrollContentPresenter<Derived>::CanHorizontallyScrollProperty() {
    static auto& property = binding::DependencyProperty::Register(
        "CanHorizontallyScroll",
        typeid(bool),
        typeid(ScrollContentPresenter<Derived>),
        binding::PropertyMetadata{std::any(false)}
    );
    return property;
}

template<typename Derived>
const binding::DependencyProperty& ScrollContentPresenter<Derived>::CanVerticallyScrollProperty() {
    static auto& property = binding::DependencyProperty::Register(
        "CanVerticallyScroll",
        typeid(bool),
        typeid(ScrollContentPresenter<Derived>),
        binding::PropertyMetadata{std::any(true)}
    );
    return property;
}

template<typename Derived>
const binding::DependencyProperty& ScrollContentPresenter<Derived>::HorizontalOffsetProperty() {
    static auto& property = binding::DependencyProperty::Register(
        "HorizontalOffset",
        typeid(double),
        typeid(ScrollContentPresenter<Derived>),
        binding::PropertyMetadata{std::any(0.0)}
    );
    return property;
}

template<typename Derived>
const binding::DependencyProperty& ScrollContentPresenter<Derived>::VerticalOffsetProperty() {
    static auto& property = binding::DependencyProperty::Register(
        "VerticalOffset",
        typeid(double),
        typeid(ScrollContentPresenter<Derived>),
        binding::PropertyMetadata{std::any(0.0)}
    );
    return property;
}

// 类型别名
using ScrollContentPresenter_t = ScrollContentPresenter<>;

} // namespace fk::ui
