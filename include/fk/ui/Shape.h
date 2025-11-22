/**
 * @file Shape.h
 * @brief Shape 图形基类
 * 
 * 职责�?
 * - 提供图形填充（Fill）和描边（Stroke�?
 * - 定义图形渲染接口
 * 
 * WPF 对应：Shape
 */

#pragma once

#include "fk/ui/FrameworkElement.h"
#include "fk/binding/DependencyProperty.h"
#include "fk/ui/Primitives.h"
#include "fk/render/DrawCommand.h"  // for render::Color

namespace fk::ui {

// 前向声明
class Brush;

/**
 * @brief Shape 抽象基类
 * 
 * 所�?2D 图形的基类，提供填充和描边功能�?
 * 模板参数：Derived - 派生类类型（CRTP�?
 */
template<typename Derived>
class Shape : public FrameworkElement<Derived> {
public:
    Shape() = default;
    virtual ~Shape() = default;

    // ========== 依赖属�?==========
    
    /// Fill 属性：填充画刷
    static const binding::DependencyProperty& FillProperty();
    
    /// Stroke 属性：描边画刷
    static const binding::DependencyProperty& StrokeProperty();
    
    /// StrokeThickness 属性：描边厚度
    static const binding::DependencyProperty& StrokeThicknessProperty();

    // ========== 外观属�?==========
    
    Brush* GetFill() const;
    void SetFill(Brush* value);
    Derived* Fill(Brush* brush) {
        SetFill(brush);
        return static_cast<Derived*>(this);
    }
    Brush* Fill() const { return GetFill(); }
    
    Brush* GetStroke() const;
    void SetStroke(Brush* value);
    Derived* Stroke(Brush* brush) {
        SetStroke(brush);
        return static_cast<Derived*>(this);
    }
    Brush* Stroke() const { return GetStroke(); }
    
    float GetStrokeThickness() const;
    void SetStrokeThickness(float value);
    Derived* StrokeThickness(float thickness) {
        SetStrokeThickness(thickness);
        return static_cast<Derived*>(this);
    }
    float StrokeThickness() const { return GetStrokeThickness(); }
    
    // ========== 不透明度（继承自UIElement，提供链式调用）==========
    
    Derived* Opacity(float value) {
        this->SetOpacity(value);
        return static_cast<Derived*>(this);
    }

protected:
    /**
     * @brief 渲染图形（派生类实现具体图形�?
     */
    virtual void OnRender(render::RenderContext& context);
    
    /**
     * @brief 获取图形定义的边界（用于布局�?
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
class Rectangle : public Shape<Rectangle> {
public:
    Rectangle() = default;
    virtual ~Rectangle() = default;

    // ========== 依赖属�?==========
    
    /// RadiusX 属性：圆角 X 半径
    static const binding::DependencyProperty& RadiusXProperty();
    
    /// RadiusY 属性：圆角 Y 半径
    static const binding::DependencyProperty& RadiusYProperty();

    // ========== 圆角属�?==========
    
    float GetRadiusX() const;
    void SetRadiusX(float value);
    Rectangle* RadiusX(float radius);
    
    float GetRadiusY() const;
    void SetRadiusY(float value);
    Rectangle* RadiusY(float radius);

protected:
    Rect GetDefiningGeometry() const;
    void OnRender(render::RenderContext& context);
};

/**
 * @brief 椭圆图形
 */
class Ellipse : public Shape<Ellipse> {
public:
    Ellipse() = default;
    virtual ~Ellipse() = default;

protected:
    Rect GetDefiningGeometry() const;
    void OnRender(render::RenderContext& context);
};

/**
 * @brief 直线图形
 */
class Line : public Shape<Line> {
public:
    Line() = default;
    virtual ~Line() = default;

    // ========== 依赖属�?==========
    
    /// X1 属性：起点X坐标
    static const binding::DependencyProperty& X1Property();
    
    /// Y1 属性：起点Y坐标
    static const binding::DependencyProperty& Y1Property();
    
    /// X2 属性：终点X坐标
    static const binding::DependencyProperty& X2Property();
    
    /// Y2 属性：终点Y坐标
    static const binding::DependencyProperty& Y2Property();

    // ========== 坐标属�?==========
    
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
    Rect GetDefiningGeometry() const;
    void OnRender(render::RenderContext& context);
};

/**
 * @brief 多边形图�?
 * 
 * 通过一系列点定义的闭合多边形�?
 */
class Polygon : public Shape<Polygon> {
public:
    Polygon() = default;
    virtual ~Polygon() = default;

    // ========== 点集合管�?==========
    
    /// 添加点到多边�?
    void AddPoint(const Point& point);
    
