#include "fk/ui/StackPanel.h"
#include <algorithm>

namespace fk::ui {

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
    
    Size childAvailable = availableSize;
    
    if (orientation == Orientation::Vertical) {
        childAvailable.height = std::numeric_limits<float>::infinity();
    } else {
        childAvailable.width = std::numeric_limits<float>::infinity();
    }
    
    int visibleChildCount = 0;
    
    // 测量所有子元素
    for (auto* child : children_) {
        if (child && child->GetVisibility() != Visibility::Collapsed) {
            child->Measure(childAvailable);
            Size childDesired = child->GetDesiredSize();
            
            if (orientation == Orientation::Vertical) {
                totalHeight += childDesired.height;
                maxWidth = std::max(maxWidth, childDesired.width);
            } else {
                totalWidth += childDesired.width;
                maxHeight = std::max(maxHeight, childDesired.height);
            }
            
            visibleChildCount++;
        }
    }
    
    // 添加间距（子元素之间的间距，不包括最后一个元素后面）
    if (visibleChildCount > 1) {
        float totalSpacing = spacing * (visibleChildCount - 1);
        if (orientation == Orientation::Vertical) {
            totalHeight += totalSpacing;
        } else {
            totalWidth += totalSpacing;
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
    
    for (auto* child : children_) {
        if (child && child->GetVisibility() != Visibility::Collapsed) {
            Size childDesired = child->GetDesiredSize();
            
            if (orientation == Orientation::Vertical) {
                child->Arrange(Rect(0, offset, finalSize.width, childDesired.height));
                offset += childDesired.height + spacing;
            } else {
                child->Arrange(Rect(offset, 0, childDesired.width, finalSize.height));
                offset += childDesired.width + spacing;
            }
        }
    }
    
    return finalSize;
}

} // namespace fk::ui
