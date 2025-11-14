#include "fk/app/Application.h"
#include "fk/ui/Window.h"
#include "fk/ui/TextBlock.h"
#include "fk/ui/Brush.h"

int main(int argc, char** argv) {
    fk::Application app;
    auto mainWindow = std::make_shared<fk::ui::Window>();
    mainWindow->Title("F K UI Example")
        ->Width(800)
        ->Height(600)
        ->Content((new fk::ui::TextBlock())
            ->Text("Hello, F K UI!")
            ->FontFamily("Arial")
            ->FontSize(24.0f)
            ->FontWeight(fk::ui::FontWeight::Bold)
            ->TextAlignment(fk::ui::TextAlignment::Center)
            ->TextWrapping(fk::ui::TextWrapping::Wrap)
            ->Foreground(new fk::ui::SolidColorBrush()));
    app.Run(mainWindow);
    return 0;
}
