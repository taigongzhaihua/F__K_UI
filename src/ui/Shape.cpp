/**
 * @file Shape.cpp
 * @brief Shape 图形实现
 */

#include "fk/ui/Shape.h"
#include "fk/ui/Renderer.h"
#include "fk/ui/Primitives.h"
#include <algorithm>

// 包含 FrameworkElement 模板实现（用于显式实例化）
#include "FrameworkElement.cpp"

namespace fk::ui {

// 前向声明
class Brush;

// ========== Shape 依赖属性 ==========

const binding::DependencyProperty& Shape::FillProperty() {
    static auto& property = binding::DependencyProperty::Register(
        "Fill",
        typeid(Brush*),
        typeid(Shape),
        {static_cast<Brush*>(nullptr)}
    );
    return property;
}

const binding::DependencyProperty& Shape::StrokeProperty() {
    static auto& property = binding::DependencyProperty::Register(
        "Stroke",
        typeid(Brush*),
        typeid(Shape),
        {static_cast<Brush*>(nullptr)}
    );
    return property;
}

const binding::DependencyProperty& Shape::StrokeThicknessProperty() {
    static auto& property = binding::DependencyProperty::Register(
        "StrokeThickness",
        typeid(float),
        typeid(Shape),
        {1.0f}
    );
    return property;
}

// ========== Shape 属性访问 ==========

Brush* Shape::GetFill() const {
    return GetValue<Brush*>(FillProperty());
}

void Shape::SetFill(Brush* value) {
    SetValue(FillProperty(), value);
}

Shape* Shape::Fill(Brush* brush) {
    SetFill(brush);
    return this;
}

Brush* Shape::Fill() const {
    return GetFill();
}

Brush* Shape::GetStroke() const {
    return GetValue<Brush*>(StrokeProperty());
}

void Shape::SetStroke(Brush* value) {
    SetValue(StrokeProperty(), value);
}

Shape* Shape::Stroke(Brush* brush) {
    SetStroke(brush);
    return this;
}

Brush* Shape::Stroke() const {
    return GetStroke();
}

float Shape::GetStrokeThickness() const {
    return GetValue<float>(StrokeThicknessProperty());
}

void Shape::SetStrokeThickness(float value) {
    SetValue(StrokeThicknessProperty(), value);
}

Shape* Shape::StrokeThickness(float thickness) {
    SetStrokeThickness(thickness);
    return this;
}

float Shape::StrokeThickness() const {
    return GetStrokeThickness();
}

// ========== Shape 布局 ==========

Size Shape::MeasureOverride(const Size& availableSize) {
    Rect bounds = GetDefiningGeometry();
    float strokeThickness = GetStrokeThickness();
    
    return Size(
        bounds.width + strokeThickness,
        bounds.height + strokeThickness
    );
}

Size Shape::ArrangeOverride(const Size& finalSize) {
    return finalSize;
}

void Shape::OnRender(RenderContext& context) {
    // 派生类实现具体渲染
}

void Shape::CollectDrawCommands(RenderContext& context) {
    // Call OnRender to submit draw commands for this shape
    OnRender(context);
    
    // Shapes don't have children, so no need to traverse
}

// ========== Rectangle 依赖属性 ==========

const binding::DependencyProperty& Rectangle::RadiusXProperty() {
    static auto& property = binding::DependencyProperty::Register(
        "RadiusX",
        typeid(float),
        typeid(Rectangle),
        {0.0f}
    );
    return property;
}

const binding::DependencyProperty& Rectangle::RadiusYProperty() {
    static auto& property = binding::DependencyProperty::Register(
        "RadiusY",
        typeid(float),
        typeid(Rectangle),
        {0.0f}
    );
    return property;
}

// ========== Rectangle 属性访问 ==========

float Rectangle::GetRadiusX() const {
    return GetValue<float>(RadiusXProperty());
}

void Rectangle::SetRadiusX(float value) {
    SetValue(RadiusXProperty(), value);
}

Rectangle* Rectangle::RadiusX(float radius) {
    SetRadiusX(radius);
    return this;
}

float Rectangle::GetRadiusY() const {
    return GetValue<float>(RadiusYProperty());
}

void Rectangle::SetRadiusY(float value) {
    SetValue(RadiusYProperty(), value);
}

Rectangle* Rectangle::RadiusY(float radius) {
    SetRadiusY(radius);
    return this;
}

// ========== Rectangle 几何与渲染 ==========

Rect Rectangle::GetDefiningGeometry() const {
    // 矩形边界由布局决定
    Size size = GetRenderSize();
    return Rect(0, 0, size.width, size.height);
}

void Rectangle::OnRender(RenderContext& context) {
    Renderer* renderer = context.GetRenderer();
    if (!renderer) return;
    
    Rect bounds = GetDefiningGeometry();
    float radiusX = GetRadiusX();
    float radiusY = GetRadiusY();
    
    // Convert Brush* to Color (simplified - assumes SolidColorBrush)
    // TODO: Proper Brush system implementation
    Color fillColor = Color::Transparent();
    Color strokeColor = Color::Transparent();
    
    // For now, use placeholder colors if brushes are set
    if (GetFill() != nullptr) {
        fillColor = Color(0.7f, 0.7f, 0.7f, 1.0f);  // Gray fill as placeholder
    }
    if (GetStroke() != nullptr) {
        strokeColor = Color::Black();
    }
    
    float strokeThickness = GetStrokeThickness();
    
    // Draw rounded rectangle if radius specified, otherwise regular rectangle
    if (radiusX > 0 || radiusY > 0) {
        float radius = std::max(radiusX, radiusY);
        renderer->DrawRoundedRectangle(bounds, radius, fillColor, strokeColor);
    } else {
        renderer->DrawRectangle(bounds, fillColor, strokeColor, strokeThickness);
    }
}

// ========== Ellipse 几何与渲染 ==========

Rect Ellipse::GetDefiningGeometry() const {
    // 椭圆边界由布局决定
    Size size = GetRenderSize();
    return Rect(0, 0, size.width, size.height);
}

void Ellipse::OnRender(RenderContext& context) {
    Renderer* renderer = context.GetRenderer();
    if (!renderer) return;
    
    Rect bounds = GetDefiningGeometry();
    
    // Calculate center and radius from bounds
    Point center(bounds.x + bounds.width / 2.0f, bounds.y + bounds.height / 2.0f);
    float radius = std::min(bounds.width, bounds.height) / 2.0f;
    
    // Convert Brush* to Color (simplified - assumes SolidColorBrush)
    // TODO: Proper Brush system implementation
    Color fillColor = Color::Transparent();
    Color strokeColor = Color::Transparent();
    
    // For now, use placeholder colors if brushes are set
    if (GetFill() != nullptr) {
        fillColor = Color(0.7f, 0.7f, 0.7f, 1.0f);  // Gray fill as placeholder
    }
    if (GetStroke() != nullptr) {
        strokeColor = Color::Black();
    }
    
    renderer->DrawCircle(center, radius, fillColor, strokeColor);
}

// ========== Line 依赖属性 ==========

const binding::DependencyProperty& Line::X1Property() {
    static auto& property = binding::DependencyProperty::Register(
        "X1",
        typeid(float),
        typeid(Line),
        {0.0f}
    );
    return property;
}

const binding::DependencyProperty& Line::Y1Property() {
    static auto& property = binding::DependencyProperty::Register(
        "Y1",
        typeid(float),
        typeid(Line),
        {0.0f}
    );
    return property;
}

const binding::DependencyProperty& Line::X2Property() {
    static auto& property = binding::DependencyProperty::Register(
        "X2",
        typeid(float),
        typeid(Line),
        {0.0f}
    );
    return property;
}

const binding::DependencyProperty& Line::Y2Property() {
    static auto& property = binding::DependencyProperty::Register(
        "Y2",
        typeid(float),
        typeid(Line),
        {0.0f}
    );
    return property;
}

// ========== Line 属性访问 ==========

float Line::GetX1() const {
    return GetValue<float>(X1Property());
}

void Line::SetX1(float value) {
    SetValue(X1Property(), value);
}

Line* Line::X1(float x) {
    SetX1(x);
    return this;
}

float Line::GetY1() const {
    return GetValue<float>(Y1Property());
}

void Line::SetY1(float value) {
    SetValue(Y1Property(), value);
}

Line* Line::Y1(float y) {
    SetY1(y);
    return this;
}

float Line::GetX2() const {
    return GetValue<float>(X2Property());
}

void Line::SetX2(float value) {
    SetValue(X2Property(), value);
}

Line* Line::X2(float x) {
    SetX2(x);
    return this;
}

float Line::GetY2() const {
    return GetValue<float>(Y2Property());
}

void Line::SetY2(float value) {
    SetValue(Y2Property(), value);
}

Line* Line::Y2(float y) {
    SetY2(y);
    return this;
}

// ========== Line 几何与渲染 ==========

Rect Line::GetDefiningGeometry() const {
    float x1 = GetX1();
    float y1 = GetY1();
    float x2 = GetX2();
    float y2 = GetY2();
    
    float minX = std::min(x1, x2);
    float minY = std::min(y1, y2);
    float maxX = std::max(x1, x2);
    float maxY = std::max(y1, y2);
    
    return Rect(minX, minY, maxX - minX, maxY - minY);
}

void Line::OnRender(RenderContext& context) {
    Renderer* renderer = context.GetRenderer();
    if (!renderer) return;
    
    Point start(GetX1(), GetY1());
    Point end(GetX2(), GetY2());
    
    // Convert Brush* to Color (simplified)
    // Line uses Stroke for color, not Fill
    Color lineColor = Color::Black();  // Default
    
    if (GetStroke() != nullptr) {
        // TODO: Extract color from SolidColorBrush
        lineColor = Color::Black();
    }
    
    float strokeThickness = GetStrokeThickness();
    
    renderer->DrawLine(start, end, lineColor, strokeThickness);
}

// ========== Polygon 点集合管理 ==========

void Polygon::AddPoint(const Point& point) {
    points_.push_back(point);
    InvalidateMeasure();
    InvalidateVisual();
}

void Polygon::SetPoints(const std::vector<Point>& points) {
    points_ = points;
    InvalidateMeasure();
    InvalidateVisual();
}

void Polygon::ClearPoints() {
    points_.clear();
    InvalidateMeasure();
    InvalidateVisual();
}

Point Polygon::GetPoint(size_t index) const {
    if (index >= points_.size()) {
        return Point(0, 0);
    }
    return points_[index];
}

Polygon* Polygon::Points(const std::vector<Point>& points) {
    SetPoints(points);
    return this;
}

// ========== Polygon 几何与渲染 ==========

Rect Polygon::GetDefiningGeometry() const {
    if (points_.empty()) {
        return Rect(0, 0, 0, 0);
    }
    
    float minX = points_[0].x;
    float minY = points_[0].y;
    float maxX = points_[0].x;
    float maxY = points_[0].y;
    
    for (size_t i = 1; i < points_.size(); ++i) {
        minX = std::min(minX, points_[i].x);
        minY = std::min(minY, points_[i].y);
        maxX = std::max(maxX, points_[i].x);
        maxY = std::max(maxY, points_[i].y);
    }
    
    return Rect(minX, minY, maxX - minX, maxY - minY);
}

void Polygon::OnRender(RenderContext& context) {
    Renderer* renderer = context.GetRenderer();
    if (!renderer) return;
    
    if (points_.size() < 3) return;  // 需要至少3个点才能构成多边形
    
    // Convert Brush* to Color (simplified)
    Color fillColor = Color::Transparent();
    Color strokeColor = Color::Transparent();
    
    if (GetFill() != nullptr) {
        fillColor = Color(0.7f, 0.7f, 0.7f, 1.0f);  // Gray fill as placeholder
    }
    
    if (GetStroke() != nullptr) {
        strokeColor = Color::Black();
    }
    
    float strokeThickness = GetStrokeThickness();
    
    // 绘制多边形（闭合路径）
    // TODO: Implement DrawPolygon in Renderer
    // For now, draw lines between consecutive points and close the shape
    for (size_t i = 0; i < points_.size(); ++i) {
        size_t next = (i + 1) % points_.size();
        renderer->DrawLine(points_[i], points_[next], strokeColor, strokeThickness);
    }
}

// ========== Path 路径构建API ==========

Path* Path::MoveTo(const Point& point) {
    PathSegment segment(PathCommand::MoveTo, point);
    segments_.push_back(segment);
    currentPoint_ = point;
    InvalidateMeasure();
    InvalidateVisual();
    return this;
}

Path* Path::MoveTo(float x, float y) {
    return MoveTo(Point(x, y));
}

Path* Path::LineTo(const Point& point) {
    PathSegment segment(PathCommand::LineTo, point);
    segments_.push_back(segment);
    currentPoint_ = point;
    InvalidateMeasure();
    InvalidateVisual();
    return this;
}

Path* Path::LineTo(float x, float y) {
    return LineTo(Point(x, y));
}

Path* Path::QuadraticTo(const Point& control, const Point& end) {
    PathSegment segment(PathCommand::QuadraticTo);
    segment.points.push_back(control);
    segment.points.push_back(end);
    segments_.push_back(segment);
    currentPoint_ = end;
    InvalidateMeasure();
    InvalidateVisual();
    return this;
}

Path* Path::QuadraticTo(float cx, float cy, float ex, float ey) {
    return QuadraticTo(Point(cx, cy), Point(ex, ey));
}

Path* Path::CubicTo(const Point& control1, const Point& control2, const Point& end) {
    PathSegment segment(PathCommand::CubicTo);
    segment.points.push_back(control1);
    segment.points.push_back(control2);
    segment.points.push_back(end);
    segments_.push_back(segment);
    currentPoint_ = end;
    InvalidateMeasure();
    InvalidateVisual();
    return this;
}

Path* Path::CubicTo(float c1x, float c1y, float c2x, float c2y, float ex, float ey) {
    return CubicTo(Point(c1x, c1y), Point(c2x, c2y), Point(ex, ey));
}

Path* Path::ArcTo(const Point& end, float radiusX, float radiusY, float angle, bool largeArc, bool sweep) {
    PathSegment segment(PathCommand::ArcTo);
    segment.points.push_back(Point(radiusX, radiusY));
    segment.points.push_back(Point(angle, 0));
    segment.points.push_back(Point(largeArc ? 1.0f : 0.0f, sweep ? 1.0f : 0.0f));
    segment.points.push_back(end);
    segments_.push_back(segment);
    currentPoint_ = end;
    InvalidateMeasure();
    InvalidateVisual();
    return this;
}

Path* Path::Close() {
    PathSegment segment(PathCommand::Close);
    segments_.push_back(segment);
    InvalidateMeasure();
    InvalidateVisual();
    return this;
}

void Path::ClearPath() {
    segments_.clear();
    currentPoint_ = Point(0, 0);
    InvalidateMeasure();
    InvalidateVisual();
}

// ========== Path 几何与渲染 ==========

Rect Path::GetDefiningGeometry() const {
    // Parse path segments to compute bounding box
    // Simplified implementation - returns empty rect for now
    // TODO: Implement full path bounding box calculation
    if (segments_.empty()) {
        return Rect(0, 0, 0, 0);
    }
    
    float minX = 0, minY = 0, maxX = 0, maxY = 0;
    bool first = true;
    
    for (const auto& segment : segments_) {
        for (const auto& pt : segment.points) {
            if (first) {
                minX = maxX = pt.x;
                minY = maxY = pt.y;
                first = false;
            } else {
                minX = std::min(minX, pt.x);
                minY = std::min(minY, pt.y);
                maxX = std::max(maxX, pt.x);
                maxY = std::max(maxY, pt.y);
            }
        }
    }
    
    return Rect(minX, minY, maxX - minX, maxY - minY);
}

void Path::OnRender(RenderContext& context) {
    Renderer* renderer = context.GetRenderer();
    if (!renderer) return;
    
    if (segments_.empty()) return;
    
    // Convert Brush* to Color (simplified)
    Color strokeColor = Color::Black();
    if (GetStroke() != nullptr) {
        strokeColor = Color::Black();  // TODO: Extract from brush
    }
    
    float strokeThickness = GetStrokeThickness();
    
    // Render path using segments
    // For now, approximate with line segments
    Point lastPoint(0, 0);
    for (const auto& segment : segments_) {
        switch (segment.command) {
            case PathCommand::MoveTo:
                if (!segment.points.empty()) {
                    lastPoint = segment.points[0];
                }
                break;
                
            case PathCommand::LineTo:
                if (!segment.points.empty()) {
                    renderer->DrawLine(lastPoint, segment.points[0], strokeColor, strokeThickness);
                    lastPoint = segment.points[0];
                }
                break;
                
            case PathCommand::QuadraticTo:
            case PathCommand::CubicTo:
            case PathCommand::ArcTo:
                // TODO: Implement proper curve rendering
                if (!segment.points.empty()) {
                    renderer->DrawLine(lastPoint, segment.points.back(), strokeColor, strokeThickness);
                    lastPoint = segment.points.back();
                }
                break;
                
            case PathCommand::Close:
                // Close path - would draw back to start point
                break;
        }
    }
}

// ========== 模板显式实例化 ==========
// 只需实例化 Shape，Rectangle 和 Ellipse 继承自 Shape
template class FrameworkElement<Shape>;

} // namespace fk::ui
