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
                                  ->Children({
                                      std::static_pointer_cast<ui::UIElement>(
                                          ui::textBlock()
                                              ->Text("欢迎使用 F__K_UI!")
                                              ->Foreground("#FFFFFF")
                                              ->FontSize(24.0f)
                                              ->Width(250)->Height(40)
                                      ),
                                      std::static_pointer_cast<ui::UIElement>(
                                          ui::textBlock()
                                              ->Text("这是一个测试文本块")
                                              ->Foreground("#CCCCCC")
                                              ->FontSize(16.0f)
                                              ->Width(250)->Height(30)
                                      ),
                                      ui::button()
                                          ->Width(250)->Height(50)
                                          ->CornerRadius(10.0f)
                                          ->Background("#4CAF50")  // 绿色
                                          ->BorderBrush("#2E7D32")
                                          ->BorderThickness(2.0f)
                                          ->OnClick([](ui::detail::ButtonBase&) {
                                              std::cout << ">>> Green Button Clicked!" << std::endl;
                                          }),
                                      ui::button()
                                          ->Width(250)->Height(50)
                                          ->CornerRadius(10.0f)
                                          ->Background("#2196F3")  // 蓝色
                                          ->BorderBrush("#1565C0")
                                          ->BorderThickness(2.0f)
                                          ->OnClick([](ui::detail::ButtonBase&) {
                                              std::cout << ">>> Blue Button Clicked!" << std::endl;
                                          }),
                                      ui::button()
                                          ->Width(250)->Height(50)
                                          ->CornerRadius(10.0f)
                                          ->Background("#FF9800")  // 橙色
                                          ->BorderBrush("#E65100")
                                          ->BorderThickness(2.0f)
                                          ->OnClick([](ui::detail::ButtonBase&) {
                                              std::cout << ">>> Orange Button Clicked!" << std::endl;
                                          }),
                                      ui::button()
                                          ->Width(250)->Height(50)
                                          ->CornerRadius(10.0f)
                                          ->Background("#F44336")  // 红色
                                          ->BorderBrush("#B71C1C")
                                          ->BorderThickness(2.0f)
                                          ->OnClick([](ui::detail::ButtonBase&) {
                                              std::cout << ">>> Red Button Clicked!" << std::endl;
                                          }),
                                      ui::button()
                                          ->Width(250)->Height(50)
                                          ->CornerRadius(10.0f)
                                          ->Background("#9C27B0")  // 紫色
                                          ->BorderBrush("#4A148C")
                                          ->BorderThickness(2.0f)
                                          ->OnClick([](ui::detail::ButtonBase&) {
                                              std::cout << ">>> Purple Button Clicked!" << std::endl;
                                          })
                                  })
                          );

        std::cout << "Window hierarchy created!" << std::endl;
        std::cout << "Subscribing events..." << std::endl;

        std::cout << "Window hierarchy created!" << std::endl;
        std::cout << "Subscribing events..." << std::endl;

        window->Opened += []() {
            std::cout << ">>> Window opened!" << std::endl;
        };

        window->Closed += []() {
            std::cout << ">>> Window closed!" << std::endl;
        };

        window->Resized += [](int w, int h) {
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
