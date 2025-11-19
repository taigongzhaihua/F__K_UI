#include "fk/app/Application.h"
#include "fk/ui/Window.h"
#include "fk/ui/Button.h"
#include "fk/ui/TextBlock.h"
#include "fk/ui/Grid.h"
#include "fk/ui/GridCellAttacher.h"
#include "fk/ui/Border.h"
#include "fk/ui/Brush.h"
#include <iostream>

using namespace fk::ui;

int main() {
    fk::Application app;
    auto mainWindow = std::make_shared<Window>();
    
    // 创建带Star行列的Grid
    auto grid = (new Grid())
        ->Rows("Auto, *, 50")
        ->Columns("200, *");
    
    // 顶部标题（用Border包装以支持Background）
    auto header = (new Border())
        ->Background(Brushes::Blue())
        ->Child((new TextBlock())
            ->Text("Header (Auto Row)")
            ->FontSize(20)
            ->Foreground(Brushes::White()))
        | cell(0, 0, 1, 2);
    
    // 左侧栏（固定200px）
    auto sidebar = (new Border())
        ->Background(new SolidColorBrush(220, 220, 220))
        ->Child((new TextBlock())
            ->Text("Sidebar\n(200px)")
            ->FontSize(16)
            ->Foreground(Brushes::Black()))
        | cell(1, 0);
    
    // 主内容区（Star列，应该随窗口拉伸）
    auto contentBtn = new Button();
    contentBtn->Name("mainContent")
        ->Content((new TextBlock())
            ->Text("Main Content (Star Column)\nShould resize with window")
            ->FontSize(16)
            ->Foreground(Brushes::White()))
        ->Background(Brushes::Green());
    contentBtn->SetHorizontalAlignment(HorizontalAlignment::Stretch);
    contentBtn->SetVerticalAlignment(VerticalAlignment::Stretch);
    auto content = contentBtn | cell(1, 1);
    
    // 底部状态栏（固定50px高）
    auto footer = (new Border())
        ->Background(new SolidColorBrush(60, 60, 60))
        ->Child((new TextBlock())
            ->Text("Footer (50px)")
            ->FontSize(14)
            ->Foreground(Brushes::White()))
        | cell(2, 0, 1, 2);
    
    grid->AddChild(header);
    grid->AddChild(sidebar);
    grid->AddChild(content);
    grid->AddChild(footer);
    
    mainWindow->Title("Grid Resize Test - Try resizing the window!")
        ->Width(800)
        ->Height(600)
        ->Background(Brushes::White())
        ->Content(grid);
    
    auto* btn = static_cast<Button*>(mainWindow->FindName("mainContent"));
    
    int clickCount = 0;
    btn->Click += [btn, &clickCount]() {
        clickCount++;
        auto size = btn->GetRenderSize();
        auto rect = btn->GetLayoutRect();
        std::cout << "\n=== Click #" << clickCount << " ===" << std::endl;
        std::cout << "Button RenderSize: " << size.width << " x " << size.height << std::endl;
        std::cout << "Button LayoutRect: (" << rect.x << ", " << rect.y << ") "
                  << rect.width << " x " << rect.height << std::endl;
        std::cout << "Expected: Should grow when window is resized" << std::endl;
        std::cout << "==================\n" << std::endl;
    };
    
    std::cout << "\n=== Grid Resize Test ===" << std::endl;
    std::cout << "Instructions:" << std::endl;
    std::cout << "1. The green button should fill the star column" << std::endl;
    std::cout << "2. Resize the window - the button should grow/shrink" << std::endl;
    std::cout << "3. Click the button to see current size" << std::endl;
    std::cout << "========================\n" << std::endl;
    
    app.Run(mainWindow);
    return 0;
}
