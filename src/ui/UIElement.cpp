#include "fk/ui/UIElement.h"
#include "fk/render/RenderContext.h"
#include <algorithm>

namespace fk::ui {

// 依赖属性注册
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
        binding::PropertyMetadata{1.0f}
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
    // 初始化默认值
    SetValue(VisibilityProperty(), Visibility::Visible);
    SetValue(IsEnabledProperty(), true);
    SetValue(OpacityProperty(), 1.0f);
}

UIElement::~UIElement() = default;

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
    // 检查是否需要重新排列
    // 注意：即使元素不脏，如果位置改变了也需要更新
    bool rectChanged = (layoutRect_.x != finalRect.x || 
                       layoutRect_.y != finalRect.y ||
                       layoutRect_.width != finalRect.width ||
                       layoutRect_.height != finalRect.height);
    
    if (!arrangeDirty_ && !measureDirty_ && !rectChanged) {
        return; // 已经排列过且位置没有改变
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
    
    // ArrangeCore 负责设置 renderSize_
    ArrangeCore(finalRect);
    arrangeDirty_ = false;
}

void UIElement::InvalidateMeasure() {
    measureDirty_ = true;
    arrangeDirty_ = true;
    
    // 向上传播使父节点也失效
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
    // 限制在 0.0 到 1.0 范围内
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
    // 注意：完整实现需要 RoutedEvent 参数，这里简化处理
    
    // 1. 触发本地处理器（Direct 策略）
    // 由于当前设计中 RaiseEvent 不接受 RoutedEvent 参数，
    // 简化为直接触发所有注册的处理器
    for (auto& pair : eventHandlers_) {
        if (args.handled) break;
        
        auto strategy = pair.first->GetStrategy();
        
        // 对于 Direct 策略，只在当前元素触发
        if (strategy == RoutedEvent::RoutingStrategy::Direct) {
            for (auto& handler : pair.second) {
                handler(this, args);
                if (args.handled) break;
            }
        }
        // Bubble 策略会向上传播（在这里触发，然后向父节点传播）
        else if (strategy == RoutedEvent::RoutingStrategy::Bubble) {
            for (auto& handler : pair.second) {
                handler(this, args);
                if (args.handled) break;
            }
        }
        // Tunnel 策略应该从根节点向下传播（需要特殊处理）
        // 当前简化实现不完全支持 Tunnel
    }
    
    // 2. 冒泡到父元素（Bubble 策略）
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
            // 这里简化实现：移除所有匹配的处理器
            // 实际使用中可以考虑使用句柄或 ID 来标识处理器
            handlers.erase(
                std::remove_if(handlers.begin(), handlers.end(),
                    [&](const EventHandler& h) {
                        // 无法直接比较 std::function，这里是占位实现
                        // 实际项目中应该使用 EventToken 或其他机制
                        return false; // 暂时不移除
                    }),
                handlers.end()
            );
        }
    }
}

void UIElement::OnPointerPressed(PointerEventArgs& e) {
    // 默认不处理
}

void UIElement::OnPointerReleased(PointerEventArgs& e) {
    // 默认不处理
}

void UIElement::OnPointerMoved(PointerEventArgs& e) {
    // 默认不处理
}

void UIElement::OnPointerEntered(PointerEventArgs& e) {
    // 默认不处理
}

void UIElement::OnPointerExited(PointerEventArgs& e) {
    // 默认不处理
}

void UIElement::OnKeyDown(KeyEventArgs& e) {
    // 默认不处理
}

void UIElement::OnKeyUp(KeyEventArgs& e) {
    // 默认不处理
}

std::vector<UIElement*> UIElement::GetLogicalChildren() const {
    // 默认实现：非容器元素返回空列表
    return {};
}

UIElement* UIElement::Clone() const {
    // 默认实现：创建基本 UIElement 副本
    auto* clone = new UIElement();
    
    // 克隆基本属性
    clone->SetName(name_);
    clone->SetVisibility(GetVisibility());
    clone->SetIsEnabled(GetIsEnabled());
    clone->SetOpacity(GetOpacity());
    
    // 注意：不克隆 templatedParent_，因为克隆体会设置新的父元素
    // 注意：不克隆子元素，因为基类 UIElement 没有子元素
    
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

void UIElement::CollectDrawCommands(render::RenderContext& context) {
    // 检查可见性
    auto visibility = GetVisibility();
    if (visibility == Visibility::Collapsed || visibility == Visibility::Hidden) {
        return;  // 不渲染不可见或折叠的元素
    }
    
    // 推入布局偏移
    context.PushTransform(layoutRect_.x, layoutRect_.y);

    // 绘制自身内容
    OnRender(context);

    // 收集子元素绘制命令
    Visual::CollectDrawCommands(context);

    // 弹出变换
    context.PopTransform();
}

UIElement* UIElement::FindName(const std::string& name) {
    if (name.empty()) {
        return nullptr;
    }
    
    // 检查当前元素的名称
    if (name_ == name) {
        return this;
    }
    
    // 递归搜索所有逻辑子元素
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

} // namespace fk::ui
