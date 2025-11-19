/**
 * @file example_grid_wpf_style.cpp
 * @brief Grid WPF/WinUI 风格 API 演示
 * 
 * 展示管道操作符风格的声明式 API：
 * grid->Children({
 *     new Button() | cell(0, 0),
 *     new Button() | cell(0, 1).RowSpan(2)
 * });
 */

#include "fk/ui/Grid.h"
#include "fk/ui/Button.h"
#include "fk/ui/TextBlock.h"
#include "fk/ui/Border.h"
#include "fk/ui/StackPanel.h"
#include <iostream>

using namespace fk::ui;

void ShowExample1_BasicUsage() {
    std::cout << "\n========== 示例 1: 基本用法（WPF 风格）==========\n";
    
    auto grid = new Grid();
    grid->Rows("Auto, *, Auto")
        ->Columns("*, *, *");
    
    // 使用管道操作符 | 和 cell() 函数
    grid->Children({
        new Button()->Content("按钮 (0,0)") | cell(0, 0),
        new Button()->Content("按钮 (0,1)") | cell(0, 1),
        new Button()->Content("按钮 (0,2)") | cell(0, 2),
        new Button()->Content("按钮 (1,0)") | cell(1, 0),
        new Button()->Content("按钮 (1,1)") | cell(1, 1),
        new Button()->Content("按钮 (1,2)") | cell(1, 2)
    });
    
    std::cout << "✓ 使用声明式 API 添加了 6 个按钮\n";
    std::cout << "  语法: new Button() | cell(row, col)\n";
    
    // 验证位置
    const auto& children = grid->GetChildren();
    std::cout << "\n验证子元素位置:\n";
    for (size_t i = 0; i < children.size(); i++) {
        int row = Grid::GetRow(children[i]);
        int col = Grid::GetColumn(children[i]);
        std::cout << "  子元素 " << i << ": Grid.Row=" << row << ", Grid.Column=" << col << "\n";
    }
    
    delete grid;
}

void ShowExample2_WithSpan() {
    std::cout << "\n========== 示例 2: 跨行列（WPF 风格）==========\n";
    
    auto grid = new Grid();
    grid->Rows("100, 100, 100")
        ->Columns("100, 100, 100");
    
    // 使用 RowSpan 和 ColumnSpan
    grid->Children({
        new Button()->Content("跨 2 列") | cell(0, 0).ColumnSpan(2),
        new Button()->Content("跨 2 行") | cell(1, 2).RowSpan(2),
        new Button()->Content("跨 2x2") | cell(1, 0).RowSpan(2).ColumnSpan(2),
        new Button()->Content("普通") | cell(0, 2)
    });
    
    std::cout << "✓ 创建了跨行列布局\n";
    
    const auto& children = grid->GetChildren();
    std::cout << "\n子元素跨度信息:\n";
    for (size_t i = 0; i < children.size(); i++) {
        int row = Grid::GetRow(children[i]);
        int col = Grid::GetColumn(children[i]);
        int rowSpan = Grid::GetRowSpan(children[i]);
        int colSpan = Grid::GetColumnSpan(children[i]);
        std::cout << "  子元素 " << i << ": (" << row << "," << col << ") "
                  << "RowSpan=" << rowSpan << ", ColumnSpan=" << colSpan << "\n";
    }
    
    delete grid;
}

