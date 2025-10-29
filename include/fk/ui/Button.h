#pragma once

#include "fk/ui/Control.h"
#include "fk/core/Event.h"

#include <string>
#include <functional>

namespace fk::ui {

namespace detail {

/**
 * @brief Button 基类实现
 * 
 * 提供按钮的核心功能：
 * - 点击事件
 * - 圆角半径
 * - 背景颜色
 * - 悬停/按下状态
 */
class ButtonBase : public ControlBase {
public:
    using ControlBase::ControlBase;

    ButtonBase();
    ~ButtonBase() override;

    // 依赖属性
    static const binding::DependencyProperty& CornerRadiusProperty();
    static const binding::DependencyProperty& BackgroundProperty();
    static const binding::DependencyProperty& ForegroundProperty();
    static const binding::DependencyProperty& BorderBrushProperty();
    static const binding::DependencyProperty& BorderThicknessProperty();
    static const binding::DependencyProperty& IsMouseOverProperty();
    static const binding::DependencyProperty& IsPressedProperty();

    // 圆角半径
    void SetCornerRadius(float radius);
    [[nodiscard]] float GetCornerRadius() const;

    // 背景色 (RGBA hex string: "#RRGGBBAA" or "#RRGGBB")
    void SetBackground(std::string color);
    [[nodiscard]] const std::string& GetBackground() const;

    // 前景色（文字颜色）
    void SetForeground(std::string color);
    [[nodiscard]] const std::string& GetForeground() const;

    // 边框颜色
    void SetBorderBrush(std::string color);
    [[nodiscard]] const std::string& GetBorderBrush() const;

    // 边框粗细
    void SetBorderThickness(float thickness);
    [[nodiscard]] float GetBorderThickness() const;

    // 鼠标状态（只读）
    [[nodiscard]] bool IsMouseOver() const;
    [[nodiscard]] bool IsPressed() const;

    // 获取当前实际渲染的背景色（根据状态计算）
    [[nodiscard]] std::string GetActualBackground() const;

    // 点击事件
    core::Event<ButtonBase&> Click;

    // 公共方法供外部调用 (如 Window 的鼠标事件处理)
    void HandleMouseDown() { OnMouseDown(); }
    void HandleMouseUp() { OnMouseUp(); }
    void HandleMouseEnter() { OnMouseEnter(); }
    void HandleMouseLeave() { OnMouseLeave(); }

protected:
    // 重写 UIElement 的鼠标事件
    void OnMouseButtonDown(int button, double x, double y) override;
    void OnMouseButtonUp(int button, double x, double y) override;
    void OnMouseMove(double x, double y) override;

    // 事件处理
    virtual void OnClick();
    virtual void OnMouseEnter();
    virtual void OnMouseLeave();
    virtual void OnMouseDown();
    virtual void OnMouseUp();

    // 属性变更回调
    virtual void OnCornerRadiusChanged(float oldValue, float newValue);
    virtual void OnBackgroundChanged(const std::string& oldValue, const std::string& newValue);
    virtual void OnForegroundChanged(const std::string& oldValue, const std::string& newValue);
    virtual void OnBorderBrushChanged(const std::string& oldValue, const std::string& newValue);
    virtual void OnBorderThicknessChanged(float oldValue, float newValue);
    virtual void OnIsMouseOverChanged(bool oldValue, bool newValue);
    virtual void OnIsPressedChanged(bool oldValue, bool newValue);

    // Visual 接口实现
    bool HasRenderContent() const override { return true; }

private:
    // 依赖属性元数据构建
    static binding::PropertyMetadata BuildCornerRadiusMetadata();
    static binding::PropertyMetadata BuildBackgroundMetadata();
    static binding::PropertyMetadata BuildForegroundMetadata();
    static binding::PropertyMetadata BuildBorderBrushMetadata();
    static binding::PropertyMetadata BuildBorderThicknessMetadata();
    static binding::PropertyMetadata BuildIsMouseOverMetadata();
    static binding::PropertyMetadata BuildIsPressedMetadata();

