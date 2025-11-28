#include "fk/ui/ButtonBase.h"
#include "fk/ui/ControlTemplate.h"
#include "fk/ui/Border.h"
#include "fk/ui/Brush.h"
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
    
    template<typename Derived>
    const binding::DependencyProperty &ButtonBase<Derived>::MouseOverBackgroundProperty()
    {
        static const auto &prop = binding::DependencyProperty::Register(
            "MouseOverBackground",
            typeid(Brush *),
            typeid(ButtonBase<Derived>),
            binding::PropertyMetadata{
                .defaultValue = static_cast<Brush *>(new SolidColorBrush(Color::FromRGB(16, 110, 190, 255))),
                .propertyChangedCallback = [](DependencyObject &d, const DependencyProperty &, const std::any &, const std::any &)
                {
                    auto *button = static_cast<ButtonBase<Derived> *>(&d);
                    if (animation::VisualStateManager::GetVisualStateManager(button))
                    {
                        button->InitializeVisualStates();
                    }
                }});
        return prop;
    }

    template<typename Derived>
    const binding::DependencyProperty &ButtonBase<Derived>::PressedBackgroundProperty()
    {
        static const auto &prop = binding::DependencyProperty::Register(
            "PressedBackground",
            typeid(Brush *),
            typeid(ButtonBase<Derived>),
            binding::PropertyMetadata{
                .defaultValue = static_cast<Brush *>(new SolidColorBrush(0, 90, 158, 255)),
                .propertyChangedCallback = [](DependencyObject &d, const DependencyProperty &, const std::any &, const std::any &)
                {
                    auto *button = static_cast<ButtonBase<Derived> *>(&d);
                    if (animation::VisualStateManager::GetVisualStateManager(button))
                    {
                        button->InitializeVisualStates();
                    }
                }});
        return prop;
    }

    template<typename Derived>
    const binding::DependencyProperty &ButtonBase<Derived>::PrimaryClickOnlyProperty()
    {
        static const auto &prop = binding::DependencyProperty::Register(
            "PrimaryClickOnly",
            typeid(bool),
            typeid(ButtonBase<Derived>),
            binding::PropertyMetadata{
                .defaultValue = std::any(true)
            });
        return prop;
    }

    // ========== 事件处理 ==========

    template<typename Derived>
    void ButtonBase<Derived>::OnTemplateApplied()
    {
        ContentControl<Derived>::OnTemplateApplied();
        InitializeVisualStates();
    }

    template<typename Derived>
    void ButtonBase<Derived>::OnPropertyChanged(const binding::DependencyProperty &property,
                                               const std::any &oldValue,
                                               const std::any &newValue,
                                               binding::ValueSource oldSource,
                                               binding::ValueSource newSource)
    {
        ContentControl<Derived>::OnPropertyChanged(property, oldValue, newValue, oldSource, newSource);

        if (property.Name() == "IsEnabled")
        {
            UpdateVisualState(true);
        }
    }

    template<typename Derived>
    void ButtonBase<Derived>::OnPointerPressed(PointerEventArgs &e)
    {
        ContentControl<Derived>::OnPointerPressed(e);

        if (!this->GetIsEnabled())
        {
            return;
        }

        if (GetPrimaryClickOnly() && !e.IsLeftButton())
        {
            return;
        }

        isPressed_ = true;
        pressedButton_ = e.button;
        UpdateVisualState(true);
        e.handled = true;
    }

    template<typename Derived>
    void ButtonBase<Derived>::OnPointerReleased(PointerEventArgs &e)
    {
        ContentControl<Derived>::OnPointerReleased(e);

        if (!this->GetIsEnabled())
        {
            isPressed_ = false;
            pressedButton_ = MouseButton::None;
            return;
        }

        bool isPrimaryRelease = e.IsLeftButton();
        if (isPressed_ && pressedButton_ == e.button && (!GetPrimaryClickOnly() || isPrimaryRelease))
        {
            isPressed_ = false;
            pressedButton_ = MouseButton::None;
            UpdateVisualState(true);

            OnClick();
            e.handled = true;
        }
        else
        {
            isPressed_ = false;
            pressedButton_ = MouseButton::None;
        }
    }

    template<typename Derived>
    void ButtonBase<Derived>::OnPointerEntered(PointerEventArgs &e)
    {
        ContentControl<Derived>::OnPointerEntered(e);
        UpdateVisualState(true);
    }

    template<typename Derived>
    void ButtonBase<Derived>::OnPointerExited(PointerEventArgs &e)
    {
        ContentControl<Derived>::OnPointerExited(e);

        if (isPressed_)
        {
            isPressed_ = false;
            pressedButton_ = MouseButton::None;
        }
        UpdateVisualState(true);
    }

    template<typename Derived>
    void ButtonBase<Derived>::OnClick()
    {
        Click();
    }

    // ========== 视觉状态管理 ==========

    template<typename Derived>
    void ButtonBase<Derived>::InitializeVisualStates()
    {
        if (LoadVisualStatesFromTemplate())
        {
            ResolveVisualStateTargets();
            UpdateVisualState(false);
        }
    }

    template<typename Derived>
    bool ButtonBase<Derived>::LoadVisualStatesFromTemplate()
    {
        auto *tmpl = this->GetTemplate();
        
        if (!tmpl || !tmpl->HasVisualStates())
        {
            return false;
        }

        auto manager = std::make_shared<animation::VisualStateManager>();
        animation::VisualStateManager::SetVisualStateManager(this, manager);

        for (const auto &group : tmpl->GetVisualStateGroups())
        {
            if (group)
            {
                manager->AddStateGroup(group);
            }
        }

        return true;
    }

    template<typename Derived>
    void ButtonBase<Derived>::UpdateVisualState(bool useTransitions)
    {
        if (!this->GetIsEnabled())
        {
            animation::VisualStateManager::GoToState(this, "Disabled", useTransitions);
        }
        else if (isPressed_)
        {
            animation::VisualStateManager::GoToState(this, "Pressed", useTransitions);
        }
        else if (this->IsMouseOver())
        {
            animation::VisualStateManager::GoToState(this, "MouseOver", useTransitions);
        }
        else
        {
            animation::VisualStateManager::GoToState(this, "Normal", useTransitions);
        }
    }

    template<typename Derived>
    void ButtonBase<Derived>::ResolveVisualStateTargets()
    {
        auto *manager = animation::VisualStateManager::GetVisualStateManager(this);
        if (!manager)
        {
            return;
        }

        auto *root = this->GetTemplateRoot();
        if (!root)
        {
            return;
        }

        for (auto &group : manager->GetStateGroups())
        {
            if (!group)
                continue;

            for (auto &state : group->GetStates())
            {
                if (!state)
                    continue;

                auto storyboard = state->GetStoryboard();
                if (!storyboard)
                    continue;

                animation::Storyboard::SetTemplateRoot(storyboard.get(), root);

                for (auto &child : storyboard->GetChildren())
                {
                    if (!child)
                        continue;

                    std::string targetName = animation::Storyboard::GetTargetName(child.get());
                    if (targetName.empty())
                    {
                        continue;
                    }

                    auto *target = ControlTemplate::FindName(targetName, root);
                    if (!target)
                    {
                        std::cerr << "警告：在模板中找不到名为 '" << targetName << "' 的元素\n";
                        continue;
                    }

                    std::string propertyPath = animation::Storyboard::GetTargetProperty(child.get());

                    if (propertyPath == "Background.Color")
                    {
                        auto *border = dynamic_cast<Border *>(target);
                        if (border)
                        {
                            auto *bg = border->GetBackground();
                            auto *brush = dynamic_cast<SolidColorBrush *>(bg);
                            if (brush)
                            {
                                auto *colorAnim = dynamic_cast<animation::ColorAnimation *>(child.get());
                                if (colorAnim)
                                {
                                    colorAnim->SetTarget(brush, &SolidColorBrush::ColorProperty());

                                    if (colorAnim->HasToBinding())
                                    {
                                        auto *bindingProperty = colorAnim->GetToBinding();
                                        auto value = this->GetValue(*bindingProperty);
                                        if (value.has_value())
                                        {
                                            auto *sourceBrush = std::any_cast<Brush *>(value);
                                            auto *solidBrush = dynamic_cast<SolidColorBrush *>(sourceBrush);
                                            if (solidBrush)
                                            {
                                                colorAnim->SetTo(solidBrush->GetColor());
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                    else if (propertyPath == "Opacity")
                    {
                        auto *doubleAnim = dynamic_cast<animation::DoubleAnimation *>(child.get());
                        if (doubleAnim)
                        {
                            doubleAnim->SetTarget(target, &UIElement::OpacityProperty());
                        }
                    }
                }
            }
        }
    }

} // namespace fk::ui

// 显式实例化需要的额外头文件
#include "fk/ui/Button.h"
#include "fk/ui/ToggleButton.h"
#include "fk/ui/RepeatButton.h"

// 显式实例化 ButtonBase 模板（必须在命名空间之外）
template class fk::ui::ButtonBase<fk::ui::Button>;
template class fk::ui::ButtonBase<fk::ui::ToggleButton>;
template class fk::ui::ButtonBase<fk::ui::RepeatButton>;
