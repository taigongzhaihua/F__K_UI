#pragma once

#include "fk/animation/Animation.h"
#include "fk/ui/DrawCommand.h"
#include "fk/binding/DependencyObject.h"

namespace fk::animation {

// 颜色动画
class ColorAnimation : public Animation<ui::Color> {
public:
    ColorAnimation();
    ColorAnimation(const ui::Color& fromColor, const ui::Color& toColor, Duration duration);
    ~ColorAnimation() override = default;

    // 设置目标对象和属性
    void SetTarget(binding::DependencyObject* target, const binding::DependencyProperty* property);
    
    binding::DependencyObject* GetTarget() const { return target_; }
    const binding::DependencyProperty* GetTargetProperty() const { return targetProperty_; }
    
    // 设置 To 值绑定到属性（类似 TemplateBinding）
    void SetToBinding(const DependencyProperty* sourceProperty) { toBindingProperty_ = sourceProperty; }
    const DependencyProperty* GetToBinding() const { return toBindingProperty_; }
    bool HasToBinding() const { return toBindingProperty_ != nullptr; }
    
    // 重写 Begin() 以在动画开始时捕获初始值
    void Begin() override;

protected:
    // 实现插值
    ui::Color Interpolate(const ui::Color& from, const ui::Color& to, double progress) const override;
    
    // 实现加法
    ui::Color Add(const ui::Color& value1, const ui::Color& value2) const override;
    
    // 更新当前值
    void UpdateCurrentValue(double progress) override;

private:
    binding::DependencyObject* target_{nullptr};
    const binding::DependencyProperty* targetProperty_{nullptr};
    ui::Color initialValue_;
    bool hasInitialValue_{false};
    
    // To 值绑定（从 TemplatedParent 的属性获取）
    const DependencyProperty* toBindingProperty_{nullptr};
};

} // namespace fk::animation
