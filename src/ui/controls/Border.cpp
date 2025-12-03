#include "fk/ui/controls/Border.h"
#include "fk/ui/graphics/Brush.h"
#include "fk/render/RenderContext.h"
#include <iostream>

namespace fk::ui {

// ========== 依赖属性注�?==========

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
    
    // 如果是同一个，不做任何�?
    if (oldChild == child) {
        return;
    }
    
    // 从视觉树中移除旧�?Child
    if (oldChild) {
        RemoveVisualChild(oldChild);
    }
    
    // 设置新的 Child 属性�?
    SetValue(ChildProperty(), child);
    
    // 将新�?Child 添加到视觉树
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
    
    // 计算可用于子元素的空�?
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
        // 1. 计算内容区域（Border 内部可用空间�?
        float contentX = borderThickness.left + padding.left;
        float contentY = borderThickness.top + padding.top;
        float contentWidth = std::max(0.0f, finalSize.width - borderThickness.left - 
                               borderThickness.right - padding.left - padding.right);
        float contentHeight = std::max(0.0f, finalSize.height - borderThickness.top - 
                                borderThickness.bottom - padding.top - padding.bottom);
        
        // 2. 获取子元素期望尺�?
        Size childDesired = child->GetDesiredSize();
        
        // 3. 根据对齐方式计算子元素实际位置和尺寸
        auto hAlign = child->GetHorizontalAlignment();
        auto vAlign = child->GetVerticalAlignment();
        
        float childX = contentX;
        float childY = contentY;
        float childW = contentWidth;
        float childH = contentHeight;
        
        // 水平对齐处理
        if (hAlign != HorizontalAlignment::Stretch) {
            // 非拉伸时，使用期望宽度（但不超过内容宽度�?
            childW = std::min(childDesired.width, contentWidth);
            
            if (hAlign == HorizontalAlignment::Center) {
                childX += (contentWidth - childW) / 2.0f;
            } else if (hAlign == HorizontalAlignment::Right) {
                childX += (contentWidth - childW);
            }
            // Left 默认�?0 偏移
        }
        
        // 垂直对齐处理
        if (vAlign != VerticalAlignment::Stretch) {
            // 非拉伸时，使用期望高度（但不超过内容高度�?
            childH = std::min(childDesired.height, contentHeight);
            
            if (vAlign == VerticalAlignment::Center) {
                childY += (contentHeight - childH) / 2.0f;
            } else if (vAlign == VerticalAlignment::Bottom) {
                childY += (contentHeight - childH);
            }
            // Top 默认�?0 偏移
        }
        
        // 4. 安排子元�?
        child->Arrange(Rect(childX, childY, childW, childH));
    }
    
    return finalSize;
}

void Border::ArrangeCore(const Rect& finalRect) {
    // Border �?Padding 是用于子元素布局的，不应在此处减�?
    // 但需要支持显�?Width/Height
    
    // 1. 获取期望尺寸
    auto desiredSize = GetDesiredSize();
    
    // 2. 检查是否有显式尺寸
    float explicitWidth = GetWidth();
    float explicitHeight = GetHeight();
    bool hasExplicitWidth = (explicitWidth > 0 && !std::isnan(explicitWidth));
    bool hasExplicitHeight = (explicitHeight > 0 && !std::isnan(explicitHeight));
    
    // 3. 应用对齐方式决定最终尺�?
    auto hAlign = GetHorizontalAlignment();
    auto vAlign = GetVerticalAlignment();
    
    float finalWidth = finalRect.width;
    float finalHeight = finalRect.height;
    
    // 水平对齐
    if (hAlign != HorizontalAlignment::Stretch) {
        // 如果有显式宽度，使用显式值（即使超出finalRect.width�?
        // 否则使用desiredWidth但不超过finalRect.width
        if (hasExplicitWidth) {
            finalWidth = explicitWidth;
        } else {
            finalWidth = std::min(desiredSize.width, finalRect.width);
        }
    } else if (hasExplicitWidth) {
        // 即使是Stretch，如果有显式宽度也使用显式�?
        finalWidth = explicitWidth;
    }
    
    // 垂直对齐
    if (vAlign != VerticalAlignment::Stretch) {
        // 如果有显式高度，使用显式值（即使超出finalRect.height�?
        // 否则使用desiredHeight但不超过finalRect.height
        if (hasExplicitHeight) {
            finalHeight = explicitHeight;
        } else {
            finalHeight = std::min(desiredSize.height, finalRect.height);
        }
    } else if (hasExplicitHeight) {
        // 即使是Stretch，如果有显式高度也使用显式�?
        finalHeight = explicitHeight;
    }
    
    // 4. 调用 ArrangeOverride 并设置渲染尺�?
    Size renderSize = ArrangeOverride(Size(finalWidth, finalHeight));
    SetRenderSize(renderSize);
}

