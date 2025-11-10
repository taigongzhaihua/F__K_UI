#include "fk/ui/Window.h"

namespace fk::ui {

// ========== 依赖属性注册 ==========

const binding::DependencyProperty& Window::TitleProperty() {
    static auto& property = binding::DependencyProperty::Register(
        "Title",
        typeid(std::string),
        typeid(Window),
        {std::string("")}
    );
    return property;
}

const binding::DependencyProperty& Window::WindowStateProperty() {
    static auto& property = binding::DependencyProperty::Register(
        "WindowState",
        typeid(ui::WindowState),
        typeid(Window),
        {ui::WindowState::Normal}
    );
    return property;
}

const binding::DependencyProperty& Window::LeftProperty() {
    static auto& property = binding::DependencyProperty::Register(
        "Left",
        typeid(float),
        typeid(Window),
        {0.0f}
    );
    return property;
}

const binding::DependencyProperty& Window::TopProperty() {
    static auto& property = binding::DependencyProperty::Register(
        "Top",
        typeid(float),
        typeid(Window),
        {0.0f}
    );
    return property;
}

const binding::DependencyProperty& Window::ShowInTaskbarProperty() {
    static auto& property = binding::DependencyProperty::Register(
        "ShowInTaskbar",
        typeid(bool),
        typeid(Window),
        {true}
    );
    return property;
}

const binding::DependencyProperty& Window::TopmostProperty() {
    static auto& property = binding::DependencyProperty::Register(
        "Topmost",
        typeid(bool),
        typeid(Window),
        {false}
    );
    return property;
}

// ========== 构造/析构 ==========

Window::Window() {
    // 窗口默认尺寸
    SetWidth(800);
    SetHeight(600);
}

Window::~Window() {
    if (nativeHandle_) {
        // TODO: 释放原生窗口句柄
    }
}

// ========== 窗口状态 ==========

void Window::SetWindowState(fk::ui::WindowState value) {
    auto oldState = GetWindowState();
    SetValue(WindowStateProperty(), value);
    
    if (oldState != value) {
        OnWindowStateChanged(oldState, value);
        // TODO: 更新原生窗口状态
    }
}

void Window::OnWindowStateChanged(fk::ui::WindowState oldState, fk::ui::WindowState newState) {
    // 派生类可覆写此方法
}

// ========== 窗口操作 ==========

void Window::Show() {
    if (isModal_) {
        return; // 模态窗口不能用 Show()
    }
    
    isVisible_ = true;
    
    // TODO: 创建并显示原生窗口
    // 1. 如果 nativeHandle_ 为空，创建窗口
    // 2. 应用布局
    // 3. 显示窗口
}

bool Window::ShowDialog() {
    isModal_ = true;
    
    // TODO: 创建并显示模态窗口
    // 1. 创建窗口
    // 2. 应用布局
    // 3. 进入消息循环（阻塞直到窗口关闭）
    // 4. 返回对话框结果
    
    return false; // 临时返回
}

void Window::Close() {
    if (isClosing_) {
        return; // 防止重复关闭
    }
    
    isClosing_ = true;
    
    // 触发 Closing 事件，允许取消
    if (!OnClosing()) {
        isClosing_ = false;
        return;
    }
    
    Closing();  // 触发事件
    
    // TODO: 关闭原生窗口
    // 1. 销毁窗口句柄
    // 2. 如果是模态窗口，退出消息循环
    
    OnClosed();
    Closed();  // 触发事件
    
    nativeHandle_ = nullptr;
    isClosing_ = false;
}

void Window::Activate() {
    // TODO: 激活窗口（置于前台）
    Activated();  // 触发事件
}

void Window::Hide() {
    isVisible_ = false;
    // TODO: 隐藏窗口（不销毁）
}

bool Window::ProcessEvents() {
    // TODO: 处理窗口事件
    // 返回 true 表示继续运行，false 表示关闭
    return isVisible_ && !isClosing_;
}

void Window::RenderFrame() {
    // TODO: 渲染窗口内容
}

} // namespace fk::ui
