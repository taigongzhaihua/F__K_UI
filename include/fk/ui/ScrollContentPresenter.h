/**
 * @file ScrollContentPresenter.h
 * @brief ScrollContentPresenter 滚动内容呈现器 - Phase 2
 * 
 * 设计理念：
 * 1. 负责内容的实际裁剪和偏移（ScrollViewer 只管理策略）
 * 2. 测量和排列内容，计算 extent 和 viewport
 * 3. 将滚动偏移转换为内容的视觉偏移
 * 4. 支持物理滚动（像素级）和逻辑滚动（项目级）
 * 
 * WPF 对应：ScrollContentPresenter
 * 
 * 职责分离：
 * - ScrollViewer: 管理滚动策略（可见性、范围限制）
 * - ScrollContentPresenter: 执行实际的滚动渲染
 * - ScrollBar: 提供用户交互界面
 */

#pragma once

#include "fk/ui/ContentPresenter.h"
#include "fk/ui/Enums.h"
#include "fk/core/Event.h"

namespace fk::ui {

class ScrollViewer;

/**
 * @brief IScrollInfo 接口（简化版）
 * 
 * 支持逻辑滚动的面板（如 VirtualizingPanel）可以实现此接口
 * 提供项目级的滚动而非像素级
 * 
 * WPF 对应：IScrollInfo
 */
class IScrollInfo {
public:
    virtual ~IScrollInfo() = default;
    
    // 逻辑滚动方法
    virtual void LineUp() = 0;
    virtual void LineDown() = 0;
    virtual void LineLeft() = 0;
    virtual void LineRight() = 0;
    virtual void PageUp() = 0;
    virtual void PageDown() = 0;
    virtual void PageLeft() = 0;
    virtual void PageRight() = 0;
    
    // 滚动到指定偏移
    virtual void SetHorizontalOffset(float offset) = 0;
    virtual void SetVerticalOffset(float offset) = 0;
    
    // 获取滚动信息
    virtual float GetHorizontalOffset() const = 0;
    virtual float GetVerticalOffset() const = 0;
    virtual float GetExtentWidth() const = 0;
    virtual float GetExtentHeight() const = 0;
    virtual float GetViewportWidth() const = 0;
    virtual float GetViewportHeight() const = 0;
    
    // 是否可以滚动
    virtual bool CanHorizontallyScroll() const = 0;
    virtual bool CanVerticallyScroll() const = 0;
    virtual void SetCanHorizontallyScroll(bool value) = 0;
    virtual void SetCanVerticallyScroll(bool value) = 0;
};

/**
 * @brief 滚动内容呈现器
 * 
 * 核心职责：
 * 1. 测量内容（给内容无限大小以获取真实尺寸）
 * 2. 排列内容（应用滚动偏移）
 * 3. 裁剪内容到视口区域
 * 4. 计算并更新 extent/viewport 信息
 * 5. 支持 IScrollInfo 接口（逻辑滚动）
 * 
 * 使用示例：
 * @code
 * auto presenter = new ScrollContentPresenter();
 * presenter->SetContent(myLargeContent);
 * presenter->SetCanHorizontallyScroll(true);
 * presenter->SetCanVerticallyScroll(true);
 * presenter->SetHorizontalOffset(100.0f);
 * @endcode
 */
class ScrollContentPresenter : public ContentPresenter<ScrollContentPresenter> {
public:
    ScrollContentPresenter();
    virtual ~ScrollContentPresenter() = default;
    
    // ========== 滚动偏移控制 ==========
    
    /// 获取水平滚动偏移
    float GetHorizontalOffset() const { return horizontalOffset_; }
    
    /// 设置水平滚动偏移
    void SetHorizontalOffset(float offset);
    
    /// 获取垂直滚动偏移
    float GetVerticalOffset() const { return verticalOffset_; }
    
    /// 设置垂直滚动偏移
    void SetVerticalOffset(float offset);
    
    // ========== 尺寸信息 ==========
    
    /// 获取视口宽度（可见区域）
    float GetViewportWidth() const { return viewportWidth_; }
    
    /// 获取视口高度（可见区域）
    float GetViewportHeight() const { return viewportHeight_; }
    
    /// 获取内容总宽度
    float GetExtentWidth() const { return extentWidth_; }
    
    /// 获取内容总高度
    float GetExtentHeight() const { return extentHeight_; }
    
    // ========== 滚动能力控制 ==========
    
    /// 是否可以水平滚动
    bool CanHorizontallyScroll() const { return canHorizontallyScroll_; }
    
    /// 设置是否可以水平滚动
    void SetCanHorizontallyScroll(bool value) {
        canHorizontallyScroll_ = value;
        InvalidateMeasure();
    }
    
    /// 是否可以垂直滚动
    bool CanVerticallyScroll() const { return canVerticallyScroll_; }
    
    /// 设置是否可以垂直滚动
    void SetCanVerticallyScroll(bool value) {
        canVerticallyScroll_ = value;
        InvalidateMeasure();
    }
    
    // ========== ScrollViewer 关联 ==========
    
    /// 获取关联的 ScrollViewer
    ScrollViewer* GetScrollOwner() const { return scrollOwner_; }
    
    /// 设置关联的 ScrollViewer（由 ScrollViewer 调用）
    void SetScrollOwner(ScrollViewer* owner) { scrollOwner_ = owner; }
    
    // ========== 事件 ==========
    
    /// 滚动信息变更事件（extent、viewport 或 offset 变化时触发）
    core::Event<> ScrollChanged;
    
protected:
    // ========== 布局重写 ==========
    
    /**
     * @brief 测量内容
     * 
     * 策略：
     * 1. 如果可以滚动，给内容该方向无限大小
     * 2. 否则，给内容 availableSize
     * 3. 记录内容的期望大小作为 extent
     */
    Size MeasureOverride(const Size& availableSize) override;
    
    /**
     * @brief 排列内容
     * 
     * 策略：
     * 1. 将内容排列在其完整大小（extent）
     * 2. 应用滚动偏移（负偏移）
     * 3. 内容会被自动裁剪到 finalSize（视口）
     */
    Size ArrangeOverride(const Size& finalSize) override;
    
    /**
     * @brief 总是裁剪内容到视口
     */
    bool ShouldClipToBounds() const override {
        return true;
    }
    
    /**
     * @brief 计算裁剪边界（视口区域）
     */
    Rect CalculateClipBounds() const override {
        return Rect(0, 0, viewportWidth_, viewportHeight_);
    }
    
private:
    // ========== 内部状态 ==========
    
    // 滚动偏移
    float horizontalOffset_{0.0f};
    float verticalOffset_{0.0f};
    
    // 尺寸信息
    float viewportWidth_{0.0f};   ///< 视口宽度（可见区域）
    float viewportHeight_{0.0f};  ///< 视口高度（可见区域）
    float extentWidth_{0.0f};     ///< 内容总宽度
    float extentHeight_{0.0f};    ///< 内容总高度
    
    // 滚动能力
    bool canHorizontallyScroll_{true};
    bool canVerticallyScroll_{true};
    
    // 关联的 ScrollViewer
    ScrollViewer* scrollOwner_{nullptr};
    
    // IScrollInfo 接口支持（内容可能实现此接口）
    IScrollInfo* scrollInfo_{nullptr};
    
    // ========== 辅助方法 ==========
    
    /// 强制偏移在有效范围内
    void CoerceOffsets();
    
    /// 通知 ScrollViewer 滚动信息已变更
    void NotifyScrollChanged();
    
    /// 检查内容是否实现 IScrollInfo
    void UpdateScrollInfo();
};

} // namespace fk::ui
