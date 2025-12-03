#include "fk/ui/layouts/StackPanel.h"
#include "fk/ui/graphics/Brush.h"
#include "fk/render/RenderContext.h"
#include <algorithm>
#include <limits>

namespace fk::ui {

// ========== 模板显式实例化（修复链接错误�?=========
template class FrameworkElement<StackPanel>;
template class Panel<StackPanel>;

// ========== 依赖属性注�?==========

const binding::DependencyProperty& StackPanel::OrientationProperty() {
    static auto& property = binding::DependencyProperty::Register(
        "Orientation",
        typeid(ui::Orientation),
        typeid(StackPanel),
        {ui::Orientation::Vertical}
    );
    return property;
}

const binding::DependencyProperty& StackPanel::SpacingProperty() {
    static auto& property = binding::DependencyProperty::Register(
        "Spacing",
        typeid(float),
        typeid(StackPanel),
        {0.0f}
    );
    return property;
}

Size StackPanel::MeasureOverride(const Size& availableSize) {
    auto orientation = GetOrientation();
    auto spacing = GetSpacing();
    
    float totalWidth = 0;
    float totalHeight = 0;
    float maxWidth = 0;
    float maxHeight = 0;
    float pendingMargin = 0;
    bool hasVisibleChild = false;
    
    // WPF标准行为：StackPanel在堆叠方向总是给子元素infinity
    // 这样子元素可以报告真实的期望尺寸，不受限�?
    // 非堆叠方向传递父元素的约�?
    Size childAvailable = availableSize;
    
    if (orientation == Orientation::Vertical) {
        // 垂直堆叠：高度方向给infinity，宽度传递约�?
        childAvailable.height = std::numeric_limits<float>::infinity();
    } else {
        // 水平堆叠：宽度方向给infinity，高度传递约�?
        childAvailable.width = std::numeric_limits<float>::infinity();
    }
    
    // 测量所有子元素
    for (auto* child : children_) {
        // 跳过 Collapsed 的子元素（不参与布局�?
        if (child && child->GetVisibility() != Visibility::Collapsed) {
            child->Measure(childAvailable);
            Size childDesired = child->GetDesiredSize();
            auto margin = child->GetMargin();

            if (orientation == Orientation::Vertical) {
                // === 垂直堆叠：累加高度，记录最大宽�?===
                
                if (!hasVisibleChild) {
                    // 第一个元素：直接加上顶部 Margin
                    totalHeight += margin.top;
                } else {
                    // 后续元素：Margin 折叠 + Spacing
                    // Spacing 作为"最小间距保�?参与折叠（取三者最大值）
                    // 设计理念：Spacing = N 表示"保证子元素间距至�?N 像素"
                    totalHeight += std::max({pendingMargin, margin.top, spacing});
                }

                totalHeight += childDesired.height;
                pendingMargin = margin.bottom;
                
                // 记录最大宽度（含左�?Margin�?
                maxWidth = std::max(maxWidth, childDesired.width + margin.left + margin.right);
            } else {
                // === 水平堆叠：累加宽度，记录最大高�?===
                
                if (!hasVisibleChild) {
                    // 第一个元素：直接加上左侧 Margin
                    totalWidth += margin.left;
                } else {
                    // 后续元素：Margin 折叠 + Spacing
                    // Spacing 参与折叠（取三者最大值）
                    totalWidth += std::max({pendingMargin, margin.left, spacing});
                }

                totalWidth += childDesired.width;
                pendingMargin = margin.right;
                
                // 记录最大高度（含上�?Margin�?
                maxHeight = std::max(maxHeight, childDesired.height + margin.top + margin.bottom);
            }

            hasVisibleChild = true;
        }
    }
    
    // 加上最后一个元素的尾部 Margin
    if (hasVisibleChild) {
        if (orientation == Orientation::Vertical) {
            totalHeight += pendingMargin;
        } else {
            totalWidth += pendingMargin;
        }
    }
    
    if (orientation == Orientation::Vertical) {
        return Size(maxWidth, totalHeight);
    } else {
        return Size(totalWidth, maxHeight);
    }
}

Size StackPanel::ArrangeOverride(const Size& finalSize) {
    auto orientation = GetOrientation();
    auto spacing = GetSpacing();
    float offset = 0;
    float pendingMargin = 0;
    bool hasArrangedChild = false;
    
    for (auto* child : children_) {
        // 跳过 Collapsed 的子元素
        if (child && child->GetVisibility() != Visibility::Collapsed) {
            Size childDesired = child->GetDesiredSize();
            auto margin = child->GetMargin();
            
            // 缓存 Alignment 查询结果（性能优化�?
            // UIElement 提供虚方法，FrameworkElement 重写以返回实际�?
            auto hAlign = child->GetHorizontalAlignment();
            auto vAlign = child->GetVerticalAlignment();
            
            if (orientation == Orientation::Vertical) {
                // ========== 垂直堆叠布局 ==========
                
                // 1. 计算垂直偏移（考虑 Margin 折叠�?Spacing�?
                if (!hasArrangedChild) {
                    offset += margin.top;
                } else {
                    // Spacing 参与折叠（取三者最大值）
                    offset += std::max({pendingMargin, margin.top, spacing});
                }

                // 2. 根据 HorizontalAlignment 决定宽度和水平位�?
                // WPF 规则：垂�?StackPanel 中，子元素的 HorizontalAlignment 有效
                //          VerticalAlignment 无效（高度由内容决定�?
                float availableWidth = std::max(0.0f, finalSize.width - margin.left - margin.right);
                float childWidth;
                float childX;
                
                switch (hAlign) {
                    case HorizontalAlignment::Stretch:
                        // 拉伸：填充整个可用宽�?
                        childWidth = availableWidth;
                        childX = margin.left;
                        break;
                        
                    case HorizontalAlignment::Left:
                        // 左对齐：使用期望宽度（不超过可用宽度�?
                        childWidth = std::min(childDesired.width, availableWidth);
                        childX = margin.left;
                        break;
                        
                    case HorizontalAlignment::Center:
                        // 居中：使用期望宽度，居中对齐
                        childWidth = std::min(childDesired.width, availableWidth);
                        childX = margin.left + (availableWidth - childWidth) / 2.0f;
                        break;
                        
                    case HorizontalAlignment::Right:
                        // 右对齐：使用期望宽度，靠右对�?
                        childWidth = std::min(childDesired.width, availableWidth);
                        childX = margin.left + availableWidth - childWidth;
                        break;
                        
                    default:
                        // 默认：Stretch
                        childWidth = availableWidth;
                        childX = margin.left;
                        break;
                }
                
                // WPF标准行为：在堆叠方向（垂直），给子元素其期望高度
                // 不限制子元素高度，允许超出StackPanel边界
                // 如果需要裁剪，由父元素或裁剪系统处�?
                float childHeight = childDesired.height;
                float childY = offset;

                child->Arrange(Rect(childX, childY, childWidth, childHeight));
                offset += childHeight;
                pendingMargin = margin.bottom;
                
            } else {
                // ========== 水平堆叠布局 ==========
                
                // 1. 计算水平偏移（考虑 Margin 折叠�?Spacing�?
                if (!hasArrangedChild) {
                    offset += margin.left;
                } else {
                    // Spacing 参与折叠（取三者最大值）
                    offset += std::max({pendingMargin, margin.left, spacing});
                }

                // 2. 根据 VerticalAlignment 决定高度和垂直位�?
                // WPF 规则：水�?StackPanel 中，子元素的 VerticalAlignment 有效
                //          HorizontalAlignment 无效（宽度由内容决定�?
                float availableHeight = std::max(0.0f, finalSize.height - margin.top - margin.bottom);
                float childHeight;
                float childY;
                
                switch (vAlign) {
                    case VerticalAlignment::Stretch:
                        // 拉伸：填充整个可用高�?
                        childHeight = availableHeight;
                        childY = margin.top;
                        break;
                        
                    case VerticalAlignment::Top:
                        // 顶部对齐：使用期望高度（不超过可用高度）
                        childHeight = std::min(childDesired.height, availableHeight);
                        childY = margin.top;
                        break;
                        
                    case VerticalAlignment::Center:
                        // 居中：使用期望高度，垂直居中
                        childHeight = std::min(childDesired.height, availableHeight);
                        childY = margin.top + (availableHeight - childHeight) / 2.0f;
                        break;
                        
                    case VerticalAlignment::Bottom:
                        // 底部对齐：使用期望高度，靠底对齐
                        childHeight = std::min(childDesired.height, availableHeight);
                        childY = margin.top + availableHeight - childHeight;
                        break;
                        
                    default:
                        // 默认：Stretch
                        childHeight = availableHeight;
                        childY = margin.top;
                        break;
                }
                
                // WPF标准行为：在堆叠方向（水平），给子元素其期望宽度
                // 不限制子元素宽度，允许超出StackPanel边界
                // 如果需要裁剪，由父元素或裁剪系统处�?
                float childWidth = childDesired.width;
                float childX = offset;

                child->Arrange(Rect(childX, childY, childWidth, childHeight));
                offset += childWidth;
                pendingMargin = margin.right;
            }

            hasArrangedChild = true;
        }
    }
    
    return finalSize;
}

void StackPanel::OnRender(render::RenderContext& context) {
    // 辅助函数：将 Brush 转换�?RenderContext 颜色格式
    auto brushToColor = [](Brush* brush) -> std::array<float, 4> {
        if (!brush) return {{0.0f, 0.0f, 0.0f, 0.0f}};  // 透明
        if (auto solidBrush = dynamic_cast<SolidColorBrush*>(brush)) {
            auto color = solidBrush->GetColor();
            return {{color.r, color.g, color.b, color.a}};
        }
        return {{0.0f, 0.0f, 0.0f, 0.0f}};
    };
    
    // 获取背景画刷
    auto background = GetBackground();
    if (!background) {
        return; // 没有背景，不需要绘�?
    }
    
    // 背景应该覆盖整个布局区域（包括Padding�?
    auto layoutRect = GetLayoutRect();
    Rect rect(0, 0, layoutRect.width, layoutRect.height);
    
    // 获取圆角
    auto cornerRadius = GetCornerRadius();
    
    // 转换为颜�?
    std::array<float, 4> fillColor = brushToColor(background);
    std::array<float, 4> strokeColor = {{0.0f, 0.0f, 0.0f, 0.0f}}; // 无边�?
    
    // 绘制背景矩形
    context.DrawBorder(rect, fillColor, strokeColor, 0.0f,
                      cornerRadius.topLeft, cornerRadius.topRight,
                      cornerRadius.bottomRight, cornerRadius.bottomLeft);
}

} // namespace fk::ui
