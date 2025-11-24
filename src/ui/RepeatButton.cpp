/**
 * @file RepeatButton.cpp
 * @brief RepeatButton 重复按钮实现 - Phase 3 Step 1
 */

#include "fk/ui/RepeatButton.h"

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

void RepeatButton::StartRepeating() {
    // TODO: Phase 3 - 实现定时器逻辑
    // 
    // 理想的实现应该：
    // 1. 等待 delay_ 毫秒
    // 2. 然后每隔 interval_ 毫秒调用 OnRepeat()
    // 3. 直到调用 StopRepeating()
    //
    // 可能的实现方式：
    // - 使用框架的 DispatcherTimer（如果有）
    // - 使用渲染循环的 Update 回调
    // - 使用 std::thread + std::this_thread::sleep_for（需要注意线程安全）
    //
    // 目前作为 stub 实现，等待框架提供定时器支持
}

void RepeatButton::StopRepeating() {
    // TODO: Phase 3 - 停止定时器
}

void RepeatButton::OnRepeat() {
    // 检查是否仍然按下
    if (isPressed_) {
        // 触发 Click 事件
        Click();
    }
}

} // namespace fk::ui