void ShowExample3_ComplexLayout() {
    std::cout << "\n========== 示例 3: 复杂布局（类似 WPF XAML）==========\n";
    
    auto mainGrid = new Grid();
    mainGrid->Rows("60, *, 30")
           ->Columns("200, *, 200");
    
    std::cout << "创建类似 IDE 的布局结构:\n\n";
    
    // 工具栏（跨 3 列）
    auto toolbar = new Border()->Background(0xFF2B2B2B);
    auto toolbarText = new TextBlock()->Text("工具栏");
    toolbar->Child(toolbarText);
    
    // 左侧边栏
    auto leftSidebar = new Border()->Background(0xFF1E1E1E);
    auto leftText = new TextBlock()->Text("资源管理器");
    leftSidebar->Child(leftText);
    
    // 主编辑区
    auto editor = new Border()->Background(0xFF252526);
    auto editorText = new TextBlock()->Text("编辑器");
    editor->Child(editorText);
    
    // 右侧边栏
    auto rightSidebar = new Border()->Background(0xFF1E1E1E);
    auto rightText = new TextBlock()->Text("大纲");
    rightSidebar->Child(rightText);
    
    // 状态栏（跨 3 列）
    auto statusBar = new Border()->Background(0xFF007ACC);
    auto statusText = new TextBlock()->Text("就绪");
    statusBar->Child(statusText);
    
    // 使用声明式 API 组装布局
    mainGrid->Children({
        toolbar      | cell(0, 0).ColumnSpan(3),
        leftSidebar  | cell(1, 0),
        editor       | cell(1, 1),
        rightSidebar | cell(1, 2),
        statusBar    | cell(2, 0).ColumnSpan(3)
    });
    
    std::cout << "✓ 工具栏: 行 0, 列 0-2 (跨 3 列)\n";
    std::cout << "✓ 左侧边栏: 行 1, 列 0\n";
    std::cout << "✓ 主编辑区: 行 1, 列 1\n";
    std::cout << "✓ 右侧边栏: 行 1, 列 2\n";
    std::cout << "✓ 状态栏: 行 2, 列 0-2 (跨 3 列)\n";
    
    // 测量和排列
    mainGrid->Measure(Size(1024, 768));
    mainGrid->Arrange(Rect(0, 0, 1024, 768));
    
    std::cout << "\n布局完成！总尺寸: " 
              << mainGrid->GetDesiredSize().width << " x " 
              << mainGrid->GetDesiredSize().height << "\n";
    
    delete mainGrid;
}

void ShowExample4_FormLayout() {
    std::cout << "\n========== 示例 4: 表单布局（WPF 风格）==========\n";
    
    auto formGrid = new Grid();
    formGrid->Rows("Auto, Auto, Auto, Auto, *")
            ->Columns("Auto, *");
    
    std::cout << "创建表单布局（标签 + 输入框）:\n\n";
    
    // 使用声明式 API 创建表单
    formGrid->Children({
        new TextBlock()->Text("用户名:") | cell(0, 0),
        new Border()->Background(0xFFFFFFFF)->Height(30) | cell(0, 1),
        
        new TextBlock()->Text("密码:") | cell(1, 0),
        new Border()->Background(0xFFFFFFFF)->Height(30) | cell(1, 1),
        
        new TextBlock()->Text("邮箱:") | cell(2, 0),
        new Border()->Background(0xFFFFFFFF)->Height(30) | cell(2, 1),
        
        new TextBlock()->Text("电话:") | cell(3, 0),
        new Border()->Background(0xFFFFFFFF)->Height(30) | cell(3, 1),
        
        // 按钮区域（跨 2 列）
        (new StackPanel()
            ->Orientation(Orientation::Horizontal)
            ->HorizontalAlignment(HorizontalAlignment::Right)
            ->Children({
                new Button()->Content("取消")->Width(80)->Height(32),
                new Button()->Content("提交")->Width(80)->Height(32)
            })
        ) | cell(4, 0).ColumnSpan(2)
    });
    
    std::cout << "✓ 4 行标签 + 输入框\n";
    std::cout << "✓ 1 行按钮区（跨 2 列，右对齐）\n";
    
    delete formGrid;
}

void ShowExample5_MixedAPI() {
    std::cout << "\n========== 示例 5: 混合 API 风格 ==========\n";
    
    auto grid = new Grid();
    grid->Rows("*, *")->Columns("*, *");
    
    std::cout << "展示新旧 API 可以混合使用:\n\n";
    
    // 方式 1: 传统的 AddChild + SetRow/SetColumn
    auto btn1 = new Button()->Content("传统方式 1");
    Grid::SetRow(btn1, 0);
    Grid::SetColumn(btn1, 0);
    grid->AddChild(btn1);
    std::cout << "✓ 方式 1: grid->AddChild(btn); Grid::SetRow(btn, 0);\n";
    
    // 方式 2: 流式语法 (之前实现的)
    auto btn2 = grid->AddChild(new Button()->Content("流式语法 2"))
        ->Row(0)->Column(1);
    std::cout << "✓ 方式 2: grid->AddChild(btn)->Row(0)->Column(1);\n";
    
    // 方式 3: 管道操作符（新的 WPF 风格）
    grid->Children({
        new Button()->Content("WPF 风格 3") | cell(1, 0),
        new Button()->Content("WPF 风格 4") | cell(1, 1)
    });
    std::cout << "✓ 方式 3: grid->Children({ btn | cell(1, 0) });\n";
    
    std::cout << "\n所有三种方式都可以正常工作！\n";
    
    delete grid;
}

