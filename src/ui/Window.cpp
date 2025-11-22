#include "fk/ui/Window.h"
#include "fk/ui/Brush.h"
#include "fk/render/DrawCommand.h"
#include "fk/ui/InputManager.h"
#include "fk/ui/FocusManager.h"
#include "fk/ui/TextBlock.h"
#include "fk/render/GlRenderer.h"
#include "fk/render/RenderList.h"
#include "fk/render/RenderContext.h"
#include "fk/render/TextRenderer.h"

#ifdef FK_HAS_GLFW
#include <GLFW/glfw3.h>
#ifdef FK_HAS_OPENGL
#include <GL/gl.h>
#endif
#endif

#include <iostream>
#include <chrono>
#include <thread>

namespace fk::ui {

// 模拟窗口句柄结构（用于无GLFW环境）
struct SimulatedWindow {
    std::string title;
    int width;
    int height;
    bool shouldClose{false};
    std::chrono::steady_clock::time_point lastFrame;
    
    SimulatedWindow(const std::string& t, int w, int h) 
        : title(t), width(w), height(h), lastFrame(std::chrono::steady_clock::now()) {}
};

#ifdef FK_HAS_GLFW
// GLFW 错误回调
static void glfwErrorCallback(int error, const char* description) {
    std::cerr << "GLFW Error " << error << ": " << description << std::endl;
}

// GLFW 初始化辅助函数（单例）
static bool InitializeGLFW() {
    static bool initialized = false;
    static bool initSuccess = false;
    
    if (!initialized) {
        initialized = true;
        glfwSetErrorCallback(glfwErrorCallback);
        initSuccess = (glfwInit() == GLFW_TRUE);
        if (initSuccess) {
            std::cout << "GLFW initialized successfully" << std::endl;
        } else {
            std::cerr << "Failed to initialize GLFW" << std::endl;
        }
    }
    
    return initSuccess;
}
#endif

// ========== 依赖属性注册 ==========

const binding::DependencyProperty& Window::WidthProperty() {
    static auto& property = binding::DependencyProperty::Register(
        "Width",
        typeid(float),
        typeid(Window),
        binding::PropertyMetadata{
            std::any(800.0f),  // Window默认宽度
            &Window::OnWidthChanged
        }
    );
    return property;
}

const binding::DependencyProperty& Window::HeightProperty() {
    static auto& property = binding::DependencyProperty::Register(
        "Height",
        typeid(float),
        typeid(Window),
        binding::PropertyMetadata{
            std::any(600.0f),  // Window默认高度
            &Window::OnHeightChanged
        }
    );
    return property;
}

const binding::DependencyProperty& Window::TitleProperty() {
    static auto& property = binding::DependencyProperty::Register(
        "Title",
        typeid(std::string),
        typeid(Window),
        binding::PropertyMetadata{
            std::any(std::string("")),
            &Window::OnTitleChanged
        }
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
        binding::PropertyMetadata{
            std::any(0.0f),
            &Window::OnLeftChanged
        }
    );
    return property;
}

const binding::DependencyProperty& Window::TopProperty() {
    static auto& property = binding::DependencyProperty::Register(
        "Top",
        typeid(float),
        typeid(Window),
        binding::PropertyMetadata{
            std::any(0.0f),
            &Window::OnTopChanged
        }
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
        binding::PropertyMetadata{
            std::any(false),
            &Window::OnTopmostChanged
        }
    );
    return property;
}

// ========== 构造/析构 ==========

Window::Window() {
    // 窗口默认尺寸
    SetWidth(800);
    SetHeight(600);
    
    // 初始化渲染系统（仅在支持OpenGL时）
#ifdef FK_HAS_OPENGL
    renderList_ = std::make_unique<render::RenderList>();
    renderer_ = std::make_unique<render::GlRenderer>();
#endif
    
    // 初始化焦点管理器并设置根节点为窗口本身
    focusManager_ = std::make_unique<FocusManager>();
    focusManager_->SetRoot(this);
    
    // 初始化输入管理器并设置根节点为窗口本身
    inputManager_ = std::make_unique<InputManager>();
    inputManager_->SetRoot(this);
    inputManager_->SetFocusManager(focusManager_.get());
    
    // 自动创建 NameScope（窗口级别的命名作用域）
    // 提供 O(1) 的名称查找性能
    CreateNameScope();
}

Window::~Window() {
    if (nativeHandle_) {
#ifdef FK_HAS_GLFW
        GLFWwindow* window = static_cast<GLFWwindow*>(nativeHandle_);
        glfwDestroyWindow(window);
        std::cout << "Window destroyed" << std::endl;
#else
        // 模拟窗口
        SimulatedWindow* window = static_cast<SimulatedWindow*>(nativeHandle_);
        delete window;
        std::cout << "Simulated window destroyed" << std::endl;
#endif
        nativeHandle_ = nullptr;
    }
}

// ========== 窗口状态 ==========

void Window::SetWindowState(enum WindowState value) {
    auto oldState = GetWindowState();
    SetValue(WindowStateProperty(), value);
    
    if (oldState != value) {
        OnWindowStateChanged(oldState, value);
        
#ifdef FK_HAS_GLFW
        if (nativeHandle_) {
            GLFWwindow* window = static_cast<GLFWwindow*>(nativeHandle_);
            
            switch (value) {
                case WindowState::Maximized:
                    glfwMaximizeWindow(window);
                    break;
                case WindowState::Minimized:
                    glfwIconifyWindow(window);
                    break;
                case WindowState::Normal:
                    glfwRestoreWindow(window);
                    break;
            }
        }
#endif
    }
}

void Window::OnWindowStateChanged(enum WindowState oldState, enum WindowState newState) {
    // 派生类可覆写此方法
}

// ========== 窗口操作 ==========

void Window::Show() {
    if (isModal_) {
        return; // 模态窗口不能用 Show()
    }
    
    // 创建窗口（如果还未创建）
    if (!nativeHandle_) {
#ifdef FK_HAS_GLFW
        if (!InitializeGLFW()) {
            std::cerr << "Cannot create window: GLFW not initialized" << std::endl;
            return;
        }
        
        // 设置窗口提示
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
        glfwWindowHint(GLFW_SAMPLES, 16); // 启用16x MSAA抗锯齿，提供最佳的线条质量
        
        // 获取窗口尺寸
        int width = static_cast<int>(GetWidth());
        int height = static_cast<int>(GetHeight());
        
        // 创建GLFW窗口
        GLFWwindow* window = glfwCreateWindow(
            width, 
            height, 
            GetTitle().c_str(), 
            nullptr, 
            nullptr
        );
        
        if (!window) {
            std::cerr << "Failed to create GLFW window" << std::endl;
            return;
        }
        
        nativeHandle_ = window;
        
        // 设置OpenGL上下文
        glfwMakeContextCurrent(window);
        
        // 启用垂直同步
        glfwSwapInterval(1);
        
        // 设置窗口位置（如果指定）
        float left = GetLeft();
        float top = GetTop();
        if (left != 0.0f || top != 0.0f) {
            glfwSetWindowPos(window, static_cast<int>(left), static_cast<int>(top));
        }
        
        // 应用 Topmost 属性
        ApplyTopmostToNativeWindow();
        
        // 设置用户指针，以便在回调中访问 Window 实例
        glfwSetWindowUserPointer(window, this);
        
        // 设置鼠标按钮回调
        glfwSetMouseButtonCallback(window, [](GLFWwindow* win, int button, int action, int mods) {
            auto* self = static_cast<Window*>(glfwGetWindowUserPointer(win));
            if (!self || !self->inputManager_) return;
            
            double xpos, ypos;
            glfwGetCursorPos(win, &xpos, &ypos);
            
            PlatformPointerEvent event;
            event.position = Point(static_cast<float>(xpos), static_cast<float>(ypos));
            event.button = button;
            event.ctrlKey = (mods & GLFW_MOD_CONTROL) != 0;
            event.shiftKey = (mods & GLFW_MOD_SHIFT) != 0;
            event.altKey = (mods & GLFW_MOD_ALT) != 0;
            
            if (action == GLFW_PRESS) {
                event.type = PlatformPointerEvent::Type::Down;
            } else if (action == GLFW_RELEASE) {
                event.type = PlatformPointerEvent::Type::Up;
            }
            
            self->inputManager_->ProcessPointerEvent(event);
        });
        
        // 设置鼠标移动回调
        glfwSetCursorPosCallback(window, [](GLFWwindow* win, double xpos, double ypos) {
            auto* self = static_cast<Window*>(glfwGetWindowUserPointer(win));
            if (!self || !self->inputManager_) return;
            
            PlatformPointerEvent event;
            event.type = PlatformPointerEvent::Type::Move;
            event.position = Point(static_cast<float>(xpos), static_cast<float>(ypos));
            
            self->inputManager_->ProcessPointerEvent(event);
        });
        
        // 设置滚轮回调
        glfwSetScrollCallback(window, [](GLFWwindow* win, double xoffset, double yoffset) {
            auto* self = static_cast<Window*>(glfwGetWindowUserPointer(win));
            if (!self || !self->inputManager_) return;
            
            double xpos, ypos;
            glfwGetCursorPos(win, &xpos, &ypos);
            
            PlatformPointerEvent event;
            event.type = PlatformPointerEvent::Type::Wheel;
            event.position = Point(static_cast<float>(xpos), static_cast<float>(ypos));
            event.wheelDelta = static_cast<int>(yoffset * 120); // 标准滚轮增量
            
            self->inputManager_->ProcessPointerEvent(event);
        });
        
        // 设置键盘回调
        glfwSetKeyCallback(window, [](GLFWwindow* win, int key, int scancode, int action, int mods) {
            auto* self = static_cast<Window*>(glfwGetWindowUserPointer(win));
            if (!self || !self->inputManager_) return;
            
            PlatformKeyEvent event;
            event.key = key;
            event.scanCode = scancode;
            event.isRepeat = (action == GLFW_REPEAT);
            event.ctrlKey = (mods & GLFW_MOD_CONTROL) != 0;
            event.shiftKey = (mods & GLFW_MOD_SHIFT) != 0;
            event.altKey = (mods & GLFW_MOD_ALT) != 0;
            
            if (action == GLFW_PRESS || action == GLFW_REPEAT) {
                event.type = PlatformKeyEvent::Type::Down;
            } else if (action == GLFW_RELEASE) {
                event.type = PlatformKeyEvent::Type::Up;
            }
            
            self->inputManager_->ProcessKeyboardEvent(event);
        });
        
        // 设置字符输入回调
        glfwSetCharCallback(window, [](GLFWwindow* win, unsigned int codepoint) {
            auto* self = static_cast<Window*>(glfwGetWindowUserPointer(win));
            if (!self || !self->inputManager_) return;
            
            PlatformKeyEvent event;
            event.type = PlatformKeyEvent::Type::Char;
            event.character = static_cast<char32_t>(codepoint);
            
            self->inputManager_->ProcessKeyboardEvent(event);
        });
        
        // 设置窗口刷新回调（在拖动/调整大小时继续渲染）
        glfwSetWindowRefreshCallback(window, [](GLFWwindow* win) {
            auto* self = static_cast<Window*>(glfwGetWindowUserPointer(win));
            if (!self) return;
            
            // 在拖动或调整大小时也继续渲染
            self->RenderFrame();
        });
        
        // 设置窗口尺寸变化回调
        glfwSetFramebufferSizeCallback(window, [](GLFWwindow* win, int width, int height) {
            auto* self = static_cast<Window*>(glfwGetWindowUserPointer(win));
            if (!self) return;
            
            // 更新窗口尺寸
            self->SetValue(WidthProperty(), static_cast<float>(width));
            self->SetValue(HeightProperty(), static_cast<float>(height));
            self->SetRenderSize(Size(static_cast<float>(width), static_cast<float>(height)));
            
            // 立即重新渲染
            self->RenderFrame();
        });
        
        // 注意：InputManager 和 FocusManager 的根节点已在构造函数中设置
        
        std::cout << "GLFW window created: " << GetTitle() 
                  << " (" << width << "x" << height << ")" << std::endl;
#else
        // 模拟窗口创建
        int width = static_cast<int>(GetWidth());
        int height = static_cast<int>(GetHeight());
        
        SimulatedWindow* window = new SimulatedWindow(GetTitle(), width, height);
        nativeHandle_ = window;
        
        std::cout << "Simulated window created: " << GetTitle() 
                  << " (" << width << "x" << height << ")" << std::endl;
#endif
    }
    
    isVisible_ = true;
    SetRenderSize(Size(GetWidth(), GetHeight()));
    
#ifdef FK_HAS_GLFW
    if (nativeHandle_) {
        GLFWwindow* window = static_cast<GLFWwindow*>(nativeHandle_);
        glfwShowWindow(window);
    }
#endif
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
    
    isVisible_ = false;
    
#ifdef FK_HAS_GLFW
    if (nativeHandle_) {
        GLFWwindow* window = static_cast<GLFWwindow*>(nativeHandle_);
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
#else
    if (nativeHandle_) {
        SimulatedWindow* window = static_cast<SimulatedWindow*>(nativeHandle_);
        window->shouldClose = true;
    }
#endif
    
    OnClosed();
    Closed();  // 触发事件
    
    isClosing_ = false;
}

void Window::Activate() {
#ifdef FK_HAS_GLFW
    if (nativeHandle_) {
        GLFWwindow* window = static_cast<GLFWwindow*>(nativeHandle_);
        glfwFocusWindow(window);
    }
#else
    // 模拟窗口激活
    std::cout << "Window activated: " << GetTitle() << std::endl;
#endif
    Activated();  // 触发事件
}

void Window::Hide() {
    isVisible_ = false;
#ifdef FK_HAS_GLFW
    if (nativeHandle_) {
        GLFWwindow* window = static_cast<GLFWwindow*>(nativeHandle_);
        glfwHideWindow(window);
    }
#else
    std::cout << "Window hidden: " << GetTitle() << std::endl;
#endif
}

bool Window::ProcessEvents() {
#ifdef FK_HAS_GLFW
    if (!nativeHandle_) {
        return false;
    }
    
    GLFWwindow* window = static_cast<GLFWwindow*>(nativeHandle_);
    
    // 检查窗口是否应该关闭
    if (glfwWindowShouldClose(window)) {
        isVisible_ = false;
        return false;
    }
    
    // 处理事件
    glfwPollEvents();
    
    return isVisible_ && !isClosing_;
#else
    // 模拟事件处理
    if (!nativeHandle_) {
        return false;
    }
    
    SimulatedWindow* window = static_cast<SimulatedWindow*>(nativeHandle_);
    
    if (window->shouldClose) {
        isVisible_ = false;
        return false;
    }
    
    // 模拟帧率控制（60 FPS）
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
        now - window->lastFrame
    ).count();
    
    if (elapsed < 16) { // 约 60 FPS
        std::this_thread::sleep_for(std::chrono::milliseconds(16 - elapsed));
    }
    
    window->lastFrame = std::chrono::steady_clock::now();
    
    return isVisible_ && !isClosing_;
#endif
}

void Window::RenderFrame() {
#ifdef FK_HAS_GLFW
    if (!nativeHandle_) {
        return;
    }
    
    GLFWwindow* window = static_cast<GLFWwindow*>(nativeHandle_);
    
    // 获取窗口尺寸
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    
#ifdef FK_HAS_OPENGL
    // 设置视口
    glViewport(0, 0, width, height);
    
    // 清除屏幕
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
#endif
    
    // 渲染UI内容
#ifdef FK_HAS_OPENGL
    if (renderer_ && renderList_) {
        // 初始化渲染器（如果还没初始化）
        if (!renderer_->IsInitialized()) {
            render::RendererInitParams params;
            params.initialSize.width = static_cast<std::uint32_t>(width);
            params.initialSize.height = static_cast<std::uint32_t>(height);
            renderer_->Initialize(params);
            
            // 设置全局 TextRenderer，供 TextBlock 在 Measure 阶段使用
            TextBlock::SetGlobalTextRenderer(renderer_->GetTextRenderer());
            
            // 记录初始视口大小
            lastViewportWidth_ = width;
            lastViewportHeight_ = height;
        }
        
        // 只在窗口大小改变时更新渲染器视口（性能优化）
        if (width != lastViewportWidth_ || height != lastViewportHeight_) {
            render::Extent2D newSize{
                static_cast<std::uint32_t>(width),
                static_cast<std::uint32_t>(height)
            };
            renderer_->Resize(newSize);
            
            // 更新缓存的视口大小
            lastViewportWidth_ = width;
            lastViewportHeight_ = height;
        }
        
        // 清空渲染命令列表
        renderList_->Clear();
        
        // 创建渲染上下文
        render::RenderContext context(renderList_.get(), renderer_->GetTextRenderer());
        

        
        // 从Content开始收集绘制命令  
        auto content = GetContent();
        
        if (content.has_value() && content.type() == typeid(UIElement*)) {
            auto* element = std::any_cast<UIElement*>(content);
            
            if (element) {
                // 执行布局
                auto availableSize = Size(static_cast<float>(width), static_cast<float>(height));
                element->Measure(availableSize);
                
                // 从左上角开始布局（移除居中逻辑）
                element->Arrange(Rect(0, 0, static_cast<float>(width), static_cast<float>(height)));
                
                // 收集绘制命令（不需要额外的变换偏移）
                element->CollectDrawCommands(context);
            }
        }
        
        // 渲染所有命令
        render::FrameContext frameCtx;
        frameCtx.elapsedSeconds = 0.0;
        frameCtx.deltaSeconds = 0.016;
        
        // 从 Window 的 Background 属性读取清除颜色
        auto* background = GetBackground();
        if (background) {
            // 尝试转换为 SolidColorBrush
            if (auto* solidBrush = dynamic_cast<SolidColorBrush*>(background)) {
                Color color = solidBrush->GetColor();
                frameCtx.clearColor = {color.r, color.g, color.b, color.a};
            } else {
                // 其他类型的画刷，使用默认浅灰色背景
                frameCtx.clearColor = {0.94f, 0.94f, 0.94f, 1.0f};
            }
        } else {
            // 没有设置背景，使用默认浅灰色背景
            frameCtx.clearColor = {0.94f, 0.94f, 0.94f, 1.0f};
        }
        
        renderer_->BeginFrame(frameCtx);
        renderer_->Draw(*renderList_);
        renderer_->EndFrame();
    }
#endif
    
    // 交换缓冲区
    glfwSwapBuffers(window);
#else
    // 模拟渲染
    if (!nativeHandle_) {
        return;
    }
    
    // 在模拟环境中，我们只是打印一条消息表示渲染发生了
    static int frameCount = 0;
    frameCount++;
    
    if (frameCount % 60 == 0) {
        SimulatedWindow* window = static_cast<SimulatedWindow*>(nativeHandle_);
        std::cout << "Rendering frame " << frameCount 
                  << " for window: " << window->title << std::endl;
    }
#endif
}

UIElement* Window::FindName(const std::string& name) {
    if (name.empty()) {
        return nullptr;
    }
    
    // 检查窗口自身的名称
    if (GetName() == name) {
        return this;
    }
    
    // 查找窗口内容
    auto content = GetContent();
    if (content.has_value() && content.type() == typeid(UIElement*)) {
        auto* element = std::any_cast<UIElement*>(content);
        if (element) {
            UIElement* found = element->FindName(name);
            if (found) {
                return found;
            }
        }
    }
    
    return nullptr;
}

// ========== 属性变更回调实现 ==========

void Window::OnTitleChanged(
    binding::DependencyObject& d,
    const binding::DependencyProperty& prop,
    const std::any& oldValue,
    const std::any& newValue
) {
    auto* window = dynamic_cast<Window*>(&d);
    if (!window || !window->nativeHandle_) {
        return;
    }
    
    try {
        std::string title = std::any_cast<std::string>(newValue);
        
#ifdef FK_HAS_GLFW
        GLFWwindow* glfwWindow = static_cast<GLFWwindow*>(window->nativeHandle_);
        glfwSetWindowTitle(glfwWindow, title.c_str());
#else
        SimulatedWindow* simWindow = static_cast<SimulatedWindow*>(window->nativeHandle_);
        simWindow->title = title;
#endif
    } catch (const std::bad_any_cast&) {
        // 忽略类型转换错误
    }
}

void Window::OnWidthChanged(
    binding::DependencyObject& d,
    const binding::DependencyProperty& prop,
    const std::any& oldValue,
    const std::any& newValue
) {
    auto* window = dynamic_cast<Window*>(&d);
    if (!window) {
        return;
    }
    
    try {
        float width = std::any_cast<float>(newValue);
        float height = window->GetHeight();
        window->SetRenderSize(Size(width, height));
        
#ifdef FK_HAS_GLFW
        if (window->nativeHandle_) {
            GLFWwindow* glfwWindow = static_cast<GLFWwindow*>(window->nativeHandle_);
            glfwSetWindowSize(glfwWindow, static_cast<int>(width), static_cast<int>(height));
        }
#else
        if (window->nativeHandle_) {
            SimulatedWindow* simWindow = static_cast<SimulatedWindow*>(window->nativeHandle_);
            simWindow->width = static_cast<int>(width);
        }
#endif
    } catch (const std::bad_any_cast&) {
        // 忽略类型转换错误
    }
}

void Window::OnHeightChanged(
    binding::DependencyObject& d,
    const binding::DependencyProperty& prop,
    const std::any& oldValue,
    const std::any& newValue
) {
    auto* window = dynamic_cast<Window*>(&d);
    if (!window) {
        return;
    }
    
    try {
        float width = window->GetWidth();
        float height = std::any_cast<float>(newValue);
        window->SetRenderSize(Size(width, height));
        
#ifdef FK_HAS_GLFW
        if (window->nativeHandle_) {
            GLFWwindow* glfwWindow = static_cast<GLFWwindow*>(window->nativeHandle_);
            glfwSetWindowSize(glfwWindow, static_cast<int>(width), static_cast<int>(height));
        }
#else
        if (window->nativeHandle_) {
            SimulatedWindow* simWindow = static_cast<SimulatedWindow*>(window->nativeHandle_);
            simWindow->height = static_cast<int>(height);
        }
#endif
    } catch (const std::bad_any_cast&) {
        // 忽略类型转换错误
    }
}

void Window::OnLeftChanged(
    binding::DependencyObject& d,
    const binding::DependencyProperty& prop,
    const std::any& oldValue,
    const std::any& newValue
) {
    auto* window = dynamic_cast<Window*>(&d);
    if (!window || !window->nativeHandle_) {
        return;
    }
    
    try {
        float left = std::any_cast<float>(newValue);
        float top = window->GetTop();
        
#ifdef FK_HAS_GLFW
        GLFWwindow* glfwWindow = static_cast<GLFWwindow*>(window->nativeHandle_);
        glfwSetWindowPos(glfwWindow, static_cast<int>(left), static_cast<int>(top));
#endif
        // 模拟窗口不支持位置设置
    } catch (const std::bad_any_cast&) {
        // 忽略类型转换错误
    }
}

void Window::OnTopChanged(
    binding::DependencyObject& d,
    const binding::DependencyProperty& prop,
    const std::any& oldValue,
    const std::any& newValue
) {
    auto* window = dynamic_cast<Window*>(&d);
    if (!window || !window->nativeHandle_) {
        return;
    }
    
    try {
        float left = window->GetLeft();
        float top = std::any_cast<float>(newValue);
        
#ifdef FK_HAS_GLFW
        GLFWwindow* glfwWindow = static_cast<GLFWwindow*>(window->nativeHandle_);
        glfwSetWindowPos(glfwWindow, static_cast<int>(left), static_cast<int>(top));
#endif
        // 模拟窗口不支持位置设置
    } catch (const std::bad_any_cast&) {
        // 忽略类型转换错误
    }
}

void Window::OnTopmostChanged(
    binding::DependencyObject& d,
    const binding::DependencyProperty& prop,
    const std::any& oldValue,
    const std::any& newValue
) {
    auto* window = dynamic_cast<Window*>(&d);
    if (!window || !window->nativeHandle_) {
        return;
    }
    
    window->ApplyTopmostToNativeWindow();
}

void Window::ApplyTopmostToNativeWindow() {
#ifdef FK_HAS_GLFW
    if (!nativeHandle_) {
        return;
    }
    
    GLFWwindow* glfwWindow = static_cast<GLFWwindow*>(nativeHandle_);
    bool topmost = GetTopmost();
    
    // GLFW 3.2+ 支持 glfwSetWindowAttrib
    // 设置窗口为置顶或取消置顶
    #ifdef GLFW_FLOATING
    glfwSetWindowAttrib(glfwWindow, GLFW_FLOATING, topmost ? GLFW_TRUE : GLFW_FALSE);
    #endif
#endif
    // 模拟窗口不支持置顶属性
}

} // namespace fk::ui
