/**
 * @file test_grid_complete.cpp
 * @brief Grid 完整功能测试
 * 
 * 测试内容：
 * 1. Auto/Pixel/Star 尺寸计算
 * 2. 字符串解析便捷方法
 * 3. Min/Max 约束
 * 4. 行列跨度
 * 5. 对齐和边距
 * 6. 流式附加属性语法
 * 7. 性能缓存机制
 */

#include "fk/ui/Grid.h"
#include "fk/ui/Button.h"
#include "fk/ui/TextBlock.h"
#include "fk/ui/Border.h"
#include "fk/ui/Thickness.h"
#include "fk/ui/Alignment.h"
#include <iostream>
#include <cassert>
#include <chrono>

using namespace fk::ui;

void PrintTestHeader(const char* testName) {
    std::cout << "\n========== " << testName << " ==========\n";
}

void PrintTestResult(const char* testName, bool passed) {
    std::cout << "[" << (passed ? "PASS" : "FAIL") << "] " << testName << "\n";
}

// 测试 1：基本的 Auto/Pixel/Star 尺寸
void Test1_BasicSizing() {
    PrintTestHeader("Test 1: Basic Auto/Pixel/Star Sizing");
    
    auto grid = new Grid();
    grid->RowDefinitions({
        RowDefinition::Auto(),
        RowDefinition::Pixel(100),
        RowDefinition::Star(2)
    });
    grid->ColumnDefinitions({
        ColumnDefinition::Star(1),
        ColumnDefinition::Pixel(200),
        ColumnDefinition::Star(1)
    });
    
    // 测量
    grid->Measure(Size(800, 600));
    Size desired = grid->GetDesiredSize();
    
    std::cout << "Desired Size: " << desired.width << " x " << desired.height << "\n";
    
    const auto& rows = grid->GetRowDefinitions();
    const auto& cols = grid->GetColumnDefinitions();
    
    std::cout << "Row Heights: " << rows[0].actualHeight << ", " 
              << rows[1].actualHeight << ", " << rows[2].actualHeight << "\n";
    std::cout << "Column Widths: " << cols[0].actualWidth << ", " 
              << cols[1].actualWidth << ", " << cols[2].actualWidth << "\n";
    
    // 验证：Pixel 行应该是 100
    bool passed = (rows[1].actualHeight == 100.0f) && (cols[1].actualWidth == 200.0f);
    PrintTestResult("Basic Sizing", passed);
    
    delete grid;
}

// 测试 2：字符串解析
void Test2_StringParsing() {
    PrintTestHeader("Test 2: String Parsing");
    
    auto grid = new Grid();
    grid->Rows("Auto, 100, 2*, *");
    grid->Columns("*, 200, Auto");
    
    const auto& rows = grid->GetRowDefinitions();
    const auto& cols = grid->GetColumnDefinitions();
    
    std::cout << "Parsed Rows: " << rows.size() << " (expected 4)\n";
    std::cout << "Parsed Cols: " << cols.size() << " (expected 3)\n";
    
    bool passed = (rows.size() == 4) && (cols.size() == 3);
    
    // 验证类型
    passed = passed && (rows[0].type == RowDefinition::SizeType::Auto);
    passed = passed && (rows[1].type == RowDefinition::SizeType::Pixel && rows[1].value == 100.0f);
    passed = passed && (rows[2].type == RowDefinition::SizeType::Star && rows[2].value == 2.0f);
    passed = passed && (rows[3].type == RowDefinition::SizeType::Star && rows[3].value == 1.0f);
    
    std::cout << "Row[0]: " << (rows[0].type == RowDefinition::SizeType::Auto ? "Auto" : "?") << "\n";
    std::cout << "Row[1]: Pixel(" << rows[1].value << ")\n";
    std::cout << "Row[2]: Star(" << rows[2].value << ")\n";
    std::cout << "Row[3]: Star(" << rows[3].value << ")\n";
    
    PrintTestResult("String Parsing", passed);
    
    delete grid;
}

