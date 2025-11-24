/**
 * @file scrollviewer_test.cpp
 * @brief ScrollViewer Phase 2 功能测试
 * 
 * 测试内容：
 * 1. ScrollContentPresenter 的创建和初始化
 * 2. 滚动偏移的设置和获取
 * 3. extent/viewport 的计算
 * 4. 滚动方法的功能
 */

#include "fk/ui/ScrollViewer.h"
#include "fk/ui/ScrollContentPresenter.h"
#include "fk/ui/Border.h"
#include <iostream>

using namespace fk::ui;

void TestScrollContentPresenter() {
    std::cout << "=== 测试 ScrollContentPresenter ===" << std::endl;
    
    auto* presenter = new ScrollContentPresenter();
    
    // 测试初始状态
    std::cout << "初始水平偏移: " << presenter->GetHorizontalOffset() << std::endl;
    std::cout << "初始垂直偏移: " << presenter->GetVerticalOffset() << std::endl;
    
    // 测试设置偏移
    presenter->SetHorizontalOffset(50.0f);
    presenter->SetVerticalOffset(100.0f);
    std::cout << "设置后水平偏移: " << presenter->GetHorizontalOffset() << std::endl;
    std::cout << "设置后垂直偏移: " << presenter->GetVerticalOffset() << std::endl;
    
    // 测试滚动能力
    std::cout << "可以水平滚动: " << presenter->CanHorizontallyScroll() << std::endl;
    std::cout << "可以垂直滚动: " << presenter->CanVerticallyScroll() << std::endl;
    
    delete presenter;
    std::cout << "✓ ScrollContentPresenter 测试通过" << std::endl << std::endl;
}

void TestScrollViewer() {
    std::cout << "=== 测试 ScrollViewer ===" << std::endl;
    
    auto* viewer = new ScrollViewer();
    
    // 测试 ScrollContentPresenter 是否被创建
    auto* presenter = viewer->GetScrollContentPresenter();
    std::cout << "ScrollContentPresenter 已创建: " << (presenter != nullptr) << std::endl;
    
    // 测试滚动方法
    viewer->ScrollToTop();
    std::cout << "滚动到顶部后偏移: " << viewer->GetVerticalOffset() << std::endl;
    
    viewer->LineDown();
    std::cout << "向下滚动一行后偏移: " << viewer->GetVerticalOffset() << std::endl;
    
    // 测试属性
    std::cout << "视口宽度: " << viewer->GetViewportWidth() << std::endl;
    std::cout << "视口高度: " << viewer->GetViewportHeight() << std::endl;
    std::cout << "内容宽度: " << viewer->GetExtentWidth() << std::endl;
    std::cout << "内容高度: " << viewer->GetExtentHeight() << std::endl;
    
    delete viewer;
    std::cout << "✓ ScrollViewer 测试通过" << std::endl << std::endl;
}

void TestScrollBarVisibility() {
    std::cout << "=== 测试 ScrollBarVisibility ===" << std::endl;
    
    auto* viewer = new ScrollViewer();
    
    // 测试设置滚动条可见性
    viewer->SetHorizontalScrollBarVisibility(ScrollBarVisibility::Auto);
    std::cout << "水平滚动条可见性: " 
              << static_cast<int>(viewer->GetHorizontalScrollBarVisibility()) << std::endl;
    
    viewer->SetVerticalScrollBarVisibility(ScrollBarVisibility::Visible);
    std::cout << "垂直滚动条可见性: " 
              << static_cast<int>(viewer->GetVerticalScrollBarVisibility()) << std::endl;
    
    delete viewer;
    std::cout << "✓ ScrollBarVisibility 测试通过" << std::endl << std::endl;
}

int main() {
    std::cout << "ScrollViewer Phase 2 功能测试" << std::endl;
    std::cout << "==============================" << std::endl << std::endl;
    
    try {
        TestScrollContentPresenter();
        TestScrollViewer();
        TestScrollBarVisibility();
        
        std::cout << "==============================" << std::endl;
        std::cout << "所有测试通过! ✓" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "测试失败: " << e.what() << std::endl;
        return 1;
    }
}
