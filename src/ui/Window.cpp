#include "fk/ui/Window.h"
#include "fk/ui/WindowInteropHelper.h"
#include "fk/render/RenderHost.h"
#include "fk/render/IRenderer.h"

#include <GLFW/glfw3.h>
#include <iostream>
#include <stdexcept>
#include <functional>

namespace fk::ui {

// 依赖属性定义
const binding::DependencyProperty& Window::TitleProperty() {
    static const auto& prop = binding::DependencyProperty::Register(
        "Title",
        typeid(std::string),
        typeid(Window),
        BuildTitleMetadata()
    );
    return prop;
}

binding::PropertyMetadata Window::BuildTitleMetadata() {
    return binding::PropertyMetadata(
        std::string("Window"),  // 默认值
        &Window::TitlePropertyChanged
    );
}

void Window::TitlePropertyChanged(
    binding::DependencyObject& sender,
    const binding::DependencyProperty& property,
    const std::any& oldValue,
    const std::any& newValue
) {
    auto* window = dynamic_cast<Window*>(&sender);
    if (!window || !window->interopHelper_ || !window->interopHelper_->HasHandle()) {
        return;
    }
    
    const auto& title = std::any_cast<const std::string&>(newValue);
    glfwSetWindowTitle(window->interopHelper_->GetHandle(), title.c_str());
}

// Width/Height 依赖属性
const binding::DependencyProperty& Window::WidthProperty() {
    static const auto& prop = binding::DependencyProperty::Register(
        "Width",
        typeid(int),
        typeid(Window),
        BuildWidthMetadata()
    );
    return prop;
}

const binding::DependencyProperty& Window::HeightProperty() {
    static const auto& prop = binding::DependencyProperty::Register(
        "Height",
        typeid(int),
        typeid(Window),
        BuildHeightMetadata()
    );
    return prop;
}

binding::PropertyMetadata Window::BuildWidthMetadata() {
    return binding::PropertyMetadata(
        800,  // 默认宽度
        &Window::SizePropertyChanged
    );
}

binding::PropertyMetadata Window::BuildHeightMetadata() {
    return binding::PropertyMetadata(
        600,  // 默认高度
        &Window::SizePropertyChanged
    );
}

void Window::SizePropertyChanged(
    binding::DependencyObject& sender,
    const binding::DependencyProperty& property,
    const std::any& oldValue,
    const std::any& newValue
) {
    auto* window = dynamic_cast<Window*>(&sender);
    if (!window || !window->interopHelper_ || !window->interopHelper_->HasHandle()) {
        return;
    }
    
    const int width = window->Width();
    const int height = window->Height();
    glfwSetWindowSize(window->interopHelper_->GetHandle(), width, height);
    
    // 触发布局更新
    if (window->visible_) {
        window->PerformLayout();
    }
}

Window::Window()
    : ContentControl()
    , visible_(false)
    , interopHelper_(std::make_unique<WindowInteropHelper>(this)) {
    // 默认值通过依赖属性元数据设置
}

Window::~Window() {
    // interopHelper_ 会自动销毁窗口
}

// Title Getter/Setter
std::string Window::Title() const {
    return std::any_cast<std::string>(GetValue(TitleProperty()));
}

Window::Ptr Window::Title(const std::string& title) {
    SetValue(TitleProperty(), title);
    return std::static_pointer_cast<Window>(shared_from_this());
}

// Width Getter/Setter
int Window::Width() const {
    return std::any_cast<int>(GetValue(WidthProperty()));
}

Window::Ptr Window::Width(int w) {
    SetValue(WidthProperty(), w);
    return std::static_pointer_cast<Window>(shared_from_this());
}

// Height Getter/Setter
int Window::Height() const {
    return std::any_cast<int>(GetValue(HeightProperty()));
}

Window::Ptr Window::Height(int h) {
    SetValue(HeightProperty(), h);
    return std::static_pointer_cast<Window>(shared_from_this());
}

void Window::OnContentChanged(UIElement* oldContent, UIElement* newContent) {
    // 调用基类实现
    ContentControl::OnContentChanged(oldContent, newContent);
    
    // 如果窗口可见，执行布局（布局会自动触发 InvalidateVisual）
    if (newContent && visible_) {
        PerformLayout();
    }
}

void Window::Show() {
    if (visible_) {
        return;
    }

    // 确保窗口已创建
    interopHelper_->EnsureHandle();

    glfwShowWindow(interopHelper_->GetHandle());
    visible_ = true;

    // 执行初始布局
    auto content = GetContent();
    if (content) {
        PerformLayout();
    }

    // 触发事件
    Opened();

    std::cout << "Window '" << Title() << "' opened (" 
              << Width() << "x" << Height() << ")" << std::endl;
}

void Window::Hide() {
    if (!visible_) {
        return;
    }

    if (interopHelper_->HasHandle()) {
        glfwHideWindow(interopHelper_->GetHandle());
    }

    visible_ = false;
    std::cout << "Window '" << Title() << "' hidden" << std::endl;
}

void Window::Close() {
    if (interopHelper_->HasHandle()) {
        glfwSetWindowShouldClose(interopHelper_->GetHandle(), GLFW_TRUE);
    }

    visible_ = false;
    Closed();
    std::cout << "Window '" << Title() << "' closed" << std::endl;
}
bool Window::ProcessEvents() {
    if (!interopHelper_->HasHandle()) {
        return false;
    }
    
    glfwPollEvents();
    return !glfwWindowShouldClose(interopHelper_->GetHandle());
}

void Window::RenderFrame() {
    auto content = GetContent();
    if (!renderHost_ || !content) {
        return;
    }
    
    // 请求渲染
    renderHost_->RequestRender();
    
    // 准备帧上下文
    render::FrameContext frameCtx;
    frameCtx.elapsedSeconds = glfwGetTime();
    frameCtx.deltaSeconds = 0.016f; // ~60 FPS
    frameCtx.clearColor = {0.15f, 0.2f, 0.3f, 1.0f};
    frameCtx.frameIndex = frameCount_++;
    
    // 执行渲染
    renderHost_->RenderFrame(frameCtx, *content);
}

GLFWwindow* Window::GetNativeHandle() const {
    return interopHelper_->GetHandle();
}

void Window::SetRenderHost(std::shared_ptr<render::RenderHost> renderHost) {
    renderHost_ = renderHost;
}

void Window::PerformLayout() {
    auto content = GetContent();
    if (!content) {
        return;
    }

    // 执行布局
    auto width = Width();
    auto height = Height();
    Size availableSize(static_cast<float>(width), static_cast<float>(height));
    content->Measure(availableSize);
    content->Arrange(Rect(0, 0, static_cast<float>(width), static_cast<float>(height)));

    std::cout << "Layout performed: " << width << "x" << height << std::endl;
}

void Window::RequestRender() {
    if (renderHost_) {
        renderHost_->RequestRender();
    }
}

// WindowInteropHelper 回调实现
void Window::OnNativeWindowClose() {
    visible_ = false;
    Closed();
}

void Window::OnNativeWindowResize(int width, int height) {
    // 更新依赖属性(会触发 SizePropertyChanged 回调)
    // 注意:这里直接设置值,不触发原生窗口大小更改(避免循环)
    SetValue(WidthProperty(), width);
    SetValue(HeightProperty(), height);

    // 更新渲染器视口
    if (renderHost_) {
        render::Extent2D newSize{
            static_cast<uint32_t>(width), 
            static_cast<uint32_t>(height)
        };
        renderHost_->Resize(newSize);
    }

    // 重新布局（布局会自动触发 InvalidateVisual）
    PerformLayout();

    // 触发事件
    Resized(width, height);
}

void Window::OnNativeWindowFocus(bool focused) {
    if (focused) {
        Activated();
    } else {
        Deactivated();
    }
}

void Window::OnNativeWindowMove(int x, int y) {
    // 预留给未来的窗口移动事件
}

} // namespace fk::ui
