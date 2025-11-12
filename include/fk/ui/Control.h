#pragma once

#include "fk/ui/FrameworkElement.h"
#include "fk/ui/Thickness.h"
#include "fk/ui/TextEnums.h"
#include "fk/ui/Style.h"
#include "fk/binding/DependencyProperty.h"
#include "fk/binding/BindingExpression.h"
#include <memory>
#include <string>

namespace fk::ui {

// 前向声明
class ControlTemplate;
class Brush;

/**
 * @brief 控件基类（CRTP 模式）
 * 
 * 职责：
 * - 控件模板
 * - 外观属性（前景、背景、边框等）
 * - 焦点和鼠标状态
 * 
 * 模板参数：Derived - 派生类类型（CRTP）
 * 继承：FrameworkElement<Derived>
 */
template<typename Derived>
class Control : public FrameworkElement<Derived> {
public:
    Control() = default;
    virtual ~Control() = default;

    // ========== 依赖属性声明 ==========
    
    /**
     * @brief 前景画刷依赖属性
     */
    static const binding::DependencyProperty& ForegroundProperty();
    
    /**
     * @brief 背景画刷依赖属性
     */
    static const binding::DependencyProperty& BackgroundProperty();
    
    /**
     * @brief 边框画刷依赖属性
     */
    static const binding::DependencyProperty& BorderBrushProperty();
    
    /**
     * @brief 边框厚度依赖属性
     */
    static const binding::DependencyProperty& BorderThicknessProperty();
    
    /**
     * @brief 内边距依赖属性
     */
    static const binding::DependencyProperty& PaddingProperty();
    
    /**
     * @brief 字体系列依赖属性
     */
    static const binding::DependencyProperty& FontFamilyProperty();
    
    /**
     * @brief 字体大小依赖属性
     */
    static const binding::DependencyProperty& FontSizeProperty();
    
    /**
     * @brief 字体粗细依赖属性
     */
    static const binding::DependencyProperty& FontWeightProperty();
    
    /**
     * @brief 样式依赖属性
     */
    static const binding::DependencyProperty& StyleProperty();

    // ========== 控件样式 ==========
    
    Style* GetStyle() const {
        return this->template GetValue<Style*>(StyleProperty());
    }
    void SetStyle(Style* style) {
        this->SetValue(StyleProperty(), style);
    }
    Derived* Style(Style* style) {
        SetStyle(style);
        return static_cast<Derived*>(this);
    }
    Style* Style() const { return GetStyle(); }
    
    // ========== 控件模板 ==========
    
    ControlTemplate* GetTemplate() const { return template_; }
    void SetTemplate(ControlTemplate* tmpl);
    Derived* Template(ControlTemplate* tmpl) {
        SetTemplate(tmpl);
        return static_cast<Derived*>(this);
    }
    ControlTemplate* Template() const { return GetTemplate(); }

    // ========== 外观属性 ==========
    
    // 前景
    Brush* GetForeground() const {
        auto value = this->template GetValue<Brush*>(ForegroundProperty());
        return value;
    }
    void SetForeground(Brush* brush) {
        this->SetValue(ForegroundProperty(), brush);
        this->InvalidateVisual();
    }
    Derived* Foreground(Brush* brush) {
        SetForeground(brush);
        return static_cast<Derived*>(this);
    }
    Brush* Foreground() const { return GetForeground(); }
    
    // 背景
    Brush* GetBackground() const {
        return this->template GetValue<Brush*>(BackgroundProperty());
    }
    void SetBackground(Brush* brush) {
        this->SetValue(BackgroundProperty(), brush);
        this->InvalidateVisual();
    }
    Derived* Background(Brush* brush) {
        SetBackground(brush);
        return static_cast<Derived*>(this);
    }
    Brush* Background() const { return GetBackground(); }
    
    // 边框画刷
    Brush* GetBorderBrush() const {
        return this->template GetValue<Brush*>(BorderBrushProperty());
    }
    void SetBorderBrush(Brush* brush) {
        this->SetValue(BorderBrushProperty(), brush);
        this->InvalidateVisual();
    }
    Derived* BorderBrush(Brush* brush) {
        SetBorderBrush(brush);
        return static_cast<Derived*>(this);
    }
    Brush* BorderBrush() const { return GetBorderBrush(); }
    
    // 边框厚度
    Thickness GetBorderThickness() const {
        return this->template GetValue<Thickness>(BorderThicknessProperty());
    }
    void SetBorderThickness(const Thickness& thickness) {
        this->SetValue(BorderThicknessProperty(), thickness);
        this->InvalidateMeasure();
    }
    Derived* BorderThickness(float uniform) {
        SetBorderThickness(Thickness(uniform));
        return static_cast<Derived*>(this);
    }
    Derived* BorderThickness(float left, float top, float right, float bottom) {
        SetBorderThickness(Thickness(left, top, right, bottom));
        return static_cast<Derived*>(this);
    }
    Thickness BorderThickness() const { return GetBorderThickness(); }
    
    // 内边距
    Thickness GetPadding() const {
        return this->template GetValue<Thickness>(PaddingProperty());
    }
    void SetPadding(const Thickness& padding) {
        this->SetValue(PaddingProperty(), padding);
        this->InvalidateMeasure();
    }
    Derived* Padding(float uniform) {
        SetPadding(Thickness(uniform));
        return static_cast<Derived*>(this);
    }
    Derived* Padding(float left, float top, float right, float bottom) {
        SetPadding(Thickness(left, top, right, bottom));
        return static_cast<Derived*>(this);
    }
    Thickness Padding() const { return GetPadding(); }

    // ========== 字体属性 ==========
    
