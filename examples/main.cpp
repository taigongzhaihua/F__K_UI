#include "fk/app/Application.h"
#include "fk/ui/Window.h"
int main(int argc, char** argv) {
    fk::Application app;
auto mainWindow = std::make_shared<fk::ui::Window>();
    mainWindow->Title("F K UI Example")
        ->Width(800)
        ->Height(600);
     app.Run(mainWindow);
    return 0;
}