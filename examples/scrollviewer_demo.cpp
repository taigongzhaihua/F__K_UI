/**
 * @file scrollviewer_demo.cpp
 * @brief ScrollViewer 滚动查看器演示
 * 
 * 本演示展示 F__K_UI 的滚动查看器功能：
 * 1. ScrollBar - 基础滚动条
 * 2. ScrollViewer - 滚动查看器
 * 3. 自动滚动条显示
 * 4. 滚动偏移控制
 */

#include "fk/ui/ScrollViewer.h"
#include "fk/ui/ScrollBar.h"
#include "fk/ui/StackPanel.h"
#include "fk/ui/TextBlock.h"
#include "fk/ui/Border.h"
#include <iostream>
#include <memory>

using namespace fk::ui;

void PrintSeparator() {
    std::cout << "\n" << std::string(60, '=') << "\n\n";
}

void Test1_ScrollBarBasic() {
    std::cout << "测试 1: 基础ScrollBar控件\n";
    std::cout << "------------------------\n";
    
    // 创建垂直滚动条
    auto scrollBar = new ScrollBar();
    scrollBar->SetOrientation(Orientation::Vertical);
    scrollBar->SetMinimum(0.0f);
    scrollBar->SetMaximum(100.0f);
    scrollBar->SetValue(25.0f);
    scrollBar->SetViewportSize(10.0f);
    
    std::cout << "✓ 创建垂直滚动条\n";
    std::cout << "  方向: Vertical\n";
    std::cout << "  最小值: " << scrollBar->GetMinimum() << "\n";
    std::cout << "  最大值: " << scrollBar->GetMaximum() << "\n";
    std::cout << "  当前值: " << scrollBar->GetValue() << "\n";
    std::cout << "  视口大小: " << scrollBar->GetViewportSize() << "\n";
    
    delete scrollBar;
    PrintSeparator();
}

void Test2_ScrollBarOperations() {
    std::cout << "测试 2: ScrollBar滚动操作\n";
    std::cout << "------------------------\n";
    
    auto scrollBar = new ScrollBar();
    scrollBar->SetMinimum(0.0f);
    scrollBar->SetMaximum(100.0f);
    scrollBar->SetValue(50.0f);
    scrollBar->SetSmallChange(1.0f);
    scrollBar->SetLargeChange(10.0f);
    
    std::cout << "初始值: " << scrollBar->GetValue() << "\n";
    
    // 测试各种滚动操作
    scrollBar->LineDown();
    std::cout << "LineDown后: " << scrollBar->GetValue() << " (+1)\n";
    
    scrollBar->LineUp();
    std::cout << "LineUp后: " << scrollBar->GetValue() << " (-1)\n";
    
    scrollBar->PageDown();
    std::cout << "PageDown后: " << scrollBar->GetValue() << " (+10)\n";
    
    scrollBar->PageUp();
    std::cout << "PageUp后: " << scrollBar->GetValue() << " (-10)\n";
    
    scrollBar->ScrollToStart();
    std::cout << "ScrollToStart后: " << scrollBar->GetValue() << "\n";
    
    scrollBar->ScrollToEnd();
    std::cout << "ScrollToEnd后: " << scrollBar->GetValue() << "\n";
    
    delete scrollBar;
    PrintSeparator();
}

void Test3_ScrollBarEvents() {
    std::cout << "测试 3: ScrollBar值变更事件\n";
    std::cout << "--------------------------\n";
    
    auto scrollBar = new ScrollBar();
    scrollBar->SetMinimum(0.0f);
    scrollBar->SetMaximum(100.0f);
    scrollBar->SetValue(0.0f);
    
    int eventCount = 0;
    scrollBar->ValueChanged.Connect([&eventCount](float oldValue, float newValue) {
        std::cout << "  事件触发 #" << (++eventCount) 
                  << ": " << oldValue << " → " << newValue << "\n";
    });
    
    std::cout << "订阅ValueChanged事件\n";
    std::cout << "执行滚动操作...\n";
    
    scrollBar->SetValue(25.0f);
    scrollBar->SetValue(50.0f);
    scrollBar->SetValue(75.0f);
    scrollBar->SetValue(100.0f);
    
    std::cout << "✓ 总共触发 " << eventCount << " 次事件\n";
    
    delete scrollBar;
    PrintSeparator();
}

void Test4_ScrollViewerBasic() {
    std::cout << "测试 4: 基础ScrollViewer控件\n";
    std::cout << "---------------------------\n";
    
    // 创建ScrollViewer
    auto scrollViewer = new ScrollViewer();
    scrollViewer->SetWidth(300);
    scrollViewer->SetHeight(200);
    
    // 创建大内容（模拟）
    auto content = new Border();
    content->SetWidth(600);
    content->SetHeight(400);
    
    scrollViewer->SetContent(content);
    
    std::cout << "✓ 创建ScrollViewer (300x200)\n";
    std::cout << "✓ 设置内容 (600x400)\n";
    std::cout << "  视口宽度: " << scrollViewer->GetViewportWidth() << "\n";
    std::cout << "  视口高度: " << scrollViewer->GetViewportHeight() << "\n";
    std::cout << "  内容宽度: " << scrollViewer->GetExtentWidth() << "\n";
    std::cout << "  内容高度: " << scrollViewer->GetExtentHeight() << "\n";
    std::cout << "  水平偏移: " << scrollViewer->GetHorizontalOffset() << "\n";
    std::cout << "  垂直偏移: " << scrollViewer->GetVerticalOffset() << "\n";
    
    delete scrollViewer;
    PrintSeparator();
}

