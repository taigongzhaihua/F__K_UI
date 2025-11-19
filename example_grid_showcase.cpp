/**
 * @file example_grid_showcase.cpp
 * @brief Grid 功能展示示例
 * 
 * 演示所有新增的 Grid 功能：
 * - 字符串解析
 * - Min/Max 约束
 * - 流式附加属性
 * - 对齐和边距
 * - Auto 尺寸计算
 */

#include "fk/ui/Grid.h"
#include "fk/ui/Button.h"
#include "fk/ui/TextBlock.h"
#include "fk/ui/Border.h"
#include "fk/ui/StackPanel.h"
#include <iostream>

using namespace fk::ui;

void ShowExample1_BasicLayout() {
    std::cout << "\n========== 示例 1: 基本三列布局 ==========\n";
    
    auto mainGrid = new Grid();
    
    // 使用字符串解析定义行列（最简洁的方式）
    mainGrid->Rows("60, *, 30")      // 顶栏 60px，内容区自适应，底栏 30px
            ->Columns("200, *, 200"); // 左边栏 200px，主区自适应，右边栏 200px
    
    // 顶部工具栏（跨越所有列）
    auto toolbar = mainGrid->AddChild<Border>()
        ->Row(0)->Column(0)->ColumnSpan(3)
        ->Background(0xFF2B2B2B);
    
    std::cout << "✓ 顶部工具栏：跨 3 列，高度 60px\n";
    
    // 左侧边栏
    auto leftSidebar = mainGrid->AddChild<Border>()
        ->Row(1)->Column(0)
        ->Background(0xFF1E1E1E);
    
    std::cout << "✓ 左侧边栏：宽度 200px\n";
    
    // 主内容区
    auto contentArea = mainGrid->AddChild<Border>()
        ->Row(1)->Column(1)
        ->Background(0xFF252526);
    
    std::cout << "✓ 主内容区：自适应剩余空间\n";
    
    // 右侧边栏
    auto rightSidebar = mainGrid->AddChild<Border>()
        ->Row(1)->Column(2)
        ->Background(0xFF1E1E1E);
    
    std::cout << "✓ 右侧边栏：宽度 200px\n";
    
    // 状态栏（跨越所有列）
    auto statusBar = mainGrid->AddChild<Border>()
        ->Row(2)->Column(0)->ColumnSpan(3)
        ->Background(0xFF007ACC);
    
    std::cout << "✓ 状态栏：跨 3 列，高度 30px\n";
    
    // 测量和排列
    mainGrid->Measure(Size(1024, 768));
    mainGrid->Arrange(Rect(0, 0, 1024, 768));
    
    std::cout << "\n布局结果：\n";
    std::cout << "  工具栏：" << toolbar->GetLayoutRect().width << " x " 
              << toolbar->GetLayoutRect().height << "\n";
    std::cout << "  左边栏：" << leftSidebar->GetLayoutRect().width << " x " 
              << leftSidebar->GetLayoutRect().height << "\n";
    std::cout << "  主内容：" << contentArea->GetLayoutRect().width << " x " 
              << contentArea->GetLayoutRect().height << "\n";
    std::cout << "  右边栏：" << rightSidebar->GetLayoutRect().width << " x " 
              << rightSidebar->GetLayoutRect().height << "\n";
    std::cout << "  状态栏：" << statusBar->GetLayoutRect().width << " x " 
              << statusBar->GetLayoutRect().height << "\n";
    
    delete mainGrid;
}