    /// 设置所有点
    void SetPoints(const std::vector<Point>& points);
    
    /// 获取所有点
    const std::vector<Point>& GetPoints() const { return points_; }
    
    /// 清空所有点
    void ClearPoints();
    
    /// 获取点数�?
    size_t GetPointCount() const { return points_.size(); }
    
    /// 获取指定索引的点
    Point GetPoint(size_t index) const;
    
    /// 流式API：设置点集合
    Polygon* Points(const std::vector<Point>& points);

protected:
    Rect GetDefiningGeometry() const;
    void OnRender(render::RenderContext& context);

private:
    std::vector<Point> points_;
};

/**
 * @brief 路径图形
 * 
 * 支持复杂几何路径的图形类，包括直线、贝塞尔曲线、弧等�?
 */
class Path : public Shape<Path> {
public:
    Path() = default;
    virtual ~Path() = default;

    // ========== 路径命令枚举 ==========
    
    enum class PathCommand {
        MoveTo,         // 移动到点
        LineTo,         // 直线到点
        QuadraticTo,    // 二次贝塞尔曲�?
        CubicTo,        // 三次贝塞尔曲�?
        ArcTo,          // 弧线
        Close           // 闭合路径
    };

    // ========== 路径段结�?==========
    
    struct PathSegment {
        PathCommand command;
        std::vector<Point> points;  // 根据命令类型,点数不同
        render::Color strokeColor;           // 该段的描边颜�?可�?
        bool hasStrokeColor{false};  // 是否设置了分段颜�?
        render::Color fillColor;             // 该子路径的填充颜�?可�?仅MoveTo有效)
        bool hasFillColor{false};    // 是否设置了子路径填充颜色
        render::Color subPathStrokeColor;    // 该子路径的描边颜�?可�?仅MoveTo有效)
        float subPathStrokeThickness{0.0f};  // 该子路径的描边粗�?可�?仅MoveTo有效)
        bool hasSubPathStroke{false}; // 是否设置了子路径描边
        
        PathSegment(PathCommand cmd) : command(cmd), strokeColor(0,0,0,0), fillColor(0,0,0,0), subPathStrokeColor(0,0,0,0) {}
        PathSegment(PathCommand cmd, const Point& p) 
            : command(cmd), points{p}, strokeColor(0,0,0,0), fillColor(0,0,0,0), subPathStrokeColor(0,0,0,0) {}
        PathSegment(PathCommand cmd, const std::vector<Point>& pts) 
            : command(cmd), points(pts), strokeColor(0,0,0,0), fillColor(0,0,0,0), subPathStrokeColor(0,0,0,0) {}
    };

    // ========== 路径构建API ==========
    
    /// 移动到指定点（开始新的子路径�?
    Path* MoveTo(const Point& point);
    Path* MoveTo(float x, float y);
    
    /// 直线到指定点
    Path* LineTo(const Point& point);
    Path* LineTo(float x, float y);
    
    /// 二次贝塞尔曲线（1个控制点 + 终点�?
    Path* QuadraticTo(const Point& control, const Point& end);
    Path* QuadraticTo(float cx, float cy, float ex, float ey);
    
    /// 三次贝塞尔曲线（2个控制点 + 终点�?
    Path* CubicTo(const Point& control1, const Point& control2, const Point& end);
    Path* CubicTo(float c1x, float c1y, float c2x, float c2y, float ex, float ey);
    
    /// 圆弧（TODO: 需要定义弧参数�?
    Path* ArcTo(const Point& end, float radiusX, float radiusY, float angle = 0.0f, bool largeArc = false, bool sweep = false);
    
    /// 闭合当前路径
    Path* Close();
    
    /// 设置当前段的描边颜色(必须在添加段后立即调�?
    Path* SetSegmentStroke(const render::Color& color);
    Path* SetSegmentStroke(float r, float g, float b, float a = 1.0f);
    
    /// 设置当前子路径的填充颜色(在MoveTo后调�?
    Path* SetSubPathFill(const render::Color& color);
    Path* SetSubPathFill(float r, float g, float b, float a = 1.0f);
    
    /// 设置当前子路径的描边(在MoveTo后调�?
    Path* SetSubPathStroke(const render::Color& color, float thickness);
    Path* SetSubPathStroke(float r, float g, float b, float a, float thickness);
    
    /// 清空路径
    void ClearPath();
    
    /// 获取所有路径段
    const std::vector<PathSegment>& GetSegments() const { return segments_; }

protected:
    Rect GetDefiningGeometry() const;
    void OnRender(render::RenderContext& context);

private:
    std::vector<PathSegment> segments_;
    Point currentPoint_{0, 0};  // 当前绘制位置
};

} // namespace fk::ui
