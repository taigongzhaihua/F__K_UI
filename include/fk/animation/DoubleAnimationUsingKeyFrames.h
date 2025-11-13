#pragma once

#include "fk/animation/KeyFrameAnimation.h"

namespace fk::animation {

// Double 类型的关键帧动画
class DoubleAnimationUsingKeyFrames : public KeyFrameAnimation<double> {
public:
    DoubleAnimationUsingKeyFrames() = default;
    ~DoubleAnimationUsingKeyFrames() override = default;

protected:
    double GetDefaultValue() const override {
        return 0.0;
    }
    
    double InterpolateValue(const double& from, const double& to, double progress) const override {
        return from + (to - from) * progress;
    }
};

// Double 线性关键帧特化
template<>
inline double LinearKeyFrame<double>::InterpolateValueCore(const double& from, const double& to, double progress) {
    return from + (to - from) * progress;
}

// Double 缓动关键帧特化
template<>
inline double EasingKeyFrame<double>::InterpolateValueCore(const double& from, const double& to, double progress) {
    return from + (to - from) * progress;
}

// Double Spline 关键帧特化
template<>
inline double SplineKeyFrame<double>::InterpolateValueCore(const double& from, const double& to, double progress) {
    // 三次贝塞尔曲线计算: B(t) = (1-t)³P0 + 3(1-t)²tP1 + 3(1-t)t²P2 + t³P3
    // 其中 P0 = (0,0), P3 = (1,1)
    double t = progress;
    double t2 = t * t;
    double t3 = t2 * t;
    double mt = 1.0 - t;
    double mt2 = mt * mt;
    double mt3 = mt2 * mt;
    
    // Y 坐标
    double bezierY = 3.0 * mt2 * t * controlPoint1Y_ + 
                     3.0 * mt * t2 * controlPoint2Y_ + 
                     t3;
    
    return from + (to - from) * bezierY;
}

} // namespace fk::animation
