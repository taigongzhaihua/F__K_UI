#pragma once

#include <cmath>
#include <memory>

namespace fk::animation {

// 缓动模式
enum class EasingMode {
    EaseIn,     // 开始慢，结束快
    EaseOut,    // 开始快，结束慢
    EaseInOut   // 开始和结束都慢，中间快
};

// 缓动函数基类
class EasingFunctionBase {
public:
    virtual ~EasingFunctionBase() = default;
    
    // 应用缓动函数
    double Ease(double normalizedTime) const {
        switch (easingMode_) {
            case EasingMode::EaseIn:
                return EaseInCore(normalizedTime);
            case EasingMode::EaseOut:
                return 1.0 - EaseInCore(1.0 - normalizedTime);
            case EasingMode::EaseInOut:
                if (normalizedTime < 0.5) {
                    return EaseInCore(normalizedTime * 2.0) / 2.0;
                } else {
                    return 0.5 + (1.0 - EaseInCore((1.0 - normalizedTime) * 2.0)) / 2.0;
                }
            default:
                return normalizedTime;
        }
    }
    
    void SetEasingMode(EasingMode mode) { easingMode_ = mode; }
    EasingMode GetEasingMode() const { return easingMode_; }

protected:
    // 子类实现的核心缓动函数（仅实现 EaseIn）
    virtual double EaseInCore(double normalizedTime) const = 0;
    
private:
    EasingMode easingMode_{EasingMode::EaseIn};
};

// 线性缓动（无缓动效果）
class LinearEase : public EasingFunctionBase {
protected:
    double EaseInCore(double normalizedTime) const override {
        return normalizedTime;
    }
};

// 二次方缓动
class QuadraticEase : public EasingFunctionBase {
protected:
    double EaseInCore(double normalizedTime) const override {
        return normalizedTime * normalizedTime;
    }
};

// 三次方缓动
class CubicEase : public EasingFunctionBase {
protected:
    double EaseInCore(double normalizedTime) const override {
        return normalizedTime * normalizedTime * normalizedTime;
    }
};

// 四次方缓动
class QuarticEase : public EasingFunctionBase {
protected:
    double EaseInCore(double normalizedTime) const override {
        double t2 = normalizedTime * normalizedTime;
        return t2 * t2;
    }
};

// 五次方缓动
class QuinticEase : public EasingFunctionBase {
protected:
    double EaseInCore(double normalizedTime) const override {
        double t2 = normalizedTime * normalizedTime;
        return t2 * t2 * normalizedTime;
    }
};

// 正弦缓动
class SineEase : public EasingFunctionBase {
protected:
    double EaseInCore(double normalizedTime) const override {
        return 1.0 - std::cos(normalizedTime * M_PI / 2.0);
    }
};

// 指数缓动
class ExponentialEase : public EasingFunctionBase {
public:
    ExponentialEase() : exponent_(2.0) {}
    explicit ExponentialEase(double exponent) : exponent_(exponent) {}
    
    void SetExponent(double value) { exponent_ = value; }
    double GetExponent() const { return exponent_; }

protected:
    double EaseInCore(double normalizedTime) const override {
        if (normalizedTime == 0.0) return 0.0;
        return (std::exp(exponent_ * normalizedTime) - 1.0) / (std::exp(exponent_) - 1.0);
    }

private:
    double exponent_;
};

// 圆形缓动
class CircleEase : public EasingFunctionBase {
protected:
    double EaseInCore(double normalizedTime) const override {
        return 1.0 - std::sqrt(1.0 - normalizedTime * normalizedTime);
    }
};

// 回弹缓动
class BackEase : public EasingFunctionBase {
public:
    BackEase() : amplitude_(1.0) {}
    explicit BackEase(double amplitude) : amplitude_(amplitude) {}
    
    void SetAmplitude(double value) { amplitude_ = value; }
    double GetAmplitude() const { return amplitude_; }

protected:
    double EaseInCore(double normalizedTime) const override {
        double s = amplitude_;
        return normalizedTime * normalizedTime * ((s + 1.0) * normalizedTime - s);
    }

private:
    double amplitude_;
};

// 弹性缓动
class ElasticEase : public EasingFunctionBase {
public:
    ElasticEase() : oscillations_(3), springiness_(3.0) {}
    ElasticEase(int oscillations, double springiness) 
        : oscillations_(oscillations), springiness_(springiness) {}
    
    void SetOscillations(int value) { oscillations_ = value; }
    int GetOscillations() const { return oscillations_; }
    
    void SetSpringiness(double value) { springiness_ = value; }
    double GetSpringiness() const { return springiness_; }

protected:
    double EaseInCore(double normalizedTime) const override {
        if (normalizedTime == 0.0 || normalizedTime == 1.0) {
            return normalizedTime;
        }
        
        double s = springiness_;
        double o = oscillations_;
        return std::pow(2.0, s * (normalizedTime - 1.0)) * 
               std::sin((normalizedTime - 1.0 - 0.075) * (2.0 * M_PI * o));
    }

private:
    int oscillations_;
    double springiness_;
};

// 反弹缓动
class BounceEase : public EasingFunctionBase {
public:
    BounceEase() : bounces_(3), bounciness_(2.0) {}
    BounceEase(int bounces, double bounciness)
        : bounces_(bounces), bounciness_(bounciness) {}
    
    void SetBounces(int value) { bounces_ = value; }
    int GetBounces() const { return bounces_; }
    
    void SetBounciness(double value) { bounciness_ = value; }
    double GetBounciness() const { return bounciness_; }

protected:
    double EaseInCore(double normalizedTime) const override {
        // 反向应用 BounceOut
        return 1.0 - BounceOut(1.0 - normalizedTime);
    }

private:
    double BounceOut(double t) const {
        if (t < 1.0 / 2.75) {
            return 7.5625 * t * t;
        } else if (t < 2.0 / 2.75) {
            t -= 1.5 / 2.75;
            return 7.5625 * t * t + 0.75;
        } else if (t < 2.5 / 2.75) {
            t -= 2.25 / 2.75;
            return 7.5625 * t * t + 0.9375;
        } else {
            t -= 2.625 / 2.75;
            return 7.5625 * t * t + 0.984375;
        }
    }
    
    int bounces_;
    double bounciness_;
};

// 幂次缓动
class PowerEase : public EasingFunctionBase {
public:
    PowerEase() : power_(2.0) {}
    explicit PowerEase(double power) : power_(power) {}
    
    void SetPower(double value) { power_ = value; }
    double GetPower() const { return power_; }

protected:
    double EaseInCore(double normalizedTime) const override {
        return std::pow(normalizedTime, power_);
    }

private:
    double power_;
};

} // namespace fk::animation
