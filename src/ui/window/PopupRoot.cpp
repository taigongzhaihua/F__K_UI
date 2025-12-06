#include "fk/ui/window/PopupRoot.h"
#include "fk/ui/base/UIElement.h"
#include "fk/render/GlRenderer.h"
#include "fk/render/RenderList.h"
#include "fk/render/RenderContext.h"
#include "fk/render/TextRenderer.h"
#include "fk/ui/text/TextBlock.h"
#include "fk/ui/input/InputManager.h"

#ifdef FK_HAS_GLFW
#include <GLFW/glfw3.h>
#ifdef FK_HAS_OPENGL
#include <GL/gl.h>
#endif
#endif

#include <iostream>

namespace fk::ui {

PopupRoot::PopupRoot() {
    // 构造函数保持简单，实际初始化在 Initialize() 中进行
}

PopupRoot::~PopupRoot() {
    Shutdown();
}

// ========== 生命周期管理 ==========

void PopupRoot::Initialize() {
    if (initialized_) {
        return;
    }
    
    // 初始化渲染系统
#ifdef FK_HAS_OPENGL
    renderList_ = std::make_unique<render::RenderList>();
    // renderer_ 将在窗口创建后初始化
#endif
    
    // 初始化输入管理器
    inputManager_ = std::make_unique<InputManager>();
    // content_ 设置后将设置 root
    
    initialized_ = true;
    std::cout << "[PopupRoot] Initialized" << std::endl;
}

void PopupRoot::Shutdown() {
    if (!initialized_) {
        return;
    }
    
    // 销毁窗口
    Destroy();
    
    // 清理渲染资源
    CleanupRenderer();
    
    initialized_ = false;
    std::cout << "[PopupRoot] Shutdown" << std::endl;
}

// ========== 窗口管理 ==========

void PopupRoot::Create(int width, int height) {
    if (nativeHandle_) {
        std::cerr << "[PopupRoot] Window already created" << std::endl;
        return;
    }
    
#ifdef FK_HAS_GLFW
    // 设置窗口提示
    glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);        // 无边框
    glfwWindowHint(GLFW_FLOATING, GLFW_TRUE);          // 置顶
    glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, allowsTransparency_ ? GLFW_TRUE : GLFW_FALSE);
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);          // 初始隐藏
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);        // 不可调整大小
    glfwWindowHint(GLFW_FOCUSED, GLFW_FALSE);          // 不自动获取焦点
    
    // 设置 OpenGL 版本
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    // 创建窗口
    GLFWwindow* window = glfwCreateWindow(width, height, "Popup", nullptr, nullptr);
    
    if (!window) {
        std::cerr << "[PopupRoot] Failed to create GLFW window" << std::endl;
        return;
    }
    
    nativeHandle_ = window;
    width_ = width;
    height_ = height;
    
    std::cout << "[PopupRoot] Window created: " << width << "x" << height << std::endl;
    
    // 设置用户指针，以便在回调中访问 PopupRoot 实例
    glfwSetWindowUserPointer(window, this);
    
    // 设置鼠标按钮回调
    glfwSetMouseButtonCallback(window, [](GLFWwindow* win, int button, int action, int mods) {
        auto* self = static_cast<PopupRoot*>(glfwGetWindowUserPointer(win));
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
        auto* self = static_cast<PopupRoot*>(glfwGetWindowUserPointer(win));
        if (!self || !self->inputManager_) return;
        
        PlatformPointerEvent event;
        event.type = PlatformPointerEvent::Type::Move;
        event.position = Point(static_cast<float>(xpos), static_cast<float>(ypos));
        
        self->inputManager_->ProcessPointerEvent(event);
    });
    
    // 设置滚轮回调
    glfwSetScrollCallback(window, [](GLFWwindow* win, double xoffset, double yoffset) {
        auto* self = static_cast<PopupRoot*>(glfwGetWindowUserPointer(win));
        if (!self || !self->inputManager_) return;
        
        double xpos, ypos;
        glfwGetCursorPos(win, &xpos, &ypos);
        
        PlatformPointerEvent event;
        event.type = PlatformPointerEvent::Type::Wheel;
        event.position = Point(static_cast<float>(xpos), static_cast<float>(ypos));
        event.wheelDelta = static_cast<int>(yoffset * 120);
        
        self->inputManager_->ProcessPointerEvent(event);
    });
    
    // 设置键盘回调
    glfwSetKeyCallback(window, [](GLFWwindow* win, int key, int scancode, int action, int mods) {
        auto* self = static_cast<PopupRoot*>(glfwGetWindowUserPointer(win));
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
        auto* self = static_cast<PopupRoot*>(glfwGetWindowUserPointer(win));
        if (!self || !self->inputManager_) return;
        
        PlatformKeyEvent event;
        event.type = PlatformKeyEvent::Type::Char;
        event.character = static_cast<char32_t>(codepoint);
        
        self->inputManager_->ProcessKeyboardEvent(event);
    });
    
    // 初始化渲染器
    InitializeRenderer();
