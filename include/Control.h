#pragma once

#include <memory>
#include <string>
#include <utility>

#include "View.h"

namespace fk {

template <typename Derived>
class Control : public View<Derived> {
public:
    using Ptr = std::shared_ptr<Derived>;

    Ptr IsEnabled(bool enabled)
    {
        enabled_ = enabled;
        return this->Self();
    }

    bool IsEnabled() const noexcept { return enabled_; }

    Ptr IsFocused(bool focused)
    {
        focused_ = focused;
        return this->Self();
    }

    bool IsFocused() const noexcept { return focused_; }

    Ptr TabIndex(int index)
    {
        tabIndex_ = index;
        return this->Self();
    }

    int TabIndex() const noexcept { return tabIndex_; }

    Ptr Cursor(std::string cursor)
    {
        cursor_ = std::move(cursor);
        return this->Self();
    }

    const std::string& Cursor() const noexcept { return cursor_; }

    Ptr Content(const ViewElementPtr& content)
    {
        content_ = content;
        return this->Self();
    }

    ViewElementPtr Content() const { return content_; }

    Ptr Focus()
    {
        focused_ = true;
        return this->Self();
    }

    Ptr Blur()
    {
        focused_ = false;
        return this->Self();
    }

    virtual void OnClick() {}
    virtual void OnKeyDown(int /*keyCode*/) {}
    virtual void OnKeyUp(int /*keyCode*/) {}
    virtual void OnMouseDown(int /*button*/) {}
    virtual void OnMouseUp(int /*button*/) {}
    virtual void OnMouseMove(int /*x*/, int /*y*/) {}
    virtual void OnMouseWheel(int /*delta*/) {}

protected:
    Control() = default;

private:
    bool enabled_{true};
    bool focused_{false};
    int tabIndex_{0};
    std::string cursor_{"arrow"};
    ViewElementPtr content_{};
};

} // namespace fk
