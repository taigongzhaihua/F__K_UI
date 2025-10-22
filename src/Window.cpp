#include "Window.h"
#include <iostream>

namespace fk {

Window::Window()
    : title_("Untitled"), width_(800), height_(600), visible_(false) {}

Window::~Window() {}

WindowPtr Window::Title(const std::string& t) { title_ = t; return shared_from_this(); }
std::string Window::Title() const { return title_; }

WindowPtr Window::Width(int w) { width_ = w; Resized(width_, height_); return shared_from_this(); }
int Window::Width() const { return width_; }

WindowPtr Window::Height(int h) { height_ = h; Resized(width_, height_); return shared_from_this(); }
int Window::Height() const { return height_; }

void Window::Show() {
    // 标记为可见并触发 Opened 事件
    if (!visible_) {
        visible_ = true;
        // 触发事件，外部订阅者会被调用
        Opened();
        std::cout << "Window '" << title_ << "' opened." << std::endl;
    }
}

void Window::Hide() {
    // 标记为不可见并触发 Closed 事件
    if (visible_) {
        visible_ = false;
        Closed();
        std::cout << "Window '" << title_ << "' closed." << std::endl;
    }
}

bool Window::IsVisible() const { return visible_; }

} // namespace fk