void Test5_ScrollViewerMethods() {
    std::cout << "测试 5: ScrollViewer滚动方法\n";
    std::cout << "---------------------------\n";
    
    auto scrollViewer = new ScrollViewer();
    scrollViewer->SetWidth(300);
    scrollViewer->SetHeight(200);
    
    // 创建内容
    auto content = new Border();
    content->SetWidth(600);
    content->SetHeight(800);
    scrollViewer->SetContent(content);
    
    std::cout << "初始偏移: H=" << scrollViewer->GetHorizontalOffset() 
              << ", V=" << scrollViewer->GetVerticalOffset() << "\n";
    
    // 测试滚动方法
    scrollViewer->LineDown();
    std::cout << "LineDown后: V=" << scrollViewer->GetVerticalOffset() << "\n";
    
    scrollViewer->PageDown();
    std::cout << "PageDown后: V=" << scrollViewer->GetVerticalOffset() << "\n";
    
    scrollViewer->ScrollToBottom();
    std::cout << "ScrollToBottom后: V=" << scrollViewer->GetVerticalOffset() << "\n";
    
    scrollViewer->ScrollToTop();
    std::cout << "ScrollToTop后: V=" << scrollViewer->GetVerticalOffset() << "\n";
    
    scrollViewer->LineRight();
    std::cout << "LineRight后: H=" << scrollViewer->GetHorizontalOffset() << "\n";
    
    scrollViewer->ScrollToRightEnd();
    std::cout << "ScrollToRightEnd后: H=" << scrollViewer->GetHorizontalOffset() << "\n";
    
    delete scrollViewer;
    PrintSeparator();
}

void Test6_ScrollBarVisibility() {
    std::cout << "测试 6: 滚动条可见性模式\n";
    std::cout << "-----------------------\n";
    
    auto scrollViewer = new ScrollViewer();
    scrollViewer->SetWidth(300);
    scrollViewer->SetHeight(200);
    
    // 测试不同可见性模式
    std::cout << "1. Auto模式（默认）\n";
    scrollViewer->SetHorizontalScrollBarVisibility(ScrollBarVisibility::Auto);
    scrollViewer->SetVerticalScrollBarVisibility(ScrollBarVisibility::Auto);
    std::cout << "   当内容超出时自动显示滚动条\n";
    
    std::cout << "\n2. Visible模式\n";
    scrollViewer->SetHorizontalScrollBarVisibility(ScrollBarVisibility::Visible);
    scrollViewer->SetVerticalScrollBarVisibility(ScrollBarVisibility::Visible);
    std::cout << "   始终显示滚动条\n";
    
    std::cout << "\n3. Hidden模式\n";
    scrollViewer->SetHorizontalScrollBarVisibility(ScrollBarVisibility::Hidden);
    scrollViewer->SetVerticalScrollBarVisibility(ScrollBarVisibility::Hidden);
    std::cout << "   隐藏滚动条但仍可滚动\n";
    
    std::cout << "\n4. Disabled模式\n";
    scrollViewer->SetHorizontalScrollBarVisibility(ScrollBarVisibility::Disabled);
    scrollViewer->SetVerticalScrollBarVisibility(ScrollBarVisibility::Disabled);
    std::cout << "   禁用滚动功能\n";
    
    delete scrollViewer;
    PrintSeparator();
}

void Test7_HorizontalScrolling() {
    std::cout << "测试 7: 水平滚动\n";
    std::cout << "---------------\n";
    
    auto scrollBar = new ScrollBar();
    scrollBar->SetOrientation(Orientation::Horizontal);
    scrollBar->SetMinimum(0.0f);
    scrollBar->SetMaximum(200.0f);
    scrollBar->SetValue(0.0f);
    scrollBar->SetViewportSize(50.0f);
    
    std::cout << "✓ 创建水平滚动条\n";
    std::cout << "  方向: Horizontal\n";
    std::cout << "  最大值: " << scrollBar->GetMaximum() << "\n";
    std::cout << "  视口大小: " << scrollBar->GetViewportSize() << "\n";
    
    // 模拟滚动
    for (int i = 0; i <= 4; ++i) {
        float value = i * 50.0f;
        scrollBar->SetValue(value);
        std::cout << "  位置 " << i << ": " << scrollBar->GetValue() << "\n";
    }
    
    delete scrollBar;
    PrintSeparator();
}

int main() {
    std::cout << "\n";
    std::cout << "╔══════════════════════════════════════════════════════════╗\n";
    std::cout << "║      F__K_UI 滚动查看器系统演示 (Phase 3)              ║\n";
    std::cout << "╚══════════════════════════════════════════════════════════╝\n";
    
    PrintSeparator();
    
    try {
        Test1_ScrollBarBasic();
        Test2_ScrollBarOperations();
        Test3_ScrollBarEvents();
        Test4_ScrollViewerBasic();
        Test5_ScrollViewerMethods();
        Test6_ScrollBarVisibility();
        Test7_HorizontalScrolling();
        
        std::cout << "\n";
        std::cout << "╔══════════════════════════════════════════════════════════╗\n";
        std::cout << "║                   所有测试完成！                        ║\n";
        std::cout << "╠══════════════════════════════════════════════════════════╣\n";
        std::cout << "║  ✓ ScrollBar - 滚动条控件                               ║\n";
        std::cout << "║  ✓ ScrollViewer - 滚动查看器                            ║\n";
        std::cout << "║  ✓ 水平/垂直滚动支持                                    ║\n";
        std::cout << "║  ✓ 自动滚动条显示                                       ║\n";
        std::cout << "║  ✓ 滚动偏移控制                                         ║\n";
        std::cout << "║  ✓ 滚动事件系统                                         ║\n";
        std::cout << "╚══════════════════════════════════════════════════════════╝\n";
        std::cout << "\n";
        
    } catch (const std::exception& e) {
        std::cerr << "\n❌ 错误: " << e.what() << "\n";
        return 1;
    }
    
    return 0;
}
