/**
 * @file button_custom_visual_states_demo.cpp
 * @brief 演示如何在ControlTemplate中自定义Button的视觉状态
 * 
 * 这个示例展示了三种方式来定义Button的视觉状态：
 * 1. 使用默认状态（Button内置）
 * 2. 使用VisualStateBuilder在模板中声明式定义状态
 * 3. 手动创建VisualState对象并添加到模板
 */

#include <iostream>
#include <memory>

#include "fk/ui/Button.h"
#include "fk/ui/Border.h"
#include "fk/ui/ContentPresenter.h"
#include "fk/ui/ControlTemplate.h"
#include "fk/ui/DrawCommand.h"
#include "fk/animation/VisualStateBuilder.h"
#include "fk/animation/VisualStateManager.h"
#include "fk/animation/VisualState.h"
#include "fk/animation/VisualStateGroup.h"
#include "fk/animation/Storyboard.h"
#include "fk/animation/ColorAnimation.h"
#include "fk/binding/TemplateBinding.h"

using namespace fk;
using namespace std::chrono_literals;

void PrintSeparator(const std::string& title) {
    std::cout << "\n" << std::string(70, '=') << "\n";
    std::cout << " " << title << "\n";
    std::cout << std::string(70, '=') << "\n\n";
}

/**
 * 示例1：使用默认视觉状态
 * Button会自动使用内置的默认状态
 */
void Example1_DefaultVisualStates() {
    PrintSeparator("示例1: 使用默认视觉状态");
    
    auto button = std::make_unique<ui::Button>();
    button->Content("默认样式按钮");
    
    std::cout << "创建了一个使用默认视觉状态的Button\n";
    std::cout << "默认状态包括：\n";
    std::cout << "  - Normal:    浅灰色背景 RGB(240, 240, 240)\n";
    std::cout << "  - MouseOver: 浅蓝色背景 RGB(229, 241, 251)\n";
    std::cout << "  - Pressed:   深蓝色背景 RGB(204, 228, 247)\n";
    std::cout << "  - Disabled:  灰色背景 RGB(200, 200, 200)\n";
    std::cout << "\n这些状态是在Button类内部定义的。\n";
}

/**
 * 示例2：使用VisualStateBuilder在模板中定义自定义状态
 * 
 * 这是推荐的方式，类似于WPF中在XAML中定义VisualStateManager.VisualStateGroups
 */
void Example2_CustomStatesWithBuilder() {
    PrintSeparator("示例2: 使用VisualStateBuilder自定义视觉状态");
    
    std::cout << "创建自定义ControlTemplate，并使用VisualStateBuilder定义状态...\n\n";
    
    // 创建模板
    auto tmpl = new ui::ControlTemplate();
    tmpl->SetTargetType(typeid(ui::Button));
    
    // 定义视觉树（使用工厂函数）
    tmpl->SetFactory([]() -> ui::UIElement* {
        auto border = new ui::Border();
        border->Background(binding::TemplateBinding(ui::Control<ui::Button>::BackgroundProperty()))
              ->BorderBrush(new ui::SolidColorBrush(ui::Color::FromRGB(100, 100, 200, 255)))  // 蓝色边框
              ->BorderThickness(2.0f)
              ->Padding(15.0f, 8.0f, 15.0f, 8.0f)
              ->CornerRadius(8.0f);  // 更圆的角
        
        auto presenter = new ui::ContentPresenter<>();
        presenter->SetHAlign(ui::HorizontalAlignment::Center)
                 ->SetVAlign(ui::VerticalAlignment::Center);
        
        border->Child(presenter);
        return border;
    });
    
    // 使用VisualStateBuilder创建视觉状态组
    // 注意：这里我们创建一个临时的SolidColorBrush用于演示
    // 在实际使用中，动画会作用于模板实例化后的Border
    auto dummyBrush = new ui::SolidColorBrush(ui::Color::FromRGB(255, 255, 255));
    
    auto stateGroup = animation::VisualStateBuilder::CreateGroup("CommonStates")
        ->State("Normal")
            ->ColorAnimation(dummyBrush, &ui::SolidColorBrush::ColorProperty())
                ->To(ui::Color::FromRGB(255, 200, 200))  // 浅红色
                ->Duration(250)
            ->EndAnimation()
        ->EndState()
        ->State("MouseOver")
            ->ColorAnimation(dummyBrush, &ui::SolidColorBrush::ColorProperty())
                ->To(ui::Color::FromRGB(255, 150, 150))  // 中红色
                ->Duration(200)
            ->EndAnimation()
        ->EndState()
        ->State("Pressed")
            ->ColorAnimation(dummyBrush, &ui::SolidColorBrush::ColorProperty())
                ->To(ui::Color::FromRGB(200, 100, 100))  // 深红色
                ->Duration(100)
            ->EndAnimation()
        ->EndState()
        ->State("Disabled")
            ->ColorAnimation(dummyBrush, &ui::SolidColorBrush::ColorProperty())
                ->To(ui::Color::FromRGB(180, 180, 180))  // 灰色
                ->Duration(250)
            ->EndAnimation()
        ->EndState()
        ->Build();
    
    // 将视觉状态组添加到模板
    tmpl->AddVisualStateGroup(stateGroup);
    
    std::cout << "✓ 创建了自定义视觉状态组 'CommonStates'\n";
    std::cout << "  包含4个状态：Normal, MouseOver, Pressed, Disabled\n";
    std::cout << "  使用红色系配色方案，而不是默认的灰蓝色\n\n";
    
    // 创建Button并应用自定义模板
    auto button = std::make_unique<ui::Button>();
    button->Content("自定义红色主题按钮");
    button->SetTemplate(tmpl);
    
    std::cout << "✓ Button已应用自定义模板\n";
    std::cout << "  当用户交互时，按钮会显示红色系的视觉反馈\n";
    
    delete dummyBrush;
}

