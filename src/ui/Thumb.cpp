/**
 * @file Thumb.cpp
 * @brief Thumb 可拖动滑块实现 - Phase 3 Step 2
 */

#include "fk/ui/Thumb.h"

namespace fk::ui {

// ========== 构造函数 ==========

Thumb::Thumb() {
    isDragging_ = false;
    dragStartX_ = 0.0f;
    dragStartY_ = 0.0f;
    lastX_ = 0.0f;
    lastY_ = 0.0f;
}

// ========== 公共方法 ==========

void Thumb::CancelDrag() {
    if (isDragging_) {
        isDragging_ = false;
        // 不触发 DragCompleted，因为是取消
        
        // TODO: 添加指针捕获释放
        // 需要从 UIElement 访问 Window 的 InputManager
        // window->GetInputManager()->ReleasePointerCapture(pointerId);
    }
}

// ========== 事件重写 ==========

void Thumb::OnPointerPressed(PointerEventArgs& e) {
    Control<Thumb>::OnPointerPressed(e);
    
    // TODO: 捕获指针以确保后续的移动和释放事件都能收到
    // 需要从 UIElement 访问 Window 的 InputManager
    // 理想实现：window->GetInputManager()->CapturePointer(this, e.pointerId);
    // 
    // 当前限制：框架需要提供从 UIElement 获取 Window 的方法
    // 例如：GetWindow() 或通过可视树向上查找
    
    // 开始拖动
    StartDrag(e.position.x, e.position.y);
    e.handled = true;
}

void Thumb::OnPointerMoved(PointerEventArgs& e) {
    Control<Thumb>::OnPointerMoved(e);
    
    if (isDragging_) {
        // 更新拖动
        UpdateDrag(e.position.x, e.position.y);
        e.handled = true;
    }
}

void Thumb::OnPointerReleased(PointerEventArgs& e) {
    Control<Thumb>::OnPointerReleased(e);
    
    if (isDragging_) {
        // 结束拖动
        EndDrag();
        
        // TODO: 释放指针捕获
        // window->GetInputManager()->ReleasePointerCapture(e.pointerId);
        
        e.handled = true;
    }
}

void Thumb::OnPointerExited(PointerEventArgs& e) {
    Control<Thumb>::OnPointerExited(e);
    
    // 注意：在指针被捕获时，鼠标移出元素边界不应该结束拖动
    // 当前由于未实现指针捕获，这里会导致拖动被终止
    // 
    // 理想行为：
    // - 如果有指针捕获，OnPointerExited 不会被调用（或被忽略）
    // - 只有 OnPointerReleased 或 OnPointerCaptureLost 才应结束拖动
    //
    // 临时方案：保持拖动直到释放，但这可能导致鼠标移出后无法停止
    // if (isDragging_) {
    //     EndDrag();
    // }
}

// ========== 内部方法 ==========

void Thumb::StartDrag(float x, float y) {
    isDragging_ = true;
    dragStartX_ = x;
    dragStartY_ = y;
    lastX_ = x;
    lastY_ = y;
    
    // 触发 DragStarted 事件
    DragStarted();
}

void Thumb::UpdateDrag(float x, float y) {
    if (!isDragging_) {
        return;
    }
    
    // 计算相对于上次位置的偏移
    float deltaX = x - lastX_;
    float deltaY = y - lastY_;
    
    // 更新上次位置
    lastX_ = x;
    lastY_ = y;
    
    // 触发 DragDelta 事件
    DragDeltaEventArgs args;
    args.horizontalChange = deltaX;
    args.verticalChange = deltaY;
    DragDelta(args);
}

void Thumb::EndDrag() {
    if (!isDragging_) {
        return;
    }
    
    isDragging_ = false;
    
    // 触发 DragCompleted 事件
    DragCompleted();
}

} // namespace fk::ui
