#include "fk/app/Application.h"
#include "fk/ui/Window.h"
#include "fk/ui/Button.h"
#include "fk/ui/TextBlock.h"
#include "fk/ui/StackPanel.h"
#include "fk/ui/Template.h"

#include <iostream>

using namespace fk;
using namespace fk::ui;

int main() {
    Application application;
    
    // 创建主窗口
    auto window = ui::window()
        ->Title("ControlTemplate 演示")
        ->Width(600.0f)
        ->Height(400.0f);
    
    // 创建主面板
    auto root = ui::stackPanel()
        ->Orientation(Orientation::Vertical);
    root->SetPadding(fk::Thickness{20.0f});
    
    // 🎯 创建自定义按钮模板
    auto customButtonTemplate = std::make_shared<ControlTemplate>();
    customButtonTemplate->SetVisualTreeFactory([]() -> std::shared_ptr<FrameworkElement> {
        auto panel = ui::stackPanel()
            ->Background("#4CAF50");
        panel->SetPadding(fk::Thickness{16.0f, 12.0f});
        panel->SetHorizontalAlignment(HorizontalAlignment::Center);
        panel->SetVerticalAlignment(VerticalAlignment::Center);
        
        auto text = ui::textBlock()
            ->Text("✨ 自定义模板按钮 ✨")
            ->Foreground("#FFFFFF")
            ->FontSize(18.0f);
        text->SetHorizontalAlignment(HorizontalAlignment::Center);
        text->SetVerticalAlignment(VerticalAlignment::Center);
        
        panel->AddChild(text);
        return panel;
    });
    
    // 创建使用自定义模板的按钮
    auto customButton = ui::button()
        ->Width(250.0f)
        ->Height(60.0f)
        ->Template(customButtonTemplate);
    customButton->SetMargin(fk::Thickness{0, 10, 0, 10});
    
    // 创建默认按钮作为对比
    auto defaultButton = ui::button()
        ->Width(250.0f)
        ->Height(60.0f);
    auto defaultText = ui::textBlock()->Text("默认样式按钮")->FontSize(16.0f);
    defaultButton->SetContent(defaultText);
    defaultButton->SetMargin(fk::Thickness{0, 10, 0, 10});
    
    // 添加说明文本
    auto label1 = ui::textBlock()
        ->Text("使用自定义 ControlTemplate 的按钮：")
        ->FontSize(14.0f)
        ->Foreground("#333333");
    label1->SetMargin(fk::Thickness{0, 10, 0, 5});
    
    auto label2 = ui::textBlock()
        ->Text("使用默认样式的按钮：")
        ->FontSize(14.0f)
        ->Foreground("#333333");
    label2->SetMargin(fk::Thickness{0, 20, 0, 5});
    
    root->AddChild(label1);
    root->AddChild(customButton);
    root->AddChild(label2);
    root->AddChild(defaultButton);
    
    window->SetContent(root);
    window->Show();
    
    return application.Run();
}
