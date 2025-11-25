/**
 * @file Thumb.cpp
 * @brief Thumb 可拖动滑块控件实现
 */

#include "fk/ui/Thumb.h"
#include "fk/animation/VisualStateManager.h"
#include "fk/animation/VisualState.h"
#include "fk/animation/VisualStateGroup.h"
#include "fk/animation/Storyboard.h"
#include "fk/animation/ColorAnimation.h"
#include "fk/ui/Border.h"
#include "fk/ui/Brush.h"

namespace fk::ui {

// ========== 构造函数 ==========

Thumb::Thumb() {
    // 初始化视觉状态
    InitializeVisualStates();
}

// ========== 拖动控制 ==========

void Thumb::CancelDrag() {
    if (!isDragging_) {
        return;
    }
    
    // 释放鼠标捕获
    ReleasePointerCapture();
    
    isDragging_ = false;
    
    // 触发拖动完成事件（标记为取消）
    DragCompletedEventArgs args(totalHorizontalChange_, totalVerticalChange_, true);
    DragCompleted(args);
    
    // 重置状态
    totalHorizontalChange_ = 0.0f;
    totalVerticalChange_ = 0.0f;
    
    // 更新视觉状态
    UpdateVisualState(true);
}

// ========== 鼠标事件处理 ==========

void Thumb::OnPointerPressed(PointerEventArgs& e) {
    Control<Thumb>::OnPointerPressed(e);
    
    // 只响应左键
    if (e.button != MouseButton::Left) {
        return;
    }
    
    // 如果已经在拖动，忽略
    if (isDragging_) {
        return;
    }
    
    // 捕获鼠标
    CapturePointer(e.pointerId);
    
    // 记录起始点（使用本地坐标）
    dragStartPoint_ = e.position;
    lastDragPoint_ = e.position;
    
    // 计算相对于 Thumb 的偏移
    dragStartOffset_ = e.position;
    
    // 重置累计变化量
    totalHorizontalChange_ = 0.0f;
    totalVerticalChange_ = 0.0f;
    
    // 进入拖动状态
    isDragging_ = true;
    
    // 触发拖动开始事件
    DragStartedEventArgs args(dragStartOffset_.x, dragStartOffset_.y);
    DragStarted(args);
    
    // 更新视觉状态
    UpdateVisualState(true);
    
    // 标记事件已处理
    e.handled = true;
}

void Thumb::OnPointerMoved(PointerEventArgs& e) {
    Control<Thumb>::OnPointerMoved(e);
    
    // 如果没有在拖动，忽略
    if (!isDragging_) {
        return;
    }
    
    // 计算增量（使用本地坐标）
    float deltaX = e.position.x - lastDragPoint_.x;
    float deltaY = e.position.y - lastDragPoint_.y;
    
    // 更新上次拖动点
    lastDragPoint_ = e.position;
    
    // 累计变化量
    totalHorizontalChange_ += deltaX;
    totalVerticalChange_ += deltaY;
    
    // 触发拖动增量事件
    DragDeltaEventArgs args(deltaX, deltaY);
    DragDelta(args);
    
    // 标记事件已处理
    e.handled = true;
}

void Thumb::OnPointerReleased(PointerEventArgs& e) {
    Control<Thumb>::OnPointerReleased(e);
    
    // 只响应左键
    if (e.button != MouseButton::Left) {
        return;
    }
    
    // 如果没有在拖动，忽略
    if (!isDragging_) {
        return;
    }
    
    // 释放鼠标捕获
    ReleasePointerCapture(e.pointerId);
    
    isDragging_ = false;
    
    // 触发拖动完成事件
    DragCompletedEventArgs args(totalHorizontalChange_, totalVerticalChange_, false);
    DragCompleted(args);
    
    // 重置状态
    totalHorizontalChange_ = 0.0f;
    totalVerticalChange_ = 0.0f;
    
    // 更新视觉状态
    UpdateVisualState(true);
    
    // 标记事件已处理
    e.handled = true;
}

// ========== 视觉状态 ==========

void Thumb::UpdateVisualState(bool useTransitions) {
    // 确定当前状态名称
    std::string stateName;
    
    if (!GetIsEnabled()) {
        stateName = "Disabled";
    } else if (isDragging_) {
        stateName = "Pressed";
    } else if (IsMouseOver()) {
        stateName = "MouseOver";
    } else {
        stateName = "Normal";
    }
    
    // 切换到对应的视觉状态
    animation::VisualStateManager::GoToState(this, stateName, useTransitions);
}

void Thumb::InitializeVisualStates() {
    // 创建 CommonStates 视觉状态组
    auto commonStates = std::make_shared<animation::VisualStateGroup>("CommonStates");
    
    // Normal 状态
    auto normalState = std::make_shared<animation::VisualState>("Normal");
    commonStates->AddState(normalState);
    
    // MouseOver 状态
    auto mouseOverState = std::make_shared<animation::VisualState>("MouseOver");
    commonStates->AddState(mouseOverState);
    
    // Pressed 状态（拖动中）
    auto pressedState = std::make_shared<animation::VisualState>("Pressed");
    commonStates->AddState(pressedState);
    
    // Disabled 状态
    auto disabledState = std::make_shared<animation::VisualState>("Disabled");
    commonStates->AddState(disabledState);
    
    // 注册视觉状态组
    auto manager = std::make_shared<animation::VisualStateManager>();
    animation::VisualStateManager::SetVisualStateManager(this, manager);
    manager->AddStateGroup(commonStates);
}

void Thumb::OnTemplateApplied() {
    Control<Thumb>::OnTemplateApplied();
    
    // 更新到初始视觉状态
    UpdateVisualState(false);
}

} // namespace fk::ui
