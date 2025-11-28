#include "fk/animation/ColorAnimation.h"
#include <algorithm>
#include <iostream>

namespace fk::animation {

ColorAnimation::ColorAnimation() {
}

ColorAnimation::~ColorAnimation() = default;

ColorAnimation::ColorAnimation(const Color& fromColor, const Color& toColor, Duration duration) {
    SetFrom(fromColor);
    SetTo(toColor);
    SetDuration(duration);
}

std::shared_ptr<ColorAnimation> ColorAnimation::Clone() const {
    auto clone = std::make_shared<ColorAnimation>();
    
    // 只复制显式设置过的From值
    if (hasExplicitFrom_) {
        clone->SetFrom(GetFrom());
        clone->hasExplicitFrom_ = true;
    }
    
    clone->SetTo(GetTo());
    clone->SetDuration(GetDuration());
    clone->SetToBinding(toBindingProperty_);
    // 注意：不复制 target_ 和 targetProperty_，因为这些需要在 ResolveVisualStateTargets 中重新绑定
    return clone;
}

void ColorAnimation::SetFrom(const Color& value) {
    Animation<Color>::SetFrom(value);
    hasExplicitFrom_ = true;
}

void ColorAnimation::Begin() {
    // 每次Begin时重置初始值标志
    // 这样UpdateCurrentValue会在第一次调用时重新捕获当前颜色
    // 确保从当前状态平滑过渡，即使是重复使用同一个动画对象
    hasInitialValue_ = false;
    
    // 调用基类的 Begin() 方法
    Animation<Color>::Begin();
}

void ColorAnimation::SetTarget(binding::DependencyObject* target, 
                                const binding::DependencyProperty* property) {
    target_ = target;
    targetProperty_ = property;
    // 不在这里捕获初始值，而是在 Begin() 时捕获
    hasInitialValue_ = false;
}

Color ColorAnimation::Interpolate(const Color& from, const Color& to, double progress) const {
    // 线性插值每个颜色分量
    float r = from.r + (to.r - from.r) * static_cast<float>(progress);
    float g = from.g + (to.g - from.g) * static_cast<float>(progress);
    float b = from.b + (to.b - from.b) * static_cast<float>(progress);
    float a = from.a + (to.a - from.a) * static_cast<float>(progress);
    
    return Color(r, g, b, a);
}

Color ColorAnimation::Add(const Color& value1, const Color& value2) const {
    // 颜色加法（饱和处理）
    float r = std::min(1.0f, value1.r + value2.r);
    float g = std::min(1.0f, value1.g + value2.g);
    float b = std::min(1.0f, value1.b + value2.b);
    float a = std::min(1.0f, value1.a + value2.a);
    
    return Color(r, g, b, a);
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
                initialValue_ = std::any_cast<Color>(value);
            }
        } catch (...) {
            initialValue_ = Color(1.0f, 1.0f, 1.0f, 1.0f);
        }
        hasInitialValue_ = true;
    }

    // 计算当前值
    Color currentValue = GetCurrentValue(initialValue_, GetTo(), progress);
    
    // 应用到目标属性
    target_->SetValue(*targetProperty_, currentValue);
}

} // namespace fk::animation
