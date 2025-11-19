/**
 * @file test_stackpanel_alignment.cpp
 * @brief StackPanel Alignment 和 Spacing 折叠功能测试
 * 
 * 测试内容：
 * 1. Spacing 参与 Margin 折叠的行为
 * 2. 垂直 StackPanel 的 HorizontalAlignment 支持
 * 3. 水平 StackPanel 的 VerticalAlignment 支持
 * 4. 可视化布局结果输出
 */

#include "fk/ui/StackPanel.h"
#include "fk/ui/Button.h"
#include "fk/ui/Border.h"
#include <iostream>
#include <iomanip>
#include <cassert>

using namespace fk::ui;
using namespace fk;

// 辅助函数：打印矩形信息
void PrintRect(const char* label, const Rect& rect) {
    std::cout << std::setw(30) << std::left << label 
              << " Rect(x=" << std::fixed << std::setprecision(1) << rect.x
              << ", y=" << rect.y
              << ", width=" << rect.width
              << ", height=" << rect.height << ")\n";
}

// 辅助函数：创建简单的测试元素
Button* CreateTestButton(float width, float height, const Thickness& margin = Thickness(0)) {
    auto* btn = new Button();
    btn->SetWidth(width);
    btn->SetHeight(height);
    btn->SetMargin(margin);
    return btn;
}

// ============================================================
// 测试 1: Spacing 参与 Margin 折叠
// ============================================================
void Test_Spacing_Collapse() {
    std::cout << "\n========== Test 1: Spacing \u53c2\u4e0e Margin \u6298\u53e0 ==========\n\n";
    
    auto* panel = new StackPanel();
    panel->SetOrientation(Orientation::Vertical);
    panel->SetWidth(200);
    panel->SetHeight(500);
    
    // 场景 1: Spacing 大于 Margin
    std::cout << "--- \u573a\u666f 1: Spacing=20, Margin=10/15 ---\n";
    panel->SetSpacing(20);
    panel->AddChild(CreateTestButton(100, 50, Thickness(0, 10, 0, 10)));  // margin-top=10, bottom=10
    panel->AddChild(CreateTestButton(100, 50, Thickness(0, 15, 0, 5)));   // margin-top=15, bottom=5
    panel->AddChild(CreateTestButton(100, 50, Thickness(0, 5, 0, 5)));    // margin-top=5, bottom=5
    
    panel->Measure(Size(200, 500));
    panel->Arrange(Rect(0, 0, 200, 500));
    
    auto children = panel->GetChildren();
    PrintRect("Child 0:", children[0]->GetLayoutRect());
    PrintRect("Child 1:", children[1]->GetLayoutRect());
    PrintRect("Child 2:", children[2]->GetLayoutRect());
    
    // 验证间距
    float gap1 = children[1]->GetLayoutRect().y - (children[0]->GetLayoutRect().y + children[0]->GetLayoutRect().height);
    float gap2 = children[2]->GetLayoutRect().y - (children[1]->GetLayoutRect().y + children[1]->GetLayoutRect().height);
    
    std::cout << "\u95f4\u8ddd 0-1: " << gap1 << " (\u671f\u671b: max(10, 15, 20) = 20)\n";
    std::cout << "\u95f4\u8ddd 1-2: " << gap2 << " (\u671f\u671b: max(5, 5, 20) = 20)\n";
    
    assert(std::abs(gap1 - 20.0f) < 0.1f && "Spacing should be 20");
    assert(std::abs(gap2 - 20.0f) < 0.1f && "Spacing should be 20");
    std::cout << "\u2705 \u6d4b\u8bd5\u901a\u8fc7\uff01\n";
    
    // 清理
    panel->ClearChildren();
    
    // 场景 2: Margin 大于 Spacing
    std::cout << "\n--- \u573a\u666f 2: Spacing=5, Margin=10/30 ---\n";
    panel->SetSpacing(5);
    panel->AddChild(CreateTestButton(100, 50, Thickness(0, 10, 0, 10)));  // margin-top=10, bottom=10
    panel->AddChild(CreateTestButton(100, 50, Thickness(0, 30, 0, 15)));  // margin-top=30, bottom=15
    
    panel->Measure(Size(200, 500));
    panel->Arrange(Rect(0, 0, 200, 500));
    
    children = panel->GetChildren();
    PrintRect("Child 0:", children[0]->GetLayoutRect());
    PrintRect("Child 1:", children[1]->GetLayoutRect());
    
    gap1 = children[1]->GetLayoutRect().y - (children[0]->GetLayoutRect().y + children[0]->GetLayoutRect().height);
    std::cout << "\u95f4\u8ddd 0-1: " << gap1 << " (\u671f\u671b: max(10, 30, 5) = 30)\n";
    
    assert(std::abs(gap1 - 30.0f) < 0.1f && "Margin should win");
    std::cout << "\u2705 \u6d4b\u8bd5\u901a\u8fc7\uff01\n";
    
    delete panel;
}