/**
 * 示例3：手动创建VisualState对象
 * 
 * 这种方式提供了最大的灵活性，但代码较为冗长
 */
void Example3_ManualStateCreation() {
    PrintSeparator("示例3: 手动创建VisualState对象");
    
    std::cout << "手动创建视觉状态（最灵活但代码较多）...\n\n";
    
    // 创建模板
    auto tmpl = new ui::ControlTemplate();
    tmpl->SetTargetType(typeid(ui::Button));
    
    // 定义视觉树
    tmpl->SetFactory([]() -> ui::UIElement* {
        auto border = new ui::Border();
        border->Background(binding::TemplateBinding(ui::Control<ui::Button>::BackgroundProperty()))
              ->BorderBrush(new ui::SolidColorBrush(ui::Color::FromRGB(50, 150, 50, 255)))  // 绿色边框
              ->BorderThickness(2.0f)
              ->Padding(12.0f, 6.0f, 12.0f, 6.0f)
              ->CornerRadius(4.0f);
        
        auto presenter = new ui::ContentPresenter<>();
        presenter->SetHAlign(ui::HorizontalAlignment::Center)
                 ->SetVAlign(ui::VerticalAlignment::Center);
        
        border->Child(presenter);
        return border;
    });
    
    // 手动创建状态组
    auto commonStates = std::make_shared<animation::VisualStateGroup>("CommonStates");
    
    // 创建Normal状态
    auto normalState = std::make_shared<animation::VisualState>("Normal");
    auto normalStoryboard = std::make_shared<animation::Storyboard>();
    
    auto dummyBrush = new ui::SolidColorBrush(ui::Color::FromRGB(255, 255, 255));
    auto normalAnim = std::make_shared<animation::ColorAnimation>();
    normalAnim->SetTo(ui::Color::FromRGB(200, 255, 200));  // 浅绿色
    normalAnim->SetDuration(animation::Duration(std::chrono::milliseconds(250)));
    normalAnim->SetTarget(dummyBrush, &ui::SolidColorBrush::ColorProperty());
    normalStoryboard->AddChild(normalAnim);
    normalState->SetStoryboard(normalStoryboard);
    commonStates->AddState(normalState);
    
    // 创建MouseOver状态
    auto mouseOverState = std::make_shared<animation::VisualState>("MouseOver");
    auto mouseOverStoryboard = std::make_shared<animation::Storyboard>();
    auto mouseOverAnim = std::make_shared<animation::ColorAnimation>();
    mouseOverAnim->SetTo(ui::Color::FromRGB(150, 255, 150));  // 中绿色
    mouseOverAnim->SetDuration(animation::Duration(std::chrono::milliseconds(200)));
    mouseOverAnim->SetTarget(dummyBrush, &ui::SolidColorBrush::ColorProperty());
    mouseOverStoryboard->AddChild(mouseOverAnim);
    mouseOverState->SetStoryboard(mouseOverStoryboard);
    commonStates->AddState(mouseOverState);
    
    std::cout << "✓ 手动创建了2个状态：Normal 和 MouseOver\n";
    std::cout << "  这种方式代码较长，但提供完全控制\n";
    std::cout << "  适合需要复杂动画或特殊效果的场景\n\n";
    
    // 将状态组添加到模板
    tmpl->AddVisualStateGroup(commonStates);
    
    // 创建Button
    auto button = std::make_unique<ui::Button>();
    button->Content("自定义绿色主题按钮");
    button->SetTemplate(tmpl);
    
    std::cout << "✓ Button已应用包含手动创建状态的模板\n";
    
    delete dummyBrush;
}

