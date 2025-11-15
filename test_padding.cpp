#include "fk/ui/TextBlock.h"
#include "fk/ui/StackPanel.h"
#include "fk/ui/Brush.h"
#include <iostream>

int main() {
    std::cout << "=== Padding 属性测试 ===" << std::endl;
    
    // 测试 StackPanel 的 Padding
    auto* stackPanel = new fk::ui::StackPanel();
    stackPanel->Padding(15);  // 设置 15px padding
    stackPanel->Margin(5);    // 设置 5px margin
    
    std::cout << "\nStackPanel:" << std::endl;
    std::cout << "  Padding: " << stackPanel->GetPadding().top << std::endl;
    std::cout << "  Margin: " << stackPanel->GetMargin().top << std::endl;
    
    // 添加文本元素
    auto* text1 = (new fk::ui::TextBlock())
                       ->Text("测试文本1")
                       ->FontSize(20)
                       ->Margin(10);
    
    auto* text2 = (new fk::ui::TextBlock())
                       ->Text("测试文本2")
                       ->FontSize(20)
                       ->Margin(10)
                       ->Padding(5);  // TextBlock 现在也可以有 Padding
    
    std::cout << "\nText1:" << std::endl;
    std::cout << "  Margin: " << text1->GetMargin().top << std::endl;
    std::cout << "  Padding: " << text1->GetPadding().top << std::endl;
    
    std::cout << "\nText2:" << std::endl;
    std::cout << "  Margin: " << text2->GetMargin().top << std::endl;
    std::cout << "  Padding: " << text2->GetPadding().top << std::endl;
    
    stackPanel->Children({text1, text2});
    
    // 执行布局
    fk::ui::Size availableSize(800, 600);
    stackPanel->Measure(availableSize);
    stackPanel->Arrange(fk::ui::Rect(0, 0, 800, 600));
    
    std::cout << "\n=== 布局结果 ===" << std::endl;
    
    auto panelDesired = stackPanel->GetDesiredSize();
    auto panelLayout = stackPanel->GetLayoutRect();
    
    std::cout << "\nStackPanel:" << std::endl;
    std::cout << "  DesiredSize: (" << panelDesired.width << ", " << panelDesired.height << ")" << std::endl;
    std::cout << "  LayoutRect: (" << panelLayout.x << ", " << panelLayout.y 
              << ", " << panelLayout.width << ", " << panelLayout.height << ")" << std::endl;
    
    auto text1Desired = text1->GetDesiredSize();
    auto text1Layout = text1->GetLayoutRect();
    
    std::cout << "\nText1:" << std::endl;
    std::cout << "  DesiredSize: (" << text1Desired.width << ", " << text1Desired.height << ")" << std::endl;
    std::cout << "  LayoutRect: (" << text1Layout.x << ", " << text1Layout.y 
              << ", " << text1Layout.width << ", " << text1Layout.height << ")" << std::endl;
    
    auto text2Desired = text2->GetDesiredSize();
    auto text2Layout = text2->GetLayoutRect();
    
    std::cout << "\nText2:" << std::endl;
    std::cout << "  DesiredSize: (" << text2Desired.width << ", " << text2Desired.height << ")" << std::endl;
    std::cout << "  LayoutRect: (" << text2Layout.x << ", " << text2Layout.y 
              << ", " << text2Layout.width << ", " << text2Layout.height << ")" << std::endl;
    
    std::cout << "\n=== Padding 效果验证 ===" << std::endl;
    
    // StackPanel 的 Padding 应该影响子元素的起始位置
    std::cout << "\nStackPanel Padding 效果:" << std::endl;
    std::cout << "  第一个子元素应该从 Y=" << (stackPanel->GetPadding().top + text1->GetMargin().top) 
              << " 开始" << std::endl;
    std::cout << "  实际 Text1.Y: " << text1Layout.y << std::endl;
    
    // Text2 的 Padding 应该增加其 desiredSize
    float text2ExpectedHeight = 20 * 1.2f + text2->GetPadding().top + text2->GetPadding().bottom;
    std::cout << "\nText2 Padding 效果:" << std::endl;
    std::cout << "  预期高度（含 Padding）: " << text2ExpectedHeight << std::endl;
    std::cout << "  实际 DesiredSize.height: " << text2Desired.height << std::endl;
    
    // 验证 desiredSize 包含 Padding 但不含 Margin
    std::cout << "\n✅ desiredSize 包含 Padding: " 
              << (text2Desired.height > 20 * 1.2f ? "是" : "否") << std::endl;
    std::cout << "✅ desiredSize 不含 Margin: " 
              << (text2Desired.height < (20 * 1.2f + text2->GetPadding().top + text2->GetPadding().bottom + text2->GetMargin().top + text2->GetMargin().bottom) ? "是" : "否") 
              << std::endl;
    
    delete stackPanel;
    
    return 0;
}
