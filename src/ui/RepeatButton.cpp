/**
 * @file RepeatButton.cpp
 * @brief RepeatButton 重复按钮实现 - Phase 3 Step 1
 */

#include "fk/ui/RepeatButton.h"
#include "fk/app/Application.h"
#include "fk/ui/Window.h"

namespace fk::ui {

// ========== 构造函数 ==========

RepeatButton::RepeatButton() {
    // 设置默认值
    delay_ = 500;     // 500ms 延迟
    interval_ = 33;   // 33ms 间隔 (约 30fps)
    isPressed_ = false;
}

RepeatButton::~RepeatButton() {
    // 确保停止重复
    StopRepeating();
}

// ========== 依赖属性定义 ==========

const binding::DependencyProperty& RepeatButton::DelayProperty() {
    static auto& property = 
        binding::DependencyProperty::Register(
            "Delay",
            typeid(int),
            typeid(RepeatButton),
            binding::PropertyMetadata{std::any(500)}
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

// ========== 事件重写 ==========

void RepeatButton::OnPointerPressed(PointerEventArgs& e) {
    // 调用基类处理
    ButtonBase<RepeatButton>::OnPointerPressed(e);
    
    // 标记为按下状态
    isPressed_ = true;
    
    // 立即触发第一次点击
    Click();
    
    // 开始重复触发
    StartRepeating();
}

void RepeatButton::OnPointerReleased(PointerEventArgs& e) {
    // 调用基类处理
    ButtonBase<RepeatButton>::OnPointerReleased(e);
    
    // 停止重复
    isPressed_ = false;
    StopRepeating();
}

void RepeatButton::OnPointerExited(PointerEventArgs& e) {
    // 调用基类处理
    ButtonBase<RepeatButton>::OnPointerExited(e);
    
    // 鼠标离开时也要停止重复
    isPressed_ = false;
    StopRepeating();
}

// ========== 内部方法 ==========

std::shared_ptr<core::Dispatcher> RepeatButton::GetDispatcher() {
    // TODO: 理想情况下应该从元素所属的窗口获取 Dispatcher
    // 由于 Window 类当前没有暴露 Dispatcher，
    // 这里创建一个临时的 Dispatcher 用于定时器
    // 
    // 限制：这个 Dispatcher 需要有人调用 Run() 才能工作
    // 更好的方案是框架提供全局或窗口级别的 Dispatcher
    
    // 为简化实现，返回 nullptr，定时器功能暂时不可用
    // 用户可以通过其他方式（如渲染循环）实现重复触发
    return nullptr;
    
    // 备选实现（需要框架支持）：
    // auto* app = Application::Current();
    // if (app) {
    //     auto mainWindow = app->GetMainWindow();
    //     if (mainWindow) {
    //         return mainWindow->GetDispatcher(); // Window 需要暴露此方法
    //     }
    // }
    // return nullptr;
}

void RepeatButton::StartRepeating() {
    // 停止现有的定时器
    StopRepeating();
    
    auto dispatcher = GetDispatcher();
    if (!dispatcher) {
        // 无法获取 Dispatcher，定时器功能不可用
        // TODO: 当框架提供窗口级别的 Dispatcher 访问后，
        // 可以实现完整的重复触发功能
        // 
        // 临时方案：
        // 1. 首次点击已经在 OnPointerPressed 中触发
        // 2. 后续的重复触发需要框架支持或通过其他机制实现
        //    （如渲染循环的更新回调）
        return;
    }
    
    // 创建延迟定时器（delay_ 毫秒后触发一次）
    delayTimer_ = std::make_shared<core::Timer>(dispatcher);
    delayTimer_->Tick.Connect([this]() {
        OnDelayComplete();
    });
    delayTimer_->Start(std::chrono::milliseconds(delay_), false);
}

void RepeatButton::StopRepeating() {
    // 停止延迟定时器
    if (delayTimer_) {
        delayTimer_->Stop();
        delayTimer_.reset();
    }
    
    // 停止间隔定时器
    if (intervalTimer_) {
        intervalTimer_->Stop();
        intervalTimer_.reset();
    }
}

void RepeatButton::OnDelayComplete() {
    // 延迟结束，如果仍然按下，开始间隔触发
    if (!isPressed_) {
        return;
    }
    
    auto dispatcher = GetDispatcher();
    if (!dispatcher) {
        return;
    }
    
    // 创建间隔定时器（每 interval_ 毫秒触发一次）
    intervalTimer_ = std::make_shared<core::Timer>(dispatcher);
    intervalTimer_->Tick.Connect([this]() {
        OnRepeat();
    });
    intervalTimer_->Start(std::chrono::milliseconds(interval_), true);
}

void RepeatButton::OnRepeat() {
    // 检查是否仍然按下
    if (isPressed_) {
        // 触发 Click 事件
        Click();
    } else {
        // 如果不再按下，停止定时器
        StopRepeating();
    }
}

} // namespace fk::ui
