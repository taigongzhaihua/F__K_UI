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

// ========== 模板显式实例化 ==========
// 只需实例化 Shape，Rectangle 和 Ellipse 继承自 Shape
template class FrameworkElement<Shape>;

} // namespace fk::ui

// =============================================================================
// Polygon 实现
// =============================================================================

Polygon::Polygon() {
    // 构造函数初始化
}

// ========== Polygon 属性依赖属性 ==========

const binding::DependencyProperty& Polygon::PointsProperty() {
    static auto& property = binding::DependencyProperty::Register(
        "Points",
        typeid(std::vector<Point>),
        typeid(Polygon),
        binding::PropertyMetadata{
            std::any(std::vector<Point>{}),
            &Polygon::OnPointsChanged
        }
    );
    return property;
}

const binding::DependencyProperty& Polygon::FillRuleProperty() {
    static auto& property = binding::DependencyProperty::Register(
        "FillRule",
        typeid(int),  // 0=EvenOdd, 1=NonZero
        typeid(Polygon),
        binding::PropertyMetadata{
            std::any(0),  // Default: EvenOdd
            nullptr
        }
    );
    return property;
}

// ========== Polygon 属性访问器 ==========

std::vector<Point> Polygon::GetPoints() const {
    return GetValue<std::vector<Point>>(PointsProperty());
}

void Polygon::SetPoints(const std::vector<Point>& value) {
    SetValue(PointsProperty(), value);
}

Polygon* Polygon::Points(const std::vector<Point>& value) {
    SetPoints(value);
    return this;
}

int Polygon::GetFillRule() const {
    return GetValue<int>(FillRuleProperty());
}

void Polygon::SetFillRule(int value) {
    SetValue(FillRuleProperty(), value);
}

Polygon* Polygon::FillRule(int value) {
    SetFillRule(value);
    return this;
}

// ========== Polygon 几何与渲染 ==========

Rect Polygon::GetDefiningGeometry() const {
    auto points = GetPoints();
    if (points.empty()) {
        return Rect(0, 0, 0, 0);
    }
    
    float minX = points[0].x;
    float minY = points[0].y;
    float maxX = points[0].x;
    float maxY = points[0].y;
    
    for (size_t i = 1; i < points.size(); ++i) {
        minX = std::min(minX, points[i].x);
        minY = std::min(minY, points[i].y);
        maxX = std::max(maxX, points[i].x);
        maxY = std::max(maxY, points[i].y);
    }
    
    return Rect(minX, minY, maxX - minX, maxY - minY);
}

void Polygon::OnRender(RenderContext& context) {
    Renderer* renderer = context.GetRenderer();
    if (!renderer) return;
    
    auto points = GetPoints();
    if (points.size() < 3) return;  // 需要至少3个点才能构成多边形
    
    // Convert Brush* to Color (simplified)
    Color fillColor = Color::White();
    Color strokeColor = Color::Black();
    
    if (GetFill() != nullptr) {
        // TODO: Extract color from SolidColorBrush
        fillColor = Color::White();
    }
    
    if (GetStroke() != nullptr) {
        // TODO: Extract color from SolidColorBrush
        strokeColor = Color::Black();
    }
    
    float strokeThickness = GetStrokeThickness();
    
    // 绘制多边形（闭合路径）
    // TODO: Implement DrawPolygon in Renderer
    // For now, draw lines between consecutive points and close the shape
    for (size_t i = 0; i < points.size(); ++i) {
        size_t next = (i + 1) % points.size();
        renderer->DrawLine(points[i], points[next], strokeColor, strokeThickness);
    }
}

void Polygon::OnPointsChanged(
    binding::DependencyObject& d,
    const binding::DependencyProperty& prop,
    const std::any& oldValue,
    const std::any& newValue
) {
    auto* polygon = dynamic_cast<Polygon*>(&d);
    if (polygon) {
        polygon->InvalidateMeasure();
        polygon->InvalidateVisual();
    }
}

// =============================================================================
// Path 实现
// =============================================================================

Path::Path() {
    // 构造函数初始化
}

// ========== Path 属性依赖属性 ==========

const binding::DependencyProperty& Path::DataProperty() {
    static auto& property = binding::DependencyProperty::Register(
        "Data",
        typeid(std::string),  // Path data string (SVG-like syntax)
        typeid(Path),
        binding::PropertyMetadata{
            std::any(std::string{}),
            &Path::OnDataChanged
        }
    );
    return property;
}

// ========== Path 属性访问器 ==========

std::string Path::GetData() const {
    return GetValue<std::string>(DataProperty());
}

