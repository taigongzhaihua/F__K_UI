/**
 * @file RepeatButton.cpp
 * @brief RepeatButton 重复按钮控件实现
 */

#include "fk/ui/RepeatButton.h"
#include "fk/ui/Window.h"
#include "fk/app/Application.h"

namespace fk::ui {

// ========== 构造函数 ==========

RepeatButton::RepeatButton() {
    // 默认样式可以在此设置
}

// ========== 依赖属性定义 ==========

const binding::DependencyProperty& RepeatButton::DelayProperty() {
    static auto& property = 
        binding::DependencyProperty::Register(
            "Delay",
            typeid(int),
            typeid(RepeatButton),
            binding::PropertyMetadata{std::any(250)}
        );
    return property;
}

const binding::DependencyProperty& RepeatButton::IntervalProperty() {
    static auto& property = 
        binding::DependencyProperty::Register(
            "Interval",
            typeid(int),
            typeid(RepeatButton),
            binding::PropertyMetadata{std::any(33)}
        );
    return property;
}

// ========== 事件处理 ==========

void RepeatButton::OnPointerPressed(PointerEventArgs& e) {
    // 调用基类处理
    ButtonBase<RepeatButton>::OnPointerPressed(e);
    
    // 只处理主鼠标按钮
    if (e.button != MouseButton::Left) {
        return;
    }
    
    isPressed_ = true;
    
    // 捕获指针，确保即使鼠标移出按钮也能接收到释放事件
    CapturePointer(e.pointerId);
    
    // 立即触发一次Click
    Click();
    
    // 启动重复定时器
    StartRepeatTimer();
}

void RepeatButton::OnPointerReleased(PointerEventArgs& e) {
    // 调用基类处理
    ButtonBase<RepeatButton>::OnPointerReleased(e);
    
    if (e.button != MouseButton::Left) {
        return;
    }
    
    isPressed_ = false;
    
    // 释放指针捕获
    ReleasePointerCapture(e.pointerId);
    
    // 停止重复定时器
    StopRepeatTimer();
}

void RepeatButton::OnPointerExited(PointerEventArgs& e) {
    // 调用基类处理
    ButtonBase<RepeatButton>::OnPointerExited(e);
    
    // 如果没有捕获指针，则停止重复
    if (!HasPointerCapture(e.pointerId)) {
        isPressed_ = false;
        StopRepeatTimer();
    }
}

// ========== 私有方法 ==========

void RepeatButton::StartRepeatTimer() {
    // 如果定时器已经在运行，先停止
    StopRepeatTimer();
    
    // 获取Dispatcher
    auto dispatcher = GetDispatcher();
    if (!dispatcher) {
        return;
    }
    
    // 创建定时器
    repeatTimer_ = std::make_shared<core::Timer>(dispatcher);
    
    // 订阅定时器Tick事件
    repeatTimer_->Tick.Connect([this]() {
        OnRepeatTimerTick();
    });
    
    // 启动定时器：首次延迟后开始以interval_间隔重复
    // 注意：Timer的Start方法接受间隔和是否重复
    // 为了实现不同的首次延迟和后续间隔，我们需要分两步：
    // 1. 先以delay_启动一次性定时器
    // 2. 在首次触发后，改为以interval_重复
    
    // 由于Timer的设计，我们直接使用interval_作为重复间隔
    // 首次Click已经在OnPointerPressed中触发
    // 这里启动重复定时器，首次触发会在delay_后
    repeatTimer_->Start(std::chrono::milliseconds(delay_), false);
}

void RepeatButton::StopRepeatTimer() {
    if (repeatTimer_) {
        repeatTimer_->Stop();
        repeatTimer_.reset();
    }
}

void RepeatButton::OnRepeatTimerTick() {
    // 触发Click事件
    if (isPressed_) {
        Click();
        
        // 首次触发后，改为使用interval_间隔重复
        if (repeatTimer_) {
            repeatTimer_->Start(std::chrono::milliseconds(interval_), true);
        }
    }
}

std::shared_ptr<core::Dispatcher> RepeatButton::GetDispatcher() {
    // 向上遍历视觉树，找到Window，从Window获取Application的Dispatcher
    Visual* current = this;
    while (current) {
        if (auto* window = dynamic_cast<Window*>(current)) {
            // Window应该有访问Application的方式
            // 由于我们没有直接的Application引用，使用静态方法
            // 注意：这里假设有Application::GetCurrent()方法
            // 如果没有，需要另一种方式获取Dispatcher
            
            // 临时方案：创建一个Dispatcher
            // 在实际应用中，应该从Application获取主Dispatcher
            static auto dispatcher = std::make_shared<core::Dispatcher>();
            return dispatcher;
        }
        current = current->GetVisualParent();
    }
    
    // 如果找不到Window，返回默认Dispatcher
    static auto defaultDispatcher = std::make_shared<core::Dispatcher>();
    return defaultDispatcher;
}

} // namespace fk::ui