/**
 * 示例4：对比默认状态和自定义状态
 */
void Example4_Comparison() {
    PrintSeparator("示例4: 对比默认状态和自定义状态");
    
    std::cout << "对比分析：\n\n";
    
    std::cout << "1. 默认状态（Button内置）\n";
    std::cout << "   优点：\n";
    std::cout << "     • 无需任何代码，自动生效\n";
    std::cout << "     • 统一的外观和行为\n";
    std::cout << "     • 适合快速原型开发\n";
    std::cout << "   缺点：\n";
    std::cout << "     • 无法自定义颜色和效果\n";
    std::cout << "     • 所有Button看起来一样\n\n";
    
    std::cout << "2. VisualStateBuilder（推荐）\n";
    std::cout << "   优点：\n";
    std::cout << "     • 声明式API，类似WPF/WinUI的XAML\n";
    std::cout << "     • 代码简洁易读\n";
    std::cout << "     • 链式调用，开发效率高\n";
    std::cout << "   适用场景：\n";
    std::cout << "     • 需要自定义按钮主题\n";
    std::cout << "     • 定义多个相似控件的样式\n";
    std::cout << "     • 创建可重用的控件模板\n\n";
    
    std::cout << "3. 手动创建（最灵活）\n";
    std::cout << "   优点：\n";
    std::cout << "     • 完全控制每个细节\n";
    std::cout << "     • 可以实现复杂的动画组合\n";
    std::cout << "     • 适合高级定制需求\n";
    std::cout << "   缺点：\n";
    std::cout << "     • 代码冗长\n";
    std::cout << "     • 需要深入理解动画系统\n\n";
}

/**
 * 示例5：演示如何在模板中定义状态并在多个Button实例中复用
 */
