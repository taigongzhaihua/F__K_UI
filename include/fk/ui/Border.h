#pragma once

#include "fk/ui/Control.h"
#include "fk/ui/Thickness.h"
#include "fk/ui/CornerRadius.h"
#include "fk/binding/DependencyProperty.h"

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
    
    Brush* GetBackground() const { return GetValue<Brush*>(BackgroundProperty()); }
    void SetBackground(Brush* value) { SetValue(BackgroundProperty(), value); }
    
    Border* Background(Brush* brush) {
        SetBackground(brush);
        return this;
    }
    Border* Background(binding::Binding binding) {
        SetBinding(BackgroundProperty(), std::move(binding));
        return this;
    }
    Brush* Background() const { return GetBackground(); }
    
    Brush* GetBorderBrush() const { return GetValue<Brush*>(BorderBrushProperty()); }
    void SetBorderBrush(Brush* value) { SetValue(BorderBrushProperty(), value); }
    
    Border* BorderBrush(Brush* brush) {
        SetBorderBrush(brush);
        return this;
    }
    Border* BorderBrush(binding::Binding binding) {
        SetBinding(BorderBrushProperty(), std::move(binding));
        return this;
    }
    Brush* BorderBrush() const { return GetBorderBrush(); }
    
    Thickness GetBorderThickness() const { return GetValue<Thickness>(BorderThicknessProperty()); }
    void SetBorderThickness(const Thickness& value) { SetValue(BorderThicknessProperty(), value); }
    
    Border* BorderThickness(float uniform) {
        SetBorderThickness(Thickness(uniform));
        return this;
    }
    Border* BorderThickness(float left, float top, float right, float bottom) {
        SetBorderThickness(Thickness(left, top, right, bottom));
        return this;
    }
    Thickness BorderThickness() const { return GetBorderThickness(); }
    
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

protected:
    Size MeasureOverride(const Size& availableSize) override;
    Size ArrangeOverride(const Size& finalSize) override;
    void OnRender(render::RenderContext& context) override;
};

} // namespace fk::ui
