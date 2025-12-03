/**
 * @file Thumb.h
 * @brief Thumb 可拖动滑块控件
 * 
 * 设计理念：
 * 1. 继承自 Control，提供基本的控件功能
 * 2. 支持鼠标拖动交互
 * 3. 触发拖动事件（DragStarted、DragDelta、DragCompleted）
 * 4. 不直接修改位置，由父控件（Track）响应事件并更新布局
 * 
 * WPF 对应：System.Windows.Controls.Primitives.Thumb
 * 
 * 使用场景：
 * - ScrollBar 的滑块
 * - Slider 的滑块
 * - GridSplitter 等需要拖动的场景
 */

#pragma once

#include "fk/ui/controls/Control.h"
#include "fk/core/Event.h"

namespace fk::ui {

/**
 * @brief 拖动增量事件参数
 */
struct DragDeltaEventArgs {
    float horizontalChange{0.0f};  ///< 水平方向的变化量（像素）
    float verticalChange{0.0f};    ///< 垂直方向的变化量（像素）
    
    DragDeltaEventArgs() = default;
    DragDeltaEventArgs(float h, float v) : horizontalChange(h), verticalChange(v) {}
};

/**
 * @brief 拖动开始事件参数
 */
struct DragStartedEventArgs {
    float horizontalOffset{0.0f};  ///< 拖动起始点相对于 Thumb 左边缘的水平偏移
    float verticalOffset{0.0f};    ///< 拖动起始点相对于 Thumb 上边缘的垂直偏移
    
    DragStartedEventArgs() = default;
    DragStartedEventArgs(float h, float v) : horizontalOffset(h), verticalOffset(v) {}
};

/**
 * @brief 拖动完成事件参数
 */
struct DragCompletedEventArgs {
    float horizontalChange{0.0f};  ///< 从拖动开始到结束的总水平变化量
    float verticalChange{0.0f};    ///< 从拖动开始到结束的总垂直变化量
    bool canceled{false};          ///< 拖动是否被取消（如按 Escape 键）
    
    DragCompletedEventArgs() = default;
    DragCompletedEventArgs(float h, float v, bool c = false) 
        : horizontalChange(h), verticalChange(v), canceled(c) {}
};

/**
 * @brief 可拖动的滑块控件
 * 
 * Thumb 提供拖动交互，但不负责自己的位置更新。
 * 父控件（如 Track）应该监听 DragDelta 事件并更新 Thumb 的位置。
 * 
 * 职责分离：
 * - Thumb：检测拖动，报告偏移量
 * - Track/Slider：根据偏移量计算新值，更新位置
 * 
 * 使用示例：
 * @code
 * auto* thumb = new Thumb();
 * thumb->Width(20)->Height(40);
 * thumb->Background(Color::Gray);
 * 
 * thumb->DragStarted += [](const DragStartedEventArgs& e) {
 *     std::cout << "开始拖动" << std::endl;
 * };
 * 
 * thumb->DragDelta += [](const DragDeltaEventArgs& e) {
 *     // 父控件根据偏移量计算新值
 *     float newValue = currentValue + e.verticalChange * ratio;
 * };
 * 
 * thumb->DragCompleted += [](const DragCompletedEventArgs& e) {
 *     if (!e.canceled) {
 *         std::cout << "拖动完成" << std::endl;
 *     }
 * };
 * @endcode
 */
class Thumb : public Control<Thumb> {
public:
    Thumb();
    virtual ~Thumb() = default;
    
    // ========== 事件 ==========
    
    /// 拖动开始事件
    core::Event<const DragStartedEventArgs&> DragStarted;
    
    /// 拖动增量事件（拖动过程中持续触发）
    core::Event<const DragDeltaEventArgs&> DragDelta;
    
    /// 拖动完成事件
    core::Event<const DragCompletedEventArgs&> DragCompleted;
    
    // ========== 状态查询 ==========
    
    /// 是否正在拖动
    bool IsDragging() const { return isDragging_; }
    
    // ========== 拖动控制 ==========
    
    /// 取消当前拖动操作
    void CancelDrag();

protected:
    // ========== 鼠标事件处理 ==========
    
    void OnPointerPressed(PointerEventArgs& e) override;
    void OnPointerMoved(PointerEventArgs& e) override;
    void OnPointerReleased(PointerEventArgs& e) override;
    
    // ========== 视觉状态 ==========
    
    /// 更新视觉状态
    virtual void UpdateVisualState(bool useTransitions);
    
    /// 初始化视觉状态
    void InitializeVisualStates();
    
    /// 模板应用后的处理
    void OnTemplateApplied() override;

private:
    // ========== 内部状态 ==========
    
    bool isDragging_{false};           ///< 是否正在拖动
    Point dragStartPoint_;             ///< 拖动起始点（屏幕坐标）
    Point lastDragPoint_;              ///< 上次拖动点（用于计算 delta）
    Point dragStartOffset_;            ///< 拖动起始点相对于 Thumb 的偏移
    float totalHorizontalChange_{0.0f}; ///< 累计水平变化量
    float totalVerticalChange_{0.0f};   ///< 累计垂直变化量
};

} // namespace fk::ui
