#include "fk/ui/StackPanel.h"

#include <algorithm>

namespace fk::ui {

// 依赖属性定义
const binding::DependencyProperty& StackPanel::OrientationProperty() {
    static const auto& prop = binding::DependencyProperty::Register(
        "Orientation",
        typeid(ui::Orientation),
        typeid(StackPanel),
        BuildOrientationMetadata()
    );
    return prop;
}

const binding::DependencyProperty& StackPanel::SpacingProperty() {
    static const auto& prop = binding::DependencyProperty::Register(
        "Spacing",
        typeid(float),
        typeid(StackPanel),
        BuildSpacingMetadata()
    );
    return prop;
}

binding::PropertyMetadata StackPanel::BuildOrientationMetadata() {
    return binding::PropertyMetadata(
        ui::Orientation::Vertical,  // 默认值
        &StackPanel::OrientationPropertyChanged
    );
}

binding::PropertyMetadata StackPanel::BuildSpacingMetadata() {
    return binding::PropertyMetadata(
        0.0f,  // 默认间距为 0
        &StackPanel::SpacingPropertyChanged
    );
}

void StackPanel::OrientationPropertyChanged(
    binding::DependencyObject& sender,
    const binding::DependencyProperty& property,
    const std::any& oldValue,
    const std::any& newValue
) {
    auto* panel = dynamic_cast<StackPanel*>(&sender);
    if (!panel) return;
    
    // 触发布局更新
    panel->InvalidateMeasure();
    panel->InvalidateArrange();
}

void StackPanel::SpacingPropertyChanged(
    binding::DependencyObject& sender,
    const binding::DependencyProperty& property,
    const std::any& oldValue,
    const std::any& newValue
) {
    auto* panel = dynamic_cast<StackPanel*>(&sender);
    if (!panel) return;
    
    // 触发布局更新
    panel->InvalidateMeasure();
    panel->InvalidateArrange();
}

StackPanel::StackPanel() = default;

StackPanel::~StackPanel() = default;

// Getter: 从依赖属性读取
ui::Orientation StackPanel::Orientation() const {
    return std::any_cast<ui::Orientation>(GetValue(OrientationProperty()));
}

// Setter: 写入依赖属性
std::shared_ptr<StackPanel> StackPanel::Orientation(ui::Orientation orientation) {
    SetValue(OrientationProperty(), orientation);
    return Self();
}

// Getter: 获取间距
float StackPanel::Spacing() const {
    const auto& value = GetValue(SpacingProperty());
    if (!value.has_value()) return 0.0f;
    return std::any_cast<float>(value);
}

// Setter: 设置间距
std::shared_ptr<StackPanel> StackPanel::Spacing(float spacing) {
    SetValue(SpacingProperty(), spacing);
    return Self();
}

Size StackPanel::MeasureOverride(const Size& availableSize) {
    Size desired{};

    const auto orientation = Orientation();  // 从依赖属性读取
    const auto spacing = Spacing();  // 获取间距
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
    const auto orientation = Orientation();  // 从依赖属性读取
    const auto spacing = Spacing();  // 获取间距
    const auto children = ChildSpan();
    float offset = 0.0f;

    for (const auto& child : children) {
        if (!child) {
            continue;
        }

        if (orientation == ui::Orientation::Horizontal) {
            const auto childWidth = child->DesiredSize().width;
            ArrangeChild(*child, Rect{ offset, 0.0f, childWidth, finalSize.height });
            offset += childWidth + spacing;  // 添加间距
        } else {
            const auto childHeight = child->DesiredSize().height;
            ArrangeChild(*child, Rect{ 0.0f, offset, finalSize.width, childHeight });
            offset += childHeight + spacing;  // 添加间距
        }
    }

    return finalSize;
}

} // namespace fk::ui
