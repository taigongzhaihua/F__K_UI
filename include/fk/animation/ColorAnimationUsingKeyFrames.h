#pragma once

#include "fk/animation/KeyFrameAnimation.h"
#include "fk/render/DrawCommand.h"

namespace fk::animation {

using Color = fk::render::Color;

// Color 类型的关键帧动画
class ColorAnimationUsingKeyFrames : public KeyFrameAnimation<Color> {
public:
    ColorAnimationUsingKeyFrames() = default;
    ~ColorAnimationUsingKeyFrames() override = default;

protected:
    Color GetDefaultValue() const override {
        return Color{0, 0, 0, 0};
    }
    
    Color InterpolateValue(const Color& from, const Color& to, double progress) const override {
        return Color{
            from.r + (to.r - from.r) * static_cast<float>(progress),
            from.g + (to.g - from.g) * static_cast<float>(progress),
            from.b + (to.b - from.b) * static_cast<float>(progress),
            from.a + (to.a - from.a) * static_cast<float>(progress)
        };
    }
};

// Color 线性关键帧特化
template<>
inline Color LinearKeyFrame<Color>::InterpolateValueCore(const Color& from, const Color& to, double progress) {
    return Color{
        from.r + (to.r - from.r) * static_cast<float>(progress),
        from.g + (to.g - from.g) * static_cast<float>(progress),
        from.b + (to.b - from.b) * static_cast<float>(progress),
        from.a + (to.a - from.a) * static_cast<float>(progress)
    };
}

// Color 缓动关键帧特化
template<>
inline Color EasingKeyFrame<Color>::InterpolateValueCore(const Color& from, const Color& to, double progress) {
    return Color{
        from.r + (to.r - from.r) * static_cast<float>(progress),
        from.g + (to.g - from.g) * static_cast<float>(progress),
        from.b + (to.b - from.b) * static_cast<float>(progress),
        from.a + (to.a - from.a) * static_cast<float>(progress)
    };
}

// Color Spline 关键帧特化
template<>
inline Color SplineKeyFrame<Color>::InterpolateValueCore(const Color& from, const Color& to, double progress) {
    // 使用贝塞尔曲线进行颜色插值
    return Color{
        from.r + (to.r - from.r) * static_cast<float>(progress),
        from.g + (to.g - from.g) * static_cast<float>(progress),
        from.b + (to.b - from.b) * static_cast<float>(progress),
        from.a + (to.a - from.a) * static_cast<float>(progress)
    };
}

} // namespace fk::animation
