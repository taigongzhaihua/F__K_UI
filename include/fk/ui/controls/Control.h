#pragma once

#include "fk/ui/base/FrameworkElement.h"
#include "fk/ui/styling/Thickness.h"
#include "fk/ui/text/TextEnums.h"
#include "fk/ui/styling/Style.h"
#include "fk/ui/styling/ControlTemplate.h"
#include "fk/animation/VisualStateManager.h"
#include "fk/binding/DependencyProperty.h"
#include "fk/binding/BindingExpression.h"
#include <memory>
#include <string>

namespace fk::ui {

// 前向声明
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
    Control() {
        // 订阅Loaded事件以在控件加载时应用隐式样式
        this->Loaded += [this]() {
            this->OnLoaded();
        };
    }
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
    
    /**
     * @brief 控件模板依赖属性
     */
    static const binding::DependencyProperty& TemplateProperty();

    // ========== 控件样式 ==========
    
    fk::ui::Style* GetStyle() const {
        return this->template GetValue<fk::ui::Style*>(StyleProperty());
    }
    void SetStyle(fk::ui::Style* style) {
        this->SetValue(StyleProperty(), style);
    }
    Derived* StyleProperty(fk::ui::Style* style) {
        SetStyle(style);
        return static_cast<Derived*>(this);
    }
    fk::ui::Style* StyleValue() const { return GetStyle(); }
    
    /**
     * @brief 查找并应用隐式样式
     * 
     * 在控件初始化时自动调用，从ResourceDictionary中查找类型匹配的样式
     * 查找规则：
     * 1. 先在本地Resources中查找类型名称对应的样式
     * 2. 如果未找到，向上遍历可视树查找父元素的Resources
     * 3. 最后查找Application的Resources
     */
    void ApplyImplicitStyle() {
        // 如果已经显式设置了Style，则不应用隐式样式
        if (GetStyle() != nullptr) {
            return;
        }
        
        // 获取控件类型名称作为资源键
        std::string typeName = typeid(Derived).name();
        // 移除名称中的命名空间前缀，只保留类名
        auto pos = typeName.find_last_of(':');
        if (pos != std::string::npos) {
            typeName = typeName.substr(pos + 1);
        }
        
        // 查找隐式样式
        auto* implicitStyle = FindResource<fk::ui::Style*>(typeName);
        if (implicitStyle != nullptr && implicitStyle->IsApplicableTo(typeid(Derived))) {
            SetStyle(implicitStyle);
        }
    }
    
    /**
     * @brief 查找资源
     * 
     * 从当前元素的Resources开始，向上遍历可视树查找资源
     * 
     * @param key 资源键
     * @return 找到的资源，未找到返回默认值
     */
    template<typename T>
    T FindResource(const std::string& key) {
        // 1. 在自身Resources中查找
        auto& myResources = this->GetResources();
        if (myResources.Contains(key)) {
            return myResources.template Get<T>(key);
        }
        
        // 2. 向上遍历可视树查找父元素的Resources
        auto* parent = this->GetVisualParent();
        while (parent != nullptr) {
            // 尝试将parent转换为FrameworkElement以访问Resources
            // 使用基类类型而不是Derived，因为父节点可能是不同类型
            auto* frameworkParent = dynamic_cast<FrameworkElement<Derived>*>(parent);
            if (frameworkParent) {
                auto& parentResources = frameworkParent->GetResources();
                if (parentResources.Contains(key)) {
                    return parentResources.template Get<T>(key);
                }
            }
            parent = parent->GetVisualParent();
        }
        
        // 3. 查找Application的Resources（TODO: 需要Application单例支持）
        // if (Application::Current() != nullptr) {
        //     return Application::Current()->GetResources().template Get<T>(key);
        // }
        
        return T{};  // 未找到，返回默认值
    }
    
    // ========== 控件模板 ==========
    
    ControlTemplate* GetTemplate() const {
        return this->template GetValue<ControlTemplate*>(TemplateProperty());
    }
    void SetTemplate(ControlTemplate* tmpl) {
        this->SetValue(TemplateProperty(), tmpl);
    }
    Derived* Template(ControlTemplate* tmpl) {
        SetTemplate(tmpl);
        return static_cast<Derived*>(this);
    }
    ControlTemplate* TemplateValue() const { return GetTemplate(); }

    // ========== 外观属性 ==========
    
    // 使用 PropertyMacros 简化属性声明（从 45 行减少到 3 行）
    FK_PROPERTY_VISUAL(Foreground, Brush*, Derived)
    FK_PROPERTY_VISUAL(Background, Brush*, Derived)
    FK_PROPERTY_VISUAL(BorderBrush, Brush*, Derived)
    
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
    
    /**
     * @brief 检查此控件是否有自己的模板
     * Control 有模板时返回 true
     */
    bool HasOwnTemplate() const override {
        return GetTemplate() != nullptr;
    }

protected:
    /**
     * @brief 获取模板根元素
     */
    UIElement* GetTemplateRoot() const { return templateRoot_; }
    
