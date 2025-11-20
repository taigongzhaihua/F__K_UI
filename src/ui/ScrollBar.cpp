/**
 * @file ScrollBar.cpp
 * @brief ScrollBar 滚动条控件实现
 */

#include "fk/ui/ScrollBar.h"
#include "fk/ui/DrawCommand.h"
#include "fk/render/RenderContext.h"
#include <algorithm>

namespace fk::ui {

// ========== 构造函数 ==========

ScrollBar::ScrollBar() {
    SetWidth(20.0f);  // 默认宽度
    SetHeight(100.0f);  // 默认高度
}

// ========== 依赖属性定义 ==========

const binding::DependencyProperty& ScrollBar::OrientationProperty() {
    static auto& property = 
        binding::DependencyProperty::Register(
            "Orientation",
            typeid(Orientation),
            typeid(ScrollBar),
            binding::PropertyMetadata{std::any(Orientation::Vertical)}
        );
    return property;
}

const binding::DependencyProperty& ScrollBar::MinimumProperty() {
    static auto& property = 
        binding::DependencyProperty::Register(
            "Minimum",
            typeid(float),
            typeid(ScrollBar),
            binding::PropertyMetadata{std::any(0.0f)}
        );
    return property;
}

const binding::DependencyProperty& ScrollBar::MaximumProperty() {
    static auto& property = 
        binding::DependencyProperty::Register(
            "Maximum",
            typeid(float),
            typeid(ScrollBar),
            binding::PropertyMetadata{std::any(100.0f)}
        );
    return property;
}

const binding::DependencyProperty& ScrollBar::ValueProperty() {
    static auto& property = 
        binding::DependencyProperty::Register(
            "Value",
            typeid(float),
            typeid(ScrollBar),
            binding::PropertyMetadata{std::any(0.0f)}
        );
    return property;
}

const binding::DependencyProperty& ScrollBar::ViewportSizeProperty() {
    static auto& property = 
        binding::DependencyProperty::Register(
            "ViewportSize",
            typeid(float),
            typeid(ScrollBar),
            binding::PropertyMetadata{std::any(10.0f)}
        );
    return property;
}

const binding::DependencyProperty& ScrollBar::SmallChangeProperty() {
    static auto& property = 
        binding::DependencyProperty::Register(
            "SmallChange",
            typeid(float),
            typeid(ScrollBar),
            binding::PropertyMetadata{std::any(1.0f)}
        );
    return property;
}

const binding::DependencyProperty& ScrollBar::LargeChangeProperty() {
    static auto& property = 
        binding::DependencyProperty::Register(
            "LargeChange",
            typeid(float),
            typeid(ScrollBar),
            binding::PropertyMetadata{std::any(10.0f)}
        );
    return property;
}

// ========== 滚动操作 ==========

void ScrollBar::LineUp() {
    SetValue(value_ - smallChange_);
}

void ScrollBar::LineDown() {
    SetValue(value_ + smallChange_);
}

void ScrollBar::PageUp() {
    SetValue(value_ - largeChange_);
}

void ScrollBar::PageDown() {
    SetValue(value_ + largeChange_);
}

void ScrollBar::ScrollToStart() {
    SetValue(minimum_);
}

void ScrollBar::ScrollToEnd() {
    SetValue(maximum_);
}

// ========== 布局 ==========

Size ScrollBar::MeasureOverride(Size availableSize) {
    if (orientation_ == Orientation::Vertical) {
        return Size(20.0f, availableSize.height);
    } else {
        return Size(availableSize.width, 20.0f);
    }
}

Size ScrollBar::ArrangeOverride(Size finalSize) {
    return finalSize;
}

// ========== 渲染 ==========

void ScrollBar::OnRender(render::RenderContext& context) {
    Size size = GetRenderSize();
    
    // 绘制背景
    Rect bgRect(0, 0, size.width, size.height);
    context.DrawRectangle(bgRect, {0.9f, 0.9f, 0.9f, 1.0f});
    
    // 计算滑块矩形
    Rect thumbRect = CalculateThumbRect(size);
    
    // 绘制滑块
    context.DrawRectangle(thumbRect, {0.6f, 0.6f, 0.6f, 1.0f});
}

// ========== 事件处理 ==========

void ScrollBar::OnValueChanged(float oldValue, float newValue) {
    ValueChanged(oldValue, newValue);
    InvalidateVisual();
}

// ========== 私有方法 ==========

void ScrollBar::CoerceValue() {
    value_ = std::max(minimum_, std::min(maximum_, value_));
}

Rect ScrollBar::CalculateThumbRect(Size containerSize) const {
    float range = maximum_ - minimum_;
    if (range <= 0.0f) {
        return Rect(0, 0, 0, 0);
    }
    
    if (orientation_ == Orientation::Vertical) {
        // 垂直滚动条
        float trackHeight = containerSize.height;
        float thumbHeight = std::max(20.0f, (viewportSize_ / (range + viewportSize_)) * trackHeight);
        float thumbY = ((value_ - minimum_) / range) * (trackHeight - thumbHeight);
        
        return Rect(0, thumbY, containerSize.width, thumbHeight);
    } else {
        // 水平滚动条
        float trackWidth = containerSize.width;
        float thumbWidth = std::max(20.0f, (viewportSize_ / (range + viewportSize_)) * trackWidth);
        float thumbX = ((value_ - minimum_) / range) * (trackWidth - thumbWidth);
        
        return Rect(thumbX, 0, thumbWidth, containerSize.height);
    }
}

} // namespace fk::ui
