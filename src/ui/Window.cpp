#include "fk/ui/Window.h"

#include <iostream>

namespace fk {

Window::Window()
    : title_("Untitled"), width_(800), height_(600), visible_(false) {}

Window::~Window() = default;

Window::Ptr Window::Title(const std::string& t) {
    title_ = t;
    return shared_from_this();
}

std::string Window::Title() const { return title_; }

Window::Ptr Window::Width(int w) {
    width_ = w;
    Resized(width_, height_);
    return shared_from_this();
}

int Window::Width() const { return width_; }

Window::Ptr Window::Height(int h) {
    height_ = h;
    Resized(width_, height_);
    return shared_from_this();
}

int Window::Height() const { return height_; }

void Window::Show() {
    if (!visible_) {
        visible_ = true;
        Opened();
        std::cout << "Window '" << title_ << "' opened." << std::endl;
    }
}

void Window::Hide() {
    if (visible_) {
        visible_ = false;
        Closed();
        std::cout << "Window '" << title_ << "' closed." << std::endl;
    }
}

bool Window::IsVisible() const { return visible_; }

} // namespace fk
