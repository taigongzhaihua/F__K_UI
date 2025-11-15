#include "fk/app/Application.h"
#include "fk/ui/Window.h"
#include "fk/ui/TextBlock.h"
#include "fk/ui/Brush.h"
#include "fk/ui/StackPanel.h"
#include <iostream>

int main(int argc, char **argv)
{
    std::cout << "=== Debug Layout Test ===" << std::endl;
    
    // Create elements
    auto* stackPanel = new fk::ui::StackPanel();
    stackPanel->SetOrientation(fk::ui::Orientation::Vertical);
    stackPanel->SetSpacing(10.0f);
    
    auto* text1 = new fk::ui::TextBlock();
    text1->SetText("First Element");
    text1->SetFontSize(32);
    text1->SetForeground(fk::ui::Brushes::Blue());
    text1->SetMargin(fk::Thickness(20));
    
    auto* text2 = new fk::ui::TextBlock();
    text2->SetText("Second Element");
    text2->SetFontSize(16);
    text2->SetForeground(fk::ui::Brushes::Red());
    text2->SetMargin(fk::Thickness(20));
    
    auto* text3 = new fk::ui::TextBlock();
    text3->SetText("Third Element");
    text3->SetFontSize(16);
    text3->SetForeground(fk::ui::Brushes::Green());
    text3->SetMargin(fk::Thickness(20));
    
    // Add children to stack panel
    stackPanel->Children({text1, text2, text3});
    
    // Simulate layout
    std::cout << "\n--- Before Layout ---" << std::endl;
    std::cout << "Text1 layoutRect: (" << text1->GetLayoutRect().x << ", " 
              << text1->GetLayoutRect().y << ", " 
              << text1->GetLayoutRect().width << ", " 
              << text1->GetLayoutRect().height << ")" << std::endl;
    std::cout << "Text2 layoutRect: (" << text2->GetLayoutRect().x << ", " 
              << text2->GetLayoutRect().y << ", " 
              << text2->GetLayoutRect().width << ", " 
              << text2->GetLayoutRect().height << ")" << std::endl;
    std::cout << "Text3 layoutRect: (" << text3->GetLayoutRect().x << ", " 
              << text3->GetLayoutRect().y << ", " 
              << text3->GetLayoutRect().width << ", " 
              << text3->GetLayoutRect().height << ")" << std::endl;
    
    // Measure phase
    std::cout << "\n--- Measure Phase ---" << std::endl;
    fk::ui::Size availableSize(800, 600);
    stackPanel->Measure(availableSize);
    
    std::cout << "StackPanel desired size: (" << stackPanel->GetDesiredSize().width 
              << ", " << stackPanel->GetDesiredSize().height << ")" << std::endl;
    std::cout << "Text1 desired size: (" << text1->GetDesiredSize().width 
              << ", " << text1->GetDesiredSize().height << ")" << std::endl;
    std::cout << "Text2 desired size: (" << text2->GetDesiredSize().width 
              << ", " << text2->GetDesiredSize().height << ")" << std::endl;
    std::cout << "Text3 desired size: (" << text3->GetDesiredSize().width 
              << ", " << text3->GetDesiredSize().height << ")" << std::endl;
    
    // Arrange phase
    std::cout << "\n--- Arrange Phase ---" << std::endl;
    stackPanel->Arrange(fk::ui::Rect(0, 0, 800, 600));
    
    std::cout << "StackPanel layoutRect: (" << stackPanel->GetLayoutRect().x << ", " 
              << stackPanel->GetLayoutRect().y << ", " 
              << stackPanel->GetLayoutRect().width << ", " 
              << stackPanel->GetLayoutRect().height << ")" << std::endl;
    
    std::cout << "\n--- After Layout ---" << std::endl;
    std::cout << "Text1 layoutRect: (" << text1->GetLayoutRect().x << ", " 
              << text1->GetLayoutRect().y << ", " 
              << text1->GetLayoutRect().width << ", " 
              << text1->GetLayoutRect().height << ")" << std::endl;
    std::cout << "Text2 layoutRect: (" << text2->GetLayoutRect().x << ", " 
              << text2->GetLayoutRect().y << ", " 
              << text2->GetLayoutRect().width << ", " 
              << text2->GetLayoutRect().height << ")" << std::endl;
    std::cout << "Text3 layoutRect: (" << text3->GetLayoutRect().x << ", " 
              << text3->GetLayoutRect().y << ", " 
              << text3->GetLayoutRect().width << ", " 
              << text3->GetLayoutRect().height << ")" << std::endl;
    
    // Check if elements overlap
    std::cout << "\n--- Overlap Check ---" << std::endl;
    auto rect1 = text1->GetLayoutRect();
    auto rect2 = text2->GetLayoutRect();
    auto rect3 = text3->GetLayoutRect();
    
    bool overlap12 = (rect1.y == rect2.y);
    bool overlap23 = (rect2.y == rect3.y);
    bool overlap13 = (rect1.y == rect3.y);
    
    if (overlap12 || overlap23 || overlap13) {
        std::cout << "WARNING: Elements are overlapping!" << std::endl;
        if (overlap12) std::cout << "  - Text1 and Text2 overlap (same Y position)" << std::endl;
        if (overlap23) std::cout << "  - Text2 and Text3 overlap (same Y position)" << std::endl;
        if (overlap13) std::cout << "  - Text1 and Text3 overlap (same Y position)" << std::endl;
    } else {
        std::cout << "OK: Elements are properly spaced" << std::endl;
    }
    
    // Calculate expected positions
    std::cout << "\n--- Expected Positions ---" << std::endl;
    float expectedY1 = rect1.y;
    float expectedY2 = rect1.y + rect1.height + std::max(text1->GetMargin().bottom, text2->GetMargin().top) + 10.0f;
    float expectedY3 = rect2.y + rect2.height + std::max(text2->GetMargin().bottom, text3->GetMargin().top) + 10.0f;
    
    std::cout << "Expected Y for Text1: " << expectedY1 << ", Actual: " << rect1.y << std::endl;
    std::cout << "Expected Y for Text2: " << expectedY2 << ", Actual: " << rect2.y << std::endl;
    std::cout << "Expected Y for Text3: " << expectedY3 << ", Actual: " << rect3.y << std::endl;
    
    delete stackPanel;
    
    return 0;
}
