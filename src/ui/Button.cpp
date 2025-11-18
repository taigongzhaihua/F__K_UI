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
#include <iostream>

namespace fk::ui
{

    // ========== 依赖属性注册 ==========

    const binding::DependencyProperty &Button::MouseOverBackgroundProperty()
    {
        static const auto &prop = binding::DependencyProperty::Register(
            "MouseOverBackground",
            typeid(Brush *),
            typeid(Button),
            binding::PropertyMetadata{
                .defaultValue = static_cast<Brush *>(new SolidColorBrush(Color::FromRGB(229, 241, 251, 255))), // 默认浅蓝色
                .propertyChangedCallback = [](DependencyObject &d, const DependencyProperty &, const std::any &, const std::any &)
                {
                    auto *button = static_cast<Button *>(&d);
                    if (animation::VisualStateManager::GetVisualStateManager(button))
                    {
                        button->InitializeVisualStates();
                    }
                }});
        return prop;
    }

    const binding::DependencyProperty &Button::PressedBackgroundProperty()
    {
        static const auto &prop = binding::DependencyProperty::Register(
            "PressedBackground",
            typeid(Brush *),
            typeid(Button),
            binding::PropertyMetadata{
                .defaultValue = static_cast<Brush *>(new SolidColorBrush(Color::FromRGB(204, 228, 247, 255))), // 默认深蓝色
                .propertyChangedCallback = [](DependencyObject &d, const DependencyProperty &, const std::any &, const std::any &)
                {
                    auto *button = static_cast<Button *>(&d);
                    if (animation::VisualStateManager::GetVisualStateManager(button))
                    {
                        button->InitializeVisualStates();
                    }
                }});
        return prop;
    }

    // 创建 Button 的默认 ControlTemplate
    static ControlTemplate *CreateDefaultButtonTemplate()
    {
        auto *tmpl = new ControlTemplate();
        tmpl->SetTargetType(typeid(Button))
            ->SetFactory([]() -> UIElement *
                         { return (new Border())
                               ->Background(binding::TemplateBinding(Control<Button>::BackgroundProperty()))
                               ->BorderBrush(new SolidColorBrush(Color::FromRGB(172, 172, 172, 255)))
                               ->BorderThickness(1.0f)
                               ->Padding(10.0f, 5.0f, 10.0f, 5.0f)
                               ->CornerRadius(4.0f)
                               ->Child(
                                   (new ContentPresenter<>())
                                       ->SetHAlign(HorizontalAlignment::Center)
                                       ->SetVAlign(VerticalAlignment::Center)); });

        return tmpl;
    }

    Button::Button() : isPressed_(false)
    {
        // 设置默认对齐方式：Button 不应该拉伸，而是根据内容大小决定
        SetHorizontalAlignment(HorizontalAlignment::Left);
        SetVerticalAlignment(VerticalAlignment::Top);

        // 设置默认背景色（浅灰色）
        // 用户可以通过 Background() 方法覆盖此默认值
        if (!GetBackground())
        {
            SetBackground(new SolidColorBrush(Color::FromRGB(240, 240, 240, 255)));
        }

        // 设置默认模板
        if (!GetTemplate())
        {
            SetTemplate(CreateDefaultButtonTemplate());
        }

        // 注意：视觉状态初始化在 OnTemplateApplied() 中进行
        // 因为需要等待模板应用后才能访问 Border 等子元素
    }

    void Button::OnTemplateApplied()
    {
        ContentControl<Button>::OnTemplateApplied();

        // 手动同步 Background 属性到模板中的 Border
        // 虽然模板中使用了 TemplateBinding，但当前实现还需要手动触发同步
        SyncBackgroundToBorder();

        // 初始化视觉状态管理（需要在模板应用后执行）
        InitializeVisualStates();
    }

