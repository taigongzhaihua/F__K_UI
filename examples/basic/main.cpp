#include "fk/app/Application.h"
#include "fk/ui/Window.h"
#include "fk/ui/TextBlock.h"
#include "fk/ui/Brush.h"
#include "fk/ui/StackPanel.h"
#include "fk/ui/Button.h"
#include "fk/ui/ToggleButton.h"
#include "fk/ui/CheckBox.h"
#include "fk/ui/RadioButton.h"
#include "fk/ui/Border.h"
#include "fk/ui/Grid.h"
#include "fk/ui/GridCellAttacher.h"
#include "fk/ui/Thickness.h"
#include "fk/core/Logger.h"

#include <iostream>

int main(int argc, char **argv)
{
    fk::Application app;
    auto mainWindow = std::make_shared<fk::ui::Window>();
    mainWindow->Title("F__K UI - Complex Layout Example")
        ->Width(1200)
        ->Height(800)
        ->Background(new fk::ui::SolidColorBrush(240, 245, 255))
        ->Content(
            (new fk::ui::Grid())
                ->Rows("60, *, 40") // Header, Content, Footer
                ->Columns("200, *") // Sidebar, Main content
                ->Children(         // Top header bar - spans 2 columns
                    {
                        (new fk::ui::Grid())
                                ->Background(new fk::ui::SolidColorBrush(60, 80, 120))
                                ->Columns("*, Auto, Auto, Auto")
                                ->Children(
                                    {(new fk::ui::TextBlock())
                                             ->Text("F__K UI Dashboard")
                                             ->FontSize(24)
                                             ->SetVAlign(fk::ui::VerticalAlignment::Center)
                                             ->Foreground(fk::ui::Brushes::White())
                                             ->Margin(fk::ui::Thickness(20, 0, 0, 0)) |
                                         fk::ui::cell(0, 0),
                                     (new fk::ui::Button())
                                             ->Name("btnHome")
                                             ->SetVAlign(fk::ui::VerticalAlignment::Center)
                                             ->Content(
                                                 (new fk::ui::TextBlock())
                                                     ->Text("Home")
                                                     ->FontSize(14)
                                                     ->Foreground(fk::ui::Brushes::White()))
                                             ->Background(new fk::ui::SolidColorBrush(0, 0, 0, 0))
                                             ->MouseOverBackground(fk::ui::Color::FromRGB(255, 255, 255, 20))
                                             ->PressedBackground(fk::ui::Color::FromRGB(255, 255, 255, 30))
                                             ->BorderBrush(fk::ui::Brushes::White())
                                             ->BorderThickness(1)
                                             ->Margin(fk::ui::Thickness(10)) |
                                         fk::ui::cell(0, 1),
                                     (new fk::ui::Button())
                                             ->Name("btnSettings")
                                             ->SetVAlign(fk::ui::VerticalAlignment::Center)
                                             ->Content(
                                                 (new fk::ui::TextBlock())
                                                     ->Text("Settings")
                                                     ->FontSize(14)
                                                     ->Foreground(fk::ui::Brushes::White()))
                                             ->Background(new fk::ui::SolidColorBrush(0, 0, 0, 0))
                                             ->MouseOverBackground(fk::ui::Color::FromRGB(255, 255, 255, 20))
                                             ->PressedBackground(fk::ui::Color::FromRGB(255, 255, 255, 30))
                                             ->BorderBrush(fk::ui::Brushes::White())
                                             ->BorderThickness(1)
                                             ->Margin(fk::ui::Thickness(10)) |
                                         fk::ui::cell(0, 2),
                                     (new fk::ui::Button())
                                             ->Name("btnAbout")
                                             ->SetVAlign(fk::ui::VerticalAlignment::Center)
                                             ->Content(
                                                 (new fk::ui::TextBlock())
                                                     ->Text("About")
                                                     ->FontSize(14)
                                                     ->Foreground(fk::ui::Brushes::White()))
                                             ->Background(new fk::ui::SolidColorBrush(0, 0, 0, 0))
                                             ->MouseOverBackground(fk::ui::Color::FromRGB(255, 255, 255, 20))
                                             ->PressedBackground(fk::ui::Color::FromRGB(255, 255, 255, 30))
                                             ->BorderBrush(fk::ui::Brushes::White())
                                             ->BorderThickness(1)
                                             ->Margin(fk::ui::Thickness(10, 10, 20, 10)) |
                                         fk::ui::cell(0, 3)}) |
                            fk::ui::cell(0, 0, 1, 2),

                        // Left sidebar

                        (new fk::ui::StackPanel())
                                ->Background(new fk::ui::SolidColorBrush(250, 250, 250))
                                ->Children(
                                    {(new fk::ui::TextBlock())
                                         ->Text("Navigation")
                                         ->FontSize(18)
                                         ->Foreground(new fk::ui::SolidColorBrush(60, 80, 120))
                                         ->Margin(fk::ui::Thickness(15, 20, 15, 15)),
                                     (new fk::ui::Button())
                                         ->Name("navDashboard")
                                         ->Content(
                                             (new fk::ui::TextBlock())
                                                 ->Text("Dashboard")
                                                 ->FontSize(14))
                                         ->Background(new fk::ui::SolidColorBrush(100, 150, 255))
                                         ->MouseOverBackground(fk::ui::Color::FromRGB(130, 180, 255))
                                         ->PressedBackground(fk::ui::Color::FromRGB(80, 130, 230))
                                         ->Margin(fk::ui::Thickness(10, 5, 10, 5)),
                                     (new fk::ui::Button())
                                         ->Name("navProjects")
                                         ->Content(
                                             (new fk::ui::TextBlock())
                                                 ->Text("Projects")
                                                 ->FontSize(14))
                                         ->Background(fk::ui::Brushes::White())
                                         ->MouseOverBackground(fk::ui::Color::FromRGB(230, 240, 255))
                                         ->PressedBackground(fk::ui::Color::FromRGB(200, 220, 255))
                                         ->Margin(fk::ui::Thickness(10, 5, 10, 5)),
                                     (new fk::ui::Button())
                                         ->Name("navTasks")
                                         ->Content(
                                             (new fk::ui::TextBlock())
                                                 ->Text("Tasks")
                                                 ->FontSize(14))
                                         ->Background(fk::ui::Brushes::White())
                                         ->MouseOverBackground(fk::ui::Color::FromRGB(230, 240, 255))
                                         ->PressedBackground(fk::ui::Color::FromRGB(200, 220, 255))
                                         ->Margin(fk::ui::Thickness(10, 5, 10, 5)),
                                     (new fk::ui::Button())
                                         ->Name("navAnalytics")
                                         ->Content(
                                             (new fk::ui::TextBlock())
                                                 ->Text("Analytics")
                                                 ->FontSize(14))
                                         ->Background(fk::ui::Brushes::White())
                                         ->MouseOverBackground(fk::ui::Color::FromRGB(230, 240, 255))
                                         ->PressedBackground(fk::ui::Color::FromRGB(200, 220, 255))
                                         ->Margin(fk::ui::Thickness(10, 5, 10, 5))}) |
                            fk::ui::cell(1, 0),

                        // Main content area - contains multiple cards
                        (new fk::ui::Grid())
                                ->Rows("*, *")
                                ->Columns("*, *")
                                ->Margin(fk::ui::Thickness(20))
                                ->Children( // Card 1 - Welcome
                                    {(new fk::ui::Grid())
                                             ->Name("Card1")
                                             ->Background(fk::ui::Brushes::White())
                                             ->CornerRadius(8)
                                             ->Rows("Auto, *")
                                             ->Margin(fk::ui::Thickness(0, 0, 10, 10))
                                             ->Children(
                                                 {(new fk::ui::TextBlock())
                                                          ->Text("Welcome Back!")
                                                          ->FontSize(20)
                                                          ->Foreground(new fk::ui::SolidColorBrush(60, 80, 120))
                                                          ->Margin(fk::ui::Thickness(15)) |
                                                      fk::ui::cell(0, 0),
                                                  (new fk::ui::TextBlock())
                                                          ->Text("This is a complex layout example\nwith nested Grids and controls")
                                                          ->FontSize(14)
                                                          ->Foreground(fk::ui::Brushes::DarkGray())
                                                          ->Margin(fk::ui::Thickness(15, 0, 15, 15)) |
                                                      fk::ui::cell(1, 0)}) |
                                         fk::ui::cell(0, 0),

                                     // Card 2 - ToggleButton Demo
                                     (new fk::ui::Grid())
                                             ->Name("Card2")
                                             ->Background(new fk::ui::SolidColorBrush(255, 250, 240))
                                             ->CornerRadius(8)
                                             ->Rows("Auto, *")
                                             ->Margin(fk::ui::Thickness(10, 0, 0, 10))
                                             ->Padding(fk::ui::Thickness(0,0,0,15))
                                             ->Children(
                                                 {(new fk::ui::TextBlock())
                                                          ->Text("ToggleButton Demo")
                                                          ->FontSize(20)
                                                          ->Foreground(new fk::ui::SolidColorBrush(255, 140, 0))
                                                          ->Margin(fk::ui::Thickness(15)) |
                                                      fk::ui::cell(0, 0),
                                                  (new fk::ui::StackPanel())
                                                          ->Margin(fk::ui::Thickness(15, 0, 15, 0))
                                                          ->Children(
                                                              {(new fk::ui::TextBlock())
                                                                   ->Text("Click to toggle state:")
                                                                   ->FontSize(14)
                                                                   ->Margin(fk::ui::Thickness(0, 5, 0, 10)),
                                                               (new fk::ui::ToggleButton())
                                                                   ->Name("toggleWifi")
                                                                   ->Content(
                                                                       (new fk::ui::TextBlock())
                                                                           ->Text("WiFi")
                                                                           ->FontSize(14))
                                                                   ->Background(new fk::ui::SolidColorBrush(120, 120, 120))
                                                                   ->CheckedBackground(new fk::ui::SolidColorBrush(50, 200, 50))
                                                                   ->Margin(fk::ui::Thickness(0, 5, 0, 5)),
                                                               (new fk::ui::ToggleButton())
                                                                   ->Name("toggleBluetooth")
                                                                   ->Content(
                                                                       (new fk::ui::TextBlock())
                                                                           ->Text("Bluetooth")
                                                                           ->FontSize(14))
                                                                   ->Background(new fk::ui::SolidColorBrush(120, 120, 120))
                                                                   ->CheckedBackground(new fk::ui::SolidColorBrush(0, 120, 215))
                                                                   ->IsChecked(true)
                                                                   ->Margin(fk::ui::Thickness(0, 5, 0, 5)),
                                                               (new fk::ui::ToggleButton())
                                                                   ->Name("toggleAirplane")
                                                                   ->Content(
                                                                       (new fk::ui::TextBlock())
                                                                           ->Text("Airplane Mode")
                                                                           ->FontSize(14))
                                                                   ->Background(new fk::ui::SolidColorBrush(120, 120, 120))
                                                                   ->CheckedBackground(new fk::ui::SolidColorBrush(255, 140, 0))
                                                                   ->Margin(fk::ui::Thickness(0, 5, 0, 5)),
                                                               (new fk::ui::CheckBox())
                                                                   ->Name("checkUpdates")
                                                                   ->Content(
                                                                       (new fk::ui::TextBlock())
                                                                           ->Text("Enable Automatic Updates")
                                                                           ->FontSize(14))
                                                                   ->Background(new fk::ui::SolidColorBrush(0, 0, 0, 0))
                                                                   ->BorderBrush(new fk::ui::SolidColorBrush(0, 0, 0, 0)),
                                                               (new fk::ui::TextBlock())
                                                                   ->Text("Select Theme:")
                                                                   ->FontSize(14)
                                                                   ->Margin(fk::ui::Thickness(0, 15, 0, 10)),
                                                               (new fk::ui::RadioButton())
                                                                   ->Name("radioLight")
                                                                   ->Content(
                                                                       (new fk::ui::TextBlock())
                                                                           ->Text("Light Theme")
                                                                           ->FontSize(14))
                                                                   ->IsChecked(true)
                                                                   ->Margin(fk::ui::Thickness(0, 5, 0, 5)),
                                                               (new fk::ui::RadioButton())
                                                                   ->Name("radioDark")
                                                                   ->Content(
                                                                       (new fk::ui::TextBlock())
                                                                           ->Text("Dark Theme")
                                                                           ->FontSize(14))
                                                                   ->Margin(fk::ui::Thickness(0, 5, 0, 5)),
                                                               (new fk::ui::RadioButton())
                                                                   ->Name("radioAuto")
                                                                   ->Content(
                                                                       (new fk::ui::TextBlock())
                                                                           ->Text("Auto (System)")
                                                                           ->FontSize(14))
                                                                   ->Margin(fk::ui::Thickness(0, 5, 0, 5))}) |
                                                      fk::ui::cell(1, 0)}) |
                                         fk::ui::cell(0, 1),

                                     // Card 3 - Quick Actions
                                     (new fk::ui::Grid())
                                             ->Background(new fk::ui::SolidColorBrush(240, 255, 240))
                                             ->CornerRadius(8)
                                             ->Rows("Auto, *")
                                             ->Margin(fk::ui::Thickness(0, 10, 10, 0))
                                             ->Children(
                                                 {(new fk::ui::TextBlock())
                                                          ->Text("Quick Actions")
                                                          ->FontSize(20)
                                                          ->Foreground(new fk::ui::SolidColorBrush(34, 139, 34))
                                                          ->Margin(fk::ui::Thickness(15)) |
                                                      fk::ui::cell(0, 0),
                                                  (new fk::ui::Grid())
                                                          ->Rows("Auto, Auto")
                                                          ->Columns("*, *")
                                                          ->Margin(fk::ui::Thickness(15, 0, 15, 15))
                                                          ->Children(
                                                              {(new fk::ui::Button())
                                                                       ->Name("btnNewProject")
                                                                       ->Content(
                                                                           (new fk::ui::TextBlock())
                                                                               ->Text("New Project")
                                                                               ->FontSize(14))
                                                                       ->Background(new fk::ui::SolidColorBrush(50, 200, 50))
                                                                       ->MouseOverBackground(fk::ui::Color::FromRGB(70, 220, 70))
                                                                       ->PressedBackground(fk::ui::Color::FromRGB(30, 180, 30))
                                                                       ->Margin(fk::ui::Thickness(5)) |
                                                                   fk::ui::cell(0, 0),
                                                               (new fk::ui::Button())
                                                                       ->Name("btnNewTask")
                                                                       ->Content(
                                                                           (new fk::ui::TextBlock())
                                                                               ->Text("New Task")
                                                                               ->FontSize(14))
                                                                       ->Background(new fk::ui::SolidColorBrush(100, 180, 255))
                                                                       ->MouseOverBackground(fk::ui::Color::FromRGB(120, 200, 255))
                                                                       ->PressedBackground(fk::ui::Color::FromRGB(80, 160, 230))
                                                                       ->Margin(fk::ui::Thickness(5)) |
                                                                   fk::ui::cell(0, 1),
                                                               (new fk::ui::Button())
                                                                       ->Name("btnImport")
                                                                       ->Content(
                                                                           (new fk::ui::TextBlock())
                                                                               ->Text("Import")
                                                                               ->FontSize(14))
                                                                       ->Background(new fk::ui::SolidColorBrush(255, 200, 100))
                                                                       ->MouseOverBackground(fk::ui::Color::FromRGB(255, 220, 120))
                                                                       ->PressedBackground(fk::ui::Color::FromRGB(230, 180, 80))
                                                                       ->Margin(fk::ui::Thickness(5)) |
                                                                   fk::ui::cell(1, 0),
                                                               (new fk::ui::Button())
                                                                       ->Name("btnExport")
                                                                       ->Content(
                                                                           (new fk::ui::TextBlock())
                                                                               ->Text("Export")
                                                                               ->FontSize(14))
                                                                       ->Background(new fk::ui::SolidColorBrush(255, 150, 200))
                                                                       ->MouseOverBackground(fk::ui::Color::FromRGB(255, 170, 220))
                                                                       ->PressedBackground(fk::ui::Color::FromRGB(230, 130, 180))
                                                                       ->Margin(fk::ui::Thickness(5)) |
                                                                   fk::ui::cell(1, 1)}) |
                                                      fk::ui::cell(1, 0)}) |
                                         fk::ui::cell(1, 0),

                                     // Card 4 - Recent Activity
                                     (new fk::ui::Grid())
                                             ->Background(new fk::ui::SolidColorBrush(240, 248, 255))
                                             ->CornerRadius(8)
                                             ->Rows("Auto, *")
                                             ->Margin(fk::ui::Thickness(10, 10, 0, 0))
                                             ->Children(
                                                 {(new fk::ui::TextBlock())
                                                          ->Text("Recent Activity")
                                                          ->FontSize(20)
                                                          ->Foreground(new fk::ui::SolidColorBrush(70, 130, 180))
                                                          ->Margin(fk::ui::Thickness(15)) |
                                                      fk::ui::cell(0, 0),
                                                  (new fk::ui::StackPanel())
                                                          ->Margin(fk::ui::Thickness(15, 0, 15, 15))
                                                          ->Children(
                                                              {(new fk::ui::TextBlock())
                                                                   ->Text("- Updated UI framework docs")
                                                                   ->FontSize(12)
                                                                   ->Foreground(fk::ui::Brushes::DarkGray())
                                                                   ->Margin(fk::ui::Thickness(0, 3, 0, 3)),
                                                               (new fk::ui::TextBlock())
                                                                   ->Text("- Optimized layout system")
                                                                   ->FontSize(12)
                                                                   ->Foreground(fk::ui::Brushes::DarkGray())
                                                                   ->Margin(fk::ui::Thickness(0, 3, 0, 3)),
                                                               (new fk::ui::TextBlock())
                                                                   ->Text("- Added button visual states")
                                                                   ->FontSize(12)
                                                                   ->Foreground(fk::ui::Brushes::DarkGray())
                                                                   ->Margin(fk::ui::Thickness(0, 3, 0, 3)),
                                                               (new fk::ui::TextBlock())
                                                                   ->Text("- Fixed Grid layout issues")
                                                                   ->FontSize(12)
                                                                   ->Foreground(fk::ui::Brushes::DarkGray())
                                                                   ->Margin(fk::ui::Thickness(0, 3, 0, 3))}) |
                                                      fk::ui::cell(1, 0)}) |
                                         fk::ui::cell(1, 1)}) |
                            fk::ui::cell(1, 1),

                        // Bottom status bar - spans 2 columns
                        (new fk::ui::Grid())
                                ->Background(new fk::ui::SolidColorBrush(60, 80, 120))
                                ->Columns("Auto, *, Auto")
                                ->Children(
                                    {(new fk::ui::TextBlock())
                                             ->Text("Ready | ")
                                             ->SetVAlign(fk::ui::VerticalAlignment::Center)
                                             ->FontSize(12)
                                             ->Foreground(fk::ui::Brushes::White())
                                             ->Margin(fk::ui::Thickness(15, 0, 0, 0)) |
                                         fk::ui::cell(0, 0),
                                     (new fk::ui::TextBlock())
                                             ->Text("F__K UI v1.0 - Complex Layout Demo")
                                             ->SetVAlign(fk::ui::VerticalAlignment::Center)
                                             ->FontSize(12)
                                             ->Foreground(new fk::ui::SolidColorBrush(255, 255, 255, 200))
                                             ->Margin(fk::ui::Thickness(0)) |
                                         fk::ui::cell(0, 1),
                                     (new fk::ui::TextBlock())
                                             ->Text("Nov 19, 2025")
                                             ->SetVAlign(fk::ui::VerticalAlignment::Center)
                                             ->FontSize(12)
                                             ->Foreground(fk::ui::Brushes::White())
                                             ->Margin(fk::ui::Thickness(0, 0, 15, 0)) |
                                         fk::ui::cell(0, 2)}) |
                            fk::ui::cell(2, 0, 1, 2)}));

    // Bind click events for all buttons
    auto btnHome = static_cast<fk::ui::Button *>(mainWindow->FindName("btnHome"));
    btnHome->Click += []()
    { std::cout << "Clicked: Home" << std::endl; };

    auto btnSettings = static_cast<fk::ui::Button *>(mainWindow->FindName("btnSettings"));
    btnSettings->Click += []()
    { std::cout << "Clicked: Settings" << std::endl; };

    auto btnAbout = static_cast<fk::ui::Button *>(mainWindow->FindName("btnAbout"));
    btnAbout->Click += []()
    { std::cout << "Clicked: About" << std::endl; };

    auto navDashboard = static_cast<fk::ui::Button *>(mainWindow->FindName("navDashboard"));
    navDashboard->Click += []()
    { std::cout << "Navigate to: Dashboard" << std::endl; };

    auto navProjects = static_cast<fk::ui::Button *>(mainWindow->FindName("navProjects"));
    navProjects->Click += []()
    { std::cout << "Navigate to: Projects" << std::endl; };

    auto navTasks = static_cast<fk::ui::Button *>(mainWindow->FindName("navTasks"));
    navTasks->Click += []()
    { std::cout << "Navigate to: Tasks" << std::endl; };

    auto navAnalytics = static_cast<fk::ui::Button *>(mainWindow->FindName("navAnalytics"));
    navAnalytics->Click += []()
    { std::cout << "Navigate to: Analytics" << std::endl; };

    auto btnNewProject = static_cast<fk::ui::Button *>(mainWindow->FindName("btnNewProject"));
    btnNewProject->Click += []()
    { std::cout << "Action: New Project" << std::endl; };

    auto btnNewTask = static_cast<fk::ui::Button *>(mainWindow->FindName("btnNewTask"));
    btnNewTask->Click += []()
    { std::cout << "Action: New Task" << std::endl; };

    // Bind ToggleButton events
    auto toggleWifi = static_cast<fk::ui::ToggleButton *>(mainWindow->FindName("toggleWifi"));
    toggleWifi->Checked += [](std::optional<bool> state)
    { std::cout << "WiFi: ON" << std::endl; };
    toggleWifi->Unchecked += [](std::optional<bool> state)
    { std::cout << "WiFi: OFF" << std::endl; };

    auto toggleBluetooth = static_cast<fk::ui::ToggleButton *>(mainWindow->FindName("toggleBluetooth"));
    toggleBluetooth->Checked += [](std::optional<bool> state)
    { std::cout << "Bluetooth: ON" << std::endl; };
    toggleBluetooth->Unchecked += [](std::optional<bool> state)
    { std::cout << "Bluetooth: OFF" << std::endl; };

    auto toggleAirplane = static_cast<fk::ui::ToggleButton *>(mainWindow->FindName("toggleAirplane"));
    toggleAirplane->Checked += [](std::optional<bool> state)
    { std::cout << "Airplane Mode: ON" << std::endl; };
    toggleAirplane->Unchecked += [](std::optional<bool> state)
    { std::cout << "Airplane Mode: OFF" << std::endl; };

    auto btnImport = static_cast<fk::ui::Button *>(mainWindow->FindName("btnImport"));
    btnImport->Click += []()
    { std::cout << "Action: Import" << std::endl; };

    auto btnExport = static_cast<fk::ui::Button *>(mainWindow->FindName("btnExport"));
    btnExport->Click += []()
    { std::cout << "Action: Export" << std::endl; };

    std::cout << "\n=== F__K UI Complex Layout Example ===" << std::endl;
    std::cout << "Features:" << std::endl;
    std::cout << "- Top navigation bar (3 buttons)" << std::endl;
    std::cout << "- Left sidebar (4 navigation buttons)" << std::endl;
    std::cout << "- Main content area (4 cards with buttons)" << std::endl;
    std::cout << "- Bottom status bar" << std::endl;
    std::cout << "======================================\n"
              << std::endl;

    app.Run(mainWindow);
    return 0;
}
