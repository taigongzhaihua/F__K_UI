#include "fk/app/Application.h"
#include "fk/ui/Window.h"
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
        ->Rows("Auto, Auto, Auto")
        ->Columns("*");
    
    // 测试1: 限制宽度的TextBlock (应该裁剪超出部分)
    auto border1 = (new Border())
        ->Background(new SolidColorBrush(255, 200, 200))
        ->BorderBrush(Brushes::Red())
        ->BorderThickness(fk::Thickness(2))
        ->Width(200)
        ->Height(50)
        ->Child((new TextBlock())
            ->Text("这是一段很长很长很长很长很长很长很长的文本,应该被裁剪")
            ->FontSize(16)
            ->Foreground(Brushes::Black())
            ->TextWrapping(TextWrapping::NoWrap))
        | cell(0, 0);
    
    // 测试2: 限制高度的TextBlock (多行文本应该被裁剪)
    auto border2 = (new Border())
        ->Background(new SolidColorBrush(200, 255, 200))
        ->BorderBrush(Brushes::Green())
        ->BorderThickness(fk::Thickness(2))
        ->Width(300)
        ->Height(60)
        ->Child((new TextBlock())
            ->Text("第一行\n第二行\n第三行\n第四行\n第五行(应该被裁剪)")
            ->FontSize(16)
            ->Foreground(Brushes::Black())
            ->TextWrapping(TextWrapping::NoWrap))
        ->Margin(fk::Thickness(0, 20, 0, 0))
        | cell(1, 0);
    
    // 测试3: 自动换行但限制高度
    auto border3 = (new Border())
        ->Background(new SolidColorBrush(200, 200, 255))
        ->BorderBrush(Brushes::Blue())
        ->BorderThickness(fk::Thickness(2))
        ->Width(250)
        ->Height(80)
        ->Child((new TextBlock())
            ->Text("这是一段会自动换行的长文本。它应该在边界内换行,但如果超出高度限制,超出的部分应该被裁剪掉,不应该绘制到边框外面。")
            ->FontSize(14)
            ->Foreground(Brushes::Black())
            ->TextWrapping(TextWrapping::Wrap))
        ->Margin(fk::Thickness(0, 20, 0, 0))
        | cell(2, 0);
    
    grid->AddChild(border1);
    grid->AddChild(border2);
    grid->AddChild(border3);
    
    mainWindow->Title("TextBlock Clipping Test - 文本不应超出红色边框")
        ->Width(800)
        ->Height(600)
        ->Background(Brushes::White())
        ->Content(grid);
    
    std::cout << "\n=== TextBlock Clipping Test ===" << std::endl;
    std::cout << "Instructions:" << std::endl;
    std::cout << "1. 红框: 长文本应该被裁剪在200px宽度内" << std::endl;
    std::cout << "2. 绿框: 多行文本应该被裁剪在60px高度内" << std::endl;
    std::cout << "3. 蓝框: 换行文本超出80px高度的部分应该被裁剪" << std::endl;
    std::cout << "4. 所有文本都不应该绘制到边框外" << std::endl;
    std::cout << "============================\n" << std::endl;
    
    app.Run(mainWindow);
    return 0;
}
