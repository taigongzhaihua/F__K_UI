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

RepeatButton::~RepeatButton() {
    // 确保在析构时停止定时器，避免悬空指针问题
    StopRepeatTimer();
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
    
    // 重置首次触发标志
    isFirstTick_ = true;
    
    // 订阅定时器Tick事件
    repeatTimer_->Tick.Connect([this]() {
        OnRepeatTimerTick();
    });
    
    // 启动定时器：首次以delay_延迟触发（不重复）
    // 在首次触发后，会在OnRepeatTimerTick中改为以interval_重复
    repeatTimer_->Start(std::chrono::milliseconds(delay_), false);
}

void RepeatButton::StopRepeatTimer() {
    if (repeatTimer_) {
        repeatTimer_->Stop();
        repeatTimer_.reset();
    }
    // 注意：isFirstTick_会在StartRepeatTimer中重置，这里不需要重复设置
}

void RepeatButton::OnRepeatTimerTick() {
    // 只在按下状态时触发
    if (!isPressed_) {
        return;
    }
    
    // 触发Click事件
    Click();
    
    // 如果是首次触发，改为使用interval_间隔重复
    if (isFirstTick_ && repeatTimer_) {
        isFirstTick_ = false;
        repeatTimer_->Start(std::chrono::milliseconds(interval_), true);
    }
}

std::shared_ptr<core::Dispatcher> RepeatButton::GetDispatcher() {
    // 使用全局单例Dispatcher
    // 注意：所有UI操作都应该在主线程/UI线程上执行
    // 这个单例Dispatcher用于所有UI定时器和异步操作
    static auto globalDispatcher = std::make_shared<core::Dispatcher>();
    return globalDispatcher;
}

} // namespace fk::ui
