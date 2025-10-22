#include <iostream>
#include <memory>
#include "../src/Application.h"
#include "../src/Window.h"
#include <thread>
#include <chrono>

using namespace fk;

int main()
{
    Application app;

    auto win = window()
                   ->Title("Demo Window")
                   ->Width(640)
                   ->Height(480);

    win->Opened += []()
    { std::cout << "Event: Opened" << std::endl; };
    win->Closed += []()
    { std::cout << "Event: Closed" << std::endl; };
    win->Resized += [](int w, int h)
    { std::cout << "Event: Resized " << w << "x" << h << std::endl; };

    const std::string windowName = "MainWindow";
    app.AddWindow(win, windowName);

    // Run will exit when no windows remain (in this simple demo we remove after a bit)
    std::thread t([&]()
                  {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        // simulate closing by name
        app.RemoveWindow(windowName); });

    app.Run();

    if (t.joinable())
    {
        t.join();
    }
    return 0;
}
