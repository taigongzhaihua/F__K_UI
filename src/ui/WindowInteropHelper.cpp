#include "fk/ui/WindowInteropHelper.h"
#include "fk/ui/Window.h"
#include <GLFW/glfw3.h>
#include <iostream>

namespace fk::ui {

static bool glfwInitialized = false;

WindowInteropHelper::WindowInteropHelper(Window* owner)
    : owner_(owner) {
}

WindowInteropHelper::~WindowInteropHelper() {
    DestroyHandle();
}

void WindowInteropHelper::EnsureHandle() {
    if (handle_) {
        return;
    }
    CreateNativeWindow();
}

void WindowInteropHelper::DestroyHandle() {
    if (handle_) {
        glfwDestroyWindow(handle_);
        handle_ = nullptr;
    }
}

void WindowInteropHelper::CreateNativeWindow() {
    // 初始化 GLFW（只需初始化一次）
    if (!glfwInitialized) {
        if (!glfwInit()) {
            std::cerr << "Failed to initialize GLFW" << std::endl;
            return;
        }
        glfwInitialized = true;
        std::cout << "GLFW initialized" << std::endl;
    }

    // 设置 OpenGL 版本
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);  // 创建时隐藏，等 Show() 时再显示
    
    // 启用 4x 多重采样抗锯齿
    glfwWindowHint(GLFW_SAMPLES, 4);

    // 创建窗口
    handle_ = glfwCreateWindow(
        owner_->Width(),
        owner_->Height(),
        owner_->Title().c_str(),
        nullptr,
        nullptr
    );

    if (!handle_) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        return;
    }

    std::cout << "GLFW window created: " << owner_->Width() << "x" << owner_->Height() << std::endl;

    // 设置窗口用户指针（用于回调中访问 Window 对象）
    glfwSetWindowUserPointer(handle_, owner_);

    // 设置回调
    glfwSetWindowCloseCallback(handle_, OnWindowClose);
    glfwSetWindowSizeCallback(handle_, OnWindowResize);
    glfwSetWindowFocusCallback(handle_, OnWindowFocus);
    glfwSetWindowPosCallback(handle_, OnWindowPos);
    glfwSetMouseButtonCallback(handle_, OnMouseButton);
    glfwSetCursorPosCallback(handle_, OnMouseMove);

    // 设置当前上下文
    glfwMakeContextCurrent(handle_);
}

void WindowInteropHelper::OnWindowClose(GLFWwindow* window) {
    auto* owner = static_cast<Window*>(glfwGetWindowUserPointer(window));
    if (owner) {
        owner->OnNativeWindowClose();
    }
}

void WindowInteropHelper::OnWindowResize(GLFWwindow* window, int width, int height) {
    auto* owner = static_cast<Window*>(glfwGetWindowUserPointer(window));
    if (owner) {
        owner->OnNativeWindowResize(width, height);
    }
}

void WindowInteropHelper::OnWindowFocus(GLFWwindow* window, int focused) {
    auto* owner = static_cast<Window*>(glfwGetWindowUserPointer(window));
    if (owner) {
        owner->OnNativeWindowFocus(focused != 0);
    }
}

void WindowInteropHelper::OnWindowPos(GLFWwindow* window, int xpos, int ypos) {
    auto* owner = static_cast<Window*>(glfwGetWindowUserPointer(window));
    if (owner) {
        owner->OnNativeWindowMove(xpos, ypos);
    }
}

void WindowInteropHelper::OnMouseButton(GLFWwindow* window, int button, int action, int mods) {
    auto* owner = static_cast<Window*>(glfwGetWindowUserPointer(window));
    if (owner) {
        owner->OnNativeMouseButton(button, action, mods);
    }
}

void WindowInteropHelper::OnMouseMove(GLFWwindow* window, double xpos, double ypos) {
    auto* owner = static_cast<Window*>(glfwGetWindowUserPointer(window));
    if (owner) {
        owner->OnNativeMouseMove(xpos, ypos);
    }
}

} // namespace fk::ui
