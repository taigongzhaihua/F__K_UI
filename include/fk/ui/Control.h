#pragma once

#include "fk/ui/UIElement.h"
#include "fk/ui/View.h"
#include "fk/ui/BindingMacros.h"
#include "fk/ui/DependencyPropertyMacros.h"
#include "fk/ui/Template.h"

#include <any>
#include <memory>
#include <string>
#include <utility>

namespace fk::ui {

class UIElement;

namespace detail {

class ControlBase : public FrameworkElement {
public:
    using FrameworkElement::FrameworkElement;

    ControlBase();
    ~ControlBase() override;

    // 依赖属性（使用宏）
    FK_DEPENDENCY_PROPERTY_DECLARE(IsFocused, bool)
    FK_DEPENDENCY_PROPERTY_DECLARE(TabIndex, int)
    FK_DEPENDENCY_PROPERTY_DECLARE_REF(Cursor, std::string)
    FK_DEPENDENCY_PROPERTY_DECLARE(Padding, fk::Thickness)

public:
    // Content 属性（特殊处理，保留原有方法）
    static const binding::DependencyProperty& ContentProperty();
    void SetContent(std::shared_ptr<UIElement> content);
    void ClearContent();
    [[nodiscard]] std::shared_ptr<UIElement> GetContent() const;
    [[nodiscard]] bool HasContent() const { return GetContent() != nullptr; }

    // 🎯 Template 支持
    void SetTemplate(std::shared_ptr<ControlTemplate> controlTemplate);
    [[nodiscard]] std::shared_ptr<ControlTemplate> GetTemplate() const { return template_; }
    
    /**
     * @brief 应用控件模板
     * 从模板创建可视化树并替换当前 Content
     * @return 是否成功应用模板
     */
    bool ApplyTemplate();
    
    /**
     * @brief 模板应用完成后的回调
     * 子类重写此方法以获取模板中的命名部件
     */
    virtual void OnApplyTemplate() {}

    bool Focus();
    [[nodiscard]] bool HasFocus() const { return GetIsFocused(); }
    static ControlBase* GetFocusedControl();

protected:
    void OnAttachedToLogicalTree() override;
    void OnDetachedFromLogicalTree() override;

    Size MeasureOverride(const Size& availableSize) override;
    Size ArrangeOverride(const Size& finalSize) override;
    
    // 重写以返回 Content 作为子元素
    std::vector<Visual*> GetVisualChildren() const override;
    
    // 重写鼠标事件,传递给 Content
    bool OnMouseButtonDown(int button, double x, double y) override;
    bool OnMouseButtonUp(int button, double x, double y) override;
    bool OnMouseMove(double x, double y) override;
    bool OnMouseWheel(double xoffset, double yoffset, double mouseX, double mouseY) override;
    
    // 命中测试重写
    UIElement* HitTestChildren(double x, double y) override;

    virtual void OnContentChanged(UIElement* oldContent, UIElement* newContent);
    virtual void OnFocusGained();
    virtual void OnFocusLost();

private:
    // Content 属性元数据
    static binding::PropertyMetadata BuildContentMetadata();
    static void ContentPropertyChanged(binding::DependencyObject& sender, const binding::DependencyProperty& property,
        const std::any& oldValue, const std::any& newValue);

    static ControlBase* focusedControl_;

    static bool ValidateTabIndex(const std::any& value);
    static bool ValidateCursor(const std::any& value);
    static bool ValidateContent(const std::any& value);

    static std::shared_ptr<UIElement> ToElement(const std::any& value);

    void AttachContent(UIElement* content);
    void DetachContent(UIElement* content);
    void SyncContentAttachment();
    
    std::shared_ptr<ControlTemplate> template_;  // 🎯 控件模板
    bool templateApplied_{false};                 // 🎯 模板是否已应用
};

} // namespace detail

template <typename Derived>
class Control : public View<Derived, detail::ControlBase> {
public:
    using Base = View<Derived, detail::ControlBase>;
    using ControlBase = detail::ControlBase;
    using Ptr = typename Base::Ptr;
    using ContentPtr = std::shared_ptr<UIElement>;

    using Base::Base;

    // 🎯 使用宏简化绑定支持
    FK_BINDING_PROPERTY_VALUE_BASE(IsFocused, bool, ControlBase)
    FK_BINDING_PROPERTY_VALUE_BASE(TabIndex, int, ControlBase)
    FK_BINDING_PROPERTY_BASE(Cursor, std::string, ControlBase)
    
    // Content 属性 - 需要特殊处理（因为类型是 shared_ptr）
    [[nodiscard]] ContentPtr Content() const {
        return this->GetContent();
    }
    
    Ptr Content(ContentPtr content) {
        this->SetContent(std::move(content));
        return this->Self();
    }
    
    Ptr Content(binding::Binding binding) {
        this->SetBinding(ControlBase::ContentProperty(), std::move(binding));
        return this->Self();
    }

    Ptr ClearContentValue() {
        static_cast<ControlBase*>(this)->ClearContent();
        return this->Self();
    }

    // Padding 属性 - 支持多种重载
    [[nodiscard]] const fk::Thickness& Padding() const {
        return this->GetPadding();
    }

    Ptr Padding(const fk::Thickness& padding) {
        this->SetPadding(padding);
        return this->Self();
    }

    Ptr Padding(float uniform) {
        this->SetPadding(fk::Thickness{uniform});
        return this->Self();
    }

    Ptr Padding(float horizontal, float vertical) {
        this->SetPadding(fk::Thickness{horizontal, vertical});
        return this->Self();
    }

    Ptr Padding(float left, float top, float right, float bottom) {
        this->SetPadding(fk::Thickness{left, top, right, bottom});
        return this->Self();
    }
    
    Ptr Padding(binding::Binding binding) {
        this->SetBinding(ControlBase::PaddingProperty(), std::move(binding));
        return this->Self();
    }

    // 🎯 Template 属性
    [[nodiscard]] std::shared_ptr<ControlTemplate> Template() const {
        return static_cast<const ControlBase*>(this)->GetTemplate();
    }
    
    Ptr Template(std::shared_ptr<ControlTemplate> controlTemplate) {
        static_cast<ControlBase*>(this)->SetTemplate(std::move(controlTemplate));
        return this->Self();
    }

};

} // namespace fk::ui