void ShowExample2_FluentSyntax() {
    std::cout << "\n========== 示例 2: 流式语法演示 ==========\n";
    
    auto grid = new Grid();
    grid->Rows("*, *, *")->Columns("*, *, *");
    
    std::cout << "创建 3x3 网格，使用流式语法添加按钮：\n\n";
    
    // 使用流式语法优雅地设置附加属性
    auto btn1 = grid->AddChild<Button>()
        ->Row(0)->Column(0)
        ->Content("按钮 1")
        ->Width(100)->Height(40);
    
    std::cout << "button1->Row(0)->Column(0)\n";
    
    auto btn2 = grid->AddChild<Button>()
        ->Row(0)->Column(1)->ColumnSpan(2)  // 跨 2 列
        ->Content("按钮 2 (跨列)")
        ->Width(200)->Height(40);
    
    std::cout << "button2->Row(0)->Column(1)->ColumnSpan(2)\n";
    
    auto btn3 = grid->AddChild<Button>()
        ->Row(1)->Column(0)->RowSpan(2)     // 跨 2 行
        ->Content("按钮 3 (跨行)")
        ->Width(100)->Height(80);
    
    std::cout << "button3->Row(1)->Column(0)->RowSpan(2)\n";
    
    auto btn4 = grid->AddChild<Button>()
        ->Row(1)->Column(1)
        ->Content("按钮 4")
        ->HorizontalAlignment(HorizontalAlignment::Center)
        ->VerticalAlignment(VerticalAlignment::Center)
        ->Width(80)->Height(30);
    
    std::cout << "button4->Row(1)->Column(1) [居中对齐]\n";
    
    // 验证附加属性
    std::cout << "\n验证附加属性：\n";
    std::cout << "  按钮 2 ColumnSpan: " << Grid::GetColumnSpan(btn2) << " (期望: 2)\n";
    std::cout << "  按钮 3 RowSpan: " << Grid::GetRowSpan(btn3) << " (期望: 2)\n";
    
    delete grid;
}

void ShowExample3_Constraints() {
    std::cout << "\n========== 示例 3: Min/Max 约束 ==========\n";
    
    auto grid = new Grid();
    
    // 创建带约束的行定义
    auto row1 = RowDefinition::Star(1)
        .MinHeight(50)
        .MaxHeight(200);
    
    auto row2 = RowDefinition::Star(2)
        .MinHeight(100)
        .MaxHeight(400);
    
    auto row3 = RowDefinition::Star(1)
        .MinHeight(50)
        .MaxHeight(200);
    
    grid->AddRowDefinition(row1);
    grid->AddRowDefinition(row2);
    grid->AddRowDefinition(row3);
    grid->AddColumnDefinition(ColumnDefinition::Star(1));
    
    std::cout << "定义 3 行（1*, 2*, 1*），每行都有 Min/Max 约束：\n";
    std::cout << "  行 1: Min=50, Max=200\n";
    std::cout << "  行 2: Min=100, Max=400\n";
    std::cout << "  行 3: Min=50, Max=200\n\n";
    
    // 场景 1：大窗口（约束不生效）
    std::cout << "场景 1: 大窗口 (800x600)\n";
    grid->Measure(Size(800, 600));
    
    const auto& rows = grid->GetRowDefinitions();
    std::cout << "  实际高度: " << rows[0].actualHeight << ", " 
              << rows[1].actualHeight << ", " << rows[2].actualHeight << "\n";
    std::cout << "  比例: 1:" << (rows[1].actualHeight / rows[0].actualHeight) 
              << ":1 (接近 1:2:1)\n\n";
    
    // 场景 2：小窗口（Min 约束生效）
    std::cout << "场景 2: 小窗口 (800x300)\n";
    grid->Measure(Size(800, 300));
    std::cout << "  实际高度: " << rows[0].actualHeight << ", " 
              << rows[1].actualHeight << ", " << rows[2].actualHeight << "\n";
    std::cout << "  注意: 第二行高度受 MinHeight(100) 保护\n\n";
    
    // 场景 3：超大窗口（Max 约束生效）
    std::cout << "场景 3: 超大窗口 (800x1200)\n";
    grid->Measure(Size(800, 1200));
    std::cout << "  实际高度: " << rows[0].actualHeight << ", " 
              << rows[1].actualHeight << ", " << rows[2].actualHeight << "\n";
    std::cout << "  注意: 各行高度受 MaxHeight 限制\n";
    
    delete grid;
}

