#include "fk/app/Application.h"
#include "fk/ui/Window.h"
#include "fk/ui/TextBlock.h"
#include "fk/ui/Brush.h"
#include <iostream>

int main(int argc, char** argv) {
    try {
        std::cout << "创建 Application..." << std::endl;
        fk::Application app;
        
        std::cout << "创建 Window..." << std::endl;
        auto mainWindow = std::make_shared<fk::ui::Window>();
        
        std::cout << "创建 TextBlock..." << std::endl;
        auto* textBlock = new fk::ui::TextBlock();
        textBlock->Text("Hello, F K UI!")
            ->FontFamily("Arial")
            ->FontSize(24.0f)
            ->FontWeight(fk::ui::FontWeight::Bold)
            ->TextAlignment(fk::ui::TextAlignment::Center)
            ->TextWrapping(fk::ui::TextWrapping::Wrap)
            ->Foreground(new fk::ui::SolidColorBrush());
        
        std::cout << "配置 Window..." << std::endl;
        try {
            std::cout << "  设置 Title..." << std::endl;
            mainWindow->Title("F K UI Example");
            std::cout << "  设置 Width..." << std::endl;
            mainWindow->Width(800);
            std::cout << "  设置 Height..." << std::endl;
            mainWindow->Height(600);
            std::cout << "  设置 Content..." << std::endl;
            mainWindow->Content(textBlock);
            std::cout << "  Window 配置完成" << std::endl;
        } catch (const std::exception& e) {
            std::cerr << "配置 Window 时出错: " << e.what() << std::endl;
            throw;
        }
        
        std::cout << "运行应用..." << std::endl;
        app.Run(mainWindow);
        
        std::cout << "应用结束" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "错误: " << e.what() << std::endl;
        return 1;
    }
}
