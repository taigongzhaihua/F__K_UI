#include "fk/app/Application.h"
#include "fk/ui/Window.h"
#include "fk/ui/TextBlock.h"
#include "fk/ui/Brush.h"
#include "fk/ui/StackPanel.h"

int main(int argc, char **argv)
{
    fk::Application app;
    auto mainWindow = std::make_shared<fk::ui::Window>();
    mainWindow->Title("F K UI Example")
        ->Width(800)
        ->Height(600)
        ->Content((new fk::ui::StackPanel())
                      ->Children({(new fk::ui::TextBlock())
                                      ->Text("Hello, F K UI!")
                                      ->FontSize(32)
                                      ->Foreground(fk::ui::Brushes::Blue())
                                      ->Margin(fk::Thickness(20)),
                                  (new fk::ui::TextBlock())
                                      ->Text("This is a simple example of F K UI framework.")
                                      ->FontSize(16)
                                      ->Foreground(fk::ui::Brushes::DarkGray())
                                      ->Margin(fk::Thickness(20))}));
    app.Run(mainWindow);
    return 0;
}