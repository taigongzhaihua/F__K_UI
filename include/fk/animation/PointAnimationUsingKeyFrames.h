#pragma once

#include "fk/animation/KeyFrameAnimation.h"
#include "fk/ui/graphics/Primitives.h"

namespace fk::animation {

// Point 类型的关键帧动画
class PointAnimationUsingKeyFrames : public KeyFrameAnimation<ui::Point> {
public:
    PointAnimationUsingKeyFrames() = default;
    ~PointAnimationUsingKeyFrames() override = default;

protected:
    ui::Point GetDefaultValue() const override {
        return ui::Point{0.0f, 0.0f};
    }
    
    ui::Point InterpolateValue(const ui::Point& from, const ui::Point& to, double progress) const override {
        return ui::Point{
            from.x + (to.x - from.x) * static_cast<float>(progress),
            from.y + (to.y - from.y) * static_cast<float>(progress)
        };
    }
};

// Point 线性关键帧特化
template<>
inline ui::Point LinearKeyFrame<ui::Point>::InterpolateValueCore(const ui::Point& from, const ui::Point& to, double progress) {
    return ui::Point{
        from.x + (to.x - from.x) * static_cast<float>(progress),
        from.y + (to.y - from.y) * static_cast<float>(progress)
    };
}

// Point 缓动关键帧特�?
template<>
inline ui::Point EasingKeyFrame<ui::Point>::InterpolateValueCore(const ui::Point& from, const ui::Point& to, double progress) {
    return ui::Point{
        from.x + (to.x - from.x) * static_cast<float>(progress),
        from.y + (to.y - from.y) * static_cast<float>(progress)
    };
}

// Point Spline 关键帧特�?
template<>
inline ui::Point SplineKeyFrame<ui::Point>::InterpolateValueCore(const ui::Point& from, const ui::Point& to, double progress) {
    return ui::Point{
        from.x + (to.x - from.x) * static_cast<float>(progress),
        from.y + (to.y - from.y) * static_cast<float>(progress)
    };
}

} // namespace fk::animation
