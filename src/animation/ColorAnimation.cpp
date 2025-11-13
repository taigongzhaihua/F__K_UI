#include "fk/animation/ColorAnimation.h"
#include <algorithm>

namespace fk::animation {

ColorAnimation::ColorAnimation() {
}

ColorAnimation::ColorAnimation(const ui::Color& fromColor, const ui::Color& toColor, Duration duration) {
    SetFrom(fromColor);
    SetTo(toColor);
    SetDuration(duration);
}

void ColorAnimation::SetTarget(binding::DependencyObject* target, 
                                const binding::DependencyProperty* property) {
    target_ = target;
    targetProperty_ = property;
    hasInitialValue_ = false;
}

ui::Color ColorAnimation::Interpolate(const ui::Color& from, const ui::Color& to, double progress) const {
    // 线性插值每个颜色分量
    float r = from.r + (to.r - from.r) * static_cast<float>(progress);
    float g = from.g + (to.g - from.g) * static_cast<float>(progress);
    float b = from.b + (to.b - from.b) * static_cast<float>(progress);
    float a = from.a + (to.a - from.a) * static_cast<float>(progress);
    
    return ui::Color(r, g, b, a);
}

ui::Color ColorAnimation::Add(const ui::Color& value1, const ui::Color& value2) const {
    // 颜色加法（饱和处理）
    float r = std::min(1.0f, value1.r + value2.r);
    float g = std::min(1.0f, value1.g + value2.g);
    float b = std::min(1.0f, value1.b + value2.b);
    float a = std::min(1.0f, value1.a + value2.a);
    
    return ui::Color(r, g, b, a);
}

void ColorAnimation::UpdateCurrentValue(double progress) {
    if (!target_ || !targetProperty_) {
        return;
    }

    // 首次更新时，保存初始值
    if (!hasInitialValue_) {
        try {
            auto value = target_->GetValue(*targetProperty_);
            if (value.has_value()) {
                initialValue_ = std::any_cast<ui::Color>(value);
            }
        } catch (...) {
            initialValue_ = ui::Color(1.0f, 1.0f, 1.0f, 1.0f);
        }
        hasInitialValue_ = true;
    }

    // 计算当前值
    ui::Color currentValue = GetCurrentValue(initialValue_, GetTo(), progress);
    
    // 应用到目标属性
    target_->SetValue(*targetProperty_, currentValue);
}

} // namespace fk::animation
