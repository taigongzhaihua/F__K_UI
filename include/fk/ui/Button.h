#pragma once

#include "fk/ui/Control.h"
#include "fk/ui/TextBlock.h"
#include "fk/ui/BindingMacros.h"
#include "fk/ui/DependencyPropertyMacros.h"
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

    // 依赖属性（使用宏声明）
    FK_DEPENDENCY_PROPERTY_DECLARE(CornerRadius, float)
    FK_DEPENDENCY_PROPERTY_DECLARE_REF(Background, std::string)
    FK_DEPENDENCY_PROPERTY_DECLARE_REF(Foreground, std::string)
    FK_DEPENDENCY_PROPERTY_DECLARE_REF(HoveredBackground, std::string)
    FK_DEPENDENCY_PROPERTY_DECLARE_REF(PressedBackground, std::string)
    FK_DEPENDENCY_PROPERTY_DECLARE_REF(BorderBrush, std::string)
    FK_DEPENDENCY_PROPERTY_DECLARE(BorderThickness, float)
    FK_DEPENDENCY_PROPERTY_DECLARE(IsMouseOver, bool)
    FK_DEPENDENCY_PROPERTY_DECLARE(IsPressed, bool)

public:
    // 便利方法（无 Get 前缀，更符合 bool 属性的命名习惯）
    [[nodiscard]] bool IsMouseOver() const { return GetIsMouseOver(); }
    [[nodiscard]] bool IsPressed() const { return GetIsPressed(); }

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
    bool OnMouseButtonDown(int button, double x, double y) override;
    bool OnMouseButtonUp(int button, double x, double y) override;
    bool OnMouseMove(double x, double y) override;

    // 事件处理
    virtual void OnClick();
    virtual void OnMouseEnter();
    virtual void OnMouseLeave();
    virtual void OnMouseDown();
    virtual void OnMouseUp();

    // Visual 接口实现
    bool HasRenderContent() const override { return true; }

private:
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

    // 🎯 使用宏简化绑定支持
    FK_BINDING_PROPERTY_VALUE_BASE(CornerRadius, float, ButtonBase)
    FK_BINDING_PROPERTY_BASE(Background, std::string, ButtonBase)
    FK_BINDING_PROPERTY_BASE(Foreground, std::string, ButtonBase)
    FK_BINDING_PROPERTY_BASE(HoveredBackground, std::string, ButtonBase)
    FK_BINDING_PROPERTY_BASE(PressedBackground, std::string, ButtonBase)
    FK_BINDING_PROPERTY_BASE(BorderBrush, std::string, ButtonBase)
    FK_BINDING_PROPERTY_VALUE_BASE(BorderThickness, float, ButtonBase)

    /**
     * @brief 设置按钮内容
     */
    Ptr Content(ContentPtr content) {
        this->SetContent(std::move(content));
        return this->Self();
    }

    /**
     * @brief 设置按钮文本内容（便捷方法）
     * @param text 按钮文本
     * @note 自动创建一个 14.0f 字体大小的 TextBlock,并应用 Button 的 Foreground 颜色
     */
    Ptr Content(const std::string& text) {
        auto textBlock = ui::textBlock()
            ->Text(text)
            ->FontSize(14.0f)
            ->Foreground(this->GetForeground());
        this->SetContent(std::static_pointer_cast<UIElement>(textBlock));
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