void ShowExample4_AutoSizing() {
    std::cout << "\n========== 示例 4: Auto 尺寸计算 ==========\n";
    
    auto grid = new Grid();
    grid->Rows("Auto, Auto, *")  // 两个 Auto 行，一个 Star 行
        ->Columns("*");
    
    std::cout << "定义 3 行：Auto, Auto, * (Star 填充剩余空间)\n\n";
    
    // 第一行：小按钮
    auto btn1 = grid->AddChild<Button>()
        ->Row(0)
        ->Content("小按钮")
        ->Height(40);
    
    std::cout << "行 0: 按钮高度 40px\n";
    
    // 第二行：大按钮
    auto btn2 = grid->AddChild<Button>()
        ->Row(1)
        ->Content("大按钮")
        ->Height(80);
    
    std::cout << "行 1: 按钮高度 80px\n";
    
    // 第三行：自适应
    auto btn3 = grid->AddChild<Button>()
        ->Row(2)
        ->Content("填充剩余空间");
    
    std::cout << "行 2: Star (填充剩余)\n\n";
    
    // 测量
    grid->Measure(Size(400, 300));
    
    const auto& rows = grid->GetRowDefinitions();
    std::cout << "测量结果 (总高度 300px):\n";
    std::cout << "  行 0 (Auto): " << rows[0].actualHeight << "px (应为 ~40)\n";
    std::cout << "  行 1 (Auto): " << rows[1].actualHeight << "px (应为 ~80)\n";
    std::cout << "  行 2 (Star): " << rows[2].actualHeight << "px (应为 ~180)\n";
    
    delete grid;
}

void ShowExample5_AlignmentAndMargin() {
    std::cout << "\n========== 示例 5: 对齐和边距 ==========\n";
    
    auto grid = new Grid();
    grid->Rows("*, *, *")->Columns("*, *, *");
    
    std::cout << "创建 3x3 网格，演示不同对齐方式：\n\n";
    
    // 左上对齐
    auto btn1 = grid->AddChild<Button>()
        ->Row(0)->Column(0)
        ->Content("左上")
        ->Width(60)->Height(30)
        ->HorizontalAlignment(HorizontalAlignment::Left)
        ->VerticalAlignment(VerticalAlignment::Top)
        ->Margin(Thickness(5, 5, 5, 5));
    
    std::cout << "单元格 (0,0): 左上对齐，边距 5px\n";
    
    // 居中对齐
    auto btn2 = grid->AddChild<Button>()
        ->Row(0)->Column(1)
        ->Content("居中")
        ->Width(60)->Height(30)
        ->HorizontalAlignment(HorizontalAlignment::Center)
        ->VerticalAlignment(VerticalAlignment::Center);
    
    std::cout << "单元格 (0,1): 居中对齐\n";
    
    // 右下对齐
    auto btn3 = grid->AddChild<Button>()
        ->Row(0)->Column(2)
        ->Content("右下")
        ->Width(60)->Height(30)
        ->HorizontalAlignment(HorizontalAlignment::Right)
        ->VerticalAlignment(VerticalAlignment::Bottom)
        ->Margin(Thickness(5, 5, 5, 5));
    
    std::cout << "单元格 (0,2): 右下对齐，边距 5px\n";
    
    // 拉伸填充
    auto btn4 = grid->AddChild<Button>()
        ->Row(1)->Column(1)
        ->Content("拉伸填充")
        ->HorizontalAlignment(HorizontalAlignment::Stretch)
        ->VerticalAlignment(VerticalAlignment::Stretch)
        ->Margin(Thickness(10, 10, 10, 10));
    
    std::cout << "单元格 (1,1): 拉伸填充，边距 10px\n\n";
    
    // 测量和排列
    grid->Measure(Size(300, 300));
    grid->Arrange(Rect(0, 0, 300, 300));
    
    std::cout << "布局结果 (每个单元格 100x100):\n";
    
    auto rect1 = btn1->GetLayoutRect();
    std::cout << "  左上按钮: (" << rect1.x << ", " << rect1.y << ") "
              << rect1.width << "x" << rect1.height << "\n";
    
    auto rect2 = btn2->GetLayoutRect();
    std::cout << "  居中按钮: (" << rect2.x << ", " << rect2.y << ") "
              << rect2.width << "x" << rect2.height << "\n";
    
    auto rect4 = btn4->GetLayoutRect();
    std::cout << "  拉伸按钮: (" << rect4.x << ", " << rect4.y << ") "
              << rect4.width << "x" << rect4.height 
              << " (应为 80x80，因为边距 10px)\n";
    
    delete grid;
}