// 测试 3：Min/Max 约束
void Test3_Constraints() {
    PrintTestHeader("Test 3: Min/Max Constraints");
    
    auto grid = new Grid();
    
    // 创建带约束的行定义
    auto row1 = RowDefinition::Star(1).MinHeight(50).MaxHeight(150);
    auto row2 = RowDefinition::Star(1).MinHeight(100).MaxHeight(200);
    
    grid->AddRowDefinition(row1);
    grid->AddRowDefinition(row2);
    grid->AddColumnDefinition(ColumnDefinition::Star(1));
    
    // 测量（总高度 300，应该分配 150 + 150）
    grid->Measure(Size(400, 300));
    
    const auto& rows = grid->GetRowDefinitions();
    
    std::cout << "Row[0] Height: " << rows[0].actualHeight 
              << " (min=" << rows[0].minHeight << ", max=" << rows[0].maxHeight << ")\n";
    std::cout << "Row[1] Height: " << rows[1].actualHeight 
              << " (min=" << rows[1].minHeight << ", max=" << rows[1].maxHeight << ")\n";
    
    // 验证：第一行应该被限制在 maxHeight (150)
    bool passed = (rows[0].actualHeight <= rows[0].maxHeight + 0.1f) &&
                  (rows[0].actualHeight >= rows[0].minHeight - 0.1f);
    
    PrintTestResult("Min/Max Constraints", passed);
    
    delete grid;
}

// 测试 4：行列跨度
void Test4_Spanning() {
    PrintTestHeader("Test 4: Row/Column Spanning");
    
    auto grid = new Grid();
    grid->Rows("100, 100, 100");
    grid->Columns("100, 100, 100");
    
    auto button = new Button();
    button->Width(250)->Height(250);
    Grid::SetRow(button, 0);
    Grid::SetColumn(button, 0);
    Grid::SetRowSpan(button, 2);
    Grid::SetColumnSpan(button, 2);
    
    grid->AddChild(button);
    
    // 测量和排列
    grid->Measure(Size(300, 300));
    grid->Arrange(Rect(0, 0, 300, 300));
    
    Rect buttonRect = button->GetLayoutRect();
    
    std::cout << "Button Layout: " << buttonRect.x << ", " << buttonRect.y 
              << ", " << buttonRect.width << " x " << buttonRect.height << "\n";
    
    // 验证：按钮应该占据 2x2 单元格（200x200）
    bool passed = (buttonRect.width >= 199.0f && buttonRect.width <= 201.0f) &&
                  (buttonRect.height >= 199.0f && buttonRect.height <= 201.0f);
    
    PrintTestResult("Row/Column Spanning", passed);
    
    delete grid;
}

// 测试 5：对齐和边距
void Test5_AlignmentAndMargin() {
    PrintTestHeader("Test 5: Alignment and Margin");
    
    auto grid = new Grid();
    grid->Rows("200");
    grid->Columns("200");
    
    auto button = new Button();
    button->Width(100)->Height(50);
    button->Margin(fk::Thickness(10, 10, 10, 10));
    button->SetHAlign(HorizontalAlignment::Center);
    button->SetVAlign(VerticalAlignment::Center);
    
    grid->AddChild(button);
    
    // 测量和排列
    grid->Measure(Size(200, 200));
    grid->Arrange(Rect(0, 0, 200, 200));
    
    Rect buttonRect = button->GetLayoutRect();
    
    std::cout << "Button Position: " << buttonRect.x << ", " << buttonRect.y << "\n";
    std::cout << "Button Size: " << buttonRect.width << " x " << buttonRect.height << "\n";
    
    // 验证：按钮应该居中
    // 可用空间：200 - 20(margin) = 180
    // 按钮宽度：100，应该在 (10 + (180-100)/2) = 50
    float expectedX = 10 + (180 - 100) / 2.0f;
    float expectedY = 10 + (180 - 50) / 2.0f;
    
    bool passed = (std::abs(buttonRect.x - expectedX) < 1.0f) &&
                  (std::abs(buttonRect.y - expectedY) < 1.0f);
    
    std::cout << "Expected Position: " << expectedX << ", " << expectedY << "\n";
    
    PrintTestResult("Alignment and Margin", passed);
    
    delete grid;
}

