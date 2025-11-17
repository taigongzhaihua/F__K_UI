/**
 * @file TemplateBinding.h
 * @brief 模板绑定 - 绑定到模板化父元素的属性
 * 
 * WPF 对应：{TemplateBinding Property}
 * 
 * TemplateBinding 是一种特殊的绑定，用于在 ControlTemplate 中
 * 将模板内元素的属性绑定到应用模板的控件（TemplatedParent）的属性。
 * 
 * 示例用法：
 * ```cpp
 * // 在 Button 的 ControlTemplate 中：
 * auto border = new Border();
 * auto binding = TemplateBinding(Button::BackgroundProperty());
 * border->SetBinding(Border::BackgroundProperty(), binding);
 * // 现在 Border 的 Background 自动跟随 Button 的 Background
 * ```
 */

#pragma once

#include "fk/binding/Binding.h"

namespace fk::binding {

/**
 * @brief 模板绑定类
 * 
 * TemplateBinding 简化了在 ControlTemplate 中绑定到 TemplatedParent 的语法。
 * 它本质上是一个 Binding，其 Source 自动设置为 TemplatedParent。
 */
class TemplateBinding : public Binding {
public:
    /**
     * @brief 构造 TemplateBinding
     * @param sourceProperty 要绑定的 TemplatedParent 属性
     */
    explicit TemplateBinding(const DependencyProperty& sourceProperty)
        : Binding()
        , sourceProperty_(&sourceProperty)
    {
        // 使用 Binding 的链式 API
        Path(sourceProperty.Name());
        Mode(BindingMode::OneWay);  // TemplateBinding 默认单向
        
        // 注意：Source 不在这里设置，而是在应用绑定时
        // 由 BindingExpression 检测 TemplateBinding 并自动设置为 TemplatedParent
    }
    
    ~TemplateBinding() override = default;
    
    /**
     * @brief 标记这是一个 TemplateBinding
     */
    [[nodiscard]] bool IsTemplateBinding() const noexcept override { return true; }
    
    /**
     * @brief 获取源属性（TemplatedParent 的属性）
     */
    [[nodiscard]] const DependencyProperty* GetTemplateBindingSourceProperty() const noexcept override { return sourceProperty_; }
    
    /**
     * @brief 克隆 TemplateBinding 对象
     */
    [[nodiscard]] std::unique_ptr<Binding> Clone() const override { 
        return std::make_unique<TemplateBinding>(*sourceProperty_); 
    }

private:
    const DependencyProperty* sourceProperty_{nullptr};
};

} // namespace fk::binding