// ============================================================
// 测试 2: 垂直 StackPanel - HorizontalAlignment
// ============================================================
void Test_Vertical_HorizontalAlignment() {
    std::cout << "\n========== Test 2: \u5782\u76f4 StackPanel - HorizontalAlignment ==========\n\n";
    
    auto* panel = new StackPanel();
    panel->SetOrientation(Orientation::Vertical);
    panel->SetWidth(200);
    panel->SetHeight(300);
    panel->SetSpacing(10);
    
    // 测试各种 HorizontalAlignment
    auto* btnStretch = CreateTestButton(100, 50, Thickness(10, 5, 10, 5));
    btnStretch->SetHorizontalAlignment(HorizontalAlignment::Stretch);
    
    auto* btnLeft = CreateTestButton(100, 50, Thickness(10, 5, 10, 5));
    btnLeft->SetHorizontalAlignment(HorizontalAlignment::Left);
    
    auto* btnCenter = CreateTestButton(100, 50, Thickness(10, 5, 10, 5));
    btnCenter->SetHorizontalAlignment(HorizontalAlignment::Center);
    
    auto* btnRight = CreateTestButton(100, 50, Thickness(10, 5, 10, 5));
    btnRight->SetHorizontalAlignment(HorizontalAlignment::Right);
    
    panel->AddChild(btnStretch);
    panel->AddChild(btnLeft);
    panel->AddChild(btnCenter);
    panel->AddChild(btnRight);
    
    panel->Measure(Size(200, 300));
    panel->Arrange(Rect(0, 0, 200, 300));
    
    auto children = panel->GetChildren();
    
    std::cout << "Panel \u5bbd\u5ea6: 200, \u53ef\u7528\u5bbd\u5ea6(\u51cf Margin): 180\n\n";
    
    PrintRect("Stretch:", children[0]->GetLayoutRect());
    std::cout << "  \u671f\u671b: x=10, width=180 (\u586b\u5145\u6574\u4e2a\u5bbd\u5ea6)\n";
    assert(std::abs(children[0]->GetLayoutRect().x - 10.0f) < 0.1f);
    assert(std::abs(children[0]->GetLayoutRect().width - 180.0f) < 0.1f);
    
    PrintRect("Left:", children[1]->GetLayoutRect());
    std::cout << "  \u671f\u671b: x=10, width=100 (\u671f\u671b\u5bbd\u5ea6\uff0c\u5de6\u5bf9\u9f50)\n";
    assert(std::abs(children[1]->GetLayoutRect().x - 10.0f) < 0.1f);
    assert(std::abs(children[1]->GetLayoutRect().width - 100.0f) < 0.1f);
    
    PrintRect("Center:", children[2]->GetLayoutRect());
    std::cout << "  \u671f\u671b: x=50, width=100 (\u5c45\u4e2d: 10 + (180-100)/2)\n";
    assert(std::abs(children[2]->GetLayoutRect().x - 50.0f) < 0.1f);
    assert(std::abs(children[2]->GetLayoutRect().width - 100.0f) < 0.1f);
    
    PrintRect("Right:", children[3]->GetLayoutRect());
    std::cout << "  \u671f\u671b: x=90, width=100 (\u53f3\u5bf9\u9f50: 10 + 180 - 100)\n";
    assert(std::abs(children[3]->GetLayoutRect().x - 90.0f) < 0.1f);
    assert(std::abs(children[3]->GetLayoutRect().width - 100.0f) < 0.1f);
    
    std::cout << "\u2705 \u6240\u6709 HorizontalAlignment \u6d4b\u8bd5\u901a\u8fc7\uff01\n";
    
    delete panel;
}

