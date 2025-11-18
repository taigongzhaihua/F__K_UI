/**
 * @file button_visual_states_simple_test.cpp
 * @brief 简单测试Button的视觉状态系统
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

int main() {
    std::cout << "测试1: 使用默认视觉状态的Button\n";
    
    try {
        std::cout << "  创建Button对象...\n";
        auto button1 = new ui::Button();
        std::cout << "  设置Content...\n";
        button1->Content("默认按钮");
        std::cout << "✓ 创建默认Button成功\n";
        delete button1;
        std::cout << "✓ Button删除成功\n\n";
    } catch (const std::exception& e) {
        std::cerr << "❌ 错误: " << e.what() << "\n";
        return 1;
    }
    
    std::cout << "测试2: 使用VisualStateBuilder创建状态组\n";
    
    try {
        auto dummyBrush = std::make_shared<ui::SolidColorBrush>(ui::Color::FromRGB(255, 255, 255));
        
        auto stateGroup = animation::VisualStateBuilder::CreateGroup("CommonStates")
            ->State("Normal")
                ->ColorAnimation(dummyBrush.get(), &ui::SolidColorBrush::ColorProperty())
                    ->To(ui::Color::FromRGB(255, 200, 200))
                    ->Duration(250)
                ->EndAnimation()
            ->EndState()
            ->State("MouseOver")
                ->ColorAnimation(dummyBrush.get(), &ui::SolidColorBrush::ColorProperty())
                    ->To(ui::Color::FromRGB(255, 150, 150))
                    ->Duration(200)
                ->EndAnimation()
            ->EndState()
            ->Build();
        
        std::cout << "✓ VisualStateBuilder工作正常\n";
        std::cout << "  状态组名称: " << stateGroup->GetName() << "\n";
        std::cout << "  状态数量: " << stateGroup->GetStates().size() << "\n\n";
        
    } catch (const std::exception& e) {
        std::cerr << "❌ 错误: " << e.what() << "\n";
        return 1;
    }
    
    std::cout << "测试3: 创建包含视觉状态的ControlTemplate\n";
    
    try {
        auto tmpl = std::make_unique<ui::ControlTemplate>();
        tmpl->SetTargetType(typeid(ui::Button));
        
        tmpl->SetFactory([]() -> ui::UIElement* {
            auto border = new ui::Border();
            border->Background(new ui::SolidColorBrush(ui::Color::FromRGB(240, 240, 240)));
            border->BorderBrush(new ui::SolidColorBrush(ui::Color::FromRGB(100, 100, 200)));
            border->BorderThickness(2.0f);
            border->Padding(15.0f, 8.0f, 15.0f, 8.0f);
            border->CornerRadius(8.0f);
            
            auto presenter = new ui::ContentPresenter<>();
            presenter->SetHAlign(ui::HorizontalAlignment::Center);
            presenter->SetVAlign(ui::VerticalAlignment::Center);
            
            border->Child(presenter);
            return border;
        });
        
        auto dummyBrush = std::make_shared<ui::SolidColorBrush>(ui::Color::FromRGB(255, 255, 255));
        
        auto stateGroup = animation::VisualStateBuilder::CreateGroup("CommonStates")
            ->State("Normal")
                ->ColorAnimation(dummyBrush.get(), &ui::SolidColorBrush::ColorProperty())
                    ->To(ui::Color::FromRGB(255, 200, 200))
                    ->Duration(250)
                ->EndAnimation()
            ->EndState()
            ->Build();
        
        tmpl->AddVisualStateGroup(stateGroup);
        
        std::cout << "✓ ControlTemplate创建成功\n";
        std::cout << "  包含视觉状态: " << (tmpl->HasVisualStates() ? "是" : "否") << "\n";
        std::cout << "  状态组数量: " << tmpl->GetVisualStateGroups().size() << "\n\n";
        
    } catch (const std::exception& e) {
        std::cerr << "❌ 错误: " << e.what() << "\n";
        return 1;
    }
    
    std::cout << "✓ 所有测试通过！\n";
    return 0;
}
