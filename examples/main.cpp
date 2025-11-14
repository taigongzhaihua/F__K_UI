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
            ->FontSize(24)
            ->Foreground((new fk::ui::SolidColorBrush())->FromArgb(0xFF1E90FF)));
     app.Run(mainWindow);
    return 0;
}