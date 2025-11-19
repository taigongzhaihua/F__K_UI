#include "fk/app/Application.h"
#include "fk/ui/Window.h"
#include "fk/ui/TextBlock.h"
#include "fk/ui/Brush.h"
#include "fk/ui/StackPanel.h"
#include "fk/ui/Button.h"
#include "fk/ui/Grid.h"
#include "fk/ui/GridCellAttacher.h"
#include "fk/core/Logger.h"

#include <iostream>

int main(int argc, char **argv)
{
    fk::Application app;
    auto mainWindow = std::make_shared<fk::ui::Window>();
    mainWindow->Title("F__K UI - 复杂布局示例")
        ->Width(1200)
        ->Height(800)
        ->Background(new fk::ui::SolidColorBrush(240, 245, 255))
        ->Content((new fk::ui::Grid())
            ->Rows("60, *, 40")  // 标题栏，内容区，状态栏
            ->Columns("200, *")  // 侧边栏，主内容区
            ->Children({
                // 顶部标题�?- 跨越两列
                (new fk::ui::Grid())
                    ->Background(new fk::ui::SolidColorBrush(60, 80, 120))
                    ->Columns("*, Auto, Auto, Auto")
                    ->Children({
                        (new fk::ui::TextBlock())
                            ->Text("F__K UI Dashboard")
                            ->FontSize(24)
                            ->Foreground(fk::ui::Brushes::White())
                            ->Margin(fk::Thickness(20, 0, 0, 0)) | fk::ui::cell(0, 0),
                        (new fk::ui::Button())
                            ->Name("btnHome")
                            ->Content((new fk::ui::TextBlock())
                                ->Text("首页")
                                ->FontSize(14)
                                ->Foreground(fk::ui::Brushes::White()))
                            ->Background(new fk::ui::SolidColorBrush(0, 0, 0, 0))
                            ->MouseOverBackground(fk::ui::Color::FromRGB(255, 255, 255, 100))
                            ->PressedBackground(fk::ui::Color::FromRGB(255, 255, 255, 150))
                            ->Margin(fk::Thickness(10)) | fk::ui::cell(0, 1),
                        (new fk::ui::Button())
                            ->Name("btnSettings")
                            ->Content((new fk::ui::TextBlock())
                                ->Text("设置")
                                ->FontSize(14)
                                ->Foreground(fk::ui::Brushes::White()))
                            ->Background(new fk::ui::SolidColorBrush(0, 0, 0, 0))
                            ->MouseOverBackground(fk::ui::Color::FromRGB(255, 255, 255, 100))
                            ->PressedBackground(fk::ui::Color::FromRGB(255, 255, 255, 150))
                            ->Margin(fk::Thickness(10)) | fk::ui::cell(0, 2),
                        (new fk::ui::Button())
                            ->Name("btnAbout")
                            ->Content((new fk::ui::TextBlock())
                                ->Text("关于")
                                ->FontSize(14)
                                ->Foreground(fk::ui::Brushes::White()))
                            ->Background(new fk::ui::SolidColorBrush(0, 0, 0, 0))
                            ->MouseOverBackground(fk::ui::Color::FromRGB(255, 255, 255, 100))
                            ->PressedBackground(fk::ui::Color::FromRGB(255, 255, 255, 150))
                            ->Margin(fk::Thickness(10, 10, 20, 10)) | fk::ui::cell(0, 3)
                    }) | fk::ui::cell(0, 0, 1, 2),
                
                // 左侧边栏
                (new fk::ui::StackPanel())
                    ->Background(new fk::ui::SolidColorBrush(250, 250, 250))
                    ->Children({
                        (new fk::ui::TextBlock())
                            ->Text("导航菜单")
                            ->FontSize(18)
                            ->Foreground(new fk::ui::SolidColorBrush(60, 80, 120))
                            ->Margin(fk::Thickness(15, 20, 15, 15)),
                        (new fk::ui::Button())
                            ->Name("navDashboard")
                            ->Content((new fk::ui::TextBlock())
                                ->Text("📊 仪表�?)
                                ->FontSize(14))
                            ->Background(new fk::ui::SolidColorBrush(100, 150, 255))
                            ->MouseOverBackground(fk::ui::Color::FromRGB(130, 180, 255))
                            ->PressedBackground(fk::ui::Color::FromRGB(80, 130, 230))
                            ->Margin(fk::Thickness(10, 5, 10, 5)),
                        (new fk::ui::Button())
                            ->Name("navProjects")
                            ->Content((new fk::ui::TextBlock())
                                ->Text("📁 项目")
                                ->FontSize(14))
                            ->Background(fk::ui::Brushes::White())
                            ->MouseOverBackground(fk::ui::Color::FromRGB(230, 240, 255))
                            ->PressedBackground(fk::ui::Color::FromRGB(200, 220, 255))
                            ->Margin(fk::Thickness(10, 5, 10, 5)),
                        (new fk::ui::Button())
                            ->Name("navTasks")
                            ->Content((new fk::ui::TextBlock())
                                ->Text("�?任务")
                                ->FontSize(14))
                            ->Background(fk::ui::Brushes::White())
                            ->MouseOverBackground(fk::ui::Color::FromRGB(230, 240, 255))
                            ->PressedBackground(fk::ui::Color::FromRGB(200, 220, 255))
                            ->Margin(fk::Thickness(10, 5, 10, 5)),
                        (new fk::ui::Button())
                            ->Name("navAnalytics")
                            ->Content((new fk::ui::TextBlock())
                                ->Text("📈 分析")
                                ->FontSize(14))
                            ->Background(fk::ui::Brushes::White())
                            ->MouseOverBackground(fk::ui::Color::FromRGB(230, 240, 255))
                            ->PressedBackground(fk::ui::Color::FromRGB(200, 220, 255))
                            ->Margin(fk::Thickness(10, 5, 10, 5))
                    }) | fk::ui::cell(1, 0),
                
                // 主内容区 - 包含多个卡片
                (new fk::ui::Grid())
                    ->Rows("*, *")
                    ->Columns("*, *")
                    ->Margin(fk::Thickness(20))
                    ->Children({
                        // 卡片1 - 欢迎信息
                        (new fk::ui::Grid())
                            ->Background(fk::ui::Brushes::White())
                            ->Rows("Auto, *")
                            ->Margin(fk::Thickness(0, 0, 10, 10))
                            ->Children({
                                (new fk::ui::TextBlock())
                                    ->Text("欢迎回来�?)
                                    ->FontSize(20)
                                    ->Foreground(new fk::ui::SolidColorBrush(60, 80, 120))
                                    ->Margin(fk::Thickness(15)) | fk::ui::cell(0, 0),
                                (new fk::ui::TextBlock())
                                    ->Text("这是一个展示复杂布局的示例\n包含多层嵌套的Grid和各种控�?)
                                    ->FontSize(14)
                                    ->Foreground(fk::ui::Brushes::DarkGray())
                                    ->Margin(fk::Thickness(15, 0, 15, 15)) | fk::ui::cell(1, 0)
                            }) | fk::ui::cell(0, 0),
                        
                        // 卡片2 - 统计信息
                        (new fk::ui::Grid())
                            ->Background(new fk::ui::SolidColorBrush(255, 250, 240))
                            ->Rows("Auto, *")
                            ->Margin(fk::Thickness(10, 0, 0, 10))
                            ->Children({
                                (new fk::ui::TextBlock())
                                    ->Text("项目统计")
                                    ->FontSize(20)
                                    ->Foreground(new fk::ui::SolidColorBrush(255, 140, 0))
                                    ->Margin(fk::Thickness(15)) | fk::ui::cell(0, 0),
                                (new fk::ui::StackPanel())
                                    ->Margin(fk::Thickness(15, 0, 15, 15))
                                    ->Children({
                                        (new fk::ui::TextBlock())
                                            ->Text("总项目数: 42")
                                            ->FontSize(14)
                                            ->Margin(fk::Thickness(0, 5, 0, 5)),
                                        (new fk::ui::TextBlock())
                                            ->Text("进行�? 15")
                                            ->FontSize(14)
                                            ->Margin(fk::Thickness(0, 5, 0, 5)),
                                        (new fk::ui::TextBlock())
                                            ->Text("已完�? 27")
                                            ->FontSize(14)
                                            ->Margin(fk::Thickness(0, 5, 0, 5))
                                    }) | fk::ui::cell(1, 0)
                            }) | fk::ui::cell(0, 1),
                        
                        // 卡片3 - 快速操�?
                        (new fk::ui::Grid())
                            ->Background(new fk::ui::SolidColorBrush(240, 255, 240))
                            ->Rows("Auto, *")
                            ->Margin(fk::Thickness(0, 10, 10, 0))
                            ->Children({
                                (new fk::ui::TextBlock())
                                    ->Text("快速操�?)
                                    ->FontSize(20)
                                    ->Foreground(new fk::ui::SolidColorBrush(34, 139, 34))
                                    ->Margin(fk::Thickness(15)) | fk::ui::cell(0, 0),
                                (new fk::ui::Grid())
                                    ->Rows("Auto, Auto")
                                    ->Columns("*, *")
                                    ->Margin(fk::Thickness(15, 0, 15, 15))
                                    ->Children({
                                        (new fk::ui::Button())
                                            ->Name("btnNewProject")
                                            ->Content((new fk::ui::TextBlock())
                                                ->Text("新建项目")
                                                ->FontSize(14))
                                            ->Background(new fk::ui::SolidColorBrush(50, 200, 50))
                                            ->MouseOverBackground(fk::ui::Color::FromRGB(70, 220, 70))
                                            ->PressedBackground(fk::ui::Color::FromRGB(30, 180, 30))
                                            ->Margin(fk::Thickness(5)) | fk::ui::cell(0, 0),
                                        (new fk::ui::Button())
                                            ->Name("btnNewTask")
                                            ->Content((new fk::ui::TextBlock())
                                                ->Text("新建任务")
                                                ->FontSize(14))
                                            ->Background(new fk::ui::SolidColorBrush(100, 180, 255))
                                            ->MouseOverBackground(fk::ui::Color::FromRGB(120, 200, 255))
                                            ->PressedBackground(fk::ui::Color::FromRGB(80, 160, 230))
                                            ->Margin(fk::Thickness(5)) | fk::ui::cell(0, 1),
                                        (new fk::ui::Button())
                                            ->Name("btnImport")
                                            ->Content((new fk::ui::TextBlock())
                                                ->Text("导入")
                                                ->FontSize(14))
                                            ->Background(new fk::ui::SolidColorBrush(255, 200, 100))
                                            ->MouseOverBackground(fk::ui::Color::FromRGB(255, 220, 120))
                                            ->PressedBackground(fk::ui::Color::FromRGB(230, 180, 80))
                                            ->Margin(fk::Thickness(5)) | fk::ui::cell(1, 0),
                                        (new fk::ui::Button())
                                            ->Name("btnExport")
                                            ->Content((new fk::ui::TextBlock())
                                                ->Text("导出")
                                                ->FontSize(14))
                                            ->Background(new fk::ui::SolidColorBrush(255, 150, 200))
                                            ->MouseOverBackground(fk::ui::Color::FromRGB(255, 170, 220))
                                            ->PressedBackground(fk::ui::Color::FromRGB(230, 130, 180))
                                            ->Margin(fk::Thickness(5)) | fk::ui::cell(1, 1)
                                    }) | fk::ui::cell(1, 0)
                            }) | fk::ui::cell(1, 0),
                        
                        // 卡片4 - 最近活�?
                        (new fk::ui::Grid())
                            ->Background(new fk::ui::SolidColorBrush(240, 248, 255))
                            ->Rows("Auto, *")
                            ->Margin(fk::Thickness(10, 10, 0, 0))
                            ->Children({
                                (new fk::ui::TextBlock())
                                    ->Text("最近活�?)
                                    ->FontSize(20)
                                    ->Foreground(new fk::ui::SolidColorBrush(70, 130, 180))
                                    ->Margin(fk::Thickness(15)) | fk::ui::cell(0, 0),
                                (new fk::ui::StackPanel())
                                    ->Margin(fk::Thickness(15, 0, 15, 15))
                                    ->Children({
                                        (new fk::ui::TextBlock())
                                            ->Text("�?更新�?UI 框架文档")
                                            ->FontSize(12)
                                            ->Foreground(fk::ui::Brushes::DarkGray())
                                            ->Margin(fk::Thickness(0, 3, 0, 3)),
                                        (new fk::ui::TextBlock())
                                            ->Text("�?完成了布局系统优化")
                                            ->FontSize(12)
                                            ->Foreground(fk::ui::Brushes::DarkGray())
                                            ->Margin(fk::Thickness(0, 3, 0, 3)),
                                        (new fk::ui::TextBlock())
                                            ->Text("�?新增了按钮视觉状�?)
                                            ->FontSize(12)
                                            ->Foreground(fk::ui::Brushes::DarkGray())
                                            ->Margin(fk::Thickness(0, 3, 0, 3)),
                                        (new fk::ui::TextBlock())
                                            ->Text("�?修复�?Grid 布局问题")
                                            ->FontSize(12)
                                            ->Foreground(fk::ui::Brushes::DarkGray())
                                            ->Margin(fk::Thickness(0, 3, 0, 3))
                                    }) | fk::ui::cell(1, 0)
                            }) | fk::ui::cell(1, 1)
                    }) | fk::ui::cell(1, 1),
                
                // 底部状态栏 - 跨越两列
                (new fk::ui::Grid())
                    ->Background(new fk::ui::SolidColorBrush(60, 80, 120))
                    ->Columns("Auto, *, Auto")
                    ->Children({
                        (new fk::ui::TextBlock())
                            ->Text("就绪")
                            ->FontSize(12)
                            ->Foreground(fk::ui::Brushes::White())
                            ->Margin(fk::Thickness(15, 0, 0, 0)) | fk::ui::cell(0, 0),
                        (new fk::ui::TextBlock())
                            ->Text("F__K UI v1.0 - 复杂布局演示")
                            ->FontSize(12)
                            ->Foreground(new fk::ui::SolidColorBrush(255, 255, 255, 200))
                            ->Margin(fk::Thickness(0)) | fk::ui::cell(0, 1),
                        (new fk::ui::TextBlock())
                            ->Text("2025�?1�?9�?)
                            ->FontSize(12)
                            ->Foreground(fk::ui::Brushes::White())
                            ->Margin(fk::Thickness(0, 0, 15, 0)) | fk::ui::cell(0, 2)
                    }) | fk::ui::cell(2, 0, 1, 2)
            }));

    // 绑定所有按钮的点击事件
    auto btnHome = static_cast<fk::ui::Button *>(mainWindow->FindName("btnHome"));
    btnHome->Click += []() { std::cout << "点击了：首页" << std::endl; };
    
    auto btnSettings = static_cast<fk::ui::Button *>(mainWindow->FindName("btnSettings"));
    btnSettings->Click += []() { std::cout << "点击了：设置" << std::endl; };
    
    auto btnAbout = static_cast<fk::ui::Button *>(mainWindow->FindName("btnAbout"));
    btnAbout->Click += []() { std::cout << "点击了：关于" << std::endl; };
    
    auto navDashboard = static_cast<fk::ui::Button *>(mainWindow->FindName("navDashboard"));
    navDashboard->Click += []() { std::cout << "导航到：仪表�? << std::endl; };
    
    auto navProjects = static_cast<fk::ui::Button *>(mainWindow->FindName("navProjects"));
    navProjects->Click += []() { std::cout << "导航到：项目" << std::endl; };
    
    auto navTasks = static_cast<fk::ui::Button *>(mainWindow->FindName("navTasks"));
    navTasks->Click += []() { std::cout << "导航到：任务" << std::endl; };
    
    auto navAnalytics = static_cast<fk::ui::Button *>(mainWindow->FindName("navAnalytics"));
    navAnalytics->Click += []() { std::cout << "导航到：分析" << std::endl; };
    
    auto btnNewProject = static_cast<fk::ui::Button *>(mainWindow->FindName("btnNewProject"));
    btnNewProject->Click += []() { std::cout << "执行：新建项�? << std::endl; };
    
    auto btnNewTask = static_cast<fk::ui::Button *>(mainWindow->FindName("btnNewTask"));
    btnNewTask->Click += []() { std::cout << "执行：新建任�? << std::endl; };
    
    auto btnImport = static_cast<fk::ui::Button *>(mainWindow->FindName("btnImport"));
    btnImport->Click += []() { std::cout << "执行：导�? << std::endl; };
    
    auto btnExport = static_cast<fk::ui::Button *>(mainWindow->FindName("btnExport"));
    btnExport->Click += []() { std::cout << "执行：导�? << std::endl; };
    
    std::cout << "\n=== F__K UI 复杂布局示例 ===" << std::endl;
    std::cout << "包含以下功能�? << std::endl;
    std::cout << "�?顶部导航栏（3个按钮）" << std::endl;
    std::cout << "�?左侧边栏�?个导航按钮）" << std::endl;
    std::cout << "�?主内容区�?个卡片，包含多个按钮�? << std::endl;
    std::cout << "�?底部状态栏" << std::endl;
    std::cout << "================================\n" << std::endl;
    
    app.Run(mainWindow);
    return 0;
}
