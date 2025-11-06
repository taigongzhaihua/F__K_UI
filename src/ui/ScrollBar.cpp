#include "fk/ui/ScrollBar.h"
#include "fk/binding/DependencyProperty.h"

#include <algorithm>
#include <cmath>
#include <limits>

namespace fk::ui::detail {

// ============================================================================
// ScrollBarBase 实现
// ============================================================================

ScrollBarBase::ScrollBarBase() {
    ApplyOrientationLayout(GetOrientation());
    // 显式设置默认厚度
    SetThickness(8.0f);
}

ScrollBarBase::~ScrollBarBase() = default;

// ============================================================================
// 依赖属性定义（使用宏）
// ============================================================================

FK_DEPENDENCY_PROPERTY_DEFINE(ScrollBarBase, Minimum, double, 0.0)
FK_DEPENDENCY_PROPERTY_DEFINE(ScrollBarBase, Maximum, double, 100.0)
FK_DEPENDENCY_PROPERTY_DEFINE(ScrollBarBase, Value, double, 0.0)
FK_DEPENDENCY_PROPERTY_DEFINE(ScrollBarBase, ViewportSize, double, 0.0)
FK_DEPENDENCY_PROPERTY_DEFINE(ScrollBarBase, Orientation, ui::Orientation, ui::Orientation::Vertical)
FK_DEPENDENCY_PROPERTY_DEFINE_REF(ScrollBarBase, ThumbBrush, std::string)
FK_DEPENDENCY_PROPERTY_DEFINE_REF(ScrollBarBase, TrackBrush, std::string)
FK_DEPENDENCY_PROPERTY_DEFINE(ScrollBarBase, Thickness, float, 8.0f)

// ============================================================================
// 布局
// ============================================================================

Size ScrollBarBase::MeasureOverride(const Size& availableSize) {
    auto orientation = GetOrientation();
    float thickness = GetThickness();
    
    if (orientation == ui::Orientation::Vertical) {
        // 垂直滚动条: 宽度使用 Thickness, 高度填满可用空间
        float width = thickness;
        float height = std::isinf(availableSize.height) ? 100.0f : availableSize.height;
        return Size(width, height);
    }
    
    // 水平滚动条: 高度使用 Thickness, 宽度填满可用空间
    float height = thickness;
    float width = std::isinf(availableSize.width) ? 100.0f : availableSize.width;
    return Size(width, height);
}

Size ScrollBarBase::ArrangeOverride(const Size& finalSize) {
    return finalSize;
}

bool ScrollBarBase::HasRenderContent() const {
    return true;  // ScrollBar 始终有渲染内容
}

// ============================================================================
// 鼠标事件
// ============================================================================

bool ScrollBarBase::OnMouseButtonDown(int button, double x, double y) {
    if (button == 0) {  // 左键
        // x, y 已经是局部坐标（由父元素转换过）
        HandleMouseDown(static_cast<float>(x), static_cast<float>(y));
        return true; // ScrollBar 处理了事件
    }
    return false;
}

bool ScrollBarBase::OnMouseButtonUp(int button, double x, double y) {
    if (button == 0) {  // 左键
        bool wasDragging = isDragging_;
        HandleMouseUp();
        return wasDragging; // 只有在拖动时才算处理了事件
    }
    return false;
}

bool ScrollBarBase::OnMouseMove(double x, double y) {
    // x, y 已经是局部坐标（由父元素转换过）
    HandleMouseMove(static_cast<float>(x), static_cast<float>(y));
    return isDragging_; // 只有在拖动时才算处理了事件
}

// ============================================================================
// 辅助方法
// ============================================================================

void ScrollBarBase::CoerceValue() {
    double min = GetMinimum();
    double max = GetMaximum();
    double val = GetValue();
    
    // 限制 Value �?[Minimum, Maximum] 范围�?
    if (val < min) {
        SetValue(min);
    } else if (val > max) {
        SetValue(max);
    }
}

Rect ScrollBarBase::GetThumbBounds() const {
    double min = GetMinimum();
    double max = GetMaximum();
    double val = GetValue();
    double viewportSize = GetViewportSize();
    auto orientation = GetOrientation();
    
    auto bounds = GetRenderBounds();
    float trackLength = (orientation == ui::Orientation::Vertical) ? bounds.height : bounds.width;
    float trackBreadth = (orientation == ui::Orientation::Vertical) ? bounds.width : bounds.height;
    
    // 如果没有滚动范围,滑块填满整个轨道
    double range = max - min;
    if (range <= 0.0) {
        if (orientation == ui::Orientation::Vertical) {
            return Rect(0, 0, trackBreadth, trackLength);
        } else {
            return Rect(0, 0, trackLength, trackBreadth);
        }
    }
    
    // 计算 Thumb 大小: viewportSize / (viewportSize + range)
    // 例如: viewport=150, extent=322, range=172
    // thumbRatio = 150 / 322 = 0.466 (46.6% 的轨道长度)
    double extentSize = viewportSize + range;
    double thumbRatio = viewportSize / extentSize;
    float thumbLength = static_cast<float>(thumbRatio * trackLength);
    
    // 限制 Thumb 最小大小(至少 20px)
    thumbLength = std::max(thumbLength, 20.0f);
    // 确保 Thumb 不超过轨道长度
    thumbLength = std::min(thumbLength, trackLength);
    
    // 计算 Thumb 位置: (val / range) * (trackLength - thumbLength)
    // 确保滑块不会超出轨道
    double valueRatio = std::clamp((val - min) / range, 0.0, 1.0);
    float availableTrack = trackLength - thumbLength;
    float thumbPosition = static_cast<float>(valueRatio * availableTrack);
    
    // 确保位置在有效范围内
    thumbPosition = std::clamp(thumbPosition, 0.0f, availableTrack);
    
    if (orientation == ui::Orientation::Vertical) {
        return Rect(0, thumbPosition, trackBreadth, thumbLength);
    } else {
        return Rect(thumbPosition, 0, thumbLength, trackBreadth);
    }
}

double ScrollBarBase::PointToValue(float x, float y) const {
    double min = GetMinimum();
    double max = GetMaximum();
    double viewportSize = GetViewportSize();
    auto orientation = GetOrientation();
    
    auto bounds = GetRenderBounds();
    auto thumbBounds = GetThumbBounds();
    
    float trackLength = (orientation == ui::Orientation::Vertical) ? bounds.height : bounds.width;
    float thumbLength = (orientation == ui::Orientation::Vertical) ? thumbBounds.height : thumbBounds.width;
    float clickPosition = (orientation == ui::Orientation::Vertical) ? y : x;
    
    // 计算可用轨道长度 (减去 Thumb 大小)
    float availableTrack = trackLength - thumbLength;
    if (availableTrack <= 0.0f) {
        return min;
    }
    
    // 将位置映射到�?
    double ratio = clickPosition / availableTrack;
    ratio = std::clamp(ratio, 0.0, 1.0);
    
    return min + ratio * (max - min);
}

// ============================================================================
// 鼠标事件处理
// ============================================================================

void ScrollBarBase::HandleMouseDown(float x, float y) {
    auto thumbBounds = GetThumbBounds();
    
    // 检查是否点击在 Thumb 上
    if (x >= thumbBounds.x && x <= thumbBounds.x + thumbBounds.width &&
        y >= thumbBounds.y && y <= thumbBounds.y + thumbBounds.height) {
        // 开始拖动 Thumb
        isDragging_ = true;
        dragStartX_ = x;
        dragStartY_ = y;
        dragStartValue_ = GetValue();
    } else {
        // 点击轨道,跳转到该位置
        double newValue = PointToValue(x, y);
        SetValue(newValue);
    }
}

void ScrollBarBase::HandleMouseMove(float x, float y) {
    if (!isDragging_) return;
    
    auto orientation = GetOrientation();
    auto bounds = GetRenderBounds();
    auto thumbBounds = GetThumbBounds();
    
    float trackLength = (orientation == ui::Orientation::Vertical) ? bounds.height : bounds.width;
    float thumbLength = (orientation == ui::Orientation::Vertical) ? thumbBounds.height : thumbBounds.width;
    float delta = (orientation == ui::Orientation::Vertical) ? (y - dragStartY_) : (x - dragStartX_);
    
    // 计算值的变化
    double range = GetMaximum() - GetMinimum();
    float availableTrack = trackLength - thumbLength;
    
    if (availableTrack > 0.0f) {
        double deltaValue = (delta / availableTrack) * range;
        double newValue = dragStartValue_ + deltaValue;
        SetValue(newValue);
    }
}

void ScrollBarBase::HandleMouseUp() {
    isDragging_ = false;
}

// ============================================================================
// 属性变更回�?
// ============================================================================

void ScrollBarBase::OnMinimumChanged(double oldValue, double newValue) {
    CoerceValue();
    InvalidateMeasure();
    InvalidateVisual();
}

void ScrollBarBase::OnMaximumChanged(double oldValue, double newValue) {
    CoerceValue();
    InvalidateMeasure();
    InvalidateVisual();
}

void ScrollBarBase::OnValueChanged(double oldValue, double newValue) {
    // 限制值在有效范围�?
    double min = GetMinimum();
    double max = GetMaximum();
    double coercedValue = std::clamp(newValue, min, max);
    
    if (coercedValue != newValue) {
        // 值被修正,重新设置
        binding::DependencyObject::SetValue(ValueProperty(), coercedValue);
        return;
    }
    
    // 触发事件
    ValueChanged(newValue);
    InvalidateVisual();
}

void ScrollBarBase::OnViewportSizeChanged(double oldValue, double newValue) {
    InvalidateMeasure();
    InvalidateVisual();
}

void ScrollBarBase::OnOrientationChanged(ui::Orientation oldValue, ui::Orientation newValue) {
    ApplyOrientationLayout(newValue);
    InvalidateMeasure();
    InvalidateVisual();
}

void ScrollBarBase::ApplyOrientationLayout(ui::Orientation orientation) {
    const float thickness = GetThickness();
    const float unset = std::numeric_limits<float>::quiet_NaN();

    if (orientation == ui::Orientation::Vertical) {
        SetWidth(thickness);
        SetHeight(unset);
        SetHorizontalAlignment(ui::HorizontalAlignment::Right);
        SetVerticalAlignment(ui::VerticalAlignment::Stretch);
    } else {
        SetHeight(thickness);
        SetWidth(unset);
        SetHorizontalAlignment(ui::HorizontalAlignment::Stretch);
        SetVerticalAlignment(ui::VerticalAlignment::Bottom);
    }
}

void ScrollBarBase::OnThicknessChanged(float oldValue, float newValue) {
    ApplyOrientationLayout(GetOrientation());
    InvalidateMeasure();
    InvalidateVisual();
}

void ScrollBarBase::OnThumbBrushChanged(const std::string& oldValue, const std::string& newValue) {
    InvalidateVisual();
}

void ScrollBarBase::OnTrackBrushChanged(const std::string& oldValue, const std::string& newValue) {
    InvalidateVisual();
}

// ============================================================================
// 元数据构�?
// ============================================================================

binding::PropertyMetadata ScrollBarBase::BuildMinimumMetadata() {
    binding::PropertyMetadata metadata(0.0);
    metadata.propertyChangedCallback = MinimumPropertyChanged;
    return metadata;
}

binding::PropertyMetadata ScrollBarBase::BuildMaximumMetadata() {
    binding::PropertyMetadata metadata(100.0);
    metadata.propertyChangedCallback = MaximumPropertyChanged;
    return metadata;
}

binding::PropertyMetadata ScrollBarBase::BuildValueMetadata() {
    binding::PropertyMetadata metadata(0.0);
    metadata.propertyChangedCallback = ValuePropertyChanged;
    return metadata;
}

binding::PropertyMetadata ScrollBarBase::BuildViewportSizeMetadata() {
    binding::PropertyMetadata metadata(0.0);
    metadata.propertyChangedCallback = ViewportSizePropertyChanged;
    return metadata;
}

binding::PropertyMetadata ScrollBarBase::BuildOrientationMetadata() {
    binding::PropertyMetadata metadata(ui::Orientation::Vertical);
    metadata.propertyChangedCallback = OrientationPropertyChanged;
    return metadata;
}

binding::PropertyMetadata ScrollBarBase::BuildThumbBrushMetadata() {
    binding::PropertyMetadata metadata(std::string("#606060"));  // 默认灰色
    metadata.propertyChangedCallback = ThumbBrushPropertyChanged;
    return metadata;
}

binding::PropertyMetadata ScrollBarBase::BuildTrackBrushMetadata() {
    binding::PropertyMetadata metadata(std::string("#202020"));  // 默认深灰色
    metadata.propertyChangedCallback = TrackBrushPropertyChanged;
    return metadata;
}

binding::PropertyMetadata ScrollBarBase::BuildThicknessMetadata() {
    binding::PropertyMetadata metadata(8.0f);  // 默认厚度 8px
    metadata.propertyChangedCallback = ThicknessPropertyChanged;
    return metadata;
}

// ============================================================================
// 验证回调
// ============================================================================

bool ScrollBarBase::ValidateValue(const std::any& value) {
    try {
        std::any_cast<double>(value);
        return true;
    } catch (...) {
        return false;
    }
}

} // namespace fk::ui::detail
