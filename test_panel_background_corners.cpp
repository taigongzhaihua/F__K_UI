/**
 * 测试Panel（StackPanel和Grid）的背景和圆角功能
 */

#include "fk/ui/StackPanel.h"
#include "fk/ui/Grid.h"
#include "fk/ui/Border.h"
#include "fk/ui/TextBlock.h"
#include "fk/ui/Brush.h"
#include <iostream>
#include <cassert>

using namespace fk::ui;

void PrintTestHeader(const char* testName) {
    std::cout << "\n========== " << testName << " ==========\n";
}

void PrintTestResult(const char* testName, bool passed) {
    std::cout << "[" << (passed ? "PASS" : "FAIL") << "] " << testName << "\n";
}

int main() {
    try {
        // ========== 测试1：StackPanel 属性设置 ==========
        PrintTestHeader("Test 1: StackPanel Background and CornerRadius Properties");
        
        auto stackPanel1 = new StackPanel();
        auto blueBrush = new SolidColorBrush(Color{0.2f, 0.6f, 0.9f, 1.0f});
        stackPanel1->Background(blueBrush);
        stackPanel1->CornerRadius(15.0f);
        
        bool test1Pass = stackPanel1->GetBackground() == blueBrush &&
                         stackPanel1->GetCornerRadius() == CornerRadius(15.0f);
        PrintTestResult("StackPanel 统一圆角设置", test1Pass);
        
        // ========== 测试2：StackPanel 不同圆角 ==========
        PrintTestHeader("Test 2: StackPanel Different Corner Radii");
        
        auto stackPanel2 = new StackPanel();
        auto redBrush = new SolidColorBrush(Color{0.9f, 0.3f, 0.3f, 1.0f});
        stackPanel2->Background(redBrush);
        stackPanel2->CornerRadius(30.0f, 10.0f, 30.0f, 10.0f);
        
        auto corner2 = stackPanel2->GetCornerRadius();
        bool test2Pass = stackPanel2->GetBackground() == redBrush &&
                         corner2.topLeft == 30.0f &&
                         corner2.topRight == 10.0f &&
                         corner2.bottomRight == 30.0f &&
                         corner2.bottomLeft == 10.0f;
        PrintTestResult("StackPanel 不同圆角设置", test2Pass);
        
        // ========== 测试3：Grid 背景和圆角 ==========
        PrintTestHeader("Test 3: Grid Background and CornerRadius");
        
        auto grid = new Grid();
        auto greenBrush = new SolidColorBrush(Color{0.3f, 0.8f, 0.4f, 1.0f});
        grid->Background(greenBrush);
        grid->CornerRadius(5.0f, 20.0f, 35.0f, 50.0f);
        
        auto corner3 = grid->GetCornerRadius();
        bool test3Pass = grid->GetBackground() == greenBrush &&
                         corner3.topLeft == 5.0f &&
                         corner3.topRight == 20.0f &&
                         corner3.bottomRight == 35.0f &&
                         corner3.bottomLeft == 50.0f;
        PrintTestResult("Grid 递增圆角设置", test3Pass);
        
        // ========== 测试4：Border 不同圆角 ==========
        PrintTestHeader("Test 4: Border Different Corner Radii");
        
        auto border = new Border();
        auto yellowBrush = new SolidColorBrush(Color{1.0f, 0.9f, 0.2f, 1.0f});
        border->Background(yellowBrush);
        border->CornerRadius(20.0f, 40.0f, 10.0f, 30.0f);
        
        auto corner4 = border->GetCornerRadius();
        bool test4Pass = border->GetBackground() == yellowBrush &&
                         corner4.topLeft == 20.0f &&
                         corner4.topRight == 40.0f &&
                         corner4.bottomRight == 10.0f &&
                         corner4.bottomLeft == 30.0f;
        PrintTestResult("Border 不同圆角设置", test4Pass);
        
        // 汇总测试结果
        std::cout << "\n========== 测试汇总 ==========\n";
        bool allPass = test1Pass && test2Pass && test3Pass && test4Pass;
        std::cout << "所有测试 " << (allPass ? "通过" : "失败") << "!\n";
        
        // 清理内存
        delete stackPanel1;
        delete stackPanel2;
        delete grid;
        delete border;
        delete blueBrush;
        delete redBrush;
        delete greenBrush;
        delete yellowBrush;
        
        return allPass ? 0 : 1;
        
    } catch (const std::exception& e) {
        std::cerr << "异常: " << e.what() << std::endl;
        return 1;
    }
}
