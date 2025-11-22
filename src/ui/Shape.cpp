/**
 * @file Shape.cpp
 * @brief Shape 图形实现
 */

#include "fk/ui/Shape.h"
#include "fk/ui/Brush.h"
#include "fk/ui/Primitives.h"
#include "fk/render/RenderContext.h"
#include <algorithm>

// 包含 FrameworkElement 模板实现（用于显式实例化）
#include "FrameworkElement.cpp"

namespace fk::ui {

// ========== Shape 依赖属性 ==========

template<typename Derived>
const binding::DependencyProperty& Shape<Derived>::FillProperty() {
    static auto& property = binding::DependencyProperty::Register(
        "Fill",
        typeid(Brush*),
        typeid(Derived),
        {static_cast<Brush*>(nullptr)}
    );
    return property;
}

template<typename Derived>
const binding::DependencyProperty& Shape<Derived>::StrokeProperty() {
    static auto& property = binding::DependencyProperty::Register(
        "Stroke",
        typeid(Brush*),
        typeid(Derived),
        {static_cast<Brush*>(nullptr)}
    );
    return property;
}

template<typename Derived>
const binding::DependencyProperty& Shape<Derived>::StrokeThicknessProperty() {
    static auto& property = binding::DependencyProperty::Register(
        "StrokeThickness",
        typeid(float),
        typeid(Derived),
        {1.0f}
    );
    return property;
}

// ========== Shape 属性访问 ==========

template<typename Derived>
Brush* Shape<Derived>::GetFill() const {
    return this->template GetValue<Brush*>(FillProperty());
}

template<typename Derived>
void Shape<Derived>::SetFill(Brush* value) {
    this->SetValue(FillProperty(), value);
}

template<typename Derived>
Brush* Shape<Derived>::GetStroke() const {
    return this->template GetValue<Brush*>(StrokeProperty());
}

template<typename Derived>
void Shape<Derived>::SetStroke(Brush* value) {
    this->SetValue(StrokeProperty(), value);
}

template<typename Derived>
float Shape<Derived>::GetStrokeThickness() const {
    return this->template GetValue<float>(StrokeThicknessProperty());
}

template<typename Derived>
void Shape<Derived>::SetStrokeThickness(float value) {
    this->SetValue(StrokeThicknessProperty(), value);
}

// ========== Shape 布局 ==========

template<typename Derived>
Size Shape<Derived>::MeasureOverride(const Size& availableSize) {
    // Shape 使用显式设置的 Width/Height，如果没有设置则使用几何边界
    float width = this->GetWidth();
    float height = this->GetHeight();
    
    // 如果没有显式设置 Width/Height (默认值为 -1.0)，使用几何边界
    if (width <= 0.0f || height <= 0.0f) {
        Rect bounds = GetDefiningGeometry();
        if (width <= 0.0f) width = bounds.width;
        if (height <= 0.0f) height = bounds.height;
    }
    
    // GetDefiningGeometry 已经包含了线宽的计算，直接返回
    return Size(width, height);
}

template<typename Derived>
Size Shape<Derived>::ArrangeOverride(const Size& finalSize) {
    return finalSize;
}

template<typename Derived>
void Shape<Derived>::OnRender(render::RenderContext& context) {
    // 派生类实现具体渲染
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

void Rectangle::OnRender(render::RenderContext& context) {
    Rect bounds = GetDefiningGeometry();
    float radiusX = GetRadiusX();
    float radiusY = GetRadiusY();
    
    // 从 Brush 获取颜色（转换为 RenderContext 格式）
    auto brushToColor = [](Brush* brush) -> std::array<float, 4> {
        if (!brush) return {{0.0f, 0.0f, 0.0f, 0.0f}};  // 透明
        if (auto solidBrush = dynamic_cast<SolidColorBrush*>(brush)) {
            auto color = solidBrush->GetColor();
            return {{color.r, color.g, color.b, color.a}};
        }
        return {{0.0f, 0.0f, 0.0f, 0.0f}};
    };
    
    std::array<float, 4> fillColor = brushToColor(GetFill());
    std::array<float, 4> strokeColor = brushToColor(GetStroke());
    float strokeThickness = GetStrokeThickness();
    
    // 使用新的 DrawRectangle API，支持 radiusX 和 radiusY
    // 符合 WPF Rectangle 的语义：四个角使用相同的椭圆半径
    context.DrawRectangle(bounds, fillColor, strokeColor, strokeThickness, radiusX, radiusY);
}

// ========== Ellipse 几何与渲染 ==========

Rect Ellipse::GetDefiningGeometry() const {
    // 椭圆边界由布局决定
    Size size = GetRenderSize();
    return Rect(0, 0, size.width, size.height);
}

void Ellipse::OnRender(render::RenderContext& context) {
    Rect bounds = GetDefiningGeometry();
    
    // 从 Brush 获取颜色（转换为 RenderContext 格式）
    auto brushToColor = [](Brush* brush) -> std::array<float, 4> {
        if (!brush) return {{0.0f, 0.0f, 0.0f, 0.0f}};  // 透明
        if (auto solidBrush = dynamic_cast<SolidColorBrush*>(brush)) {
            auto color = solidBrush->GetColor();
            return {{color.r, color.g, color.b, color.a}};
        }
        return {{0.0f, 0.0f, 0.0f, 0.0f}};
    };
    
    std::array<float, 4> fillColor = brushToColor(GetFill());
    std::array<float, 4> strokeColor = brushToColor(GetStroke());
    float strokeThickness = GetStrokeThickness();
    
    // 使用 RenderContext 绘制椭圆
    context.DrawEllipse(bounds, fillColor, strokeColor, strokeThickness);
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
    float halfThickness = GetStrokeThickness() / 2.0f;
    
    // 计算线段的方向向量
    float dx = x2 - x1;
    float dy = y2 - y1;
    float length = std::sqrt(dx * dx + dy * dy);
    
    if (length < 0.001f) {
        // 退化为点，返回一个以线宽为半径的正方形
        float size = halfThickness * 2.0f;
        return Rect(0.0f, 0.0f, size, size);
    }
    
    // 计算垂直于线段的单位向量
    float perpX = -dy / length;
    float perpY = dx / length;
    
    // 线段端点沿垂直方向扩展半个线宽后的四个顶点
    float p1x1 = x1 + perpX * halfThickness;
    float p1y1 = y1 + perpY * halfThickness;
    float p1x2 = x1 - perpX * halfThickness;
    float p1y2 = y1 - perpY * halfThickness;
    
    float p2x1 = x2 + perpX * halfThickness;
    float p2y1 = y2 + perpY * halfThickness;
    float p2x2 = x2 - perpX * halfThickness;
    float p2y2 = y2 - perpY * halfThickness;
    
    // 找出四个顶点的包围盒
    float minX = std::min({p1x1, p1x2, p2x1, p2x2});
    float minY = std::min({p1y1, p1y2, p2y1, p2y2});
    float maxX = std::max({p1x1, p1x2, p2x1, p2x2});
    float maxY = std::max({p1y1, p1y2, p2y1, p2y2});
    
    float width = maxX - minX;
    float height = maxY - minY;
    
    // 返回从 (0, 0) 开始的矩形，控件内容相对于自身原点
    return Rect(0.0f, 0.0f, width, height);
}

void Line::OnRender(render::RenderContext& context) {
    float x1 = GetX1();
    float y1 = GetY1();
    float x2 = GetX2();
    float y2 = GetY2();
    float halfThickness = GetStrokeThickness() / 2.0f;
    
    // 计算线段的方向向量和垂直向量（与 GetDefiningGeometry 相同的计算）
    float dx = x2 - x1;
    float dy = y2 - y1;
    float length = std::sqrt(dx * dx + dy * dy);
    
    float perpX = 0.0f, perpY = 0.0f;
    if (length >= 0.001f) {
        perpX = -dy / length;
        perpY = dx / length;
    }
    
    // 计算四个顶点来找到偏移量
    float p1x1 = x1 + perpX * halfThickness;
    float p1y1 = y1 + perpY * halfThickness;
    float p1x2 = x1 - perpX * halfThickness;
    float p1y2 = y1 - perpY * halfThickness;
    float p2x1 = x2 + perpX * halfThickness;
    float p2y1 = y2 + perpY * halfThickness;
    float p2x2 = x2 - perpX * halfThickness;
    float p2y2 = y2 - perpY * halfThickness;
    
    float minX = std::min({p1x1, p1x2, p2x1, p2x2});
    float minY = std::min({p1y1, p1y2, p2y1, p2y2});
    
    // 调整坐标到控件坐标系
    Point start(x1 - minX, y1 - minY);
    Point end(x2 - minX, y2 - minY);
    
    // Line 使用 Stroke 作为线条颜色，不使用 Fill
    auto brushToColor = [](Brush* brush) -> std::array<float, 4> {
        if (!brush) return {{0.0f, 0.0f, 0.0f, 0.0f}};  // 透明
        if (auto solidBrush = dynamic_cast<SolidColorBrush*>(brush)) {
            auto color = solidBrush->GetColor();
            return {{color.r, color.g, color.b, color.a}};
        }
        return {{0.0f, 0.0f, 0.0f, 0.0f}};
    };
    
    std::array<float, 4> lineColor = brushToColor(GetStroke());
    
    // 如果没有设置 Stroke，默认使用黑色
    if (lineColor[3] == 0.0f) {
        lineColor = {{0.0f, 0.0f, 0.0f, 1.0f}};
    }
    
    float strokeThickness = GetStrokeThickness();
    
    // 使用 RenderContext 绘制线条
    context.DrawLine(start, end, lineColor, strokeThickness);
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
    
    // 找到所有点的包围盒
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
    
    // 扩展半个线宽（线宽会向外扩展）
    float halfThickness = GetStrokeThickness() / 2.0f;
    
    float width = (maxX - minX) + halfThickness * 2.0f;
    float height = (maxY - minY) + halfThickness * 2.0f;
    
    // 返回从 (0, 0) 开始的矩形
    return Rect(0.0f, 0.0f, width, height);
}

void Polygon::OnRender(render::RenderContext& context) {
    if (points_.size() < 3) return;  // 需要至少3个点才能构成多边形
    
    // 计算偏移量（使多边形相对于控件的 (0, 0) 位置）
    float minX = points_[0].x;
    float minY = points_[0].y;
    
    for (size_t i = 1; i < points_.size(); ++i) {
        minX = std::min(minX, points_[i].x);
        minY = std::min(minY, points_[i].y);
    }
    
    // 线宽会向外扩展半个厚度
    float halfThickness = GetStrokeThickness() / 2.0f;
    float offsetX = minX - halfThickness;
    float offsetY = minY - halfThickness;
    
    // 调整所有点到控件坐标系
    std::vector<Point> adjustedPoints;
    adjustedPoints.reserve(points_.size());
    for (const auto& pt : points_) {
        adjustedPoints.emplace_back(pt.x - offsetX, pt.y - offsetY);
    }
    
    // 从 Brush 获取颜色（转换为 RenderContext 格式）
    auto brushToColor = [](Brush* brush) -> std::array<float, 4> {
        if (!brush) return {{0.0f, 0.0f, 0.0f, 0.0f}};  // 透明
        if (auto solidBrush = dynamic_cast<SolidColorBrush*>(brush)) {
            auto color = solidBrush->GetColor();
            return {{color.r, color.g, color.b, color.a}};
        }
        return {{0.0f, 0.0f, 0.0f, 0.0f}};
    };
    
    std::array<float, 4> fillColor = brushToColor(GetFill());
    std::array<float, 4> strokeColor = brushToColor(GetStroke());
    float strokeThickness = GetStrokeThickness();
    
    // 使用 RenderContext 绘制多边形
    context.DrawPolygon(adjustedPoints, fillColor, strokeColor, strokeThickness);
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

Path* Path::SetSegmentStroke(const render::Color& color) {
    if (!segments_.empty()) {
        segments_.back().strokeColor = color;
        segments_.back().hasStrokeColor = true;
        InvalidateVisual();
    }
    return this;
}

Path* Path::SetSegmentStroke(float r, float g, float b, float a) {
    return SetSegmentStroke(render::Color::FromRGB(static_cast<uint8_t>(r * 255),
                                                    static_cast<uint8_t>(g * 255),
                                                    static_cast<uint8_t>(b * 255),
                                                    static_cast<uint8_t>(a * 255)));
}

Path* Path::SetSubPathFill(const render::Color& color) {
    // 找到最近的MoveTo段
    for (auto it = segments_.rbegin(); it != segments_.rend(); ++it) {
        if (it->command == PathCommand::MoveTo) {
            it->fillColor = color;
            it->hasFillColor = true;
            InvalidateVisual();
            break;
        }
    }
    return this;
}

Path* Path::SetSubPathFill(float r, float g, float b, float a) {
    return SetSubPathFill(render::Color::FromRGB(static_cast<uint8_t>(r * 255),
                                                  static_cast<uint8_t>(g * 255),
                                                  static_cast<uint8_t>(b * 255),
                                                  static_cast<uint8_t>(a * 255)));
}

Path* Path::SetSubPathStroke(const render::Color& color, float thickness) {
    // 找到最近的MoveTo段
    for (auto it = segments_.rbegin(); it != segments_.rend(); ++it) {
        if (it->command == PathCommand::MoveTo) {
            it->subPathStrokeColor = color;
            it->subPathStrokeThickness = thickness;
            it->hasSubPathStroke = true;
            InvalidateVisual();
            break;
        }
    }
    return this;
}

Path* Path::SetSubPathStroke(float r, float g, float b, float a, float thickness) {
    return SetSubPathStroke(render::Color::FromRGB(static_cast<uint8_t>(r * 255),
                                                    static_cast<uint8_t>(g * 255),
                                                    static_cast<uint8_t>(b * 255),
                                                    static_cast<uint8_t>(a * 255)),
                            thickness);
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

void Path::OnRender(render::RenderContext& context) {
    if (segments_.empty()) return;
    
    // 从 Brush 获取颜色（转换为 RenderContext 格式）
    auto brushToColor = [](Brush* brush) -> std::array<float, 4> {
        if (!brush) return {{0.0f, 0.0f, 0.0f, 0.0f}};  // 透明
        if (auto solidBrush = dynamic_cast<SolidColorBrush*>(brush)) {
            auto color = solidBrush->GetColor();
            return {{color.r, color.g, color.b, color.a}};
        }
        return {{0.0f, 0.0f, 0.0f, 0.0f}};
    };
    
    std::array<float, 4> fillColor = brushToColor(GetFill());
    std::array<float, 4> strokeColor = brushToColor(GetStroke());
    
    // 如果没有设置 Stroke，默认使用黑色
    if (strokeColor[3] == 0.0f) {
        strokeColor = {{0.0f, 0.0f, 0.0f, 1.0f}};
    }
    
    float strokeThickness = GetStrokeThickness();
    
    // 将 ui::Path::PathSegment 转换为 render::PathSegment
    std::vector<render::PathSegment> renderSegments;
    renderSegments.reserve(segments_.size());
    
    for (const auto& seg : segments_) {
        render::PathSegment renderSeg;
        
        // 转换命令类型
        switch (seg.command) {
            case PathCommand::MoveTo:
                renderSeg.type = render::PathSegmentType::MoveTo;
                break;
            case PathCommand::LineTo:
                renderSeg.type = render::PathSegmentType::LineTo;
                break;
            case PathCommand::QuadraticTo:
                renderSeg.type = render::PathSegmentType::QuadraticBezierTo;
                break;
            case PathCommand::CubicTo:
                renderSeg.type = render::PathSegmentType::CubicBezierTo;
                break;
            case PathCommand::ArcTo:
                renderSeg.type = render::PathSegmentType::ArcTo;
                break;
            case PathCommand::Close:
                renderSeg.type = render::PathSegmentType::Close;
                break;
        }
        
        // 复制点
        renderSeg.points = seg.points;
        
        // 复制分段颜色
        if (seg.hasStrokeColor) {
            auto c = seg.strokeColor;
            renderSeg.strokeColor = {{c.r, c.g, c.b, c.a}};
            renderSeg.hasStrokeColor = true;
        }
        
        // 复制子路径填充颜色
        if (seg.hasFillColor) {
            auto c = seg.fillColor;
            renderSeg.fillColor = {{c.r, c.g, c.b, c.a}};
            renderSeg.hasFillColor = true;
        }
        
        // 复制子路径描边
        if (seg.hasSubPathStroke) {
            auto c = seg.subPathStrokeColor;
            renderSeg.subPathStrokeColor = {{c.r, c.g, c.b, c.a}};
            renderSeg.subPathStrokeThickness = seg.subPathStrokeThickness;
            renderSeg.hasSubPathStroke = true;
        }
        
        renderSegments.push_back(renderSeg);
    }
    
    // 检查是否有子路径独立设置
    bool hasSubPathSettings = false;
    for (const auto& seg : renderSegments) {
        if (seg.hasFillColor || seg.hasSubPathStroke) {
            hasSubPathSettings = true;
            break;
        }
    }
    
    if (hasSubPathSettings) {
        // 按子路径分组渲染
        std::vector<render::PathSegment> subPath;
        std::array<float, 4> subPathFillColor = fillColor;
        std::array<float, 4> subPathStrokeColor = strokeColor;
        float subPathStrokeThickness = strokeThickness;
        
        for (size_t i = 0; i < renderSegments.size(); ++i) {
            const auto& seg = renderSegments[i];
            
            if (seg.type == render::PathSegmentType::MoveTo) {
                // 绘制前一个子路径
                if (!subPath.empty()) {
                    context.DrawPath(subPath, subPathFillColor, subPathStrokeColor, subPathStrokeThickness);
                    subPath.clear();
                }
                // 开始新子路径,设置颜色
                if (seg.hasFillColor) {
                    subPathFillColor = seg.fillColor;
                } else {
                    subPathFillColor = fillColor;
                }
                if (seg.hasSubPathStroke) {
                    subPathStrokeColor = seg.subPathStrokeColor;
                    subPathStrokeThickness = seg.subPathStrokeThickness;
                } else {
                    subPathStrokeColor = strokeColor;
                    subPathStrokeThickness = strokeThickness;
                }
            }
            
            subPath.push_back(seg);
        }
        
        // 绘制最后一个子路径
        if (!subPath.empty()) {
            context.DrawPath(subPath, subPathFillColor, subPathStrokeColor, subPathStrokeThickness);
        }
    } else {
        // 整体渲染
        context.DrawPath(renderSegments, fillColor, strokeColor, strokeThickness);
    }
}

// ========== 模板显式实例化 ==========

// 显式实例化 Shape 模板
template class Shape<Rectangle>;
template class Shape<Ellipse>;
template class Shape<Line>;
template class Shape<Polygon>;
template class Shape<Path>;

} // namespace fk::ui
