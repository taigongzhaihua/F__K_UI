#include "fk/animation/DoubleAnimation.h"
#include <algorithm>
#include <iostream>

namespace fk::animation {

DoubleAnimation::DoubleAnimation() {
}

DoubleAnimation::DoubleAnimation(double fromValue, double toValue, Duration duration) {
    SetFrom(fromValue);
    SetTo(toValue);
    SetDuration(duration);
}

std::shared_ptr<DoubleAnimation> DoubleAnimation::Clone() const {
    auto clone = std::make_shared<DoubleAnimation>();
    
    // 只复制显式设置过的From值
    if (hasExplicitFrom_) {
        clone->SetFrom(GetFrom());
        clone->hasExplicitFrom_ = true;
    }
    
    // 只在显式设置了To值时才复制
    if (HasTo()) {
        clone->SetTo(GetTo());
    }
    clone->SetDuration(GetDuration());
    // 注意：不复制 target_ 和 targetProperty_，因为这些需要在 ResolveVisualStateTargets 中重新绑定
    return clone;
}

void DoubleAnimation::SetFrom(double value) {
    Animation<double>::SetFrom(value);
    hasExplicitFrom_ = true;
}

void DoubleAnimation::Begin() {
    // 每次Begin时重置初始值标志
    // 这样UpdateCurrentValue会在第一次调用时重新捕获当前值
    // 确保从当前状态平滑过渡，即使是重复使用同一个动画对象
    hasInitialValue_ = false;
    
    // 清除显式的 From 值,让动画从当前值开始
    // 这样即使是重复使用同一个动画对象也能正确工作
    if (!hasExplicitFrom_) {
        SetValue(FromProperty(), std::any());  // 清除 From 属性
    }
    
    // 调用基类的 Begin() 方法
    Animation<double>::Begin();
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
