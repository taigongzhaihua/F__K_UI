#include "fk/ui/Border.h"
#include "fk/render/RenderContext.h"

namespace fk::ui {

// ========== 依赖属性注册 ==========

const binding::DependencyProperty& Border::ChildProperty() {
    static auto& property = binding::DependencyProperty::Register(
        "Child",
        typeid(UIElement*),
        typeid(Border),
        {static_cast<UIElement*>(nullptr)}
    );
    return property;
}

const binding::DependencyProperty& Border::BorderBrushProperty() {
    static auto& property = binding::DependencyProperty::Register(
        "BorderBrush",
        typeid(Brush*),
        typeid(Border),
        {static_cast<Brush*>(nullptr)}
    );
    return property;
}

const binding::DependencyProperty& Border::BorderThicknessProperty() {
    static auto& property = binding::DependencyProperty::Register(
        "BorderThickness",
        typeid(Thickness),
        typeid(Border),
        {Thickness(0)}
    );
    return property;
}

const binding::DependencyProperty& Border::CornerRadiusProperty() {
    static auto& property = binding::DependencyProperty::Register(
        "CornerRadius",
        typeid(ui::CornerRadius),
        typeid(Border),
        {ui::CornerRadius(0)}
    );
    return property;
}

const binding::DependencyProperty& Border::BackgroundProperty() {
    static auto& property = binding::DependencyProperty::Register(
        "Background",
        typeid(Brush*),
        typeid(Border),
        {static_cast<Brush*>(nullptr)}
    );
    return property;
}

const binding::DependencyProperty& Border::PaddingProperty() {
    static auto& property = binding::DependencyProperty::Register(
        "Padding",
        typeid(Thickness),
        typeid(Border),
        {Thickness(0)}
    );
    return property;
}

Size Border::MeasureOverride(const Size& availableSize) {
    auto borderThickness = GetBorderThickness();
    auto padding = GetPadding();
    auto child = GetChild();
    
    // 计算可用于子元素的空间
    float borderAndPaddingWidth = borderThickness.left + borderThickness.right +
                                   padding.left + padding.right;
    float borderAndPaddingHeight = borderThickness.top + borderThickness.bottom +
                                    padding.top + padding.bottom;
    
    Size childAvailable(
        std::max(0.0f, availableSize.width - borderAndPaddingWidth),
        std::max(0.0f, availableSize.height - borderAndPaddingHeight)
    );
    
    Size childDesired(0, 0);
    if (child && child->GetVisibility() != Visibility::Collapsed) {
        child->Measure(childAvailable);
        childDesired = child->GetDesiredSize();
    }
    
    return Size(
        childDesired.width + borderAndPaddingWidth,
        childDesired.height + borderAndPaddingHeight
    );
}

Size Border::ArrangeOverride(const Size& finalSize) {
    auto borderThickness = GetBorderThickness();
    auto padding = GetPadding();
    auto child = GetChild();
    
    if (child && child->GetVisibility() != Visibility::Collapsed) {
        // 计算子元素的布局矩形
        float x = borderThickness.left + padding.left;
        float y = borderThickness.top + padding.top;
        float width = std::max(0.0f, finalSize.width - borderThickness.left - 
                               borderThickness.right - padding.left - padding.right);
        float height = std::max(0.0f, finalSize.height - borderThickness.top - 
                                borderThickness.bottom - padding.top - padding.bottom);
        
        child->Arrange(Rect(x, y, width, height));
    }
    
    return finalSize;
}

void Border::CollectDrawCommands(render::RenderContext& context) {
    // TODO: 添加背景和边框绘制命令
    // auto background = GetBackground();
    // auto borderBrush = GetBorderBrush();
    // auto borderThickness = GetBorderThickness();
    // if (background) context.DrawRectangle(bounds, background);
    // if (borderBrush) context.DrawBorder(bounds, borderBrush, borderThickness);
    
    // 收集子元素的绘制命令
    auto child = GetChild();
    if (child) {
        child->CollectDrawCommands(context);
    }
}

} // namespace fk::ui