    /**
     * @brief 模板应用钩子
     */
    void OnApplyTemplate() override {
        // 清除旧模板的VisualStateManager
        // 这确保派生类设置新模板时，会加载新模板的视觉状态，而不是继续使用基类模板的状态
        animation::VisualStateManager::SetVisualStateManager(this, nullptr);
        
        // 实例化并应用 ControlTemplate
        auto* tmpl = GetTemplate();
        if (tmpl && tmpl->IsValid()) {
            // 移除旧的模板根
            if (templateRoot_) {
                this->RemoveVisualChild(templateRoot_);
                templateRoot_ = nullptr;
            }
            
            // 实例化新模板
            templateRoot_ = tmpl->Instantiate(static_cast<UIElement*>(this));
            if (templateRoot_) {
                this->AddVisualChild(templateRoot_);
                this->TakeOwnership(templateRoot_);
            }
        }
        
        // 调用派生类的钩子（用于查找模板部件）
        OnTemplateApplied();
    }
    
    /**
     * @brief 模板应用后的钩子（派生类覆写以获取模板部件）
     */
    virtual void OnTemplateApplied() {
        // 派生类覆写以获取模板部件
    }
    
    // ========== 布局逻辑 ==========
    
    /**
     * @brief Control的测量逻辑：测量模板根元素
     */
    Size MeasureOverride(const Size& availableSize) override {
        // 确保模板已应用（在测量之前）
        auto* tmpl = GetTemplate();
        if (tmpl && tmpl->IsValid() && !templateRoot_) {
            this->ApplyTemplate();
        }
        
        if (templateRoot_) {
            templateRoot_->Measure(availableSize);
            return templateRoot_->GetDesiredSize();
        }
        return Size(0, 0);
    }
    
    /**
     * @brief Control的排列逻辑：排列模板根元素
     */
    Size ArrangeOverride(const Size& finalSize) override {
        if (templateRoot_) {
            templateRoot_->Arrange(Rect(0, 0, finalSize.width, finalSize.height));
            return finalSize;
        }
        return Size(0, 0);
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
    
    /**
     * @brief 控件加载时的钩子函数
     * 
     * 在控件添加到可视树并完成初始化后调用
     * 用于应用隐式样式和模板
     */
    virtual void OnLoaded() {
        ApplyImplicitStyle();
        ApplyImplicitTemplate();
    }
    
    /**
     * @brief Template属性变更回调
     */
    static void OnTemplateChanged(
        binding::DependencyObject& d,
        const binding::DependencyProperty& prop,
        const std::any& oldValue,
        const std::any& newValue
    );
    
    /**
     * @brief 应用隐式模板
     * 
     * 如果控件没有显式设置模板，尝试从默认样式或Resources中查找
     */
    void ApplyImplicitTemplate() {
        // 如果已经有显式模板，不需要应用隐式模板
        if (GetTemplate() != nullptr) {
            return;
        }
        
        // TODO: 从Style中获取模板
        // 如果Style包含Template的Setter，它会在Style.Apply时自动设置
        
        // TODO: 从Resources中查找默认模板
        // std::string templateKey = GetDefaultStyleKey().name() + "Template";
        // auto* implicitTemplate = FindResource<ControlTemplate*>(templateKey);
        // if (implicitTemplate) {
        //     SetTemplate(implicitTemplate);
        // }
    }
    
    // ========== 裁剪系统 ==========
    
    /**
     * @brief Control启用裁剪以调试问题
     */
    bool ShouldClipToBounds() const override {
        return true;  // 临时启用以调试
    }

private:
    // 状态（非依赖属性，用于内部状态跟踪）
    bool isFocused_{false};
    bool isMouseOver_{false};
    
    // 模板根元素
    UIElement* templateRoot_{nullptr};
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
const binding::DependencyProperty& Control<Derived>::TemplateProperty() {
    static auto& property = binding::DependencyProperty::Register(
        "Template",
        typeid(ControlTemplate*),
        typeid(Control<Derived>),
        binding::PropertyMetadata{
            std::any(static_cast<ControlTemplate*>(nullptr)),
            &Control<Derived>::OnTemplateChanged
        }
    );
    return property;
}

template<typename Derived>
void Control<Derived>::OnTemplateChanged(
    binding::DependencyObject& d,
    const binding::DependencyProperty& prop,
    const std::any& oldValue,
    const std::any& newValue
) {
    auto* control = dynamic_cast<Control<Derived>*>(&d);
    if (!control) {
        return;
    }
    
    // 当模板改变时，标记需要重新应用模板
    // ApplyTemplate() 将在下次测量时被调用
    control->templateApplied_ = false;
    
    // 触发视觉更新
    control->InvalidateVisual();
    control->InvalidateArrange();
}

} // namespace fk::ui