void Border::OnRender(render::RenderContext& context) {
    // 辅助函数：将 Brush 转换�?RenderContext 颜色格式
    auto brushToColor = [](Brush* brush) -> std::array<float, 4> {
        if (!brush) return {{0.0f, 0.0f, 0.0f, 0.0f}};  // 透明
        if (auto solidBrush = dynamic_cast<SolidColorBrush*>(brush)) {
            auto color = solidBrush->GetColor();
            return {{color.r, color.g, color.b, color.a}};
        }
        return {{0.0f, 0.0f, 0.0f, 0.0f}};
    };
    
    // 背景和边框应该覆盖整个布局区域（包括Padding�?
    auto layoutRect = GetLayoutRect();
    Rect rect(0, 0, layoutRect.width, layoutRect.height);
    
    // 获取背景和边框属�?
    auto background = GetBackground();
    auto borderBrush = GetBorderBrush();
    auto borderThickness = GetBorderThickness();
    auto cornerRadius = GetCornerRadius();
    
    // 转换为颜色数�?
    std::array<float, 4> fillColor = brushToColor(background);
    std::array<float, 4> strokeColor = brushToColor(borderBrush);
    
    // 计算边框宽度（使用平均值或最大值）
    // 注意：RenderContext::DrawBorder 只接受单一�?strokeWidth
    // 这里使用平均值作为近�?
    float strokeWidth = (borderThickness.left + borderThickness.right + 
                        borderThickness.top + borderThickness.bottom) / 4.0f;
    
    // 如果边框宽度�?或非常小，将边框颜色设置为透明，避免显示极细边�?
    if (strokeWidth <= 0.0f) {
        strokeColor = {{0.0f, 0.0f, 0.0f, 0.0f}};
    }
    
    // 使用 CornerRadius 的四个独立圆角�?
    // 绘制边框（带背景、边框和圆角�?
    context.DrawBorder(rect, fillColor, strokeColor, strokeWidth, 
                      cornerRadius.topLeft, cornerRadius.topRight,
                      cornerRadius.bottomRight, cornerRadius.bottomLeft,
                      render::StrokeAlignment::Inside);
}

void Border::OnPropertyChanged(const binding::DependencyProperty& property,
                               const std::any& oldValue,
                               const std::any& newValue,
                               binding::ValueSource oldSource,
                               binding::ValueSource newSource) {
    FrameworkElement<Border>::OnPropertyChanged(property, oldValue, newValue, oldSource, newSource);
    
    // �?Background �?BorderBrush 改变时，监听新画刷的属性变�?
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
    // 断开旧连�?
    if (connection.IsConnected()) {
        connection.Disconnect();
    }
    
    // 监听新画刷的属性变�?
    if (brush) {
        connection = brush->PropertyChanged.Connect([this](const binding::DependencyProperty&, const std::any&, const std::any&, binding::ValueSource, binding::ValueSource) {
            // 画刷属性变化时，触发重�?
            this->InvalidateVisual();
        });
    }
}

ui::Rect Border::CalculateClipBounds() const {
    auto borderThickness = GetBorderThickness();
    auto padding = GetPadding();
    auto size = GetRenderSize();
    
    // 计算内容区域（排除BorderThickness和Padding�?
    float left = borderThickness.left + padding.left;
    float top = borderThickness.top + padding.top;
    float right = borderThickness.right + padding.right;
    float bottom = borderThickness.bottom + padding.bottom;
    
    return ui::Rect{
        left,
        top,
        std::max(0.0f, size.width - left - right),
        std::max(0.0f, size.height - top - bottom)
    };
}

} // namespace fk::ui