#else
    std::cerr << "[PopupRoot] GLFW not available" << std::endl;
#endif
}

void PopupRoot::Destroy() {
    if (!nativeHandle_) {
        return;
    }
    
#ifdef FK_HAS_GLFW
    GLFWwindow* window = static_cast<GLFWwindow*>(nativeHandle_);
    glfwDestroyWindow(window);
    std::cout << "[PopupRoot] Window destroyed" << std::endl;
#endif
    
    nativeHandle_ = nullptr;
    isVisible_ = false;
}

void PopupRoot::Show(Point screenPos) {
    if (!nativeHandle_) {
        std::cerr << "[PopupRoot] Cannot show: window not created" << std::endl;
        return;
    }
    
#ifdef FK_HAS_GLFW
    GLFWwindow* window = static_cast<GLFWwindow*>(nativeHandle_);
    
    // 设置窗口位置（屏幕坐标）
    glfwSetWindowPos(window, static_cast<int>(screenPos.x), static_cast<int>(screenPos.y));
    
    // 显示窗口
    glfwShowWindow(window);
    
    isVisible_ = true;
    std::cout << "[PopupRoot] Shown at (" << screenPos.x << ", " << screenPos.y << ")" << std::endl;
#endif
}

void PopupRoot::Hide() {
    if (!nativeHandle_) {
        return;
    }
    
#ifdef FK_HAS_GLFW
    GLFWwindow* window = static_cast<GLFWwindow*>(nativeHandle_);
    glfwHideWindow(window);
    isVisible_ = false;
    std::cout << "[PopupRoot] Hidden" << std::endl;
#endif
}

void PopupRoot::SetSize(int width, int height) {
    width_ = width;
    height_ = height;
    
    if (!nativeHandle_) {
        return;
    }
    
#ifdef FK_HAS_GLFW
    GLFWwindow* window = static_cast<GLFWwindow*>(nativeHandle_);
    glfwSetWindowSize(window, width, height);
    
    // 通知渲染器调整大小
    Resize(width, height);
    
    std::cout << "[PopupRoot] Resized to " << width << "x" << height << std::endl;
#endif
}

Size PopupRoot::GetSize() const {
    return Size(static_cast<float>(width_), static_cast<float>(height_));
}

// ========== 内容管理 ==========

void PopupRoot::SetContent(UIElement* content) {
    content_ = content;
    
    // 设置 InputManager 的根节点
    if (inputManager_ && content_) {
        // content_ 需要是 Visual 类型，大多数 UIElement 都继承自 Visual
        inputManager_->SetRoot(dynamic_cast<Visual*>(content_));
    }
    
    std::cout << "[PopupRoot] Content set" << std::endl;
}

// ========== 渲染 ==========

void PopupRoot::RenderFrame() {
    if (!nativeHandle_ || !isVisible_) {
        return;
    }
    
#ifdef FK_HAS_GLFW
    GLFWwindow* window = static_cast<GLFWwindow*>(nativeHandle_);
    
    // 使窗口上下文为当前
    glfwMakeContextCurrent(window);
    
    // 获取窗口尺寸
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    
#ifdef FK_HAS_OPENGL
    // 设置视口
    glViewport(0, 0, width, height);
    
    // 清除屏幕（透明背景）
    if (allowsTransparency_) {
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    } else {
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    }
    glClear(GL_COLOR_BUFFER_BIT);
#endif
    
    // 渲染内容
#ifdef FK_HAS_OPENGL
    if (renderer_ && renderList_ && content_) {
        // 检查视口大小变化
        if (width != lastViewportWidth_ || height != lastViewportHeight_) {
            Resize(width, height);
        }
        
        // 清空渲染命令列表
        renderList_->Clear();
        
        // 创建渲染上下文
        render::RenderContext context(renderList_.get(), renderer_->GetTextRenderer());
        
        // 执行布局
        auto availableSize = Size(static_cast<float>(width), static_cast<float>(height));
        content_->Measure(availableSize);
        content_->Arrange(Rect(0, 0, static_cast<float>(width), static_cast<float>(height)));
        
        // 收集绘制命令
        content_->CollectDrawCommands(context);
        
        // 渲染所有命令
        render::FrameContext frameCtx;
        frameCtx.elapsedSeconds = 0.0;
        frameCtx.deltaSeconds = 0.016;
        frameCtx.clearColor = {0.0f, 0.0f, 0.0f, 0.0f};  // 透明背景
        
        renderer_->BeginFrame(frameCtx);
        renderer_->Draw(*renderList_);
        renderer_->EndFrame();
    }
#endif
    
    // 交换缓冲区
    glfwSwapBuffers(window);
#endif
}

