#pragma once

#include "fk/ui/Panel.h"

namespace fk::ui {

enum class Orientation {
    Horizontal,
    Vertical
};

class StackPanel : public Panel {
public:
    using Panel::Panel;

    StackPanel();
    ~StackPanel() override;

    void SetOrientation(Orientation orientation);
    [[nodiscard]] Orientation GetOrientation() const noexcept { return orientation_; }

protected:
    Size MeasureOverride(const Size& availableSize) override;
    Size ArrangeOverride(const Size& finalSize) override;

private:
    Orientation orientation_{Orientation::Vertical};
};

} // namespace fk::ui
