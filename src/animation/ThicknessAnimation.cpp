#include "fk/animation/ThicknessAnimation.h"

namespace fk::animation {

ThicknessAnimation::ThicknessAnimation() {
}

ThicknessAnimation::ThicknessAnimation(const Thickness& fromValue, const Thickness& toValue, Duration duration) {
    SetFrom(fromValue);
    SetTo(toValue);
    SetDuration(duration);
}

void ThicknessAnimation::SetTarget(binding::DependencyObject* target, 
                                    const binding::DependencyProperty* property) {
    target_ = target;
    targetProperty_ = property;
    hasInitialValue_ = false;
}

Thickness ThicknessAnimation::Interpolate(const Thickness& from, const Thickness& to, double progress) const {
    // 线性插值每个边的厚度
    float left = from.left + (to.left - from.left) * static_cast<float>(progress);
    float top = from.top + (to.top - from.top) * static_cast<float>(progress);
    float right = from.right + (to.right - from.right) * static_cast<float>(progress);
    float bottom = from.bottom + (to.bottom - from.bottom) * static_cast<float>(progress);
    
    return Thickness(left, top, right, bottom);
}

Thickness ThicknessAnimation::Add(const Thickness& value1, const Thickness& value2) const {
    return Thickness(
        value1.left + value2.left,
        value1.top + value2.top,
        value1.right + value2.right,
        value1.bottom + value2.bottom
    );
}

void ThicknessAnimation::UpdateCurrentValue(double progress) {
    if (!target_ || !targetProperty_) {
        return;
    }

    // 首次更新时，保存初始值
    if (!hasInitialValue_) {
        try {
            auto value = target_->GetValue(*targetProperty_);
            if (value.has_value()) {
                initialValue_ = std::any_cast<Thickness>(value);
            }
        } catch (...) {
            initialValue_ = Thickness(0.0f, 0.0f, 0.0f, 0.0f);
        }
        hasInitialValue_ = true;
    }

    // 计算当前值
    Thickness currentValue = GetCurrentValue(initialValue_, GetTo(), progress);
    
    // 应用到目标属性
    target_->SetValue(*targetProperty_, currentValue);
}

} // namespace fk::animation