void PopupRoot::Resize(int width, int height) {
    if (!renderer_) {
        return;
    }
    
#ifdef FK_HAS_OPENGL
    render::Extent2D newSize{
        static_cast<std::uint32_t>(width),
        static_cast<std::uint32_t>(height)
    };
    renderer_->Resize(newSize);
    
    lastViewportWidth_ = width;
    lastViewportHeight_ = height;
    
    std::cout << "[PopupRoot] Renderer resized to " << width << "x" << height << std::endl;
#endif
}

// ========== 属性设置 ==========

void PopupRoot::SetAllowsTransparency(bool allow) {
    allowsTransparency_ = allow;
    
    // 如果窗口已创建，需要重新创建才能应用透明设置
    if (nativeHandle_) {
        std::cerr << "[PopupRoot] Warning: Transparency setting changed after window creation" << std::endl;
    }
}

void PopupRoot::SetTopmost(bool topmost) {
    isTopmost_ = topmost;
    
#ifdef FK_HAS_GLFW
    if (nativeHandle_) {
        GLFWwindow* window = static_cast<GLFWwindow*>(nativeHandle_);
        glfwSetWindowAttrib(window, GLFW_FLOATING, topmost ? GLFW_TRUE : GLFW_FALSE);
        std::cout << "[PopupRoot] Topmost set to " << topmost << std::endl;
    }
#endif
}

// ========== 私有方法 ==========

void PopupRoot::InitializeRenderer() {
    if (!nativeHandle_) {
        return;
    }
    
#ifdef FK_HAS_GLFW
    GLFWwindow* window = static_cast<GLFWwindow*>(nativeHandle_);
    glfwMakeContextCurrent(window);
#endif
    
#ifdef FK_HAS_OPENGL
    // 创建渲染器
    renderer_ = std::make_unique<render::GlRenderer>();
    
    // 初始化渲染器
    render::RendererInitParams params;
    params.initialSize.width = static_cast<std::uint32_t>(width_);
    params.initialSize.height = static_cast<std::uint32_t>(height_);
    renderer_->Initialize(params);
    
    // 设置全局 TextRenderer
    TextBlock::SetGlobalTextRenderer(renderer_->GetTextRenderer());
    
    lastViewportWidth_ = width_;
    lastViewportHeight_ = height_;
    
    std::cout << "[PopupRoot] Renderer initialized" << std::endl;
#endif
}

void PopupRoot::CleanupRenderer() {
    renderer_.reset();
    renderList_.reset();
    std::cout << "[PopupRoot] Renderer cleaned up" << std::endl;
}

// ========== 事件处理 ==========

bool PopupRoot::ProcessEvents() {
    if (!nativeHandle_) {
        return false;
    }
    
#ifdef FK_HAS_GLFW
    GLFWwindow* window = static_cast<GLFWwindow*>(nativeHandle_);
    
    // 检查窗口是否应该关闭
    if (glfwWindowShouldClose(window)) {
        return false;
    }
    
    // GLFW 事件会通过回调自动处理
    // 这里只需要检查窗口状态
    return true;
#else
    return false;
#endif
}

bool PopupRoot::ContainsScreenPoint(int screenX, int screenY) const {
    if (!nativeHandle_ || !isVisible_) {
        return false;
    }
    
#ifdef FK_HAS_GLFW
    GLFWwindow* window = static_cast<GLFWwindow*>(nativeHandle_);
    
    // 获取窗口位置
    int winX, winY;
    glfwGetWindowPos(window, &winX, &winY);
    
    // 获取窗口大小
    int winWidth, winHeight;
    glfwGetWindowSize(window, &winWidth, &winHeight);
    
    // 检查点是否在窗口矩形内
    return (screenX >= winX && screenX < winX + winWidth &&
            screenY >= winY && screenY < winY + winHeight);
#else
    return false;
#endif
}

} // namespace fk::ui
