#pragma once

#include "fk/animation/Animation.h"
#include "fk/render/DrawCommand.h"
#include "fk/binding/DependencyObject.h"

namespace fk::animation {

// 使用 render 模块的 Color
using Color = fk::render::Color;

// 颜色动画
class ColorAnimation : public Animation<Color> {
public:
    ColorAnimation();
    ColorAnimation(const Color& fromColor, const Color& toColor, Duration duration);
    ~ColorAnimation() override;

    // 克隆动画
    std::shared_ptr<ColorAnimation> Clone() const;
    
    // 重写SetFrom以标记显式设置
    void SetFrom(const Color& value);

    // 设置目标对象和属性
    void SetTarget(binding::DependencyObject* target, const binding::DependencyProperty* property);
    
    binding::DependencyObject* GetTarget() const { return target_; }
    const binding::DependencyProperty* GetTargetProperty() const { return targetProperty_; }
    
    // 设置 To 值绑定到属性（类似 TemplateBinding）
    void SetToBinding(const DependencyProperty* sourceProperty) { toBindingProperty_ = sourceProperty; }
    const DependencyProperty* GetToBinding() const { return toBindingProperty_; }
    bool HasToBinding() const { return toBindingProperty_ != nullptr; }
    
    // 设置/获取已解析的 ToBinding 原始值（避免被动画修改后的 Brush 影响）
    void SetResolvedToValue(const Color& value) { resolvedToValue_ = value; hasResolvedToValue_ = true; }
    Color GetResolvedToValue() const { return resolvedToValue_; }
    bool HasResolvedToValue() const { return hasResolvedToValue_; }
    
    // 清除已解析的值（用于在动画开始时重新解析）
    void ClearResolvedToValue() { hasResolvedToValue_ = false; }
    
    // 重写 Begin() 以在动画开始时捕获初始值
    void Begin() override;

protected:
    // 实现插值
    Color Interpolate(const Color& from, const Color& to, double progress) const override;
    
    // 实现加法
    Color Add(const Color& value1, const Color& value2) const override;
    
    // 更新当前值
    void UpdateCurrentValue(double progress) override;

private:
    binding::DependencyObject* target_{nullptr};
    const binding::DependencyProperty* targetProperty_{nullptr};
    Color initialValue_;
    bool hasInitialValue_{false};
    bool hasExplicitFrom_{false};  // 标记From值是否被显式设置
    
    // To 值绑定（从 TemplatedParent 的属性获取）
    const DependencyProperty* toBindingProperty_{nullptr};
    
    // 已解析的 ToBinding 原始值
    Color resolvedToValue_;
    bool hasResolvedToValue_{false};
};

} // namespace fk::animation
