/**
 * @file simple_arc_test.cpp
 * @brief 简单的圆弧测试 - 验证圆弧坐标计算
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
    std::cout << "=== 简单圆弧测试 ===\n";

    auto application = std::make_unique<fk::Application>();

    auto window = std::make_shared<Window>();
    window->Title("Simple Arc Test")
          ->Width(600)
          ->Height(400)
          ->Background(new SolidColorBrush(Color::FromRGB(240, 240, 240, 255)));

    auto* panel = new StackPanel();
    panel->SetOrient(Orientation::Vertical)
         ->Spacing(20.0f)
         ->Margin(30.0f);

    // 标题
    auto* title = new TextBlock();
    title->Text("圆弧坐标测试")->FontSize(20.0f);
    panel->AddChild(title);

    // 第一行:贝塞尔曲线(参考)
    auto* row1 = new StackPanel();
    row1->SetOrient(Orientation::Horizontal)->SetSpacing(20.0f);
    
    auto* label1 = new TextBlock();
    label1->Text("二次贝塞尔(参考):")->FontSize(14.0f)->Width(150);
    row1->AddChild(label1);
    
    auto* bezier = new Path();
    bezier->Width(200)->Height(100);
    bezier->MoveTo(10, 80)
          ->QuadraticTo(Point(50, 10), Point(90, 80))
          ->Fill(new SolidColorBrush(Color::FromRGB(200, 220, 255, 255)))
          ->Stroke(new SolidColorBrush(Color::FromRGB(0, 0, 255, 255)))
          ->StrokeThickness(2.0f);
    row1->AddChild(bezier);
    
    panel->AddChild(row1);

    // 第二行:简单圆弧
    auto* row2 = new StackPanel();
    row2->SetOrient(Orientation::Horizontal)->SetSpacing(20.0f);
    
    auto* label2 = new TextBlock();
    label2->Text("简单圆弧:")->FontSize(14.0f)->Width(150);
    row2->AddChild(label2);
    
    auto* arc = new Path();
    arc->Width(200)->Height(100);
    arc->MoveTo(10, 80)
       ->ArcTo(Point(90, 80), 40, 40, 0, false, true)
       ->Fill(new SolidColorBrush(Color::FromRGB(255, 220, 220, 255)))
       ->Stroke(new SolidColorBrush(Color::FromRGB(255, 0, 0, 255)))
       ->StrokeThickness(2.0f);
    row2->AddChild(arc);
    
    panel->AddChild(row2);

    // 第三行:直线(参考)
    auto* row3 = new StackPanel();
    row3->SetOrient(Orientation::Horizontal)->SetSpacing(20.0f);
    
    auto* label3 = new TextBlock();
    label3->Text("直线(参考):")->FontSize(14.0f)->Width(150);
    row3->AddChild(label3);
    
    auto* line = new Path();
    line->Width(200)->Height(100);
    line->MoveTo(10, 80)
        ->LineTo(Point(90, 80))
        ->Stroke(new SolidColorBrush(Color::FromRGB(100, 100, 100, 255)))
        ->StrokeThickness(2.0f);
    row3->AddChild(line);
    
    panel->AddChild(row3);

    // 说明
    auto* desc = new TextBlock();
    desc->Text("如果圆弧正常,应该看到一个向下的弧形,类似贝塞尔曲线\n"
               "如果是满屏大椭圆,说明圆弧坐标计算有误")
        ->FontSize(12.0f)
        ->Foreground(new SolidColorBrush(Color::FromRGB(100, 100, 100, 255)));
    panel->AddChild(desc);

    window->Content(panel);
    application->Run(window);
    return 0;
}
