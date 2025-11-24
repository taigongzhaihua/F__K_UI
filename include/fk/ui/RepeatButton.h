/**
 * @file RepeatButton.h
 * @brief RepeatButton 重复按钮控件
 * 
 * 职责：
 * - 提供按下时持续触发Click事件的按钮
 * - 支持初始延迟和重复间隔配置
 * - 主要用于滚动条、数字调整器等需要连续操作的场景
 * 
 * WPF 对应：RepeatButton
 */

#pragma once

#include "fk/ui/ButtonBase.h"
#include "fk/core/Timer.h"
#include "fk/core/Dispatcher.h"
#include <memory>

namespace fk::ui {

/**
 * @brief 重复按钮控件
 * 
 * RepeatButton 在按下时会持续触发 Click 事件，直到释放为止。
 * 常用于滚动条的增减按钮、数字输入框的上下调节按钮等场景。
 */
class RepeatButton : public ButtonBase<RepeatButton> {
public:
    RepeatButton();
    virtual ~RepeatButton() = default;
    
    // ========== 依赖属性 ==========
    
    /// Delay 属性：按下后首次重复触发前的延迟（毫秒）
    static const binding::DependencyProperty& DelayProperty();
    
    /// Interval 属性：重复触发的间隔（毫秒）
    static const binding::DependencyProperty& IntervalProperty();
    
    // ========== 属性访问 ==========
    
    /**
     * @brief 获取延迟时间（毫秒）
     * @return 首次重复前的延迟时间
     */
    int GetDelay() const { return delay_; }
    
    /**
     * @brief 设置延迟时间（毫秒）
     * @param value 首次重复前的延迟时间，默认250ms
     * @return 返回this以支持链式调用
     */
    RepeatButton* SetDelay(int value) {
        delay_ = value;
        return this;
    }
    
    /**
     * @brief 获取重复间隔（毫秒）
     * @return 重复触发的间隔
     */
    int GetInterval() const { return interval_; }
    
    /**
     * @brief 设置重复间隔（毫秒）
     * @param value 重复触发的间隔，默认33ms（约30Hz）
     * @return 返回this以支持链式调用
     */
    RepeatButton* SetInterval(int value) {
        interval_ = value;
        return this;
    }
    
    /**
     * @brief 流式API：设置延迟时间
     */
    RepeatButton* Delay(int value) {
        return SetDelay(value);
    }
    
    /**
     * @brief 流式API：设置重复间隔
     */
    RepeatButton* Interval(int value) {
        return SetInterval(value);
    }

protected:
    // ========== 重写事件处理 ==========
    
    /**
     * @brief 处理指针按下事件
     */
    void OnPointerPressed(PointerEventArgs& e) override;
    
    /**
     * @brief 处理指针释放事件
     */
    void OnPointerReleased(PointerEventArgs& e) override;
    
    /**
     * @brief 处理指针离开事件
     */
    void OnPointerExited(PointerEventArgs& e) override;

private:
    int delay_{250};        // 首次重复前的延迟（毫秒）
    int interval_{33};      // 重复间隔（毫秒），约30Hz
    
    std::shared_ptr<core::Timer> repeatTimer_;  // 重复定时器
    bool isPressed_{false};  // 是否处于按下状态
    
    /**
     * @brief 启动重复定时器
     */
    void StartRepeatTimer();
    
    /**
     * @brief 停止重复定时器
     */
    void StopRepeatTimer();
    
    /**
     * @brief 定时器回调：触发Click事件
     */
    void OnRepeatTimerTick();
    
    /**
     * @brief 获取Dispatcher（用于创建Timer）
     */
    std::shared_ptr<core::Dispatcher> GetDispatcher();
};

} // namespace fk::ui
