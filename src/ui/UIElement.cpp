#include "fk/ui/UIElement.h"
#include "fk/ui/Visual.h"
#include "fk/ui/Window.h"
#include "fk/ui/ThreadingConfig.h"
#include "fk/render/RenderHost.h"

#include <algorithm>
#include <stdexcept>
#include <any>
#include <iostream>

namespace fk::ui {

using binding::DependencyProperty;

// 增强的线程访问检查（支持不同模式）
static void VerifyAccessEnhanced(const DispatcherObject* obj) {
    auto& config = ThreadingConfig::Instance();
    
    if (!config.IsThreadCheckEnabled()) {
        return;  // 检查已禁用
    }
    
    if (obj->HasThreadAccess()) {
        return;  // 在正确的线程上
    }
    
    // 跨线程访问检测到
    auto mode = config.GetThreadCheckMode();
    
    if (mode == ThreadCheckMode::WarnOnly) {
        // 只警告
        std::cerr << "[WARNING] Cross-thread access detected on UIElement. "
                  << "Use Dispatcher::Invoke() or Dispatcher::InvokeAsync() for thread-safe calls." 
                  << std::endl;
        return;
    }
    
    // 默认：抛出异常（ThrowException 模式）
    throw std::runtime_error(
        "Cross-thread operation not allowed on UIElement. "
        "UI objects can only be accessed from the thread they were created on. "
        "Use Dispatcher::Invoke() or Dispatcher::InvokeAsync() for cross-thread calls."
    );
}

UIElement::UIElement() = default;

UIElement::~UIElement() = default;

const DependencyProperty& UIElement::VisibilityProperty() {
    static const DependencyProperty& property = DependencyProperty::Register(
        "Visibility",
        typeid(fk::ui::Visibility),
        typeid(UIElement),
        BuildVisibilityMetadata());
    return property;
}

const DependencyProperty& UIElement::IsEnabledProperty() {
    static const DependencyProperty& property = DependencyProperty::Register(
        "IsEnabled",
        typeid(bool),
        typeid(UIElement),
        BuildIsEnabledMetadata());
    return property;
}

const DependencyProperty& UIElement::OpacityProperty() {
    static const DependencyProperty& property = DependencyProperty::Register(
        "Opacity",
        typeid(float),
        typeid(UIElement),
        BuildOpacityMetadata());
    return property;
}

const DependencyProperty& UIElement::ClipToBoundsProperty() {
    static const DependencyProperty& property = DependencyProperty::Register(
        "ClipToBounds",
        typeid(bool),
        typeid(UIElement),
        BuildClipToBoundsMetadata());
    return property;
}

void UIElement::SetVisibility(fk::ui::Visibility visibility) {
    VerifyAccessEnhanced(this);
    SetValue(VisibilityProperty(), visibility);
}

Visibility UIElement::GetVisibility() const {
    return GetValue<fk::ui::Visibility>(VisibilityProperty());
}

void UIElement::SetIsEnabled(bool enabled) {
    VerifyAccessEnhanced(this);
    SetValue(IsEnabledProperty(), enabled);
}

bool UIElement::IsEnabled() const {
    return GetValue<bool>(IsEnabledProperty());
}

void UIElement::SetOpacity(float value) {
    VerifyAccessEnhanced(this);
    SetValue(OpacityProperty(), value);
}

float UIElement::GetOpacity() const {
    return GetValue<float>(OpacityProperty());
}

void UIElement::SetClipToBounds(bool clip) {
    VerifyAccessEnhanced(this);
    SetValue(ClipToBoundsProperty(), clip);
}

bool UIElement::GetClipToBounds() const {
    return GetValue<bool>(ClipToBoundsProperty());
}

Size UIElement::Measure(const Size& availableSize) {
    VerifyAccessEnhanced(this);

    Size finalAvailable = availableSize;
    const auto visibility = GetVisibility();
    if (visibility == Visibility::Collapsed) {
        desiredSize_ = Size{};
        isMeasureValid_ = true;
        return desiredSize_;
    }

    desiredSize_ = MeasureCore(finalAvailable);
    isMeasureValid_ = true;
    return desiredSize_;
}

void UIElement::Arrange(const Rect& finalRect) {
    VerifyAccessEnhanced(this);

    layoutSlot_ = finalRect;

    if (GetVisibility() == Visibility::Collapsed) {
        layoutSlot_.width = 0;
        layoutSlot_.height = 0;
        isArrangeValid_ = true;
        return;
    }

    ArrangeCore(finalRect);
    isArrangeValid_ = true;
    
    // 布局完成后触发重绘
    InvalidateVisual();
}

void UIElement::InvalidateMeasure() {
    VerifyAccessEnhanced(this);
    if (!isMeasureValid_) {
        return;
    }
    isMeasureValid_ = false;
    MeasureInvalidated(*this);
}

void UIElement::InvalidateArrange() {
    VerifyAccessEnhanced(this);
    if (!isArrangeValid_) {
        return;
    }
    isArrangeValid_ = false;
    ArrangeInvalidated(*this);
    
    // 🔥 向上传播失效标记,确保根元素也被标记为失效
    auto* current = this;
    while (current) {
        auto* parent = dynamic_cast<UIElement*>(current->GetLogicalParent());
        if (!parent) {
            // 到达根元素(Window)
            if (auto* window = dynamic_cast<Window*>(current)) {
                current->isArrangeValid_ = false;
            }
            break;
        }
        parent->isArrangeValid_ = false;
        current = parent;
    }
}

void UIElement::InvalidateVisual() {
    VerifyAccessEnhanced(this);
    
    // 获取 RenderHost 并通知它元素需要重绘
    auto* renderHost = GetRenderHost();
    if (renderHost) {
        renderHost->InvalidateElement(this);
    }
}

void UIElement::OnAttachedToLogicalTree() {
    DependencyObject::OnAttachedToLogicalTree();
    InvalidateMeasure();
    InvalidateArrange();
}

void UIElement::OnDetachedFromLogicalTree() {
    DependencyObject::OnDetachedFromLogicalTree();
    InvalidateMeasure();
    InvalidateArrange();
}

Size UIElement::MeasureCore(const Size& availableSize) {
    return Size{
        std::max(0.0f, availableSize.width),
        std::max(0.0f, availableSize.height)};
}

void UIElement::ArrangeCore(const Rect& finalRect) {
    layoutSlot_ = finalRect;
}

void UIElement::OnVisibilityChanged(fk::ui::Visibility oldValue, fk::ui::Visibility newValue) {
    if (newValue == fk::ui::Visibility::Collapsed) {
        InvalidateMeasure();
        InvalidateArrange();
    } else {
        InvalidateMeasure();
    }
    
    // 可见性变化影响渲染
    InvalidateVisual();
}

void UIElement::OnIsEnabledChanged(bool, bool) {
    // 启用状态变化可能影响外观
    InvalidateVisual();
}

void UIElement::OnOpacityChanged(float, float) {
    // 不透明度变化影响渲染
    InvalidateVisual();
}

void UIElement::OnClipToBoundsChanged(bool, bool) {
    // 裁切设置变化影响渲染
    InvalidateVisual();
}

// Visual 接口实现
Rect UIElement::GetRenderBounds() const {
    return layoutSlot_;
}

// GetOpacity() 和 GetVisibility() 已在上面实现,同时满足 UIElement 和 Visual 接口

std::vector<Visual*> UIElement::GetVisualChildren() const {
    // UIElement 基类没有子元素，由派生类重写
    return {};
}

bool UIElement::HasRenderContent() const {
    // 基类默认没有渲染内容，由派生类重写
    return false;
}

binding::PropertyMetadata UIElement::BuildVisibilityMetadata() {
    binding::PropertyMetadata metadata;
    metadata.defaultValue = Visibility::Visible;
    metadata.propertyChangedCallback = &UIElement::VisibilityPropertyChanged;
    return metadata;
}

binding::PropertyMetadata UIElement::BuildIsEnabledMetadata() {
    binding::PropertyMetadata metadata;
    metadata.defaultValue = true;
    metadata.propertyChangedCallback = &UIElement::IsEnabledPropertyChanged;
    return metadata;
}

binding::PropertyMetadata UIElement::BuildOpacityMetadata() {
    binding::PropertyMetadata metadata;
    metadata.defaultValue = 1.0f;
    metadata.propertyChangedCallback = &UIElement::OpacityPropertyChanged;
    metadata.validateCallback = &UIElement::ValidateOpacity;
    return metadata;
}

binding::PropertyMetadata UIElement::BuildClipToBoundsMetadata() {
    binding::PropertyMetadata metadata;
    metadata.defaultValue = false;
    metadata.propertyChangedCallback = &UIElement::ClipToBoundsPropertyChanged;
    return metadata;
}

void UIElement::VisibilityPropertyChanged(binding::DependencyObject& sender, const DependencyProperty&, const std::any& oldValue, const std::any& newValue) {
    auto* element = dynamic_cast<UIElement*>(&sender);
    if (!element) {
        return;
    }
    const auto oldVisibility = std::any_cast<fk::ui::Visibility>(oldValue);
    const auto newVisibility = std::any_cast<fk::ui::Visibility>(newValue);
    element->OnVisibilityChanged(oldVisibility, newVisibility);
}

void UIElement::IsEnabledPropertyChanged(binding::DependencyObject& sender, const DependencyProperty&, const std::any& oldValue, const std::any& newValue) {
    auto* element = dynamic_cast<UIElement*>(&sender);
    if (!element) {
        return;
    }
    const auto oldEnabled = std::any_cast<bool>(oldValue);
    const auto newEnabled = std::any_cast<bool>(newValue);
    element->OnIsEnabledChanged(oldEnabled, newEnabled);
}

void UIElement::OpacityPropertyChanged(binding::DependencyObject& sender, const DependencyProperty&, const std::any& oldValue, const std::any& newValue) {
    auto* element = dynamic_cast<UIElement*>(&sender);
    if (!element) {
        return;
    }
    const auto oldOpacity = std::any_cast<float>(oldValue);
    const auto newOpacity = std::any_cast<float>(newValue);
    element->OnOpacityChanged(oldOpacity, newOpacity);
}

void UIElement::ClipToBoundsPropertyChanged(binding::DependencyObject& sender, const DependencyProperty&, const std::any& oldValue, const std::any& newValue) {
    auto* element = dynamic_cast<UIElement*>(&sender);
    if (!element) {
        return;
    }
    const auto oldClip = std::any_cast<bool>(oldValue);
    const auto newClip = std::any_cast<bool>(newValue);
    element->OnClipToBoundsChanged(oldClip, newClip);
}

bool UIElement::ValidateOpacity(const std::any& value) {
    if (!value.has_value()) {
        return true;
    }
    if (value.type() != typeid(float)) {
        return false;
    }
    const auto opacity = std::any_cast<float>(value);
    return opacity >= 0.0f && opacity <= 1.0f;
}

render::RenderHost* UIElement::GetRenderHost() const {
    // 向上遍历逻辑树，找到 Window，然后获取其 RenderHost
    auto* current = const_cast<UIElement*>(this);
    while (current) {
        // 检查当前元素是否是 Window
        if (auto* window = dynamic_cast<Window*>(current)) {
            return window->GetRenderHost().get();
        }
        
        // 获取父级
        auto* parent = current->GetLogicalParent();
        if (!parent) {
            break;
        }
        
        current = dynamic_cast<UIElement*>(parent);
    }
    
    return nullptr;
}

// ============================================================================
// 鼠标事件处理 (默认实现 - 不处理，让子类重写)
// ============================================================================

void UIElement::OnMouseButtonDown(int button, double x, double y) {
    // 默认不处理
}

void UIElement::OnMouseButtonUp(int button, double x, double y) {
    // 默认不处理
}

void UIElement::OnMouseMove(double x, double y) {
    // 默认不处理
}

void UIElement::OnMouseWheel(double xoffset, double yoffset, double mouseX, double mouseY) {
    // 默认不处理,由子类重写
}

bool UIElement::HitTest(double x, double y) const {
    if (GetVisibility() != Visibility::Visible) {
        return false;
    }
    
    auto bounds = GetRenderBounds();
    return x >= bounds.x && x < bounds.x + bounds.width &&
           y >= bounds.y && y < bounds.y + bounds.height;
}

UIElement* UIElement::HitTestChildren(double x, double y) {
    // 基类默认没有子元素
    return nullptr;
}

} // namespace fk::ui