// ============================================================
// 测试 3: 水平 StackPanel - VerticalAlignment
// ============================================================
void Test_Horizontal_VerticalAlignment() {
    std::cout << "\n========== Test 3: \u6c34\u5e73 StackPanel - VerticalAlignment ==========\n\n";
    
    auto* panel = new StackPanel();
    panel->SetOrientation(Orientation::Horizontal);
    panel->SetWidth(500);
    panel->SetHeight(200);
    panel->SetSpacing(10);
    
    // 测试各种 VerticalAlignment
    auto* btnStretch = CreateTestButton(80, 100, Thickness(5, 10, 5, 10));
    btnStretch->SetVerticalAlignment(VerticalAlignment::Stretch);
    
    auto* btnTop = CreateTestButton(80, 100, Thickness(5, 10, 5, 10));
    btnTop->SetVerticalAlignment(VerticalAlignment::Top);
    
    auto* btnCenter = CreateTestButton(80, 100, Thickness(5, 10, 5, 10));
    btnCenter->SetVerticalAlignment(VerticalAlignment::Center);
    
    auto* btnBottom = CreateTestButton(80, 100, Thickness(5, 10, 5, 10));
    btnBottom->SetVerticalAlignment(VerticalAlignment::Bottom);
    
    panel->AddChild(btnStretch);
    panel->AddChild(btnTop);
    panel->AddChild(btnCenter);
    panel->AddChild(btnBottom);
    
    panel->Measure(Size(500, 200));
    panel->Arrange(Rect(0, 0, 500, 200));
    
    auto children = panel->GetChildren();
    
    std::cout << "Panel \u9ad8\u5ea6: 200, \u53ef\u7528\u9ad8\u5ea6(\u51cf Margin): 180\n\n";
    
    PrintRect("Stretch:", children[0]->GetLayoutRect());
    std::cout << "  \u671f\u671b: y=10, height=180 (\u586b\u5145\u6574\u4e2a\u9ad8\u5ea6)\n";
    assert(std::abs(children[0]->GetLayoutRect().y - 10.0f) < 0.1f);
    assert(std::abs(children[0]->GetLayoutRect().height - 180.0f) < 0.1f);
    
    PrintRect("Top:", children[1]->GetLayoutRect());
    std::cout << "  \u671f\u671b: y=10, height=100 (\u671f\u671b\u9ad8\u5ea6\uff0c\u9876\u90e8\u5bf9\u9f50)\n";
    assert(std::abs(children[1]->GetLayoutRect().y - 10.0f) < 0.1f);
    assert(std::abs(children[1]->GetLayoutRect().height - 100.0f) < 0.1f);
    
    PrintRect("Center:", children[2]->GetLayoutRect());
    std::cout << "  \u671f\u671b: y=50, height=100 (\u5c45\u4e2d: 10 + (180-100)/2)\n";
    assert(std::abs(children[2]->GetLayoutRect().y - 50.0f) < 0.1f);
    assert(std::abs(children[2]->GetLayoutRect().height - 100.0f) < 0.1f);
    
    PrintRect("Bottom:", children[3]->GetLayoutRect());
    std::cout << "  \u671f\u671b: y=90, height=100 (\u5e95\u90e8\u5bf9\u9f50: 10 + 180 - 100)\n";
    assert(std::abs(children[3]->GetLayoutRect().y - 90.0f) < 0.1f);
    assert(std::abs(children[3]->GetLayoutRect().height - 100.0f) < 0.1f);
    
    std::cout << "\u2705 \u6240\u6709 VerticalAlignment \u6d4b\u8bd5\u901a\u8fc7\uff01\n";
    
    delete panel;
}

