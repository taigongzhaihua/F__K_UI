#include "fk/app/Application.h"
#include "fk/ui/Window.h"
#include "fk/ui/TextBlock.h"
#include "fk/ui/Brush.h"
#include "fk/ui/StackPanel.h"
#include "fk/ui/ToggleButton.h"
#include "fk/ui/Thickness.h"
#include <iostream>

int main(int argc, char **argv)
{
    fk::Application app;
    auto mainWindow = std::make_shared<fk::ui::Window>();
    mainWindow->Title("ToggleButton Test")
        ->Width(400)
        ->Height(300)
        ->Background(new fk::ui::SolidColorBrush(240, 240, 240))
        ->Content(
            (new fk::ui::StackPanel())
                ->Margin(fk::ui::Thickness(20))
                ->Children({
                    (new fk::ui::TextBlock())
                        ->Text("ToggleButton Demo")
                        ->FontSize(24)
                        ->Margin(fk::ui::Thickness(0, 0, 0, 20)),
                    
                    (new fk::ui::ToggleButton())
                        ->Name("toggleWifi")
                        ->Content(
                            (new fk::ui::TextBlock())
                                ->Text("WiFi")
                                ->FontSize(16))
                        ->Background(new fk::ui::SolidColorBrush(120, 120, 120))
                        ->CheckedBackground(new fk::ui::SolidColorBrush(50, 200, 50))
                        ->Margin(fk::ui::Thickness(0, 5, 0, 5)),
                    
                    (new fk::ui::ToggleButton())
                        ->Name("toggleBluetooth")
                        ->Content(
                            (new fk::ui::TextBlock())
                                ->Text("Bluetooth")
                                ->FontSize(16))
                        ->Background(new fk::ui::SolidColorBrush(120, 120, 120))
                        ->CheckedBackground(new fk::ui::SolidColorBrush(0, 120, 215))
                        ->IsChecked(true)
                        ->Margin(fk::ui::Thickness(0, 5, 0, 5)),
                    
                    (new fk::ui::ToggleButton())
                        ->Name("toggleAirplane")
                        ->Content(
                            (new fk::ui::TextBlock())
                                ->Text("Airplane Mode")
                                ->FontSize(16))
                        ->Background(new fk::ui::SolidColorBrush(120, 120, 120))
                        ->CheckedBackground(new fk::ui::SolidColorBrush(255, 140, 0))
                        ->Margin(fk::ui::Thickness(0, 5, 0, 5)),
                    
                    (new fk::ui::ToggleButton())
                        ->Name("toggleThreeState")
                        ->Content(
                            (new fk::ui::TextBlock())
                                ->Text("Three-State Toggle")
                                ->FontSize(16))
                        ->Background(new fk::ui::SolidColorBrush(120, 120, 120))
                        ->CheckedBackground(new fk::ui::SolidColorBrush(100, 150, 255))
                        ->IsThreeState(true)
                        ->Margin(fk::ui::Thickness(0, 5, 0, 5))
                }));

    // Bind ToggleButton events
    auto toggleWifi = static_cast<fk::ui::ToggleButton *>(mainWindow->FindName("toggleWifi"));
    toggleWifi->Checked += [](std::optional<bool> state) {
        std::cout << "WiFi: ON" << std::endl;
    };
    toggleWifi->Unchecked += [](std::optional<bool> state) {
        std::cout << "WiFi: OFF" << std::endl;
    };

    auto toggleBluetooth = static_cast<fk::ui::ToggleButton *>(mainWindow->FindName("toggleBluetooth"));
    toggleBluetooth->Checked += [](std::optional<bool> state) {
        std::cout << "Bluetooth: ON" << std::endl;
    };
    toggleBluetooth->Unchecked += [](std::optional<bool> state) {
        std::cout << "Bluetooth: OFF" << std::endl;
    };

    auto toggleAirplane = static_cast<fk::ui::ToggleButton *>(mainWindow->FindName("toggleAirplane"));
    toggleAirplane->Checked += [](std::optional<bool> state) {
        std::cout << "Airplane Mode: ON" << std::endl;
    };
    toggleAirplane->Unchecked += [](std::optional<bool> state) {
        std::cout << "Airplane Mode: OFF" << std::endl;
    };

    auto toggleThreeState = static_cast<fk::ui::ToggleButton *>(mainWindow->FindName("toggleThreeState"));
    toggleThreeState->Checked += [](std::optional<bool> state) {
        std::cout << "Three-State: CHECKED" << std::endl;
    };
    toggleThreeState->Unchecked += [](std::optional<bool> state) {
        std::cout << "Three-State: UNCHECKED" << std::endl;
    };
    toggleThreeState->Indeterminate += [](std::optional<bool> state) {
        std::cout << "Three-State: INDETERMINATE" << std::endl;
    };

    app.Run(mainWindow);
    return 0;
}
