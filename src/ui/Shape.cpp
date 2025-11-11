/**
 * @file Shape.cpp
 * @brief Shape 图形实现
 */

#include "fk/ui/Shape.h"

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
    // TODO: 实现矩形渲染
    // 使用 Fill, Stroke, StrokeThickness, RadiusX, RadiusY
}

// ========== Ellipse 几何与渲染 ==========

Rect Ellipse::GetDefiningGeometry() const {
    // 椭圆边界由布局决定
    Size size = GetRenderSize();
    return Rect(0, 0, size.width, size.height);
}

void Ellipse::OnRender(RenderContext& context) {
    // TODO: 实现椭圆渲染
    // 使用 Fill, Stroke, StrokeThickness
}

// ========== 模板显式实例化 ==========
// 只需实例化 Shape，Rectangle 和 Ellipse 继承自 Shape
template class FrameworkElement<Shape>;

} // namespace fk::ui
