#include "fk/ui/CheckBox.h"
#include "fk/ui/Control.h"
#include "fk/ui/ControlTemplate.h"
#include "fk/ui/Border.h"
#include "fk/ui/ContentPresenter.h"
#include "fk/ui/StackPanel.h"
#include "fk/ui/Grid.h"
#include "fk/ui/GridCellAttacher.h"
#include "fk/ui/Brush.h"
#include "fk/ui/Shape.h"
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

    // ========== 依赖属性注册 ==========

    const binding::DependencyProperty &CheckBox::CheckMarkColorProperty()
    {
        static const auto &prop = binding::DependencyProperty::Register(
            "CheckMarkColor",
            typeid(Brush *),
            typeid(CheckBox),
            binding::PropertyMetadata{
                .defaultValue = static_cast<Brush *>(new SolidColorBrush(Color::FromRGB(255, 255, 255, 255))), // 白色勾选标记
                .propertyChangedCallback = [](DependencyObject &d, const DependencyProperty &, const std::any &, const std::any &)
                {
                    auto *checkbox = static_cast<CheckBox *>(&d);
                    if (animation::VisualStateManager::GetVisualStateManager(checkbox))
                    {
                        checkbox->InitializeToggleVisualStates();
                    }
                }});
        return prop;
    }

    const binding::DependencyProperty &CheckBox::CheckBoxBorderColorProperty()
    {
        static const auto &prop = binding::DependencyProperty::Register(
            "CheckBoxBorderColor",
            typeid(Brush *),
            typeid(CheckBox),
            binding::PropertyMetadata{
                .defaultValue = static_cast<Brush *>(new SolidColorBrush(Color::FromRGB(120, 120, 120, 255))), // 灰色边框
                .propertyChangedCallback = [](DependencyObject &d, const DependencyProperty &, const std::any &, const std::any &)
                {
                    auto *checkbox = static_cast<CheckBox *>(&d);
                    if (animation::VisualStateManager::GetVisualStateManager(checkbox))
                    {
                        checkbox->InitializeToggleVisualStates();
                    }
                }});
        return prop;
    }

    // 创建 CheckBox 的默认 ControlTemplate
    static ControlTemplate *CreateDefaultCheckBoxTemplate()
    {
        auto *tmpl = new ControlTemplate();
        tmpl->SetTargetType(typeid(CheckBox))
            ->SetFactory([]() -> UIElement *
                         {
                // StackPanel (水平布局)
                //   ├─ CheckBoxBorder (20x20 方框，灰色边框，白色背景)
                //   │    └─ CheckMark (12x12 外层容器，初始隐藏)
                //   │         └─ CheckMarkContent (16x16 蓝色方块，实际内容)
                //   └─ ContentPresenter (显示文本标签)
                
                return (new StackPanel())
                    ->SetOrient(Orientation::Horizontal)
                    ->SetVAlign(VerticalAlignment::Center)
                    ->Children({
                        (new Border())
                            ->Name("CheckBoxBorder")
                            ->Width(20.0f)
                            ->Height(20.0f)
                            ->BorderThickness(2.0f)
                            ->CornerRadius(3.0f)
                            ->BorderBrush(new SolidColorBrush(Color::FromRGB(120, 120, 120, 255)))
                            ->Background(new SolidColorBrush(Color::FromRGB(255, 255, 255, 255)))
                            ->Margin(0.0f, 0.0f, 8.0f, 0.0f)
                            ->Child(
                                (new Border())
                                    ->Name("CheckMark")
                                    ->Width(12.0f)
                                    ->Height(12.0f)
                                    ->SetVAlign(VerticalAlignment::Center)
                                    ->SetHAlign(HorizontalAlignment::Center)
                                    ->Opacity(0.0)  // 初始隐藏
                                    ->Child(
                                        (new Border())
                                            ->Width(12.0f)
                                            ->Height(12.0f)
                                            ->Background(new SolidColorBrush(Color::FromRGB(0, 120, 215, 255)))
                                            ->CornerRadius(2.0f)
                                    )
                            ),
                        (new ContentPresenter<>())
                            ->SetVAlign(VerticalAlignment::Center)
                    }); })
            // CommonStates 状态组（鼠标交互效果）
            ->AddVisualStateGroup(
                animation::VisualStateBuilder::CreateGroup("CommonStates")
                    ->State("Normal")
                    ->ColorAnimation("CheckBoxBorder", "BorderBrush.Color")
                    ->ToBinding(CheckBox::CheckBoxBorderColorProperty())
                    ->Duration(150)
                    ->EndAnimation()
                    ->EndState()
                    ->State("MouseOver")
                    // 鼠标悬停时边框高亮和背景变化
                    ->ColorAnimation("CheckBoxBorder", "Background.Color")
                    ->To(Color::FromRGB(240, 248, 255, 255)) // 淡蓝色背景
                    ->Duration(100)
                    ->EndAnimation()
                    ->ColorAnimation("CheckBoxBorder", "BorderBrush.Color")
                    ->To(Color::FromRGB(0, 120, 215, 255)) // 蓝色高亮边框
                    ->Duration(100)
                    ->EndAnimation()
                    ->EndState()
                    ->State("Pressed")
                    // 按下时边框更深
                    ->ColorAnimation("CheckBoxBorder", "BorderBrush.Color")
                    ->To(Color::FromRGB(50, 120, 215, 255)) // 深蓝色
                    ->Duration(50)
                    ->EndAnimation()
                    ->ColorAnimation("CheckBoxBorder", "Background.Color")
                    ->To(Color::FromRGB(230, 240, 250, 255)) // 淡蓝色背景
                    ->Duration(50)
                    ->EndAnimation()
                    ->EndState()
                    ->State("Disabled")
                    // 禁用状态
                    ->ColorAnimation("CheckBoxBorder", "BorderBrush.Color")
                    ->To(Color::FromRGB(200, 200, 200, 255)) // 灰色边框
                    ->Duration(100)
                    ->EndAnimation()
                    ->ColorAnimation("CheckBoxBorder", "Background.Color")
                    ->To(Color::FromRGB(240, 240, 240, 255)) // 浅灰色背景
                    ->Duration(100)
                    ->EndAnimation()
                    ->DoubleAnimation("CheckBoxBorder", "Opacity")
                    ->To(0.5)
                    ->Duration(100)
                    ->EndAnimation()
                    ->EndState()
                    ->Build())
            // CheckStates 状态组（选中状态）
            // 设计原则：只控制勾选标记，不与 CommonStates 冲突
            // CommonStates 负责鼠标交互效果（背景和边框颜色）
            // CheckStates 负责勾选标记的显示/隐藏
            ->AddVisualStateGroup(
                animation::VisualStateBuilder::CreateGroup("CheckStates")
                    ->State("Unchecked")
                    // 未选中 - 隐藏勾选标记
                    ->DoubleAnimation("CheckMark", "Opacity")
                    ->To(0.0)
                    ->Duration(150)
                    ->EndAnimation()
                    ->EndState()
                    ->State("Checked")
                    // 选中 - 显示勾选标记
                    ->DoubleAnimation("CheckMark", "Opacity")
                    ->To(1.0)
                    ->Duration(150)
                    ->EndAnimation()
                    ->EndState()
                    ->State("Indeterminate")
                    // 不确定 - 半透明勾选标记
                    ->DoubleAnimation("CheckMark", "Opacity")
                    ->To(0.5)
                    ->Duration(150)
                    ->EndAnimation()
                    ->EndState()
                    ->Build());

        return tmpl;
    }

    CheckBox::CheckBox()
    {
        // 设置默认对齐方式
        SetHorizontalAlignment(HorizontalAlignment::Left);
        SetVerticalAlignment(VerticalAlignment::Center);

        // 设置默认选中背景色（CheckBox 通常使用蓝色表示选中）
        if (!GetCheckedBackground())
        {
            SetCheckedBackground(new SolidColorBrush(Color::FromRGB(0, 120, 215, 255))); // 蓝色
        }

        // 设置默认模板
        SetTemplate(CreateDefaultCheckBoxTemplate());
    }

    void CheckBox::OnTemplateApplied()
    {
        ToggleButton::OnTemplateApplied();

        // 初始化 CheckBox 特有的视觉状态
        InitializeToggleVisualStates();
    }

    void CheckBox::OnPointerPressed(PointerEventArgs &e)
    {
        ToggleButton::OnPointerPressed(e);
    }

    void CheckBox::OnPointerReleased(PointerEventArgs &e)
    {
        ToggleButton::OnPointerReleased(e);
    }

    void CheckBox::OnClick()
    {
        ToggleButton::OnClick();
    }

} // namespace fk::ui
