/**
 * @file ScrollBar.cpp
 * @brief ScrollBar 滚动条控件实现 - Phase 3 Step 4
 * 
 * 整合 Track、Thumb、RepeatButton 组件，创建完整的滚动条 UI
 * 
 * 实现状态：
 * - Phase 1: 基础架构和接口定义 ✓
 * - Phase 2: ScrollContentPresenter 实现 ✓
 * - Phase 3 Step 1-3: RepeatButton/Thumb/Track ✓
 * - Phase 3 Step 4: ScrollBar 整合 ✓（本文件）
 * - Phase 3 Step 5-6: 待实现
 */

#include "fk/ui/ScrollBar.h"
#include "fk/ui/Track.h"
#include "fk/ui/Thumb.h"
#include "fk/ui/RepeatButton.h"
#include <algorithm>

namespace fk::ui {

// ========== 构造函数 ==========

ScrollBar::ScrollBar() {
    // Phase 3 Step 4: 初始化组件
    InitializeComponents();
    ConnectEvents();
    SyncPropertiesToTrack();
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

// ========== 属性访问器 ==========

Orientation ScrollBar::GetOrientation() const {
    return orientation_;
}

ScrollBar* ScrollBar::SetOrientation(Orientation value) {
    orientation_ = value;
    if (track_) {
        track_->SetOrientation(value);
        InvalidateArrange();
    }
    return this;
}

float ScrollBar::GetMinimum() const {
    return minimum_;
}

ScrollBar* ScrollBar::SetMinimum(float value) {
    minimum_ = value;
    CoerceValue();
    if (track_) {
        track_->SetMinimum(value);
    }
    return this;
}

float ScrollBar::GetMaximum() const {
    return maximum_;
}

ScrollBar* ScrollBar::SetMaximum(float value) {
    maximum_ = value;
    CoerceValue();
    if (track_) {
        track_->SetMaximum(value);
    }
    return this;
}

float ScrollBar::GetValue() const {
    return value_;
}

ScrollBar* ScrollBar::SetValue(float value) {
    float oldValue = value_;
    value_ = value;
    CoerceValue();  // 使用统一的值强制逻辑
    if (track_) {
        track_->SetValue(value_);
    }
    if (value_ != oldValue) {
        OnValueChanged(oldValue, value_);
    }
    return this;
}

float ScrollBar::GetViewportSize() const {
    return viewportSize_;
}

ScrollBar* ScrollBar::SetViewportSize(float value) {
    viewportSize_ = value;
    if (track_) {
        track_->SetViewportSize(value);
        InvalidateArrange();  // 重新布局以更新 Thumb 大小
    }
    return this;
}

float ScrollBar::GetSmallChange() const {
    return smallChange_;
}

ScrollBar* ScrollBar::SetSmallChange(float value) {
    smallChange_ = value;
    return this;
}

float ScrollBar::GetLargeChange() const {
    return largeChange_;
}

ScrollBar* ScrollBar::SetLargeChange(float value) {
    largeChange_ = value;
    return this;
}

// ========== 命令方法（Stub）==========

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

// ========== 事件处理 ==========

void ScrollBar::OnValueChanged(float oldValue, float newValue) {
    ValueChanged(oldValue, newValue);
    // Track 会自动根据 Value 更新 Thumb 位置
    // TODO Phase 4: 触发路由事件
}

// ========== 私有方法 ==========

void ScrollBar::CoerceValue() {
    value_ = std::max(minimum_, std::min(maximum_, value_));
}

// ========== Phase 3 Step 4: 组件初始化和事件连接 ==========

void ScrollBar::InitializeComponents() {
    // 创建组件
    track_ = new Track();
    thumb_ = new Thumb();
    decreaseButton_ = new RepeatButton();
    increaseButton_ = new RepeatButton();
    
    // 配置 Track
    track_->SetThumb(thumb_);
    track_->SetDecreaseRepeatButton(decreaseButton_);
    track_->SetIncreaseRepeatButton(increaseButton_);
    
    // TODO: 将 track_ 添加到可视树
    // AddVisualChild(track_);
}

void ScrollBar::ConnectEvents() {
    if (!thumb_ || !decreaseButton_ || !increaseButton_ || !track_) {
        return;
    }
    
    // Thumb 拖动事件
    thumb_->DragDelta.Connect([this](const DragDeltaEventArgs& args) {
        float newValue = track_->ValueFromDistance(args.horizontalChange, args.verticalChange);
        SetValue(newValue);
    });
    
    // Decrease 按钮点击事件
    decreaseButton_->Click.Connect([this]() {
        LineUp();
    });
    
    // Increase 按钮点击事件
    increaseButton_->Click.Connect([this]() {
        LineDown();
    });
}

void ScrollBar::SyncPropertiesToTrack() {
    if (!track_) {
        return;
    }
    
    track_->SetOrientation(orientation_);
    track_->SetMinimum(minimum_);
    track_->SetMaximum(maximum_);
    track_->SetValue(value_);
    track_->SetViewportSize(viewportSize_);
}

// ========== Phase 3 Step 4: 布局重写 ==========

Size ScrollBar::MeasureOverride(const Size& availableSize) {
    if (track_) {
        track_->Measure(availableSize);
        return track_->GetDesiredSize();
    }
    return Size(0, 0);
}

Size ScrollBar::ArrangeOverride(const Size& finalSize) {
    if (track_) {
        track_->Arrange(Rect(0, 0, finalSize.width, finalSize.height));
    }
    return finalSize;
}

} // namespace fk::ui
