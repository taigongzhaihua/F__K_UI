#include "fk/app/Application.h"
#include "fk/ui/Window.h"
#include "fk/ui/TextBlock.h"
#include "fk/ui/Brush.h"
#include "fk/ui/StackPanel.h"
#include "fk/ui/Button.h"
#include "fk/ui/Border.h"
#include "fk/ui/Grid.h"
#include "fk/ui/GridCellAttacher.h"
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
                    {(new fk::ui::Border())
                             ->Background(new fk::ui::SolidColorBrush(60, 80, 120))
                             ->Child(
                                 (new fk::ui::Grid())
                                     ->Columns("*, Auto, Auto, Auto")
                                     ->Children(
                                         {(new fk::ui::TextBlock())
                                                  ->Text("F__K UI Dashboard")
                                                  ->FontSize(24)->SetVAlign(fk::ui::VerticalAlignment::Center)
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
                                                  ->MouseOverBackground(fk::ui::Color::FromRGB(120, 130, 255, 100))
                                                  ->PressedBackground(fk::ui::Color::FromRGB(120, 130, 255, 150))
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
                                                  ->MouseOverBackground(fk::ui::Color::FromRGB(120, 130, 255, 100))
                                                  ->PressedBackground(fk::ui::Color::FromRGB(120, 130, 255, 150))
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
                                                  ->MouseOverBackground(fk::ui::Color::FromRGB(120, 130, 255, 100))
                                                  ->PressedBackground(fk::ui::Color::FromRGB(120, 130, 255, 150))
                                                  ->Margin(fk::ui::Thickness(10, 10, 20, 10)) |
                                              fk::ui::cell(0, 3)})) |
                         fk::ui::cell(0, 0, 1, 2),

                     // Left sidebar
                     (new fk::ui::Border())
                             ->Background(new fk::ui::SolidColorBrush(250, 250, 250))
                             ->Child(
                                 (new fk::ui::StackPanel())
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
                                              ->Margin(fk::ui::Thickness(10, 5, 10, 5))})) |
                         fk::ui::cell(1, 0),

                     // Main content area - contains multiple cards
                     (new fk::ui::Grid())
                             ->Rows("*, *")
                             ->Columns("*, *")
                             ->Margin(fk::ui::Thickness(20))
                             ->Children( // Card 1 - Welcome
                                 {(new fk::ui::Grid())
                                          ->Background(fk::ui::Brushes::White())
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

                                  // Card 2 - Statistics
                                  (new fk::ui::Grid())
                                          ->Background(new fk::ui::SolidColorBrush(255, 250, 240))
                                          ->Rows("Auto, *")
                                          ->Margin(fk::ui::Thickness(10, 0, 0, 10))
                                          ->Children(
                                              {(new fk::ui::TextBlock())
                                                       ->Text("Project Stats")
                                                       ->FontSize(20)
                                                       ->Foreground(new fk::ui::SolidColorBrush(255, 140, 0))
                                                       ->Margin(fk::ui::Thickness(15)) |
                                                   fk::ui::cell(0, 0),
                                               (new fk::ui::StackPanel())
                                                       ->Margin(fk::ui::Thickness(15, 0, 15, 15))
                                                       ->Children(
                                                           {(new fk::ui::TextBlock())
                                                                ->Text("Total Projects: 42")
                                                                ->FontSize(14)
                                                                ->Margin(fk::ui::Thickness(0, 5, 0, 5)),
                                                            (new fk::ui::TextBlock())
                                                                ->Text("In Progress: 15")
                                                                ->FontSize(14)
                                                                ->Margin(fk::ui::Thickness(0, 5, 0, 5)),
                                                            (new fk::ui::TextBlock())
                                                                ->Text("Completed: 27")
                                                                ->FontSize(14)
                                                                ->Margin(fk::ui::Thickness(0, 5, 0, 5))}) |
                                                   fk::ui::cell(1, 0)}) |
                                      fk::ui::cell(0, 1),

                                  // Card 3 - Quick Actions
                                  (new fk::ui::Grid())
                                          ->Background(new fk::ui::SolidColorBrush(240, 255, 240))
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
                                          ->Text("Ready")
                                          ->FontSize(12)
                                          ->Foreground(fk::ui::Brushes::White())
                                          ->Margin(fk::ui::Thickness(15, 0, 0, 0)) |
                                      fk::ui::cell(0, 0),
                                  (new fk::ui::TextBlock())
                                          ->Text("F__K UI v1.0 - Complex Layout Demo")
                                          ->FontSize(12)
                                          ->Foreground(new fk::ui::SolidColorBrush(255, 255, 255, 200))
                                          ->Margin(fk::ui::Thickness(0)) |
                                      fk::ui::cell(0, 1),
                                  (new fk::ui::TextBlock())
                                          ->Text("Nov 19, 2025")
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
