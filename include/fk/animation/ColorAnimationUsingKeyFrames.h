#pragma once

#include "fk/animation/KeyFrameAnimation.h"
#include "fk/ui/DrawCommand.h"

namespace fk::animation {

// Color 类型的关键帧动画
class ColorAnimationUsingKeyFrames : public KeyFrameAnimation<ui::Color> {
public:
    ColorAnimationUsingKeyFrames() = default;
    ~ColorAnimationUsingKeyFrames() override = default;

protected:
    ui::Color GetDefaultValue() const override {
        return ui::Color{0, 0, 0, 0};
    }
    
    ui::Color InterpolateValue(const ui::Color& from, const ui::Color& to, double progress) const override {
        return ui::Color{
            from.r + (to.r - from.r) * static_cast<float>(progress),
            from.g + (to.g - from.g) * static_cast<float>(progress),
            from.b + (to.b - from.b) * static_cast<float>(progress),
            from.a + (to.a - from.a) * static_cast<float>(progress)
        };
    }
};

// Color 线性关键帧特化
template<>
inline ui::Color LinearKeyFrame<ui::Color>::InterpolateValueCore(const ui::Color& from, const ui::Color& to, double progress) {
    return ui::Color{
        from.r + (to.r - from.r) * static_cast<float>(progress),
        from.g + (to.g - from.g) * static_cast<float>(progress),
        from.b + (to.b - from.b) * static_cast<float>(progress),
        from.a + (to.a - from.a) * static_cast<float>(progress)
    };
}

// Color 缓动关键帧特化
template<>
inline ui::Color EasingKeyFrame<ui::Color>::InterpolateValueCore(const ui::Color& from, const ui::Color& to, double progress) {
    return ui::Color{
        from.r + (to.r - from.r) * static_cast<float>(progress),
        from.g + (to.g - from.g) * static_cast<float>(progress),
        from.b + (to.b - from.b) * static_cast<float>(progress),
        from.a + (to.a - from.a) * static_cast<float>(progress)
    };
}

// Color Spline 关键帧特化
template<>
inline ui::Color SplineKeyFrame<ui::Color>::InterpolateValueCore(const ui::Color& from, const ui::Color& to, double progress) {
    // 使用贝塞尔曲线进行颜色插值
    return ui::Color{
        from.r + (to.r - from.r) * static_cast<float>(progress),
        from.g + (to.g - from.g) * static_cast<float>(progress),
        from.b + (to.b - from.b) * static_cast<float>(progress),
        from.a + (to.a - from.a) * static_cast<float>(progress)
    };
}

} // namespace fk::animation
