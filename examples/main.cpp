#include "fk/app/Application.h"
#include "fk/ui/Window.h"
#include "fk/ui/TextBlock.h"
#include "fk/ui/Brush.h"
#include "fk/ui/StackPanel.h"
#include "fk/ui/Button.h"
#include "fk/core/Logger.h"



int main(int argc, char **argv)
{
    fk::Application app;
    auto mainWindow = std::make_shared<fk::ui::Window>();
    mainWindow->Title("F K UI Example")
        ->Width(800)
        ->Height(600)
        ->Background(fk::ui::Brushes::LightGray())
        ->Content((new fk::ui::StackPanel())
                      ->Children(
                          {(new fk::ui::TextBlock())
                               ->Text("Hello, F K UI!")
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
                               ->Background(fk::ui::Brushes::Blue())
                               ->Content((new fk::ui::TextBlock())
                                             ->Text("Click Me")
                                             ->FontSize(20)
                                             ->Foreground(fk::ui::Brushes::White()))}));
    static_cast<fk::ui::Button*>(mainWindow->FindName("myButton"))->Click+=[]() {
        fk::core::ConsoleLogger Logger;
        Logger.Log(fk::core::Logger::Level::Info, "Button was clicked!");
    };
    app.Run(mainWindow);
    return 0;
}