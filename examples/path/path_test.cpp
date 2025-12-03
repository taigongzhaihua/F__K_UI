/**
 * @file path_test.cpp
 * @brief Path 路径控件测试
 * 
 * 测试各种路径命令：
 * - MoveTo/LineTo: 直线路径
 * - QuadraticTo: 二次贝塞尔曲线
 * - CubicTo: 三次贝塞尔曲线
 * - Close: 闭合路径
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
    std::cout << "=== Path 路径测试 ===\n";

    auto application = std::make_unique<fk::Application>();

    auto window = std::make_shared<Window>();
    window->Title("Path Test - 路径测试")
          ->Width(800)
          ->Height(600)
          ->Background(new SolidColorBrush(Color::FromRGB(250, 250, 250, 255)));

    auto* panel = new StackPanel();
    panel->SetOrient(Orientation::Vertical)
         ->Spacing(30.0f)
         ->Margin(20.0f);

    // 标题
    auto* title = new TextBlock();
    title->Text("Path 路径测试 - 贝塞尔曲线")
         ->FontSize(24.0f)
         ->Foreground(new SolidColorBrush(Color::FromRGB(0, 0, 0, 255)));
    panel->AddChild(title);

    // 第一行: 简单曲线
    auto* row1 = new StackPanel();
    row1->SetOrient(Orientation::Horizontal)->SetSpacing(30.0f);

    // 1. 直线路径
    auto* path1Label = new TextBlock();
    path1Label->Text("直线路径")->FontSize(14.0f);
    auto* path1Container = new StackPanel();
    path1Container->SetOrient(Orientation::Vertical)->SetSpacing(5.0f);
    path1Container->AddChild(path1Label);
    
    auto* simpleLine = new Path();
    simpleLine->MoveTo(10, 10)
              ->LineTo(90, 10)
              ->LineTo(90, 90)
              ->LineTo(10, 90)
              ->Close()
              ->Fill(new SolidColorBrush(Color::FromRGB(100, 200, 255, 255)))
              ->Stroke(new SolidColorBrush(Color::FromRGB(0, 100, 200, 255)))
              ->StrokeThickness(3.0f);
    path1Container->AddChild(simpleLine);
    row1->AddChild(path1Container);

    // 2. 二次贝塞尔曲线
    auto* path2Label = new TextBlock();
    path2Label->Text("二次贝塞尔")->FontSize(14.0f);
    auto* path2Container = new StackPanel();
    path2Container->SetOrient(Orientation::Vertical)->SetSpacing(5.0f);
    path2Container->AddChild(path2Label);
    
    auto* quadratic = new Path();
    quadratic->MoveTo(10, 80)
             ->QuadraticTo(50, 10, 90, 80)
             ->LineTo(10, 80)
             ->Close()
             ->Fill(new SolidColorBrush(Color::FromRGB(255, 200, 100, 255)))
             ->Stroke(new SolidColorBrush(Color::FromRGB(200, 100, 0, 255)))
             ->StrokeThickness(3.0f);
    path2Container->AddChild(quadratic);
    row1->AddChild(path2Container);

    // 3. 三次贝塞尔曲线 (波浪)
    auto* path3Label = new TextBlock();
    path3Label->Text("三次贝塞尔")->FontSize(14.0f);
    auto* path3Container = new StackPanel();
    path3Container->SetOrient(Orientation::Vertical)->SetSpacing(5.0f);
    path3Container->AddChild(path3Label);
    
    auto* cubic = new Path();
    cubic->MoveTo(10, 50)
         ->CubicTo(30, 10, 70, 90, 90, 50)
         ->LineTo(90, 90)
         ->LineTo(10, 90)
         ->Close()
         ->Fill(new SolidColorBrush(Color::FromRGB(150, 255, 150, 255)))
         ->Stroke(new SolidColorBrush(Color::FromRGB(50, 200, 50, 255)))
         ->StrokeThickness(3.0f);
    path3Container->AddChild(cubic);
    row1->AddChild(path3Container);

    panel->AddChild(row1);

    // 第二行: 复杂形状
    auto* row2 = new StackPanel();
    row2->SetOrient(Orientation::Horizontal)->SetSpacing(30.0f);

    // 4. 心形
    auto* path4Label = new TextBlock();
    path4Label->Text("心形")->FontSize(14.0f);
    auto* path4Container = new StackPanel();
    path4Container->SetOrient(Orientation::Vertical)->SetSpacing(5.0f);
    path4Container->AddChild(path4Label);
    
    auto* heart = new Path();
    heart->MoveTo(50, 25)
         ->CubicTo(50, 17, 43, 10, 35, 10)
         ->CubicTo(22, 10, 12, 22, 12, 37)
         ->CubicTo(12, 52, 27, 67, 50, 88)
         ->CubicTo(73, 67, 88, 52, 88, 37)
         ->CubicTo(88, 22, 78, 10, 65, 10)
         ->CubicTo(57, 10, 50, 17, 50, 25)
         ->Close()
         ->Fill(new SolidColorBrush(Color::FromRGB(255, 100, 120, 255)))
         ->Stroke(new SolidColorBrush(Color::FromRGB(200, 50, 70, 255)))
         ->StrokeThickness(3.0f);
    path4Container->AddChild(heart);
    row2->AddChild(path4Container);

    // 5. S形曲线
    auto* path5Label = new TextBlock();
    path5Label->Text("S曲线")->FontSize(14.0f);
    auto* path5Container = new StackPanel();
    path5Container->SetOrient(Orientation::Vertical)->SetSpacing(5.0f);
    path5Container->AddChild(path5Label);
    
    auto* sCurve = new Path();
    sCurve->MoveTo(10, 10)
          ->CubicTo(40, 10, 40, 40, 50, 50)
          ->CubicTo(60, 60, 60, 90, 90, 90)
          ->LineTo(90, 100)
          ->LineTo(10, 100)
          ->Close()
          ->Fill(new SolidColorBrush(Color::FromRGB(200, 150, 255, 255)))
          ->Stroke(new SolidColorBrush(Color::FromRGB(100, 50, 200, 255)))
          ->StrokeThickness(3.0f);
    path5Container->AddChild(sCurve);
    row2->AddChild(path5Container);

    // 6. 花瓣
    auto* path6Label = new TextBlock();
    path6Label->Text("花瓣")->FontSize(14.0f);
    auto* path6Container = new StackPanel();
    path6Container->SetOrient(Orientation::Vertical)->SetSpacing(5.0f);
    path6Container->AddChild(path6Label);
    
    auto* petal = new Path();
    petal->MoveTo(50, 50)
         ->QuadraticTo(30, 20, 50, 10)
         ->QuadraticTo(70, 20, 50, 50)
         ->Close()
         ->Fill(new SolidColorBrush(Color::FromRGB(255, 180, 200, 255)))
         ->Stroke(new SolidColorBrush(Color::FromRGB(255, 100, 150, 255)))
         ->StrokeThickness(2.0f);
    path6Container->AddChild(petal);
    row2->AddChild(path6Container);

    panel->AddChild(row2);

    // 第三行: 更复杂的路径
    auto* row3 = new StackPanel();
    row3->SetOrient(Orientation::Horizontal)->SetSpacing(30.0f);

    // 7. 云朵形状
    auto* path7Label = new TextBlock();
    path7Label->Text("云朵")->FontSize(14.0f);
    auto* path7Container = new StackPanel();
    path7Container->SetOrient(Orientation::Vertical)->SetSpacing(5.0f);
    path7Container->AddChild(path7Label);
    
    auto* cloud = new Path();
    cloud->MoveTo(20, 50)
         ->QuadraticTo(20, 30, 35, 30)
         ->QuadraticTo(35, 15, 50, 15)
         ->QuadraticTo(65, 15, 65, 30)
         ->QuadraticTo(80, 30, 80, 50)
         ->QuadraticTo(80, 65, 65, 65)
         ->LineTo(35, 65)
         ->QuadraticTo(20, 65, 20, 50)
         ->Close()
         ->Fill(new SolidColorBrush(Color::FromRGB(220, 240, 255, 255)))
         ->Stroke(new SolidColorBrush(Color::FromRGB(100, 150, 200, 255)))
         ->StrokeThickness(2.0f);
    path7Container->AddChild(cloud);
    row3->AddChild(path7Container);

    panel->AddChild(row3);

    // 第四行: 圆弧测试
    auto* row4 = new StackPanel();
    row4->SetOrient(Orientation::Horizontal)->SetSpacing(30.0f);

    // 8. 简单圆弧
    auto* path8Label = new TextBlock();
    path8Label->Text("圆弧")->FontSize(14.0f);
    auto* path8Container = new StackPanel();
    path8Container->SetOrient(Orientation::Vertical)->SetSpacing(5.0f);
    path8Container->AddChild(path8Label);
    
    auto* simpleArc = new Path();
    simpleArc->MoveTo(10, 50)
             ->ArcTo(Point(90, 50), 40, 40, 0, true, true)
             ->LineTo(90, 80)
             ->LineTo(10, 80)
             ->Close()
             ->Fill(new SolidColorBrush(Color::FromRGB(255, 200, 150, 255)))
             ->Stroke(new SolidColorBrush(Color::FromRGB(200, 100, 50, 255)))
             ->StrokeThickness(2.0f);
    path8Container->AddChild(simpleArc);
    row4->AddChild(path8Container);

    // 9. 椭圆弧
    auto* path9Label = new TextBlock();
    path9Label->Text("椭圆弧")->FontSize(14.0f);
    auto* path9Container = new StackPanel();
    path9Container->SetOrient(Orientation::Vertical)->SetSpacing(5.0f);
    path9Container->AddChild(path9Label);
    
    auto* ellipseArc = new Path();
    ellipseArc->MoveTo(10, 50)
              ->ArcTo(Point(90, 50), 40, 25, 0, false, true)
              ->LineTo(90, 80)
              ->LineTo(10, 80)
              ->Close()
              ->Fill(new SolidColorBrush(Color::FromRGB(200, 255, 200, 255)))
              ->Stroke(new SolidColorBrush(Color::FromRGB(50, 200, 100, 255)))
              ->StrokeThickness(2.0f);
    path9Container->AddChild(ellipseArc);
    row4->AddChild(path9Container);

    // 10. 旋转的椭圆弧
    auto* path10Label = new TextBlock();
    path10Label->Text("旋转椭圆弧")->FontSize(14.0f);
    auto* path10Container = new StackPanel();
    path10Container->SetOrient(Orientation::Vertical)->SetSpacing(5.0f);
    path10Container->AddChild(path10Label);
    
    auto* rotatedArc = new Path();
    rotatedArc->MoveTo(10, 50)
              ->ArcTo(Point(90, 50), 40, 25, 45, true, false)
              ->LineTo(90, 80)
              ->LineTo(10, 80)
              ->Close()
              ->Fill(new SolidColorBrush(Color::FromRGB(255, 220, 255, 255)))
              ->Stroke(new SolidColorBrush(Color::FromRGB(200, 100, 200, 255)))
              ->StrokeThickness(2.0f);
    path10Container->AddChild(rotatedArc);
    row4->AddChild(path10Container);

    panel->AddChild(row4);

    // 第五行: 组合圆弧形状
    auto* row5 = new StackPanel();
    row5->SetOrient(Orientation::Horizontal)->SetSpacing(30.0f);

    // 11. 圆角矩形(用圆弧实现)
    auto* path11Label = new TextBlock();
    path11Label->Text("圆角矩形")->FontSize(14.0f);
    auto* path11Container = new StackPanel();
    path11Container->SetOrient(Orientation::Vertical)->SetSpacing(5.0f);
    path11Container->AddChild(path11Label);
    
    auto* roundedRect = new Path();
    roundedRect->MoveTo(20, 10)
               ->LineTo(80, 10)
               ->ArcTo(Point(90, 20), 10, 10, 0, false, true)
               ->LineTo(90, 70)
               ->ArcTo(Point(80, 80), 10, 10, 0, false, true)
               ->LineTo(20, 80)
               ->ArcTo(Point(10, 70), 10, 10, 0, false, true)
               ->LineTo(10, 20)
               ->ArcTo(Point(20, 10), 10, 10, 0, false, true)
               ->Close()
               ->Fill(new SolidColorBrush(Color::FromRGB(180, 220, 255, 255)))
               ->Stroke(new SolidColorBrush(Color::FromRGB(50, 100, 200, 255)))
               ->StrokeThickness(2.0f);
    path11Container->AddChild(roundedRect);
    row5->AddChild(path11Container);

    // 12. 扇形
    auto* path12Label = new TextBlock();
    path12Label->Text("扇形")->FontSize(14.0f);
    auto* path12Container = new StackPanel();
    path12Container->SetOrient(Orientation::Vertical)->SetSpacing(5.0f);
    path12Container->AddChild(path12Label);
    
    auto* sector = new Path();
    sector->MoveTo(50, 50)
          ->LineTo(50, 10)
          ->ArcTo(Point(85, 50), 40, 40, 0, false, true)
          ->Close()
          ->Fill(new SolidColorBrush(Color::FromRGB(255, 230, 150, 255)))
          ->Stroke(new SolidColorBrush(Color::FromRGB(220, 150, 50, 255)))
          ->StrokeThickness(2.0f);
    path12Container->AddChild(sector);
    row5->AddChild(path12Container);

    panel->AddChild(row5);

    window->Content(panel);
    application->Run(window);
    return 0;
}