// ============================================================
// 测试 4: 综合场景 - 实际应用示例
// ============================================================
void Test_Practical_Scenario() {
    std::cout << "\n========== Test 4: \u7efc\u5408\u573a\u666f - \u6309\u94ae\u5217\u8868 ==========\n\n";
    
    auto* panel = new StackPanel();
    panel->SetOrientation(Orientation::Vertical);
    panel->SetWidth(300);
    panel->SetHeight(400);
    panel->SetSpacing(15);  // 统一间距 15 像素
    
    // 创建不同样式的按钮
    auto* btn1 = CreateTestButton(200, 40, Thickness(0, 5, 0, 5));
    btn1->SetHorizontalAlignment(HorizontalAlignment::Stretch);
    
    auto* btn2 = CreateTestButton(150, 40, Thickness(0, 10, 0, 10));
    btn2->SetHorizontalAlignment(HorizontalAlignment::Center);
    
    auto* btn3 = CreateTestButton(180, 40, Thickness(0, 0, 0, 0));
    btn3->SetHorizontalAlignment(HorizontalAlignment::Left);
    
    auto* btn4 = CreateTestButton(120, 40, Thickness(20, 8, 20, 8));
    btn4->SetHorizontalAlignment(HorizontalAlignment::Right);
    
    panel->AddChild(btn1);
    panel->AddChild(btn2);
    panel->AddChild(btn3);
    panel->AddChild(btn4);
    
    panel->Measure(Size(300, 400));
    panel->Arrange(Rect(0, 0, 300, 400));
    
    auto children = panel->GetChildren();
    
    std::cout << "\u6309\u94ae\u5217\u8868\u5e03\u5c40\u7ed3\u679c\uff1a\n";
    PrintRect("Button 1 (Stretch):", children[0]->GetLayoutRect());
    PrintRect("Button 2 (Center):", children[1]->GetLayoutRect());
    PrintRect("Button 3 (Left):", children[2]->GetLayoutRect());
    PrintRect("Button 4 (Right):", children[3]->GetLayoutRect());
    
    // 验证间距（应该都是 15 或更大）
    for (size_t i = 0; i < children.size() - 1; ++i) {
        float gap = children[i + 1]->GetLayoutRect().y - 
                   (children[i]->GetLayoutRect().y + children[i]->GetLayoutRect().height);
        std::cout << "\u95f4\u8ddd " << i << "-" << (i+1) << ": " << gap << " px\n";
        assert(gap >= 14.9f && "\u95f4\u8ddd\u5e94\u8be5\u81f3\u5c11\u4e3a 15");
    }
    
    std::cout << "\u2705 \u7efc\u5408\u573a\u666f\u6d4b\u8bd5\u901a\u8fc7\uff01\n";
    
    delete panel;
}

// ============================================================
// 主函数
// ============================================================
int main() {
    std::cout << "\n";
    std::cout << "================================================\n";
    std::cout << "  StackPanel Alignment \u548c Spacing \u529f\u80fd\u6d4b\u8bd5\n";
    std::cout << "================================================\n";
    
    try {
        Test_Spacing_Collapse();
        Test_Vertical_HorizontalAlignment();
        Test_Horizontal_VerticalAlignment();
        Test_Practical_Scenario();
        
        std::cout << "\n";
        std::cout << "================================================\n";
        std::cout << "  \u2705 \u6240\u6709\u6d4b\u8bd5\u5747\u901a\u8fc7\uff01\n";
        std::cout << "================================================\n";
        std::cout << "\n";
        
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "\n\u274c \u6d4b\u8bd5\u5931\u8d25: " << e.what() << "\n";
        return 1;
    }
}
