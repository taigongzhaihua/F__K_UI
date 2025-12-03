#include "fk/ui/layouts/GridCellAttacher.h"
#include "fk/ui/layouts/Grid.h"

namespace fk::ui {

void GridCellAttacher::ApplyToElement(UIElement* element) const {
    if (element) {
        Grid::SetRow(element, row);
        Grid::SetColumn(element, col);
        Grid::SetRowSpan(element, rowSpan);
        Grid::SetColumnSpan(element, colSpan);
    }
}

} // namespace fk::ui
