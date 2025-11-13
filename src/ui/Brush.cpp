/**
 * @file Brush.cpp
 * @brief Brush 画刷系统实现
 */

#include "fk/ui/Brush.h"
#include "fk/ui/RenderBackend.h"
#include <any>

namespace fk::ui {

// ========== SolidColorBrush 依赖属性 ==========

const binding::DependencyProperty& SolidColorBrush::ColorProperty() {
    static auto& property = 
        binding::DependencyProperty::Register(
            "Color",
            typeid(fk::ui::Color),
            typeid(SolidColorBrush),
            binding::PropertyMetadata{std::any(fk::ui::Color::Black())}
        );
    return property;
}

// ========== SolidColorBrush 实现 ==========

Color SolidColorBrush::GetColor() const {
    return color_;
}

void SolidColorBrush::SetColor(Color value) {
    color_ = value;
    SetValue(ColorProperty(), std::any(value));
}

void SolidColorBrush::Apply(RenderContext* context) {
    if (!context) return;
    
    // 将颜色应用到渲染上下文
    // 这里调用渲染后端设置颜色
    // context->SetColor(color_);
    
    // 注意：实际的渲染设置由 Shape::OnRender() 中处理
    // 这个方法主要用于未来扩展（如渐变画刷）
}

Brush* SolidColorBrush::Clone() const {
    return new SolidColorBrush(color_);
}

} // namespace fk::ui
