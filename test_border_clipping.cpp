#include "fk/app/Application.h"
#include "fk/ui/Window.h"
#include "fk/ui/Button.h"
#include "fk/ui/TextBlock.h"
#include "fk/ui/Border.h"
#include "fk/ui/Grid.h"
#include "fk/ui/GridCellAttacher.h"
#include "fk/ui/Brush.h"
#include <iostream>

using namespace fk::ui;

int main() {
    fk::Application app;
    auto mainWindow = std::make_shared<Window>();
    
    auto grid = (new Grid())
        ->Rows("Auto, Auto, Auto, Auto")
        ->Columns("*")
        ->Margin(fk::Thickness(20));
    
    // 测试1: 正常宽度的按钮
    auto btn1 = (new Button())
        ->Name("btn1")
        ->Width(300)
        ->Height(50)
        ->Content((new TextBlock())
            ->Text("Normal Width Button - Full Text Visible")
            ->FontSize(16)
            ->Foreground(Brushes::Black()))
        ->Background(Brushes::Blue())
        | cell(0, 0);
    
    // 测试2: 缩短的按钮 - 文字应该被裁剪在按钮边界内
    auto btn2 = (new Button())
        ->Name("btn2")
        ->Width(150)  // 很窄，文字会超出
        ->Height(50)
        ->Content((new TextBlock())
            ->Text("This is a very long text that should be clipped")
            ->FontSize(16)
            ->Foreground(Brushes::Black()))
        ->Background(Brushes::Red())
        ->Margin(fk::Thickness(0, 10, 0, 0))
        | cell(1, 0);
    
    // 测试3: 带边框的Border，内容应该被裁剪
    auto border3 = (new Border())
        ->Width(200)
        ->Height(60)
        ->Background(new SolidColorBrush(255, 200, 100))
        ->BorderBrush(Brushes::Black())
        ->BorderThickness(fk::Thickness(3))
        ->Padding(5)
        ->Child((new TextBlock())
            ->Text("Long text in Border should be clipped at border boundary, not overflow outside")
            ->FontSize(14)
            ->Foreground(Brushes::Black())
            ->TextWrapping(TextWrapping::Wrap))
        ->Margin(fk::Thickness(0, 10, 0, 0))
        | cell(2, 0);
    border3->SetHorizontalAlignment(HorizontalAlignment::Left);
    
    // 测试4: 小尺寸Border with 多行文本
    auto border4 = (new Border())
        ->Width(180)
        ->Height(50)  // 高度不够显示所有行
        ->Background(new SolidColorBrush(200, 255, 200))
        ->BorderBrush(Brushes::Green())
        ->BorderThickness(fk::Thickness(2))
        ->Padding(8)
        ->Child((new TextBlock())
            ->Text("Line 1\nLine 2\nLine 3\nLine 4 (should be clipped)")
            ->FontSize(14)
            ->Foreground(Brushes::Black()))
        ->Margin(fk::Thickness(0, 10, 0, 0))
        | cell(3, 0);
    border4->SetHorizontalAlignment(HorizontalAlignment::Left);
    
    grid->AddChild(btn1);
    grid->AddChild(btn2);
    grid->AddChild(border3);
    grid->AddChild(border4);
    
    mainWindow->Title("Border Clipping Test - 文本不应超出边界")
        ->Width(800)
        ->Height(600)
        ->Background(Brushes::White())
        ->Content(grid);
    
    auto* button1 = static_cast<Button*>(mainWindow->FindName("btn1"));
    auto* button2 = static_cast<Button*>(mainWindow->FindName("btn2"));
    
    button1->Click += []() { 
        std::cout << "Clicked: Normal width button" << std::endl; 
    };
    
    button2->Click += []() { 
        std::cout << "Clicked: Narrow button (text should be clipped)" << std::endl; 
    };
    
    std::cout << "\n=== Border Clipping Test ===" << std::endl;
    std::cout << "Expected behavior:" << std::endl;
    std::cout << "1. 蓝色按钮: 正常宽度，文字完整显示" << std::endl;
    std::cout << "2. 红色按钮: 宽度150px，长文本应该被裁剪在按钮内" << std::endl;
    std::cout << "3. 橙色Border: 文字应该在边框和padding内换行并裁剪" << std::endl;
    std::cout << "4. 绿色Border: 多行文本超出高度的部分应该被裁剪" << std::endl;
    std::cout << "==> 所有文本都不应该绘制到容器外部!" << std::endl;
    std::cout << "==============================\n" << std::endl;
    
    app.Run(mainWindow);
    return 0;
}
