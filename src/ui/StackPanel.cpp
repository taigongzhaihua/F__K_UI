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

binding::PropertyMetadata StackPanel::BuildOrientationMetadata() {
    return binding::PropertyMetadata(
        ui::Orientation::Vertical,  // 默认值
        &StackPanel::OrientationPropertyChanged
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

Size StackPanel::MeasureOverride(const Size& availableSize) {
    Size desired{};

    const auto orientation = Orientation();  // 从依赖属性读取
    const auto children = ChildSpan();
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
    }

    desired.width = std::min(desired.width, availableSize.width);
    desired.height = std::min(desired.height, availableSize.height);

    return desired;
}

Size StackPanel::ArrangeOverride(const Size& finalSize) {
    const auto orientation = Orientation();  // 从依赖属性读取
    const auto children = ChildSpan();
    float offset = 0.0f;

    for (const auto& child : children) {
        if (!child) {
            continue;
        }

        if (orientation == ui::Orientation::Horizontal) {
            const auto childWidth = child->DesiredSize().width;
            ArrangeChild(*child, Rect{ offset, 0.0f, childWidth, finalSize.height });
            offset += childWidth;
        } else {
            const auto childHeight = child->DesiredSize().height;
            ArrangeChild(*child, Rect{ 0.0f, offset, finalSize.width, childHeight });
            offset += childHeight;
        }
    }

    return finalSize;
}

} // namespace fk::ui
