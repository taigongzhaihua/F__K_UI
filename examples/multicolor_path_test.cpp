/**
 * @file multicolor_path_test.cpp
 * @brief Path分段着色测试
 */

#include "fk/app/Application.h"
#include "fk/ui/StackPanel.h"
#include "fk/ui/TextBlock.h"
#include "fk/ui/Shape.h"
#include "fk/ui/Brush.h"
#include <iostream>

using namespace fk;
using namespace fk::ui;

int main() {
    std::cout << "=== Multicolor Path Test ===" << std::endl;
    
    auto application = std::make_unique<fk::Application>();
    
    auto window = std::make_shared<Window>();
    window->Title("Multicolor Path Test")
          ->Width(900)
          ->Height(600)
          ->Background(new SolidColorBrush(render::Color::FromRGB(240, 240, 240, 255)));
    
    // 主容器 - 垂直布局
    auto* mainPanel = new StackPanel();
    mainPanel->SetOrient(Orientation::Vertical)
             ->Spacing(15.0f)
             ->Margin(20.0f);
    window->Content(mainPanel);
    
    // 标题
    auto* title = new TextBlock();
    title->Text("Path分段着色示例")->FontSize(24.0f)->Foreground(new SolidColorBrush(render::Color::FromRGB(50, 50, 50)));
    mainPanel->AddChild(title);
    
    // 第一行 - 三段直线
    auto* row1 = new StackPanel();
    row1->SetOrient(Orientation::Horizontal)->Spacing(20.0f);
    mainPanel->AddChild(row1);
    
    // 1. 红-绿-蓝三段直线
    auto* label1 = new TextBlock();
    label1->Text("红-绿-蓝三段")->FontSize(14.0f)->Width(150);
    row1->AddChild(label1);
    
    auto* path1 = new Path();
    path1->Width(200)->Height(120);
    path1->MoveTo(10, 60)
         ->LineTo(70, 60)->SetSegmentStroke(1.0f, 0.0f, 0.0f) // 红色
         ->LineTo(130, 60)->SetSegmentStroke(0.0f, 1.0f, 0.0f) // 绿色
         ->LineTo(190, 60)->SetSegmentStroke(0.0f, 0.0f, 1.0f) // 蓝色
         ->Fill(nullptr)
         ->StrokeThickness(5.0f);
    row1->AddChild(path1);
    
    // 2. 彩虹折线
    auto* label2 = new TextBlock();
    label2->Text("彩虹折线")->FontSize(14.0f)->Width(150);
    row1->AddChild(label2);
    
    auto* path2 = new Path();
    path2->Width(200)->Height(120);
    path2->MoveTo(10, 100)
         ->LineTo(40, 20)->SetSegmentStroke(1.0f, 0.0f, 0.0f)    // 红
         ->LineTo(75, 80)->SetSegmentStroke(1.0f, 0.5f, 0.0f)    // 橙
         ->LineTo(110, 30)->SetSegmentStroke(1.0f, 1.0f, 0.0f)   // 黄
         ->LineTo(145, 90)->SetSegmentStroke(0.0f, 1.0f, 0.0f)   // 绿
         ->LineTo(180, 40)->SetSegmentStroke(0.0f, 0.0f, 1.0f)   // 蓝
         ->Fill(nullptr)
         ->StrokeThickness(4.0f);
    row1->AddChild(path2);
    
    // 第二行 - 曲线
    auto* row2 = new StackPanel();
    row2->SetOrient(Orientation::Horizontal)->Spacing(20.0f);
    mainPanel->AddChild(row2);
    
    // 3. 红蓝贝塞尔曲线
    auto* label3 = new TextBlock();
    label3->Text("红蓝贝塞尔")->FontSize(14.0f)->Width(150);
    row2->AddChild(label3);
    
    auto* path3 = new Path();
    path3->Width(200)->Height(120);
    path3->MoveTo(10, 60)
         ->QuadraticTo(Point(60, 10), Point(110, 60))->SetSegmentStroke(1.0f, 0.0f, 0.0f) // 红色曲线
         ->QuadraticTo(Point(160, 110), Point(190, 60))->SetSegmentStroke(0.0f, 0.0f, 1.0f) // 蓝色曲线
         ->Fill(nullptr)
         ->StrokeThickness(3.0f);
    row2->AddChild(path3);
    
    // 4. 三次贝塞尔渐变
    auto* label4 = new TextBlock();
    label4->Text("S形渐变")->FontSize(14.0f)->Width(150);
    row2->AddChild(label4);
    
    auto* path4 = new Path();
    path4->Width(200)->Height(120);
    path4->MoveTo(20, 60)
         ->CubicTo(Point(70, 10), Point(70, 110), Point(120, 60))->SetSegmentStroke(1.0f, 0.0f, 0.5f) // 粉红
         ->CubicTo(Point(170, 10), Point(170, 110), Point(180, 60))->SetSegmentStroke(0.5f, 0.0f, 1.0f) // 紫色
         ->Fill(nullptr)
         ->StrokeThickness(4.0f);
    row2->AddChild(path4);
    
    // 第三行 - 圆弧
    auto* row3 = new StackPanel();
    row3->SetOrient(Orientation::Horizontal)->Spacing(20.0f);
    mainPanel->AddChild(row3);
    
    // 5. 红绿圆弧
    auto* label5 = new TextBlock();
    label5->Text("红绿圆弧")->FontSize(14.0f)->Width(150);
    row3->AddChild(label5);
    
    auto* path5 = new Path();
    path5->Width(200)->Height(120);
    path5->MoveTo(10, 60)
         ->ArcTo(Point(100, 60), 45, 45, 0, false, true)->SetSegmentStroke(1.0f, 0.0f, 0.0f) // 红色弧
         ->ArcTo(Point(190, 60), 45, 45, 0, false, true)->SetSegmentStroke(0.0f, 1.0f, 0.0f) // 绿色弧
         ->Fill(nullptr)
         ->StrokeThickness(3.0f);
    row3->AddChild(path5);
    
    // 6. 混合路径 - 直线+曲线+圆弧
    auto* label6 = new TextBlock();
    label6->Text("混合多色")->FontSize(14.0f)->Width(150);
    row3->AddChild(label6);
    
    auto* path6 = new Path();
    path6->Width(200)->Height(120);
    path6->MoveTo(10, 80)
         ->LineTo(50, 80)->SetSegmentStroke(1.0f, 0.0f, 0.0f)     // 红色直线
         ->QuadraticTo(Point(75, 20), Point(100, 80))->SetSegmentStroke(0.0f, 1.0f, 0.0f) // 绿色曲线
         ->ArcTo(Point(150, 40), 25, 40, 0, false, false)->SetSegmentStroke(0.0f, 0.5f, 1.0f) // 蓝色弧
         ->LineTo(180, 100)->SetSegmentStroke(1.0f, 0.5f, 0.0f)   // 橙色直线
         ->Fill(nullptr)
         ->StrokeThickness(4.0f);
    row3->AddChild(path6);
    
    // 第四行 - 对比示例
    auto* row4 = new StackPanel();
    row4->SetOrient(Orientation::Horizontal)->Spacing(20.0f);
    mainPanel->AddChild(row4);
    
    // 7. 无分段着色(全黑)
    auto* label7 = new TextBlock();
    label7->Text("无分段着色")->FontSize(14.0f)->Width(150);
    row4->AddChild(label7);
    
    auto* path7 = new Path();
    path7->Width(200)->Height(120);
    path7->MoveTo(10, 60)
         ->LineTo(50, 20)
         ->LineTo(90, 60)
         ->LineTo(130, 100)
         ->LineTo(170, 60)
         ->Fill(nullptr)
         ->Stroke(new SolidColorBrush(render::Color::FromRGB(0, 0, 0)))
         ->StrokeThickness(3.0f);
    row4->AddChild(path7);
    
    // 8. 全部分段着色
    auto* label8 = new TextBlock();
    label8->Text("全部分段")->FontSize(14.0f)->Width(150);
    row4->AddChild(label8);
    
    auto* path8 = new Path();
    path8->Width(200)->Height(120);
    path8->MoveTo(10, 60)
         ->LineTo(50, 20)->SetSegmentStroke(1.0f, 0.0f, 0.0f)
         ->LineTo(90, 60)->SetSegmentStroke(0.0f, 1.0f, 0.0f)
         ->LineTo(130, 100)->SetSegmentStroke(0.0f, 0.0f, 1.0f)
         ->LineTo(170, 60)->SetSegmentStroke(1.0f, 0.5f, 0.0f)
         ->Fill(nullptr)
         ->StrokeThickness(3.0f);
    row4->AddChild(path8);
    
    application->Run(window);
    return 0;
}
