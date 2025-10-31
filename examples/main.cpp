#include <iostream>
#include <memory>
#include "fk/app/Application.h"
#include "fk/ui/Window.h"
#include "fk/ui/StackPanel.h"
#include "fk/ui/ContentControl.h"
#include "fk/ui/Button.h"
#include "fk/ui/TextBlock.h"

using namespace fk;

int main()
{
    try
    {
        std::cout << "\n=== F__K_UI Nested Chaining Demo ===" << std::endl;
        std::cout << "Creating window with nested button hierarchy..." << std::endl;

        // 使用 Children() 接口批量添加子元素 - 更简洁!
        auto window = ui::window()
                          ->Title("F__K_UI Demo - Nested Chaining")
                          ->Width(800)
                          ->Height(600)
                          ->Content(
                              ui::stackPanel()
                                  ->Orientation(ui::Orientation::Vertical)
                                  ->Spacing(10.0f) // 设置子元素间距为 10px
                                  ->Children(
                                      {ui::textBlock()
                                           ->Text("欢迎使用 F__K_UI!")
                                           ->Foreground("#FFFFFF")
                                           ->FontSize(24.0f)
                                           ->Width(250)
                                           ->Height(40),

                                       ui::textBlock()
                                           ->Text("这是一个测试文本块")
                                           ->Foreground("#CCCCCC")
                                           ->FontSize(18.0f)
                                           ->Width(250)
                                           ->Height(30),

                                       ui::textBlock()
                                           ->Text("这是一段很长的文本,用来测试自动换行功能。当文本内容超过控件宽度时,应该能够自动换行显示,这样用户就可以看到完整的内容。")
                                           ->Foreground("#FFFF00")
                                           ->FontSize(16.0f)
                                           ->TextWrapping(ui::TextWrapping::Wrap),

                                       ui::textBlock()
                                           ->Text("这是一段很长的文本,用来测试截断功能。当文本超出宽度时会自动添加省略号...")
                                           ->Foreground("#00FFFF")
                                           ->FontSize(16.0f)
                                           ->Width(200)
                                           ->TextTrimming(ui::TextTrimming::CharacterEllipsis),

                                       ui::button()
                                           ->HorizontalAlignment(ui::HorizontalAlignment::Center)
                                           ->Content("默认按钮")
                                           ->OnClick([](ui::detail::ButtonBase &)
                                                     { std::cout << ">>> Default Button Clicked!" << std::endl; }),
                                       ui::button()
                                           ->HorizontalAlignment(ui::HorizontalAlignment::Center)
                                           ->Background("#008500") // 绿色 (成功)
                                           ->Content("成功按钮")
                                           ->OnClick([](ui::detail::ButtonBase &)
                                                     { std::cout << ">>> Success Button Clicked!" << std::endl; }),
                                       ui::button()
                                           ->HorizontalAlignment(ui::HorizontalAlignment::Center)
                                           ->Background("#e93b40") // 红色 (危险)
                                           ->Content("危险按钮")
                                           ->OnClick([](ui::detail::ButtonBase &)
                                                     { std::cout << ">>> Danger Button Clicked!" << std::endl; }),
                                       ui::button()
                                           ->HorizontalAlignment(ui::HorizontalAlignment::Center)
                                           ->Background("#F7630C") // 橙色 (警告)
                                           ->Content("警告按钮")
                                           ->OnClick([](ui::detail::ButtonBase &)
                                                     { std::cout << ">>> Warning Button Clicked!" << std::endl; }),
                                       ui::button()
                                           ->HorizontalAlignment(ui::HorizontalAlignment::Center)
                                           ->Background("#5C2D91") // 紫色 (次要)
                                           ->Content("次要按钮")
                                           ->OnClick([](ui::detail::ButtonBase &)
                                                     { std::cout << ">>> Secondary Button Clicked!" << std::endl; })}));

        std::cout << "Window hierarchy created!" << std::endl;
        std::cout << "Subscribing events..." << std::endl;

        std::cout << "Window hierarchy created!" << std::endl;
        std::cout << "Subscribing events..." << std::endl;

        window->Opened += []()
        {
            std::cout << ">>> Window opened!" << std::endl;
        };

        window->Closed += []()
        {
            std::cout << ">>> Window closed!" << std::endl;
        };

        window->Resized += [](int w, int h)
        {
            std::cout << ">>> Window resized to " << w << "x" << h << std::endl;
        };

        std::cout << "Creating application..." << std::endl;
        Application app;

        std::cout << "Running application with main window..." << std::endl;
        app.Run(window);

        std::cout << "=== Demo completed ===" << std::endl;
    }
    catch (const std::exception &e)
    {
        std::cerr << "ERROR: " << e.what() << std::endl;
        return -1;
    }
    catch (...)
    {
        std::cerr << "ERROR: Unknown exception" << std::endl;
        return -1;
    }

    return 0;
}
