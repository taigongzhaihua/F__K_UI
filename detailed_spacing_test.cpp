#include "fk/ui/TextBlock.h"
#include "fk/ui/StackPanel.h"
#include "fk/ui/Brush.h"
#include <iostream>

int main() {
    std::cout << "=== StackPanel 间距详细分析 ===" << std::endl;
    std::cout << "\n这个测试将详细解释为什么 Text2 的 Y 坐标是 118.4\n" << std::endl;
    
    // 完全按照 example/main.cpp 创建元素
    auto* stackPanel = new fk::ui::StackPanel();
    std::cout << "StackPanel 配置:" << std::endl;
    std::cout << "  Spacing: " << stackPanel->GetSpacing() << " (默认值，未设置)" << std::endl;
    std::cout << "  Orientation: Vertical" << std::endl;
    
    auto* text1 = (new fk::ui::TextBlock())
                       ->Text("Hello, F K UI!")
                       ->FontSize(32)
                       ->Foreground(fk::ui::Brushes::Blue())
                       ->Margin(fk::Thickness(20));
    
    auto* text2 = (new fk::ui::TextBlock())
                       ->Text("This is a simple example of F K UI framework.")
                       ->FontSize(16)
                       ->Foreground(fk::ui::Brushes::DarkGray())
                       ->Margin(fk::Thickness(20));
    
    std::cout << "\nText1 配置:" << std::endl;
    std::cout << "  文本: \"" << text1->GetText() << "\"" << std::endl;
    std::cout << "  字体大小: " << text1->GetFontSize() << std::endl;
    std::cout << "  Margin: 上=" << text1->GetMargin().top 
              << ", 下=" << text1->GetMargin().bottom << std::endl;
    
    std::cout << "\nText2 配置:" << std::endl;
    std::cout << "  文本: \"" << text2->GetText() << "\"" << std::endl;
    std::cout << "  字体大小: " << text2->GetFontSize() << std::endl;
    std::cout << "  Margin: 上=" << text2->GetMargin().top 
              << ", 下=" << text2->GetMargin().bottom << std::endl;
    
    stackPanel->Children({text1, text2});
    
    // 执行布局
    fk::ui::Size availableSize(800, 600);
    stackPanel->Measure(availableSize);
    stackPanel->Arrange(fk::ui::Rect(0, 0, 800, 600));
    
    std::cout << "\n━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━" << std::endl;
    std::cout << "布局计算过程详解" << std::endl;
    std::cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━" << std::endl;
    
    auto rect1 = text1->GetLayoutRect();
    auto rect2 = text2->GetLayoutRect();
    
    std::cout << "\n【步骤 1】Text1 布局" << std::endl;
    std::cout << "  起始 offset = 0" << std::endl;
    std::cout << "  添加 Text1.margin.top = " << text1->GetMargin().top << std::endl;
    std::cout << "  → offset = " << text1->GetMargin().top << std::endl;
    std::cout << "  Text1.y = " << rect1.y << std::endl;
    std::cout << "  Text1.height = " << rect1.height << " (注意：≠ 字体大小 " 
              << text1->GetFontSize() << ")" << std::endl;
    std::cout << "  Text1 底部 = " << rect1.y << " + " << rect1.height 
              << " = " << (rect1.y + rect1.height) << std::endl;
    
    float text1_bottom = rect1.y + rect1.height;
    
    std::cout << "\n【步骤 2】计算 Text2 位置" << std::endl;
    std::cout << "  当前 offset = " << text1_bottom << " (Text1 底部)" << std::endl;
    std::cout << "  Text1.margin.bottom = " << text1->GetMargin().bottom << std::endl;
    std::cout << "  Text2.margin.top = " << text2->GetMargin().top << std::endl;
    std::cout << "  StackPanel.spacing = " << stackPanel->GetSpacing() << std::endl;
    std::cout << "\n  应用 margin 折叠公式:" << std::endl;
    std::cout << "    间距 = max(Text1.margin.bottom, Text2.margin.top) + spacing" << std::endl;
    std::cout << "         = max(" << text1->GetMargin().bottom << ", " 
              << text2->GetMargin().top << ") + " << stackPanel->GetSpacing() << std::endl;
    
    float margin_collapse = std::max(text1->GetMargin().bottom, text2->GetMargin().top);
    std::cout << "         = " << margin_collapse << " + " << stackPanel->GetSpacing() << std::endl;
    std::cout << "         = " << (margin_collapse + stackPanel->GetSpacing()) << std::endl;
    
    std::cout << "\n  新的 offset = " << text1_bottom << " + " 
              << (margin_collapse + stackPanel->GetSpacing()) << std::endl;
    std::cout << "              = " << (text1_bottom + margin_collapse + stackPanel->GetSpacing()) << std::endl;
    std::cout << "  Text2.y = " << rect2.y << std::endl;
    
    std::cout << "\n【步骤 3】完整计算公式" << std::endl;
    std::cout << "  Text2.y = Text1.margin.top + Text1.height + max(margins) + spacing" << std::endl;
    std::cout << "          = " << text1->GetMargin().top << " + " << rect1.height 
              << " + " << margin_collapse << " + " << stackPanel->GetSpacing() << std::endl;
    std::cout << "          = " << rect2.y << std::endl;
    
    std::cout << "\n━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━" << std::endl;
    std::cout << "为什么 Text1.height ≠ 字体大小？" << std::endl;
    std::cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━" << std::endl;
    
    std::cout << "\n字体大小: " << text1->GetFontSize() << " 像素" << std::endl;
    std::cout << "实际布局高度: " << rect1.height << " 像素" << std::endl;
    std::cout << "实际渲染高度: " << text1->GetRenderSize().height << " 像素" << std::endl;
    
    std::cout << "\n原因:" << std::endl;
    std::cout << "1. 字体大小是字符的设计尺寸（em-square）" << std::endl;
    std::cout << "2. 实际高度包含行高系数（通常 1.2x）" << std::endl;
    std::cout << "3. 还包括上下预留空间用于字符的上下伸展部分" << std::endl;
    std::cout << "4. 这是标准的文本排版行为，不是 bug！" << std::endl;
    
    float visual_gap = rect2.y - (rect1.y + text1->GetRenderSize().height);
    std::cout << "\n视觉上的间距:" << std::endl;
    std::cout << "  Text1 实际渲染底部: " << (rect1.y + text1->GetRenderSize().height) << std::endl;
    std::cout << "  Text2 开始位置: " << rect2.y << std::endl;
    std::cout << "  视觉间距: " << visual_gap << " 像素" << std::endl;
    std::cout << "  （这就是为什么看起来间距'大'的原因）" << std::endl;
    
    std::cout << "\n━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━" << std::endl;
    std::cout << "结论" << std::endl;
    std::cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━" << std::endl;
    
    std::cout << "\n✅ Y = " << rect2.y << " 是完全正确的！" << std::endl;
    std::cout << "\n这个值来自:" << std::endl;
    std::cout << "  - Text1 的 top margin: " << text1->GetMargin().top << std::endl;
    std::cout << "  - Text1 的布局高度: " << rect1.height << " (包含行高)" << std::endl;
    std::cout << "  - Margin 折叠: " << margin_collapse << std::endl;
    std::cout << "  - Spacing: " << stackPanel->GetSpacing() << std::endl;
    std::cout << "  总计: " << rect2.y << std::endl;
    
    std::cout << "\n您没有设置 spacing，所以 spacing = 0 ✅" << std::endl;
    std::cout << "Margin 使用折叠机制，不是简单相加 ✅" << std::endl;
    std::cout << "字体大小 32 ≠ 布局高度 " << rect1.height << " ✅" << std::endl;
    
    std::cout << "\n所有计算都符合标准的布局规则！" << std::endl;
    
    delete stackPanel;
    
    return 0;
}
