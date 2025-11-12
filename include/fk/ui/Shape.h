/**
 * @file Shape.h
 * @brief Shape 图形基类
 * 
 * 职责：
 * - 提供图形填充（Fill）和描边（Stroke）
 * - 定义图形渲染接口
 * 
 * WPF 对应：Shape
 */

#pragma once

#include "fk/ui/FrameworkElement.h"
#include "fk/binding/DependencyProperty.h"

namespace fk::ui {

// 前向声明
class Brush;

/**
 * @brief Shape 抽象基类
 * 
 * 所有 2D 图形的基类，提供填充和描边功能。
 */
class Shape : public FrameworkElement<Shape> {
public:
    Shape() = default;
    virtual ~Shape() = default;

    // ========== 依赖属性 ==========
    
    /// Fill 属性：填充画刷
    static const binding::DependencyProperty& FillProperty();
    
    /// Stroke 属性：描边画刷
    static const binding::DependencyProperty& StrokeProperty();
    
    /// StrokeThickness 属性：描边厚度
    static const binding::DependencyProperty& StrokeThicknessProperty();

    // ========== 外观属性 ==========
    
    Brush* GetFill() const;
    void SetFill(Brush* value);
    Shape* Fill(Brush* brush);
    Brush* Fill() const;
    
    Brush* GetStroke() const;
    void SetStroke(Brush* value);
    Shape* Stroke(Brush* brush);
    Brush* Stroke() const;
    
    float GetStrokeThickness() const;
    void SetStrokeThickness(float value);
    Shape* StrokeThickness(float thickness);
    float StrokeThickness() const;

    /**
     * @brief 收集绘制命令
     */
    void CollectDrawCommands(class RenderContext& context) override;

protected:
    /**
     * @brief 渲染图形（派生类实现具体图形）
     */
    virtual void OnRender(class RenderContext& context);
    
    /**
     * @brief 获取图形定义的边界（用于布局）
     */
    virtual Rect GetDefiningGeometry() const = 0;
    
    /**
     * @brief 测量覆写
     */
    Size MeasureOverride(const Size& availableSize) override;
    
    /**
     * @brief 排列覆写
     */
    Size ArrangeOverride(const Size& finalSize) override;
};

/**
 * @brief 矩形图形
 */
class Rectangle : public Shape {
public:
    Rectangle() = default;
    virtual ~Rectangle() = default;

    // ========== 依赖属性 ==========
    
    /// RadiusX 属性：圆角 X 半径
    static const binding::DependencyProperty& RadiusXProperty();
    
    /// RadiusY 属性：圆角 Y 半径
    static const binding::DependencyProperty& RadiusYProperty();

    // ========== 圆角属性 ==========
    
    float GetRadiusX() const;
    void SetRadiusX(float value);
    Rectangle* RadiusX(float radius);
    
    float GetRadiusY() const;
    void SetRadiusY(float value);
    Rectangle* RadiusY(float radius);

protected:
    Rect GetDefiningGeometry() const override;
    void OnRender(class RenderContext& context) override;
};

/**
 * @brief 椭圆/圆形图形
 */
class Ellipse : public Shape {
public:
    Ellipse() = default;
    virtual ~Ellipse() = default;

protected:
    Rect GetDefiningGeometry() const override;
    void OnRender(class RenderContext& context) override;
};

/**
 * @brief 线条图形
 */
class Line : public Shape {
public:
    Line() = default;
    virtual ~Line() = default;

    // ========== 依赖属性 ==========
    
    /// X1 属性：起点X坐标
    static const binding::DependencyProperty& X1Property();
    
    /// Y1 属性：起点Y坐标
    static const binding::DependencyProperty& Y1Property();
    
    /// X2 属性：终点X坐标
    static const binding::DependencyProperty& X2Property();
    
    /// Y2 属性：终点Y坐标
    static const binding::DependencyProperty& Y2Property();

    // ========== 坐标属性 ==========
    
    float GetX1() const;
    void SetX1(float value);
    Line* X1(float x);
    
    float GetY1() const;
    void SetY1(float value);
    Line* Y1(float y);
    
    float GetX2() const;
    void SetX2(float value);
    Line* X2(float x);
    
    float GetY2() const;
    void SetY2(float value);
    Line* Y2(float y);

protected:
    Rect GetDefiningGeometry() const override;
    void OnRender(class RenderContext& context) override;
};

} // namespace fk::ui
