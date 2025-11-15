#pragma once

#include "fk/ui/UIElement.h"
#include "fk/ui/Thickness.h"
#include "fk/ui/Alignment.h"
#include "fk/ui/ResourceDictionary.h"
#include "fk/core/Event.h"
#include "fk/binding/DependencyProperty.h"
#include <any>
#include <string>
#include <memory>

namespace fk::ui {

// 前向声明
class Style;

/**
 * @brief 框架元素基类（CRTP 模式）
 * 
 * 职责：
 * - 数据上下文
 * - 样式和资源
 * - 生命周期事件
 * - 尺寸约束
 * 
 * 模板参数：Derived - 派生类类型（CRTP）
 * 继承：UIElement
 */
template<typename Derived>
class FrameworkElement : public UIElement {
public:
    FrameworkElement() = default;
    virtual ~FrameworkElement() = default;

    // ========== 依赖属性声明 ==========
    
    /**
     * @brief 宽度依赖属性
     */
    static const binding::DependencyProperty& WidthProperty();
    
    /**
     * @brief 高度依赖属性
     */
    static const binding::DependencyProperty& HeightProperty();
    
    /**
     * @brief 最小宽度依赖属性
     */
    static const binding::DependencyProperty& MinWidthProperty();
    
    /**
     * @brief 最大宽度依赖属性
     */
    static const binding::DependencyProperty& MaxWidthProperty();
    
    /**
     * @brief 最小高度依赖属性
     */
    static const binding::DependencyProperty& MinHeightProperty();
    
    /**
     * @brief 最大高度依赖属性
     */
    static const binding::DependencyProperty& MaxHeightProperty();
    
    /**
     * @brief 数据上下文依赖属性
     */
    static const binding::DependencyProperty& DataContextProperty();
    
    /**
     * @brief 外边距依赖属性
     */
    static const binding::DependencyProperty& MarginProperty();
    
    /**
     * @brief 内边距依赖属性
     */
    static const binding::DependencyProperty& PaddingProperty();
    
    /**
     * @brief 水平对齐方式依赖属性
     */
    static const binding::DependencyProperty& HorizontalAlignmentProperty();
    
    /**
     * @brief 垂直对齐方式依赖属性
     */
    static const binding::DependencyProperty& VerticalAlignmentProperty();

    // ========== 数据上下文 ==========
    
    void SetDataContext(const std::any& value) {
        SetValue(DataContextProperty(), value);
        InvalidateVisual();
    }
    
    std::any GetDataContext() const {
        return GetValue<std::any>(DataContextProperty());
    }
    
    Derived* DataContext(const std::any& value) {
        SetDataContext(value);
        return static_cast<Derived*>(this);
    }
    
    std::any DataContext() const { return GetDataContext(); }

    // ========== 尺寸约束 ==========
    
    void SetWidth(float value) { SetValue(WidthProperty(), value); InvalidateMeasure(); }
    float GetWidth() const { return GetValue<float>(WidthProperty()); }
    Derived* Width(float value) { SetWidth(value); return static_cast<Derived*>(this); }
    float Width() const { return GetWidth(); }
    
    void SetHeight(float value) { SetValue(HeightProperty(), value); InvalidateMeasure(); }
    float GetHeight() const { return GetValue<float>(HeightProperty()); }
    Derived* Height(float value) { SetHeight(value); return static_cast<Derived*>(this); }
    float Height() const { return GetHeight(); }
    
    void SetMinWidth(float value) { SetValue(MinWidthProperty(), value); InvalidateMeasure(); }
    float GetMinWidth() const { return GetValue<float>(MinWidthProperty()); }
    Derived* MinWidth(float value) { SetMinWidth(value); return static_cast<Derived*>(this); }
    float MinWidth() const { return GetMinWidth(); }
    
    void SetMaxWidth(float value) { SetValue(MaxWidthProperty(), value); InvalidateMeasure(); }
    float GetMaxWidth() const { return GetValue<float>(MaxWidthProperty()); }
    Derived* MaxWidth(float value) { SetMaxWidth(value); return static_cast<Derived*>(this); }
    float MaxWidth() const { return GetMaxWidth(); }
    
