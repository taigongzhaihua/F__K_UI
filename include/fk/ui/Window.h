#pragma once

#include <memory>
#include <string>

#include "fk/core/Event.h"

namespace fk {

class Window : public std::enable_shared_from_this<Window> {
public:
    using Ptr = std::shared_ptr<Window>;

    Window();
    virtual ~Window();

    Ptr Title(const std::string& t);
    std::string Title() const;

    Ptr Width(int w);
    int Width() const;

    Ptr Height(int h);
    int Height() const;

    void Show();
    void Hide();
    bool IsVisible() const;

    core::Event<> Closed;
    core::Event<> Opened;
    core::Event<int,int> Resized;
    core::Event<> Activated;
    core::Event<> Deactivated;


private:
    std::string title_;
    int width_;
    int height_;
    bool visible_;
};

using WindowPtr = Window::Ptr;

inline WindowPtr window() {
    return std::make_shared<Window>();
}
} // namespace fk