void ShowExample6_ComparingWithXAML() {
    std::cout << "\n========== 示例 6: 与 WPF XAML 对比 ==========\n";
    
    std::cout << "WPF XAML 代码:\n";
    std::cout << "----------------------------------------\n";
    std::cout << "<Grid>\n";
    std::cout << "  <Grid.RowDefinitions>\n";
    std::cout << "    <RowDefinition Height=\"Auto\"/>\n";
    std::cout << "    <RowDefinition Height=\"*\"/>\n";
    std::cout << "  </Grid.RowDefinitions>\n";
    std::cout << "  <Grid.ColumnDefinitions>\n";
    std::cout << "    <ColumnDefinition Width=\"*\"/>\n";
    std::cout << "    <ColumnDefinition Width=\"*\"/>\n";
    std::cout << "  </Grid.ColumnDefinitions>\n";
    std::cout << "  \n";
    std::cout << "  <Button Grid.Row=\"0\" Grid.Column=\"0\">A</Button>\n";
    std::cout << "  <Button Grid.Row=\"0\" Grid.Column=\"1\" \n";
    std::cout << "          Grid.RowSpan=\"2\">B (跨 2 行)</Button>\n";
    std::cout << "  <Button Grid.Row=\"1\" Grid.Column=\"0\">C</Button>\n";
    std::cout << "</Grid>\n";
    
    std::cout << "\n等价的 C++ 代码:\n";
    std::cout << "----------------------------------------\n";
    std::cout << "auto grid = new Grid();\n";
    std::cout << "grid->Rows(\"Auto, *\")\n";
    std::cout << "    ->Columns(\"*, *\")\n";
    std::cout << "    ->Children({\n";
    std::cout << "        new Button()->Content(\"A\") | cell(0, 0),\n";
    std::cout << "        new Button()->Content(\"B (跨 2 行)\") | cell(0, 1).RowSpan(2),\n";
    std::cout << "        new Button()->Content(\"C\") | cell(1, 0)\n";
    std::cout << "    });\n";
    
    std::cout << "\n✨ 注意相似性:\n";
    std::cout << "  - Grid.Row=\"0\" → cell(0, 0)\n";
    std::cout << "  - Grid.RowSpan=\"2\" → cell(0, 1).RowSpan(2)\n";
    std::cout << "  - 链式调用保持代码结构清晰\n";
    
    // 实际创建该布局
    auto grid = new Grid();
    grid->Rows("Auto, *")
        ->Columns("*, *")
        ->Children({
            new Button()->Content("A") | cell(0, 0),
            new Button()->Content("B (跨 2 行)") | cell(0, 1).RowSpan(2),
            new Button()->Content("C") | cell(1, 0)
        });
    
    std::cout << "\n✓ 布局创建成功！\n";
    
    delete grid;
}

int main() {
    std::cout << "\n";
    std::cout << "╔═══════════════════════════════════════════════════════╗\n";
    std::cout << "║        Grid WPF/WinUI 风格 API 演示程序              ║\n";
    std::cout << "║                                                       ║\n";
    std::cout << "║  新的声明式 API:                                      ║\n";
    std::cout << "║  grid->Children({                                    ║\n";
    std::cout << "║      new Button() | cell(row, col),                  ║\n";
    std::cout << "║      new Button() | cell(row, col).RowSpan(2)        ║\n";
    std::cout << "║  });                                                  ║\n";
    std::cout << "╚═══════════════════════════════════════════════════════╝\n";
    
    ShowExample1_BasicUsage();
    ShowExample2_WithSpan();
    ShowExample3_ComplexLayout();
    ShowExample4_FormLayout();
    ShowExample5_MixedAPI();
    ShowExample6_ComparingWithXAML();
    
    std::cout << "\n";
    std::cout << "╔═══════════════════════════════════════════════════════╗\n";
    std::cout << "║  所有示例运行完成！                                    ║\n";
    std::cout << "║                                                       ║\n";
    std::cout << "║  API 优势:                                            ║\n";
    std::cout << "║  • 与 WPF/WinUI XAML 风格一致                         ║\n";
    std::cout << "║  • 声明式、可读性强                                   ║\n";
    std::cout << "║  • 支持管道操作符（operator|）                        ║\n";
    std::cout << "║  • 向后兼容旧 API                                     ║\n";
    std::cout << "╚═══════════════════════════════════════════════════════╝\n";
    std::cout << "\n";
    
    return 0;
}
