#include <iostream>
#include <memory>
#include "fk/app/Application.h"
#include "fk/ui/Window.h"
#include "fk/ui/StackPanel.h"
#include "fk/ui/ContentControl.h"

using namespace fk;

int main()
{
    try {
        std::cout << "\n=== F__K_UI Window Example (Fluent API) ===" << std::endl;
        std::cout << "Step 1: Creating UI tree with fluent API..." << std::endl;
        
        auto rootPanel = std::make_shared<ui::StackPanel>();
        rootPanel->Orientation(ui::Orientation::Vertical)
                 ->AddChild(std::make_shared<ui::ContentControl>())
                 ->AddChild(std::make_shared<ui::ContentControl>())
                 ->AddChild(std::make_shared<ui::ContentControl>());
        
        std::cout << "Step 2: Creating window..." << std::endl;
        
        auto window = ui::window()
            ->Title("F__K_UI Demo - Fluent API")
            ->Width(800)
            ->Height(600);
        
        std::cout << "Step 3: Setting content (using inherited Content())..." << std::endl;
        window->Content(rootPanel);  // 使用 ContentControl 继承的 Content() 方法
        
        std::cout << "Step 4: Subscribing events..." << std::endl;
        
        window->Opened += []() {
            std::cout << ">>> Window opened!" << std::endl;
        };
        
        window->Closed += []() {
            std::cout << ">>> Window closed!" << std::endl;
        };
        
        window->Resized += [](int w, int h) {
            std::cout << ">>> Window resized to " << w << "x" << h << std::endl;
        };

        std::cout << "Step 5: Creating application..." << std::endl;
        Application app;
        
        std::cout << "Step 6: Running application with main window..." << std::endl;
        app.Run(window);  // Application 接管消息循环
        
        std::cout << "=== Demo completed ===" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "ERROR: " << e.what() << std::endl;
        return -1;
    } catch (...) {
        std::cerr << "ERROR: Unknown exception" << std::endl;
        return -1;
    }
    
    return 0;
}
