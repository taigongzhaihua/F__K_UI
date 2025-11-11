/**
 * @file style_demo.cpp
 * @brief Style 系统演示
 * 
 * 演示内容：
 * 1. 创建 Style 对象
 * 2. 添加 Setter 设置属性
 * 3. 应用 Style 到 Button
 * 4. Style 继承 (BasedOn)
 * 
 * 注意：由于 Brush 类尚未实现，此示例使用简单的数值属性演示
 */

#include "fk/app/Application.h"
#include "fk/ui/Window.h"
#include "fk/ui/Button.h"
#include "fk/ui/StackPanel.h"
#include "fk/ui/Style.h"
#include "fk/ui/Setter.h"
#include <iostream>

using namespace fk;
using namespace fk::ui;

int main() {
    // 创建应用
    fk::app::Application app;
    
    // 创建窗口
    auto window = new Window();
    window->Title("Style System Demo")
          ->Width(600)
          ->Height(500);
    
    // 创建容器
    auto stackPanel = new StackPanel();
    stackPanel->Orientation(Orientation::Vertical)
              ->Spacing(15)
              ->HorizontalAlignment(HorizontalAlignment::Center)
              ->VerticalAlignment(VerticalAlignment::Center);
    
    // ========== 创建基础按钮样式 ==========
    auto baseButtonStyle = new Style();
    baseButtonStyle->SetTargetType(typeid(Button));
    
    // 添加 Setter：宽度
    auto widthSetter = new Setter();
    widthSetter->SetProperty(&FrameworkElement<Button>::WidthProperty());
    widthSetter->SetValue(250.0f);
    baseButtonStyle->Setters().Add(widthSetter);
    
    // 添加 Setter：高度
    auto heightSetter = new Setter();
    heightSetter->SetProperty(&FrameworkElement<Button>::HeightProperty());
    heightSetter->SetValue(60.0f);
    baseButtonStyle->Setters().Add(heightSetter);
    
    // 添加 Setter：字体大小
    auto fontSizeSetter = new Setter();
    fontSizeSetter->SetProperty(&Control<Button>::FontSizeProperty());
    fontSizeSetter->SetValue(18.0f);
    baseButtonStyle->Setters().Add(fontSizeSetter);
    
    // 添加 Setter：内边距
    auto paddingSetter = new Setter();
    paddingSetter->SetProperty(&Control<Button>::PaddingProperty());
    paddingSetter->SetValue(Thickness(10));
    baseButtonStyle->Setters().Add(paddingSetter);
    
    // 封印样式
    baseButtonStyle->Seal();
    
    // ========== 创建大号字体样式（基于基础样式）==========
    auto largeButtonStyle = new Style();
    largeButtonStyle->SetTargetType(typeid(Button));
    largeButtonStyle->SetBasedOn(baseButtonStyle);
    
    // 覆盖字体大小为更大
    auto largeFontSetter = new Setter();
    largeFontSetter->SetProperty(&Control<Button>::FontSizeProperty());
    largeFontSetter->SetValue(24.0f);
    largeButtonStyle->Setters().Add(largeFontSetter);
    
    // 覆盖高度为更大
    auto largeHeightSetter = new Setter();
    largeHeightSetter->SetProperty(&FrameworkElement<Button>::HeightProperty());
    largeHeightSetter->SetValue(80.0f);
    largeButtonStyle->Setters().Add(largeHeightSetter);
    
    largeButtonStyle->Seal();
    
    // ========== 创建粗体样式（基于基础样式）==========
    auto boldButtonStyle = new Style();
    boldButtonStyle->SetTargetType(typeid(Button));
    boldButtonStyle->SetBasedOn(baseButtonStyle);
    
    // 设置粗体字体
    auto boldFontWeightSetter = new Setter();
    boldFontWeightSetter->SetProperty(&Control<Button>::FontWeightProperty());
    boldFontWeightSetter->SetValue(FontWeight::Bold);
    boldButtonStyle->Setters().Add(boldFontWeightSetter);
    
    // 设置字体大小
    auto boldFontSizeSetter = new Setter();
    boldFontSizeSetter->SetProperty(&Control<Button>::FontSizeProperty());
    boldFontSizeSetter->SetValue(20.0f);
    boldButtonStyle->Setters().Add(boldFontSizeSetter);
    
    boldButtonStyle->Seal();
    
    // ========== 创建按钮并应用样式 ==========
    
    // 按钮 1：应用基础样式
    auto button1 = new Button();
    button1->Content("Base Style Button")
           ->StyleProperty(baseButtonStyle);
    stackPanel->Children()->Add(button1);
    
    // 按钮 2：应用大号字体样式
    auto button2 = new Button();
    button2->Content("Large Font Button")
           ->StyleProperty(largeButtonStyle);
    stackPanel->Children()->Add(button2);
    
    // 按钮 3：应用粗体样式
    auto button3 = new Button();
    button3->Content("Bold Button")
           ->StyleProperty(boldButtonStyle);
    stackPanel->Children()->Add(button3);
    
    // 按钮 4：无样式（使用默认）
    auto button4 = new Button();
    button4->Content("Default Button")
           ->Width(250)
           ->Height(60);
    stackPanel->Children()->Add(button4);
    
    // 设置窗口内容
    window->Content(stackPanel);
    
    // 打印调试信息
    std::cout << "=== Style System Demo ===" << std::endl;
    std::cout << "Base Style:" << std::endl;
    std::cout << "  - Setters Count: " << baseButtonStyle->Setters().Count() << std::endl;
    std::cout << "  - Is Sealed: " << (baseButtonStyle->IsSealed() ? "Yes" : "No") << std::endl;
    std::cout << std::endl;
    
    std::cout << "Large Style:" << std::endl;
    std::cout << "  - Setters Count: " << largeButtonStyle->Setters().Count() << std::endl;
    std::cout << "  - BasedOn: " << (largeButtonStyle->GetBasedOn() ? "Base Style" : "None") << std::endl;
    std::cout << "  - Is Sealed: " << (largeButtonStyle->IsSealed() ? "Yes" : "No") << std::endl;
    std::cout << std::endl;
    
    std::cout << "Bold Style:" << std::endl;
    std::cout << "  - Setters Count: " << boldButtonStyle->Setters().Count() << std::endl;
    std::cout << "  - BasedOn: " << (boldButtonStyle->GetBasedOn() ? "Base Style" : "None") << std::endl;
    std::cout << "  - Is Sealed: " << (boldButtonStyle->IsSealed() ? "Yes" : "No") << std::endl;
    std::cout << std::endl;
    
    std::cout << "Buttons:" << std::endl;
    std::cout << "  - Button1 Style: " << (button1->GetStyle() ? "Applied" : "None") << std::endl;
    std::cout << "  - Button2 Style: " << (button2->GetStyle() ? "Applied" : "None") << std::endl;
    std::cout << "  - Button3 Style: " << (button3->GetStyle() ? "Applied" : "None") << std::endl;
    std::cout << "  - Button4 Style: " << (button4->GetStyle() ? "Applied" : "None") << std::endl;
    std::cout << "=========================" << std::endl;
    
    // 运行应用
    return app.Run(window);
}