// 测试 6：流式附加属性语法
void Test6_FluentAttachedProperties() {
    PrintTestHeader("Test 6: Fluent Attached Property Syntax");
    
    auto grid = new Grid();
    grid->Rows("*, *")->Columns("*, *");
    
    // 使用 Grid 静态方法
    auto button1 = new Button();
    button1->Width(100)->Height(50);
    Grid::SetRow(button1, 0);
    Grid::SetColumn(button1, 0);
    grid->AddChild(button1);
    
    auto button2 = new Button();
    button2->Width(100)->Height(50);
    Grid::SetRow(button2, 0);
    Grid::SetColumn(button2, 1);
    Grid::SetRowSpan(button2, 2);
    grid->AddChild(button2);
    
    auto button3 = new Button();
    button3->Width(100)->Height(50);
    Grid::SetRow(button3, 1);
    Grid::SetColumn(button3, 0);
    grid->AddChild(button3);
    
    // 验证属性
    int row1 = Grid::GetRow(button1);
    int col1 = Grid::GetColumn(button1);
    int row2 = Grid::GetRow(button2);
    int col2 = Grid::GetColumn(button2);
    int rowSpan2 = Grid::GetRowSpan(button2);
    
    std::cout << "Button1: Row=" << row1 << ", Col=" << col1 << "\n";
    std::cout << "Button2: Row=" << row2 << ", Col=" << col2 << ", RowSpan=" << rowSpan2 << "\n";
    
    bool passed = (row1 == 0 && col1 == 0) &&
                  (row2 == 0 && col2 == 1 && rowSpan2 == 2);
    
    PrintTestResult("Fluent Attached Properties", passed);
    
    delete grid;
}

// 测试 7：Auto 尺寸内容测量
void Test7_AutoSizingWithContent() {
    PrintTestHeader("Test 7: Auto Sizing with Content");
    
    auto grid = new Grid();
    grid->Rows("Auto, Auto, *");
    grid->Columns("*");
    
    // 第一行：带固定高度的按钮
    auto button1 = new Button();
    button1->Height(50);
    Grid::SetRow(button1, 0);
    grid->AddChild(button1);
    
    // 第二行：带固定高度的按钮
    auto button2 = new Button();
    button2->Height(80);
    Grid::SetRow(button2, 1);
    grid->AddChild(button2);
    
    // 第三行：Star（填充剩余空间）
    auto button3 = new Button();
    Grid::SetRow(button3, 2);
    grid->AddChild(button3);
    
    // 测量
    grid->Measure(Size(400, 300));
    
    const auto& rows = grid->GetRowDefinitions();
    
    std::cout << "Row[0] (Auto): " << rows[0].actualHeight << " (expected ~50)\n";
    std::cout << "Row[1] (Auto): " << rows[1].actualHeight << " (expected ~80)\n";
    std::cout << "Row[2] (Star): " << rows[2].actualHeight << " (expected ~170)\n";
    
    // 验证：Auto 行应该根据内容调整
    bool passed = (rows[0].actualHeight >= 49.0f && rows[0].actualHeight <= 51.0f) &&
                  (rows[1].actualHeight >= 79.0f && rows[1].actualHeight <= 81.0f);
    
    PrintTestResult("Auto Sizing with Content", passed);
    
    delete grid;
}

