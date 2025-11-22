/**
 * @file multicolor_simple_test.cpp  
 * @brief Path分段着色简单测试
 */

#include "fk/app/Application.h"
#include "fk/ui/Window.h"
#include "fk/ui/StackPanel.h"
#include "fk/ui/TextBlock.h"
#include "fk/ui/Shape.h"
#include "fk/ui/Brush.h"
#include "fk/render/DrawCommand.h"
#include <iostream>

using namespace fk::ui;
using Color = fk::render::Color;

int main() {
    std::cout << "=== Multicolor Path Simple Test ===\n";

    auto application = std::make_unique<fk::Application>();

    auto window = std::make_shared<Window>();
    window->Title("Multicolor Path Test")
          ->Width(600)
          ->Height(400)
          ->Background(new SolidColorBrush(Color::FromRGB(240, 240, 240, 255)));

    auto* panel = new StackPanel();
    panel->SetOrient(Orientation::Vertical)
         ->Spacing(20.0f)
         ->Margin(30.0f);
    window->Content(panel);

    // 标题
    auto* title = new TextBlock();
    title->Text("Path分段着色测试")->FontSize(20.0f);
    panel->AddChild(title);

    // 红-绿-蓝三段直线
    auto* path1 = new Path();
    path1->Width(400)->Height(100);
    path1->MoveTo(10, 50)
         ->LineTo(140, 50)->SetSegmentStroke(1.0f, 0.0f, 0.0f) // 红
         ->LineTo(270, 50)->SetSegmentStroke(0.0f, 1.0f, 0.0f) // 绿
         ->LineTo(390, 50)->SetSegmentStroke(0.0f, 0.0f, 1.0f) // 蓝
         ->Fill(nullptr)
         ->StrokeThickness(5.0f);
    panel->AddChild(path1);

    // 彩虹折线
    auto* path2 = new Path();
    path2->Width(400)->Height(120);
    path2->MoveTo(10, 100)
         ->LineTo(90, 20)->SetSegmentStroke(1.0f, 0.0f, 0.0f)    // 红
         ->LineTo(170, 80)->SetSegmentStroke(1.0f, 0.5f, 0.0f)   // 橙
         ->LineTo(250, 30)->SetSegmentStroke(1.0f, 1.0f, 0.0f)   // 黄
         ->LineTo(330, 90)->SetSegmentStroke(0.0f, 1.0f, 0.0f)   // 绿
         ->LineTo(390, 50)->SetSegmentStroke(0.0f, 0.0f, 1.0f)   // 蓝
         ->Fill(nullptr)
         ->StrokeThickness(4.0f);
    panel->AddChild(path2);

    application->Run(window);
    return 0;
}
