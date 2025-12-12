#include "fk/app/Application.h"
#include "fk/ui/Window.h"
#include "fk/ui/text/TextBlock.h"
#include "fk/ui/graphics/Brush.h"
#include "fk/ui/layouts/StackPanel.h"
#include "fk/ui/buttons/Button.h"
#include "fk/ui/buttons/ToggleButton.h"
#include "fk/ui/buttons/CheckBox.h"
#include "fk/ui/buttons/RadioButton.h"
#include "fk/ui/controls/Border.h"
#include "fk/ui/controls/Popup.h"
#include "fk/ui/layouts/Grid.h"
#include "fk/ui/layouts/GridCellAttacher.h"
#include "fk/ui/styling/Thickness.h"
#include "fk/core/Logger.h"

#include <iostream>
#include <memory>

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
                                ->Rows("*, *, Auto")
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
                                                          ->Text("Welcome Back! 👋😊")
                                                          ->FontSize(20)
                                                          ->Foreground(new fk::ui::SolidColorBrush(60, 80, 120))
                                                          ->Margin(fk::ui::Thickness(15)) |
                                                      fk::ui::cell(0, 0),
                                                  (new fk::ui::TextBlock())
                                                          ->Text("Emoji Test: 😀 😎 🚀 ❤️ 🌟 🎉\nSymbols: ★☆ ①②③ ▲▼◆ ✓✗")
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
                                         fk::ui::cell(1, 1),

                                     // Card 5 - Popup Demo (spans 2 columns)
                                     (new fk::ui::Grid())
                                             ->Background(new fk::ui::SolidColorBrush(255, 245, 250))
                                             ->CornerRadius(8)
                                             ->Rows("Auto, Auto, Auto")
                                             ->Margin(fk::ui::Thickness(0, 10, 0, 0))
                                             ->Children(
                                                 {(new fk::ui::TextBlock())
                                                          ->Text("Popup Demo 🎯")
                                                          ->FontSize(20)
                                                          ->Foreground(new fk::ui::SolidColorBrush(219, 39, 119))
                                                          ->Margin(fk::ui::Thickness(15)) |
                                                      fk::ui::cell(0, 0),
                                                  (new fk::ui::TextBlock())
                                                          ->Text("点击按钮显示不同类型的弹出窗口：")
                                                          ->FontSize(14)
                                                          ->Foreground(fk::ui::Brushes::DarkGray())
                                                          ->Margin(fk::ui::Thickness(15, 0, 15, 10)) |
                                                      fk::ui::cell(1, 0),
                                                  (new fk::ui::Grid())
                                                          ->Rows("Auto")
                                                          ->Columns("*, *, *")
                                                          ->Margin(fk::ui::Thickness(15, 0, 15, 15))
                                                          ->Children(
                                                              {(new fk::ui::Button())
                                                                       ->Name("btnPopupMenu")
                                                                       ->Content(
                                                                           (new fk::ui::TextBlock())
                                                                               ->Text("下拉菜单\n(Bottom)")
                                                                               ->FontSize(13))
                                                                       ->Background(new fk::ui::SolidColorBrush(147, 51, 234))
                                                                       ->MouseOverBackground(fk::ui::Color::FromRGB(167, 71, 254))
                                                                       ->PressedBackground(fk::ui::Color::FromRGB(127, 31, 214))
                                                                       ->Margin(fk::ui::Thickness(5))
                                                                       ->MinHeight(60) |
                                                                   fk::ui::cell(0, 0),
                                                               (new fk::ui::Button())
                                                                       ->Name("btnPopupTooltip")
                                                                       ->Content(
                                                                           (new fk::ui::TextBlock())
                                                                               ->Text("工具提示\n(Top)")
                                                                               ->FontSize(13))
                                                                       ->Background(new fk::ui::SolidColorBrush(59, 130, 246))
                                                                       ->MouseOverBackground(fk::ui::Color::FromRGB(79, 150, 255))
                                                                       ->PressedBackground(fk::ui::Color::FromRGB(39, 110, 226))
                                                                       ->Margin(fk::ui::Thickness(5))
                                                                       ->MinHeight(60) |
                                                                   fk::ui::cell(0, 1),
                                                               (new fk::ui::Button())
                                                                       ->Name("btnPopupNotify")
                                                                       ->Content(
                                                                           (new fk::ui::TextBlock())
                                                                               ->Text("通知弹窗\n(Right)")
                                                                               ->FontSize(13))
                                                                       ->Background(new fk::ui::SolidColorBrush(16, 185, 129))
                                                                       ->MouseOverBackground(fk::ui::Color::FromRGB(36, 205, 149))
                                                                       ->PressedBackground(fk::ui::Color::FromRGB(0, 165, 109))
                                                                       ->Margin(fk::ui::Thickness(5))
                                                                       ->MinHeight(60) |
                                                                   fk::ui::cell(0, 2)}) |
                                                      fk::ui::cell(2, 0)}) |
                                         fk::ui::cell(2, 0, 1, 2)}) |
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

    // ========== Popup Demo Setup ==========
    
    // 创建下拉菜单 Popup（复杂用户资料卡片）
    auto btnPopupMenu = static_cast<fk::ui::Button *>(mainWindow->FindName("btnPopupMenu"));
    auto popupMenu = std::make_shared<fk::ui::Popup>();
    popupMenu->SetPlacementTarget(btnPopupMenu);
    popupMenu->SetPlacement(fk::ui::PlacementMode::Bottom);
    popupMenu->SetStaysOpen(false);
    popupMenu->SetAllowsTransparency(true);
    popupMenu->SetWidth(320.0f);
    popupMenu->SetHeight(380.0f);
    
    // 创建用户资料卡片
    auto menuContent = new fk::ui::Grid();
    menuContent->Background(new fk::ui::SolidColorBrush(255, 255, 255))
               ->CornerRadius(12)
               ->Rows("Auto, Auto, *")
               ->Padding(0);
    
    // 头部区域
    auto header = new fk::ui::Grid();
    header->Background(new fk::ui::SolidColorBrush(100, 149, 237))
          ->CornerRadius(12, 12, 0, 0)
          ->Padding(20, 15, 20, 15)
          ->Rows("Auto, Auto, Auto");
    
    auto avatar = new fk::ui::Border();
    avatar->Background(new fk::ui::SolidColorBrush(255, 255, 255))
          ->CornerRadius(40)
          ->Width(80)->Height(80)
          ->SetHAlign(fk::ui::HorizontalAlignment::Center)
          ->Child((new fk::ui::TextBlock())->Text("👤")->FontSize(50)
                  ->SetHAlign(fk::ui::HorizontalAlignment::Center)
                  ->SetVAlign(fk::ui::VerticalAlignment::Center));
    header->AddChild(avatar | fk::ui::cell(0, 0));
    header->AddChild((new fk::ui::TextBlock())->Text("张三")->FontSize(20)
                     ->Foreground(fk::ui::Brushes::White())
                     ->SetHAlign(fk::ui::HorizontalAlignment::Center)
                     ->Margin(0, 10, 0, 5) | fk::ui::cell(1, 0));
    header->AddChild((new fk::ui::TextBlock())->Text("zhangsan@example.com")->FontSize(12)
                     ->Foreground(new fk::ui::SolidColorBrush(255, 255, 255, 200))
                     ->SetHAlign(fk::ui::HorizontalAlignment::Center) | fk::ui::cell(2, 0));
    menuContent->AddChild(header | fk::ui::cell(0, 0));
    
    // 快捷操作区
    auto quickActions = new fk::ui::Grid();
    quickActions->Columns("*, *")->Rows("Auto, Auto")->Padding(15, 15, 15, 10);
    quickActions->AddChild((new fk::ui::Button())
        ->Content((new fk::ui::TextBlock())->Text("📧 消息")->FontSize(13))
        ->Background(new fk::ui::SolidColorBrush(240, 248, 255))
        ->MouseOverBackground(fk::ui::Color::FromRGB(220, 238, 255))
        ->Padding(10, 8, 10, 8)->Margin(0, 0, 5, 5) | fk::ui::cell(0, 0));
    quickActions->AddChild((new fk::ui::Button())
        ->Content((new fk::ui::TextBlock())->Text("⚙️ 设置")->FontSize(13))
        ->Background(new fk::ui::SolidColorBrush(240, 248, 255))
        ->MouseOverBackground(fk::ui::Color::FromRGB(220, 238, 255))
        ->Padding(10, 8, 10, 8)->Margin(5, 0, 0, 5) | fk::ui::cell(0, 1));
    quickActions->AddChild((new fk::ui::Button())
        ->Content((new fk::ui::TextBlock())->Text("📊 统计")->FontSize(13))
        ->Background(new fk::ui::SolidColorBrush(240, 248, 255))
        ->MouseOverBackground(fk::ui::Color::FromRGB(220, 238, 255))
        ->Padding(10, 8, 10, 8)->Margin(0, 5, 5, 0) | fk::ui::cell(1, 0));
    quickActions->AddChild((new fk::ui::Button())
        ->Content((new fk::ui::TextBlock())->Text("❤️ 收藏")->FontSize(13))
        ->Background(new fk::ui::SolidColorBrush(240, 248, 255))
        ->MouseOverBackground(fk::ui::Color::FromRGB(220, 238, 255))
        ->Padding(10, 8, 10, 8)->Margin(5, 5, 0, 0) | fk::ui::cell(1, 1));
    menuContent->AddChild(quickActions | fk::ui::cell(1, 0));
    
    // 菜单列表区
    auto menuList = new fk::ui::StackPanel();
    menuList->Padding(10, 5, 10, 5);
    menuList->AddChild((new fk::ui::Button())
        ->Content((new fk::ui::TextBlock())->Text("👤 个人资料")->FontSize(14))
        ->Background(fk::ui::Brushes::White())
        ->MouseOverBackground(fk::ui::Color::FromRGB(245, 245, 250))
        ->Padding(15, 10, 15, 10)->Margin(5, 2, 5, 2));
    menuList->AddChild((new fk::ui::Button())
        ->Content((new fk::ui::TextBlock())->Text("🔔 通知设置")->FontSize(14))
        ->Background(fk::ui::Brushes::White())
        ->MouseOverBackground(fk::ui::Color::FromRGB(245, 245, 250))
        ->Padding(15, 10, 15, 10)->Margin(5, 2, 5, 2));
    menuList->AddChild((new fk::ui::Button())
        ->Content((new fk::ui::TextBlock())->Text("🔒 隐私安全")->FontSize(14))
        ->Background(fk::ui::Brushes::White())
        ->MouseOverBackground(fk::ui::Color::FromRGB(245, 245, 250))
        ->Padding(15, 10, 15, 10)->Margin(5, 2, 5, 2));
    menuList->AddChild((new fk::ui::Button())
        ->Content((new fk::ui::TextBlock())->Text("❓ 帮助中心")->FontSize(14))
        ->Background(fk::ui::Brushes::White())
        ->MouseOverBackground(fk::ui::Color::FromRGB(245, 245, 250))
        ->Padding(15, 10, 15, 10)->Margin(5, 2, 5, 2));
    menuContent->AddChild(menuList | fk::ui::cell(2, 0));
    popupMenu->SetChild(menuContent);
    
    btnPopupMenu->Click += [popupMenu]() {
        popupMenu->SetIsOpen(!popupMenu->GetIsOpen());
        std::cout << "Popup Menu: " << (popupMenu->GetIsOpen() ? "Opened" : "Closed") << std::endl;
    };
    
    // 创建工具提示 Popup
    auto btnPopupTooltip = static_cast<fk::ui::Button *>(mainWindow->FindName("btnPopupTooltip"));
    auto popupTooltip = std::make_shared<fk::ui::Popup>();
    popupTooltip->SetPlacementTarget(btnPopupTooltip);
    popupTooltip->SetPlacement(fk::ui::PlacementMode::Top);
    popupTooltip->SetStaysOpen(false);
    popupTooltip->SetAllowsTransparency(true);
    popupTooltip->SetWidth(220.0f);
    popupTooltip->SetHeight(80.0f);
    popupTooltip->SetVerticalOffset(-5.0f);
    
    auto tooltipContent = new fk::ui::Border();
    tooltipContent->Background(new fk::ui::SolidColorBrush(40, 40, 40))
                  ->CornerRadius(4)
                  ->Child(
                      (new fk::ui::TextBlock())
                          ->Text("💡 这是一个工具提示\n显示在按钮上方")
                          ->FontSize(12)
                          ->Foreground(fk::ui::Brushes::White())
                          ->Margin(fk::ui::Thickness(10))
                  );
    popupTooltip->SetChild(tooltipContent);
    
    btnPopupTooltip->Click += [popupTooltip]() {
        popupTooltip->SetIsOpen(!popupTooltip->GetIsOpen());
        std::cout << "Popup Tooltip: " << (popupTooltip->GetIsOpen() ? "Opened" : "Closed") << std::endl;
    };
    
    // 创建通知弹窗 Popup（多功能通知面板）
    auto btnPopupNotify = static_cast<fk::ui::Button *>(mainWindow->FindName("btnPopupNotify"));
    auto popupNotify = std::make_shared<fk::ui::Popup>();
    popupNotify->SetPlacementTarget(btnPopupNotify);
    popupNotify->SetPlacement(fk::ui::PlacementMode::Right);
    popupNotify->SetStaysOpen(false);
    popupNotify->SetAllowsTransparency(true);
    popupNotify->SetWidth(350.0f);
    popupNotify->SetHeight(450.0f);
    popupNotify->SetHorizontalOffset(10.0f);
    
    auto notifyContent = new fk::ui::Grid();
    notifyContent->Background(new fk::ui::SolidColorBrush(255, 255, 255))
                 ->CornerRadius(10)
                 ->Rows("Auto, *");
    
    // 标题栏
    auto titleBar = new fk::ui::Grid();
    titleBar->Background(new fk::ui::LinearGradientBrush(
                fk::ui::Color::FromRGB(106, 90, 205),
                fk::ui::Color::FromRGB(138, 43, 226)))
            ->CornerRadius(10, 10, 0, 0)
            ->Padding(15, 12, 15, 12)
            ->Columns("*, Auto");
    titleBar->AddChild((new fk::ui::TextBlock())
        ->Text("🔔 通知中心")->FontSize(16)
        ->Foreground(fk::ui::Brushes::White()) | fk::ui::cell(0, 0));
    titleBar->AddChild((new fk::ui::TextBlock())
        ->Text("3 条未读")->FontSize(12)
        ->Foreground(new fk::ui::SolidColorBrush(255, 255, 255, 180)) | fk::ui::cell(0, 1));
    notifyContent->AddChild(titleBar | fk::ui::cell(0, 0));
    
    // 通知列表
    auto notifyList = new fk::ui::StackPanel();
    notifyList->Padding(10);
    
    // 成功通知
    auto successNotify = new fk::ui::Border();
    successNotify->Background(new fk::ui::SolidColorBrush(212, 237, 218))
                 ->CornerRadius(8)->Padding(12)->Margin(5, 5, 5, 5);
    auto successGrid = new fk::ui::Grid();
    successGrid->Rows("Auto, Auto, Auto");
    successGrid->AddChild((new fk::ui::TextBlock())
        ->Text("✅ 文件保存成功")->FontSize(14)
        ->Foreground(new fk::ui::SolidColorBrush(25, 135, 84)) | fk::ui::cell(0, 0));
    successGrid->AddChild((new fk::ui::TextBlock())
        ->Text("document.txt 已成功保存到云端")->FontSize(12)
        ->Foreground(new fk::ui::SolidColorBrush(40, 70, 50))
        ->Margin(0, 5, 0, 5) | fk::ui::cell(1, 0));
    successGrid->AddChild((new fk::ui::TextBlock())
        ->Text("刚刚")->FontSize(11)
        ->Foreground(new fk::ui::SolidColorBrush(108, 117, 125)) | fk::ui::cell(2, 0));
    successNotify->Child(successGrid);
    notifyList->AddChild(successNotify);
    
    // 警告通知
    auto warningNotify = new fk::ui::Border();
    warningNotify->Background(new fk::ui::SolidColorBrush(255, 243, 205))
                 ->CornerRadius(8)->Padding(12)->Margin(5, 5, 5, 5);
    auto warningGrid = new fk::ui::Grid();
    warningGrid->Rows("Auto, Auto, Auto");
    warningGrid->AddChild((new fk::ui::TextBlock())
        ->Text("⚠️ 存储空间不足")->FontSize(14)
        ->Foreground(new fk::ui::SolidColorBrush(255, 193, 7)) | fk::ui::cell(0, 0));
    warningGrid->AddChild((new fk::ui::TextBlock())
        ->Text("仅剩 15% 可用空间，建议清理文件")->FontSize(12)
        ->Foreground(new fk::ui::SolidColorBrush(120, 80, 0))
        ->Margin(0, 5, 0, 5) | fk::ui::cell(1, 0));
    warningGrid->AddChild((new fk::ui::TextBlock())
        ->Text("5 分钟前")->FontSize(11)
        ->Foreground(new fk::ui::SolidColorBrush(108, 117, 125)) | fk::ui::cell(2, 0));
    warningNotify->Child(warningGrid);
    notifyList->AddChild(warningNotify);
    
    // 信息通知
    auto infoNotify = new fk::ui::Border();
    infoNotify->Background(new fk::ui::SolidColorBrush(209, 236, 241))
              ->CornerRadius(8)->Padding(12)->Margin(5, 5, 5, 5);
    auto infoGrid = new fk::ui::Grid();
    infoGrid->Rows("Auto, Auto, Auto");
    infoGrid->AddChild((new fk::ui::TextBlock())
        ->Text("ℹ️ 系统更新可用")->FontSize(14)
        ->Foreground(new fk::ui::SolidColorBrush(13, 110, 253)) | fk::ui::cell(0, 0));
    infoGrid->AddChild((new fk::ui::TextBlock())
        ->Text("发现新版本 v2.1.0，包含性能优化")->FontSize(12)
        ->Foreground(new fk::ui::SolidColorBrush(0, 50, 100))
        ->Margin(0, 5, 0, 5) | fk::ui::cell(1, 0));
    infoGrid->AddChild((new fk::ui::TextBlock())
        ->Text("1 小时前")->FontSize(11)
        ->Foreground(new fk::ui::SolidColorBrush(108, 117, 125)) | fk::ui::cell(2, 0));
    infoNotify->Child(infoGrid);
    notifyList->AddChild(infoNotify);
    
    // 操作按钮
    auto actionButtons = new fk::ui::Grid();
    actionButtons->Columns("*, *")->Margin(5, 10, 5, 5);
    actionButtons->AddChild((new fk::ui::Button())
        ->Content((new fk::ui::TextBlock())->Text("全部标记已读")->FontSize(13))
        ->Background(new fk::ui::SolidColorBrush(108, 117, 125))
        ->MouseOverBackground(fk::ui::Color::FromRGB(90, 98, 104))
        ->Foreground(fk::ui::Brushes::White())
        ->Padding(10, 8, 10, 8)->Margin(0, 0, 5, 0) | fk::ui::cell(0, 0));
    actionButtons->AddChild((new fk::ui::Button())
        ->Content((new fk::ui::TextBlock())->Text("清空通知")->FontSize(13))
        ->Background(new fk::ui::SolidColorBrush(220, 53, 69))
        ->MouseOverBackground(fk::ui::Color::FromRGB(200, 35, 51))
        ->Foreground(fk::ui::Brushes::White())
        ->Padding(10, 8, 10, 8)->Margin(5, 0, 0, 0) | fk::ui::cell(0, 1));
    notifyList->AddChild(actionButtons);
    
    notifyContent->AddChild(notifyList | fk::ui::cell(1, 0));
    popupNotify->SetChild(notifyContent);
    
    btnPopupNotify->Click += [popupNotify]() {
        popupNotify->SetIsOpen(!popupNotify->GetIsOpen());
        std::cout << "Popup Notify: " << (popupNotify->GetIsOpen() ? "Opened" : "Closed") << std::endl;
    };
    
    // 获取 toggle buttons 并绑定事件
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
    std::cout << "- Main content area (5 cards with demos)" << std::endl;
    std::cout << "- Popup Demo: 下拉菜单、工具提示、通知弹窗" << std::endl;
    std::cout << "- Bottom status bar" << std::endl;
    std::cout << "\nPopup Features:" << std::endl;
    std::cout << "✓ 智能边界处理 - 自动翻转避免超出屏幕" << std::endl;
    std::cout << "✓ 淡入淡出动画 - 平滑的视觉效果" << std::endl;
    std::cout << "✓ 外部点击关闭 - StaysOpen=false" << std::endl;
    std::cout << "✓ 透明背景支持 - AllowsTransparency=true" << std::endl;
    std::cout << "======================================\n"
              << std::endl;

    app.Run(mainWindow);
    return 0;
}
