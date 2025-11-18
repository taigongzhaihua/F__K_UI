#include "fk/ui/StackPanel.h"
#include <algorithm>
#include <limits>

namespace fk::ui {

// ========== 模板显式实例化（修复链接错误）==========
template class FrameworkElement<StackPanel>;
template class Panel<StackPanel>;

// ========== 依赖属性注册 ==========

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
    
    Size childAvailable = availableSize;
    
    if (orientation == Orientation::Vertical) {
        childAvailable.height = std::numeric_limits<float>::infinity();
    } else {
        childAvailable.width = std::numeric_limits<float>::infinity();
    }
    
    // 测量所有子元素
    for (auto* child : children_) {
        if (child && child->GetVisibility() != Visibility::Collapsed) {
            child->Measure(childAvailable);
            Size childDesired = child->GetDesiredSize();
            auto margin = child->GetMargin();

            if (orientation == Orientation::Vertical) {
                if (!hasVisibleChild) {
                    totalHeight += margin.top;
                } else {
                    totalHeight += std::max(pendingMargin, margin.top) + spacing;
                }

                totalHeight += childDesired.height;
                pendingMargin = margin.bottom;
                maxWidth = std::max(maxWidth, childDesired.width + margin.left + margin.right);
            } else {
                if (!hasVisibleChild) {
                    totalWidth += margin.left;
                } else {
                    totalWidth += std::max(pendingMargin, margin.left) + spacing;
                }

                totalWidth += childDesired.width;
                pendingMargin = margin.right;
                maxHeight = std::max(maxHeight, childDesired.height + margin.top + margin.bottom);
            }

            hasVisibleChild = true;
        }
    }
    
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
        if (child && child->GetVisibility() != Visibility::Collapsed) {
            Size childDesired = child->GetDesiredSize();
            auto margin = child->GetMargin();
            
            if (orientation == Orientation::Vertical) {
                if (!hasArrangedChild) {
                    offset += margin.top;
                } else {
                    offset += std::max(pendingMargin, margin.top) + spacing;
                }

                // 根据子元素的 HorizontalAlignment 决定宽度
                // 垂直 StackPanel：子元素的宽度由其 HorizontalAlignment 决定
                // - Stretch: 拉伸到整个宽度
                // - 其他: 使用子元素的期望宽度
                float availableWidth = std::max(0.0f, finalSize.width - margin.left - margin.right);
                float childWidth = childDesired.width;  // desiredSize 已经不含 Margin
                childWidth = std::min(childWidth, availableWidth);  // 但不超过可用宽度
                
                float childHeight = std::max(0.0f, childDesired.height);
                float childX = margin.left;
                float childY = offset;

                child->Arrange(Rect(childX, childY, childWidth, childHeight));
                offset += childHeight;
                pendingMargin = margin.bottom;
            } else {
                if (!hasArrangedChild) {
                    offset += margin.left;
                } else {
                    offset += std::max(pendingMargin, margin.left) + spacing;
                }

                float childWidth = std::max(0.0f, childDesired.width);
                
                // 根据子元素的 VerticalAlignment 决定高度
                // 水平 StackPanel：子元素的高度由其 VerticalAlignment 决定
                // - Stretch: 拉伸到整个高度
                // - 其他: 使用子元素的期望高度
                float availableHeight = std::max(0.0f, finalSize.height - margin.top - margin.bottom);
                float childHeight = childDesired.height;  // desiredSize 已经不含 Margin
                childHeight = std::min(childHeight, availableHeight);  // 但不超过可用高度
                
                float childX = offset;
                float childY = margin.top;

                child->Arrange(Rect(childX, childY, childWidth, childHeight));
                offset += childWidth;
                pendingMargin = margin.right;
            }

            hasArrangedChild = true;
        }
    }
    
    return finalSize;
}

} // namespace fk::ui
