#pragma once

#include "fk/ui/ToggleButton.h"
#include "fk/ui/PropertyMacros.h"
#include <string>

namespace fk::ui {

/**
 * @brief 单选按钮控件
 * 
 * 职责：
 * - 继承自 ToggleButton，提供单选按钮的视觉呈现
 * - 显示一个圆形的可选择标记和内容
 * - 支持分组：同一组内只能有一个被选中
 * - 一旦选中，用户点击不会取消选中（只能通过选择同组其他按钮来取消）
 * 
 * WPF 对应：RadioButton
 */
class RadioButton : public ToggleButton {
public:
    RadioButton();
    virtual ~RadioButton() = default;
    
    // ========== 依赖属性 ==========
    
    /// 分组名称依赖属性
    static const binding::DependencyProperty& GroupNameProperty();
    
    /// 单选按钮标记颜色依赖属性
    static const binding::DependencyProperty& RadioMarkColorProperty();
    
    /// 单选按钮边框颜色依赖属性
    static const binding::DependencyProperty& RadioBorderColorProperty();
    
    // ========== 属性访问器（使用宏生成） ==========
    
    /// 分组名称属性（Get/Set/链式调用/绑定）
    /// 空字符串表示使用父容器作为分组依据
    FK_PROPERTY_SIMPLE(GroupName, std::string, RadioButton, {})
    
    /// 单选按钮标记颜色属性（Get/Set/链式调用/绑定）
    FK_PROPERTY_VISUAL(RadioMarkColor, Brush*, RadioButton)
    
    /// 单选按钮边框颜色属性（Get/Set/链式调用/绑定）
    FK_PROPERTY_VISUAL(RadioBorderColor, Brush*, RadioButton)
    
    /// 便捷方法：接受颜色参数的链式调用
    RadioButton* RadioMarkColor(const Color& color) { 
        SetRadioMarkColor(new SolidColorBrush(color)); 
        return this; 
    }
    RadioButton* RadioBorderColor(const Color& color) { 
        SetRadioBorderColor(new SolidColorBrush(color)); 
        return this; 
    }

protected:
    /**
     * @brief 模板应用后的钩子
     */
    void OnTemplateApplied() override;

    /**
     * @brief 点击事件处理 - RadioButton 特殊逻辑
     * 
     * RadioButton 的行为：
     * - 点击未选中的按钮 -> 选中
     * - 点击已选中的按钮 -> 保持选中（不像 CheckBox 会切换）
     * - 选中新按钮时，自动取消同组其他按钮的选中状态
     */
    void OnClick() override;
    
    /**
     * @brief 重写 Toggle 方法
     * RadioButton 不支持通过 Toggle 取消选中
     */
    void Toggle() override;

private:
    /// 取消同组其他 RadioButton 的选中状态
    void UncheckOtherRadioButtons();
    
    /// 获取同组的所有 RadioButton
    std::vector<RadioButton*> GetGroupMembers();
};

} // namespace fk::ui
