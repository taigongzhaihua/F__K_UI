#include "fk/ui/Window.h"
#include "fk/ui/WindowInteropHelper.h"
#include "fk/ui/Button.h"
#include "fk/ui/Panel.h"
#include "fk/ui/Control.h"
#include "fk/render/RenderHost.h"
#include "fk/render/IRenderer.h"
#include "fk/render/GlRenderer.h"

#include <GLFW/glfw3.h>
#include <iostream>
#include <stdexcept>
#include <functional>
#include <typeinfo>

namespace fk::ui {

// 依赖属性定义
FK_DEPENDENCY_PROPERTY_REGISTER(Window, Title, std::string);
FK_DEPENDENCY_PROPERTY_IMPL_REF(Window, Title, std::string);
FK_DEPENDENCY_PROPERTY_CALLBACK_REF(Window, Title, std::string);

FK_DEPENDENCY_PROPERTY_REGISTER(Window, Width, int);
FK_DEPENDENCY_PROPERTY_IMPL(Window, Width, int, 800);
FK_DEPENDENCY_PROPERTY_CALLBACK(Window, Width, int, 800);

FK_DEPENDENCY_PROPERTY_REGISTER(Window, Height, int);
FK_DEPENDENCY_PROPERTY_IMPL(Window, Height, int, 600);
FK_DEPENDENCY_PROPERTY_CALLBACK(Window, Height, int, 600);

// BuildMetadata 实现
binding::PropertyMetadata Window::BuildTitleMetadata() {
    return binding::PropertyMetadata(
        std::string("Window"),  // 默认值
        &Window::TitlePropertyChanged
    );
}

binding::PropertyMetadata Window::BuildWidthMetadata() {
    return binding::PropertyMetadata(
        800,  // 默认宽度
        &Window::WidthPropertyChanged
    );
}

binding::PropertyMetadata Window::BuildHeightMetadata() {
    return binding::PropertyMetadata(
        600,  // 默认高度
        &Window::HeightPropertyChanged
    );
}

// OnChanged 实现
void Window::OnTitleChanged(const std::string& oldValue, const std::string& newValue) {
    if (interopHelper_ && interopHelper_->HasHandle()) {
        glfwSetWindowTitle(interopHelper_->GetHandle(), newValue.c_str());
    }
}

void Window::OnWidthChanged(int oldValue, int newValue) {
    if (interopHelper_ && interopHelper_->HasHandle()) {
        const int width = GetWidth();
        const int height = GetHeight();
        glfwSetWindowSize(interopHelper_->GetHandle(), width, height);
        
        // 触发布局更新
        if (visible_) {
            PerformLayout();
        }
    }
}

void Window::OnHeightChanged(int oldValue, int newValue) {
    if (interopHelper_ && interopHelper_->HasHandle()) {
        const int width = GetWidth();
        const int height = GetHeight();
        glfwSetWindowSize(interopHelper_->GetHandle(), width, height);
        
        // 触发布局更新
        if (visible_) {
            PerformLayout();
        }
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

// 流式 API 包装（返回 Window::Ptr）
Window::Ptr Window::Title(const std::string& title) {
    SetTitle(title);
    return std::static_pointer_cast<Window>(shared_from_this());
}

std::string Window::Title() const {
    return GetTitle();
}

Window::Ptr Window::Width(int w) {
    SetWidth(w);
    return std::static_pointer_cast<Window>(shared_from_this());
}

int Window::Width() const {
    return GetWidth();
}

Window::Ptr Window::Height(int h) {
    SetHeight(h);
    return std::static_pointer_cast<Window>(shared_from_this());
}

int Window::Height() const {
    return GetHeight();
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
    
    // 🔥 关键修复: 将 Window 附加到逻辑树,这样所有子元素才能通过 GetRenderHost() 找到 RenderHost
    if (!IsAttachedToLogicalTree()) {
        AttachToLogicalTree(GetDispatcher());
    }

    // 创建 RenderHost（如果还没有）
    if (!renderHost_) {
        auto renderer = std::make_unique<render::GlRenderer>();
        
        render::RenderHostConfig config;
        config.window = std::static_pointer_cast<Window>(shared_from_this());
        config.renderer = std::move(renderer);
        
        renderHost_ = std::make_shared<render::RenderHost>(std::move(config));
        
        // 初始化渲染器
        render::RendererInitParams params;
        params.initialSize = {static_cast<uint32_t>(Width()), static_cast<uint32_t>(Height())};
        renderHost_->Initialize(params);
        
        std::cout << "RenderHost initialized for window (" << Width() << "x" << Height() << ")" << std::endl;
    }

    glfwShowWindow(interopHelper_->GetHandle());
    visible_ = true;

    // 执行初始布局
    auto content = GetContent();
    if (content) {
        PerformLayout();
        // 初始渲染请求
        if (renderHost_) {
            renderHost_->RequestRender();
        }
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
    
    // 如果布局失效,执行布局
    if (!content->IsArrangeValid() || !content->IsMeasureValid()) {
        PerformLayout();
    }
    
    // 准备帧上下文
    render::FrameContext frameCtx;
    frameCtx.elapsedSeconds = glfwGetTime();
    frameCtx.deltaSeconds = 0.016f; // ~60 FPS
    frameCtx.clearColor = {0.15f, 0.2f, 0.3f, 1.0f};
    frameCtx.frameIndex = frameCount_++;
    
    // 执行渲染 (只有在 renderPending_ 为 true 时才真正渲染)
    bool didRender = renderHost_->RenderFrame(frameCtx, *content);
    
    // 如果真的渲染了,交换缓冲区
    if (didRender) {
        auto handle = GetNativeHandle();
        if (handle) {
            glfwSwapBuffers(handle);
        }
    }
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

void Window::OnNativeMouseButton(int button, int action, int mods) {
    // GLFW button: GLFW_MOUSE_BUTTON_LEFT = 0, GLFW_MOUSE_BUTTON_RIGHT = 1, GLFW_MOUSE_BUTTON_MIDDLE = 2
    // GLFW action: GLFW_PRESS = 1, GLFW_RELEASE = 0
    
    if (button != 0) { // 只处理左键
        return;
    }

    double xpos, ypos;
    glfwGetCursorPos(interopHelper_->GetHandle(), &xpos, &ypos);

    auto content = GetContent();
    if (!content) {
        return;
    }

    // 直接将事件传递给 content,让事件系统自动路由
    if (action == 1) { // GLFW_PRESS
        content->OnMouseButtonDown(button, xpos, ypos);
    } else if (action == 0) { // GLFW_RELEASE
        content->OnMouseButtonUp(button, xpos, ypos);
    }
}

void Window::OnNativeMouseMove(double xpos, double ypos) {
    auto content = GetContent();
    if (content) {
        content->OnMouseMove(xpos, ypos);
    }
}

void Window::OnNativeMouseScroll(double xoffset, double yoffset) {
    auto content = GetContent();
    if (content) {
        // 获取当前鼠标位置
        double mouseX, mouseY;
        glfwGetCursorPos(interopHelper_->GetHandle(), &mouseX, &mouseY);
        
        // 传递给内容,包含鼠标位置信息
        content->OnMouseWheel(xoffset, yoffset, mouseX, mouseY);
    }
}

void Window::OnNativeKey(int key, int scancode, int action, int mods) {
    auto* focused = detail::ControlBase::GetFocusedControl();
    const bool isPress = action == GLFW_PRESS || action == GLFW_REPEAT;
    const bool isRelease = action == GLFW_RELEASE;

    if (focused) {
        if (isPress) {
            if (focused->OnKeyDown(key, scancode, mods)) {
                return;
            }
        } else if (isRelease) {
            if (focused->OnKeyUp(key, scancode, mods)) {
                return;
            }
        }
    }

    if (auto content = GetContent()) {
        if (isPress) {
            content->OnKeyDown(key, scancode, mods);
        } else if (isRelease) {
            content->OnKeyUp(key, scancode, mods);
        }
    }
}

void Window::OnNativeChar(unsigned int codepoint) {
    if (auto* focused = detail::ControlBase::GetFocusedControl()) {
        if (focused->OnTextInput(codepoint)) {
            return;
        }
    }

    if (auto content = GetContent()) {
        content->OnTextInput(codepoint);
    }
}

} // namespace fk::ui
