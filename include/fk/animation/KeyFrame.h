#pragma once

#include "fk/animation/Timeline.h"
#include "fk/animation/EasingFunction.h"
#include <memory>
#include <any>

namespace fk::animation {

// KeyTime - 关键帧时间
struct KeyTime {
    enum class KeyTimeType {
        Uniform,    // 均匀分布
        Paced,      // 根据距离分布
        Percent,    // 百分比（0.0-1.0）
        TimeSpan    // 绝对时间
    };
    
    KeyTimeType type{KeyTimeType::Uniform};
    double percent{0.0};  // 当 type == Percent 时使用
    std::chrono::milliseconds timeSpan{0};  // 当 type == TimeSpan 时使用
    
    KeyTime() = default;
    
    // 百分比构造
    static KeyTime FromPercent(double percent) {
        KeyTime kt;
        kt.type = KeyTimeType::Percent;
        kt.percent = percent;
        return kt;
    }
    
    // 时间跨度构造
    static KeyTime FromTimeSpan(std::chrono::milliseconds ms) {
        KeyTime kt;
        kt.type = KeyTimeType::TimeSpan;
        kt.timeSpan = ms;
        return kt;
    }
    
    static KeyTime Uniform() {
        KeyTime kt;
        kt.type = KeyTimeType::Uniform;
        return kt;
    }
    
    static KeyTime Paced() {
        KeyTime kt;
        kt.type = KeyTimeType::Paced;
        return kt;
    }
};

// KeyFrame 基类（模板）
template<typename T>
class KeyFrame {
public:
    KeyFrame() = default;
    KeyFrame(const T& value, KeyTime keyTime) 
        : value_(value), keyTime_(keyTime) {}
    
    virtual ~KeyFrame() = default;
    
    // 获取/设置值
    T GetValue() const { return value_; }
    void SetValue(const T& value) { value_ = value; }
    
    // 获取/设置关键时间
    KeyTime GetKeyTime() const { return keyTime_; }
    void SetKeyTime(KeyTime keyTime) { keyTime_ = keyTime; }
    
    // 缓动函数
    void SetEasingFunction(std::shared_ptr<EasingFunctionBase> easing) {
        easingFunction_ = easing;
    }
    
    std::shared_ptr<EasingFunctionBase> GetEasingFunction() const {
        return easingFunction_;
    }
    
    // 插值方法（子类可以重写）
    virtual T InterpolateValue(const T& baseValue, double progress) {
        // 应用缓动函数
        if (easingFunction_) {
            progress = easingFunction_->Ease(progress);
        }
        
        // 线性插值（子类应该重写以实现特定的插值逻辑）
        return InterpolateValueCore(baseValue, value_, progress);
    }

protected:
    // 核心插值方法（子类需要实现）
    virtual T InterpolateValueCore(const T& from, const T& to, double progress) = 0;

private:
    T value_;
    KeyTime keyTime_;
    std::shared_ptr<EasingFunctionBase> easingFunction_;
};

// 线性关键帧（使用线性插值）
template<typename T>
class LinearKeyFrame : public KeyFrame<T> {
public:
    using KeyFrame<T>::KeyFrame;
    
protected:
    T InterpolateValueCore(const T& from, const T& to, double progress) override;
};

// 离散关键帧（突变，无插值）
template<typename T>
class DiscreteKeyFrame : public KeyFrame<T> {
public:
    using KeyFrame<T>::KeyFrame;
    
    T InterpolateValue(const T& baseValue, double progress) override {
        // 离散关键帧不进行插值，progress >= 1.0 时才返回目标值
        if (progress >= 1.0) {
            return this->GetValue();
        }
        return baseValue;
    }
    
protected:
    T InterpolateValueCore(const T& from, const T& to, double progress) override {
        return progress >= 1.0 ? to : from;
    }
};

// 缓动关键帧（使用缓动函数）
template<typename T>
class EasingKeyFrame : public KeyFrame<T> {
public:
    using KeyFrame<T>::KeyFrame;
    
protected:
    T InterpolateValueCore(const T& from, const T& to, double progress) override;
};

// Spline 关键帧（使用贝塞尔曲线）
template<typename T>
class SplineKeyFrame : public KeyFrame<T> {
public:
    using KeyFrame<T>::KeyFrame;
    
    // 设置控制点
    void SetKeySpline(double x1, double y1, double x2, double y2) {
        controlPoint1X_ = x1;
        controlPoint1Y_ = y1;
        controlPoint2X_ = x2;
        controlPoint2Y_ = y2;
    }
    
    // 计算贝塞尔曲线值
    double CalculateBezier(double t) const;
    
protected:
    T InterpolateValueCore(const T& from, const T& to, double progress) override;
    
    // 三次贝塞尔曲线控制点
    double controlPoint1X_{0.0};
    double controlPoint1Y_{0.0};
    double controlPoint2X_{1.0};
    double controlPoint2Y_{1.0};
};

} // namespace fk::animation
