#include "fk/ui/CheckBox.h"
#include "fk/ui/Control.h"
#include "fk/ui/ControlTemplate.h"
#include "fk/ui/Border.h"
#include "fk/ui/ContentPresenter.h"
#include "fk/ui/StackPanel.h"
#include "fk/ui/Grid.h"
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
                // 使用 Grid 来组织复选框和内容
                auto *rootGrid = (new Grid())
                    ->Columns("Auto, *"); // 第一列自动大小（复选框），第二列填充剩余空间（内容）
                
                // 复选框本身 - 一个带边框的方框
                auto *checkBoxBorder = (new Border())
                    ->Name("CheckBoxBorder")
                    ->Width(20.0f)
                    ->Height(20.0f)
                    ->BorderThickness(2.0f)
                    ->CornerRadius(3.0f)
                    ->BorderBrush(binding::TemplateBinding(CheckBox::CheckBoxBorderColorProperty()))
                    ->Background(new SolidColorBrush(Color::FromRGB(255, 255, 255, 255))) // 白色背景
                    ->SetVAlign(VerticalAlignment::Center)
                    ->SetHAlign(HorizontalAlignment::Center)
                    ->Margin(0.0f, 0.0f, 8.0f, 0.0f); // 右边距与内容分隔
                
                // 勾选标记 - 使用 Border 模拟勾选标记（简化版本）
                // 实际应用中可以使用 Path 或自定义形状来绘制更精确的勾选标记
                auto *checkMark = (new Border())
                    ->Name("CheckMark")
                    ->Width(12.0f)
                    ->Height(12.0f)
                    ->Background(binding::TemplateBinding(CheckBox::CheckMarkColorProperty()))
                    ->CornerRadius(2.0f)
                    ->Opacity(0.0) // 初始不可见
                    ->SetVAlign(VerticalAlignment::Center)
                    ->SetHAlign(HorizontalAlignment::Center);
                
                checkBoxBorder->Child(checkMark);
                
                // 内容呈现器
                auto *contentPresenter = (new ContentPresenter<>())
                    ->SetVAlign(VerticalAlignment::Center)
                    ->SetHAlign(HorizontalAlignment::Left);
                
                // 将复选框和内容添加到 Grid
                rootGrid->AddChild(checkBoxBorder);
                rootGrid->AddChild(contentPresenter);
                
                // 设置 Grid 列位置
                GridCellAttacher::SetColumn(checkBoxBorder, 0);
                GridCellAttacher::SetColumn(contentPresenter, 1);
                
                return rootGrid; })
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
                    // 鼠标悬停时边框高亮
                    ->ColorAnimation("CheckBoxBorder", "BorderBrush.Color")
                    ->To(Color::FromRGB(0, 120, 215, 255)) // 蓝色高亮
                    ->Duration(100)
                    ->EndAnimation()
                    ->EndState()
                    ->State("Pressed")
                    // 按下时边框更深
                    ->ColorAnimation("CheckBoxBorder", "BorderBrush.Color")
                    ->To(Color::FromRGB(0, 90, 158, 255)) // 深蓝色
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
            ->AddVisualStateGroup(
                animation::VisualStateBuilder::CreateGroup("CheckStates")
                    ->State("Unchecked")
                    // 未选中状态 - 隐藏勾选标记
                    ->DoubleAnimation("CheckMark", "Opacity")
                    ->To(0.0)
                    ->Duration(150)
                    ->EndAnimation()
                    ->ColorAnimation("CheckBoxBorder", "Background.Color")
                    ->To(Color::FromRGB(255, 255, 255, 255)) // 白色背景
                    ->Duration(150)
                    ->EndAnimation()
                    ->EndState()
                    ->State("Checked")
                    // 选中状态 - 显示勾选标记，背景变色
                    ->DoubleAnimation("CheckMark", "Opacity")
                    ->To(1.0)
                    ->Duration(150)
                    ->EndAnimation()
                    ->ColorAnimation("CheckBoxBorder", "Background.Color")
                    ->ToBinding(CheckBox::CheckedBackgroundProperty()) // 使用选中背景色
                    ->Duration(150)
                    ->EndAnimation()
                    ->ColorAnimation("CheckBoxBorder", "BorderBrush.Color")
                    ->ToBinding(CheckBox::CheckedBackgroundProperty()) // 边框也使用选中色
                    ->Duration(150)
                    ->EndAnimation()
                    ->EndState()
                    ->State("Indeterminate")
                    // 不确定状态 - 部分显示标记
                    ->DoubleAnimation("CheckMark", "Opacity")
                    ->To(0.6)
                    ->Duration(150)
                    ->EndAnimation()
                    ->ColorAnimation("CheckBoxBorder", "Background.Color")
                    ->To(Color::FromRGB(200, 200, 200, 255)) // 灰色表示不确定
                    ->Duration(150)
                    ->EndAnimation()
                    ->ColorAnimation("CheckBoxBorder", "BorderBrush.Color")
                    ->To(Color::FromRGB(150, 150, 150, 255))
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
        if (!GetTemplate())
        {
            SetTemplate(CreateDefaultCheckBoxTemplate());
        }
    }

} // namespace fk::ui