    // 属性变更回调
    static void CornerRadiusPropertyChanged(binding::DependencyObject& sender,
        const binding::DependencyProperty& property, const std::any& oldValue, const std::any& newValue);
    static void BackgroundPropertyChanged(binding::DependencyObject& sender,
        const binding::DependencyProperty& property, const std::any& oldValue, const std::any& newValue);
    static void ForegroundPropertyChanged(binding::DependencyObject& sender,
        const binding::DependencyProperty& property, const std::any& oldValue, const std::any& newValue);
    static void BorderBrushPropertyChanged(binding::DependencyObject& sender,
        const binding::DependencyProperty& property, const std::any& oldValue, const std::any& newValue);
    static void BorderThicknessPropertyChanged(binding::DependencyObject& sender,
        const binding::DependencyProperty& property, const std::any& oldValue, const std::any& newValue);
    static void IsMouseOverPropertyChanged(binding::DependencyObject& sender,
        const binding::DependencyProperty& property, const std::any& oldValue, const std::any& newValue);
    static void IsPressedPropertyChanged(binding::DependencyObject& sender,
        const binding::DependencyProperty& property, const std::any& oldValue, const std::any& newValue);

    // 验证函数
    static bool ValidateCornerRadius(const std::any& value);
    static bool ValidateColor(const std::any& value);
    static bool ValidateBorderThickness(const std::any& value);

    // 内部状态
    bool isMouseCaptured_{false};
};

} // namespace detail

/**
 * @brief Button 控件 - 支持圆角、自定义背景色的按钮
 * 
 * 使用示例：
 * @code
 * auto button = Button<>::Create()
 *     ->Content(TextBlock::Create()->Text("点击我"))
 *     ->CornerRadius(8.0f)
 *     ->Background("#4CAF50")
 *     ->Foreground("#FFFFFF")
 *     ->Width(120)
 *     ->Height(40);
 * 
 * button->OnClick([](ButtonBase& btn) {
 *     std::cout << "按钮被点击！" << std::endl;
 * });
 * @endcode
 */
template <typename Derived = void>
class Button : public View<std::conditional_t<std::is_void_v<Derived>, Button<>, Derived>, detail::ButtonBase> {
public:
    using ButtonBase = detail::ButtonBase;
    using Base = View<std::conditional_t<std::is_void_v<Derived>, Button<>, Derived>, detail::ButtonBase>;
    using Ptr = typename Base::Ptr;
    using ContentPtr = std::shared_ptr<UIElement>;

    using Base::Base;

    /**
     * @brief 设置圆角半径（单位：像素）
     */
    Ptr CornerRadius(float radius) {
        this->SetCornerRadius(radius);
        return this->Self();
    }

    /**
     * @brief 设置背景颜色
     * @param color 颜色字符串，格式：
     *   - "#RRGGBB" (如 "#FF5733")
     *   - "#RRGGBBAA" (如 "#FF5733FF")
     */
    Ptr Background(std::string color) {
        this->SetBackground(std::move(color));
        return this->Self();
    }

    /**
     * @brief 设置前景色（文字颜色）
     */
    Ptr Foreground(std::string color) {
        this->SetForeground(std::move(color));
        return this->Self();
    }

    /**
     * @brief 设置边框颜色
     */
    Ptr BorderBrush(std::string color) {
        this->SetBorderBrush(std::move(color));
        return this->Self();
    }

    /**
     * @brief 设置边框粗细
     */
    Ptr BorderThickness(float thickness) {
        this->SetBorderThickness(thickness);
        return this->Self();
    }

    /**
     * @brief 设置按钮内容
     */
    Ptr Content(ContentPtr content) {
        this->SetContent(std::move(content));
        return this->Self();
    }

    /**
     * @brief 订阅点击事件
     * @param handler 事件处理函数
     */
    Ptr OnClick(std::function<void(ButtonBase&)> handler) {
        this->Click.Add(std::move(handler));
        return this->Self();
    }

    /**
     * @brief 创建按钮实例
     */
    static Ptr Create() {
        return std::make_shared<std::conditional_t<std::is_void_v<Derived>, Button<>, Derived>>();
    }
};

// 默认实例化
using ButtonPtr = Button<>::Ptr;

/**
 * @brief 便捷的按钮工厂函数
 * @return 新创建的 Button 实例
 * 
 * 使用示例:
 * auto btn = button()->Width(100)->Height(50)->Background("#4CAF50");
 */
inline ButtonPtr button() {
    return Button<>::Create();
}

} // namespace fk::ui
