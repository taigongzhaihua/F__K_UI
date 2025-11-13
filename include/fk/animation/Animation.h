#pragma once

#include "fk/animation/Timeline.h"
#include "fk/animation/EasingFunction.h"
#include "fk/binding/DependencyProperty.h"
#include <any>
#include <memory>

namespace fk::animation {

// Animation 基类 - 所有属性动画的基础
template<typename T>
class Animation : public Timeline {
public:
    Animation() = default;
    virtual ~Animation() = default;

    // 依赖属性
    static const binding::DependencyProperty& FromProperty() {
        static auto& property = binding::DependencyProperty::Register(
            "From",
            typeid(T),
            typeid(Animation<T>),
            binding::PropertyMetadata{std::any()}  // 默认为空，表示使用当前值
        );
        return property;
    }

    static const binding::DependencyProperty& ToProperty() {
        static auto& property = binding::DependencyProperty::Register(
            "To",
            typeid(T),
            typeid(Animation<T>),
            binding::PropertyMetadata{std::any()}
        );
        return property;
    }

    static const binding::DependencyProperty& ByProperty() {
        static auto& property = binding::DependencyProperty::Register(
            "By",
            typeid(T),
            typeid(Animation<T>),
            binding::PropertyMetadata{std::any()}  // 相对变化量
        );
        return property;
    }

    // 属性访问器
    bool HasFrom() const {
        return GetValue(FromProperty()).has_value();
    }

    T GetFrom() const {
        if (HasFrom()) {
            return std::any_cast<T>(GetValue(FromProperty()));
        }
        return T{};
    }

    void SetFrom(const T& value) {
        SetValue(FromProperty(), value);
    }

    bool HasTo() const {
        return GetValue(ToProperty()).has_value();
    }

    T GetTo() const {
        if (HasTo()) {
            return std::any_cast<T>(GetValue(ToProperty()));
        }
        return T{};
    }

    void SetTo(const T& value) {
        SetValue(ToProperty(), value);
    }

    bool HasBy() const {
        return GetValue(ByProperty()).has_value();
    }

    T GetBy() const {
        if (HasBy()) {
            return std::any_cast<T>(GetValue(ByProperty()));
        }
        return T{};
    }

    void SetBy(const T& value) {
        SetValue(ByProperty(), value);
    }

    // 缓动函数
    void SetEasingFunction(std::shared_ptr<EasingFunctionBase> easingFunction) {
        easingFunction_ = easingFunction;
    }

    std::shared_ptr<EasingFunctionBase> GetEasingFunction() const {
        return easingFunction_;
    }

    // 获取当前动画值
    virtual T GetCurrentValue(const T& defaultOriginValue, const T& defaultDestinationValue, double progress) {
        // 应用缓动函数
        if (easingFunction_) {
            progress = easingFunction_->Ease(progress);
        }

        // 确定起始值和目标值
        T from = defaultOriginValue;
        T to = defaultDestinationValue;

        if (HasFrom()) {
            from = GetFrom();
        }

        if (HasTo()) {
            to = GetTo();
        } else if (HasBy()) {
            to = Add(from, GetBy());
        }

        // 插值计算
        return Interpolate(from, to, progress);
    }

protected:
    // 子类需要实现的插值方法
    virtual T Interpolate(const T& from, const T& to, double progress) const = 0;
    
    // 子类需要实现的加法（用于 By 属性）
    virtual T Add(const T& value1, const T& value2) const = 0;

    void UpdateCurrentValue(double progress) override {
        // 基类实现为空，子类应该重写以应用动画值到目标对象
    }

private:
    std::shared_ptr<EasingFunctionBase> easingFunction_;
};

} // namespace fk::animation
