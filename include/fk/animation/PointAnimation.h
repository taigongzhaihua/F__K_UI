#pragma once

#include "fk/animation/Animation.h"
#include "fk/ui/graphics/Primitives.h"
#include "fk/render/DrawCommand.h"
#include "fk/binding/DependencyObject.h"

namespace fk::animation {

// 点位置动�?
class PointAnimation : public Animation<ui::Point> {
public:
    PointAnimation();
    PointAnimation(const ui::Point& fromPoint, const ui::Point& toPoint, Duration duration);
    ~PointAnimation() override = default;

    // 设置目标对象和属�?
    void SetTarget(binding::DependencyObject* target, const binding::DependencyProperty* property);
    
    binding::DependencyObject* GetTarget() const { return target_; }
    const binding::DependencyProperty* GetTargetProperty() const { return targetProperty_; }

protected:
    // 实现插�?
    ui::Point Interpolate(const ui::Point& from, const ui::Point& to, double progress) const override;
    
    // 实现加法
    ui::Point Add(const ui::Point& value1, const ui::Point& value2) const override;
    
    // 更新当前�?
    void UpdateCurrentValue(double progress) override;

private:
    binding::DependencyObject* target_{nullptr};
    const binding::DependencyProperty* targetProperty_{nullptr};
    ui::Point initialValue_;
    bool hasInitialValue_{false};
};

} // namespace fk::animation
