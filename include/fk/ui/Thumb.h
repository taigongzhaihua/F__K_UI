/**
 * @file Thumb.h
 * @brief Thumb 可拖动滑块 - Phase 3 Step 2
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

#include "fk/ui/Control.h"
#include "fk/core/Event.h"

namespace fk::ui {

/**
 * @brief 拖动事件参数
 */
struct DragDeltaEventArgs {
    float horizontalChange;  ///< 水平方向的变化量
    float verticalChange;    ///< 垂直方向的变化量
};

/**
 * @brief 可拖动的滑块控件
 * 
 * Thumb 提供拖动交互，但不负责自己的位置更新。
 * 父控件（如 Track）应该监听 DragDelta 事件并更新 Thumb 的位置。
 * 
 * 使用示例：
 * @code
 * auto* thumb = new Thumb();
 * 
 * thumb->DragStarted.Connect([]() {
 *     // 开始拖动
 * });
 * 
 * thumb->DragDelta.Connect([](DragDeltaEventArgs args) {
 *     // 处理拖动偏移
 *     float newValue = CalculateValueFromDelta(args.verticalChange);
 *     scrollBar->SetValue(newValue);
 * });
 * 
 * thumb->DragCompleted.Connect([]() {
 *     // 完成拖动
 * });
 * @endcode
 */
class Thumb : public Control<Thumb> {
public:
    Thumb();
    virtual ~Thumb() = default;
    
    // ========== 拖动事件 ==========
    
    /// 开始拖动事件
    core::Event<> DragStarted;
    
    /// 拖动中事件（提供偏移量）
    core::Event<DragDeltaEventArgs> DragDelta;
    
    /// 完成拖动事件
    core::Event<> DragCompleted;
    
    // ========== 拖动状态 ==========
    
    /// 是否正在拖动
    bool IsDragging() const { return isDragging_; }
    
    /// 取消当前拖动
    void CancelDrag();
    
protected:
    // ========== 事件重写 ==========
    
    void OnPointerPressed(PointerEventArgs& e) override;
    void OnPointerMoved(PointerEventArgs& e) override;
    void OnPointerReleased(PointerEventArgs& e) override;
    void OnPointerExited(PointerEventArgs& e) override;
    
private:
    // ========== 内部状态 ==========
    
    bool isDragging_{false};          ///< 是否正在拖动
    float dragStartX_{0.0f};          ///< 拖动起始 X 坐标
    float dragStartY_{0.0f};          ///< 拖动起始 Y 坐标
    float lastX_{0.0f};               ///< 上次的 X 坐标
    float lastY_{0.0f};               ///< 上次的 Y 坐标
    
    /// 开始拖动
    void StartDrag(float x, float y);
    
    /// 更新拖动
    void UpdateDrag(float x, float y);
    
    /// 结束拖动
    void EndDrag();
};

} // namespace fk::ui
