/**
 * @file Popup.cpp
 * @brief Popup 控件实现
 */

#include "fk/ui/controls/Popup.h"
#include "fk/ui/window/PopupRoot.h"
#include "fk/ui/PopupService.h"
#include "fk/ui/Window.h"
#include <iostream>

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
    // 如果已经打开，不重复操作
    if (isPopupRootCreated_ && popupRoot_) {
        return;
    }

    // 创建 PopupRoot（如果还没有）
    if (!popupRoot_) {
        popupRoot_ = std::make_unique<PopupRoot>();
        popupRoot_->Initialize();
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

    // 隐藏窗口
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
    // 当前版本：简单实现
    // 未来可以添加：
    // - 检测 PlacementTarget 位置变化并更新 Popup 位置
    // - 处理 StaysOpen=false 时的外部点击检测
}

// ========== 定位计算 ==========

Point Popup::CalculateScreenPosition() {
    PlacementMode mode = GetPlacement();
    UIElement* target = GetPlacementTarget();
    float hOffset = GetHorizontalOffset();
    float vOffset = GetVerticalOffset();

    // 模式 1: Absolute - 直接使用偏移作为屏幕坐标
    if (mode == PlacementMode::Absolute) {
        return Point(hOffset, vOffset);
    }

    // 如果没有 PlacementTarget，退回到 Absolute 模式
    if (!target) {
        std::cerr << "Warning: No PlacementTarget set, using Absolute mode" << std::endl;
        return Point(hOffset, vOffset);
    }

    // 获取 Target 在屏幕上的边界
    Rect targetBounds = target->GetBoundsOnScreen();

    // 根据 PlacementMode 计算基础位置
    Point basePos;

    switch (mode) {
        case PlacementMode::Relative:
            // 相对于 Target 左上角
            basePos = Point(targetBounds.x, targetBounds.y);
            break;

        case PlacementMode::Bottom:
            // Target 左下角（常用于下拉菜单）
            basePos = Point(targetBounds.x, targetBounds.y + targetBounds.height);
            break;

        case PlacementMode::Top:
            // Target 左上角上方
            basePos = Point(targetBounds.x, targetBounds.y - GetHeight());
            break;

        case PlacementMode::Right:
            // Target 右上角
            basePos = Point(targetBounds.x + targetBounds.width, targetBounds.y);
            break;

        case PlacementMode::Left:
            // Target 左上角左侧
            basePos = Point(targetBounds.x - GetWidth(), targetBounds.y);
            break;

        case PlacementMode::Center:
            // Target 中心
            basePos = Point(
                targetBounds.x + (targetBounds.width - GetWidth()) / 2.0f,
                targetBounds.y + (targetBounds.height - GetHeight()) / 2.0f
            );
            break;

        case PlacementMode::Mouse:
            // TODO: 获取鼠标位置（需要输入管理器支持）
            // 暂时退回到 Relative 模式
            basePos = Point(targetBounds.x, targetBounds.y);
            break;

        default:
            basePos = Point(targetBounds.x, targetBounds.y);
            break;
    }

    // 应用偏移
    Point finalPos(basePos.x + hOffset, basePos.y + vOffset);

    // TODO: Day 5 - 边界检测，确保不超出屏幕
    // 当前简单实现：直接返回计算的位置

    return finalPos;
}

} // namespace fk::ui