void Example5_TemplateReuse() {
    PrintSeparator("示例5: 模板复用");
    
    std::cout << "创建一个可复用的ControlTemplate...\n\n";
    
    // 创建可复用的模板
    auto sharedTemplate = new ui::ControlTemplate();
    sharedTemplate->SetTargetType(typeid(ui::Button));
    
    sharedTemplate->SetFactory([]() -> ui::UIElement* {
        auto border = new ui::Border();
        border->Background(binding::TemplateBinding(ui::Control<ui::Button>::BackgroundProperty()))
              ->BorderBrush(new ui::SolidColorBrush(ui::Color::FromRGB(255, 165, 0, 255)))  // 橙色边框
              ->BorderThickness(3.0f)
              ->Padding(20.0f, 10.0f, 20.0f, 10.0f)
              ->CornerRadius(10.0f);
        
        auto presenter = new ui::ContentPresenter<>();
        presenter->SetHAlign(ui::HorizontalAlignment::Center)
                 ->SetVAlign(ui::VerticalAlignment::Center);
        
        border->Child(presenter);
        return border;
    });
    
    // 使用Builder定义橙色主题的视觉状态
    auto dummyBrush = new ui::SolidColorBrush(ui::Color::FromRGB(255, 255, 255));
    
    auto orangeTheme = animation::VisualStateBuilder::CreateGroup("CommonStates")
        ->State("Normal")
            ->ColorAnimation(dummyBrush, &ui::SolidColorBrush::ColorProperty())
                ->To(ui::Color::FromRGB(255, 220, 180))  // 浅橙色
                ->Duration(200)
            ->EndAnimation()
        ->EndState()
        ->State("MouseOver")
            ->ColorAnimation(dummyBrush, &ui::SolidColorBrush::ColorProperty())
                ->To(ui::Color::FromRGB(255, 180, 100))  // 中橙色
                ->Duration(150)
            ->EndAnimation()
        ->EndState()
        ->State("Pressed")
            ->ColorAnimation(dummyBrush, &ui::SolidColorBrush::ColorProperty())
                ->To(ui::Color::FromRGB(230, 140, 50))   // 深橙色
                ->Duration(100)
            ->EndAnimation()
        ->EndState()
        ->Build();
    
    sharedTemplate->AddVisualStateGroup(orangeTheme);
    
    std::cout << "✓ 创建了橙色主题的共享模板\n\n";
    
    // 创建多个Button实例，共享同一个模板
    auto button1 = std::make_unique<ui::Button>();
    button1->Content("按钮 1");
    button1->SetTemplate(sharedTemplate);
    
    auto button2 = std::make_unique<ui::Button>();
    button2->Content("按钮 2");
    button2->SetTemplate(sharedTemplate);
    
    auto button3 = std::make_unique<ui::Button>();
    button3->Content("按钮 3");
    button3->SetTemplate(sharedTemplate);
    
    std::cout << "✓ 创建了3个Button，都使用相同的橙色主题模板\n";
    std::cout << "  所有按钮将具有统一的外观和交互效果\n";
    std::cout << "  这展示了ControlTemplate的复用能力\n";
    
    delete dummyBrush;
}

int main() {
    std::cout << "╔══════════════════════════════════════════════════════════════════╗\n";
    std::cout << "║         Button自定义视觉状态演示程序                              ║\n";
    std::cout << "║   Demonstrating Custom Visual States in ControlTemplate         ║\n";
    std::cout << "╚══════════════════════════════════════════════════════════════════╝\n";
    
    try {
        Example1_DefaultVisualStates();
        Example2_CustomStatesWithBuilder();
        Example3_ManualStateCreation();
        Example4_Comparison();
        Example5_TemplateReuse();
        
        PrintSeparator("总结");
        
        std::cout << "本示例展示了如何在ControlTemplate中定义Button的视觉状态：\n\n";
        
        std::cout << "主要特性：\n";
        std::cout << "  ✓ 支持在模板中定义视觉状态（类似WPF的XAML）\n";
        std::cout << "  ✓ 提供VisualStateBuilder声明式API\n";
        std::cout << "  ✓ 向后兼容默认状态\n";
        std::cout << "  ✓ 模板可在多个Button实例间复用\n\n";
        
        std::cout << "与WPF/WinUI的对应关系：\n";
        std::cout << "  WPF/WinUI XAML:              C++ F__K_UI:\n";
        std::cout << "  <ControlTemplate>            new ControlTemplate()\n";
        std::cout << "  <VisualStateManager.         ->AddVisualStateGroup(\n";
        std::cout << "    VisualStateGroups>           VisualStateBuilder::CreateGroup(...)\n";
        std::cout << "    <VisualStateGroup>           ->State(\"Normal\")\n";
        std::cout << "      <VisualState>                ->ColorAnimation(...)\n";
        std::cout << "        <Storyboard>                 ->To(...)->Duration(...)\n";
        std::cout << "          <ColorAnimation/>          ->EndAnimation()\n";
        std::cout << "        </Storyboard>              ->EndState()\n";
        std::cout << "      </VisualState>               ->Build()\n";
        std::cout << "    </VisualStateGroup>          )\n";
        std::cout << "  </VisualStateManager...>\n\n";
        
        std::cout << "✓ 所有示例运行完成！\n\n";
        
    } catch (const std::exception& e) {
        std::cerr << "\n❌ 错误: " << e.what() << "\n";
        return 1;
    }
    
    return 0;
}