    void SetMinHeight(float value) { SetValue(MinHeightProperty(), value); InvalidateMeasure(); }
    float GetMinHeight() const { return GetValue<float>(MinHeightProperty()); }
    Derived* MinHeight(float value) { SetMinHeight(value); return static_cast<Derived*>(this); }
    float MinHeight() const { return GetMinHeight(); }
    
    void SetMaxHeight(float value) { SetValue(MaxHeightProperty(), value); InvalidateMeasure(); }
    float GetMaxHeight() const { return GetValue<float>(MaxHeightProperty()); }
    Derived* MaxHeight(float value) { SetMaxHeight(value); return static_cast<Derived*>(this); }
    float MaxHeight() const { return GetMaxHeight(); }

    // ========== 布局属性 ==========
    
    void SetMargin(const Thickness& value) { SetValue(MarginProperty(), value); InvalidateMeasure(); }
    Thickness GetMargin() const override { return GetValue<Thickness>(MarginProperty()); }
    Derived* Margin(const Thickness& value) { SetMargin(value); return static_cast<Derived*>(this); }
    Derived* Margin(float uniform) { SetMargin(Thickness(uniform)); return static_cast<Derived*>(this); }
    Derived* Margin(float left, float top, float right, float bottom) { 
        SetMargin(Thickness(left, top, right, bottom)); 
        return static_cast<Derived*>(this); 
    }
    Thickness Margin() const { return GetMargin(); }
    
    void SetPadding(const Thickness& value) { SetValue(PaddingProperty(), value); InvalidateMeasure(); }
    Thickness GetPadding() const { return GetValue<Thickness>(PaddingProperty()); }
    Derived* Padding(const Thickness& value) { SetPadding(value); return static_cast<Derived*>(this); }
    Derived* Padding(float uniform) { SetPadding(Thickness(uniform)); return static_cast<Derived*>(this); }
    Derived* Padding(float left, float top, float right, float bottom) { 
        SetPadding(Thickness(left, top, right, bottom)); 
        return static_cast<Derived*>(this); 
    }
    Thickness Padding() const { return GetPadding(); }
    
    void SetHorizontalAlignment(HorizontalAlignment value) { 
        SetValue(HorizontalAlignmentProperty(), value); 
        InvalidateArrange(); 
    }
    HorizontalAlignment GetHorizontalAlignment() const { 
        return GetValue<ui::HorizontalAlignment>(HorizontalAlignmentProperty()); 
    }
    // 流式 API：使用 SetHorizontalAlignment 避免与枚举类型名称冲突
    Derived* SetHAlign(ui::HorizontalAlignment value) { 
        SetHorizontalAlignment(value); 
        return static_cast<Derived*>(this); 
    }
    
    void SetVerticalAlignment(VerticalAlignment value) { 
        SetValue(VerticalAlignmentProperty(), value); 
        InvalidateArrange(); 
    }
    VerticalAlignment GetVerticalAlignment() const { 
        return GetValue<ui::VerticalAlignment>(VerticalAlignmentProperty()); 
    }
    // 流式 API：使用 SetVAlign 避免与枚举类型名称冲突
    Derived* SetVAlign(ui::VerticalAlignment value) { 
        SetVerticalAlignment(value); 
        return static_cast<Derived*>(this); 
    }

    // ========== 样式和资源 ==========
    
    ResourceDictionary& GetResources() { return *resources_; }
    
    fk::ui::Style* GetStyle() const { return style_; }
    void SetStyle(fk::ui::Style* style);
    Derived* Style(fk::ui::Style* style) { SetStyle(style); return static_cast<Derived*>(this); }
    fk::ui::Style* Style() const { return GetStyle(); }

    // ========== 模板 ==========
    
    /**
     * @brief 应用模板
     */
    void ApplyTemplate();
    
    /**
     * @brief 模板应用钩子（派生类覆写）
     */
    virtual void OnApplyTemplate() {}

    // ========== 生命周期事件 ==========
    
    core::Event<> Loaded;
    core::Event<> Unloaded;
    core::Event<const std::any&, const std::any&> DataContextChanged;

protected:
    /**
     * @brief 数据上下文变更钩子
     */
    virtual void OnDataContextChanged(const std::any& oldValue, const std::any& newValue) {}
    
