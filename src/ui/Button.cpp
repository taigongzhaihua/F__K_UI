#include "fk/ui/Button.h"
#include "fk/ui/ControlTemplate.h"
#include "fk/ui/Border.h"
#include "fk/ui/ContentPresenter.h"
#include "fk/ui/Brush.h"
#include "fk/binding/TemplateBinding.h"
#include "fk/ui/Control.h"
#include "fk/animation/VisualStateManager.h"
#include "fk/animation/VisualState.h"
#include "fk/animation/VisualStateGroup.h"
#include "fk/animation/Storyboard.h"
#include "fk/animation/ColorAnimation.h"
#include "fk/animation/DoubleAnimation.h"

namespace fk::ui {

// 创建 Button 的默认 ControlTemplate
static ControlTemplate* CreateDefaultButtonTemplate() {
    auto* tmpl = new ControlTemplate();
    tmpl->SetTargetType(typeid(Button));
    
    // 模板工厂：创建 Border + ContentPresenter
    tmpl->SetFactory([]() -> UIElement* {
        auto* border = new Border();
        
        // 使用 TemplateBinding 绑定 Button 的 Background 到 Border 的 Background
        // 这样设置 Button 的 Background 属性时，Border 的背景色会自动更新
        border->SetBinding(
            Border::BackgroundProperty(),
            binding::TemplateBinding(Control<Button>::BackgroundProperty())
        );
        
        // 设置默认背景色（当 Button 的 Background 为空时使用）
        // 注意：由于使用了 TemplateBinding，如果 Button 没有设置 Background，
        // 这里设置的默认值可能不会生效。需要在 Button 构造函数中设置默认值。
        // border->Background(new SolidColorBrush(Color(240, 240, 240, 255)));  // 浅灰色背景
        
        border->BorderBrush(new SolidColorBrush(Color::FromRGB(172, 172, 172, 255))); // 灰色边框
        border->BorderThickness(1.0f);
        border->Padding(10.0f, 5.0f, 10.0f, 5.0f);
        border->CornerRadius(3.0f);  // 统一圆角
        
        // ContentPresenter 用于显示按钮的内容
        auto* presenter = new ContentPresenter<>();
        presenter->SetHorizontalAlignment(HorizontalAlignment::Center);
        presenter->SetVerticalAlignment(VerticalAlignment::Center);
        
        border->Child(presenter);
        
        return border;
    });
    
    return tmpl;
}

Button::Button() : isPressed_(false) {
    // 设置默认对齐方式：Button 不应该拉伸，而是根据内容大小决定
    SetHorizontalAlignment(HorizontalAlignment::Left);
    SetVerticalAlignment(VerticalAlignment::Top);
    
    // 设置默认背景色（浅灰色）
    // 用户可以通过 Background() 方法覆盖此默认值
    if (!GetBackground()) {
        SetBackground(new SolidColorBrush(Color::FromRGB(240, 240, 240, 255)));
    }
    
    // 设置默认模板
    if (!GetTemplate()) {
        SetTemplate(CreateDefaultButtonTemplate());
    }
    
    // 注意：视觉状态初始化在 OnTemplateApplied() 中进行
    // 因为需要等待模板应用后才能访问 Border 等子元素
}

void Button::OnTemplateApplied() {
    ContentControl<Button>::OnTemplateApplied();
    
    // 手动同步 Background 属性到模板中的 Border
    // 这是因为 TemplateBinding 还没有完全实现
    SyncBackgroundToBorder();
    
    // 初始化视觉状态管理（需要在模板应用后执行）
    InitializeVisualStates();
}

void Button::SyncBackgroundToBorder() {
    // 获取模板根（应该是 Border）
    if (GetVisualChildrenCount() > 0) {
        auto* firstChild = GetVisualChild(0);
        auto* border = dynamic_cast<Border*>(firstChild);
        if (border) {
            // 将 Button 的 Background 同步到 Border
            auto* bg = GetBackground();
            if (bg) {
                border->Background(bg);
            }
        }
    }
}

void Button::OnPropertyChanged(const binding::DependencyProperty& property, 
                               const std::any& oldValue, 
                               const std::any& newValue,
                               binding::ValueSource oldSource,
                               binding::ValueSource newSource) {
    ContentControl<Button>::OnPropertyChanged(property, oldValue, newValue, oldSource, newSource);
    
    // 当 Background 属性改变时，同步到模板中的 Border
    if (property.Name() == "Background") {
        SyncBackgroundToBorder();
    }
    // 当 IsEnabled 属性改变时，更新视觉状态
    else if (property.Name() == "IsEnabled") {
        UpdateVisualState(true);
    }
}

void Button::OnPointerPressed(PointerEventArgs& e) {
    ContentControl<Button>::OnPointerPressed(e);
    
    if (this->GetIsEnabled()) {
        isPressed_ = true;
        UpdateVisualState(true);  // 使用视觉状态管理
        e.handled = true;
    }
}

void Button::OnPointerReleased(PointerEventArgs& e) {
    ContentControl<Button>::OnPointerReleased(e);
    
    if (isPressed_ && this->GetIsEnabled()) {
        isPressed_ = false;
        UpdateVisualState(true);  // 使用视觉状态管理
        
        // 触发 Click 事件
        Click();  // 使用 operator() 触发事件
        e.handled = true;
    }
}

void Button::OnPointerEntered(PointerEventArgs& e) {
    ContentControl<Button>::OnPointerEntered(e);
    UpdateVisualState(true);  // 使用视觉状态管理
}

void Button::OnPointerExited(PointerEventArgs& e) {
    ContentControl<Button>::OnPointerExited(e);
    
    if (isPressed_) {
        isPressed_ = false;
    }
    UpdateVisualState(true);  // 使用视觉状态管理
}

// ========== 视觉状态管理实现 ==========

void Button::InitializeVisualStates() {
    // 创建 VisualStateManager
    auto manager = std::make_shared<animation::VisualStateManager>();
    animation::VisualStateManager::SetVisualStateManager(this, manager);
    
    // 创建 CommonStates 状态组
    auto commonStates = std::make_shared<animation::VisualStateGroup>("CommonStates");
    
    // 添加所有状态
    commonStates->AddState(CreateNormalState());
    commonStates->AddState(CreateMouseOverState());
    commonStates->AddState(CreatePressedState());
    commonStates->AddState(CreateDisabledState());
    
    // 添加状态组到管理器
    manager->AddStateGroup(commonStates);
    
    // 设置初始状态
    UpdateVisualState(false);
}

void Button::UpdateVisualState(bool useTransitions) {
    // 根据当前状态决定应该进入哪个视觉状态
    if (!GetIsEnabled()) {
        animation::VisualStateManager::GoToState(this, "Disabled", useTransitions);
    } else if (isPressed_) {
        animation::VisualStateManager::GoToState(this, "Pressed", useTransitions);
    } else if (IsMouseOver()) {
        animation::VisualStateManager::GoToState(this, "MouseOver", useTransitions);
    } else {
        animation::VisualStateManager::GoToState(this, "Normal", useTransitions);
    }
}

std::shared_ptr<animation::VisualState> Button::CreateNormalState() {
    auto state = std::make_shared<animation::VisualState>("Normal");
    auto storyboard = std::make_shared<animation::Storyboard>();
    
    // 获取Border（模板根元素）
    if (GetVisualChildrenCount() > 0) {
        auto* border = dynamic_cast<Border*>(GetVisualChild(0));
        if (border) {
            // 背景色动画：恢复到默认颜色（浅灰色）
            auto currentBg = border->GetBackground();
            if (currentBg) {
                auto* solidBrush = dynamic_cast<SolidColorBrush*>(currentBg);
                if (solidBrush) {
                    auto bgAnim = std::make_shared<animation::ColorAnimation>(
                        solidBrush->GetColor(),
                        Color::FromRGB(240, 240, 240, 255),  // 浅灰色
                        animation::Duration(std::chrono::milliseconds(200))
                    );
                    bgAnim->SetTarget(border, &Border::BackgroundProperty());
                    storyboard->AddChild(bgAnim);
                }
            }
        }
    }
    
    state->SetStoryboard(storyboard);
    return state;
}

std::shared_ptr<animation::VisualState> Button::CreateMouseOverState() {
    auto state = std::make_shared<animation::VisualState>("MouseOver");
    auto storyboard = std::make_shared<animation::Storyboard>();
    
    // 获取Border（模板根元素）
    if (GetVisualChildrenCount() > 0) {
        auto* border = dynamic_cast<Border*>(GetVisualChild(0));
        if (border) {
            // 背景色动画：鼠标悬停时变成浅蓝色
            auto currentBg = border->GetBackground();
            if (currentBg) {
                auto* solidBrush = dynamic_cast<SolidColorBrush*>(currentBg);
                if (solidBrush) {
                    auto bgAnim = std::make_shared<animation::ColorAnimation>(
                        solidBrush->GetColor(),
                        Color::FromRGB(229, 241, 251, 255),  // 浅蓝色
                        animation::Duration(std::chrono::milliseconds(150))
                    );
                    bgAnim->SetTarget(border, &Border::BackgroundProperty());
                    storyboard->AddChild(bgAnim);
                }
            }
        }
    }
    
    state->SetStoryboard(storyboard);
    return state;
}

std::shared_ptr<animation::VisualState> Button::CreatePressedState() {
    auto state = std::make_shared<animation::VisualState>("Pressed");
    auto storyboard = std::make_shared<animation::Storyboard>();
    
    // 获取Border（模板根元素）
    if (GetVisualChildrenCount() > 0) {
        auto* border = dynamic_cast<Border*>(GetVisualChild(0));
        if (border) {
            // 背景色动画：按下时变成深蓝色
            auto currentBg = border->GetBackground();
            if (currentBg) {
                auto* solidBrush = dynamic_cast<SolidColorBrush*>(currentBg);
                if (solidBrush) {
                    auto bgAnim = std::make_shared<animation::ColorAnimation>(
                        solidBrush->GetColor(),
                        Color::FromRGB(204, 228, 247, 255),  // 深蓝色
                        animation::Duration(std::chrono::milliseconds(100))
                    );
                    bgAnim->SetTarget(border, &Border::BackgroundProperty());
                    storyboard->AddChild(bgAnim);
                }
            }
        }
    }
    
    state->SetStoryboard(storyboard);
    return state;
}

std::shared_ptr<animation::VisualState> Button::CreateDisabledState() {
    auto state = std::make_shared<animation::VisualState>("Disabled");
    auto storyboard = std::make_shared<animation::Storyboard>();
    
    // 获取Border（模板根元素）
    if (GetVisualChildrenCount() > 0) {
        auto* border = dynamic_cast<Border*>(GetVisualChild(0));
        if (border) {
            // 背景色动画：禁用时变成灰色
            auto currentBg = border->GetBackground();
            if (currentBg) {
                auto* solidBrush = dynamic_cast<SolidColorBrush*>(currentBg);
                if (solidBrush) {
                    auto bgAnim = std::make_shared<animation::ColorAnimation>(
                        solidBrush->GetColor(),
                        Color::FromRGB(200, 200, 200, 255),  // 灰色
                        animation::Duration(std::chrono::milliseconds(200))
                    );
                    bgAnim->SetTarget(border, &Border::BackgroundProperty());
                    storyboard->AddChild(bgAnim);
                }
            }
        }
    }
    
    // 透明度动画：禁用时半透明
    auto opacityAnim = std::make_shared<animation::DoubleAnimation>(
        static_cast<double>(GetOpacity()),
        0.6,
        animation::Duration(std::chrono::milliseconds(200))
    );
    opacityAnim->SetTarget(this, &Button::OpacityProperty());
    storyboard->AddChild(opacityAnim);
    
    state->SetStoryboard(storyboard);
    return state;
}

} // namespace fk::ui

