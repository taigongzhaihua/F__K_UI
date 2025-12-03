#include "fk/ui/base/UIElement.h"
#include "fk/ui/input/NameScope.h"
#include "fk/ui/input/InputManager.h"
#include "fk/ui/Window.h"
#include "fk/render/RenderContext.h"
#include <algorithm>
#include <iostream>

namespace fk::ui {

// 依赖属性注�?
const binding::DependencyProperty& UIElement::VisibilityProperty() {
    static auto& property = binding::DependencyProperty::Register(
        "Visibility",
        typeid(Visibility),
        typeid(UIElement),
        binding::PropertyMetadata{Visibility::Visible}
    );
    return property;
}

const binding::DependencyProperty& UIElement::IsEnabledProperty() {
    static auto& property = binding::DependencyProperty::Register(
        "IsEnabled",
        typeid(bool),
        typeid(UIElement),
        binding::PropertyMetadata{true}
    );
    return property;
}

const binding::DependencyProperty& UIElement::OpacityProperty() {
    static auto& property = binding::DependencyProperty::Register(
        "Opacity",
        typeid(float),
        typeid(UIElement),
        binding::PropertyMetadata(
            1.0f,
            [](binding::DependencyObject& d, const binding::DependencyProperty& prop, const std::any& oldValue, const std::any& newValue) {
                if (auto* element = dynamic_cast<UIElement*>(&d)) {
                    element->InvalidateVisual();
                }
            }
        )
    );
    return property;
}

const binding::DependencyProperty& UIElement::ClipProperty() {
    static auto& property = binding::DependencyProperty::Register(
        "Clip",
        typeid(Rect),
        typeid(UIElement),
        binding::PropertyMetadata{Rect(0, 0, 0, 0)}
    );
    return property;
}

const binding::DependencyProperty& UIElement::RenderTransformProperty() {
    static auto& property = binding::DependencyProperty::Register(
        "RenderTransform",
        typeid(Transform*),
        typeid(UIElement),
        binding::PropertyMetadata{static_cast<Transform*>(nullptr)}
    );
    return property;
}

UIElement::UIElement() 
    : desiredSize_(0, 0)
    , renderSize_(0, 0)
    , layoutRect_(0, 0, 0, 0)
    , measureDirty_(true)
    , arrangeDirty_(true) {
    // 初始化默认�?
    SetValue(VisibilityProperty(), Visibility::Visible);
    SetValue(IsEnabledProperty(), true);
    SetValue(OpacityProperty(), 1.0f);
}

UIElement::~UIElement() {
    // 释放所有指针捕获，防止InputManager持有悬空指针
    // 注意：通常只会捕获pointerId=0（主指针�?
    // 如果有更多的pointerId被捕获，它们也应该在控件逻辑中被显式释放
    constexpr int MAX_COMMON_POINTER_IDS = 10; // 支持最�?0个触控点
    for (int pointerId = 0; pointerId < MAX_COMMON_POINTER_IDS; ++pointerId) {
        if (HasPointerCapture(pointerId)) {
            ReleasePointerCapture(pointerId);
        }
    }
}

void UIElement::SetName(const std::string& name) {
    std::string oldName = GetElementName();
    SetElementName(name);
    
    // 查找最近的 NameScope 并更新名称索�?
    NameScope* scope = FindNearestNameScope();
    if (scope) {
        scope->UpdateName(oldName, name, this);
    }
}

void UIElement::Measure(const Size& availableSize) {
    if (!measureDirty_ && desiredSize_.width > 0 && desiredSize_.height > 0) {
        return; // 已经测量过且有效
    }
    
    auto visibility = GetValue<Visibility>(VisibilityProperty());
    if (visibility == Visibility::Collapsed) {
        desiredSize_ = Size(0, 0);
        measureDirty_ = false;
        return;
    }
    
    desiredSize_ = MeasureCore(availableSize);
    measureDirty_ = false;
}

void UIElement::Arrange(const Rect& finalRect) {
    // 检查是否需要重新排�?
    // 注意：如�?arrangeDirty_ �?true，即使位置没变也需要重新排�?
    //       因为子元素可能需要重新排�?
    bool rectChanged = (layoutRect_.x != finalRect.x || 
                       layoutRect_.y != finalRect.y ||
                       layoutRect_.width != finalRect.width ||
                       layoutRect_.height != finalRect.height);
    
    // 只有当既不脏也不需要位置更新时才跳�?
    if (!arrangeDirty_ && !measureDirty_ && !rectChanged) {
        return; // 已经排列过且位置没有改变，且不需要重新排列子元素
    }
    
    auto visibility = GetValue<Visibility>(VisibilityProperty());
    if (visibility == Visibility::Collapsed) {
        renderSize_ = Size(0, 0);
        layoutRect_ = Rect(0, 0, 0, 0);
        arrangeDirty_ = false;
        return;
    }
    
    // 存储布局矩形
    layoutRect_ = finalRect;
    
    // ArrangeCore 负责设置 renderSize_ 并排列子元素
    ArrangeCore(finalRect);
    arrangeDirty_ = false;
}

void UIElement::InvalidateMeasure() {
    measureDirty_ = true;
    arrangeDirty_ = true;
    
    // 向上传播使父节点也失�?
    if (auto* parent = GetVisualParent()) {
        if (auto* parentElement = dynamic_cast<UIElement*>(parent)) {
            parentElement->InvalidateMeasure();
        }
    }
}

void UIElement::InvalidateArrange() {
    arrangeDirty_ = true;
    
    // 向上传播
    if (auto* parent = GetVisualParent()) {
        if (auto* parentElement = dynamic_cast<UIElement*>(parent)) {
            parentElement->InvalidateArrange();
        }
    }
}

void UIElement::SetVisibility(Visibility value) {
    SetValue(VisibilityProperty(), value);
    InvalidateMeasure();
    InvalidateVisual();
}

Visibility UIElement::GetVisibility() const {
    return GetValue<Visibility>(VisibilityProperty());
}

void UIElement::SetIsEnabled(bool value) {
    SetValue(IsEnabledProperty(), value);
    InvalidateVisual();
}

bool UIElement::GetIsEnabled() const {
    return GetValue<bool>(IsEnabledProperty());
}

void UIElement::SetOpacity(float value) {
    // 限制�?0.0 �?1.0 范围�?
    if (value < 0.0f) value = 0.0f;
    if (value > 1.0f) value = 1.0f;
    SetValue(OpacityProperty(), value);
    InvalidateVisual();
}

float UIElement::GetOpacity() const {
    return GetValue<float>(OpacityProperty());
}

void UIElement::SetClip(const Rect& value) {
    SetValue(ClipProperty(), value);
    InvalidateVisual();
}

Rect UIElement::GetClip() const {
    return GetValue<Rect>(ClipProperty());
}

bool UIElement::HasClip() const {
    Rect clip = GetClip();
    return clip.width > 0 && clip.height > 0;
}

Thickness UIElement::GetMargin() const {
    return Thickness(0);
}

HorizontalAlignment UIElement::GetHorizontalAlignment() const {
    return HorizontalAlignment::Stretch;  // 默认拉伸
}

VerticalAlignment UIElement::GetVerticalAlignment() const {
    return VerticalAlignment::Stretch;  // 默认拉伸
}

void UIElement::SetRenderTransform(Transform* value) {
    SetValue(RenderTransformProperty(), value);
    InvalidateVisual();
}

Transform* UIElement::GetRenderTransform() const {
    return GetValue<Transform*>(RenderTransformProperty());
}

void UIElement::RaiseEvent(RoutedEventArgs& args) {
    if (!args.source) {
        args.source = this;
    }
    
    if (args.handled) {
        return;
    }
    
    // 根据路由策略处理事件传播
    // 注意：完整实现需�?RoutedEvent 参数，这里简化处�?
    
    // 1. 触发本地处理器（Direct 策略�?
    // 由于当前设计�?RaiseEvent 不接�?RoutedEvent 参数�?
    // 简化为直接触发所有注册的处理�?
    for (auto& pair : eventHandlers_) {
        if (args.handled) break;
        
        auto strategy = pair.first->GetStrategy();
        
        // 对于 Direct 策略，只在当前元素触�?
        if (strategy == RoutedEvent::RoutingStrategy::Direct) {
            for (auto& handler : pair.second) {
                handler(this, args);
                if (args.handled) break;
            }
        }
        // Bubble 策略会向上传播（在这里触发，然后向父节点传播�?
        else if (strategy == RoutedEvent::RoutingStrategy::Bubble) {
            for (auto& handler : pair.second) {
                handler(this, args);
                if (args.handled) break;
            }
        }
        // Tunnel 策略应该从根节点向下传播（需要特殊处理）
        // 当前简化实现不完全支持 Tunnel
    }
    
    // 2. 冒泡到父元素（Bubble 策略�?
    if (!args.handled) {
        if (auto* parent = GetVisualParent()) {
            if (auto* parentElement = dynamic_cast<UIElement*>(parent)) {
                parentElement->RaiseEvent(args);
            }
        }
    }
}

void UIElement::AddHandler(RoutedEvent* routedEvent, EventHandler handler) {
    if (routedEvent) {
        eventHandlers_[routedEvent].push_back(handler);
    }
}

void UIElement::RemoveHandler(RoutedEvent* routedEvent, EventHandler handler) {
    if (routedEvent) {
        auto it = eventHandlers_.find(routedEvent);
        if (it != eventHandlers_.end()) {
            auto& handlers = it->second;
            // 注意：std::function 没有直接的比较操作符
            // 这里简化实现：移除所有匹配的处理�?
            // 实际使用中可以考虑使用句柄�?ID 来标识处理器
            handlers.erase(
                std::remove_if(handlers.begin(), handlers.end(),
                    [&](const EventHandler& h) {
                        // 无法直接比较 std::function，这里是占位实现
                        // 实际项目中应该使�?EventToken 或其他机�?
                        return false; // 暂时不移�?
                    }),
                handlers.end()
            );
        }
    }
}

void UIElement::OnPointerPressed(PointerEventArgs& e) {
    // 默认不处�?
}

void UIElement::OnPointerReleased(PointerEventArgs& e) {
    // 默认不处�?
}

void UIElement::OnPointerMoved(PointerEventArgs& e) {
    // 默认不处�?
}

void UIElement::OnPointerEntered(PointerEventArgs& e) {
    // 默认不处�?
}

void UIElement::OnPointerExited(PointerEventArgs& e) {
    // 默认不处�?
}

void UIElement::OnMouseWheel(PointerEventArgs& e) {
    // 默认不处�?
}

void UIElement::OnKeyDown(KeyEventArgs& e) {
    // 默认不处�?
}

void UIElement::OnKeyUp(KeyEventArgs& e) {
    // 默认不处�?
}

std::vector<UIElement*> UIElement::GetLogicalChildren() const {
    // 默认实现：非容器元素返回空列�?
    return {};
}

UIElement* UIElement::Clone() const {
    // 默认实现：创建基�?UIElement 副本
    auto* clone = new UIElement();
    
    // 克隆基本属�?
    clone->SetName(GetElementName());
    clone->SetVisibility(GetVisibility());
    clone->SetIsEnabled(GetIsEnabled());
    clone->SetOpacity(GetOpacity());
    
    // 注意：不克隆 templatedParent_，因为克隆体会设置新的父元素
    // 注意：不克隆子元素，因为基类 UIElement 没有子元�?
    
    return clone;
}

Size UIElement::MeasureCore(const Size& availableSize) {
    // 默认实现：返回零尺寸
    return Size(0, 0);
}

void UIElement::ArrangeCore(const Rect& finalRect) {
    // 默认实现：使用最终矩形的尺寸
    SetRenderSize(Size(finalRect.width, finalRect.height));
}

void UIElement::TakeOwnership(UIElement* child) {
    if (child) {
        ownedChildren_.emplace_back(child);
    }
}

void UIElement::OnRender(render::RenderContext& /*context*/) {
    // 默认不绘制，由派生类实现
}

std::optional<ui::Rect> UIElement::DetermineClipRegion() const {
    // 优先�?：显式ClipProperty
    if (HasClip()) {
        return GetClip();
    }
    
    // 优先�?：容器自动裁�?
    if (ShouldClipToBounds()) {
        return CalculateClipBounds();
    }
    
    // 不需要裁�?
    return std::nullopt;
}

void UIElement::CollectDrawCommands(render::RenderContext& context) {
    // 检查可见�?
    auto visibility = GetVisibility();
    if (visibility == Visibility::Collapsed || visibility == Visibility::Hidden) {
        return;  // 不渲染不可见或折叠的元素
    }
    
    // 推入布局偏移
    context.PushTransform(layoutRect_.x, layoutRect_.y);

    // 应用不透明度（Opacity属性）
    float opacity = GetOpacity();
    bool hasOpacity = (opacity < 1.0f);
    if (hasOpacity) {
        context.PushLayer(opacity);
    }

    // TODO: 应用渲染变换（RenderTransform属性）
    // Transform* renderTransform = GetRenderTransform();
    // if (renderTransform) {
    //     // 需要RenderContext支持任意变换矩阵
    //     // context.PushTransform(renderTransform->GetMatrix());
    // }

    // 绘制自身内容（不受裁剪影响）
    OnRender(context);

    // 统一的裁剪处理（新设计）- 只影响子元素
    auto clipRegion = DetermineClipRegion();
    if (clipRegion.has_value()) {
        context.PushClip(*clipRegion);
    }

    // 收集子元素绘制命�?
    Visual::CollectDrawCommands(context);

    // 弹出裁剪区域
    if (clipRegion.has_value()) {
        context.PopClip();
    }

    // 弹出不透明度层
    if (hasOpacity) {
        context.PopLayer();
    }

    // 弹出变换
    context.PopTransform();
}

UIElement* UIElement::FindName(const std::string& name) {
    if (name.empty()) {
        return nullptr;
    }
    
    // 检查当前元素的名称（使用DependencyObject的elementName_�?
    if (GetElementName() == name) {
        return this;
    }
    
    // 递归搜索所有逻辑子元�?
    for (UIElement* child : GetLogicalChildren()) {
        if (child) {
            UIElement* found = child->FindName(name);
            if (found) {
                return found;
            }
        }
    }
    
    return nullptr;
}

void UIElement::CreateNameScope() {
    if (!nameScope_) {
        nameScope_ = std::make_unique<NameScope>();
        
        // 注册当前元素的名�?
        const std::string& name = GetElementName();
        if (!name.empty()) {
            nameScope_->RegisterName(name, this);
        }
        
        // 递归注册所有现有子元素的名�?
        RegisterNamesToScope(this, nameScope_.get());
    }
}

NameScope* UIElement::GetNameScope() const {
    return nameScope_.get();
}

NameScope* UIElement::FindNearestNameScope() const {
    // 从当前元素开始向上查�?
    const UIElement* current = this;
    while (current) {
        if (current->nameScope_) {
            return current->nameScope_.get();
        }
        
        // 向上到逻辑父元�?
        auto* parent = current->GetLogicalParent();
        current = dynamic_cast<const UIElement*>(parent);
    }
    
    return nullptr;
}

UIElement* UIElement::FindNameFast(const std::string& name) {
    if (name.empty()) {
        return nullptr;
    }
    
    // 策略1：尝试使用最近的 NameScope（O(1)�?
    NameScope* scope = FindNearestNameScope();
    if (scope) {
        auto* found = scope->FindName(name);
        if (found) {
            // 找到了，�?DependencyObject 转换�?UIElement
            return dynamic_cast<UIElement*>(found);
        }
    }
    
    // 策略2：回退到递归查找（O(n)�?
    return FindName(name);
}

// 辅助方法：递归注册子元素名称到 NameScope
void UIElement::RegisterNamesToScope(UIElement* element, NameScope* scope) {
    if (!element || !scope) {
        return;
    }
    
    // 注册当前元素
    const std::string& name = element->GetElementName();
    if (!name.empty()) {
        scope->RegisterName(name, element);
    }
    
    // 递归注册子元�?
    for (auto* child : element->GetLogicalChildren()) {
        if (auto* uiChild = dynamic_cast<UIElement*>(child)) {
            RegisterNamesToScope(uiChild, scope);
        }
    }
}

// ========== Grid 附加属性流式方法实�?==========

UIElement* UIElement::Row(int row) {
    // 前向声明，实际调�?Grid::SetRow
    // 为避免循环依赖，在头文件中包�?Grid.h
    extern void SetGridRow(UIElement* element, int row);
    SetGridRow(this, row);
    return this;
}

UIElement* UIElement::Column(int col) {
    extern void SetGridColumn(UIElement* element, int col);
    SetGridColumn(this, col);
    return this;
}

UIElement* UIElement::RowSpan(int span) {
    extern void SetGridRowSpan(UIElement* element, int span);
    SetGridRowSpan(this, span);
    return this;
}

UIElement* UIElement::ColumnSpan(int span) {
    extern void SetGridColumnSpan(UIElement* element, int span);
    SetGridColumnSpan(this, span);
    return this;
}

void UIElement::SetTemplatedParent(UIElement* parent) {
    auto oldParent = templatedParent_;
    templatedParent_ = parent;
    
    // 当设�?TemplatedParent 后，需要刷新所�?TemplateBinding
    // 触发 DataContext Changed 事件会让所有绑定重新订阅源
    if (oldParent != parent && parent != nullptr) {
        // 触发一个假�?DataContextChanged 来让绑定重新订阅
        // 这会导致 BindingExpression::RefreshSourceSubscription() 被调�?
        auto dummyOld = std::any{};
        auto dummyNew = std::any{};
        DataContextChanged(dummyOld, dummyNew);
    }
}

// ========== 指针捕获 ==========

bool UIElement::CapturePointer(int pointerId) {
    InputManager* inputManager = GetInputManager();
    if (inputManager) {
        inputManager->CapturePointer(this, pointerId);
        return true;
    }
    return false;
}

void UIElement::ReleasePointerCapture(int pointerId) {
    InputManager* inputManager = GetInputManager();
    if (inputManager) {
        inputManager->ReleasePointerCapture(pointerId);
    }
}

bool UIElement::HasPointerCapture(int pointerId) const {
    InputManager* inputManager = GetInputManager();
    if (inputManager) {
        return inputManager->GetPointerCapture(pointerId) == this;
    }
    return false;
}

InputManager* UIElement::GetInputManager() const {
    // 向上遍历视觉树，找到Window
    // 注意：使用const_cast是因为Visual::GetVisualParent()返回非const指针
    // 但这个遍历操作本身不会修改任何对�?
    const Visual* current = this;
    while (current) {
        // 尝试转换为Window
        if (const auto* window = dynamic_cast<const Window*>(current)) {
            // GetInputManager()返回非const指针是合理的，因为我们需要调用捕�?释放操作
            return const_cast<Window*>(window)->GetInputManager();
        }
        current = current->GetVisualParent();
    }
    return nullptr;
}

} // namespace fk::ui
