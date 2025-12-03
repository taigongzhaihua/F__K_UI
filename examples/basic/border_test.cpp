#include "fk/app/Application.h"
#include "fk/ui/Window.h"
#include "fk/ui/text/TextBlock.h"
#include "fk/ui/controls/Border.h"
#include "fk/ui/graphics/Brush.h"
#include <iostream>

int main(int argc, char **argv)
{
    std::cout << "=== Border Child Element Test ===" << std::endl;
    
    fk::Application app;
    auto mainWindow = std::make_shared<fk::ui::Window>();
    
    // 创建一个简单的 Border 包含一个 TextBlock
    auto border = new fk::ui::Border();
    border->SetBackground(new fk::ui::SolidColorBrush(200, 200, 200)); // 灰色背景
    border->SetBorderBrush(new fk::ui::SolidColorBrush(0, 0, 255));    // 蓝色边框
    border->SetBorderThickness(fk::ui::Thickness(5));
    border->SetPadding(fk::ui::Thickness(20));
    border->SetWidth(400);
    border->SetHeight(300);
    
    // 添加子元素
    auto textBlock = new fk::ui::TextBlock();
    textBlock->SetText("Hello, Border Child!");
    textBlock->SetFontSize(24);
    textBlock->SetForeground(new fk::ui::SolidColorBrush(255, 0, 0)); // 红色文字
    
    border->SetChild(textBlock);
    
    std::cout << "Border created with child" << std::endl;
    std::cout << "Border size: " << border->GetWidth() << " x " << border->GetHeight() << std::endl;
    std::cout << "Border has child: " << (border->GetChild() != nullptr ? "Yes" : "No") << std::endl;
    
    mainWindow->Title("Border Child Test")
        ->Width(600)
        ->Height(500)
        ->Background(new fk::ui::SolidColorBrush(255, 255, 255)) // 白色背景
        ->Content(border);
    
    std::cout << "Starting application..." << std::endl;
    app.Run(mainWindow);
    return 0;
}
