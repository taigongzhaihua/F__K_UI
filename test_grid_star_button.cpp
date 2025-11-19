#include "fk/app/Application.h"
#include "fk/ui/Window.h"
#include "fk/ui/Button.h"
#include "fk/ui/TextBlock.h"
#include "fk/ui/Grid.h"
#include "fk/ui/GridCellAttacher.h"
#include "fk/ui/Brush.h"
#include <iostream>

using namespace fk::ui;

int main() {
    fk::Application app;
    auto mainWindow = std::make_shared<Window>();
    
    // 创建简单的 2x2 Grid
    auto grid = (new Grid())
        ->Rows("100, *")  // 第一行固定 100，第二行自动填充
        ->Columns("*, *");
    
    // 在第一行添加一个文本（Auto 行）
    grid->Children({
        (new TextBlock())
            ->Text("Row 0 (100px)")
            ->FontSize(16)
            ->Foreground(Brushes::Black())
            | cell(0, 0),
        
        // 在第二行（Star行）添加 Button
        (new Button())
            ->Name("testBtn")
            ->Width(200)
            ->Height(60)
            ->Background(Brushes::Blue())
            ->Content(
                (new TextBlock())
                    ->Text("Button in Star Row")
                    ->FontSize(16)
                    ->Foreground(Brushes::White())
            )
            | cell(1, 0)
    });
    
    mainWindow->Title("Grid Star Row Button Test")
        ->Width(600)
        ->Height(400)
        ->Background(Brushes::White())
        ->Content(grid);
    
    auto* btn = static_cast<Button*>(mainWindow->FindName("testBtn"));
    
    btn->Click += [btn]() {
        auto size = btn->GetRenderSize();
        auto rect = btn->GetLayoutRect();
        std::cout << "\n=== Button Click Info ===" << std::endl;
        std::cout << "RenderSize: " << size.width << " x " << size.height << std::endl;
        std::cout << "LayoutRect: (" << rect.x << ", " << rect.y << ") "
                  << rect.width << " x " << rect.height << std::endl;
        std::cout << "========================\n" << std::endl;
    };
    
    std::cout << "\n=== Before Run ===" << std::endl;
    std::cout << "Button RenderSize: " << btn->GetRenderSize().width << " x " 
              << btn->GetRenderSize().height << std::endl;
    std::cout << "Button DesiredSize: " << btn->GetDesiredSize().width << " x " 
              << btn->GetDesiredSize().height << std::endl;
    std::cout << "==================\n" << std::endl;
    
    app.Run(mainWindow);
    return 0;
}
