#include "fk/ui/StackPanel.h"
#include "fk/ui/ContentControl.h"
#include <iostream>

int main() {
    try {
        std::cout << "=== Testing Getter Overloads ===" << std::endl;
        
        // 创建 StackPanel
        auto panel = std::make_shared<fk::ui::StackPanel>();
        
        // 使用 setter (流式API)
        panel->Orientation(fk::ui::Orientation::Horizontal)
             ->Width(800.0f)
             ->Height(600.0f)
             ->Opacity(0.9f);
        
        // 使用 getter (重载)
        std::cout << "Orientation: " << (panel->Orientation() == fk::ui::Orientation::Horizontal ? "Horizontal" : "Vertical") << std::endl;
        std::cout << "Width: " << panel->Width() << std::endl;
        std::cout << "Height: " << panel->Height() << std::endl;
        std::cout << "Opacity: " << panel->Opacity() << std::endl;
        
        // 验证值是否正确
        if (panel->Width() == 800.0f && 
            panel->Height() == 600.0f && 
            panel->Opacity() == 0.9f &&
            panel->Orientation() == fk::ui::Orientation::Horizontal) {
            std::cout << "\n✅ All getter overloads work correctly!" << std::endl;
            return 0;
        } else {
            std::cout << "\n❌ Getter values don't match!" << std::endl;
            return 1;
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
