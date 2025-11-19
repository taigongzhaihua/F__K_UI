#include "fk/app/Application.h"
#include "fk/ui/Window.h"
#include "fk/ui/TextBlock.h"
#include "fk/ui/Brush.h"
#include "fk/ui/Grid.h"
#include "fk/ui/GridCellAttacher.h"
#include "fk/core/Logger.h"

#include <iostream>

int main(int argc, char **argv)
{
    fk::Application app;
    auto mainWindow = std::make_shared<fk::ui::Window>();
    mainWindow->Title("Text Measurement Test")
        ->Width(800)
        ->Height(600)
        ->Background(new fk::ui::SolidColorBrush(240, 240, 240))
        ->Content((new fk::ui::Grid())
            ->Rows("Auto, Auto, Auto, Auto, Auto")
            ->Margin(fk::Thickness(20))
            ->Children({
                // Test 1: Wide characters (W, M)
                (fk::ui::UIElement*)((new fk::ui::TextBlock())
                    ->Name("test1")
                    ->Text("WWWWWWWWWW")
                    ->FontSize(24)
                    ->FontFamily("Microsoft YaHei")
                    ->Foreground(new fk::ui::SolidColorBrush(255, 0, 0))
                    ->Margin(fk::Thickness(0, 10, 0, 10))) | fk::ui::cell(0, 0),
                
                // Test 2: Narrow characters (i, l)
                (fk::ui::UIElement*)((new fk::ui::TextBlock())
                    ->Name("test2")
                    ->Text("iiiiiiiiii")
                    ->FontSize(24)
                    ->FontFamily("Microsoft YaHei")
                    ->Foreground(new fk::ui::SolidColorBrush(0, 128, 0))
                    ->Margin(fk::Thickness(0, 10, 0, 10))) | fk::ui::cell(1, 0),
                
                // Test 3: Mixed characters
                (fk::ui::UIElement*)((new fk::ui::TextBlock())
                    ->Name("test3")
                    ->Text("WiWiWiWiWi")
                    ->FontSize(24)
                    ->FontFamily("Microsoft YaHei")
                    ->Foreground(new fk::ui::SolidColorBrush(0, 0, 255))
                    ->Margin(fk::Thickness(0, 10, 0, 10))) | fk::ui::cell(2, 0),
                
                // Test 4: Chinese characters
                (fk::ui::UIElement*)((new fk::ui::TextBlock())
                    ->Name("test4")
                    ->Text("中文字符测试")
                    ->FontSize(24)
                    ->FontFamily("Microsoft YaHei")
                    ->Foreground(new fk::ui::SolidColorBrush(255, 140, 0))
                    ->Margin(fk::Thickness(0, 10, 0, 10))) | fk::ui::cell(3, 0),
                
                // Test 5: Long text
                (fk::ui::UIElement*)((new fk::ui::TextBlock())
                    ->Name("test5")
                    ->Text("This is a long text to test accurate measurement of proportional fonts")
                    ->FontSize(18)
                    ->FontFamily("Microsoft YaHei")
                    ->Foreground(new fk::ui::SolidColorBrush(128, 0, 128))
                    ->Margin(fk::Thickness(0, 10, 0, 10))) | fk::ui::cell(4, 0)
            }));

    mainWindow->Loaded += [mainWindow]() {
        std::cout << "\n=== Text Measurement Test ===" << std::endl;
        
        auto test1 = static_cast<fk::ui::TextBlock*>(mainWindow->FindName("test1"));
        auto size1 = test1->GetRenderSize();
        std::cout << "Test 1 (WWWWWWWWWW): " << size1.width << " x " << size1.height << std::endl;
        
        auto test2 = static_cast<fk::ui::TextBlock*>(mainWindow->FindName("test2"));
        auto size2 = test2->GetRenderSize();
        std::cout << "Test 2 (iiiiiiiiii): " << size2.width << " x " << size2.height << std::endl;
        
        auto test3 = static_cast<fk::ui::TextBlock*>(mainWindow->FindName("test3"));
        auto size3 = test3->GetRenderSize();
        std::cout << "Test 3 (WiWiWiWiWi): " << size3.width << " x " << size3.height << std::endl;
        
        auto test4 = static_cast<fk::ui::TextBlock*>(mainWindow->FindName("test4"));
        auto size4 = test4->GetRenderSize();
        std::cout << "Test 4 (中文字符测试): " << size4.width << " x " << size4.height << std::endl;
        
        auto test5 = static_cast<fk::ui::TextBlock*>(mainWindow->FindName("test5"));
        auto size5 = test5->GetRenderSize();
        std::cout << "Test 5 (long text): " << size5.width << " x " << size5.height << std::endl;
        
        std::cout << "\nNote: With accurate measurement, 'W' should be wider than 'i'" << std::endl;
        std::cout << "      Test 1 width should be larger than Test 2 width" << std::endl;
        std::cout << "==============================\n" << std::endl;
    };
    
    app.Run(mainWindow);
    return 0;
}