// 测试 8：性能缓存机制
void Test8_PerformanceCache() {
    PrintTestHeader("Test 8: Performance Cache Mechanism");
    
    auto grid = new Grid();
    grid->Rows("*, *")->Columns("*, *");
    
    for (int i = 0; i < 4; i++) {
        auto btn = new Button();
        Grid::SetRow(btn, i / 2);
        Grid::SetColumn(btn, i % 2);
        grid->AddChild(btn);
    }
    
    // 第一次测量
    auto start1 = std::chrono::high_resolution_clock::now();
    grid->Measure(Size(400, 400));
    auto end1 = std::chrono::high_resolution_clock::now();
    auto duration1 = std::chrono::duration_cast<std::chrono::microseconds>(end1 - start1).count();
    
    // 相同尺寸再次测量（应该使用缓存）
    auto start2 = std::chrono::high_resolution_clock::now();
    grid->Measure(Size(400, 400));
    auto end2 = std::chrono::high_resolution_clock::now();
    auto duration2 = std::chrono::duration_cast<std::chrono::microseconds>(end2 - start2).count();
    
    // 不同尺寸测量（应该重新计算）
    auto start3 = std::chrono::high_resolution_clock::now();
    grid->Measure(Size(500, 500));
    auto end3 = std::chrono::high_resolution_clock::now();
    auto duration3 = std::chrono::duration_cast<std::chrono::microseconds>(end3 - start3).count();
    
    std::cout << "First measure: " << duration1 << " μs\n";
    std::cout << "Cached measure: " << duration2 << " μs\n";
    std::cout << "Different size: " << duration3 << " μs\n";
    
    // 验证：缓存测量应该更快（至少快 20%）
    bool passed = (duration2 < duration1 * 0.8);
    
    PrintTestResult("Performance Cache", passed);
    
    delete grid;
}

// 测试 9：复杂布局场景
void Test9_ComplexLayout() {
    PrintTestHeader("Test 9: Complex Layout Scenario");
    
    auto grid = new Grid();
    grid->Rows("Auto, *, Auto")
        ->Columns("200, *, 200");
    
    // 标题栏（跨越所有列）
    auto header = new Border();
    header->Height(60);
    Grid::SetRow(header, 0);
    Grid::SetColumn(header, 0);
    Grid::SetColumnSpan(header, 3);
    grid->AddChild(header);
    
    // 左侧边栏
    auto sidebar = new Border();
    Grid::SetRow(sidebar, 1);
    Grid::SetColumn(sidebar, 0);
    grid->AddChild(sidebar);
    
    // 主内容区
    auto content = new Border();
    Grid::SetRow(content, 1);
    Grid::SetColumn(content, 1);
    grid->AddChild(content);
    
    // 右侧边栏
    auto rightbar = new Border();
    Grid::SetRow(rightbar, 1);
    Grid::SetColumn(rightbar, 2);
    grid->AddChild(rightbar);
    
    // 状态栏（跨越所有列）
    auto footer = new Border();
    footer->Height(30);
    Grid::SetRow(footer, 2);
    Grid::SetColumn(footer, 0);
    Grid::SetColumnSpan(footer, 3);
    grid->AddChild(footer);
    
    // 测量和排列
    grid->Measure(Size(1024, 768));
    grid->Arrange(Rect(0, 0, 1024, 768));
    
    auto headerRect = header->GetLayoutRect();
    auto contentRect = content->GetLayoutRect();
    auto footerRect = footer->GetLayoutRect();
    
    std::cout << "Header: " << headerRect.width << " x " << headerRect.height << "\n";
    std::cout << "Content: " << contentRect.width << " x " << contentRect.height << "\n";
    std::cout << "Footer: " << footerRect.width << " x " << footerRect.height << "\n";
    
    // 验证：标题和底栏应该跨越整个宽度
    bool passed = (headerRect.width >= 1023.0f) && 
                  (footerRect.width >= 1023.0f) &&
                  (contentRect.width >= 623.0f && contentRect.width <= 625.0f); // 1024 - 200 - 200
    
    PrintTestResult("Complex Layout", passed);
    
    delete grid;
}

int main() {
    std::cout << "\n";
    std::cout << "╔════════════════════════════════════════════════╗\n";
    std::cout << "║   Grid Complete Functionality Test Suite      ║\n";
    std::cout << "╚════════════════════════════════════════════════╝\n";
    
    Test1_BasicSizing();
    Test2_StringParsing();
    Test3_Constraints();
    Test4_Spanning();
    Test5_AlignmentAndMargin();
    Test6_FluentAttachedProperties();
    Test7_AutoSizingWithContent();
    Test8_PerformanceCache();
    Test9_ComplexLayout();
    
    std::cout << "\n";
    std::cout << "╔════════════════════════════════════════════════╗\n";
    std::cout << "║   All Tests Completed                          ║\n";
    std::cout << "╚════════════════════════════════════════════════╝\n";
    std::cout << "\n";
    
    return 0;
}