void ShowExample6_ComplexLayout() {
    std::cout << "\n========== 示例 6: 复杂表单布局 ==========\n";
    
    auto formGrid = new Grid();
    
    // 使用字符串解析快速定义
    formGrid->Rows("Auto, Auto, Auto, Auto, *")  // 4 行输入 + 按钮区
            ->Columns("Auto, *");                 // 标签列 + 输入列
    
    std::cout << "创建表单布局：\n";
    std::cout << "  行：4 个 Auto (标签+输入) + 1 个 Star (按钮区)\n";
    std::cout << "  列：Auto (标签) + Star (输入)\n\n";
    
    // 标签和输入框（重复模式）
    const char* labels[] = {"用户名:", "密码:", "邮箱:", "电话:"};
    
    for (int i = 0; i < 4; i++) {
        // 标签
        auto label = formGrid->AddChild<TextBlock>()
            ->Row(i)->Column(0)
            ->Text(labels[i])
            ->VerticalAlignment(VerticalAlignment::Center)
            ->Margin(Thickness(0, 0, 10, 5));
        
        // 输入框
        auto input = formGrid->AddChild<Border>()  // 用 Border 模拟 TextBox
            ->Row(i)->Column(1)
            ->Height(30)
            ->Background(0xFFFFFFFF)
            ->Margin(Thickness(0, 0, 0, 5));
        
        std::cout << "  行 " << i << ": \"" << labels[i] << "\" + 输入框\n";
    }
    
    // 按钮区域（跨两列）
    auto buttonPanel = formGrid->AddChild<StackPanel>()
        ->Row(4)->Column(0)->ColumnSpan(2)
        ->Orientation(Orientation::Horizontal)
        ->HorizontalAlignment(HorizontalAlignment::Right)
        ->Margin(Thickness(0, 10, 0, 0));
    
    auto cancelBtn = buttonPanel->AddChild<Button>()
        ->Content("取消")
        ->Width(80)->Height(32)
        ->Margin(Thickness(0, 0, 10, 0));
    
    auto submitBtn = buttonPanel->AddChild<Button>()
        ->Content("提交")
        ->Width(80)->Height(32);
    
    std::cout << "\n  行 4: 按钮区（取消 + 提交，右对齐）\n";
    
    // 测量
    formGrid->Measure(Size(400, 400));
    formGrid->Arrange(Rect(0, 0, 400, 400));
    
    std::cout << "\n布局完成！总尺寸: " 
              << formGrid->GetDesiredSize().width << " x " 
              << formGrid->GetDesiredSize().height << "\n";
    
    delete formGrid;
}

int main() {
    std::cout << "\n";
    std::cout << "╔═══════════════════════════════════════════════════════╗\n";
    std::cout << "║          Grid 功能展示示例程序                         ║\n";
    std::cout << "║                                                       ║\n";
    std::cout << "║  演示所有新增功能：                                    ║\n";
    std::cout << "║  • 字符串解析 (\"Auto, *, 100\")                       ║\n";
    std::cout << "║  • 流式附加属性 (->Row(0)->Column(1))                 ║\n";
    std::cout << "║  • Min/Max 约束                                       ║\n";
    std::cout << "║  • Auto 尺寸计算                                      ║\n";
    std::cout << "║  • 对齐和边距                                         ║\n";
    std::cout << "║  • 复杂布局场景                                       ║\n";
    std::cout << "╚═══════════════════════════════════════════════════════╝\n";
    
    ShowExample1_BasicLayout();
    ShowExample2_FluentSyntax();
    ShowExample3_Constraints();
    ShowExample4_AutoSizing();
    ShowExample5_AlignmentAndMargin();
    ShowExample6_ComplexLayout();
    
    std::cout << "\n";
    std::cout << "╔═══════════════════════════════════════════════════════╗\n";
    std::cout << "║  所有示例运行完成！                                    ║\n";
    std::cout << "║                                                       ║\n";
    std::cout << "║  查看详细文档：                                        ║\n";
    std::cout << "║  - GRID_COMPLETE_IMPLEMENTATION.md                    ║\n";
    std::cout << "║  - GRID_QUICK_REFERENCE.md                            ║\n";
    std::cout << "╚═══════════════════════════════════════════════════════╝\n";
    std::cout << "\n";
    
    return 0;
}
