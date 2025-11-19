#pragma once

#include "fk/animation/KeyFrameAnimation.h"
#include "fk/ui/Thickness.h"

namespace fk::animation {

// Thickness 类型的关键帧动画
class ThicknessAnimationUsingKeyFrames : public KeyFrameAnimation<Thickness> {
public:
    ThicknessAnimationUsingKeyFrames() = default;
    ~ThicknessAnimationUsingKeyFrames() override = default;

protected:
    Thickness GetDefaultValue() const override {
        return Thickness{};
    }
    
    Thickness InterpolateValue(const Thickness& from, const Thickness& to, double progress) const override {
        return Thickness(
            from.left + (to.left - from.left) * static_cast<float>(progress),
            from.top + (to.top - from.top) * static_cast<float>(progress),
            from.right + (to.right - from.right) * static_cast<float>(progress),
            from.bottom + (to.bottom - from.bottom) * static_cast<float>(progress)
        );
    }
};

// Thickness 线性关键帧特化
template<>
inline Thickness LinearKeyFrame<Thickness>::InterpolateValueCore(const Thickness& from, const Thickness& to, double progress) {
    return Thickness(
        from.left + (to.left - from.left) * static_cast<float>(progress),
        from.top + (to.top - from.top) * static_cast<float>(progress),
        from.right + (to.right - from.right) * static_cast<float>(progress),
        from.bottom + (to.bottom - from.bottom) * static_cast<float>(progress)
    );
}

// Thickness 缓动关键帧特化
template<>
inline Thickness EasingKeyFrame<Thickness>::InterpolateValueCore(const Thickness& from, const Thickness& to, double progress) {
    return Thickness(
        from.left + (to.left - from.left) * static_cast<float>(progress),
        from.top + (to.top - from.top) * static_cast<float>(progress),
        from.right + (to.right - from.right) * static_cast<float>(progress),
        from.bottom + (to.bottom - from.bottom) * static_cast<float>(progress)
    );
}

// Thickness Spline 关键帧特化
template<>
inline Thickness SplineKeyFrame<Thickness>::InterpolateValueCore(const Thickness& from, const Thickness& to, double progress) {
    return Thickness(
        from.left + (to.left - from.left) * static_cast<float>(progress),
        from.top + (to.top - from.top) * static_cast<float>(progress),
        from.right + (to.right - from.right) * static_cast<float>(progress),
        from.bottom + (to.bottom - from.bottom) * static_cast<float>(progress)
    );
}

} // namespace fk::animation
