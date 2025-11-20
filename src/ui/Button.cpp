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

    // 创建 Button 的默认 ControlTemplate（使用链式API定义视觉状态）
    // 设计说明：
    // - 模板中定义状态结构和动画参数（duration等）
    // - 使用 ToBinding() 将 To 值绑定到 Button 的属性（类似 WPF 的 TemplateBinding）
    // - 运行时在 ResolveVisualStateTargets() 中自动从绑定的属性获取颜色值
    // - 这是真正的属性绑定机制，不是事后替换
    static ControlTemplate *CreateDefaultButtonTemplate()
    {
        auto *tmpl = new ControlTemplate();
        tmpl->SetTargetType(typeid(Button))
            ->SetFactory([]() -> UIElement *
                         { return (new Border())
                               ->Name("RootBorder") // 链式设置名称
                               ->Background(binding::TemplateBinding(Control<Button>::BackgroundProperty()))
                               ->BorderBrush(binding::TemplateBinding(Control<Button>::BorderBrushProperty()))
                               ->BorderThickness(binding::TemplateBinding(Control<Button>::BorderThicknessProperty()))
                               ->Padding(10.0f, 5.0f, 10.0f, 5.0f)
                               ->CornerRadius(4.0f)
                               ->Child(
                                   (new ContentPresenter<>())
                                       ->SetHAlign(HorizontalAlignment::Center)
                                       ->SetVAlign(VerticalAlignment::Center)); })
            ->AddVisualStateGroup(
                animation::VisualStateBuilder::CreateGroup("CommonStates")
                    ->State("Normal")
                    ->ColorAnimation("RootBorder", "Background.Color")
                    ->ToBinding(Control<Button>::BackgroundProperty()) // 绑定到 Background 属性！
                    ->Duration(75)
                    ->EndAnimation()
                    ->EndState()
                    ->State("MouseOver")
                    ->ColorAnimation("RootBorder", "Background.Color")
                    ->ToBinding(Button::MouseOverBackgroundProperty()) // 绑定到 MouseOverBackground 属性！
                    ->Duration(50)
                    ->EndAnimation()
                    ->EndState()
                    ->State("Pressed")
                    ->ColorAnimation("RootBorder", "Background.Color")
                    ->ToBinding(Button::PressedBackgroundProperty()) // 绑定到 PressedBackground 属性！
                    ->Duration(50)
                    ->EndAnimation()
                    ->EndState()
                    ->State("Disabled")
                    ->ColorAnimation("RootBorder", "Background.Color")
                    ->To(Color::FromRGB(200, 200, 200, 255)) // Disabled 使用固定灰色
                    ->Duration(100)
                    ->EndAnimation()
                    ->DoubleAnimation("RootBorder", "Opacity")
                    ->To(0.6)
                    ->Duration(100)
                    ->EndAnimation()
                    ->EndState()
                    ->Build());

        return tmpl;
    }

    Button::Button()
    {
        // 设置默认对齐方式：Button 不应该拉伸，而是根据内容大小决定
        SetHorizontalAlignment(HorizontalAlignment::Left);
        SetVerticalAlignment(VerticalAlignment::Top);

        // 设置默认背景色（WinUI3 Fluent Accent Blue）
        // 用户可以通过 Background() 方法覆盖此默认值
        if (!GetBackground())
        {
            SetBackground(new SolidColorBrush(Color::FromRGB(0, 120, 212, 255))); // WinUI3 Accent Blue #0078D4
        }

        if (!GetBorderBrush())
        {
            SetBorderBrush(new SolidColorBrush(Color::FromRGB(0, 120, 212, 255))); // 与背景相同
        }

        // 设置默认模板
        if (!GetTemplate())
        {
            SetTemplate(CreateDefaultButtonTemplate());
        }

        // 注意：视觉状态初始化在 OnTemplateApplied() 中进行
        // 因为需要等待模板应用后才能访问 Border 等子元素
    }

} // namespace fk::ui