    /**
     * @brief 自定义测量逻辑
     */
    virtual Size MeasureOverride(const Size& availableSize) {
        return Size(0, 0);
    }
    
    /**
     * @brief 自定义排列逻辑
     */
    virtual Size ArrangeOverride(const Size& finalSize) {
        return finalSize;
    }
    
    /**
     * @brief 应用尺寸约束
     */
    Size ApplySizeConstraints(const Size& size) const {
        float w = size.width;
        float h = size.height;
        
        float width = GetWidth();
        float height = GetHeight();
        float minWidth = GetMinWidth();
        float maxWidth = GetMaxWidth();
        float minHeight = GetMinHeight();
        float maxHeight = GetMaxHeight();
        
        if (width > 0) w = width;
        if (height > 0) h = height;
        
        w = std::max(minWidth, std::min(maxWidth, w));
        h = std::max(minHeight, std::min(maxHeight, h));
        
        return Size(w, h);
    }

    // 覆写基类方法
    Size MeasureCore(const Size& availableSize) override {
        auto margin = GetMargin();
        auto padding = GetPadding();
        
        // 1. 减去 Margin 和 Padding
        float availWidth = std::max(0.0f, 
            availableSize.width - margin.left - margin.right - padding.left - padding.right);
        float availHeight = std::max(0.0f,
            availableSize.height - margin.top - margin.bottom - padding.top - padding.bottom);
        
        // 2. 应用尺寸约束
        auto constrainedSize = ApplySizeConstraints(Size(availWidth, availHeight));
        
        // 3. 调用派生类的测量逻辑
        auto desiredSize = MeasureOverride(constrainedSize);
        
        // 4. 加上 Padding（但不加 Margin！）
        // Margin 是外边距，由父容器处理
        // Padding 是内边距，由元素自己处理
        desiredSize.width += padding.left + padding.right;
        desiredSize.height += padding.top + padding.bottom;
        
        return desiredSize;
    }
    
    void ArrangeCore(const Rect& finalRect) override {
        auto padding = GetPadding();
        
        // 1. 减去 Padding（注意：Margin 已由父容器处理，不在此处减去）
        // finalRect 是父容器分配给我们的矩形，已经考虑了 Margin
        float arrangeWidth = std::max(0.0f,
            finalRect.width - padding.left - padding.right);
        float arrangeHeight = std::max(0.0f,
            finalRect.height - padding.top - padding.bottom);
        
        // 2. 获取期望尺寸（已经不含 Margin，但含 Padding）
        auto desiredSize = GetDesiredSize();
        float desiredWidth = std::max(0.0f, desiredSize.width - padding.left - padding.right);
        float desiredHeight = std::max(0.0f, desiredSize.height - padding.top - padding.bottom);
        
        // 3. 应用对齐方式
        auto hAlign = GetHorizontalAlignment();
        auto vAlign = GetVerticalAlignment();
        
        float finalWidth = arrangeWidth;
        float finalHeight = arrangeHeight;
        
        // 水平对齐
        if (hAlign != HorizontalAlignment::Stretch) {
            finalWidth = std::min(desiredWidth, arrangeWidth);
        }
        
        // 垂直对齐
        if (vAlign != VerticalAlignment::Stretch) {
            finalHeight = std::min(desiredHeight, arrangeHeight);
        }
        
        // 4. 调用派生类的排列逻辑，获取实际渲染尺寸
        auto actualSize = ArrangeOverride(Size(finalWidth, finalHeight));
        
        // 5. 设置渲染尺寸
        SetRenderSize(actualSize);
    }

private:
    std::unique_ptr<ResourceDictionary> resources_{std::make_unique<ResourceDictionary>()};
    fk::ui::Style* style_{nullptr};
    bool templateApplied_{false};
};

// 模板实现需要在头文件中
template<typename Derived>
void FrameworkElement<Derived>::SetStyle(fk::ui::Style* style) {
    if (style_ != style) {
        style_ = style;
        // TODO: 应用样式
    }
}

template<typename Derived>
void FrameworkElement<Derived>::ApplyTemplate() {
    if (!templateApplied_) {
        OnApplyTemplate();
        templateApplied_ = true;
    }
}

} // namespace fk::ui
