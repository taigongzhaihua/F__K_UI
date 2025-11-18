#include <iostream>
#include "fk/animation/VisualStateBuilder.h"
#include "fk/ui/DrawCommand.h"
#include "fk/ui/Brush.h"

int main() {
    std::cout << "测试VisualStateBuilder...\n";
    
    try {
        std::cout << "1. 调用CreateGroup...\n";
        auto builder = fk::animation::VisualStateBuilder::CreateGroup("Test");
        std::cout << "   ✓ CreateGroup成功\n";
        
        std::cout << "2. 创建SolidColorBrush...\n";
        auto brush = std::make_shared<fk::ui::SolidColorBrush>(fk::ui::Color::FromRGB(255, 255, 255));
        std::cout << "   ✓ Brush创建成功\n";
        
        std::cout << "3. 调用State...\n";
        builder->State("Normal");
        std::cout << "   ✓ State成功\n";
        
        std::cout << "4. 调用ColorAnimation...\n";
        builder->ColorAnimation(brush.get(), &fk::ui::SolidColorBrush::ColorProperty());
        std::cout << "   ✓ ColorAnimation成功\n";
        
        std::cout << "5. 调用To...\n";
        builder->To(fk::ui::Color::FromRGB(255, 0, 0));
        std::cout << "   ✓ To成功\n";
        
        std::cout << "6. 调用Duration...\n";
        builder->Duration(200);
        std::cout << "   ✓ Duration成功\n";
        
        std::cout << "7. 调用EndAnimation...\n";
        builder->EndAnimation();
        std::cout << "   ✓ EndAnimation成功\n";
        
        std::cout << "8. 调用EndState...\n";
        builder->EndState();
        std::cout << "   ✓ EndState成功\n";
        
        std::cout << "9. 调用Build...\n";
        auto group = builder->Build();
        std::cout << "   ✓ Build成功\n";
        
        std::cout << "10. 检查结果...\n";
        std::cout << "   组名: " << group->GetName() << "\n";
        std::cout << "   状态数: " << group->GetStates().size() << "\n";
        
    } catch (const std::exception& e) {
        std::cerr << "❌ 异常: " << e.what() << "\n";
        return 1;
    }
    
    std::cout << "\n✓ 所有测试通过!\n";
    return 0;
}
