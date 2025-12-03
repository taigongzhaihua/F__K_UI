/**
 * @file Track.cpp
 * @brief Track 轨道布局组件实现
 */

#include "fk/ui/scrolling/Track.h"
#include "fk/binding/DependencyProperty.h"
#include <algorithm>
#include <cmath>
#include <limits>

namespace fk::ui {

// ========== 模板显式实例�?==========
template class FrameworkElement<Track>;

// ========== 依赖属性定�?==========

const binding::DependencyProperty& Track::OrientationProperty() {
    static auto& prop = binding::DependencyProperty::Register(
        "Orientation",
        typeid(ui::Orientation),
        typeid(Track),
        binding::PropertyMetadata{std::any(ui::Orientation::Vertical)}
    );
    return prop;
}

const binding::DependencyProperty& Track::MinimumProperty() {
    static auto& prop = binding::DependencyProperty::Register(
        "Minimum",
        typeid(float),
        typeid(Track),
        binding::PropertyMetadata{std::any(0.0f)}
    );
    return prop;
}

const binding::DependencyProperty& Track::MaximumProperty() {
    static auto& prop = binding::DependencyProperty::Register(
        "Maximum",
        typeid(float),
        typeid(Track),
        binding::PropertyMetadata{std::any(100.0f)}
    );
    return prop;
}

const binding::DependencyProperty& Track::ValueProperty() {
    static auto& prop = binding::DependencyProperty::Register(
        "Value",
        typeid(float),
        typeid(Track),
        binding::PropertyMetadata{
            std::any(0.0f),
            [](DependencyObject& obj, const DependencyProperty&, const std::any& oldValue, const std::any& newValue) {
                auto& track = static_cast<Track&>(obj);
                float oldVal = std::any_cast<float>(oldValue);
                float newVal = std::any_cast<float>(newValue);
                track.InvalidateArrange();
                track.ValueChanged(oldVal, newVal);  // 使用 operator() 调用事件
            }
        }
    );
    return prop;
}

const binding::DependencyProperty& Track::ViewportSizeProperty() {
    static auto& prop = binding::DependencyProperty::Register(
        "ViewportSize",
        typeid(float),
        typeid(Track),
        binding::PropertyMetadata{std::any(std::numeric_limits<float>::quiet_NaN())}
    );
    return prop;
}

const binding::DependencyProperty& Track::IsDirectionReversedProperty() {
    static auto& prop = binding::DependencyProperty::Register(
        "IsDirectionReversed",
        typeid(bool),
        typeid(Track),
        binding::PropertyMetadata{std::any(false)}
    );
    return prop;
}

// ========== 构�?析构 ==========

Track::Track() {
    // 确保依赖属性已注册
    OrientationProperty();
    MinimumProperty();
    MaximumProperty();
    ValueProperty();
    ViewportSizeProperty();
    IsDirectionReversedProperty();
}

Track::~Track() {
    UnbindComponentEvents();
}

// ========== 属性访问器 ==========

ui::Orientation Track::GetOrientation() const {
    return DependencyObject::GetValue<ui::Orientation>(OrientationProperty());
}

Track* Track::SetOrientation(ui::Orientation value) {
    DependencyObject::SetValue(OrientationProperty(), value);
    return this;
}

float Track::GetMinimum() const {
    return DependencyObject::GetValue<float>(MinimumProperty());
}

Track* Track::SetMinimum(float value) {
    DependencyObject::SetValue(MinimumProperty(), value);
    return this;
}

float Track::GetMaximum() const {
    return DependencyObject::GetValue<float>(MaximumProperty());
}

Track* Track::SetMaximum(float value) {
    DependencyObject::SetValue(MaximumProperty(), value);
    return this;
}

float Track::GetValue() const {
    return DependencyObject::GetValue<float>(ValueProperty());
}

Track* Track::SetValue(float value) {
    float min = GetMinimum();
    float max = GetMaximum();
    float clamped = std::clamp(value, min, max);
    DependencyObject::SetValue(ValueProperty(), clamped);
    return this;
}

float Track::GetViewportSize() const {
    return DependencyObject::GetValue<float>(ViewportSizeProperty());
}

Track* Track::SetViewportSize(float value) {
    DependencyObject::SetValue(ViewportSizeProperty(), value);
    return this;
}

bool Track::GetIsDirectionReversed() const {
    return DependencyObject::GetValue<bool>(IsDirectionReversedProperty());
}

Track* Track::SetIsDirectionReversed(bool value) {
    DependencyObject::SetValue(IsDirectionReversedProperty(), value);
    return this;
}

// ========== 组件设置 ==========

Track* Track::SetDecreaseRepeatButton(RepeatButton* button) {
    if (decreaseRepeatButton_ == button) return this;
    
    // 移除旧组�?
    if (decreaseRepeatButton_) {
        RemoveVisualChild(decreaseRepeatButton_);
    }
    
    decreaseRepeatButton_ = button;
    
    // 添加新组�?
    if (decreaseRepeatButton_) {
        AddVisualChild(decreaseRepeatButton_);
        TakeOwnership(decreaseRepeatButton_);
        
        // 绑定点击事件
        decreaseRepeatButton_->Click += [this]() {
            OnDecreaseButtonClick();
        };
    }
    
    InvalidateMeasure();
    return this;
}

Track* Track::SetIncreaseRepeatButton(RepeatButton* button) {
    if (increaseRepeatButton_ == button) return this;
    
    // 移除旧组�?
    if (increaseRepeatButton_) {
        RemoveVisualChild(increaseRepeatButton_);
    }
    
    increaseRepeatButton_ = button;
    
    // 添加新组�?
    if (increaseRepeatButton_) {
        AddVisualChild(increaseRepeatButton_);
        TakeOwnership(increaseRepeatButton_);
        
        // 绑定点击事件
        increaseRepeatButton_->Click += [this]() {
            OnIncreaseButtonClick();
        };
    }
    
    InvalidateMeasure();
    return this;
}

Track* Track::SetThumb(Thumb* thumb) {
    if (thumb_ == thumb) return this;
    
    // 移除旧组�?
    if (thumb_) {
        RemoveVisualChild(thumb_);
    }
    
    thumb_ = thumb;
    
    // 添加新组�?
    if (thumb_) {
        AddVisualChild(thumb_);
        TakeOwnership(thumb_);
        
        // 绑定拖动事件
        thumb_->DragDelta += [this](const DragDeltaEventArgs& e) {
            OnThumbDragDelta(e);
        };
    }
    
    InvalidateMeasure();
    return this;
}

// ========== 布局实现 ==========

Size Track::MeasureOverride(const Size& availableSize) {
    Size desiredSize{0, 0};
    
    // 测量所有子组件
    if (decreaseRepeatButton_) {
        decreaseRepeatButton_->Measure(availableSize);
    }
    
    if (thumb_) {
        thumb_->Measure(availableSize);
        // 使用 Thumb 的期望尺寸作为次轴尺寸参�?
        auto thumbDesired = thumb_->GetDesiredSize();
        if (GetOrientation() == ui::Orientation::Horizontal) {
            desiredSize.height = std::max(desiredSize.height, thumbDesired.height);
        } else {
            desiredSize.width = std::max(desiredSize.width, thumbDesired.width);
        }
    }
    
    if (increaseRepeatButton_) {
        increaseRepeatButton_->Measure(availableSize);
    }
    
    // Track 本身不请求主轴尺寸，由父元素决定
    return desiredSize;
}

Size Track::ArrangeOverride(const Size& finalSize) {
    bool isHorizontal = (GetOrientation() == ui::Orientation::Horizontal);
    float trackLength = isHorizontal ? finalSize.width : finalSize.height;
    float crossSize = isHorizontal ? finalSize.height : finalSize.width;
    
    // 计算 Thumb 大小和位�?
    float thumbSize = CalculateThumbSize(trackLength);
    float thumbOffset = CalculateThumbOffset(trackLength, thumbSize);
    
    if (isHorizontal) {
        // 水平方向布局
        // DecreaseButton: �?0 �?thumbOffset
        if (decreaseRepeatButton_) {
            float width = std::max(0.0f, thumbOffset);
            decreaseRepeatButton_->Arrange(Rect{0, 0, width, crossSize});
        }
        
        // Thumb: �?thumbOffset 开�?
        if (thumb_) {
            thumb_->Arrange(Rect{thumbOffset, 0, thumbSize, crossSize});
        }
        
        // IncreaseButton: �?thumbOffset + thumbSize 到末�?
        if (increaseRepeatButton_) {
            float start = thumbOffset + thumbSize;
            float width = std::max(0.0f, trackLength - start);
            increaseRepeatButton_->Arrange(Rect{start, 0, width, crossSize});
        }
    } else {
        // 垂直方向布局
        // DecreaseButton: �?0 �?thumbOffset
        if (decreaseRepeatButton_) {
            float height = std::max(0.0f, thumbOffset);
            decreaseRepeatButton_->Arrange(Rect{0, 0, crossSize, height});
        }
        
        // Thumb: �?thumbOffset 开�?
        if (thumb_) {
            thumb_->Arrange(Rect{0, thumbOffset, crossSize, thumbSize});
        }
        
        // IncreaseButton: �?thumbOffset + thumbSize 到末�?
        if (increaseRepeatButton_) {
            float start = thumbOffset + thumbSize;
            float height = std::max(0.0f, trackLength - start);
            increaseRepeatButton_->Arrange(Rect{0, start, crossSize, height});
        }
    }
    
    return finalSize;
}

// ========== 布局计算 ==========

float Track::CalculateThumbSize(float trackLength) {
    float viewportSize = GetViewportSize();
    float range = GetMaximum() - GetMinimum();
    
    // 如果没有有效�?ViewportSize（如 Slider），使用固定大小
    if (std::isnan(viewportSize) || viewportSize <= 0 || range <= 0) {
        return std::min(DefaultThumbSize, trackLength / 3.0f);
    }
    
    // ScrollBar 模式：Thumb 大小�?ViewportSize 成比�?
    float total = range + viewportSize;
    float ratio = viewportSize / total;
    float thumbSize = trackLength * ratio;
    
    // 限制最小尺�?
    return std::max(thumbSize, MinThumbSize);
}

float Track::CalculateThumbOffset(float trackLength, float thumbSize) {
    float range = GetMaximum() - GetMinimum();
    if (range <= 0) return 0.0f;
    
    float availableLength = trackLength - thumbSize;
    if (availableLength <= 0) return 0.0f;
    
    float value = GetValue();
    float min = GetMinimum();
    float valueRatio = (value - min) / range;
    
    // 限制在有效范围内
    valueRatio = std::clamp(valueRatio, 0.0f, 1.0f);
    
    if (GetIsDirectionReversed()) {
        return availableLength * (1.0f - valueRatio);
    }
    return availableLength * valueRatio;
}

// ========== 工具方法 ==========

float Track::ValueFromDistance(float horizontal, float vertical) {
    bool isHorizontal = (GetOrientation() == ui::Orientation::Horizontal);
    Size renderSize = GetRenderSize();
    float trackLength = isHorizontal ? renderSize.width : renderSize.height;
    float thumbSize = CalculateThumbSize(trackLength);
    float availableLength = trackLength - thumbSize;
    
    if (availableLength <= 0) return 0.0f;
    
    float range = GetMaximum() - GetMinimum();
    if (range <= 0) return 0.0f;
    
    float distance = isHorizontal ? horizontal : vertical;
    float valueDelta = (distance / availableLength) * range;
    
    return GetIsDirectionReversed() ? -valueDelta : valueDelta;
}

// ========== 事件处理 ==========

void Track::OnThumbDragDelta(const DragDeltaEventArgs& e) {
    float valueDelta = ValueFromDistance(e.horizontalChange, e.verticalChange);
    float currentValue = GetValue();
    float newValue = currentValue + valueDelta;
    
    // SetValue 内部会做 clamp
    SetValue(newValue);
}

void Track::OnDecreaseButtonClick() {
    float change = GetLargeChange();
    float newValue = GetValue() - change;
    SetValue(newValue);
}

void Track::OnIncreaseButtonClick() {
    float change = GetLargeChange();
    float newValue = GetValue() + change;
    SetValue(newValue);
}

float Track::GetLargeChange() const {
    float viewportSize = GetViewportSize();
    if (!std::isnan(viewportSize) && viewportSize > 0) {
        return viewportSize;
    }
    // 默认使用范围�?10%
    float range = GetMaximum() - GetMinimum();
    return range * 0.1f;
}

// ========== 辅助方法 ==========

void Track::BindComponentEvents() {
    // 事件绑定�?SetXxx 方法中完�?
}

void Track::UnbindComponentEvents() {
    // 事件解绑在组件销毁时自动处理（通过 TakeOwnership�?
}

} // namespace fk::ui
