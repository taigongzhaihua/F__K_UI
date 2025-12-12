/**
 * @file Popup.cpp
 * @brief Popup 控件实现
 */

#include "fk/ui/controls/Popup.h"
#include "fk/ui/window/PopupRoot.h"
#include "fk/ui/PopupService.h"
#include "fk/ui/Window.h"
#include <iostream>
#include <algorithm>
#include <cmath>
#include <limits>

#ifdef FK_HAS_GLFW
#include <GLFW/glfw3.h>
#endif

namespace fk::ui {

// 显式实例化 FrameworkElement<Popup> 模板
template class FrameworkElement<Popup>;

// ========== 依赖属性定义 ==========

const binding::DependencyProperty& Popup::IsOpenProperty() {
    static auto& property = binding::DependencyProperty::Register(
        "IsOpen",
        typeid(bool),
        typeid(Popup),
        binding::PropertyMetadata(
            false, // 默认关闭
            &Popup::OnIsOpenChanged
        )
    );
    return property;
}

const binding::DependencyProperty& Popup::ChildProperty() {
    static auto& property = binding::DependencyProperty::Register(
        "Child",
        typeid(UIElement*),
        typeid(Popup),
        binding::PropertyMetadata(
            static_cast<UIElement*>(nullptr),
            &Popup::OnChildChanged
        )
    );
    return property;
}

const binding::DependencyProperty& Popup::PlacementTargetProperty() {
    static auto& property = binding::DependencyProperty::Register(
        "PlacementTarget",
        typeid(UIElement*),
        typeid(Popup),
        {static_cast<UIElement*>(nullptr)}
    );
    return property;
}

const binding::DependencyProperty& Popup::PlacementProperty() {
    static auto& property = binding::DependencyProperty::Register(
        "Placement",
        typeid(PlacementMode),
        typeid(Popup),
        {PlacementMode::Bottom} // 默认底部对齐
    );
    return property;
}

const binding::DependencyProperty& Popup::HorizontalOffsetProperty() {
    static auto& property = binding::DependencyProperty::Register(
        "HorizontalOffset",
        typeid(float),
        typeid(Popup),
        {0.0f}
    );
    return property;
}

const binding::DependencyProperty& Popup::VerticalOffsetProperty() {
    static auto& property = binding::DependencyProperty::Register(
        "VerticalOffset",
        typeid(float),
        typeid(Popup),
        {0.0f}
    );
    return property;
}

const binding::DependencyProperty& Popup::StaysOpenProperty() {
    static auto& property = binding::DependencyProperty::Register(
        "StaysOpen",
        typeid(bool),
        typeid(Popup),
        {true} // 默认保持打开
    );
    return property;
}

const binding::DependencyProperty& Popup::AllowsTransparencyProperty() {
    static auto& property = binding::DependencyProperty::Register(
        "AllowsTransparency",
        typeid(bool),
        typeid(Popup),
        {false} // 默认不透明
    );
    return property;
}

// 注：Width/Height 属性由 FrameworkElement<Popup> 提供

// ========== 构造/析构 ==========

Popup::Popup() {
    // Popup 本身不在可视树中渲染，因此不需要额外初始化
    // 设置默认大小
    SetWidth(200.0f);
    SetHeight(150.0f);
}

Popup::~Popup() {
    // 确保关闭并清理
    if (GetIsOpen()) {
        Close();
    }
}

// ========== 依赖属性回调 ==========

void Popup::OnIsOpenChanged(
    binding::DependencyObject& d,
    const binding::DependencyProperty& prop,
    const std::any& oldValue,
    const std::any& newValue
) {
    auto* popup = dynamic_cast<Popup*>(&d);
    if (!popup) {
        return;
    }

    bool isOpen = std::any_cast<bool>(newValue);
    
    if (isOpen) {
        popup->Open();
    } else {
        popup->Close();
    }
}

void Popup::OnChildChanged(
    binding::DependencyObject& d,
    const binding::DependencyProperty& prop,
    const std::any& oldValue,
    const std::any& newValue
) {
    auto* popup = dynamic_cast<Popup*>(&d);
    if (!popup || !popup->popupRoot_) {
        return;
    }

    // 更新 PopupRoot 的内容
    UIElement* child = std::any_cast<UIElement*>(newValue);
    popup->popupRoot_->SetContent(child);
}

// ========== 核心方法 ==========

void Popup::Open() {
    // 如果已经显示，不重复操作
    if (isPopupRootCreated_ && popupRoot_ && popupRoot_->IsVisible()) {
        std::cout << "Popup already visible, skipping Open()" << std::endl;
        return;
    }

    // 创建 PopupRoot（如果还没有）
    if (!popupRoot_) {
        popupRoot_ = std::make_unique<PopupRoot>();
        popupRoot_->Initialize();
    }

    // 如果窗口已创建但被隐藏，重新显示
    if (isPopupRootCreated_ && !popupRoot_->IsVisible()) {
        std::cout << "Reusing existing PopupRoot window" << std::endl;
        // 重新计算位置
        Point screenPos = CalculateScreenPosition();
        // 显示窗口
        popupRoot_->Show(screenPos);
        
        // 启动淡入动画
#ifdef FK_HAS_GLFW
        isAnimating_ = true;
        isClosingAnimation_ = false;
        animationProgress_ = 0.0f;
        animationStartTime_ = static_cast<float>(glfwGetTime());
#endif
        
        // 注册到 PopupService
        PopupService::Instance().RegisterPopup(this);
        
        // 触发 Opened 事件
        Opened();
        
        std::cout << "Popup reopened at (" << screenPos.x << ", " << screenPos.y << ")" << std::endl;
        return;
    }

    // 首次创建窗口
    // 设置透明度支持
    bool allowsTransparency = GetAllowsTransparency();
    popupRoot_->SetAllowsTransparency(allowsTransparency);
    
    // 设置主窗口句柄（用于焦点管理）
    // Popup 本身不在可视树中，需要通过 PlacementTarget 找到根窗口
    Window* rootWindow = nullptr;
    UIElement* placementTarget = GetPlacementTarget();
    if (placementTarget) {
        rootWindow = placementTarget->GetRootWindow();
    }
    
    if (rootWindow) {
        void* nativeHandle = rootWindow->GetNativeHandle();
        std::cout << "[Popup] Root window found: " << rootWindow 
                  << ", native handle: " << nativeHandle << std::endl;
        popupRoot_->SetOwnerWindow(nativeHandle);
    } else {
        std::cout << "[Popup] Warning: No root window found (PlacementTarget: " 
                  << placementTarget << ")" << std::endl;
    }

    // 创建窗口
    int width = static_cast<int>(GetWidth());
    int height = static_cast<int>(GetHeight());
    popupRoot_->Create(width, height);
    isPopupRootCreated_ = true;

    // 设置内容
    UIElement* child = GetChild();
    if (child) {
        popupRoot_->SetContent(child);
    }

    // 计算位置
    Point screenPos = CalculateScreenPosition();

    // 显示窗口
    popupRoot_->Show(screenPos);

    // 启动淡入动画
#ifdef FK_HAS_GLFW
    isAnimating_ = true;
    isClosingAnimation_ = false;
    animationProgress_ = 0.0f;
    animationStartTime_ = static_cast<float>(glfwGetTime());
#endif

    // 注册到 PopupService
    PopupService::Instance().RegisterPopup(this);

    // 触发 Opened 事件
    Opened();

    std::cout << "Popup opened at (" << screenPos.x << ", " << screenPos.y 
              << ") size: " << width << "x" << height << std::endl;
}

void Popup::Close() {
    if (!isPopupRootCreated_ || !popupRoot_) {
        return;
    }

    // 启动淡出动画
#ifdef FK_HAS_GLFW
    if (!isAnimating_) {
        isAnimating_ = true;
        isClosingAnimation_ = true;
        animationProgress_ = 0.0f;
        animationStartTime_ = static_cast<float>(glfwGetTime());
        // 淡出动画会在 UpdatePopup 中处理，完成后才真正关闭
        return;
    }
#endif

    // 直接关闭（无动画或动画已完成）
    popupRoot_->Hide();

    // 从 PopupService 注销
    PopupService::Instance().UnregisterPopup(this);

    // 触发 Closed 事件
    Closed();

    std::cout << "Popup closed" << std::endl;

    // 注意：不销毁 PopupRoot，保留以便重新打开
    // 如果需要完全销毁，在析构函数中处理
}

void Popup::RenderPopup() {
    if (popupRoot_ && isPopupRootCreated_) {
        popupRoot_->RenderFrame();
    }
}

void Popup::UpdatePopup() {
    // 处理 PopupRoot 的窗口事件（输入等）
    if (popupRoot_) {
        popupRoot_->ProcessEvents();
    }
    
    // 更新动画
#ifdef FK_HAS_GLFW
    if (isAnimating_) {
        float currentTime = static_cast<float>(glfwGetTime());
        float elapsed = currentTime - animationStartTime_;
        animationProgress_ = std::min(elapsed / animationDuration_, 1.0f);
        
        // 计算不透明度 (使用 ease-out 缓动)
        float eased = animationProgress_ * (2.0f - animationProgress_); // ease-out quad
        float opacity = isClosingAnimation_ ? (1.0f - eased) : eased;
        
        // 设置窗口不透明度
        if (popupRoot_) {
            GLFWwindow* window = static_cast<GLFWwindow*>(popupRoot_->GetNativeHandle());
            if (window) {
                glfwSetWindowOpacity(window, opacity);
            }
        }
        
        // 检查动画是否完成
        if (animationProgress_ >= 1.0f) {
            isAnimating_ = false;
            
            // 如果是关闭动画，现在真正关闭
            if (isClosingAnimation_) {
                isClosingAnimation_ = false;
                
                // 重置不透明度
                if (popupRoot_) {
                    GLFWwindow* window = static_cast<GLFWwindow*>(popupRoot_->GetNativeHandle());
                    if (window) {
                        glfwSetWindowOpacity(window, 1.0f);
                    }
                }
                
                // 真正关闭
                popupRoot_->Hide();
                PopupService::Instance().UnregisterPopup(this);
                Closed();
                std::cout << "Popup closed (after fade-out animation)" << std::endl;
            }
        }
    }
#endif
    
    // 未来可以添加：
    // - 检测 PlacementTarget 位置变化并更新 Popup 位置
}

// ========== 定位计算 ==========

Point Popup::CalculateScreenPosition() {
    PlacementMode mode = GetPlacement();
    UIElement* target = GetPlacementTarget();
    float hOffset = GetHorizontalOffset();
    float vOffset = GetVerticalOffset();
    float popupWidth = GetWidth();
    float popupHeight = GetHeight();

    // 模式 1: Absolute - 直接使用偏移作为屏幕坐标
    if (mode == PlacementMode::Absolute) {
        Point pos(hOffset, vOffset);
        // 仍然应用边界约束，防止完全超出屏幕
        return ApplyBoundaryConstraints(pos, popupWidth, popupHeight);
    }

    // 如果没有 PlacementTarget，退回到 Absolute 模式
    if (!target) {
        std::cerr << "Warning: No PlacementTarget set, using Absolute mode" << std::endl;
        Point pos(hOffset, vOffset);
        return ApplyBoundaryConstraints(pos, popupWidth, popupHeight);
    }

    // 获取 Target 在屏幕上的边界
    Rect targetBounds = target->GetBoundsOnScreen();
    
    // Day 9: 获取 Target 所在的显示器工作区域
    Rect workArea = GetMonitorWorkAreaAt(
        targetBounds.x + targetBounds.width / 2.0f,
        targetBounds.y + targetBounds.height / 2.0f
    );
    
    // Day 9: 智能翻转 - 尝试自动调整 Placement 模式以适应屏幕边界
    PlacementMode effectiveMode = TryFlipPlacement(mode, targetBounds, popupWidth, popupHeight, workArea);
    
    // 计算基础位置（使用可能已翻转的模式）
    Point basePos = CalculateBasePlacement(effectiveMode, targetBounds, popupWidth, popupHeight);

    // 应用偏移
    Point finalPos(basePos.x + hOffset, basePos.y + vOffset);

    // 最后应用边界约束，确保不超出屏幕（作为最后的保障）
    finalPos = ApplyBoundaryConstraints(finalPos, popupWidth, popupHeight);

    return finalPos;
}

Point Popup::ApplyBoundaryConstraints(Point position, float popupWidth, float popupHeight) {
    // Day 9: 使用包含 Popup 位置的显示器工作区域
    Rect workArea = GetMonitorWorkAreaAt(position.x, position.y);
    
    // 确保 Popup 右边不超出屏幕
    if (position.x + popupWidth > workArea.x + workArea.width) {
        position.x = workArea.x + workArea.width - popupWidth;
    }
    
    // 确保 Popup 左边不超出屏幕
    if (position.x < workArea.x) {
        position.x = workArea.x;
    }
    
    // 确保 Popup 底部不超出屏幕
    if (position.y + popupHeight > workArea.y + workArea.height) {
        position.y = workArea.y + workArea.height - popupHeight;
    }
    
    // 确保 Popup 顶部不超出屏幕
    if (position.y < workArea.y) {
        position.y = workArea.y;
    }
    
    return position;
}

Rect Popup::GetScreenWorkArea() {
#ifdef FK_HAS_GLFW
    // 获取主显示器信息
    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    if (!monitor) {
        // 如果获取失败，返回一个默认屏幕大小
        return Rect(0, 0, 1920, 1080);
    }
    
    // 获取显示器工作区域（排除任务栏等）
    int xpos, ypos, width, height;
    glfwGetMonitorWorkarea(monitor, &xpos, &ypos, &width, &height);
    
    return Rect(static_cast<float>(xpos), static_cast<float>(ypos),
                static_cast<float>(width), static_cast<float>(height));
#else
    // 模拟环境：返回默认屏幕大小
    return Rect(0, 0, 1920, 1080);
#endif
}

Rect Popup::GetMonitorWorkAreaAt(float screenX, float screenY) {
#ifdef FK_HAS_GLFW
    // 获取所有显示器
    int monitorCount;
    GLFWmonitor** monitors = glfwGetMonitors(&monitorCount);
    
    if (!monitors || monitorCount == 0) {
        return GetScreenWorkArea(); // 回退到主显示器
    }
    
    // 查找包含指定坐标的显示器
    for (int i = 0; i < monitorCount; ++i) {
        int xpos, ypos, width, height;
        glfwGetMonitorWorkarea(monitors[i], &xpos, &ypos, &width, &height);
        
        // 检查点是否在显示器范围内
        if (screenX >= xpos && screenX < xpos + width &&
            screenY >= ypos && screenY < ypos + height) {
            return Rect(static_cast<float>(xpos), static_cast<float>(ypos),
                       static_cast<float>(width), static_cast<float>(height));
        }
    }
    
    // 如果找不到包含该点的显示器，返回最近的显示器
    float minDistance = std::numeric_limits<float>::max();
    Rect nearestWorkArea;
    
    for (int i = 0; i < monitorCount; ++i) {
        int xpos, ypos, width, height;
        glfwGetMonitorWorkarea(monitors[i], &xpos, &ypos, &width, &height);
        
        // 计算到显示器中心的距离
        float centerX = xpos + width / 2.0f;
        float centerY = ypos + height / 2.0f;
        float distance = std::sqrt((screenX - centerX) * (screenX - centerX) +
                                  (screenY - centerY) * (screenY - centerY));
        
        if (distance < minDistance) {
            minDistance = distance;
            nearestWorkArea = Rect(static_cast<float>(xpos), static_cast<float>(ypos),
                                  static_cast<float>(width), static_cast<float>(height));
        }
    }
    
    return nearestWorkArea;
#else
    return GetScreenWorkArea();
#endif
}

bool Popup::IsOutOfBounds(Point position, float popupWidth, float popupHeight, const Rect& workArea) {
    // 检查任意边是否超出屏幕
    bool outLeft = position.x < workArea.x;
    bool outRight = position.x + popupWidth > workArea.x + workArea.width;
    bool outTop = position.y < workArea.y;
    bool outBottom = position.y + popupHeight > workArea.y + workArea.height;
    
    return outLeft || outRight || outTop || outBottom;
}

Point Popup::CalculateBasePlacement(PlacementMode mode, const Rect& targetBounds, 
                                     float popupWidth, float popupHeight) {
    Point basePos;
    
    switch (mode) {
        case PlacementMode::Relative:
            basePos = Point(targetBounds.x, targetBounds.y);
            break;
            
        case PlacementMode::Bottom:
            basePos = Point(targetBounds.x, targetBounds.y + targetBounds.height);
            break;
            
        case PlacementMode::Top:
            basePos = Point(targetBounds.x, targetBounds.y - popupHeight);
            break;
            
        case PlacementMode::Right:
            basePos = Point(targetBounds.x + targetBounds.width, targetBounds.y);
            break;
            
        case PlacementMode::Left:
            basePos = Point(targetBounds.x - popupWidth, targetBounds.y);
            break;
            
        case PlacementMode::Center:
            basePos = Point(
                targetBounds.x + (targetBounds.width - popupWidth) / 2.0f,
                targetBounds.y + (targetBounds.height - popupHeight) / 2.0f
            );
            break;
            
        case PlacementMode::Mouse:
            basePos = GetMouseScreenPosition();
            break;
            
        default:
            basePos = Point(targetBounds.x, targetBounds.y);
            break;
    }
    
    return basePos;
}

PlacementMode Popup::TryFlipPlacement(PlacementMode mode, const Rect& targetBounds, 
                                       float popupWidth, float popupHeight, const Rect& workArea) {
    // 只对需要翻转的模式进行处理
    if (mode != PlacementMode::Bottom && mode != PlacementMode::Top &&
        mode != PlacementMode::Right && mode != PlacementMode::Left) {
        return mode; // 不翻转 Absolute, Relative, Center, Mouse
    }
    
    // 计算当前模式的位置
    Point currentPos = CalculateBasePlacement(mode, targetBounds, popupWidth, popupHeight);
    
    // 如果当前位置没有超出边界，不需要翻转
    if (!IsOutOfBounds(currentPos, popupWidth, popupHeight, workArea)) {
        return mode;
    }
    
    // 尝试翻转到相反方向
    PlacementMode flippedMode = mode;
    
    switch (mode) {
        case PlacementMode::Bottom:
            flippedMode = PlacementMode::Top;
            break;
            
        case PlacementMode::Top:
            flippedMode = PlacementMode::Bottom;
            break;
            
        case PlacementMode::Right:
            flippedMode = PlacementMode::Left;
            break;
            
        case PlacementMode::Left:
            flippedMode = PlacementMode::Right;
            break;
            
        default:
            return mode;
    }
    
    // 计算翻转后的位置
    Point flippedPos = CalculateBasePlacement(flippedMode, targetBounds, popupWidth, popupHeight);
    
    // 如果翻转后更好（不超出边界或者超出更少），使用翻转后的模式
    if (!IsOutOfBounds(flippedPos, popupWidth, popupHeight, workArea)) {
        return flippedMode;
    }
    
    // 如果两者都超出，计算哪个超出更少
    auto calculateOverflow = [&](Point pos) -> float {
        float overflow = 0.0f;
        if (pos.x < workArea.x) overflow += workArea.x - pos.x;
        if (pos.x + popupWidth > workArea.x + workArea.width) 
            overflow += (pos.x + popupWidth) - (workArea.x + workArea.width);
        if (pos.y < workArea.y) overflow += workArea.y - pos.y;
        if (pos.y + popupHeight > workArea.y + workArea.height) 
            overflow += (pos.y + popupHeight) - (workArea.y + workArea.height);
        return overflow;
    };
    
    float currentOverflow = calculateOverflow(currentPos);
    float flippedOverflow = calculateOverflow(flippedPos);
    
    return (flippedOverflow < currentOverflow) ? flippedMode : mode;
}

Point Popup::GetMouseScreenPosition() {
    // 当前版本：简化实现
    // 由于无法直接访问 Window 的 nativeHandle_，暂时使用 PlacementTarget 位置
    // TODO: Day 6 - 添加全局鼠标位置追踪到 Window 或 InputManager
    
    UIElement* target = GetPlacementTarget();
    if (target) {
        // 使用 target 的边界作为近似位置
        Rect bounds = target->GetBoundsOnScreen();
        return Point(bounds.x, bounds.y);
    }
    
    // 如果没有 target，返回屏幕中心
    Rect workArea = GetScreenWorkArea();
    return Point(workArea.x + workArea.width / 2.0f, 
                 workArea.y + workArea.height / 2.0f);
}

} // namespace fk::ui
