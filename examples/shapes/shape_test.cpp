/**
 * @file shape_test.cpp
 * @brief 简单的 Shape 测试 - 只测试基本渲染
 */

#include "fk/app/Application.h"
#include "fk/ui/Window.h"
#include "fk/ui/layouts/StackPanel.h"
#include "fk/ui/text/TextBlock.h"
#include "fk/ui/graphics/Shape.h"
#include "fk/ui/graphics/Brush.h"
#include <iostream>

using namespace fk::ui;

int main() {
    std::cout << "Shape Test - 测试基本渲染\n";

    auto application = std::make_unique<fk::Application>();

    // 创建主窗口
    auto window = std::make_shared<Window>();
    window->Title("Shape Test")
          ->Width(800)
          ->Height(600)
          ->Background(new SolidColorBrush(Color::FromRGB(240, 245, 255, 255)));

    // 创建布局
    auto* panel = new StackPanel();
    panel->SetOrient(Orientation::Vertical)
         ->Spacing(20.0f)
         ->Margin(20.0f);

    // 测试 Rectangle
    auto* rect = new Rectangle();
    rect->Width(200.0f)->Height(100.0f)
        ->Fill(new SolidColorBrush(Color::FromRGB(255, 0, 0, 255)))
        ->Stroke(new SolidColorBrush(Color::FromRGB(0, 0, 0, 255)))
        ->StrokeThickness(2.0f);
    panel->AddChild(rect);

    // 测试 Ellipse
    auto* ellipse = new Ellipse();
    ellipse->Width(100.0f)->Height(100.0f)
           ->Fill(new SolidColorBrush(Color::FromRGB(0, 255, 0, 255)))
           ->Stroke(new SolidColorBrush(Color::FromRGB(0, 0, 0, 255)))
           ->StrokeThickness(2.0f);
    panel->AddChild(ellipse);

    // 测试 Line
    auto* line = new Line();
    line->X1(0.0f)->Y1(0.0f)->X2(200.0f)->Y2(0.0f)
        ->Stroke(new SolidColorBrush(Color::FromRGB(0, 0, 255, 255)))
        ->StrokeThickness(3.0f);
    panel->AddChild(line);

    // 设置窗口内容
    window->Content(panel);

    // 运行应用
    application->Run(window);
    return 0;
}
