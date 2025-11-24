/**
 * @file RepeatButton.h
 * @brief RepeatButton 重复按钮 - Phase 3 Step 1
 * 
 * 设计理念：
 * 1. 继承自 ButtonBase，复用基本按钮功能
 * 2. 按住时重复触发 Click 事件
 * 3. 可配置首次延迟和重复间隔
 * 4. 用于滚动条的 LineUp/LineDown 按钮
 * 
 * WPF 对应：System.Windows.Controls.Primitives.RepeatButton
 * 
 * 使用场景：
 * - ScrollBar 两端的箭头按钮
 * - 需要连续操作的场景（如数值调整器）
 */

#pragma once

#include "fk/ui/ButtonBase.h"
#include "fk/binding/DependencyProperty.h"
#include <chrono>

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
 * button->SetDelay(500);      // 500ms 后开始重复
 * button->SetInterval(33);    // 每 33ms 触发一次
 * button->Click.Connect([]() {
 *     // 执行滚动操作
 * });
 * @endcode
 */
class RepeatButton : public ButtonBase<RepeatButton> {
public:
    RepeatButton();
    virtual ~RepeatButton();
    
    // ========== 依赖属性 ==========
    
    /// Delay 属性（首次重复前的延迟，毫秒）
    static const binding::DependencyProperty& DelayProperty();
    
    /// Interval 属性（重复间隔，毫秒）
    static const binding::DependencyProperty& IntervalProperty();
    
    // ========== 属性访问器 ==========
    
    /// 获取首次重复延迟（毫秒）
    int GetDelay() const { return delay_; }
    
    /// 设置首次重复延迟（毫秒）
    RepeatButton* SetDelay(int milliseconds) {
        delay_ = milliseconds;
        return this;
    }
    
    /// 获取重复间隔（毫秒）
    int GetInterval() const { return interval_; }
    
    /// 设置重复间隔（毫秒）
    RepeatButton* SetInterval(int milliseconds) {
        interval_ = milliseconds;
        return this;
    }
    
protected:
    // ========== 事件重写 ==========
    
    void OnPointerPressed(PointerEventArgs& e) override;
    void OnPointerReleased(PointerEventArgs& e) override;
    void OnPointerExited(PointerEventArgs& e) override;
    
private:
    // ========== 内部状态 ==========
    
    int delay_{500};         ///< 首次重复延迟（毫秒）
    int interval_{33};       ///< 重复间隔（毫秒，约 30fps）
    
    bool isPressed_{false};  ///< 是否处于按下状态
    
    // TODO: 实现定时器逻辑
    // 由于目前框架可能没有完整的定时器系统，
    // 这里先提供接口定义，具体实现可能需要：
    // 1. 使用框架的定时器（如果有）
    // 2. 使用渲染循环的更新回调
    // 3. 使用独立的线程（不推荐，可能有线程安全问题）
    
    /// 开始重复触发
    void StartRepeating();
    
    /// 停止重复触发
    void StopRepeating();
    
    /// 触发一次 Click（由定时器调用）
    void OnRepeat();
};

} // namespace fk::ui
