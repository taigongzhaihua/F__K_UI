#include "fk/ui/buttons/RadioButton.h"
#include "fk/ui/controls/Control.h"
#include "fk/ui/styling/ControlTemplate.h"
#include "fk/ui/controls/Border.h"
#include "fk/ui/controls/ContentPresenter.h"
#include "fk/ui/layouts/StackPanel.h"
#include "fk/ui/layouts/Grid.h"
#include "fk/ui/graphics/Brush.h"
#include "fk/ui/graphics/Shape.h"
#include "fk/ui/layouts/Panel.h"
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

    const binding::DependencyProperty &RadioButton::GroupNameProperty()
    {
        static const auto &prop = binding::DependencyProperty::Register(
            "GroupName",
            typeid(std::string),
            typeid(RadioButton),
            binding::PropertyMetadata{
                .defaultValue = std::string("") // 空字符串表示使用父容器作为分�?
            });
        return prop;
    }

    const binding::DependencyProperty &RadioButton::RadioMarkColorProperty()
    {
        static const auto &prop = binding::DependencyProperty::Register(
            "RadioMarkColor",
            typeid(Brush *),
            typeid(RadioButton),
            binding::PropertyMetadata{
                .defaultValue = static_cast<Brush *>(new SolidColorBrush(Color::FromRGB(0, 120, 215, 255))), // 蓝色标记
                .propertyChangedCallback = [](DependencyObject &d, const DependencyProperty &, const std::any &, const std::any &)
                {
                    auto *radioBtn = static_cast<RadioButton *>(&d);
                    if (animation::VisualStateManager::GetVisualStateManager(radioBtn))
                    {
                        radioBtn->InitializeToggleVisualStates();
                    }
                }});
        return prop;
    }

    const binding::DependencyProperty &RadioButton::RadioBorderColorProperty()
    {
        static const auto &prop = binding::DependencyProperty::Register(
            "RadioBorderColor",
            typeid(Brush *),
            typeid(RadioButton),
            binding::PropertyMetadata{
                .defaultValue = static_cast<Brush *>(new SolidColorBrush(Color::FromRGB(120, 120, 120, 255))), // 灰色边框
                .propertyChangedCallback = [](DependencyObject &d, const DependencyProperty &, const std::any &, const std::any &)
                {
                    auto *radioBtn = static_cast<RadioButton *>(&d);
                    if (animation::VisualStateManager::GetVisualStateManager(radioBtn))
                    {
                        radioBtn->InitializeToggleVisualStates();
                    }
                }});
        return prop;
    }

    // 创建 RadioButton 的默�?ControlTemplate
    static ControlTemplate *CreateDefaultRadioButtonTemplate()
    {
        auto *tmpl = new ControlTemplate();
        tmpl->SetTargetType(typeid(RadioButton))
            ->SetFactory([]() -> UIElement *
                         {
                // StackPanel (水平布局)
                //   ├─ RadioButtonBorder (20x20 圆形，灰色边框，白色背景)
                //   �?   └─ RadioMark (圆形标记，初始隐�?
                //   └─ ContentPresenter (显示文本标签)
                
                // 创建圆形内部标记 - 用于选中状�?
                auto* radioMark = (new Border())
                    ->Name("RadioMark")
                    ->Width(10.0f)
                    ->Height(10.0f)
                    ->SetVAlign(VerticalAlignment::Center)
                    ->SetHAlign(HorizontalAlignment::Center)
                    ->Background(new SolidColorBrush(Color::FromRGB(0, 120, 215, 255)))
                    ->CornerRadius(5.0f)  // 完全圆形
                    ->Opacity(0.0);  // 初始隐藏
                
                return (new StackPanel())
                    ->SetOrient(Orientation::Horizontal)
                    ->SetVAlign(VerticalAlignment::Center)
                    ->Children({
                        (new Border())
                            ->Name("RadioButtonBorder")
                            ->Width(20.0f)
                            ->Height(20.0f)
                            ->BorderThickness(2.0f)
                            ->CornerRadius(10.0f)  // 完全圆形
                            ->BorderBrush(new SolidColorBrush(Color::FromRGB(120, 120, 120, 255)))
                            ->Background(new SolidColorBrush(Color::FromRGB(255, 255, 255, 255)))
                            ->Margin(0.0f, 0.0f, 8.0f, 0.0f)
                            ->Child(radioMark),
                        (new ContentPresenter<>())
                            ->SetVAlign(VerticalAlignment::Center)
                    }); })
            // CommonStates 状态组（鼠标交互效果）
            ->AddVisualStateGroup(
                animation::VisualStateBuilder::CreateGroup("CommonStates")
                    ->State("Normal")
                    ->ColorAnimation("RadioButtonBorder", "BorderBrush.Color")
                    ->ToBinding(RadioButton::RadioBorderColorProperty())
                    ->Duration(150)
                    ->EndAnimation()
                    ->EndState()
                    ->State("MouseOver")
                    // 鼠标悬停时边框高亮和背景变化
                    ->ColorAnimation("RadioButtonBorder", "Background.Color")
                    ->To(Color::FromRGB(240, 248, 255, 255)) // 淡蓝色背�?
                    ->Duration(100)
                    ->EndAnimation()
                    ->ColorAnimation("RadioButtonBorder", "BorderBrush.Color")
                    ->To(Color::FromRGB(0, 120, 215, 255)) // 蓝色高亮边框
                    ->Duration(100)
                    ->EndAnimation()
                    ->EndState()
                    ->State("Pressed")
                    // 按下时边框更�?
                    ->ColorAnimation("RadioButtonBorder", "BorderBrush.Color")
                    ->To(Color::FromRGB(50, 120, 215, 255)) // 深蓝�?
                    ->Duration(50)
                    ->EndAnimation()
                    ->ColorAnimation("RadioButtonBorder", "Background.Color")
                    ->To(Color::FromRGB(230, 240, 250, 255)) // 更淡的蓝色背�?
                    ->Duration(50)
                    ->EndAnimation()
                    ->EndState()
                    ->State("Disabled")
                    // 禁用状�?
                    ->ColorAnimation("RadioButtonBorder", "BorderBrush.Color")
                    ->To(Color::FromRGB(200, 200, 200, 255)) // 灰色边框
                    ->Duration(100)
                    ->EndAnimation()
                    ->ColorAnimation("RadioButtonBorder", "Background.Color")
                    ->To(Color::FromRGB(240, 240, 240, 255)) // 浅灰色背�?
                    ->Duration(100)
                    ->EndAnimation()
                    ->DoubleAnimation("RadioButtonBorder", "Opacity")
                    ->To(0.5)
                    ->Duration(100)
                    ->EndAnimation()
                    ->EndState()
                    ->Build())
            // CheckStates 状态组（选中状态）
            // RadioButton 不支�?Indeterminate 状态，只有 Checked �?Unchecked
            ->AddVisualStateGroup(
                animation::VisualStateBuilder::CreateGroup("CheckStates")
                    ->State("Unchecked")
                    // 未选中 - 隐藏圆形标记
                    ->DoubleAnimation("RadioMark", "Opacity")
                    ->To(0.0)
                    ->Duration(150)
                    ->EndAnimation()
                    ->DoubleAnimation("RadioMark", "Width")
                    ->To(0.0)
                    ->Duration(150)
                    ->EndAnimation()
                    ->DoubleAnimation("RadioMark", "Height")
                    ->To(0.0)
                    ->Duration(150)
                    ->EndAnimation()
                    ->EndState()
                    ->State("Checked")
                    // 选中 - 显示圆形标记，带缩放动画
                    ->DoubleAnimation("RadioMark", "Opacity")
                    ->To(1.0)
                    ->Duration(150)
                    ->EndAnimation()
                    ->DoubleAnimation("RadioMark", "Width")
                    ->To(10.0)
                    ->Duration(150)
                    ->EndAnimation()
                    ->DoubleAnimation("RadioMark", "Height")
                    ->To(10.0)
                    ->Duration(150)
                    ->EndAnimation()
                    ->EndState()
                    ->Build());

        return tmpl;
    }

    RadioButton::RadioButton()
    {
        // 设置默认对齐方式
        SetHorizontalAlignment(HorizontalAlignment::Left);
        SetVerticalAlignment(VerticalAlignment::Center);

        // RadioButton 不支持三�?
        SetIsThreeState(false);

        // 设置默认选中背景�?
        if (!GetCheckedBackground())
        {
            SetCheckedBackground(new SolidColorBrush(Color::FromRGB(0, 120, 215, 255))); // 蓝色
        }

        // 设置默认模板
        SetTemplate(CreateDefaultRadioButtonTemplate());
    }

    void RadioButton::OnTemplateApplied()
    {
        ToggleButton::OnTemplateApplied();
    }

    void RadioButton::OnClick()
    {
        // RadioButton 的特殊行为：
        // - 如果未选中，则选中
        // - 如果已选中，则保持选中（不切换�?
        
        if (!GetIsChecked().value_or(false))
        {
            // 未选中时，先取消同组其他按�?
            UncheckOtherRadioButtons();
            
            // 然后选中自己
            SetIsChecked(true);
        }
        // 如果已选中，什么都不做（不会取消选中�?
        
        // 调用基类，触�?Click 事件
        ButtonBase::OnClick();
    }

    void RadioButton::Toggle()
    {
        // RadioButton 重写 Toggle：只能从 unchecked 变为 checked
        // 不能�?checked 变为 unchecked
        if (!GetIsChecked().value_or(false))
        {
            UncheckOtherRadioButtons();
            SetIsChecked(true);
        }
    }

    void RadioButton::UncheckOtherRadioButtons()
    {
        auto members = GetGroupMembers();
        
        for (auto* member : members)
        {
            if (member != this && member->GetIsChecked().value_or(false))
            {
                member->SetIsChecked(false);
            }
        }
    }

    std::vector<RadioButton*> RadioButton::GetGroupMembers()
    {
        std::vector<RadioButton*> members;
        
        std::string groupName = GetGroupName();
        Visual* parent = GetVisualParent();
        
        if (!parent)
        {
            return members;
        }
        
        // 递归查找同组�?RadioButton
        std::function<void(UIElement*)> findRadioButtons = [&](UIElement* element)
        {
            if (!element) return;
            
            // 检查当前元素是否为 RadioButton
            if (auto* radioBtn = dynamic_cast<RadioButton*>(element))
            {
                // 检查是否在同一�?
                if (groupName.empty())
                {
                    // 如果 GroupName 为空，同一父容器下的所�?RadioButton 视为同组
                    if (radioBtn->GetVisualParent() == parent)
                    {
                        members.push_back(radioBtn);
                    }
                }
                else
                {
                    // 如果 GroupName 不为空，比较组名
                    if (radioBtn->GetGroupName() == groupName)
                    {
                        members.push_back(radioBtn);
                    }
                }
            }
            
            // 递归查找视觉子元�?
            if (auto* visual = dynamic_cast<Visual*>(element))
            {
                for (size_t i = 0; i < visual->GetVisualChildrenCount(); ++i)
                {
                    if (auto* child = visual->GetVisualChild(i))
                    {
                        if (auto* childElement = dynamic_cast<UIElement*>(child))
                        {
                            findRadioButtons(childElement);
                        }
                    }
                }
            }
        };
        
        // 从父容器开始查�?
        // 如果�?GroupName，则从根开始查找；否则只在父容器内查找
        if (groupName.empty())
        {
            if (auto* parentElement = dynamic_cast<UIElement*>(parent))
            {
                findRadioButtons(parentElement);
            }
        }
        else
        {
            // 找到根元�?
            Visual* root = parent;
            while (root->GetVisualParent())
            {
                root = root->GetVisualParent();
            }
            if (auto* rootElement = dynamic_cast<UIElement*>(root))
            {
                findRadioButtons(rootElement);
            }
        }
        
        return members;
    }

} // namespace fk::ui
