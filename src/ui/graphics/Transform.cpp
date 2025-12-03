#include "fk/ui/graphics/Transform.h"
#include <cmath>
#include <algorithm>

namespace fk::ui {

// ========== Transform 基类 ==========

Point Transform::TransformPoint(const Point& point) const {
    Matrix3x2 mat = GetMatrix();
    return mat.TransformPoint(point);
}

Matrix3x2 Transform::GetInverseMatrix() const {
    return GetMatrix().Inverse();
}

// ========== TranslateTransform ==========

Matrix3x2 TranslateTransform::GetMatrix() const {
    return Matrix3x2::Translation(x_, y_);
}

// ========== ScaleTransform ==========

Matrix3x2 ScaleTransform::GetMatrix() const {
    // 先平移到中心点，然后缩放，再平移回来
    Matrix3x2 mat = Matrix3x2::Identity();
    
    // 平移到原�?
    if (centerX_ != 0 || centerY_ != 0) {
        mat = Matrix3x2::Translation(-centerX_, -centerY_);
    }
    
    // 应用缩放
    mat = mat * Matrix3x2::Scaling(scaleX_, scaleY_);
    
    // 平移回中心点
    if (centerX_ != 0 || centerY_ != 0) {
        mat = mat * Matrix3x2::Translation(centerX_, centerY_);
    }
    
    return mat;
}

// ========== RotateTransform ==========

Matrix3x2 RotateTransform::GetMatrix() const {
    // 先平移到中心点，然后旋转，再平移回来
    Matrix3x2 mat = Matrix3x2::Identity();
    
    // 平移到原�?
    if (centerX_ != 0 || centerY_ != 0) {
        mat = Matrix3x2::Translation(-centerX_, -centerY_);
    }
    
    // 应用旋转（角度转弧度�?
    float radians = angle_ * 3.14159265f / 180.0f;
    mat = mat * Matrix3x2::Rotation(radians);
    
    // 平移回中心点
    if (centerX_ != 0 || centerY_ != 0) {
        mat = mat * Matrix3x2::Translation(centerX_, centerY_);
    }
    
    return mat;
}

// ========== SkewTransform ==========

Matrix3x2 SkewTransform::GetMatrix() const {
    // 先平移到中心点，然后倾斜，再平移回来
    Matrix3x2 mat = Matrix3x2::Identity();
    
    // 平移到原�?
    if (centerX_ != 0 || centerY_ != 0) {
        mat = Matrix3x2::Translation(-centerX_, -centerY_);
    }
    
    // 应用倾斜
    float radiansX = angleX_ * 3.14159265f / 180.0f;
    float radiansY = angleY_ * 3.14159265f / 180.0f;
    
    Matrix3x2 skew;
    skew.m11 = 1.0f;
    skew.m12 = std::tan(radiansY);
    skew.m21 = std::tan(radiansX);
    skew.m22 = 1.0f;
    skew.m31 = 0.0f;
    skew.m32 = 0.0f;
    
    mat = mat * skew;
    
    // 平移回中心点
    if (centerX_ != 0 || centerY_ != 0) {
        mat = mat * Matrix3x2::Translation(centerX_, centerY_);
    }
    
    return mat;
}

// ========== MatrixTransform ==========

Matrix3x2 MatrixTransform::GetInverseMatrix() const {
    return matrix_.Inverse();
}

// ========== TransformGroup ==========

Matrix3x2 TransformGroup::GetMatrix() const {
    Matrix3x2 result = Matrix3x2::Identity();
    
    // 按顺序应用所有变�?
    for (Transform* child : children_) {
        if (child) {
            result = result * child->GetMatrix();
        }
    }
    
    return result;
}

void TransformGroup::AddTransform(Transform* transform) {
    if (transform) {
        children_.push_back(transform);
    }
}

void TransformGroup::RemoveTransform(Transform* transform) {
    auto it = std::find(children_.begin(), children_.end(), transform);
    if (it != children_.end()) {
        children_.erase(it);
    }
}

void TransformGroup::ClearTransforms() {
    children_.clear();
}

} // namespace fk::ui
