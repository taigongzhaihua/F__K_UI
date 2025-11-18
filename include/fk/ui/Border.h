#pragma once

#include "fk/ui/Control.h"
#include "fk/ui/Thickness.h"
#include "fk/ui/CornerRadius.h"
#include "fk/ui/PropertyMacros.h"
#include "fk/binding/DependencyProperty.h"
#include "fk/core/Event.h"

namespace fk::ui {

// 前向声明
class Brush;

/**
 * @brief 边框控件
 * 
 * 职责：
 * - 绘制边框和背景
 * - 包含单个子元素
 * 
 * WPF 对应：Border
 */
class Border : public FrameworkElement<Border> {
public:
    // ========== 依赖属性 ==========
    
    /// Child 属性：子元素
    static const binding::DependencyProperty& ChildProperty();
    
    /// BorderBrush 属性：边框画刷
    static const binding::DependencyProperty& BorderBrushProperty();
    
    /// BorderThickness 属性：边框粗细
    static const binding::DependencyProperty& BorderThicknessProperty();
    
    /// CornerRadius 属性：圆角半径
    static const binding::DependencyProperty& CornerRadiusProperty();
    
    /// Background 属性：背景画刷
    static const binding::DependencyProperty& BackgroundProperty();
    
    /// Padding 属性：内边距
    static const binding::DependencyProperty& PaddingProperty();

public:
    Border() = default;
    virtual ~Border() = default;

    // ========== 子元素 ==========
    
    UIElement* GetChild() const { return GetValue<UIElement*>(ChildProperty()); }
    void SetChild(UIElement* child);
    
    Border* Child(UIElement* child) {
        SetChild(child);
        return this;
    }
    UIElement* Child() const { return GetChild(); }

    // ========== 外观 ==========
    
    // 使用 PropertyMacros 简化属性声明（从 24 行减少到 2 行）
    FK_PROPERTY_VISUAL(Background, Brush*, Border)
    FK_PROPERTY_VISUAL(BorderBrush, Brush*, Border)
    FK_PROPERTY_VISUAL(BorderThickness, Thickness, Border)
    
    Border* BorderThickness(float uniform) {
        SetBorderThickness(Thickness(uniform));
        return this;
    }
    Border* BorderThickness(float left, float top, float right, float bottom) {
        SetBorderThickness(Thickness(left, top, right, bottom));
        return this;
    }
    
    ui::CornerRadius GetCornerRadius() const { return GetValue<ui::CornerRadius>(CornerRadiusProperty()); }
    void SetCornerRadius(const ui::CornerRadius& value) { SetValue(CornerRadiusProperty(), value); }
    
    Border* CornerRadius(float uniform) {
        SetCornerRadius(ui::CornerRadius(uniform));
        return this;
    }
    Border* CornerRadius(float topLeft, float topRight, float bottomRight, float bottomLeft) {
        SetCornerRadius(ui::CornerRadius(topLeft, topRight, bottomRight, bottomLeft));
        return this;
    }
    ui::CornerRadius CornerRadius() const { return GetCornerRadius(); }
    
    Thickness GetPadding() const { return GetValue<Thickness>(PaddingProperty()); }
    void SetPadding(const Thickness& value) { SetValue(PaddingProperty(), value); }
    
    Border* Padding(float uniform) {
        SetPadding(Thickness(uniform));
        return this;
    }
    Border* Padding(float left, float top, float right, float bottom) {
        SetPadding(Thickness(left, top, right, bottom));
        return this;
    }
    Thickness Padding() const { return GetPadding(); }
    
    /**
     * @brief 获取逻辑子元素（覆写 UIElement）
     * 
     * Border 只有一个子元素
     */
    std::vector<UIElement*> GetLogicalChildren() const override {
        std::vector<UIElement*> children;
        UIElement* child = GetChild();
        if (child) {
            children.push_back(child);
        }
        return children;
    }

protected:
    Size MeasureOverride(const Size& availableSize) override;
    Size ArrangeOverride(const Size& finalSize) override;
    void OnRender(render::RenderContext& context) override;
    
    void OnPropertyChanged(const binding::DependencyProperty& property,
                          const std::any& oldValue,
                          const std::any& newValue,
                          binding::ValueSource oldSource,
                          binding::ValueSource newSource) override;

private:
    void ObserveBrush(Brush* brush, core::Event<const binding::DependencyProperty&, const std::any&, const std::any&, binding::ValueSource, binding::ValueSource>::Connection& connection);
    
    core::Event<const binding::DependencyProperty&, const std::any&, const std::any&, binding::ValueSource, binding::ValueSource>::Connection backgroundConnection_;
    core::Event<const binding::DependencyProperty&, const std::any&, const std::any&, binding::ValueSource, binding::ValueSource>::Connection borderBrushConnection_;
};

} // namespace fk::ui
