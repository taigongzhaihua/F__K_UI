/**
 * @file RepeatButton.h
 * @brief RepeatButton 重复按钮控件
 * 
 * 设计理念：
 * 1. 继承自 ButtonBase，复用基本按钮功能
 * 2. 按住时重复触发 Click 事件
 * 3. 可配置首次延迟（Delay）和重复间隔（Interval）
 * 4. 用于滚动条的 LineUp/LineDown 按钮等场景
 * 
 * WPF 对应：System.Windows.Controls.Primitives.RepeatButton
 * 
 * 使用场景：
 * - ScrollBar 两端的箭头按钮
 * - 需要连续操作的场景（如数值调整器）
 */

#pragma once

#include "fk/ui/ButtonBase.h"
#include <memory>
#include <atomic>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <functional>

namespace fk::ui {

/**
 * @brief 重复按钮控件
 * 
 * 按住按钮时会重复触发 Click 事件，释放后停止。
 * 首次触发有延迟（Delay），后续按固定间隔（Interval）触发。
 * 
 * 使用示例：
 * @code
 * auto* button = new RepeatButton();
 * button->SetDelay(400);      // 400ms 后开始重复
 * button->SetInterval(50);    // 每 50ms 触发一次
 * button->Click += []() {
 *     // 执行滚动操作
 * };
 * @endcode
 */
class RepeatButton : public ButtonBase<RepeatButton> {
public:
    RepeatButton();
    virtual ~RepeatButton();
    
    // ========== 依赖属性 ==========
    
    /// Delay 属性（首次重复前的延迟，毫秒）
    static const binding::DependencyProperty& DelayProperty();
    
    /// Interval 属性（重复触发的间隔，毫秒）
    static const binding::DependencyProperty& IntervalProperty();
    
    // ========== 属性访问器 ==========
    
    /// 获取首次延迟时间（毫秒）
    int GetDelay() const;
    
    /// 设置首次延迟时间（毫秒）
    RepeatButton* SetDelay(int milliseconds);
    
    /// 链式调用
    RepeatButton* Delay(int milliseconds) { return SetDelay(milliseconds); }
    
    /// 绑定支持
    RepeatButton* Delay(binding::Binding binding) {
        SetBinding(DelayProperty(), std::move(binding));
        return this;
    }
    
    /// 获取重复间隔时间（毫秒）
    int GetInterval() const;
    
    /// 设置重复间隔时间（毫秒）
    RepeatButton* SetInterval(int milliseconds);
    
    /// 链式调用
    RepeatButton* Interval(int milliseconds) { return SetInterval(milliseconds); }
    
    /// 绑定支持
    RepeatButton* Interval(binding::Binding binding) {
        SetBinding(IntervalProperty(), std::move(binding));
        return this;
    }

protected:
    // ========== 事件处理重写 ==========
    
    void OnPointerPressed(PointerEventArgs& e) override;
    void OnPointerReleased(PointerEventArgs& e) override;
    void OnPointerExited(PointerEventArgs& e) override;
    
    /// 重写点击处理（由定时器调用触发 Click 事件）
    void OnClick() override;

private:
    // ========== 重复逻辑 ==========
    
    /// 启动重复计时
    void StartRepeat();
    
    /// 停止重复计时
    void StopRepeat();
    
    // ========== 定时器 ==========
    
    std::unique_ptr<std::thread> repeatThread_;  ///< 重复触发线程
    std::mutex repeatMutex_;                      ///< 保护定时器状态
    std::condition_variable repeatCv_;            ///< 用于停止定时器
    
    // ========== 状态 ==========
    
    std::atomic<bool> isRepeating_{false};        ///< 是否正在重复触发中
    std::atomic<bool> shouldStop_{false};         ///< 是否应该停止（用于线程同步）
};

} // namespace fk::ui
