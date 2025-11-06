#pragma once

#include "fk/ui/Control.h"
#include "fk/ui/Enums.h"
#include "fk/ui/BindingMacros.h"
#include "fk/ui/DependencyPropertyMacros.h"
#include "fk/core/Event.h"

namespace fk::ui {

namespace detail {

/**
 * @brief ScrollBar 基类实现
 * 
 * 提供滚动条的核心功能：
 * - 数值范围 (Minimum, Maximum, Value)
 * - 可见区域大小 (ViewportSize)
 * - 方向 (Horizontal/Vertical)
 * - Thumb (滑块) 拖动交互
 * - Track (轨道) 点击跳转
 */
class ScrollBarBase : public ControlBase {
public:
    using ControlBase::ControlBase;

    ScrollBarBase();
    ~ScrollBarBase() override;

    // 依赖属性（使用宏声明）
    FK_DEPENDENCY_PROPERTY_DECLARE(Minimum, double)
    FK_DEPENDENCY_PROPERTY_DECLARE(Maximum, double)
    FK_DEPENDENCY_PROPERTY_DECLARE(Value, double)
    FK_DEPENDENCY_PROPERTY_DECLARE(ViewportSize, double)
    FK_DEPENDENCY_PROPERTY_DECLARE(Orientation, ui::Orientation)
    FK_DEPENDENCY_PROPERTY_DECLARE_REF(ThumbBrush, std::string)
    FK_DEPENDENCY_PROPERTY_DECLARE_REF(TrackBrush, std::string)
    FK_DEPENDENCY_PROPERTY_DECLARE(Thickness, float)

public:

    // 值变更事件
    core::Event<double> ValueChanged;

    // 鼠标事件处理
    void HandleMouseDown(float x, float y);
    void HandleMouseMove(float x, float y);
    void HandleMouseUp();

    // 辅助方法 (公开,用于渲染)
    Rect GetThumbBounds() const;  // 获取 Thumb 的边界矩形

protected:
    Size MeasureOverride(const Size& availableSize) override;
    Size ArrangeOverride(const Size& finalSize) override;
    bool HasRenderContent() const override;

    // 鼠标事件重写
    bool OnMouseButtonDown(int button, double x, double y) override;
    bool OnMouseButtonUp(int button, double x, double y) override;
    bool OnMouseMove(double x, double y) override;

    // 辅助方法
    void CoerceValue();  // 限制 Value 在 [Minimum, Maximum] 范围内
    double PointToValue(float x, float y) const;  // 将鼠标位置转换为值

private:
    void ApplyOrientationLayout(ui::Orientation orientation);

    // 验证回调
    static bool ValidateValue(const std::any& value);

    // 拖动状态
    bool isDragging_ = false;
    float dragStartX_ = 0.0f;
    float dragStartY_ = 0.0f;
    double dragStartValue_ = 0.0;
};

} // namespace detail

// ScrollBar 模板类 (支持链式调用)
template <typename Derived>
class ScrollBar : public detail::ScrollBarBase, public std::enable_shared_from_this<Derived> {
public:
    using BaseType = detail::ScrollBarBase;
    using Ptr = std::shared_ptr<Derived>;

    // 🎯 Fluent API with Binding Support
    FK_BINDING_PROPERTY_VALUE(Minimum, double)
    FK_BINDING_PROPERTY_VALUE(Maximum, double)
    FK_BINDING_PROPERTY_VALUE(Value, double)
    FK_BINDING_PROPERTY_VALUE(ViewportSize, double)
    FK_BINDING_PROPERTY_ENUM(Orientation, ui::Orientation)
    FK_BINDING_PROPERTY(ThumbBrush, std::string)
    FK_BINDING_PROPERTY(TrackBrush, std::string)
    FK_BINDING_PROPERTY_VALUE(Thickness, float)

    // Fluent API: OnValueChanged (事件订阅)
    Ptr OnValueChanged(std::function<void(double)> callback) {
        ValueChanged += std::move(callback);
        return Self();
    }

protected:
    Ptr Self() {
        auto* derivedThis = static_cast<Derived*>(this);
        return std::static_pointer_cast<Derived>(derivedThis->shared_from_this());
    }
};

// 标准 ScrollBar 类
class ScrollBarView : public ScrollBar<ScrollBarView> {
public:
    using ScrollBar<ScrollBarView>::ScrollBar;
};

// 工厂函数
inline std::shared_ptr<ScrollBarView> scrollBar() {
    return std::make_shared<ScrollBarView>();
}

} // namespace fk::ui
