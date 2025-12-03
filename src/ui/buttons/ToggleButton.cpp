#include "fk/ui/buttons/ToggleButton.h"
#include "fk/ui/buttons/Button.h"
#include "fk/ui/controls/Control.h"
#include "fk/ui/styling/ControlTemplate.h"
#include "fk/ui/controls/Border.h"
#include "fk/ui/controls/ContentPresenter.h"
#include "fk/ui/graphics/Brush.h"
#include "fk/binding/TemplateBinding.h"
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

    // ========== 依赖属性注�?==========

    const binding::DependencyProperty &ToggleButton::IsCheckedProperty()
    {
        static const auto &prop = binding::DependencyProperty::Register(
            "IsChecked",
            typeid(std::optional<bool>),
            typeid(ToggleButton),
            binding::PropertyMetadata{
                .defaultValue = std::optional<bool>(false),
                .propertyChangedCallback = [](DependencyObject &d, const DependencyProperty &, const std::any &oldVal, const std::any &newVal)
                {
                    auto *toggleBtn = static_cast<ToggleButton *>(&d);
                    
                    // 获取新旧�?
                    std::optional<bool> oldValue = std::any_cast<std::optional<bool>>(oldVal);
                    std::optional<bool> newValue = std::any_cast<std::optional<bool>>(newVal);
                    
                    // 触发事件
                    toggleBtn->RaiseCheckedEvents(oldValue, newValue);
                    
                    // 更新视觉状�?
                    toggleBtn->UpdateCheckVisualState(true);
                }});
        return prop;
    }

    const binding::DependencyProperty &ToggleButton::IsThreeStateProperty()
    {
        static const auto &prop = binding::DependencyProperty::Register(
            "IsThreeState",
            typeid(bool),
            typeid(ToggleButton),
            binding::PropertyMetadata{
                .defaultValue = std::any(false)});
        return prop;
    }

    const binding::DependencyProperty &ToggleButton::CheckedBackgroundProperty()
    {
        static const auto &prop = binding::DependencyProperty::Register(
            "CheckedBackground",
            typeid(Brush *),
            typeid(ToggleButton),
            binding::PropertyMetadata{
                .defaultValue = static_cast<Brush *>(new SolidColorBrush(Color::FromRGB(0, 95, 184, 255))), // 深蓝色表示选中
                .propertyChangedCallback = [](DependencyObject &d, const DependencyProperty &, const std::any &, const std::any &)
                {
                    auto *toggleBtn = static_cast<ToggleButton *>(&d);
                    if (animation::VisualStateManager::GetVisualStateManager(toggleBtn))
                    {
                        toggleBtn->InitializeToggleVisualStates();
                    }
                }});
        return prop;
    }

    const binding::DependencyProperty &ToggleButton::CheckedForegroundProperty()
    {
        static const auto &prop = binding::DependencyProperty::Register(
            "CheckedForeground",
            typeid(Brush *),
            typeid(ToggleButton),
            binding::PropertyMetadata{
                .defaultValue = static_cast<Brush *>(new SolidColorBrush(Color::FromRGB(255, 255, 255, 255))), // 白色前景
                .propertyChangedCallback = [](DependencyObject &d, const DependencyProperty &, const std::any &, const std::any &)
                {
                    auto *toggleBtn = static_cast<ToggleButton *>(&d);
                    if (animation::VisualStateManager::GetVisualStateManager(toggleBtn))
                    {
                        toggleBtn->InitializeToggleVisualStates();
                    }
                }});
        return prop;
    }

    // 创建 ToggleButton 的默�?ControlTemplate
    static ControlTemplate *CreateDefaultToggleButtonTemplate()
    {
        auto *tmpl = new ControlTemplate();
        tmpl->SetTargetType(typeid(ToggleButton))
            ->SetFactory([]() -> UIElement *
                         { 
                             return (new Border())
                                   ->Name("RootBorder")
                                   ->Background(binding::TemplateBinding(Control<ToggleButton>::BackgroundProperty()))
                                   ->BorderBrush(binding::TemplateBinding(Control<ToggleButton>::BorderBrushProperty()))
                                   ->BorderThickness(binding::TemplateBinding(Control<ToggleButton>::BorderThicknessProperty()))
                                   ->Padding(10.0f, 5.0f, 10.0f, 5.0f)
                                   ->CornerRadius(4.0f)
                                   ->Child(
                                       (new ContentPresenter<>())
                                           ->SetHAlign(HorizontalAlignment::Center)
                                           ->SetVAlign(VerticalAlignment::Center)); 
                         })
            // CommonStates 状态组（使用透明度和边框叠加提供悬停效果�?
            ->AddVisualStateGroup(
                animation::VisualStateBuilder::CreateGroup("CommonStates")
                    ->State("Normal")
                    // Normal 状态恢复默认透明度和边框
                    ->DoubleAnimation("RootBorder", "Opacity")
                    ->To(1.0)
                    ->Duration(100)
                    ->EndAnimation()
                    ->ColorAnimation("RootBorder", "BorderBrush.Color")
                    ->ToBinding(Control<ToggleButton>::BorderBrushProperty())
                    ->Duration(100)
                    ->EndAnimation()
                    ->EndState()
                    ->State("MouseOver")
                    // 鼠标悬停：降低透明�?+ 高亮边框
                    ->DoubleAnimation("RootBorder", "Opacity")
                    ->To(0.85)
                    ->Duration(50)
                    ->EndAnimation()
                    ->ColorAnimation("RootBorder", "BorderBrush.Color")
                    ->To(Color::FromRGB(255, 255, 255, 255)) // 白色边框高亮
                    ->Duration(50)
                    ->EndAnimation()
                    ->EndState()
                    ->State("Pressed")
                    // 按下状态：更低透明�?+ 更亮边框
                    ->DoubleAnimation("RootBorder", "Opacity")
                    ->To(0.7)
                    ->Duration(50)
                    ->EndAnimation()
                    ->ColorAnimation("RootBorder", "BorderBrush.Color")
                    ->To(Color::FromRGB(255, 255, 255, 255))
                    ->Duration(50)
                    ->EndAnimation()
                    ->EndState()
                    ->State("Disabled")
                    // 禁用状态：灰色 + 低透明�?
                    ->ColorAnimation("RootBorder", "Background.Color")
                    ->To(Color::FromRGB(200, 200, 200, 255))
                    ->Duration(100)
                    ->EndAnimation()
                    ->DoubleAnimation("RootBorder", "Opacity")
                    ->To(0.5)
                    ->Duration(100)
                    ->EndAnimation()
                    ->EndState()
                    ->Build())
            // CheckStates 状态组（ToggleButton 特有�?
            ->AddVisualStateGroup(
                animation::VisualStateBuilder::CreateGroup("CheckStates")
                    ->State("Unchecked")
                    // Unchecked 状态使�?Background 属性的背景�?
                    ->ColorAnimation("RootBorder", "Background.Color")
                    ->ToBinding(Control<ToggleButton>::BackgroundProperty())
                    ->Duration(150)
                    ->EndAnimation()
                    ->EndState()
                    ->State("Checked")
                    ->ColorAnimation("RootBorder", "Background.Color")
                    ->ToBinding(ToggleButton::CheckedBackgroundProperty())
                    ->Duration(150)
                    ->EndAnimation()
                    ->EndState()
                    ->State("Indeterminate")
                    ->ColorAnimation("RootBorder", "Background.Color")
                    ->To(Color::FromRGB(120, 120, 120, 255)) // 灰色表示不确定状�?
                    ->Duration(150)
                    ->EndAnimation()
                    ->DoubleAnimation("RootBorder", "Opacity")
                    ->To(0.8)
                    ->Duration(150)
                    ->EndAnimation()
                    ->EndState()
                    ->Build());

        return tmpl;
    }

    ToggleButton::ToggleButton()
    {
        // 设置默认对齐方式
        SetHorizontalAlignment(HorizontalAlignment::Left);
        SetVerticalAlignment(VerticalAlignment::Top);

        // 设置默认背景�?
        if (!GetBackground())
        {
            SetBackground(new SolidColorBrush(Color::FromRGB(0, 120, 212, 255))); // 默认蓝色
        }

        if (!GetBorderBrush())
        {
            SetBorderBrush(new SolidColorBrush(Color::FromRGB(0, 120, 212, 255)));
        }

        // 设置默认模板
        if (!GetTemplate())
        {
            SetTemplate(CreateDefaultToggleButtonTemplate());
        }
    }

    void ToggleButton::OnTemplateApplied()
    {
        ButtonBase<ToggleButton>::OnTemplateApplied();

        // 初始�?ToggleButton 特有的视觉状�?
        InitializeToggleVisualStates();
    }

    void ToggleButton::OnPropertyChanged(const binding::DependencyProperty &property,
                                         const std::any &oldValue,
                                         const std::any &newValue,
                                         binding::ValueSource oldSource,
                                         binding::ValueSource newSource)
    {
        ButtonBase<ToggleButton>::OnPropertyChanged(property, oldValue, newValue, oldSource, newSource);
    }

    void ToggleButton::OnPointerReleased(PointerEventArgs &e)
    {
        // 调用基类处理基本逻辑
        ButtonBase<ToggleButton>::OnPointerReleased(e);
        
        // OnClick 会被基类调用，在那里处理切换逻辑
    }
    
    void ToggleButton::OnClick()
    {
        // 重写 OnClick 以在点击时切换状�?
        Toggle();
        
        // 调用基类触发 Click 事件
        ButtonBase<ToggleButton>::OnClick();
    }

    void ToggleButton::Toggle()
    {
        auto currentChecked = GetIsChecked();

        if (!GetIsThreeState())
        {
            // 二态模式：�?true �?false 之间切换
            SetIsChecked(!currentChecked.value_or(false));
        }
        else
        {
            // 三态模式：null -> false -> true -> null
            if (!currentChecked.has_value())
            {
                SetIsChecked(false);
            }
            else if (*currentChecked == false)
            {
                SetIsChecked(true);
            }
            else
            {
                SetIsChecked(std::nullopt);
            }
        }
    }

    void ToggleButton::InitializeToggleVisualStates()
    {
        // 调用基类方法初始化所有视觉状态（包括 CommonStates �?CheckStates�?
        // ButtonBase::ResolveVisualStateTargets() 会自动处理所有状态组的绑定解�?
        InitializeVisualStates();

        // 设置初始的选中状�?
        UpdateCheckVisualState(false);
    }

    void ToggleButton::UpdateVisualState(bool useTransitions)
    {
        // ToggleButton 的视觉状态策略（方案 A）：
        // 1. CheckStates 控制背景色（基于 IsChecked�?
        // 2. CommonStates 通过透明度和边框提供鼠标反馈（不影响背景色）
        // 3. 两个状态组可以同时生效，提供叠加效�?
        
        // 先更�?CheckStates（Checked/Unchecked/Indeterminate�?
        UpdateCheckVisualState(useTransitions);
        
        // 然后调用基类更新 CommonStates（Normal/MouseOver/Pressed/Disabled�?
        // 由于 CommonStates 只修�?Opacity �?BorderBrush，不会覆盖背景色
        ButtonBase<ToggleButton>::UpdateVisualState(useTransitions);
    }
    
    void ToggleButton::UpdateCheckVisualState(bool useTransitions)
    {
        // 检查是否已经初始化了视觉状态，避免在模板应用前调用
        auto* manager = animation::VisualStateManager::GetVisualStateManager(this);
        if (!manager || manager->GetStateGroups().empty())
        {
            // 视觉状态还没有初始化，稍后�?OnTemplateApplied 中会处理
            return;
        }
        
        auto checked = GetIsChecked();

        std::string targetState;
        if (!checked.has_value())
        {
            targetState = "Indeterminate";
        }
        else if (*checked)
        {
            targetState = "Checked";
        }
        else
        {
            targetState = "Unchecked";
        }

        animation::VisualStateManager::GoToState(this, targetState, useTransitions);
    }

    void ToggleButton::RaiseCheckedEvents(const std::optional<bool> &oldValue, const std::optional<bool> &newValue)
    {
        // 如果值没有变化，不触发事�?
        if (oldValue == newValue)
        {
            return;
        }

        // 根据新值触发相应的事件
        if (!newValue.has_value())
        {
            Indeterminate(newValue);
        }
        else if (*newValue)
        {
            Checked(newValue);
        }
        else
        {
            Unchecked(newValue);
        }
    }

} // namespace fk::ui
