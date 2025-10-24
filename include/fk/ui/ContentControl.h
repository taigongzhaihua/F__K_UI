#pragma once

#include "fk/ui/Control.h"
#include "fk/core/Event.h"

namespace fk::ui {

class ContentControl : public Control<ContentControl> {
public:
    using Base = Control<ContentControl>;

    ContentControl();
    ~ContentControl() override;

    core::Event<UIElement*, UIElement*> ContentChanged;

protected:
    void OnContentChanged(UIElement* oldContent, UIElement* newContent) override;
};

} // namespace fk::ui
