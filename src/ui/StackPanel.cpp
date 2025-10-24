#include "fk/ui/StackPanel.h"

#include <algorithm>

namespace fk::ui {

StackPanel::StackPanel() = default;

StackPanel::~StackPanel() = default;

void StackPanel::SetOrientation(Orientation orientation) {
    if (orientation_ == orientation) {
        return;
    }
    orientation_ = orientation;
    InvalidateMeasure();
    InvalidateArrange();
}

Size StackPanel::MeasureOverride(const Size& availableSize) {
    Size desired{};

    const auto children = ChildSpan();
    for (const auto& child : children) {
        if (!child) {
            continue;
        }

        if (orientation_ == Orientation::Horizontal) {
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
    const auto children = ChildSpan();
    float offset = 0.0f;

    for (const auto& child : children) {
        if (!child) {
            continue;
        }

        if (orientation_ == Orientation::Horizontal) {
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