void Path::SetData(const std::string& value) {
    SetValue(DataProperty(), value);
}

Path* Path::Data(const std::string& value) {
    SetData(value);
    return this;
}

// ========== Path 几何与渲染 ==========

Rect Path::GetDefiningGeometry() const {
    // Parse path data to compute bounding box
    // Simplified implementation - returns empty rect for now
    // TODO: Implement full path data parsing
    return Rect(0, 0, 0, 0);
}

void Path::OnRender(RenderContext& context) {
    Renderer* renderer = context.GetRenderer();
    if (!renderer) return;
    
    if (segments_.empty()) return;
    
    // Render path using segments
    Color fillColor = GetFill();
    Color strokeColor = GetStroke();
    float strokeWidth = GetStrokeThickness();
    
    // Begin path rendering
    // TODO: Implement actual path rendering using DrawCommand system
    // This would require extending DrawCommand to support path operations
    
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
                    // Draw line from lastPoint to segment.points[0]
                    // TODO: Use actual line drawing
                    lastPoint = segment.points[0];
                }
                break;
                
            case PathCommand::QuadraticTo:
                if (segment.points.size() >= 2) {
                    // Approximate quadratic bezier with line segments
                    // control = segment.points[0], end = segment.points[1]
                    lastPoint = segment.points[1];
                }
                break;
                
            case PathCommand::CubicTo:
                if (segment.points.size() >= 3) {
                    // Approximate cubic bezier with line segments
                    // control1 = segment.points[0], control2 = segment.points[1], end = segment.points[2]
                    lastPoint = segment.points[2];
                }
                break;
                
            case PathCommand::ArcTo:
                if (segment.points.size() >= 1) {
                    // Approximate arc with line segments
                    lastPoint = segment.points[0];
                }
                break;
                
            case PathCommand::Close:
                // Close the current sub-path
                break;
        }
    }
}

void Path::OnDataChanged(
    binding::DependencyObject& d,
    const binding::DependencyProperty& prop,
    const std::any& oldValue,
    const std::any& newValue
) {
    auto* path = dynamic_cast<Path*>(&d);
    if (path) {
        path->InvalidateMeasure();
        path->InvalidateVisual();
    }
}

// Path building API implementation

Path* Path::MoveTo(float x, float y) {
    PathSegment segment;
    segment.command = PathCommand::MoveTo;
    segment.points.push_back(Point(x, y));
    segments_.push_back(segment);
    
    InvalidateMeasure();
    InvalidateVisual();
    return this;
}

Path* Path::LineTo(float x, float y) {
    PathSegment segment;
    segment.command = PathCommand::LineTo;
    segment.points.push_back(Point(x, y));
    segments_.push_back(segment);
    
    InvalidateMeasure();
    InvalidateVisual();
    return this;
}

Path* Path::QuadraticTo(float cx, float cy, float x, float y) {
    PathSegment segment;
    segment.command = PathCommand::QuadraticTo;
    segment.points.push_back(Point(cx, cy));  // Control point
    segment.points.push_back(Point(x, y));    // End point
    segments_.push_back(segment);
    
    InvalidateMeasure();
    InvalidateVisual();
    return this;
}

Path* Path::CubicTo(float cx1, float cy1, float cx2, float cy2, float x, float y) {
    PathSegment segment;
    segment.command = PathCommand::CubicTo;
    segment.points.push_back(Point(cx1, cy1));  // First control point
    segment.points.push_back(Point(cx2, cy2));  // Second control point
    segment.points.push_back(Point(x, y));      // End point
    segments_.push_back(segment);
    
    InvalidateMeasure();
    InvalidateVisual();
    return this;
}

Path* Path::ArcTo(float rx, float ry, float angle, bool largeArc, bool sweep, float x, float y) {
    PathSegment segment;
    segment.command = PathCommand::ArcTo;
    segment.points.push_back(Point(rx, ry));         // Radii
    segment.points.push_back(Point(angle, 0));       // Rotation angle
    segment.points.push_back(Point(largeArc ? 1.0f : 0.0f, sweep ? 1.0f : 0.0f));  // Flags
    segment.points.push_back(Point(x, y));           // End point
    segments_.push_back(segment);
    
    InvalidateMeasure();
    InvalidateVisual();
    return this;
}

Path* Path::Close() {
    PathSegment segment;
    segment.command = PathCommand::Close;
    segments_.push_back(segment);
    
    InvalidateMeasure();
    InvalidateVisual();
    return this;
}

void Path::ClearPath() {
    segments_.clear();
    InvalidateMeasure();
    InvalidateVisual();
}

