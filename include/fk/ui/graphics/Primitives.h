#pragma once

#include <cmath>

namespace fk::ui {

/**
 * @brief 二维点
 */
struct Point {
    float x{0.0f};
    float y{0.0f};

    Point() = default;
    Point(float x, float y) : x(x), y(y) {}
};

/**
 * @brief 二维尺寸
 */
struct Size {
    float width{0.0f};
    float height{0.0f};

    Size() = default;
    Size(float width, float height) : width(width), height(height) {}
    
    bool IsEmpty() const { return width <= 0.0f || height <= 0.0f; }
};

/**
 * @brief 矩形区域
 */
struct Rect {
    float x{0.0f};
    float y{0.0f};
    float width{0.0f};
    float height{0.0f};

    Rect() = default;
    Rect(float x, float y, float width, float height) 
        : x(x), y(y), width(width), height(height) {}
    Rect(const Point& location, const Size& size)
        : x(location.x), y(location.y), width(size.width), height(size.height) {}
    
    float Left() const { return x; }
    float Top() const { return y; }
    float Right() const { return x + width; }
    float Bottom() const { return y + height; }
    
    Point Location() const { return Point(x, y); }
    Size GetSize() const { return Size(width, height); }
    
    bool Contains(const Point& point) const {
        return point.x >= x && point.x <= (x + width) &&
               point.y >= y && point.y <= (y + height);
    }
    
    bool IsEmpty() const { return width <= 0.0f || height <= 0.0f; }
};

/**
 * @brief 3x2 变换矩阵（2D 仿射变换）
 */
struct Matrix3x2 {
    float m11{1.0f}, m12{0.0f};
    float m21{0.0f}, m22{1.0f};
    float m31{0.0f}, m32{0.0f};

    Matrix3x2() = default;
    Matrix3x2(float m11, float m12, float m21, float m22, float m31, float m32)
        : m11(m11), m12(m12), m21(m21), m22(m22), m31(m31), m32(m32) {}
    
    static Matrix3x2 Identity() {
        return Matrix3x2(1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f);
    }
    
    static Matrix3x2 Translation(float x, float y) {
        return Matrix3x2(1.0f, 0.0f, 0.0f, 1.0f, x, y);
    }
    
    static Matrix3x2 Scale(float scaleX, float scaleY) {
        return Matrix3x2(scaleX, 0.0f, 0.0f, scaleY, 0.0f, 0.0f);
    }
    
    static Matrix3x2 Scaling(float scaleX, float scaleY) {
        return Scale(scaleX, scaleY);
    }
    
    static Matrix3x2 Rotation(float radians) {
        float cos = std::cos(radians);
        float sin = std::sin(radians);
        return Matrix3x2(cos, sin, -sin, cos, 0.0f, 0.0f);
    }
    
    // 计算逆矩阵
    Matrix3x2 Inverse() const {
        float det = m11 * m22 - m12 * m21;
        if (std::abs(det) < 1e-6f) {
            return Identity(); // 奇异矩阵，返回单位矩阵
        }
        
        float invDet = 1.0f / det;
        return Matrix3x2(
            m22 * invDet,
            -m12 * invDet,
            -m21 * invDet,
            m11 * invDet,
            (m21 * m32 - m22 * m31) * invDet,
            (m12 * m31 - m11 * m32) * invDet
        );
    }
    
    // 矩阵乘法
    Matrix3x2 operator*(const Matrix3x2& other) const {
        return Matrix3x2(
            m11 * other.m11 + m12 * other.m21,
            m11 * other.m12 + m12 * other.m22,
            m21 * other.m11 + m22 * other.m21,
            m21 * other.m12 + m22 * other.m22,
            m31 * other.m11 + m32 * other.m21 + other.m31,
            m31 * other.m12 + m32 * other.m22 + other.m32
        );
    }
    
    // 变换点
    Point TransformPoint(const Point& point) const {
        return Point(
            point.x * m11 + point.y * m21 + m31,
            point.x * m12 + point.y * m22 + m32
        );
    }
};

// 前向声明
class Visual;

/**
 * @brief 命中测试结果
 */
struct HitTestResult {
    Visual* visualHit{nullptr};
    Point pointHit;
    
    HitTestResult() = default;
    HitTestResult(Visual* visual, const Point& point)
        : visualHit(visual), pointHit(point) {}
};

} // namespace fk::ui
