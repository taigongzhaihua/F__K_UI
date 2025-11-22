/**
 * @file sweep_test.cpp
 * @brief 测试 sweep 标志的效果
 */

#include "fk/app/Application.h"
#include "fk/ui/Window.h"
#include "fk/ui/StackPanel.h"
#include "fk/ui/TextBlock.h"
#include "fk/ui/Shape.h"
#include "fk/ui/Brush.h"
#include <iostream>

using namespace fk::ui;

int main() {
    std::cout << "=== Sweep Flag Test ===\n";

    auto application = std::make_unique<fk::Application>();

    auto window = std::make_shared<Window>();
    window->Title("Sweep Flag Test")
          ->Width(700)
          ->Height(500)
          ->Background(new SolidColorBrush(Color::FromRGB(240, 240, 240, 255)));

    auto* panel = new StackPanel();
    panel->SetOrient(Orientation::Vertical)
         ->Spacing(30.0f)
         ->Margin(30.0f);

    // 标题
    auto* title = new TextBlock();
    title->Text("Sweep 标志测试 - 相同起点终点,只改变 sweep")
         ->FontSize(20.0f)
         ->FontWeight(FontWeight::SemiBold);
    panel->AddChild(title);

    // 第一行: sweep=false (逆时针)
    auto* row1 = new StackPanel();
    row1->SetOrient(Orientation::Horizontal)->SetSpacing(20.0f);
    
    auto* label1 = new TextBlock();
    label1->Text("sweep=false (逆时针):")->FontSize(16.0f)->Width(200);
    row1->AddChild(label1);
    
    auto* arc1 = new Path();
    arc1->Width(250)->Height(150);
    arc1->MoveTo(50, 75)
       ->ArcTo(Point(200, 75), 75, 75, 0, false, false)
       ->Fill(new SolidColorBrush(Color::FromRGB(255, 200, 200, 200)))
       ->Stroke(new SolidColorBrush(Color::FromRGB(255, 0, 0, 255)))
       ->StrokeThickness(3.0f);
    row1->AddChild(arc1);
    
    panel->AddChild(row1);

    // 第二行: sweep=true (顺时针)
    auto* row2 = new StackPanel();
    row2->SetOrient(Orientation::Horizontal)->SetSpacing(20.0f);
    
    auto* label2 = new TextBlock();
    label2->Text("sweep=true (顺时针):")->FontSize(16.0f)->Width(200);
    row2->AddChild(label2);
    
    auto* arc2 = new Path();
    arc2->Width(250)->Height(150);
    arc2->MoveTo(50, 75)
       ->ArcTo(Point(200, 75), 75, 75, 0, false, true)
       ->Fill(new SolidColorBrush(Color::FromRGB(200, 255, 200, 200)))
       ->Stroke(new SolidColorBrush(Color::FromRGB(0, 200, 0, 255)))
       ->StrokeThickness(3.0f);
    row2->AddChild(arc2);
    
    panel->AddChild(row2);

    // 说明
    auto* desc = new TextBlock();
    desc->Text("起点: (50, 75), 终点: (200, 75), 半径: 75\n"
               "sweep=false 应该向上凸出\n"
               "sweep=true 应该向下凸出\n"
               "如果两个方向相同,说明 sweep 实现有问题")
        ->FontSize(14.0f)
        ->Foreground(new SolidColorBrush(Color::FromRGB(100, 100, 100, 255)));
    panel->AddChild(desc);

    window->Content(panel);
    application->Run(window);
    return 0;
}
