#pragma once

#include "fk/animation/Animation.h"
#include "fk/ui/Thickness.h"
#include "fk/binding/DependencyObject.h"

namespace fk::animation {

// Thickness 动画（用于 Margin、Padding 等）
class ThicknessAnimation : public Animation<Thickness> {
public:
    ThicknessAnimation();
    ThicknessAnimation(const Thickness& fromValue, const Thickness& toValue, Duration duration);
    ~ThicknessAnimation() override = default;

    // 设置目标对象和属性
    void SetTarget(binding::DependencyObject* target, const binding::DependencyProperty* property);
    
    binding::DependencyObject* GetTarget() const { return target_; }
    const binding::DependencyProperty* GetTargetProperty() const { return targetProperty_; }

protected:
    // 实现插值
    Thickness Interpolate(const Thickness& from, const Thickness& to, double progress) const override;
    
    // 实现加法
    Thickness Add(const Thickness& value1, const Thickness& value2) const override;
    
    // 更新当前值
    void UpdateCurrentValue(double progress) override;

private:
    binding::DependencyObject* target_{nullptr};
    const binding::DependencyProperty* targetProperty_{nullptr};
    Thickness initialValue_;
    bool hasInitialValue_{false};
};

} // namespace fk::animation
