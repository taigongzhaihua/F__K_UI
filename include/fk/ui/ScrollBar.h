#pragma once

#include "fk/ui/Control.h"
#include "fk/ui/Enums.h"
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

    // 依赖属性
    static const binding::DependencyProperty& MinimumProperty();
    static const binding::DependencyProperty& MaximumProperty();
    static const binding::DependencyProperty& ValueProperty();
    static const binding::DependencyProperty& ViewportSizeProperty();
    static const binding::DependencyProperty& OrientationProperty();
    static const binding::DependencyProperty& ThumbBrushProperty();
    static const binding::DependencyProperty& TrackBrushProperty();
    static const binding::DependencyProperty& ThicknessProperty();

    // Minimum 属性 (最小值)
    void SetMinimum(double minimum);
    [[nodiscard]] double GetMinimum() const;

    // Maximum 属性 (最大值)
    void SetMaximum(double maximum);
    [[nodiscard]] double GetMaximum() const;

    // Value 属性 (当前值)
    void SetValue(double value);
    [[nodiscard]] double GetValue() const;

    // ViewportSize 属性 (可见区域大小,影响 Thumb 大小)
    void SetViewportSize(double size);
    [[nodiscard]] double GetViewportSize() const;

    // Orientation 属性 (方向)
    void SetOrientation(ui::Orientation orientation);
    [[nodiscard]] ui::Orientation GetOrientation() const;

    // ThumbBrush 属性 (滑块颜色)
    void SetThumbBrush(std::string color);
    [[nodiscard]] const std::string& GetThumbBrush() const;

    // TrackBrush 属性 (轨道颜色)
    void SetTrackBrush(std::string color);
    [[nodiscard]] const std::string& GetTrackBrush() const;

    // Thickness 属性 (滚动条厚度)
    void SetThickness(float thickness);
    [[nodiscard]] float GetThickness() const;

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
    void OnMouseButtonDown(int button, double x, double y) override;
    void OnMouseButtonUp(int button, double x, double y) override;
    void OnMouseMove(double x, double y) override;

    // 属性变更回调
    virtual void OnMinimumChanged(double oldValue, double newValue);
    virtual void OnMaximumChanged(double oldValue, double newValue);
    virtual void OnValueChanged(double oldValue, double newValue);
    virtual void OnViewportSizeChanged(double oldValue, double newValue);
    virtual void OnOrientationChanged(ui::Orientation oldValue, ui::Orientation newValue);
    virtual void OnThumbBrushChanged(const std::string& oldValue, const std::string& newValue);
    virtual void OnTrackBrushChanged(const std::string& oldValue, const std::string& newValue);
    virtual void OnThicknessChanged(float oldValue, float newValue);

    // 辅助方法
    void CoerceValue();  // 限制 Value 在 [Minimum, Maximum] 范围内
    double PointToValue(float x, float y) const;  // 将鼠标位置转换为值

private:
    void ApplyOrientationLayout(ui::Orientation orientation);

    // 元数据构建
    static binding::PropertyMetadata BuildMinimumMetadata();
    static binding::PropertyMetadata BuildMaximumMetadata();
    static binding::PropertyMetadata BuildValueMetadata();
    static binding::PropertyMetadata BuildViewportSizeMetadata();
    static binding::PropertyMetadata BuildOrientationMetadata();
    static binding::PropertyMetadata BuildThumbBrushMetadata();
    static binding::PropertyMetadata BuildTrackBrushMetadata();
    static binding::PropertyMetadata BuildThicknessMetadata();

    // 属性变更回调 (静态)
    static void MinimumPropertyChanged(
        binding::DependencyObject& sender,
        const binding::DependencyProperty& property,
        const std::any& oldValue,
        const std::any& newValue
    );

    static void MaximumPropertyChanged(
        binding::DependencyObject& sender,
        const binding::DependencyProperty& property,
        const std::any& oldValue,
        const std::any& newValue
    );

    static void ValuePropertyChanged(
        binding::DependencyObject& sender,
        const binding::DependencyProperty& property,
        const std::any& oldValue,
        const std::any& newValue
    );

    static void ViewportSizePropertyChanged(
        binding::DependencyObject& sender,
        const binding::DependencyProperty& property,
        const std::any& oldValue,
        const std::any& newValue
    );

    static void OrientationPropertyChanged(
        binding::DependencyObject& sender,
        const binding::DependencyProperty& property,
        const std::any& oldValue,
        const std::any& newValue
    );

    static void ThumbBrushPropertyChanged(
        binding::DependencyObject& sender,
        const binding::DependencyProperty& property,
        const std::any& oldValue,
        const std::any& newValue
    );

    static void TrackBrushPropertyChanged(
        binding::DependencyObject& sender,
        const binding::DependencyProperty& property,
        const std::any& oldValue,
        const std::any& newValue
    );

    static void ThicknessPropertyChanged(
        binding::DependencyObject& sender,
        const binding::DependencyProperty& property,
        const std::any& oldValue,
        const std::any& newValue
    );

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

    // Fluent API: Minimum
    [[nodiscard]] double Minimum() const {
        return GetMinimum();
    }

    Ptr Minimum(double minimum) {
        SetMinimum(minimum);
        return Self();
    }

    // Fluent API: Maximum
    [[nodiscard]] double Maximum() const {
        return GetMaximum();
    }

    Ptr Maximum(double maximum) {
        SetMaximum(maximum);
        return Self();
    }

    // Fluent API: Value
    [[nodiscard]] double Value() const {
        return GetValue();
    }

    Ptr Value(double value) {
        SetValue(value);
        return Self();
    }

    // Fluent API: ViewportSize
    [[nodiscard]] double ViewportSize() const {
        return GetViewportSize();
    }

    Ptr ViewportSize(double size) {
        SetViewportSize(size);
        return Self();
    }

    // Fluent API: Orientation
    [[nodiscard]] ui::Orientation Orientation() const {
        return GetOrientation();
    }

    Ptr Orientation(ui::Orientation orientation) {
        SetOrientation(orientation);
        return Self();
    }

    // Fluent API: ThumbBrush
    [[nodiscard]] const std::string& ThumbBrush() const {
        return GetThumbBrush();
    }

    Ptr ThumbBrush(const std::string& color) {
        SetThumbBrush(color);
        return Self();
    }

    // Fluent API: TrackBrush
    [[nodiscard]] const std::string& TrackBrush() const {
        return GetTrackBrush();
    }

    Ptr TrackBrush(const std::string& color) {
        SetTrackBrush(color);
        return Self();
    }

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
