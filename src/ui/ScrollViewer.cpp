/**
 * @file ScrollViewer.cpp
 * @brief ScrollViewer 滚动查看器控件实现 - Phase 2
 * 
 * Phase 1: 基础架构和接口定义 ✓
 * Phase 2: ScrollContentPresenter 实现 ✓
 * Phase 3: 滚动条集成和事件处理（待实现）
 * Phase 4: 模板和样式支持（待实现）
 * Phase 5: 鼠标滚轮和触摸支持（待实现）
 */

#include "fk/ui/ScrollViewer.h"
#include "fk/ui/ScrollContentPresenter.h"
#include "fk/ui/ScrollBar.h"  // Phase 3 Step 6: 创建滚动条实例
#include <algorithm>
#include <limits>  // For std::numeric_limits

namespace fk::ui {

// ========== 构造函数 ==========

ScrollViewer::ScrollViewer() {
    // Phase 2: 创建 ScrollContentPresenter
    scrollContentPresenter_ = new ScrollContentPresenter();
    scrollContentPresenter_->SetScrollOwner(this);
    
    // 订阅 ScrollContentPresenter 的滚动变更事件
    scrollContentPresenter_->ScrollChanged.Connect([this]() {
        OnScrollContentPresenterChanged();
    });
    
    // Phase 3 Step 6: 创建滚动条
    CreateScrollBars();
    ConnectScrollBarEvents();
}

ScrollViewer::~ScrollViewer() {
    // 清理 ScrollContentPresenter
    delete scrollContentPresenter_;
    scrollContentPresenter_ = nullptr;
    
    // Phase 3 Step 6: 清理滚动条
    delete verticalScrollBar_;
    verticalScrollBar_ = nullptr;
    
    delete horizontalScrollBar_;
    horizontalScrollBar_ = nullptr;
}

// ========== 依赖属性定义 ==========

const binding::DependencyProperty& ScrollViewer::HorizontalScrollBarVisibilityProperty() {
    static auto& property = 
        binding::DependencyProperty::Register(
            "HorizontalScrollBarVisibility",
            typeid(ScrollBarVisibility),
            typeid(ScrollViewer),
            binding::PropertyMetadata{std::any(ScrollBarVisibility::Auto)}
        );
    return property;
}

const binding::DependencyProperty& ScrollViewer::VerticalScrollBarVisibilityProperty() {
    static auto& property = 
        binding::DependencyProperty::Register(
            "VerticalScrollBarVisibility",
            typeid(ScrollBarVisibility),
            typeid(ScrollViewer),
            binding::PropertyMetadata{std::any(ScrollBarVisibility::Auto)}
        );
    return property;
}

const binding::DependencyProperty& ScrollViewer::HorizontalOffsetProperty() {
    static auto& property = 
        binding::DependencyProperty::Register(
            "HorizontalOffset",
            typeid(float),
            typeid(ScrollViewer),
            binding::PropertyMetadata{std::any(0.0f)}
        );
    return property;
}

const binding::DependencyProperty& ScrollViewer::VerticalOffsetProperty() {
    static auto& property = 
        binding::DependencyProperty::Register(
            "VerticalOffset",
            typeid(float),
            typeid(ScrollViewer),
            binding::PropertyMetadata{std::any(0.0f)}
        );
    return property;
}

const binding::DependencyProperty& ScrollViewer::ViewportWidthProperty() {
    static auto& property = 
        binding::DependencyProperty::Register(
            "ViewportWidth",
            typeid(float),
            typeid(ScrollViewer),
            binding::PropertyMetadata{std::any(0.0f)}
        );
    return property;
}

const binding::DependencyProperty& ScrollViewer::ViewportHeightProperty() {
    static auto& property = 
        binding::DependencyProperty::Register(
            "ViewportHeight",
            typeid(float),
            typeid(ScrollViewer),
            binding::PropertyMetadata{std::any(0.0f)}
        );
    return property;
}

const binding::DependencyProperty& ScrollViewer::ExtentWidthProperty() {
    static auto& property = 
        binding::DependencyProperty::Register(
            "ExtentWidth",
            typeid(float),
            typeid(ScrollViewer),
            binding::PropertyMetadata{std::any(0.0f)}
        );
    return property;
}

const binding::DependencyProperty& ScrollViewer::ExtentHeightProperty() {
    static auto& property = 
        binding::DependencyProperty::Register(
            "ExtentHeight",
            typeid(float),
            typeid(ScrollViewer),
            binding::PropertyMetadata{std::any(0.0f)}
        );
    return property;
}

const binding::DependencyProperty& ScrollViewer::ScrollableWidthProperty() {
    static auto& property = 
        binding::DependencyProperty::Register(
            "ScrollableWidth",
            typeid(float),
            typeid(ScrollViewer),
            binding::PropertyMetadata{std::any(0.0f)}
        );
    return property;
}

const binding::DependencyProperty& ScrollViewer::ScrollableHeightProperty() {
    static auto& property = 
        binding::DependencyProperty::Register(
            "ScrollableHeight",
            typeid(float),
            typeid(ScrollViewer),
            binding::PropertyMetadata{std::any(0.0f)}
        );
    return property;
}

const binding::DependencyProperty& ScrollViewer::CanContentScrollProperty() {
    static auto& property = 
        binding::DependencyProperty::Register(
            "CanContentScroll",
            typeid(bool),
            typeid(ScrollViewer),
            binding::PropertyMetadata{std::any(false)}
        );
    return property;
}

const binding::DependencyProperty& ScrollViewer::IsDeferredScrollingEnabledProperty() {
    static auto& property = 
        binding::DependencyProperty::Register(
            "IsDeferredScrollingEnabled",
            typeid(bool),
            typeid(ScrollViewer),
            binding::PropertyMetadata{std::any(false)}
        );
    return property;
}

// ========== 附加属性实现 ==========

ScrollBarVisibility ScrollViewer::GetHorizontalScrollBarVisibility(const UIElement* element) {
    if (!element) {
        return ScrollBarVisibility::Auto;  // 默认值
    }
    // TODO: 实现附加属性获取
    // 需要通过依赖属性系统从元素获取附加属性值
    return ScrollBarVisibility::Auto;
}

void ScrollViewer::SetHorizontalScrollBarVisibility(UIElement* element, ScrollBarVisibility value) {
    if (!element) {
        return;  // 忽略 null 元素
    }
    // TODO: 实现附加属性设置
    // 需要通过依赖属性系统设置元素的附加属性值
}

ScrollBarVisibility ScrollViewer::GetVerticalScrollBarVisibility(const UIElement* element) {
    if (!element) {
        return ScrollBarVisibility::Auto;  // 默认值
    }
    // TODO: 实现附加属性获取
    // 需要通过依赖属性系统从元素获取附加属性值
    return ScrollBarVisibility::Auto;
}

void ScrollViewer::SetVerticalScrollBarVisibility(UIElement* element, ScrollBarVisibility value) {
    if (!element) {
        return;  // 忽略 null 元素
    }
    // TODO: 实现附加属性设置
    // 需要通过依赖属性系统设置元素的附加属性值
}

// ========== 属性访问器 ==========

ScrollBarVisibility ScrollViewer::GetHorizontalScrollBarVisibility() const {
    return horizontalScrollBarVisibility_;
}

ScrollViewer* ScrollViewer::SetHorizontalScrollBarVisibility(ScrollBarVisibility value) {
    horizontalScrollBarVisibility_ = value;
    UpdateScrollBarVisibility();  // Phase 3 Step 6: 更新滚动条可见性
    return this;
}

ScrollBarVisibility ScrollViewer::GetVerticalScrollBarVisibility() const {
    return verticalScrollBarVisibility_;
}

ScrollViewer* ScrollViewer::SetVerticalScrollBarVisibility(ScrollBarVisibility value) {
    verticalScrollBarVisibility_ = value;
    UpdateScrollBarVisibility();  // Phase 3 Step 6: 更新滚动条可见性
    return this;
}

float ScrollViewer::GetHorizontalOffset() const {
    return horizontalOffset_;
}

float ScrollViewer::GetVerticalOffset() const {
    return verticalOffset_;
}

float ScrollViewer::GetViewportWidth() const {
    return viewportWidth_;
}

float ScrollViewer::GetViewportHeight() const {
    return viewportHeight_;
}

float ScrollViewer::GetExtentWidth() const {
    return extentWidth_;
}

float ScrollViewer::GetExtentHeight() const {
    return extentHeight_;
}

float ScrollViewer::GetScrollableWidth() const {
    return std::max(0.0f, extentWidth_ - viewportWidth_);
}

float ScrollViewer::GetScrollableHeight() const {
    return std::max(0.0f, extentHeight_ - viewportHeight_);
}

bool ScrollViewer::GetCanContentScroll() const {
    return canContentScroll_;
}

ScrollViewer* ScrollViewer::SetCanContentScroll(bool value) {
    canContentScroll_ = value;
    // TODO: 通知 ScrollContentPresenter
    return this;
}

// ========== 滚动方法（Phase 2 实现）==========

namespace {
    // 默认滚动增量（像素）
    constexpr float DEFAULT_LINE_DELTA = 16.0f;
}

void ScrollViewer::LineLeft() {
    if (scrollContentPresenter_) {
        float offset = scrollContentPresenter_->GetHorizontalOffset() - DEFAULT_LINE_DELTA;
        scrollContentPresenter_->SetHorizontalOffset(offset);
    }
}

void ScrollViewer::LineRight() {
    if (scrollContentPresenter_) {
        float offset = scrollContentPresenter_->GetHorizontalOffset() + DEFAULT_LINE_DELTA;
        scrollContentPresenter_->SetHorizontalOffset(offset);
    }
}

void ScrollViewer::LineUp() {
    if (scrollContentPresenter_) {
        float offset = scrollContentPresenter_->GetVerticalOffset() - DEFAULT_LINE_DELTA;
        scrollContentPresenter_->SetVerticalOffset(offset);
    }
}

void ScrollViewer::LineDown() {
    if (scrollContentPresenter_) {
        float offset = scrollContentPresenter_->GetVerticalOffset() + DEFAULT_LINE_DELTA;
        scrollContentPresenter_->SetVerticalOffset(offset);
    }
}

void ScrollViewer::PageUp() {
    if (scrollContentPresenter_) {
        float offset = scrollContentPresenter_->GetVerticalOffset() - viewportHeight_;
        scrollContentPresenter_->SetVerticalOffset(offset);
    }
}

void ScrollViewer::PageDown() {
    if (scrollContentPresenter_) {
        float offset = scrollContentPresenter_->GetVerticalOffset() + viewportHeight_;
        scrollContentPresenter_->SetVerticalOffset(offset);
    }
}

void ScrollViewer::PageLeft() {
    if (scrollContentPresenter_) {
        float offset = scrollContentPresenter_->GetHorizontalOffset() - viewportWidth_;
        scrollContentPresenter_->SetHorizontalOffset(offset);
    }
}

void ScrollViewer::PageRight() {
    if (scrollContentPresenter_) {
        float offset = scrollContentPresenter_->GetHorizontalOffset() + viewportWidth_;
        scrollContentPresenter_->SetHorizontalOffset(offset);
    }
}

void ScrollViewer::ScrollToTop() {
    if (scrollContentPresenter_) {
        scrollContentPresenter_->SetVerticalOffset(0.0f);
    }
}

void ScrollViewer::ScrollToBottom() {
    if (scrollContentPresenter_) {
        // 滚动到底部：offset = extent - viewport
        float maxOffset = std::max(0.0f, extentHeight_ - viewportHeight_);
        scrollContentPresenter_->SetVerticalOffset(maxOffset);
    }
}

void ScrollViewer::ScrollToLeftEnd() {
    if (scrollContentPresenter_) {
        scrollContentPresenter_->SetHorizontalOffset(0.0f);
    }
}

void ScrollViewer::ScrollToRightEnd() {
    if (scrollContentPresenter_) {
        // 滚动到最右：offset = extent - viewport
        float maxOffset = std::max(0.0f, extentWidth_ - viewportWidth_);
        scrollContentPresenter_->SetHorizontalOffset(maxOffset);
    }
}

void ScrollViewer::ScrollToHorizontalOffset(float offset) {
    if (scrollContentPresenter_) {
        scrollContentPresenter_->SetHorizontalOffset(offset);
    }
}

void ScrollViewer::ScrollToVerticalOffset(float offset) {
    if (scrollContentPresenter_) {
        scrollContentPresenter_->SetVerticalOffset(offset);
    }
}

void ScrollViewer::ScrollToElement(UIElement* element) {
    // 输入验证
    if (!element) {
        return;  // 忽略 null 元素
    }
    // TODO: Phase 3 - 实现滚动到元素的逻辑
    // 需要计算元素在内容中的位置，然后滚动到该位置
}

// ========== 布局重写（Phase 2 实现）==========

Size ScrollViewer::MeasureOverride(const Size& availableSize) {
    if (!scrollContentPresenter_) {
        return Size(0, 0);
    }
    
    // 配置 ScrollContentPresenter 的滚动能力
    scrollContentPresenter_->SetCanHorizontallyScroll(
        horizontalScrollBarVisibility_ != ScrollBarVisibility::Disabled
    );
    scrollContentPresenter_->SetCanVerticallyScroll(
        verticalScrollBarVisibility_ != ScrollBarVisibility::Disabled
    );
    
    // 将内容传递给 ScrollContentPresenter
    scrollContentPresenter_->SetContent(GetContent());
    
    // Phase 3 Step 6: 预留滚动条空间
    Size contentSize = availableSize;
    
    // 如果垂直滚动条可能可见，预留空间
    if (verticalScrollBar_ && verticalScrollBarVisibility_ != ScrollBarVisibility::Disabled) {
        verticalScrollBar_->Measure(Size(std::numeric_limits<float>::infinity(), availableSize.height));
        float scrollBarWidth = verticalScrollBar_->GetDesiredSize().width;
        if (scrollBarWidth > 0) {
            contentSize.width = std::max(0.0f, contentSize.width - scrollBarWidth);
        }
    }
    
    // 如果水平滚动条可能可见，预留空间
    if (horizontalScrollBar_ && horizontalScrollBarVisibility_ != ScrollBarVisibility::Disabled) {
        horizontalScrollBar_->Measure(Size(availableSize.width, std::numeric_limits<float>::infinity()));
        float scrollBarHeight = horizontalScrollBar_->GetDesiredSize().height;
        if (scrollBarHeight > 0) {
            contentSize.height = std::max(0.0f, contentSize.height - scrollBarHeight);
        }
    }
    
    // 测量 ScrollContentPresenter
    scrollContentPresenter_->Measure(contentSize);
    
    return scrollContentPresenter_->GetDesiredSize();
}

Size ScrollViewer::ArrangeOverride(const Size& finalSize) {
    if (!scrollContentPresenter_) {
        return finalSize;
    }
    
    // Phase 3 Step 6: 计算滚动条大小和位置
    float verticalScrollBarWidth = 0.0f;
    float horizontalScrollBarHeight = 0.0f;
    
    // 获取垂直滚动条宽度
    if (verticalScrollBar_ && verticalScrollBar_->GetVisibility() == Visibility::Visible) {
        verticalScrollBarWidth = verticalScrollBar_->GetDesiredSize().width;
    }
    
    // 获取水平滚动条高度
    if (horizontalScrollBar_ && horizontalScrollBar_->GetVisibility() == Visibility::Visible) {
        horizontalScrollBarHeight = horizontalScrollBar_->GetDesiredSize().height;
    }
    
    // 排列 ScrollContentPresenter（预留滚动条空间）
    float contentWidth = std::max(0.0f, finalSize.width - verticalScrollBarWidth);
    float contentHeight = std::max(0.0f, finalSize.height - horizontalScrollBarHeight);
    scrollContentPresenter_->Arrange(Rect(0, 0, contentWidth, contentHeight));
    
    // 排列垂直滚动条（右侧）
    if (verticalScrollBar_ && verticalScrollBar_->GetVisibility() == Visibility::Visible) {
        float x = finalSize.width - verticalScrollBarWidth;
        verticalScrollBar_->Arrange(Rect(x, 0, verticalScrollBarWidth, contentHeight));
    }
    
    // 排列水平滚动条（底部）
    if (horizontalScrollBar_ && horizontalScrollBar_->GetVisibility() == Visibility::Visible) {
        float y = finalSize.height - horizontalScrollBarHeight;
        horizontalScrollBar_->Arrange(Rect(0, y, contentWidth, horizontalScrollBarHeight));
    }
    
    return finalSize;
}

// ========== 内部回调（Phase 2 实现）==========

void ScrollViewer::OnScrollContentPresenterChanged() {
    if (!scrollContentPresenter_) {
        return;
    }
    
    // 从 ScrollContentPresenter 同步滚动信息
    horizontalOffset_ = scrollContentPresenter_->GetHorizontalOffset();
    verticalOffset_ = scrollContentPresenter_->GetVerticalOffset();
    viewportWidth_ = scrollContentPresenter_->GetViewportWidth();
    viewportHeight_ = scrollContentPresenter_->GetViewportHeight();
    extentWidth_ = scrollContentPresenter_->GetExtentWidth();
    extentHeight_ = scrollContentPresenter_->GetExtentHeight();
    
    // 更新滚动条
    UpdateScrollBars();
    
    // TODO: 触发 ScrollChanged 事件
}

// ========== 辅助方法（Phase 2 实现）==========

void ScrollViewer::UpdateScrollBars() {
    // Phase 3 Step 6: 更新滚动条的范围、位置和可见性
    if (!verticalScrollBar_ || !horizontalScrollBar_) {
        return;
    }
    
    // 防止循环更新
    if (isUpdatingFromScrollBar_) {
        return;
    }
    
    // 更新垂直滚动条
    verticalScrollBar_->SetMinimum(0.0f);
    verticalScrollBar_->SetMaximum(extentHeight_);
    verticalScrollBar_->SetViewportSize(viewportHeight_);
    verticalScrollBar_->SetValue(verticalOffset_);
    
    // 更新水平滚动条
    horizontalScrollBar_->SetMinimum(0.0f);
    horizontalScrollBar_->SetMaximum(extentWidth_);
    horizontalScrollBar_->SetViewportSize(viewportWidth_);
    horizontalScrollBar_->SetValue(horizontalOffset_);
    
    // 更新可见性
    UpdateScrollBarVisibility();
}

// ========== Phase 3 Step 6 实现 ==========

void ScrollViewer::CreateScrollBars() {
    // 创建垂直滚动条
    verticalScrollBar_ = new ScrollBar();
    verticalScrollBar_->SetOrientation(Orientation::Vertical);
    
    // 创建水平滚动条
    horizontalScrollBar_ = new ScrollBar();
    horizontalScrollBar_->SetOrientation(Orientation::Horizontal);
}

void ScrollViewer::ConnectScrollBarEvents() {
    if (!verticalScrollBar_ || !horizontalScrollBar_) {
        return;
    }
    
    // 连接垂直滚动条 ValueChanged 事件
    verticalScrollBar_->ValueChanged.Connect([this](float oldValue, float newValue) {
        OnScrollBarValueChanged(true, newValue);
    });
    
    // 连接水平滚动条 ValueChanged 事件
    horizontalScrollBar_->ValueChanged.Connect([this](float oldValue, float newValue) {
        OnScrollBarValueChanged(false, newValue);
    });
}

void ScrollViewer::UpdateScrollBarVisibility() {
    if (!verticalScrollBar_ || !horizontalScrollBar_) {
        return;
    }
    
    // 计算是否需要显示滚动条
    bool needsVerticalScroll = extentHeight_ > viewportHeight_;
    bool needsHorizontalScroll = extentWidth_ > viewportWidth_;
    
    // 更新垂直滚动条可见性
    switch (verticalScrollBarVisibility_) {
        case ScrollBarVisibility::Disabled:
            verticalScrollBar_->SetVisibility(Visibility::Collapsed);
            break;
        case ScrollBarVisibility::Auto:
            verticalScrollBar_->SetVisibility(
                needsVerticalScroll ? Visibility::Visible : Visibility::Collapsed
            );
            break;
        case ScrollBarVisibility::Hidden:
            verticalScrollBar_->SetVisibility(Visibility::Collapsed);
            break;
        case ScrollBarVisibility::Visible:
            verticalScrollBar_->SetVisibility(Visibility::Visible);
            break;
    }
    
    // 更新水平滚动条可见性
    switch (horizontalScrollBarVisibility_) {
        case ScrollBarVisibility::Disabled:
            horizontalScrollBar_->SetVisibility(Visibility::Collapsed);
            break;
        case ScrollBarVisibility::Auto:
            horizontalScrollBar_->SetVisibility(
                needsHorizontalScroll ? Visibility::Visible : Visibility::Collapsed
            );
            break;
        case ScrollBarVisibility::Hidden:
            horizontalScrollBar_->SetVisibility(Visibility::Collapsed);
            break;
        case ScrollBarVisibility::Visible:
            horizontalScrollBar_->SetVisibility(Visibility::Visible);
            break;
    }
}

void ScrollViewer::OnScrollBarValueChanged(bool isVertical, float value) {
    if (!scrollContentPresenter_) {
        return;
    }
    
    // 设置标志防止循环更新
    isUpdatingFromScrollBar_ = true;
    
    // 更新 ScrollContentPresenter 的偏移
    if (isVertical) {
        scrollContentPresenter_->SetVerticalOffset(value);
    } else {
        scrollContentPresenter_->SetHorizontalOffset(value);
    }
    
    // 重置标志
    isUpdatingFromScrollBar_ = false;
}

} // namespace fk::ui
