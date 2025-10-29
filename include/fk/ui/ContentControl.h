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

/**
 * @brief 便捷的 ContentControl 工厂函数
 */
inline std::shared_ptr<ContentControl> contentControl() {
    return std::make_shared<ContentControl>();
}

} // namespace fk::ui
