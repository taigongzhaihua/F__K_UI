#pragma once

#include "fk/ui/Primitives.h"
#include <vector>

namespace fk::ui {

/**
 * @brief 变换基类
 * 
 * 表示 2D 仿射变换
 */
class Transform {
public:
    virtual ~Transform() = default;
    
    /**
     * @brief 获取变换矩阵
     */
    virtual Matrix3x2 GetMatrix() const = 0;
    
    /**
     * @brief 变换点
     */
    Point TransformPoint(const Point& point) const;
    
    /**
     * @brief 获取逆变换矩阵（用于命中测试）
     */
    virtual Matrix3x2 GetInverseMatrix() const;
};

/**
 * @brief 平移变换
 */
class TranslateTransform : public Transform {
public:
    TranslateTransform(float x = 0, float y = 0) : x_(x), y_(y) {}
    
    Matrix3x2 GetMatrix() const override;
    
    void SetX(float x) { x_ = x; }
    void SetY(float y) { y_ = y; }
    float GetX() const { return x_; }
    float GetY() const { return y_; }
    
private:
    float x_{0};
    float y_{0};
};

/**
 * @brief 缩放变换
 */
class ScaleTransform : public Transform {
public:
    ScaleTransform(float scaleX = 1.0f, float scaleY = 1.0f, float centerX = 0, float centerY = 0)
        : scaleX_(scaleX), scaleY_(scaleY), centerX_(centerX), centerY_(centerY) {}
    
    Matrix3x2 GetMatrix() const override;
    
    void SetScaleX(float scaleX) { scaleX_ = scaleX; }
    void SetScaleY(float scaleY) { scaleY_ = scaleY; }
    void SetCenterX(float x) { centerX_ = x; }
    void SetCenterY(float y) { centerY_ = y; }
    
    float GetScaleX() const { return scaleX_; }
    float GetScaleY() const { return scaleY_; }
    float GetCenterX() const { return centerX_; }
    float GetCenterY() const { return centerY_; }
    
private:
    float scaleX_{1.0f};
    float scaleY_{1.0f};
    float centerX_{0};
    float centerY_{0};
};

/**
 * @brief 旋转变换
 */
class RotateTransform : public Transform {
public:
    RotateTransform(float angle = 0, float centerX = 0, float centerY = 0)
        : angle_(angle), centerX_(centerX), centerY_(centerY) {}
    
    Matrix3x2 GetMatrix() const override;
    
    void SetAngle(float angle) { angle_ = angle; }
    void SetCenterX(float x) { centerX_ = x; }
    void SetCenterY(float y) { centerY_ = y; }
    
    float GetAngle() const { return angle_; }
    float GetCenterX() const { return centerX_; }
    float GetCenterY() const { return centerY_; }
    
private:
    float angle_{0};      // 角度（度）
    float centerX_{0};
    float centerY_{0};
};

/**
 * @brief 倾斜变换
 */
class SkewTransform : public Transform {
public:
    SkewTransform(float angleX = 0, float angleY = 0, float centerX = 0, float centerY = 0)
        : angleX_(angleX), angleY_(angleY), centerX_(centerX), centerY_(centerY) {}
    
    Matrix3x2 GetMatrix() const override;
    
    void SetAngleX(float angle) { angleX_ = angle; }
    void SetAngleY(float angle) { angleY_ = angle; }
    void SetCenterX(float x) { centerX_ = x; }
    void SetCenterY(float y) { centerY_ = y; }
    
    float GetAngleX() const { return angleX_; }
    float GetAngleY() const { return angleY_; }
    float GetCenterX() const { return centerX_; }
    float GetCenterY() const { return centerY_; }
    
private:
    float angleX_{0};
    float angleY_{0};
    float centerX_{0};
    float centerY_{0};
};

/**
 * @brief 矩阵变换（通用）
 */
class MatrixTransform : public Transform {
public:
    MatrixTransform(const Matrix3x2& matrix = Matrix3x2::Identity())
        : matrix_(matrix) {}
    
    Matrix3x2 GetMatrix() const override { return matrix_; }
    Matrix3x2 GetInverseMatrix() const override;
    
    void SetMatrix(const Matrix3x2& matrix) { matrix_ = matrix; }
    
private:
    Matrix3x2 matrix_;
};

/**
 * @brief 变换组（多个变换的复合）
 */
class TransformGroup : public Transform {
public:
    Matrix3x2 GetMatrix() const override;
    
    void AddTransform(Transform* transform);
    void RemoveTransform(Transform* transform);
    void ClearTransforms();
    
    const std::vector<Transform*>& GetChildren() const { return children_; }
    
private:
    std::vector<Transform*> children_;
};

} // namespace fk::ui
