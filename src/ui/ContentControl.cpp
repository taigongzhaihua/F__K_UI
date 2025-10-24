#include "fk/ui/ContentControl.h"

namespace fk::ui {

ContentControl::ContentControl() = default;

ContentControl::~ContentControl() = default;

void ContentControl::OnContentChanged(UIElement* oldContent, UIElement* newContent) {
    detail::ControlBase::OnContentChanged(oldContent, newContent);
    ContentChanged(oldContent, newContent);
}

} // namespace fk::ui
