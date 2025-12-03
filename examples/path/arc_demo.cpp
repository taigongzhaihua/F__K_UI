/**
 * @file arc_demo.cpp
 * @brief 圆弧演示 - 展示 ArcTo 的各种参数效果
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
    std::cout << "=== 圆弧参数演示 ===\n";

    auto application = std::make_unique<fk::Application>();

    auto window = std::make_shared<Window>();
    window->Title("Arc Demo - 圆弧参数演示")
          ->Width(900)
          ->Height(700)
          ->Background(new SolidColorBrush(Color::FromRGB(245, 245, 250, 255)));

    auto* panel = new StackPanel();
    panel->SetOrient(Orientation::Vertical)
         ->Spacing(20.0f)
         ->Margin(20.0f);

    // 标题
    auto* title = new TextBlock();
    title->Text("ArcTo 参数演示 - largeArc & sweep")
         ->FontSize(24.0f)
         ->Foreground(new SolidColorBrush(Color::FromRGB(0, 0, 0, 255)));
    panel->AddChild(title);

    // 说明文字
    auto* desc = new TextBlock();
    desc->Text("两个标志位决定了4种不同的圆弧路径")
        ->FontSize(14.0f)
        ->Foreground(new SolidColorBrush(Color::FromRGB(100, 100, 100, 255)));
    panel->AddChild(desc);

    // 第一行: largeArc=false 的两种情况
    auto* row1 = new StackPanel();
    row1->SetOrient(Orientation::Horizontal)->SetSpacing(40.0f);

    // largeArc=false, sweep=false (小弧, 逆时针)
    auto* arc1Container = new StackPanel();
    arc1Container->SetOrient(Orientation::Vertical)->SetSpacing(10.0f);
    auto* arc1Label = new TextBlock();
    arc1Label->Text("largeArc=false, sweep=false")
             ->FontSize(14.0f)
             ->FontWeight(FontWeight::SemiBold);
    arc1Container->AddChild(arc1Label);
    auto* arc1Desc = new TextBlock();
    arc1Desc->Text("(小弧, 向上凸)")->FontSize(12.0f)
            ->Foreground(new SolidColorBrush(Color::FromRGB(100, 100, 100, 255)));
    arc1Container->AddChild(arc1Desc);
    
    auto* arc1 = new Path();
    arc1->Width(160)->Height(120);
    arc1->MoveTo(30, 90)
        ->ArcTo(Point(110, 30), 60, 60, 0, false, false)
        ->Fill(new SolidColorBrush(Color::FromRGB(255, 200, 200, 200)))
        ->Stroke(new SolidColorBrush(Color::FromRGB(255, 0, 0, 255)))
        ->StrokeThickness(3.0f);
    arc1Container->AddChild(arc1);
    row1->AddChild(arc1Container);

    // largeArc=false, sweep=true (小弧, 顺时针)
    auto* arc2Container = new StackPanel();
    arc2Container->SetOrient(Orientation::Vertical)->SetSpacing(10.0f);
    auto* arc2Label = new TextBlock();
    arc2Label->Text("largeArc=false, sweep=true")
             ->FontSize(14.0f)
             ->FontWeight(FontWeight::SemiBold);
    arc2Container->AddChild(arc2Label);
    auto* arc2Desc = new TextBlock();
    arc2Desc->Text("(小弧, 向下凸)")->FontSize(12.0f)
            ->Foreground(new SolidColorBrush(Color::FromRGB(100, 100, 100, 255)));
    arc2Container->AddChild(arc2Desc);
    
    auto* arc2 = new Path();
    arc2->Width(160)->Height(120);
    arc2->MoveTo(30, 40)
        ->ArcTo(Point(130, 40), 50, 50, 0, false, true)
        ->Fill(new SolidColorBrush(Color::FromRGB(200, 255, 200, 200)))
        ->Stroke(new SolidColorBrush(Color::FromRGB(0, 200, 0, 255)))
        ->StrokeThickness(3.0f);
    arc2Container->AddChild(arc2);
    row1->AddChild(arc2Container);

    panel->AddChild(row1);

    // 第二行: largeArc=true 的两种情况
    auto* row2 = new StackPanel();
    row2->SetOrient(Orientation::Horizontal)->SetSpacing(40.0f);

    // largeArc=true, sweep=false (大弧, 逆时针)
    auto* arc3Container = new StackPanel();
    arc3Container->SetOrient(Orientation::Vertical)->SetSpacing(10.0f);
    auto* arc3Label = new TextBlock();
    arc3Label->Text("largeArc=true, sweep=false")
             ->FontSize(14.0f)
             ->FontWeight(FontWeight::SemiBold);
    arc3Container->AddChild(arc3Label);
    auto* arc3Desc = new TextBlock();
    arc3Desc->Text("(大弧, 270°)")->FontSize(12.0f)
            ->Foreground(new SolidColorBrush(Color::FromRGB(100, 100, 100, 255)));
    arc3Container->AddChild(arc3Desc);
    
    auto* arc3 = new Path();
    arc3->Width(160)->Height(120);
    arc3->MoveTo(50, 60)
        ->ArcTo(Point(110, 60), 50, 50, 0, true, false)
        ->Fill(new SolidColorBrush(Color::FromRGB(200, 200, 255, 200)))
        ->Stroke(new SolidColorBrush(Color::FromRGB(0, 0, 255, 255)))
        ->StrokeThickness(3.0f);
    arc3Container->AddChild(arc3);
    row2->AddChild(arc3Container);

    // largeArc=true, sweep=true (大弧, 顺时针)
    auto* arc4Container = new StackPanel();
    arc4Container->SetOrient(Orientation::Vertical)->SetSpacing(10.0f);
    auto* arc4Label = new TextBlock();
    arc4Label->Text("largeArc=true, sweep=true")
             ->FontSize(14.0f)
             ->FontWeight(FontWeight::SemiBold);
    arc4Container->AddChild(arc4Label);
    auto* arc4Desc = new TextBlock();
    arc4Desc->Text("(椭圆弧, 向上)")->FontSize(12.0f)
            ->Foreground(new SolidColorBrush(Color::FromRGB(100, 100, 100, 255)));
    arc4Container->AddChild(arc4Desc);
    
    auto* arc4 = new Path();
    arc4->Width(160)->Height(120);
    arc4->MoveTo(30, 40)
        ->ArcTo(Point(130, 40), 80, 50, 0, false, false)
        ->Fill(new SolidColorBrush(Color::FromRGB(255, 255, 200, 200)))
        ->Stroke(new SolidColorBrush(Color::FromRGB(200, 150, 0, 255)))
        ->StrokeThickness(3.0f);
    arc4Container->AddChild(arc4);
    row2->AddChild(arc4Container);

    panel->AddChild(row2);

    // 第三行: 椭圆弧演示
    auto* row3Title = new TextBlock();
    row3Title->Text("椭圆弧与旋转")
             ->FontSize(18.0f)
             ->FontWeight(FontWeight::SemiBold)
             ->Foreground(new SolidColorBrush(Color::FromRGB(0, 0, 0, 255)));
    panel->AddChild(row3Title);

    auto* row3 = new StackPanel();
    row3->SetOrient(Orientation::Horizontal)->SetSpacing(40.0f);

    // 椭圆弧 (rx != ry)
    auto* arc5Container = new StackPanel();
    arc5Container->SetOrient(Orientation::Vertical)->SetSpacing(10.0f);
    auto* arc5Label = new TextBlock();
    arc5Label->Text("椭圆弧 (rx=60, ry=30)")
             ->FontSize(14.0f)
             ->FontWeight(FontWeight::SemiBold);
    arc5Container->AddChild(arc5Label);
    
    auto* arc5 = new Path();
    arc5->Width(160)->Height(120);
    arc5->MoveTo(20, 60)
        ->ArcTo(Point(140, 60), 70, 35, 0, false, false)
        ->Fill(new SolidColorBrush(Color::FromRGB(255, 220, 255, 200)))
        ->Stroke(new SolidColorBrush(Color::FromRGB(200, 0, 200, 255)))
        ->StrokeThickness(3.0f);
    arc5Container->AddChild(arc5);
    row3->AddChild(arc5Container);

    // 旋转45度的椭圆弧
    auto* arc6Container = new StackPanel();
    arc6Container->SetOrient(Orientation::Vertical)->SetSpacing(10.0f);
    auto* arc6Label = new TextBlock();
    arc6Label->Text("旋转45° (xAxisRotation=45)")
             ->FontSize(14.0f)
             ->FontWeight(FontWeight::SemiBold);
    arc6Container->AddChild(arc6Label);
    
    auto* arc6 = new Path();
    arc6->Width(160)->Height(120);
    arc6->MoveTo(30, 70)
        ->ArcTo(Point(130, 50), 60, 30, 30, false, true)
        ->Fill(new SolidColorBrush(Color::FromRGB(200, 255, 255, 200)))
        ->Stroke(new SolidColorBrush(Color::FromRGB(0, 150, 200, 255)))
        ->StrokeThickness(3.0f);
    arc6Container->AddChild(arc6);
    row3->AddChild(arc6Container);

    panel->AddChild(row3);

    // 第四行: 实际应用示例
    auto* row4Title = new TextBlock();
    row4Title->Text("实际应用示例")
             ->FontSize(18.0f)
             ->FontWeight(FontWeight::SemiBold)
             ->Foreground(new SolidColorBrush(Color::FromRGB(0, 0, 0, 255)));
    panel->AddChild(row4Title);

    auto* row4 = new StackPanel();
    row4->SetOrient(Orientation::Horizontal)->SetSpacing(30.0f);

    // 圆角矩形
    auto* rrLabel = new TextBlock();
    rrLabel->Text("圆角矩形")->FontSize(14.0f);
    auto* rrContainer = new StackPanel();
    rrContainer->SetOrient(Orientation::Vertical)->SetSpacing(5.0f);
    rrContainer->AddChild(rrLabel);
    
    auto* roundRect = new Path();
    roundRect->Width(120)->Height(110);
    roundRect->MoveTo(25, 15)
             ->LineTo(95, 15)
             ->ArcTo(Point(105, 25), 10, 10, 0, false, false)
             ->LineTo(105, 85)
             ->ArcTo(Point(95, 95), 10, 10, 0, false, false)
             ->LineTo(25, 95)
             ->ArcTo(Point(15, 85), 10, 10, 0, false, false)
             ->LineTo(15, 25)
             ->ArcTo(Point(25, 15), 10, 10, 0, false, false)
             ->Close()
             ->Fill(new SolidColorBrush(Color::FromRGB(150, 200, 255, 255)))
             ->Stroke(new SolidColorBrush(Color::FromRGB(50, 100, 200, 255)))
             ->StrokeThickness(2.0f);
    rrContainer->AddChild(roundRect);
    row4->AddChild(rrContainer);

    // 月牙形
    auto* moonLabel = new TextBlock();
    moonLabel->Text("月牙形")->FontSize(14.0f);
    auto* moonContainer = new StackPanel();
    moonContainer->SetOrient(Orientation::Vertical)->SetSpacing(5.0f);
    moonContainer->AddChild(moonLabel);
    
    auto* moon = new Path();
    moon->Width(120)->Height(110);
    moon->MoveTo(60, 30)
        ->ArcTo(Point(60, 90), 35, 35, 0, false, true)
        ->ArcTo(Point(60, 30), 25, 25, 0, false, false)
        ->Close()
        ->Fill(new SolidColorBrush(Color::FromRGB(255, 255, 200, 255)))
        ->Stroke(new SolidColorBrush(Color::FromRGB(200, 200, 100, 255)))
        ->StrokeThickness(2.0f);
    moonContainer->AddChild(moon);
    row4->AddChild(moonContainer);

    // 扇形
    auto* sectorLabel = new TextBlock();
    sectorLabel->Text("扇形")->FontSize(14.0f);
    auto* sectorContainer = new StackPanel();
    sectorContainer->SetOrient(Orientation::Vertical)->SetSpacing(5.0f);
    sectorContainer->AddChild(sectorLabel);
    
    auto* sector = new Path();
    sector->Width(120)->Height(110);
    sector->MoveTo(60, 60)
          ->LineTo(60, 20)
          ->ArcTo(Point(95, 60), 40, 40, 0, false, false)
          ->Close()
          ->Fill(new SolidColorBrush(Color::FromRGB(255, 200, 150, 255)))
          ->Stroke(new SolidColorBrush(Color::FromRGB(200, 100, 50, 255)))
          ->StrokeThickness(2.0f);
    sectorContainer->AddChild(sector);
    row4->AddChild(sectorContainer);

    panel->AddChild(row4);

    window->Content(panel);
    application->Run(window);
    return 0;
}
