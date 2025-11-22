/**
 * @file path_line_test.cpp
 * @brief 测试 Path 中混合使用线条和曲线
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
    std::cout << "=== Path Line Test ===\n";

    auto application = std::make_unique<fk::Application>();

    auto window = std::make_shared<Window>();
    window->Title("Path Line Test")
          ->Width(800)
          ->Height(600)
          ->Background(new SolidColorBrush(Color::FromRGB(240, 240, 240, 255)));

    auto* panel = new StackPanel();
    panel->SetOrient(Orientation::Vertical)
         ->Spacing(20.0f)
         ->Margin(30.0f);

    // 标题
    auto* title = new TextBlock();
    title->Text("Path 混合线条测试")
         ->FontSize(20.0f)
         ->FontWeight(FontWeight::SemiBold);
    panel->AddChild(title);

    // 第一行
    auto* row1 = new StackPanel();
    row1->SetOrient(Orientation::Horizontal)->SetSpacing(30.0f);

    // 1. 纯线条路径
    auto* label1 = new TextBlock();
    label1->Text("纯线条(三角形)")->FontSize(14.0f)->Width(150);
    row1->AddChild(label1);
    
    auto* triangle = new Path();
    triangle->Width(150)->Height(120);
    triangle->MoveTo(75, 20)
            ->LineTo(130, 100)
            ->LineTo(20, 100)
            ->Close()
            ->Fill(nullptr)  // 不填充
            ->Stroke(new SolidColorBrush(Color::FromRGB(50, 100, 200, 255)))
            ->StrokeThickness(3.0f);
    row1->AddChild(triangle);

    // 2. 开放折线(不封闭)
    auto* label2 = new TextBlock();
    label2->Text("开放折线")->FontSize(14.0f)->Width(150);
    row1->AddChild(label2);
    
    auto* polyline = new Path();
    polyline->Width(150)->Height(120);
    polyline->MoveTo(20, 100)
            ->LineTo(40, 30)
            ->LineTo(75, 60)
            ->LineTo(110, 25)
            ->LineTo(130, 85)
            // 不 Close,保持开放
            ->Fill(nullptr)
            ->Stroke(new SolidColorBrush(Color::FromRGB(200, 100, 50, 255)))
            ->StrokeThickness(3.0f);
    row1->AddChild(polyline);

    panel->AddChild(row1);

    // 第二行
    auto* row2 = new StackPanel();
    row2->SetOrient(Orientation::Horizontal)->SetSpacing(30.0f);

    // 3. 贝塞尔曲线(不封闭)
    auto* label3 = new TextBlock();
    label3->Text("贝塞尔曲线")->FontSize(14.0f)->Width(150);
    row2->AddChild(label3);
    
    auto* bezier = new Path();
    bezier->Width(150)->Height(120);
    bezier->MoveTo(20, 100)
          ->CubicTo(Point(20, 20), Point(130, 20), Point(130, 100))
          // 不 Close
          ->Fill(nullptr)
          ->Stroke(new SolidColorBrush(Color::FromRGB(50, 150, 50, 255)))
          ->StrokeThickness(3.0f);
    row2->AddChild(bezier);

    // 4. 圆弧线(不封闭)
    auto* label4 = new TextBlock();
    label4->Text("圆弧线")->FontSize(14.0f)->Width(150);
    row2->AddChild(label4);
    
    auto* arcLine = new Path();
    arcLine->Width(150)->Height(120);
    arcLine->MoveTo(20, 60)
           ->ArcTo(Point(130, 60), 70, 50, 0, false, false)
           // 不 Close
           ->Fill(nullptr)
           ->Stroke(new SolidColorBrush(Color::FromRGB(150, 50, 150, 255)))
           ->StrokeThickness(3.0f);
    row2->AddChild(arcLine);

    panel->AddChild(row2);

    // 第三行
    auto* row3 = new StackPanel();
    row3->SetOrient(Orientation::Horizontal)->SetSpacing(30.0f);

    // 5. 混合曲线(线条+贝塞尔+圆弧)
    auto* label5 = new TextBlock();
    label5->Text("混合曲线")->FontSize(14.0f)->Width(150);
    row3->AddChild(label5);
    
    auto* mixed = new Path();
    mixed->Width(150)->Height(120);
    mixed->MoveTo(20, 60)
         ->LineTo(40, 30)
         ->QuadraticTo(Point(75, 20), Point(110, 40))
         ->ArcTo(Point(130, 80), 30, 20, 0, false, true)
         // 不 Close
         ->Fill(nullptr)
         ->Stroke(new SolidColorBrush(Color::FromRGB(200, 150, 0, 255)))
         ->StrokeThickness(3.0f);
    row3->AddChild(mixed);

    // 6. 心形(贝塞尔+线条)
    auto* label6 = new TextBlock();
    label6->Text("心形(曲线+线条)")->FontSize(14.0f)->Width(150);
    row3->AddChild(label6);
    
    auto* heart = new Path();
    heart->Width(150)->Height(120);
    heart->MoveTo(75, 100)
         ->LineTo(30, 60)
         ->CubicTo(Point(20, 45), Point(20, 30), Point(35, 25))
         ->CubicTo(Point(50, 20), Point(65, 30), Point(75, 45))
         ->CubicTo(Point(85, 30), Point(100, 20), Point(115, 25))
         ->CubicTo(Point(130, 30), Point(130, 45), Point(120, 60))
         ->Close()
         ->Fill(new SolidColorBrush(Color::FromRGB(255, 150, 150, 200)))
         ->Stroke(new SolidColorBrush(Color::FromRGB(200, 50, 50, 255)))
         ->StrokeThickness(2.0f);
    row3->AddChild(heart);

    panel->AddChild(row3);

    // 说明
    auto* desc = new TextBlock();
    desc->Text("测试 Path 中混合使用 MoveTo, LineTo, QuadraticTo, CubicTo, ArcTo\n"
               "所有形状都应该正确显示,填充和描边都应该正常")
        ->FontSize(12.0f)
        ->Foreground(new SolidColorBrush(Color::FromRGB(100, 100, 100, 255)));
    panel->AddChild(desc);

    window->Content(panel);
    application->Run(window);
    return 0;
}
