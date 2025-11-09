#include "fk/ui/Control.h"

#include <utility>
#include <iostream>

namespace fk::ui::detail {

using binding::DependencyProperty;

ControlBase* ControlBase::focusedControl_ = nullptr;

ControlBase::ControlBase() = default;

ControlBase::~ControlBase() = default;

// ============================================================================
// 依赖属性定义（使用宏）
// ============================================================================

FK_DEPENDENCY_PROPERTY_DEFINE(ControlBase, IsFocused, bool, false)
FK_DEPENDENCY_PROPERTY_DEFINE(ControlBase, TabIndex, int, 0)
FK_DEPENDENCY_PROPERTY_DEFINE_REF(ControlBase, Cursor, std::string)
FK_DEPENDENCY_PROPERTY_DEFINE(ControlBase, Padding, fk::Thickness, fk::Thickness{})

// Content 属性（手动实现，因为需要特殊处理）
const DependencyProperty& ControlBase::ContentProperty() {
    static const auto& property = DependencyProperty::Register(
        "Content",
        typeid(std::shared_ptr<UIElement>),
        typeid(ControlBase),
        BuildContentMetadata());
    return property;
}

// Content 属性方法（手动实现）
void ControlBase::SetContent(std::shared_ptr<UIElement> content) {
    VerifyAccess();
    SetValue(ContentProperty(), std::move(content));
}

void ControlBase::ClearContent() {
    VerifyAccess();
    ClearValue(ContentProperty());
}

std::shared_ptr<UIElement> ControlBase::GetContent() const {
    return GetValue<std::shared_ptr<UIElement>>(ContentProperty());
}

// ============================================================================
// Template 支持
// ============================================================================

void ControlBase::SetTemplate(std::shared_ptr<ControlTemplate> controlTemplate) {
    if (template_ == controlTemplate) {
        return;
    }
    
    template_ = std::move(controlTemplate);
    templateApplied_ = false;
    
    // 自动应用模板
    ApplyTemplate();
}

bool ControlBase::ApplyTemplate() {
    if (!template_ || templateApplied_) {
        return templateApplied_;
    }
    
    // 从模板创建可视化树
    auto templateRoot = template_->LoadContent();
    if (!templateRoot) {
        return false;
    }
    
    // 将模板根设置为 Content
    SetContent(templateRoot);
    templateApplied_ = true;
    
    // 调用子类的回调
    OnApplyTemplate();
    
    return true;
}

bool ControlBase::Focus() {
    if (!GetIsEnabled()) {
        return false;
    }

    if (GetIsFocused()) {
        return true;
    }

    SetIsFocused(true);
    return GetIsFocused();
}

ControlBase* ControlBase::GetFocusedControl() {
    return focusedControl_;
}

// ============================================================================
// 生命周期
// ============================================================================

void ControlBase::OnAttachedToLogicalTree() {
    FrameworkElement::OnAttachedToLogicalTree();
    SyncContentAttachment();
}

void ControlBase::OnDetachedFromLogicalTree() {
    if (focusedControl_ == this) {
        SetIsFocused(false);
    }

    if (auto content = GetContent()) {
        DetachContent(content.get());
    }
    FrameworkElement::OnDetachedFromLogicalTree();
}

Size ControlBase::MeasureOverride(const Size& availableSize) {
    const auto& padding = GetPadding();
    const float paddingWidth = padding.left + padding.right;
    const float paddingHeight = padding.top + padding.bottom;

    if (const auto content = GetContent()) {
        // 测量内容时减去 Padding
        const Size childAvailable{
            std::max(0.0f, availableSize.width - paddingWidth),
            std::max(0.0f, availableSize.height - paddingHeight)
        };
        content->Measure(childAvailable);
        
        // 返回内容大小加上 Padding
        const auto& childDesired = content->DesiredSize();
        return Size{
            childDesired.width + paddingWidth,
            childDesired.height + paddingHeight
        };
    }
    
    // 没有内容时,只返回 Padding 大小
    return Size{paddingWidth, paddingHeight};
}

Size ControlBase::ArrangeOverride(const Size& finalSize) {
    if (const auto content = GetContent()) {
        const auto& padding = GetPadding();
        
        // 计算内容区域(减去 Padding)
        const float contentWidth = std::max(0.0f, finalSize.width - padding.left - padding.right);
        const float contentHeight = std::max(0.0f, finalSize.height - padding.top - padding.bottom);
        
        // 在 Padding 区域内排列内容
        content->Arrange(Rect{
            padding.left,
            padding.top,
            contentWidth,
            contentHeight
        });
    }
    return finalSize;
}

std::vector<Visual*> ControlBase::GetVisualChildren() const {
    std::vector<Visual*> children;
    if (auto content = GetContent()) {
        children.push_back(content.get());
    }
    return children;
}

bool ControlBase::OnMouseButtonDown(int button, double x, double y) {
    // 将事件传递给 Content
    auto content = GetContent();
    if (content && content->HitTest(x, y)) {
        auto bounds = content->GetRenderBounds();
        double localX = x - bounds.x;
        double localY = y - bounds.y;
        return content->OnMouseButtonDown(button, localX, localY);
    }
    return false;
}

bool ControlBase::OnMouseButtonUp(int button, double x, double y) {
    auto content = GetContent();
    if (content && content->HitTest(x, y)) {
        auto bounds = content->GetRenderBounds();
        double localX = x - bounds.x;
        double localY = y - bounds.y;
        return content->OnMouseButtonUp(button, localX, localY);
    }
    return false;
}

bool ControlBase::OnMouseMove(double x, double y) {
    auto content = GetContent();
    if (content && content->HitTest(x, y)) {
        auto bounds = content->GetRenderBounds();
        double localX = x - bounds.x;
        double localY = y - bounds.y;
        return content->OnMouseMove(localX, localY);
    }
    return false;
}

bool ControlBase::OnMouseWheel(double xoffset, double yoffset, double mouseX, double mouseY) {
    // 将滚轮事件传递给 Content
    auto content = GetContent();
    if (content && content->HitTest(mouseX, mouseY)) {
        return content->OnMouseWheel(xoffset, yoffset, mouseX, mouseY);
    }
    return false;
}

UIElement* ControlBase::HitTestChildren(double x, double y) {
    auto content = GetContent();
    if (content && content->GetVisibility() == Visibility::Visible) {
        if (content->HitTest(x, y)) {
            UIElement* hitInContent = content->HitTestChildren(x, y);
            return hitInContent ? hitInContent : content.get();
        }
    }
    return nullptr;
}

void ControlBase::OnContentChanged(UIElement*, UIElement*) {}

void ControlBase::OnIsFocusedChanged(bool /*oldValue*/, bool newValue) {
    if (newValue) {
        if (focusedControl_ && focusedControl_ != this) {
            focusedControl_->SetIsFocused(false);
        }
        focusedControl_ = this;
        OnFocusGained();
    } else {
        if (focusedControl_ == this) {
            focusedControl_ = nullptr;
        }
        OnFocusLost();
    }
}

void ControlBase::OnTabIndexChanged(int, int) {}

void ControlBase::OnCursorChanged(const std::string&, const std::string&) {}

void ControlBase::OnPaddingChanged(fk::Thickness, fk::Thickness) {}

void ControlBase::OnFocusGained() {}

void ControlBase::OnFocusLost() {}

// ============================================================================
// 元数据构建器
// ============================================================================

binding::PropertyMetadata ControlBase::BuildIsFocusedMetadata() {
    binding::PropertyMetadata metadata;
    metadata.defaultValue = false;
    metadata.propertyChangedCallback = &ControlBase::IsFocusedPropertyChanged;
    metadata.bindingOptions.defaultMode = binding::BindingMode::TwoWay;
    metadata.bindingOptions.updateSourceTrigger = binding::UpdateSourceTrigger::PropertyChanged;
    return metadata;
}

binding::PropertyMetadata ControlBase::BuildTabIndexMetadata() {
    binding::PropertyMetadata metadata;
    metadata.defaultValue = 0;
    metadata.propertyChangedCallback = &ControlBase::TabIndexPropertyChanged;
    metadata.validateCallback = &ControlBase::ValidateTabIndex;
    return metadata;
}

binding::PropertyMetadata ControlBase::BuildCursorMetadata() {
    binding::PropertyMetadata metadata;
    metadata.defaultValue = std::string{};
    metadata.propertyChangedCallback = &ControlBase::CursorPropertyChanged;
    metadata.validateCallback = &ControlBase::ValidateCursor;
    return metadata;
}

binding::PropertyMetadata ControlBase::BuildPaddingMetadata() {
    binding::PropertyMetadata metadata;
    metadata.defaultValue = fk::Thickness{};
    metadata.propertyChangedCallback = &ControlBase::PaddingPropertyChanged;
    return metadata;
}

// Content 属性元数据（手动实现）
binding::PropertyMetadata ControlBase::BuildContentMetadata() {
    binding::PropertyMetadata metadata;
    metadata.defaultValue = std::shared_ptr<UIElement>{};
    metadata.propertyChangedCallback = &ControlBase::ContentPropertyChanged;
    metadata.validateCallback = &ControlBase::ValidateContent;
    return metadata;
}

// ============================================================================
// 属性变更回调
// ============================================================================

// Content 属性回调（手动实现）
void ControlBase::ContentPropertyChanged(binding::DependencyObject& sender, const DependencyProperty&, const std::any& oldValue, const std::any& newValue) {
    auto* control = dynamic_cast<ControlBase*>(&sender);
    if (!control) {
        return;
    }

    const auto oldContent = ToElement(oldValue);
    const auto newContent = ToElement(newValue);

    if (oldContent && newContent && oldContent.get() == newContent.get()) {
        return;
    }

    if (oldContent) {
        control->DetachContent(oldContent.get());
    }

    if (newContent) {
        control->AttachContent(newContent.get());
    }

    control->OnContentChanged(oldContent.get(), newContent.get());
    control->InvalidateMeasure();
    control->InvalidateArrange();
}

// ============================================================================
// 验证回调
// ============================================================================

bool ControlBase::ValidateTabIndex(const std::any& value) {
    if (!value.has_value()) {
        return true;
    }
    if (value.type() != typeid(int)) {
        return false;
    }
    return std::any_cast<int>(value) >= 0;
}

bool ControlBase::ValidateCursor(const std::any& value) {
    if (!value.has_value()) {
        return true;
    }
    return value.type() == typeid(std::string);
}

bool ControlBase::ValidateContent(const std::any& value) {
    if (!value.has_value()) {
        return true;
    }
    return value.type() == typeid(std::shared_ptr<UIElement>);
}

std::shared_ptr<UIElement> ControlBase::ToElement(const std::any& value) {
    if (!value.has_value()) {
        return {};
    }
    if (value.type() != typeid(std::shared_ptr<UIElement>)) {
        return {};
    }
    return std::any_cast<std::shared_ptr<UIElement>>(value);
}

void ControlBase::AttachContent(UIElement* content) {
    if (!content) {
        return;
    }
    if (content->GetLogicalParent() == this) {
        if (IsAttachedToLogicalTree() && !content->IsAttachedToLogicalTree()) {
            AddLogicalChild(content);
        }
        return;
    }
    if (IsAttachedToLogicalTree()) {
        AddLogicalChild(content);
    } else {
        binding::DependencyObject::AddLogicalChild(content);
    }
}

void ControlBase::DetachContent(UIElement* content) {
    if (!content) {
        return;
    }
    if (content->GetLogicalParent() != this) {
        return;
    }
    RemoveLogicalChild(content);
}

void ControlBase::SyncContentAttachment() {
    if (const auto content = GetContent()) {
        AttachContent(content.get());
    }
}

} // namespace fk::ui::detail
