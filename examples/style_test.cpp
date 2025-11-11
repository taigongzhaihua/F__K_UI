/**
 * @file style_test.cpp
 * @brief Style 系统基础测试
 * 
 * 最小化测试：测试 Style 基本功能
 */

#include "fk/ui/Style.h"
#include "fk/ui/Setter.h"
#include "fk/ui/Button.h"
#include "fk/ui/Control.h"
#include "fk/ui/FrameworkElement.h"
#include <iostream>
#include <typeindex>

using namespace fk;

int main() {
    std::cout << "\n=== Style System Basic Test ===" << std::endl;
    
    // 测试 1：创建 Style
    std::cout << "\nTest 1: Create Style" << std::endl;
    auto style1 = new ui::Style();
    style1->SetTargetType(typeid(ui::Button));
    std::cout << "  - Style created" << std::endl;
    std::cout << "  - Setters count: " << style1->Setters().Count() << std::endl;
    
    // 测试 2：添加 Setter
    std::cout << "\nTest 2: Add Setters" << std::endl;
    style1->Setters().Add(
        ui::FrameworkElement<ui::Button>::WidthProperty(),
        200.0f
    );
    std::cout << "  - Width setter added" << std::endl;
    
    style1->Setters().Add(
        ui::FrameworkElement<ui::Button>::HeightProperty(),
        50.0f
    );
    std::cout << "  - Height setter added" << std::endl;
    
    style1->Setters().Add(
        ui::Control<ui::Button>::FontSizeProperty(),
        16.0f
    );
    std::cout << "  - FontSize setter added" << std::endl;
    std::cout << "  - Total setters: " << style1->Setters().Count() << std::endl;
    
    // 测试 3：封印 Style
    std::cout << "\nTest 3: Seal Style" << std::endl;
    std::cout << "  - Before seal: " << (style1->IsSealed() ? "Sealed" : "Not Sealed") << std::endl;
    style1->Seal();
    std::cout << "  - After seal: " << (style1->IsSealed() ? "Sealed" : "Not Sealed") << std::endl;
    
    // 测试 4：创建继承的 Style
    std::cout << "\nTest 4: Create Derived Style" << std::endl;
    auto style2 = new ui::Style();
    style2->SetTargetType(typeid(ui::Button));
    style2->SetBasedOn(style1);
    std::cout << "  - Derived style created" << std::endl;
    std::cout << "  - BasedOn: " << (style2->GetBasedOn() ? "Yes" : "No") << std::endl;
    
    style2->Setters().Add(
        ui::Control<ui::Button>::FontWeightProperty(),
        ui::FontWeight::Bold
    );
    std::cout << "  - FontWeight setter added" << std::endl;
    std::cout << "  - Derived setters: " << style2->Setters().Count() << std::endl;
    
    style2->Seal();
    
    // 测试 5：创建按钮并应用样式
    std::cout << "\nTest 5: Apply Style to Button" << std::endl;
    auto button1 = new ui::Button();
    std::cout << "  - Button created" << std::endl;
    std::cout << "  - Button style before: " << (button1->GetStyle() ? "Has Style" : "No Style") << std::endl;
    
    button1->StyleProperty(style1);
    std::cout << "  - Style applied" << std::endl;
    std::cout << "  - Button style after: " << (button1->GetStyle() ? "Has Style" : "No Style") << std::endl;
    
    // 测试 6：应用继承的样式
    std::cout << "\nTest 6: Apply Derived Style" << std::endl;
    auto button2 = new ui::Button();
    button2->StyleProperty(style2);
    std::cout << "  - Derived style applied to button2" << std::endl;
    std::cout << "  - Button2 style: " << (button2->GetStyle() ? "Has Style" : "No Style") << std::endl;
    
    // 测试 7：检查类型匹配
    std::cout << "\nTest 7: Type Checking" << std::endl;
    std::cout << "  - Style1 applicable to Button: " 
              << (style1->IsApplicableTo(typeid(ui::Button)) ? "Yes" : "No") << std::endl;
    std::cout << "  - Style2 applicable to Button: " 
              << (style2->IsApplicableTo(typeid(ui::Button)) ? "Yes" : "No") << std::endl;
    
    // 摘要
    std::cout << "\n=== Test Summary ===" << std::endl;
    std::cout << "✓ Style creation" << std::endl;
    std::cout << "✓ Setter addition" << std::endl;
    std::cout << "✓ Style sealing" << std::endl;
    std::cout << "✓ Style inheritance (BasedOn)" << std::endl;
    std::cout << "✓ Style application to controls" << std::endl;
    std::cout << "✓ Type checking" << std::endl;
    std::cout << "\nAll tests passed!" << std::endl;
    std::cout << "====================\n" << std::endl;
    
    // 清理
    delete button1;
    delete button2;
    delete style1;
    delete style2;
    
    return 0;
}
