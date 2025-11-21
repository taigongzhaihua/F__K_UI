/**
 * @file ScrollViewer.cpp
 * @brief ScrollViewer 滚动查看器控件实现
 */

#include "fk/ui/ScrollViewer.h"
#include "fk/render/DrawCommand.h"
#include "fk/render/RenderContext.h"
#include <algorithm>

namespace fk::ui {

using Color = fk::render::Color;

// ========== 构造函数 ==========

ScrollViewer::ScrollViewer() {
    // 创建滚动条
    horizontalScrollBar_ = new ScrollBar();
    horizontalScrollBar_->SetOrientation(Orientation::Horizontal);
    
    verticalScrollBar_ = new ScrollBar();
    verticalScrollBar_->SetOrientation(Orientation::Vertical);
    
    // 订阅滚动条值变更事件
    horizontalScrollBar_->ValueChanged.Connect([this](float oldValue, float newValue) {
        OnHorizontalScrollBarValueChanged(oldValue, newValue);
    });
    
    verticalScrollBar_->ValueChanged.Connect([this](float oldValue, float newValue) {
        OnVerticalScrollBarValueChanged(oldValue, newValue);
    });
    
    // 初始隐藏滚动条
    UpdateScrollBarsVisibility();
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

// ========== 属性设置 ==========

void ScrollViewer::SetHorizontalOffset(float value) {
    float maxOffset = std::max(0.0f, extentWidth_ - viewportWidth_);
    horizontalOffset_ = std::max(0.0f, std::min(maxOffset, value));
    
    if (horizontalScrollBar_) {
        horizontalScrollBar_->SetValue(horizontalOffset_);
    }
    
    InvalidateVisual();
    OnScrollChanged();
}

void ScrollViewer::SetVerticalOffset(float value) {
    float maxOffset = std::max(0.0f, extentHeight_ - viewportHeight_);
    verticalOffset_ = std::max(0.0f, std::min(maxOffset, value));
    
    if (verticalScrollBar_) {
        verticalScrollBar_->SetValue(verticalOffset_);
    }
    
    InvalidateVisual();
    OnScrollChanged();
}

// ========== 滚动方法 ==========

void ScrollViewer::LineLeft() {
    ScrollToHorizontalOffset(horizontalOffset_ - 16.0f);  // 默认16像素
}

void ScrollViewer::LineRight() {
    ScrollToHorizontalOffset(horizontalOffset_ + 16.0f);
}

void ScrollViewer::LineUp() {
    ScrollToVerticalOffset(verticalOffset_ - 16.0f);
}

void ScrollViewer::LineDown() {
    ScrollToVerticalOffset(verticalOffset_ + 16.0f);
}

void ScrollViewer::PageUp() {
    ScrollToVerticalOffset(verticalOffset_ - viewportHeight_);
}

void ScrollViewer::PageDown() {
    ScrollToVerticalOffset(verticalOffset_ + viewportHeight_);
}

void ScrollViewer::PageLeft() {
    ScrollToHorizontalOffset(horizontalOffset_ - viewportWidth_);
}

void ScrollViewer::PageRight() {
    ScrollToHorizontalOffset(horizontalOffset_ + viewportWidth_);
}

void ScrollViewer::ScrollToTop() {
    ScrollToVerticalOffset(0.0f);
}

void ScrollViewer::ScrollToBottom() {
    ScrollToVerticalOffset(extentHeight_);
}

void ScrollViewer::ScrollToLeftEnd() {
    ScrollToHorizontalOffset(0.0f);
}

void ScrollViewer::ScrollToRightEnd() {
    ScrollToHorizontalOffset(extentWidth_);
}

void ScrollViewer::ScrollToHorizontalOffset(float offset) {
    SetHorizontalOffset(offset);
}

void ScrollViewer::ScrollToVerticalOffset(float offset) {
    SetVerticalOffset(offset);
}

// ========== 布局 ==========

Size ScrollViewer::MeasureOverride(Size availableSize) {
    // 测量内容
    std::any contentAny = GetContent();
    if (contentAny.has_value()) {
        // 尝试转换为UIElement
        try {
            auto* content = std::any_cast<UIElement*>(contentAny);
            if (content) {
                // 给内容无限空间进行测量，以确定其实际大小
                Size infiniteSize(
                    std::numeric_limits<float>::infinity(),
                    std::numeric_limits<float>::infinity()
                );
                content->Measure(infiniteSize);
                
                Size contentSize = content->GetDesiredSize();
                extentWidth_ = contentSize.width;
                extentHeight_ = contentSize.height;
            }
        } catch (const std::bad_any_cast&) {
            extentWidth_ = 0.0f;
            extentHeight_ = 0.0f;
        }
    } else {
        extentWidth_ = 0.0f;
        extentHeight_ = 0.0f;
    }
    
    // 计算视口大小
    viewportWidth_ = availableSize.width;
    viewportHeight_ = availableSize.height;
    
    // 更新滚动条
    UpdateScrollBarsRange();
    UpdateScrollBarsVisibility();
    
    return availableSize;
}

Size ScrollViewer::ArrangeOverride(Size finalSize) {
    // 排列内容
    std::any contentAny = GetContent();
    if (contentAny.has_value()) {
        try {
            auto* content = std::any_cast<UIElement*>(contentAny);
            if (content) {
                // 内容按其实际大小排列
                Rect contentRect(
                    -horizontalOffset_,
                    -verticalOffset_,
                    std::max(extentWidth_, viewportWidth_),
                    std::max(extentHeight_, viewportHeight_)
                );
                content->Arrange(contentRect);
            }
        } catch (const std::bad_any_cast&) {
            // 忽略
        }
    }
    
    // 排列滚动条
    if (horizontalScrollBar_ && horizontalScrollBar_->GetVisibility() == Visibility::Visible) {
        Rect hScrollRect(0, finalSize.height - 20, finalSize.width - 20, 20);
        horizontalScrollBar_->Arrange(hScrollRect);
    }
    
    if (verticalScrollBar_ && verticalScrollBar_->GetVisibility() == Visibility::Visible) {
        Rect vScrollRect(finalSize.width - 20, 0, 20, finalSize.height - 20);
        verticalScrollBar_->Arrange(vScrollRect);
    }
    
    return finalSize;
}

// ========== 渲染 ==========

void ScrollViewer::OnRender(render::RenderContext& context) {
    // 绘制背景
    Size size = GetRenderSize();
    Rect rect(0, 0, size.width, size.height);
    auto white = Color::White();
    context.DrawBorder(rect, {white.r, white.g, white.b, white.a});
    
    // 设置裁剪区域
    Rect viewportRect = CalculateViewportRect();
    // TODO: 实现裁剪
    
    // 渲染内容
    // 内容渲染会由视觉树系统自动处理
    
    // 渲染滚动条
    if (horizontalScrollBar_ && horizontalScrollBar_->GetVisibility() == Visibility::Visible) {
        // TODO: 渲染水平滚动条
    }
    
    if (verticalScrollBar_ && verticalScrollBar_->GetVisibility() == Visibility::Visible) {
        // TODO: 渲染垂直滚动条
    }
}

// ========== 事件处理 ==========

void ScrollViewer::OnScrollChanged() {
    // 滚动变更事件
    // 可以在这里触发事件通知
}

void ScrollViewer::OnHorizontalScrollBarValueChanged(float oldValue, float newValue) {
    horizontalOffset_ = newValue;
    InvalidateArrange();
    InvalidateVisual();
}

void ScrollViewer::OnVerticalScrollBarValueChanged(float oldValue, float newValue) {
    verticalOffset_ = newValue;
    InvalidateArrange();
    InvalidateVisual();
}

// ========== 私有方法 ==========

void ScrollViewer::UpdateScrollBarsVisibility() {
    // 水平滚动条
    bool showHorizontal = false;
    switch (horizontalScrollBarVisibility_) {
        case ScrollBarVisibility::Disabled:
            showHorizontal = false;
            break;
        case ScrollBarVisibility::Auto:
            showHorizontal = (extentWidth_ > viewportWidth_);
            break;
        case ScrollBarVisibility::Hidden:
            showHorizontal = false;
            break;
        case ScrollBarVisibility::Visible:
            showHorizontal = true;
            break;
    }
    
    if (horizontalScrollBar_) {
        horizontalScrollBar_->SetVisibility(
            showHorizontal ? Visibility::Visible : Visibility::Collapsed
        );
    }
    
    // 垂直滚动条
    bool showVertical = false;
    switch (verticalScrollBarVisibility_) {
        case ScrollBarVisibility::Disabled:
            showVertical = false;
            break;
        case ScrollBarVisibility::Auto:
            showVertical = (extentHeight_ > viewportHeight_);
            break;
        case ScrollBarVisibility::Hidden:
            showVertical = false;
            break;
        case ScrollBarVisibility::Visible:
            showVertical = true;
            break;
    }
    
    if (verticalScrollBar_) {
        verticalScrollBar_->SetVisibility(
            showVertical ? Visibility::Visible : Visibility::Collapsed
        );
    }
}

void ScrollViewer::UpdateScrollBarsRange() {
    if (horizontalScrollBar_) {
        horizontalScrollBar_->SetMinimum(0.0f);
        horizontalScrollBar_->SetMaximum(std::max(0.0f, extentWidth_ - viewportWidth_));
        horizontalScrollBar_->SetViewportSize(viewportWidth_);
        horizontalScrollBar_->SetLargeChange(viewportWidth_);
    }
    
    if (verticalScrollBar_) {
        verticalScrollBar_->SetMinimum(0.0f);
        verticalScrollBar_->SetMaximum(std::max(0.0f, extentHeight_ - viewportHeight_));
        verticalScrollBar_->SetViewportSize(viewportHeight_);
        verticalScrollBar_->SetLargeChange(viewportHeight_);
    }
}

Rect ScrollViewer::CalculateViewportRect() const {
    return Rect(0, 0, viewportWidth_, viewportHeight_);
}

} // namespace fk::ui