    void Button::OnPropertyChanged(const binding::DependencyProperty &property,
                                   const std::any &oldValue,
                                   const std::any &newValue,
                                   binding::ValueSource oldSource,
                                   binding::ValueSource newSource)
    {
        ContentControl<Button>::OnPropertyChanged(property, oldValue, newValue, oldSource, newSource);

        // 当 Background 属性改变时，同步到模板中的 Border
        // 虽然使用了 TemplateBinding，但当前实现需要手动触发更新
        if (property.Name() == "Background")
        {
            SyncBackgroundToBorder();
        }
        // 当 IsEnabled 属性改变时，更新视觉状态
        else if (property.Name() == "IsEnabled")
        {
            UpdateVisualState(true);
        }
    }

    void Button::OnPointerPressed(PointerEventArgs &e)
    {
        ContentControl<Button>::OnPointerPressed(e);

        if (this->GetIsEnabled())
        {
            isPressed_ = true;
            UpdateVisualState(true); // 使用视觉状态管理
            e.handled = true;
        }
    }

    void Button::OnPointerReleased(PointerEventArgs &e)
    {
        ContentControl<Button>::OnPointerReleased(e);

        if (isPressed_ && this->GetIsEnabled())
        {
            isPressed_ = false;
            UpdateVisualState(true); // 使用视觉状态管理

            // 触发 Click 事件
            Click(); // 使用 operator() 触发事件
            e.handled = true;
        }
    }

    void Button::OnPointerEntered(PointerEventArgs &e)
    {
        ContentControl<Button>::OnPointerEntered(e);
        UpdateVisualState(true); // 使用视觉状态管理
    }

    void Button::OnPointerExited(PointerEventArgs &e)
    {
        ContentControl<Button>::OnPointerExited(e);

        if (isPressed_)
        {
            isPressed_ = false;
        }
        UpdateVisualState(true); // 使用视觉状态管理
    }

    void Button::SyncBackgroundToBorder()
    {
        // 获取模板根（应该是 Border）
        if (GetVisualChildrenCount() > 0)
        {
            auto *firstChild = GetVisualChild(0);
            auto *border = dynamic_cast<Border *>(firstChild);
            if (border)
            {
                // 将 Button 的 Background 同步到 Border
                auto *bg = GetBackground();
                if (bg)
                {
                    border->Background(bg);
                }
            }
        }
    }

    // ========== 视觉状态管理实现 ==========

