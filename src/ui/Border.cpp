#include "fk/ui/Border.h"
#include "fk/ui/Brush.h"
#include "fk/render/RenderContext.h"
#include <iostream>

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

void Border::SetChild(UIElement* child) {
    // 获取旧的 Child
    auto* oldChild = GetChild();
    
    // 如果是同一个，不做任何事
    if (oldChild == child) {
        return;
    }
    
    // 从视觉树中移除旧的 Child
    if (oldChild) {
        RemoveVisualChild(oldChild);
    }
    
    // 设置新的 Child 属性值
    SetValue(ChildProperty(), child);
    
    // 将新的 Child 添加到视觉树
    if (child) {
        AddVisualChild(child);
        TakeOwnership(child);
    }
    
    // 标记需要重新布局
    InvalidateMeasure();
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

void Border::OnRender(render::RenderContext& context) {
    // 辅助函数：将 Brush 转换为 RenderContext 颜色格式
    auto brushToColor = [](Brush* brush) -> std::array<float, 4> {
        if (!brush) return {{0.0f, 0.0f, 0.0f, 0.0f}};  // 透明
        if (auto solidBrush = dynamic_cast<SolidColorBrush*>(brush)) {
            auto color = solidBrush->GetColor();
            return {{color.r, color.g, color.b, color.a}};
        }
        return {{0.0f, 0.0f, 0.0f, 0.0f}};
    };
    
    // 获取渲染大小
    auto renderSize = GetRenderSize();
    Rect rect(0, 0, renderSize.width, renderSize.height);
    
    // 获取背景和边框属性
    auto background = GetBackground();
    auto borderBrush = GetBorderBrush();
    auto borderThickness = GetBorderThickness();
    auto cornerRadius = GetCornerRadius();
    
    // 转换为颜色数组
    std::array<float, 4> fillColor = brushToColor(background);
    std::array<float, 4> strokeColor = brushToColor(borderBrush);
    
    // 计算边框宽度（使用平均值或最大值）
    // 注意：RenderContext::DrawRectangle 只接受单一的 strokeWidth
    // 这里使用平均值作为近似
    float strokeWidth = (borderThickness.left + borderThickness.right + 
                        borderThickness.top + borderThickness.bottom) / 4.0f;
    
    // 使用 CornerRadius 的平均值作为圆角半径
    float radius = (cornerRadius.topLeft + cornerRadius.topRight + 
                   cornerRadius.bottomRight + cornerRadius.bottomLeft) / 4.0f;
    
    // 绘制矩形（带背景、边框和圆角）
    context.DrawRectangle(rect, fillColor, strokeColor, strokeWidth, radius, render::StrokeAlignment::Inside);
}

void Border::OnPropertyChanged(const binding::DependencyProperty& property,
                               const std::any& oldValue,
                               const std::any& newValue,
                               binding::ValueSource oldSource,
                               binding::ValueSource newSource) {
    FrameworkElement<Border>::OnPropertyChanged(property, oldValue, newValue, oldSource, newSource);
    
    // 当 Background 或 BorderBrush 改变时，监听新画刷的属性变化
    if (&property == &BackgroundProperty()) {
        Brush* newBrush = nullptr;
        if (newValue.has_value() && newValue.type() == typeid(Brush*)) {
            newBrush = std::any_cast<Brush*>(newValue);
        }
        ObserveBrush(newBrush, backgroundConnection_);
    } else if (&property == &BorderBrushProperty()) {
        Brush* newBrush = nullptr;
        if (newValue.has_value() && newValue.type() == typeid(Brush*)) {
            newBrush = std::any_cast<Brush*>(newValue);
        }
        ObserveBrush(newBrush, borderBrushConnection_);
    }
}

void Border::ObserveBrush(Brush* brush, core::Event<const binding::DependencyProperty&, const std::any&, const std::any&, binding::ValueSource, binding::ValueSource>::Connection& connection) {
    // 断开旧连接
    if (connection.IsConnected()) {
        connection.Disconnect();
    }
    
    // 监听新画刷的属性变化
    if (brush) {
        connection = brush->PropertyChanged.Connect([this](const binding::DependencyProperty&, const std::any&, const std::any&, binding::ValueSource, binding::ValueSource) {
            // 画刷属性变化时，触发重绘
            this->InvalidateVisual();
        });
    }
}

} // namespace fk::ui
