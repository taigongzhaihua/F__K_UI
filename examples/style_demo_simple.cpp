/**
 * @file style_demo_simple.cpp
 * @brief 简化的 Style 系统演示
 * 
 * 演示：
 * 1. 创建 Style 并设置属性
 * 2. 应用 Style 到控件
 * 3. Style 继承 (BasedOn)
 */

#include "fk/app/Application.h"
#include "fk/ui/Window.h"
#include "fk/ui/Button.h"
#include "fk/ui/StackPanel.h"
#include "fk/ui/Style.h"
#include <iostream>

using namespace fk;

int main() {
    // 创建应用
    app::Application app;
    
    // 创建窗口
    auto window = new ui::Window();
    window->Title("Style System Demo")
          ->Width(600)
          ->Height(500);
    
    // 创建容器
    auto stackPanel = new ui::StackPanel();
    stackPanel->Orientation(ui::Orientation::Vertical)
              ->Spacing(15)
              ->HorizontalAlignment(ui::HorizontalAlignment::Center)
              ->VerticalAlignment(ui::VerticalAlignment::Center);
    
    // ========== 创建基础按钮样式 ==========
    auto baseStyle = new ui::Style();
    baseStyle->SetTargetType(typeid(ui::Button));
    
    // 使用 Add(property, value) 方法添加设置器
    baseStyle->Setters().Add(
        ui::FrameworkElement<ui::Button>::WidthProperty(),
        250.0f
    );
    
    baseStyle->Setters().Add(
        ui::FrameworkElement<ui::Button>::HeightProperty(),
        60.0f
    );
    
    baseStyle->Setters().Add(
        ui::Control<ui::Button>::FontSizeProperty(),
        18.0f
    );
    
    baseStyle->Setters().Add(
        ui::Control<ui::Button>::PaddingProperty(),
        Thickness(10)
    );
    
    baseStyle->Seal();
    
    // ========== 创建大号样式（继承自基础样式）==========
    auto largeStyle = new ui::Style();
    largeStyle->SetTargetType(typeid(ui::Button));
    largeStyle->SetBasedOn(baseStyle);  // 继承基础样式
    
    // 覆盖字体大小
    largeStyle->Setters().Add(
        ui::Control<ui::Button>::FontSizeProperty(),
        24.0f
    );
    
    // 覆盖高度
    largeStyle->Setters().Add(
        ui::FrameworkElement<ui::Button>::HeightProperty(),
        80.0f
    );
    
    largeStyle->Seal();
    
    // ========== 创建粗体样式（继承自基础样式）==========
    auto boldStyle = new ui::Style();
    boldStyle->SetTargetType(typeid(ui::Button));
    boldStyle->SetBasedOn(baseStyle);
    
    boldStyle->Setters().Add(
        ui::Control<ui::Button>::FontWeightProperty(),
        ui::FontWeight::Bold
    );
    
    boldStyle->Setters().Add(
        ui::Control<ui::Button>::FontSizeProperty(),
        20.0f
    );
    
    boldStyle->Seal();
    
    // ========== 创建按钮并应用样式 ==========
    
    // 按钮 1：基础样式
    auto button1 = new ui::Button();
    button1->Content("Base Style Button")
           ->StyleProperty(baseStyle);
    stackPanel->AddChild(button1);
    
    // 按钮 2：大号样式
    auto button2 = new ui::Button();
    button2->Content("Large Font Button")
           ->StyleProperty(largeStyle);
    stackPanel->AddChild(button2);
    
    // 按钮 3：粗体样式
    auto button3 = new ui::Button();
    button3->Content("Bold Button")
           ->StyleProperty(boldStyle);
    stackPanel->AddChild(button3);
    
    // 按钮 4：默认样式
    auto button4 = new ui::Button();
    button4->Content("Default Button")
           ->Width(250)
           ->Height(60);
    stackPanel->AddChild(button4);
    
    window->Content(stackPanel);
    
    // 打印调试信息
    std::cout << "\n=== Style System Demo ===" << std::endl;
    std::cout << "\nBase Style:" << std::endl;
    std::cout << "  Setters: " << baseStyle->Setters().Count() << std::endl;
    std::cout << "  Sealed: " << (baseStyle->IsSealed() ? "Yes" : "No") << std::endl;
    
    std::cout << "\nLarge Style:" << std::endl;
    std::cout << "  Setters: " << largeStyle->Setters().Count() << std::endl;
    std::cout << "  BasedOn: " << (largeStyle->GetBasedOn() ? "Base Style" : "None") << std::endl;
    std::cout << "  Sealed: " << (largeStyle->IsSealed() ? "Yes" : "No") << std::endl;
    
    std::cout << "\nBold Style:" << std::endl;
    std::cout << "  Setters: " << boldStyle->Setters().Count() << std::endl;
    std::cout << "  BasedOn: " << (boldStyle->GetBasedOn() ? "Base Style" : "None") << std::endl;
    std::cout << "  Sealed: " << (boldStyle->IsSealed() ? "Yes" : "No") << std::endl;
    
    std::cout << "\nButton Styles:" << std::endl;
    std::cout << "  Button1: " << (button1->GetStyle() ? "Applied" : "None") << std::endl;
    std::cout << "  Button2: " << (button2->GetStyle() ? "Applied" : "None") << std::endl;
    std::cout << "  Button3: " << (button3->GetStyle() ? "Applied" : "None") << std::endl;
    std::cout << "  Button4: " << (button4->GetStyle() ? "Applied" : "None") << std::endl;
    std::cout << "\n=========================" << std::endl;
    
    // 运行应用
    return app.Run(window);
}
