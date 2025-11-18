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
#include "fk/animation/VisualStateBuilder.h"
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

    // 创建 Button 的默认 ControlTemplate（使用链式API定义视觉状态）
    // 注意：模板中定义状态结构，具体颜色值在 ResolveVisualStateTargets() 中从属性获取
    static ControlTemplate *CreateDefaultButtonTemplate()
    {
        auto *tmpl = new ControlTemplate();
        tmpl->SetTargetType(typeid(Button))
            ->SetFactory([]() -> UIElement *
                         { 
                             return (new Border())
                                   ->Name("RootBorder")  // 链式设置名称
                                   ->Background(binding::TemplateBinding(Control<Button>::BackgroundProperty()))
                                   ->BorderBrush(new SolidColorBrush(Color::FromRGB(172, 172, 172, 255)))
                                   ->BorderThickness(1.0f)
                                   ->Padding(10.0f, 5.0f, 10.0f, 5.0f)
                                   ->CornerRadius(4.0f)
                                   ->Child(
                                       (new ContentPresenter<>())
                                           ->SetHAlign(HorizontalAlignment::Center)
                                           ->SetVAlign(VerticalAlignment::Center));
                         })
            ->AddVisualStateGroup(
                animation::VisualStateBuilder::CreateGroup("CommonStates")
                    ->State("Normal")
                        ->ColorAnimation("RootBorder", "Background.Color")
                            ->To(Color::FromRGB(240, 240, 240, 255))  // 默认值，会被属性覆盖
                            ->Duration(200)
                        ->EndAnimation()
                    ->EndState()
                    ->State("MouseOver")
                        ->ColorAnimation("RootBorder", "Background.Color")
                            ->To(Color::FromRGB(229, 241, 251, 255))  // 默认值，会被 MouseOverBackground 属性覆盖
                            ->Duration(150)
                        ->EndAnimation()
                    ->EndState()
                    ->State("Pressed")
                        ->ColorAnimation("RootBorder", "Background.Color")
                            ->To(Color::FromRGB(204, 228, 247, 255))  // 默认值，会被 PressedBackground 属性覆盖
                            ->Duration(100)
                        ->EndAnimation()
                    ->EndState()
                    ->State("Disabled")
                        ->ColorAnimation("RootBorder", "Background.Color")
                            ->To(Color::FromRGB(200, 200, 200, 255))
                            ->Duration(200)
                        ->EndAnimation()
                        ->DoubleAnimation("RootBorder", "Opacity")
                            ->To(0.6)
                            ->Duration(200)
                        ->EndAnimation()
                    ->EndState()
                    ->Build()
            );

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
        // 首先尝试从模板加载视觉状态（用于自定义模板）
        if (LoadVisualStatesFromTemplate()) {
            // 如果模板中定义了视觉状态，解析TargetName并使用
            ResolveVisualStateTargets();
            UpdateVisualState(false);
            return;
        }
        
        // 使用属性驱动的视觉状态（默认模板走这里）
        // 创建 VisualStateManager
        auto manager = std::make_shared<animation::VisualStateManager>();
        animation::VisualStateManager::SetVisualStateManager(this, manager);

        // 创建 CommonStates 状态组
        auto commonStates = std::make_shared<animation::VisualStateGroup>("CommonStates");

        // 添加所有状态（这些方法会读取 Button 的属性）
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

    void Button::ResolveVisualStateTargets()
    {
        // 解析模板中定义的视觉状态的TargetName
        // 同时从 Button 属性中获取自定义颜色值
        // 获取VisualStateManager
        auto* manager = animation::VisualStateManager::GetVisualStateManager(this);
        if (!manager) {
            return;
        }
        
        // 获取模板根元素
        auto* root = GetTemplateRoot();
        if (!root) {
            return;
        }
        
        // 遍历所有状态组
        for (auto& group : manager->GetStateGroups()) {
            if (!group) continue;
            
            // 遍历组中的每个状态
            for (auto& state : group->GetStates()) {
                if (!state) continue;
                
                auto storyboard = state->GetStoryboard();
                if (!storyboard) continue;
                
                // 设置模板根，用于后续查找
                animation::Storyboard::SetTemplateRoot(storyboard.get(), root);
                
                // 获取状态名称，用于从属性获取对应的颜色
                std::string stateName = state->GetName();
                
                // 遍历storyboard中的所有动画
                for (auto& child : storyboard->GetChildren()) {
                    if (!child) continue;
                    
                    // 获取TargetName
                    std::string targetName = animation::Storyboard::GetTargetName(child.get());
                    if (targetName.empty()) {
                        continue;  // 没有使用TargetName，跳过
                    }
                    
                    // 在模板中查找命名元素
                    auto* target = ControlTemplate::FindName(targetName, root);
                    if (!target) {
                        std::cerr << "警告：在模板中找不到名为 '" << targetName << "' 的元素\n";
                        continue;
                    }
                    
                    // 获取PropertyPath（如"Background.Color"）
                    std::string propertyPath = animation::Storyboard::GetTargetProperty(child.get());
                    
                    // 简单实现：只支持"Background.Color"和"Opacity"
                    if (propertyPath == "Background.Color") {
                        // 目标是Border的Background属性中的Color
                        auto* border = dynamic_cast<Border*>(target);
                        if (border) {
                            auto* bg = border->GetBackground();
                            auto* brush = dynamic_cast<SolidColorBrush*>(bg);
                            if (brush) {
                                // 设置动画的实际目标
                                auto* colorAnim = dynamic_cast<animation::ColorAnimation*>(child.get());
                                if (colorAnim) {
                                    colorAnim->SetTarget(brush, &SolidColorBrush::ColorProperty());
                                    
                                    // 根据状态名称，从 Button 属性中获取颜色值
                                    if (stateName == "MouseOver") {
                                        auto* mouseOverBg = GetMouseOverBackground();
                                        auto* mouseOverBrush = dynamic_cast<SolidColorBrush*>(mouseOverBg);
                                        if (mouseOverBrush) {
                                            colorAnim->SetTo(mouseOverBrush->GetColor());
                                        }
                                    } else if (stateName == "Pressed") {
                                        auto* pressedBg = GetPressedBackground();
                                        auto* pressedBrush = dynamic_cast<SolidColorBrush*>(pressedBg);
                                        if (pressedBrush) {
                                            colorAnim->SetTo(pressedBrush->GetColor());
                                        }
                                    }
                                    // Normal 和 Disabled 状态使用模板中的默认值
                                }
                            }
                        }
                    } else if (propertyPath == "Opacity") {
                        // 目标是元素的Opacity属性
                        auto* doubleAnim = dynamic_cast<animation::DoubleAnimation*>(child.get());
                        if (doubleAnim) {
                            doubleAnim->SetTarget(target, &UIElement::OpacityProperty());
                        }
                    }
                }
            }
        }
    }

} // namespace fk::ui