    void Button::InitializeVisualStates()
    {
        // 首先尝试从模板加载视觉状态
        if (LoadVisualStatesFromTemplate()) {
            // 如果模板中定义了视觉状态，使用模板中的定义
            UpdateVisualState(false);
            return;
        }
        
        // 如果模板中没有定义视觉状态，使用默认状态
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
    
    bool Button::LoadVisualStatesFromTemplate()
    {
        // 获取当前模板
        auto* tmpl = GetTemplate();
        if (!tmpl) {
            return false;
        }
        
        // 检查模板是否定义了视觉状态
        if (!tmpl->HasVisualStates()) {
            return false;
        }
        
        // 创建 VisualStateManager
        auto manager = std::make_shared<animation::VisualStateManager>();
        animation::VisualStateManager::SetVisualStateManager(this, manager);
        
        // 从模板复制所有视觉状态组
        for (const auto& group : tmpl->GetVisualStateGroups()) {
            if (group) {
                manager->AddStateGroup(group);
            }
        }
        
        return true;
    }

    void Button::UpdateVisualState(bool useTransitions)
    {
        // 根据当前状态决定应该进入哪个视觉状态
        if (!GetIsEnabled())
        {
            animation::VisualStateManager::GoToState(this, "Disabled", useTransitions);
        }
        else if (isPressed_)
        {
            animation::VisualStateManager::GoToState(this, "Pressed", useTransitions);
        }
        else if (IsMouseOver())
        {
            animation::VisualStateManager::GoToState(this, "MouseOver", useTransitions);
        }
        else
        {
            animation::VisualStateManager::GoToState(this, "Normal", useTransitions);
        }
    }

    std::shared_ptr<animation::VisualState> Button::CreateNormalState()
    {
        auto state = std::make_shared<animation::VisualState>("Normal");
        auto storyboard = std::make_shared<animation::Storyboard>();

        // 直接操作 Button 的 Background
        auto currentBg = GetBackground();
        if (currentBg)
        {
            auto *solidBrush = dynamic_cast<SolidColorBrush *>(currentBg);
            if (solidBrush)
            {
                // 不指定起始色,让动画自动从当前颜色开始
                auto bgAnim = std::make_shared<animation::ColorAnimation>();
                bgAnim->SetTo(Color::FromRGB(240, 240, 240, 255)); // 浅灰色
                bgAnim->SetDuration(animation::Duration(std::chrono::milliseconds(200)));
                bgAnim->SetTarget(solidBrush, &SolidColorBrush::ColorProperty());
                storyboard->AddChild(bgAnim);
            }
        }

        state->SetStoryboard(storyboard);
        return state;
    }

    std::shared_ptr<animation::VisualState> Button::CreateMouseOverState()
    {
        auto state = std::make_shared<animation::VisualState>("MouseOver");
        auto storyboard = std::make_shared<animation::Storyboard>();

        // 直接操作 Button 的 Background（通过 TemplateBinding 会反映到 Border）
        auto currentBg = GetBackground();
        auto *currentSolidBrush = dynamic_cast<SolidColorBrush *>(currentBg);

        auto targetBg = GetMouseOverBackground();
        auto *targetSolidBrush = dynamic_cast<SolidColorBrush *>(targetBg);

        if (currentSolidBrush && targetSolidBrush)
        {
            // 不指定起始色,让动画自动从当前颜色开始
            auto bgAnim = std::make_shared<animation::ColorAnimation>();
            bgAnim->SetTo(targetSolidBrush->GetColor());
            bgAnim->SetDuration(animation::Duration(std::chrono::milliseconds(150)));
            bgAnim->SetTarget(currentSolidBrush, &SolidColorBrush::ColorProperty());
            storyboard->AddChild(bgAnim);
        }

        state->SetStoryboard(storyboard);
        return state;
    }

    std::shared_ptr<animation::VisualState> Button::CreatePressedState()
    {
        auto state = std::make_shared<animation::VisualState>("Pressed");
        auto storyboard = std::make_shared<animation::Storyboard>();

        // 直接操作 Button 的 Background
        auto currentBg = GetBackground();
        auto *currentSolidBrush = dynamic_cast<SolidColorBrush *>(currentBg);

        auto targetBg = GetPressedBackground();
        auto *targetSolidBrush = dynamic_cast<SolidColorBrush *>(targetBg);

        if (currentSolidBrush && targetSolidBrush)
        {
            // 不指定起始色,让动画自动从当前颜色开始
            auto bgAnim = std::make_shared<animation::ColorAnimation>();
            bgAnim->SetTo(targetSolidBrush->GetColor());
            bgAnim->SetDuration(animation::Duration(std::chrono::milliseconds(100))); // 按下动画更快
            bgAnim->SetTarget(currentSolidBrush, &SolidColorBrush::ColorProperty());
            storyboard->AddChild(bgAnim);
        }

        state->SetStoryboard(storyboard);
        return state;
    }

    std::shared_ptr<animation::VisualState> Button::CreateDisabledState()
    {
        auto state = std::make_shared<animation::VisualState>("Disabled");
        auto storyboard = std::make_shared<animation::Storyboard>();

        // 直接操作 Button 的 Background
        auto currentBg = GetBackground();
        if (currentBg)
        {
            auto *solidBrush = dynamic_cast<SolidColorBrush *>(currentBg);
            if (solidBrush)
            {
                // 不指定起始色,让动画自动从当前颜色开始
                auto bgAnim = std::make_shared<animation::ColorAnimation>();
                bgAnim->SetTo(Color::FromRGB(200, 200, 200, 255)); // 灰色
                bgAnim->SetDuration(animation::Duration(std::chrono::milliseconds(200)));
                bgAnim->SetTarget(solidBrush, &SolidColorBrush::ColorProperty());
                storyboard->AddChild(bgAnim);
            }
        }

        state->SetStoryboard(storyboard);
        return state;
    }

} // namespace fk::ui
