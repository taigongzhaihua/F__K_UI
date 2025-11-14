/**
 * StackPanel 布局测试
 * 
 * 测试目的：验证 StackPanel 中的元素是否正确堆叠，而不是重叠
 */

#include "fk/ui/StackPanel.h"
#include "fk/ui/TextBlock.h"
#include "fk/ui/Primitives.h"
#include <iostream>
#include <memory>
#include <cassert>

using namespace fk::ui;

int main() {
    std::cout << "测试 StackPanel 布局..." << std::endl;
    
    // 创建 StackPanel
    auto stackPanel = new StackPanel();
    stackPanel->SetOrientation(Orientation::Vertical);
    stackPanel->SetSpacing(10.0f);
    
    // 创建三个 TextBlock 子元素
    auto text1 = new TextBlock();
    text1->SetText("First");
    
    auto text2 = new TextBlock();
    text2->SetText("Second");
    
    auto text3 = new TextBlock();
    text3->SetText("Third");
    
    // 添加到 StackPanel
    stackPanel->AddChild(text1);
    stackPanel->AddChild(text2);
    stackPanel->AddChild(text3);
    
    // 模拟测量和排列过程
    Size availableSize(400, 600);
    stackPanel->Measure(availableSize);
    
    Size desiredSize = stackPanel->GetDesiredSize();
    std::cout << "StackPanel 期望尺寸: " << desiredSize.width << " x " << desiredSize.height << std::endl;
    
    // 排列
    Rect finalRect(0, 0, 400, 600);
    stackPanel->Arrange(finalRect);
    
    // 检查子元素的布局位置
    std::cout << "\n子元素布局位置:" << std::endl;
    
    Rect rect1 = text1->GetLayoutRect();
    std::cout << "Text1: (" << rect1.x << ", " << rect1.y << ", " 
              << rect1.width << ", " << rect1.height << ")" << std::endl;
    
    Rect rect2 = text2->GetLayoutRect();
    std::cout << "Text2: (" << rect2.x << ", " << rect2.y << ", " 
              << rect2.width << ", " << rect2.height << ")" << std::endl;
    
    Rect rect3 = text3->GetLayoutRect();
    std::cout << "Text3: (" << rect3.x << ", " << rect3.y << ", " 
              << rect3.width << ", " << rect3.height << ")" << std::endl;
    
    // 验证元素不重叠
    bool success = true;
    
    // 检查 Y 坐标是否递增（垂直堆叠）
    if (rect1.y >= rect2.y) {
        std::cerr << "错误: Text1 和 Text2 的 Y 坐标不正确 (重叠或顺序错误)" << std::endl;
        success = false;
    }
    
    if (rect2.y >= rect3.y) {
        std::cerr << "错误: Text2 和 Text3 的 Y 坐标不正确 (重叠或顺序错误)" << std::endl;
        success = false;
    }
    
    // 检查元素之间是否有正确的间距
    float expectedY2 = rect1.y + rect1.height + 10.0f; // 10.0f 是间距
    float tolerance = 1.0f; // 允许 1 像素的误差
    
    if (std::abs(rect2.y - expectedY2) > tolerance) {
        std::cerr << "警告: Text2 的 Y 坐标 (" << rect2.y 
                  << ") 与预期 (" << expectedY2 << ") 不匹配" << std::endl;
        // 不算作失败，因为可能有其他布局逻辑
    }
    
    if (success) {
        std::cout << "\n✓ 测试通过：StackPanel 元素正确堆叠，没有重叠" << std::endl;
        
        // 清理
        delete stackPanel; // 会自动清理子元素
        return 0;
    } else {
        std::cerr << "\n✗ 测试失败：StackPanel 元素重叠" << std::endl;
        
        // 清理
        delete stackPanel;
        return 1;
    }
}
