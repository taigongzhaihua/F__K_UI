#pragma once

#include "fk/ui/ToggleButton.h"
#include "fk/ui/PropertyMacros.h"

namespace fk::ui {

/**
 * @brief 复选框控件
 * 
 * 职责：
 * - 继承自 ToggleButton，提供复选框的视觉呈现
 * - 显示一个可勾选的方框和内容
 * - 支持三态：选中、未选中、不确定
 * 
 * WPF 对应：CheckBox
 */
class CheckBox : public ToggleButton {
public:
    CheckBox();
    virtual ~CheckBox() = default;
    
    // ========== 依赖属性 ==========
    
    /// 复选框标记颜色依赖属性
    static const binding::DependencyProperty& CheckMarkColorProperty();
    
    /// 复选框边框颜色依赖属性
    static const binding::DependencyProperty& CheckBoxBorderColorProperty();

protected:
    /**
     * @brief 模板应用后的钩子
     */
    void OnTemplateApplied() override;

    /**
     * @brief 鼠标按下事件（调试用）
     */
    void OnPointerPressed(PointerEventArgs &e) override;

    /**
     * @brief 鼠标释放事件（调试用）
     */
    void OnPointerReleased(PointerEventArgs &e) override;

    /**
     * @brief 点击事件（调试用）
     */
    void OnClick() override;
    
    // ========== 属性访问器（使用宏生成） ==========
    
    /// 复选框标记颜色属性（Get/Set/链式调用/绑定）
    FK_PROPERTY_VISUAL(CheckMarkColor, Brush*, CheckBox)
    
    /// 复选框边框颜色属性（Get/Set/链式调用/绑定）
    FK_PROPERTY_VISUAL(CheckBoxBorderColor, Brush*, CheckBox)
    
    /// 便捷方法：接受颜色参数的链式调用
    CheckBox* CheckMarkColor(const Color& color) { 
        SetCheckMarkColor(new SolidColorBrush(color)); 
        return this; 
    }
    CheckBox* CheckBoxBorderColor(const Color& color) { 
        SetCheckBoxBorderColor(new SolidColorBrush(color)); 
        return this; 
    }
};

} // namespace fk::ui
