/**
 * @file demo_panel_backgrounds.cpp
 * @brief Panel背景和圆角功能演示
 * 
 * 演示内容：
 * 1. StackPanel 带背景和统一圆角
 * 2. StackPanel 带背景和不同圆角
 * 3. Grid 带背景和递增圆角
 * 4. Border 带背景和独立圆角
 */

#include "fk/app/Application.h"
#include "fk/ui/Window.h"
#include "fk/ui/StackPanel.h"
#include "fk/ui/Grid.h"
#include "fk/ui/Border.h"
#include "fk/ui/TextBlock.h"
#include "fk/ui/Brush.h"
#include <iostream>
#include <memory>

using namespace fk;
using namespace fk::ui;

int main() {
    try {
        // 创建应用程序
        auto app = Application::Current();
        if (!app) {
            std::cerr << "无法获取应用程序实例" << std::endl;
            return 1;
        }
        
        // 创建主窗口（使用 shared_ptr 以便传入 Run）
        auto window = std::make_shared<Window>();
        window->SetTitle("Panel背景和圆角演示");
        window->SetWidth(900);
        window->SetHeight(700);
        app->AddWindow(window, "PanelBackgroundDemo");
        
        // 创建主网格布局（3行2列）
        auto mainGrid = new Grid();
        mainGrid->Rows("*, *, *");
        mainGrid->Columns("*, *");
        mainGrid->Background(new SolidColorBrush(Color{0.95f, 0.95f, 0.95f, 1.0f}));
        
        // ========== 示例1：StackPanel 统一圆角 ==========
        auto stackPanel1 = new StackPanel();
        stackPanel1->SetOrientation(Orientation::Vertical);
        stackPanel1->SetSpacing(10);
        stackPanel1->SetMargin(Thickness(15));
        stackPanel1->Background(new SolidColorBrush(Color{0.2f, 0.6f, 0.9f, 1.0f})); // 蓝色
        stackPanel1->CornerRadius(20.0f); // 统一圆角
        
        auto titleText1 = new TextBlock();
        titleText1->SetText("StackPanel");
        titleText1->SetForeground(new SolidColorBrush(Color{1.0f, 1.0f, 1.0f, 1.0f}));
        titleText1->SetFontSize(24);
        titleText1->SetMargin(Thickness(15, 15, 15, 5));
        
        auto descText1 = new TextBlock();
        descText1->SetText("统一圆角: 20px");
        descText1->SetForeground(new SolidColorBrush(Color{1.0f, 1.0f, 1.0f, 0.9f}));
        descText1->SetFontSize(16);
        descText1->SetMargin(Thickness(15, 5, 15, 15));
        
        stackPanel1->AddChild(titleText1);
        stackPanel1->AddChild(descText1);
        Grid::SetRow(stackPanel1, 0);
        Grid::SetColumn(stackPanel1, 0);
        
        // ========== 示例2：StackPanel 波浪圆角 ==========
        auto stackPanel2 = new StackPanel();
        stackPanel2->SetOrientation(Orientation::Vertical);
        stackPanel2->SetSpacing(10);
        stackPanel2->SetMargin(Thickness(15));
        stackPanel2->Background(new SolidColorBrush(Color{0.9f, 0.3f, 0.3f, 1.0f})); // 红色
        stackPanel2->CornerRadius(40.0f, 10.0f, 40.0f, 10.0f); // 波浪圆角
        
        auto titleText2 = new TextBlock();
        titleText2->SetText("StackPanel");
        titleText2->SetForeground(new SolidColorBrush(Color{1.0f, 1.0f, 1.0f, 1.0f}));
        titleText2->SetFontSize(24);
        titleText2->SetMargin(Thickness(15, 15, 15, 5));
        
        auto descText2 = new TextBlock();
        descText2->SetText("波浪圆角: (40, 10, 40, 10)");
        descText2->SetForeground(new SolidColorBrush(Color{1.0f, 1.0f, 1.0f, 0.9f}));
        descText2->SetFontSize(16);
        descText2->SetMargin(Thickness(15, 5, 15, 15));
        
        stackPanel2->AddChild(titleText2);
        stackPanel2->AddChild(descText2);
        Grid::SetRow(stackPanel2, 0);
        Grid::SetColumn(stackPanel2, 1);
        
        // ========== 示例3：Grid 递增圆角 ==========
        auto grid3 = new Grid();
        grid3->Columns("*, *");
        grid3->SetMargin(Thickness(15));
        grid3->Background(new SolidColorBrush(Color{0.3f, 0.8f, 0.4f, 1.0f})); // 绿色
        grid3->CornerRadius(5.0f, 25.0f, 45.0f, 65.0f); // 递增圆角
        
        auto titleText3 = new TextBlock();
        titleText3->SetText("Grid");
        titleText3->SetForeground(new SolidColorBrush(Color{1.0f, 1.0f, 1.0f, 1.0f}));
        titleText3->SetFontSize(24);
        titleText3->SetMargin(Thickness(15, 15, 15, 5));
        Grid::SetColumn(titleText3, 0);
        Grid::SetColumnSpan(titleText3, 2);
        
        auto descText3 = new TextBlock();
        descText3->SetText("递增圆角: (5, 25, 45, 65)");
        descText3->SetForeground(new SolidColorBrush(Color{1.0f, 1.0f, 1.0f, 0.9f}));
        descText3->SetFontSize(16);
        descText3->SetMargin(Thickness(15, 5, 15, 15));
        Grid::SetRow(descText3, 1);
        Grid::SetColumn(descText3, 0);
        Grid::SetColumnSpan(descText3, 2);
        
        grid3->AddChild(titleText3);
        grid3->AddChild(descText3);
        Grid::SetRow(grid3, 1);
        Grid::SetColumn(grid3, 0);
        
        // ========== 示例4：Grid 混合圆角 ==========
        auto grid4 = new Grid();
        grid4->Rows("*, *");
        grid4->SetMargin(Thickness(15));
        grid4->Background(new SolidColorBrush(Color{0.7f, 0.4f, 0.9f, 1.0f})); // 紫色
        grid4->CornerRadius(50.0f, 5.0f, 30.0f, 15.0f); // 混合圆角
        
        auto titleText4 = new TextBlock();
        titleText4->SetText("Grid");
        titleText4->SetForeground(new SolidColorBrush(Color{1.0f, 1.0f, 1.0f, 1.0f}));
        titleText4->SetFontSize(24);
        titleText4->SetMargin(Thickness(15, 15, 15, 5));
        
        auto descText4 = new TextBlock();
        descText4->SetText("混合圆角: (50, 5, 30, 15)");
        descText4->SetForeground(new SolidColorBrush(Color{1.0f, 1.0f, 1.0f, 0.9f}));
        descText4->SetFontSize(16);
        descText4->SetMargin(Thickness(15, 5, 15, 15));
        Grid::SetRow(descText4, 1);
        
        grid4->AddChild(titleText4);
        grid4->AddChild(descText4);
        Grid::SetRow(grid4, 1);
        Grid::SetColumn(grid4, 1);
        
        // ========== 示例5：Border 独立圆角 ==========
        auto border5 = new Border();
        border5->SetMargin(Thickness(15));
        border5->Background(new SolidColorBrush(Color{1.0f, 0.7f, 0.2f, 1.0f})); // 橙色
        border5->BorderBrush(new SolidColorBrush(Color{0.8f, 0.4f, 0.1f, 1.0f}));
        border5->BorderThickness(3.0f);
        border5->CornerRadius(10.0f, 30.0f, 50.0f, 20.0f); // 独立圆角
        
        auto innerStack5 = new StackPanel();
        innerStack5->SetOrientation(Orientation::Vertical);
        
        auto titleText5 = new TextBlock();
        titleText5->SetText("Border");
        titleText5->SetForeground(new SolidColorBrush(Color{1.0f, 1.0f, 1.0f, 1.0f}));
        titleText5->SetFontSize(24);
        titleText5->SetMargin(Thickness(15, 15, 15, 5));
        
        auto descText5 = new TextBlock();
        descText5->SetText("独立圆角: (10, 30, 50, 20)");
        descText5->SetForeground(new SolidColorBrush(Color{1.0f, 1.0f, 1.0f, 0.9f}));
        descText5->SetFontSize(16);
        descText5->SetMargin(Thickness(15, 5, 15, 15));
        
        innerStack5->AddChild(titleText5);
        innerStack5->AddChild(descText5);
        border5->Child(innerStack5);
        Grid::SetRow(border5, 2);
        Grid::SetColumn(border5, 0);
        
        // ========== 示例6：Border 无圆角 ==========
        auto border6 = new Border();
        border6->SetMargin(Thickness(15));
        border6->Background(new SolidColorBrush(Color{0.4f, 0.7f, 0.9f, 1.0f})); // 青色
        border6->BorderBrush(new SolidColorBrush(Color{0.2f, 0.5f, 0.7f, 1.0f}));
        border6->BorderThickness(3.0f);
        border6->CornerRadius(0.0f); // 无圆角
        
        auto innerStack6 = new StackPanel();
        innerStack6->SetOrientation(Orientation::Vertical);
        
        auto titleText6 = new TextBlock();
        titleText6->SetText("Border");
        titleText6->SetForeground(new SolidColorBrush(Color{1.0f, 1.0f, 1.0f, 1.0f}));
        titleText6->SetFontSize(24);
        titleText6->SetMargin(Thickness(15, 15, 15, 5));
        
        auto descText6 = new TextBlock();
        descText6->SetText("无圆角: (0, 0, 0, 0)");
        descText6->SetForeground(new SolidColorBrush(Color{1.0f, 1.0f, 1.0f, 0.9f}));
        descText6->SetFontSize(16);
        descText6->SetMargin(Thickness(15, 5, 15, 15));
        
        innerStack6->AddChild(titleText6);
        innerStack6->AddChild(descText6);
        border6->Child(innerStack6);
        Grid::SetRow(border6, 2);
        Grid::SetColumn(border6, 1);
        
        // 添加所有示例到主网格
        mainGrid->AddChild(stackPanel1);
        mainGrid->AddChild(stackPanel2);
        mainGrid->AddChild(grid3);
        mainGrid->AddChild(grid4);
        mainGrid->AddChild(border5);
        mainGrid->AddChild(border6);
        
        // 设置窗口内容
        window->SetContent(mainGrid);
        
        // 运行应用
        std::cout << "Panel背景和圆角演示运行中..." << std::endl;
        std::cout << "展示了6个不同的示例：" << std::endl;
        std::cout << "1. StackPanel - 统一圆角 (20px)" << std::endl;
        std::cout << "2. StackPanel - 波浪圆角 (40, 10, 40, 10)" << std::endl;
        std::cout << "3. Grid - 递增圆角 (5, 25, 45, 65)" << std::endl;
        std::cout << "4. Grid - 混合圆角 (50, 5, 30, 15)" << std::endl;
        std::cout << "5. Border - 独立圆角 (10, 30, 50, 20)" << std::endl;
        std::cout << "6. Border - 无圆角 (0, 0, 0, 0)" << std::endl;
        
        app->Run(window);
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "异常: " << e.what() << std::endl;
        return 1;
    }
}
