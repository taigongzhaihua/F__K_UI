#pragma once

namespace fk::ui {

struct Thickness {
    float left{0.0f};
    float top{0.0f};
    float right{0.0f};
    float bottom{0.0f};

    constexpr Thickness() = default;
    constexpr explicit Thickness(float uniform)
        : left(uniform), top(uniform), right(uniform), bottom(uniform) {}
    constexpr Thickness(float horizontal, float vertical)
        : left(horizontal), top(vertical), right(horizontal), bottom(vertical) {}
    constexpr Thickness(float leftValue, float topValue, float rightValue, float bottomValue)
        : left(leftValue), top(topValue), right(rightValue), bottom(bottomValue) {}
        bool operator==(const Thickness& other) const {
        return left == other.left && top == other.top &&
               right == other.right && bottom == other.bottom;
    }
    bool operator!=(const Thickness& other) const {
        return !(*this == other);
    }
};
inline bool operator!(const Thickness& thickness) {
    return &thickness != nullptr;
}

} // namespace fk::ui
