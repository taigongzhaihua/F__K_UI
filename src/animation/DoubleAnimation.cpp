#include "fk/animation/DoubleAnimation.h"
#include <algorithm>

namespace fk::animation {

DoubleAnimation::DoubleAnimation() {
}

DoubleAnimation::DoubleAnimation(double fromValue, double toValue, Duration duration) {
    SetFrom(fromValue);
    SetTo(toValue);
    SetDuration(duration);
}

void DoubleAnimation::SetTarget(binding::DependencyObject* target, 
                                 const binding::DependencyProperty* property) {
    target_ = target;
    targetProperty_ = property;
    hasInitialValue_ = false;
}

double DoubleAnimation::Interpolate(const double& from, const double& to, double progress) const {
    return from + (to - from) * progress;
}

double DoubleAnimation::Add(const double& value1, const double& value2) const {
    return value1 + value2;
}

void DoubleAnimation::UpdateCurrentValue(double progress) {
    if (!target_ || !targetProperty_) {
        return;
    }

    // 首次更新时，保存初始值
    if (!hasInitialValue_) {
        try {
            auto value = target_->GetValue(*targetProperty_);
            if (value.has_value()) {
                // 尝试获取 double 类型值
                try {
                    initialValue_ = std::any_cast<double>(value);
                } catch (...) {
                    // 如果失败，尝试 float 类型（Opacity属性是float）
                    try {
                        initialValue_ = static_cast<double>(std::any_cast<float>(value));
                    } catch (...) {
                        initialValue_ = 0.0;
                    }
                }
            }
        } catch (...) {
            initialValue_ = 0.0;
        }
        hasInitialValue_ = true;
    }

    // 计算当前值
    double currentValue = GetCurrentValue(initialValue_, GetTo(), progress);
    
    // 应用到目标属性
    // 检查目标属性类型，如果是float则转换
    if (targetProperty_->PropertyType() == typeid(float)) {
        target_->SetValue(*targetProperty_, static_cast<float>(currentValue));
    } else {
        target_->SetValue(*targetProperty_, currentValue);
    }
}

} // namespace fk::animation
