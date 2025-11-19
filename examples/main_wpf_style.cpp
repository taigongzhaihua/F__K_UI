/**
 * @file main_wpf_style.cpp
 * @brief Grid WPF 风格 API 演示 - 使用管道操作符
 * 
 * 展示如何使用 WPF 风格的声明式 API 创建复杂布局
 */

#include "fk/app/Application.h"
#include "fk/ui/Window.h"
#include "fk/ui/TextBlock.h"
#include "fk/ui/Brush.h"
#include "fk/ui/Grid.h"
#include "fk/ui/GridCellAttacher.h"
#include "fk/ui/Border.h"
#include "fk/ui/StackPanel.h"
#include "fk/ui/Button.h"
#include <iostream>

using namespace fk::ui;

int main(int argc, char **argv)
{
    fk::Application app;
    auto mainWindow = std::make_shared<fk::ui::Window>();
    
    // ========== 使用 WPF 风格管道操作符创建 Grid 布局 ==========
    auto mainGrid = new Grid();
    mainGrid->Rows("60, *, 30")      // 工具栏、内容、状态栏
            ->Columns("200, *, 200")  // 左边栏、主区、右边栏
            ->Children({
                // 顶部工具栏（跨 3 列）- 使用 WPF 风格 cell() 函数
                (new Border()
                    ->Background(Brushes::DarkGray())
                    ->Child((new StackPanel())
                        ->SetOrient(Orientation::Horizontal)
                        ->Children({
                            (new TextBlock())
                                ->Text("📁 File")
                                ->FontSize(16)
                                ->Foreground(Brushes::White())
                                ->Margin(fk::Thickness(15, 20, 15, 20)),
                            (new TextBlock())
                                ->Text("✏️ Edit")
                                ->FontSize(16)
                                ->Foreground(Brushes::White())
                                ->Margin(fk::Thickness(15, 20, 15, 20)),
                            (new TextBlock())
                                ->Text("🔧 Tools")
                                ->FontSize(16)
                                ->Foreground(Brushes::White())
                                ->Margin(fk::Thickness(15, 20, 15, 20))
                        }))
                ) | cell(0, 0).ColumnSpan(3),  // ⭐ WPF 风格管道操作符！
                
                // 左侧边栏
                (new Border()
                    ->Background(Brushes::Gray())
                    ->Child((new StackPanel())
                        ->Children({
                            (new TextBlock())
                                ->Text("📂 Explorer")
                                ->FontSize(18)
                                ->Foreground(Brushes::White())
                                ->Margin(fk::Thickness(10, 20, 10, 10)),
                            (new TextBlock())
                                ->Text("📄 File 1.txt")
                                ->FontSize(14)
                                ->Foreground(Brushes::LightGray())
                                ->Margin(fk::Thickness(20, 10, 10, 5)),
                            (new TextBlock())
                                ->Text("📄 File 2.cpp")
                                ->FontSize(14)
                                ->Foreground(Brushes::LightGray())
                                ->Margin(fk::Thickness(20, 5, 10, 5))
                        }))
                ) | cell(1, 0),  // ⭐ 简洁的位置指定
                
                // 中央内容区（嵌套 Grid）
                (new Border()
                    ->Background(Brushes::White())
                    ->Child((new Grid())
                        ->Rows("Auto, *, Auto")
                        ->Columns("*, *")
                        ->Children({
                            // 标题
                            (new TextBlock())
                                ->Text("🎯 WPF-Style Grid Demo")
                                ->FontSize(28)
                                ->Foreground(Brushes::Blue())
                                ->SetHAlign(HorizontalAlignment::Center)
                                ->Margin(fk::Thickness(0, 20, 0, 20))
                                | cell(0, 0).ColumnSpan(2),  // ⭐ 链式设置跨度
                            
                            // 按钮 1
                            (new Button())
                                ->Name("btn1")
                                ->Width(150)->Height(100)
                                ->MouseOverBackground(Color::FromRGB(100, 200, 255, 255))
                                ->PressedBackground(Color::FromRGB(50, 150, 255, 255))
                                ->Margin(fk::Thickness(10))
                                ->Content((new TextBlock())
                                    ->Text("Button 1")
                                    ->FontSize(18)
                                    ->Foreground(Brushes::Black()))
                                | cell(1, 0),  // ⭐ 位置 (1, 0)
                            
                            // 按钮 2
                            (new Button())
                                ->Name("btn2")
                                ->Width(150)->Height(100)
                                ->MouseOverBackground(Color::FromRGB(255, 200, 100, 255))
                                ->PressedBackground(Color::FromRGB(255, 150, 50, 255))
                                ->Margin(fk::Thickness(10))
                                ->Content((new TextBlock())
                                    ->Text("Button 2")
                                    ->FontSize(18)
                                    ->Foreground(Brushes::Black()))
                                | cell(1, 1),  // ⭐ 位置 (1, 1)
                            
                            // 信息文本
                            (new TextBlock())
                                ->Text("✨ WPF-Style API with Pipe Operator!\n"
                                       "element | cell(row, col).ColumnSpan(n)")
                                ->FontSize(14)
                                ->Foreground(Brushes::DarkGray())
                                ->SetHAlign(HorizontalAlignment::Center)
                                ->Margin(fk::Thickness(20))
                                | cell(2, 0).ColumnSpan(2)  // ⭐ 位置并跨列
                        }))
                ) | cell(1, 1),  // ⭐ 中央区域
                
                // 右侧边栏
                (new Border()
                    ->Background(Brushes::Gray())
                    ->Child((new StackPanel())
                        ->Children({
                            (new TextBlock())
                                ->Text("📊 Properties")
                                ->FontSize(18)
                                ->Foreground(Brushes::White())
                                ->Margin(fk::Thickness(10, 20, 10, 10)),
                            (new TextBlock())
                                ->Text("API: WPF-Style")
                                ->FontSize(14)
                                ->Foreground(Brushes::LightGray())
                                ->Margin(fk::Thickness(20, 10, 10, 5)),
                            (new TextBlock())
                                ->Text("Operator: |")
                                ->FontSize(14)
                                ->Foreground(Brushes::LightGray())
                                ->Margin(fk::Thickness(20, 5, 10, 5))
                        }))
                ) | cell(1, 2),  // ⭐ 右边栏
                
                // 底部状态栏（跨 3 列）
                (new Border()
                    ->Background(Brushes::Blue())
                    ->Child((new TextBlock())
                        ->Text("✅ WPF-Style Grid | Pipe Operator Active | Code: 50% Less!")
                        ->FontSize(12)
                        ->Foreground(Brushes::White())
                        ->Margin(fk::Thickness(10, 8, 10, 8)))
                ) | cell(2, 0).ColumnSpan(3)  // ⭐ 底部跨 3 列
            });
    
    // ========== 设置窗口 ==========
    mainWindow->Title("F__K UI - WPF-Style Grid Demo")
        ->Width(1024)
        ->Height(768)
        ->Background(Brushes::White())
        ->Content(mainGrid);
    
    // ========== 绑定按钮事件 ==========
    auto *button1 = static_cast<Button *>(mainWindow->FindName("btn1"));
    button1->Click += []() {
        std::cout << "✨ Button 1: WPF-style API is amazing!" << std::endl;
    };
    
    auto *button2 = static_cast<Button *>(mainWindow->FindName("btn2"));
    button2->Click += []() {
        std::cout << "🚀 Button 2: Pipe operator rocks!" << std::endl;
    };
    
    std::cout << "\n";
    std::cout << "╔═══════════════════════════════════════════════════════╗\n";
    std::cout << "║   F__K_UI WPF-Style Grid Demo                         ║\n";
    std::cout << "║   --------------------------------------------------- ║\n";
    std::cout << "║   API Style: WPF with Pipe Operator                   ║\n";
    std::cout << "║   Syntax: element | cell(row, col).ColumnSpan(n)     ║\n";
    std::cout << "║                                                       ║\n";
    std::cout << "║   Benefits:                                           ║\n";
    std::cout << "║   ✓ 50% less code vs traditional API                 ║\n";
    std::cout << "║   ✓ Clear visual structure                            ║\n";
    std::cout << "║   ✓ Matches WPF XAML syntax                           ║\n";
    std::cout << "║   ✓ Type-safe and elegant                             ║\n";
    std::cout << "╚═══════════════════════════════════════════════════════╝\n";
    std::cout << "\n";
    
    app.Run(mainWindow);
    return 0;
}
