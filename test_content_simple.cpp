#include "fk/ui/Window.h"
#include "fk/ui/TextBlock.h"
#include <iostream>
#include <memory>

int main() {
    try {
        std::cout << "创建 Window..." << std::endl;
        auto window = std::make_shared<fk::ui::Window>();
        
        std::cout << "创建 TextBlock..." << std::endl;
        auto* textBlock = new fk::ui::TextBlock();
        textBlock->Text("Hello, World!");
        
        std::cout << "设置 Content (使用 UIElement* 重载)..." << std::endl;
        window->Content(textBlock);
        
        std::cout << "成功！" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "错误: " << e.what() << std::endl;
        return 1;
    }
}
