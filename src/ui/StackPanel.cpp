#include "fk/ui/StackPanel.h"

#include <algorithm>

namespace fk::ui {

// ============================================================================
// 依赖属性定义（使用宏）
// ============================================================================

FK_DEPENDENCY_PROPERTY_DEFINE(StackPanel, Orientation, ui::Orientation, ui::Orientation::Vertical)
FK_DEPENDENCY_PROPERTY_DEFINE(StackPanel, Spacing, float, 0.0f)

// ============================================================================
// 构造/析构
// ============================================================================

StackPanel::StackPanel() = default;

StackPanel::~StackPanel() = default;

// ============================================================================
// 属性变更回调
// ============================================================================

void StackPanel::OnOrientationChanged(ui::Orientation, ui::Orientation) {
    InvalidateMeasure();
    InvalidateArrange();
}

void StackPanel::OnSpacingChanged(float, float) {
    InvalidateMeasure();
    InvalidateArrange();
}

binding::PropertyMetadata StackPanel::BuildOrientationMetadata() {
    binding::PropertyMetadata metadata;
    metadata.defaultValue = ui::Orientation::Vertical;
    metadata.propertyChangedCallback = &StackPanel::OrientationPropertyChanged;
    return metadata;
}

binding::PropertyMetadata StackPanel::BuildSpacingMetadata() {
    binding::PropertyMetadata metadata;
    metadata.defaultValue = 0.0f;
    metadata.propertyChangedCallback = &StackPanel::SpacingPropertyChanged;
    return metadata;
}

// ============================================================================
// 布局
// ============================================================================

Size StackPanel::MeasureOverride(const Size& availableSize) {
    Size desired{};

    const auto orientation = GetOrientation();  // 从依赖属性读取
    const auto spacing = GetSpacing();  // 获取间距
    const auto children = ChildSpan();
    
    size_t validChildCount = 0;
    for (const auto& child : children) {
        if (!child) {
            continue;
        }

        if (orientation == ui::Orientation::Horizontal) {
            MeasureChild(*child, Size{ std::numeric_limits<float>::max(), availableSize.height });
            desired.width += child->DesiredSize().width;
            desired.height = std::max(desired.height, child->DesiredSize().height);
        } else {
            MeasureChild(*child, Size{ availableSize.width, std::numeric_limits<float>::max() });
            desired.height += child->DesiredSize().height;
            desired.width = std::max(desired.width, child->DesiredSize().width);
        }
        validChildCount++;
    }

    // 添加子元素之间的间距
    if (validChildCount > 1) {
        float totalSpacing = spacing * (validChildCount - 1);
        if (orientation == ui::Orientation::Horizontal) {
            desired.width += totalSpacing;
        } else {
            desired.height += totalSpacing;
        }
    }

    desired.width = std::min(desired.width, availableSize.width);
    desired.height = std::min(desired.height, availableSize.height);

    return desired;
}

Size StackPanel::ArrangeOverride(const Size& finalSize) {
    const auto orientation = GetOrientation();  // 从依赖属性读取
    const auto spacing = GetSpacing();  // 获取间距
    const auto children = ChildSpan();
    float offset = 0.0f;

    for (const auto& child : children) {
        if (!child) {
            continue;
        }

        if (orientation == ui::Orientation::Horizontal) {
            const auto childWidth = child->DesiredSize().width;
            const auto childHeight = child->DesiredSize().height;
            ArrangeChild(*child, Rect{ offset, 0.0f, childWidth, childHeight });
            offset += childWidth + spacing;  // 添加间距
        } else {
            const auto childWidth = child->DesiredSize().width;
            const auto childHeight = child->DesiredSize().height;
            ArrangeChild(*child, Rect{ 0.0f, offset, childWidth, childHeight });
            offset += childHeight + spacing;  // 添加间距
        }
    }

    return finalSize;
}

} // namespace fk::ui