    std::string GetFontFamily() const {
        return this->template GetValue<std::string>(FontFamilyProperty());
    }
    void SetFontFamily(const std::string& family) {
        this->SetValue(FontFamilyProperty(), family);
        this->InvalidateMeasure();
    }
    Derived* FontFamily(const std::string& family) {
        SetFontFamily(family);
        return static_cast<Derived*>(this);
    }
    std::string FontFamily() const { return GetFontFamily(); }
    
    float GetFontSize() const {
        return this->template GetValue<float>(FontSizeProperty());
    }
    void SetFontSize(float size) {
        this->SetValue(FontSizeProperty(), size);
        this->InvalidateMeasure();
    }
    Derived* FontSize(float size) {
        SetFontSize(size);
        return static_cast<Derived*>(this);
    }
    float FontSize() const { return GetFontSize(); }
    
    ui::FontWeight GetFontWeight() const {
        return this->template GetValue<ui::FontWeight>(FontWeightProperty());
    }
    void SetFontWeight(ui::FontWeight weight) {
        this->SetValue(FontWeightProperty(), weight);
        this->InvalidateMeasure();
    }
    Derived* FontWeight(ui::FontWeight weight) {
        SetFontWeight(weight);
        return static_cast<Derived*>(this);
    }
    ui::FontWeight FontWeight() const { return GetFontWeight(); }

    // ========== 状态属性 ==========
    
    bool IsFocused() const { return isFocused_; }
    bool IsMouseOver() const { return isMouseOver_; }

    // ========== 虚方法 ==========
    
    /**
     * @brief 获取默认样式键
     */
    virtual const std::type_info& GetDefaultStyleKey() const {
        return typeid(Derived);
    }

protected:
    /**
     * @brief 模板应用钩子
     */
    void OnApplyTemplate() override {
        // 派生类覆写以获取模板部件
    }
    
    /**
     * @brief 鼠标进入
     */
    void OnPointerEntered(PointerEventArgs& e) override {
        isMouseOver_ = true;
        this->InvalidateVisual();
    }
    
    /**
     * @brief 鼠标离开
     */
    void OnPointerExited(PointerEventArgs& e) override {
        isMouseOver_ = false;
        this->InvalidateVisual();
    }
    
    /**
     * @brief 获得焦点
     */
    virtual void OnGotFocus() {
        isFocused_ = true;
        this->InvalidateVisual();
    }
    
    /**
     * @brief 失去焦点
     */
    virtual void OnLostFocus() {
        isFocused_ = false;
        this->InvalidateVisual();
        
        // Trigger UpdateSource for bindings with LostFocus trigger
        UpdateSourceOnLostFocus();
    }
    
    /**
     * @brief Helper to update all bindings with LostFocus trigger
     */
    void UpdateSourceOnLostFocus() {
        // Iterate through all properties with bindings
        const auto& store = this->GetPropertyStore();
        // Note: PropertyStore doesn't expose all bindings, so we need to check known properties
        // This is a simplified implementation - in production, PropertyStore should provide iterator
        
        // Common properties that might have TwoWay bindings
        static const DependencyProperty* commonProps[] = {
            // Text-based controls would have TextProperty, Value-based would have ValueProperty, etc.
            // For now, we'll rely on derived classes to override if they have specific properties
        };
        
        for (const auto* prop : commonProps) {
            if (prop) {
                auto binding = this->GetBinding(*prop);
                if (binding && binding->IsActive() && 
                    binding->GetEffectiveUpdateSourceTrigger() == binding::UpdateSourceTrigger::LostFocus) {
                    binding->UpdateSource();
                }
            }
        }
    }

protected:
    /**
     * @brief Style属性变更回调
     */
    static void OnStyleChanged(
        binding::DependencyObject& d,
        const binding::DependencyProperty& prop,
        const std::any& oldValue,
        const std::any& newValue
    );

private:
    ControlTemplate* template_{nullptr};
    
    // 状态（非依赖属性，用于内部状态跟踪）
    bool isFocused_{false};
    bool isMouseOver_{false};
};

// 模板实现

template<typename Derived>
const binding::DependencyProperty& Control<Derived>::StyleProperty() {
    static auto& property = binding::DependencyProperty::Register(
        "Style",
        typeid(Style*),
        typeid(Control<Derived>),
        binding::PropertyMetadata{
            std::any(static_cast<Style*>(nullptr)),
            &Control<Derived>::OnStyleChanged
        }
    );
    return property;
}

template<typename Derived>
void Control<Derived>::OnStyleChanged(
    binding::DependencyObject& d,
    const binding::DependencyProperty& prop,
    const std::any& oldValue,
    const std::any& newValue
) {
    auto* control = dynamic_cast<Control<Derived>*>(&d);
    if (!control) {
        return;
    }
    
    // 取消应用旧样式
    try {
        auto* oldStyle = std::any_cast<Style*>(oldValue);
        if (oldStyle != nullptr) {
            oldStyle->Unapply(control);
        }
    } catch (const std::bad_any_cast&) {
        // Ignore if cast fails
    }
    
    // 应用新样式
    try {
        auto* newStyle = std::any_cast<Style*>(newValue);
        if (newStyle != nullptr) {
            if (!newStyle->IsApplicableTo(typeid(Derived))) {
                // TODO: 记录警告日志
                // 类型不匹配，但仍尝试应用（允许基类样式应用到派生类）
            }
            newStyle->Apply(control);
        }
    } catch (const std::bad_any_cast&) {
        // Ignore if cast fails
    }
    
    control->InvalidateVisual();
}

template<typename Derived>
void Control<Derived>::SetTemplate(ControlTemplate* tmpl) {
    if (template_ != tmpl) {
        template_ = tmpl;
        // TODO: 重新应用模板
        this->ApplyTemplate();
    }
}

} // namespace fk::ui
