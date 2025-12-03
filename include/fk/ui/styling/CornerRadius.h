#pragma once

namespace fk::ui {

/**
 * @brief 圆角半径
 * 
 * 描述矩形四个角的圆角半径
 */
struct CornerRadius {
    float topLeft;
    float topRight;
    float bottomRight;
    float bottomLeft;

    // 构造函数
    CornerRadius() : topLeft(0), topRight(0), bottomRight(0), bottomLeft(0) {}
    
    explicit CornerRadius(float uniform) 
        : topLeft(uniform), topRight(uniform), bottomRight(uniform), bottomLeft(uniform) {}
    
    CornerRadius(float topLeft, float topRight, float bottomRight, float bottomLeft)
        : topLeft(topLeft), topRight(topRight), bottomRight(bottomRight), bottomLeft(bottomLeft) {}

    // 比较运算符
    bool operator==(const CornerRadius& other) const {
        return topLeft == other.topLeft && 
               topRight == other.topRight && 
               bottomRight == other.bottomRight && 
               bottomLeft == other.bottomLeft;
    }

    bool operator!=(const CornerRadius& other) const {
        return !(*this == other);
    }
};

} // namespace fk::ui
