#pragma once

#include "fk/animation/Animation.h"
#include "fk/binding/DependencyObject.h"
#include "fk/binding/DependencyProperty.h"

namespace fk::animation {

// 双精度浮点数动画
class DoubleAnimation : public Animation<double> {
public:
    DoubleAnimation();
    DoubleAnimation(double fromValue, double toValue, Duration duration);
    ~DoubleAnimation() override = default;

    // 克隆动画
    std::shared_ptr<DoubleAnimation> Clone() const;
    
    // 重写SetFrom以标记显式设置
    void SetFrom(double value);

    // 开始动画
    void Begin() override;

    // 设置目标对象和属性
    void SetTarget(binding::DependencyObject* target, const binding::DependencyProperty* property);
    
    binding::DependencyObject* GetTarget() const { return target_; }
    const binding::DependencyProperty* GetTargetProperty() const { return targetProperty_; }

protected:
    // 实现插值
    double Interpolate(const double& from, const double& to, double progress) const override;
    
    // 实现加法
    double Add(const double& value1, const double& value2) const override;
    
    // 更新当前值
    void UpdateCurrentValue(double progress) override;

private:
    binding::DependencyObject* target_{nullptr};
    const binding::DependencyProperty* targetProperty_{nullptr};
    double initialValue_{0.0};
    bool hasInitialValue_{false};
    bool hasExplicitFrom_{false};  // 标记From值是否被显式设置
};

} // namespace fk::animation
