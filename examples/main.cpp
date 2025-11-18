#include "fk/app/Application.h"
#include "fk/ui/Window.h"
#include "fk/ui/TextBlock.h"
#include "fk/ui/Brush.h"
#include "fk/ui/StackPanel.h"
#include "fk/ui/Button.h"
#include "fk/core/Logger.h"
#include <iostream>

int main(int argc, char **argv)
{
    fk::Application app;
    auto mainWindow = std::make_shared<fk::ui::Window>();
    mainWindow->Title("F__K UI Example")
        ->Width(800)
        ->Height(600)
        ->Background(fk::ui::Brushes::LightGray())
        ->Content((new fk::ui::StackPanel())
                      ->Children(
                          {(new fk::ui::TextBlock())
                               ->Text("Hello, F__K UI!")
                               ->FontSize(32)
                               ->Foreground(fk::ui::Brushes::Blue())
                               ->Margin(fk::Thickness(20)),
                           (new fk::ui::TextBlock())
                               ->Text("This is a simple example of F K UI framework.")
                               ->FontSize(16)
                               ->Foreground(fk::ui::Brushes::DarkGray())
                               ->Margin(fk::Thickness(20)),
                           (new fk::ui::Button())
                               ->Name("myButton")
                               ->MouseOverBackground(fk::ui::Color::FromRGB(255, 200, 200, 255)) // 悬停时浅红色
                               ->PressedBackground(fk::ui::Color::FromRGB(255, 100, 100, 255))   // 按下时深红色
                               ->Margin(fk::Thickness(20))
                               ->Content((new fk::ui::TextBlock())
                                             ->Text("Click Me")
                                             ->FontSize(20)
                                             ->Foreground(fk::ui::Brushes::Black()))}));

    // 设置按钮交互颜色并绑定点击事件
    auto *btn = static_cast<fk::ui::Button *>(mainWindow->FindName("myButton"));
    btn->Click += []()
    {
        std::cout << "Button clicked!" << std::endl;
    };
    app.Run(mainWindow);
    return 0;
}