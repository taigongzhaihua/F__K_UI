#include "fk/animation/PointAnimation.h"

namespace fk::animation {

PointAnimation::PointAnimation() {
}

PointAnimation::PointAnimation(const ui::Point& fromPoint, const ui::Point& toPoint, Duration duration) {
    SetFrom(fromPoint);
    SetTo(toPoint);
    SetDuration(duration);
}

void PointAnimation::SetTarget(binding::DependencyObject* target, 
                                const binding::DependencyProperty* property) {
    target_ = target;
    targetProperty_ = property;
    hasInitialValue_ = false;
}

ui::Point PointAnimation::Interpolate(const ui::Point& from, const ui::Point& to, double progress) const {
    // 线性插值 X 和 Y 坐标
    float x = from.x + (to.x - from.x) * static_cast<float>(progress);
    float y = from.y + (to.y - from.y) * static_cast<float>(progress);
    
    return ui::Point(x, y);
}

ui::Point PointAnimation::Add(const ui::Point& value1, const ui::Point& value2) const {
    return ui::Point(value1.x + value2.x, value1.y + value2.y);
}

void PointAnimation::UpdateCurrentValue(double progress) {
    if (!target_ || !targetProperty_) {
        return;
    }

    // 首次更新时，保存初始值
    if (!hasInitialValue_) {
        try {
            auto value = target_->GetValue(*targetProperty_);
            if (value.has_value()) {
                initialValue_ = std::any_cast<ui::Point>(value);
            }
        } catch (...) {
            initialValue_ = ui::Point{0.0f, 0.0f};
        }
        hasInitialValue_ = true;
    }

    // 计算当前值
    ui::Point currentValue = GetCurrentValue(initialValue_, GetTo(), progress);
    
    // 应用到目标属性
    target_->SetValue(*targetProperty_, currentValue);
}

} // namespace fk::animation
