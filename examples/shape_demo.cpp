/**
 * @file shape_demo.cpp
 * @brief Shape 图形控件演示
 * 
 * 展示如何使用各种 Shape 派生类：
 * - Rectangle: 矩形和圆角矩形
 * - Ellipse: 圆形和椭圆
 * - Line: 直线
 * - Polygon: 三角形、五边形等多边形
 * - Path: 复杂路径和贝塞尔曲线
 */

#include "fk/app/Application.h"
#include "fk/ui/Window.h"
#include "fk/ui/StackPanel.h"
#include "fk/ui/Grid.h"
#include "fk/ui/TextBlock.h"
#include "fk/ui/Shape.h"
#include "fk/ui/Brush.h"
#include <iostream>

using namespace fk::ui;

int main() {
    std::cout << "\n=== F__K UI Shape Demo ===\n";
    std::cout << "Features:\n";
    std::cout << "- Rectangle (圆角矩形)\n";
    std::cout << "- Ellipse (圆形/椭圆)\n";
    std::cout << "- Line (直线)\n";
    std::cout << "- Polygon (多边形)\n";
    std::cout << "- Path (贝塞尔曲线)\n";
    std::cout << "==========================\n\n";

    auto application = std::make_unique<fk::Application>();

    // 创建主窗口
    auto window = std::make_shared<Window>();
    window->Title("Shape Demo - 图形演示")
          ->Width(1000)
          ->Height(700)
          ->Background(new SolidColorBrush(Color::FromRGB(240, 245, 255, 255)));

    // 创建主布局容器
    auto* mainPanel = new StackPanel();
    mainPanel->SetOrient(Orientation::Vertical)
             ->Spacing(20.0f)
             ->Margin(20.0f);

    // 标题
    auto* title = new TextBlock();
    title->Text("Shape 图形控件演示")
         ->FontSize(24.0f)
         ->Foreground(new SolidColorBrush(Color::FromRGB(0, 0, 0, 255)))
         ->SetHorizontalAlignment(HorizontalAlignment::Center);
    mainPanel->AddChild(title);

    // ========== 1. Rectangle 演示 ==========
    auto* rectSection = new StackPanel();
    rectSection->SetOrient(Orientation::Vertical)->SetSpacing(10.0f);
    
    auto* rectTitle = new TextBlock();
    rectTitle->Text("1. Rectangle - 矩形")->FontSize(18.0f)->FontWeight(fk::ui::FontWeight::SemiBold);
    rectSection->AddChild(rectTitle);

    auto* rectRow = new StackPanel();
    rectRow->SetOrient(Orientation::Horizontal)->SetSpacing(15.0f);

    // 普通矩形
    auto* rect1 = new Rectangle();
    rect1->Width(100.0f)->Height(60.0f)
         ->Fill(new SolidColorBrush(Color::FromRGB(255, 100, 100, 255)))
         ->Stroke(new SolidColorBrush(Color::FromRGB(200, 0, 0, 255)))
         ->StrokeThickness(2.0f);
    rectRow->AddChild(rect1);

    // 圆角矩形
    auto* rect2 = new Rectangle();
    rect2->Width(100.0f)->Height(60.0f);
    rect2->RadiusX(20.0f)->RadiusY(15.0f)
         ->Fill(new SolidColorBrush(Color::FromRGB(100, 255, 100, 255)))
         ->Stroke(new SolidColorBrush(Color::FromRGB(0, 0, 0, 255)))
         ->StrokeThickness(3.0f);
    rectRow->AddChild(rect2);

    // 正方形
    auto* rect3 = new Rectangle();
    rect3->Width(60.0f)->Height(60.0f);
    rect3->RadiusX(30.0f)->RadiusY(30.0f)  // 完全圆角
         ->Fill(new SolidColorBrush(Color::FromRGB(100, 100, 255, 255)))
         ->Stroke(new SolidColorBrush(Color::FromRGB(0, 0, 200, 255)))
         ->StrokeThickness(2.0f);
    rectRow->AddChild(rect3);

    rectSection->AddChild(rectRow);
    mainPanel->AddChild(rectSection);

    // ========== 2. Ellipse 演示 ==========
    auto* ellipseSection = new StackPanel();
    ellipseSection->SetOrient(Orientation::Vertical)->SetSpacing(10.0f);
    
    auto* ellipseTitle = new TextBlock();
    ellipseTitle->Text("2. Ellipse - 圆形/椭圆")->FontSize(18.0f)->FontWeight(fk::ui::FontWeight::SemiBold);
    ellipseSection->AddChild(ellipseTitle);

    auto* ellipseRow = new StackPanel();
    ellipseRow->SetOrient(Orientation::Horizontal)->SetSpacing(15.0f);

    // 圆形
    auto* circle = new Ellipse();
    circle->Width(60.0f)->Height(60.0f)
          ->Fill(new SolidColorBrush(Color::FromRGB(255, 200, 0, 255)))
          ->Stroke(new SolidColorBrush(Color::FromRGB(200, 150, 0, 255)))
          ->StrokeThickness(2.0f);
    ellipseRow->AddChild(circle);

    // 椭圆
    auto* ellipse = new Ellipse();
    ellipse->Width(120.0f)->Height(60.0f)
           ->Fill(new SolidColorBrush(Color::FromRGB(255, 150, 200, 255)))
           ->Stroke(new SolidColorBrush(Color::FromRGB(200, 100, 150, 255)))
           ->StrokeThickness(2.0f);
    ellipseRow->AddChild(ellipse);

    // 仅描边的圆
    auto* circle2 = new Ellipse();
    circle2->Width(60.0f)->Height(60.0f)
           ->Stroke(new SolidColorBrush(Color::FromRGB(0, 200, 200, 255)))
           ->StrokeThickness(3.0f);
    ellipseRow->AddChild(circle2);

    ellipseSection->AddChild(ellipseRow);
    mainPanel->AddChild(ellipseSection);

    // ========== 3. Line 演示 ==========
    auto* lineSection = new StackPanel();
    lineSection->SetOrient(Orientation::Vertical)->SetSpacing(10.0f);
    
    auto* lineTitle = new TextBlock();
    lineTitle->Text("3. Line - 直线")->FontSize(18.0f)->FontWeight(fk::ui::FontWeight::SemiBold);
    lineSection->AddChild(lineTitle);

    auto* lineRow = new StackPanel();
    lineRow->SetOrient(Orientation::Horizontal)->SetSpacing(15.0f);

    // 水平线
    auto* hLine = new Line();
    hLine->X1(0.0f)->Y1(0.0f)->X2(100.0f)->Y2(0.0f)
         ->Stroke(new SolidColorBrush(Color::FromRGB(255, 0, 0, 255)))
         ->StrokeThickness(2.0f);
    lineRow->AddChild(hLine);

    // 垂直线
    auto* vLine = new Line();
    vLine->X1(0.0f)->Y1(0.0f)->X2(0.0f)->Y2(60.0f)
         ->Stroke(new SolidColorBrush(Color::FromRGB(0, 255, 0, 255)))
         ->StrokeThickness(2.0f);
    lineRow->AddChild(vLine);

    // 斜线
    auto* dLine = new Line();
    dLine->X1(0.0f)->Y1(0.0f)->X2(60.0f)->Y2(60.0f)
         ->Stroke(new SolidColorBrush(Color::FromRGB(0, 0, 255, 255)))
         ->StrokeThickness(3.0f);
    lineRow->AddChild(dLine);

    lineSection->AddChild(lineRow);
    mainPanel->AddChild(lineSection);

    // ========== 4. Polygon 演示 ==========
    auto* polygonSection = new StackPanel();
    polygonSection->SetOrient(Orientation::Vertical)->SetSpacing(10.0f);
    
    auto* polygonTitle = new TextBlock();
    polygonTitle->Text("4. Polygon - 多边形")->FontSize(18.0f)->FontWeight(fk::ui::FontWeight::SemiBold);
    polygonSection->AddChild(polygonTitle);

    auto* polygonRow = new StackPanel();
    polygonRow->SetOrient(Orientation::Horizontal)->SetSpacing(15.0f);

    // 三角形
    auto* triangle = new Polygon();
    triangle->AddPoint(Point(30, 0));
    triangle->AddPoint(Point(60, 60));
    triangle->AddPoint(Point(0, 60));
    triangle->Fill(new SolidColorBrush(Color::FromRGB(255, 100, 255, 255)))
            ->Stroke(new SolidColorBrush(Color::FromRGB(200, 0, 200, 255)))
            ->StrokeThickness(2.0f);
    polygonRow->AddChild(triangle);

    // 菱形
    auto* diamond = new Polygon();
    diamond->AddPoint(Point(30, 0));
    diamond->AddPoint(Point(60, 30));
    diamond->AddPoint(Point(30, 60));
    diamond->AddPoint(Point(0, 30));
    diamond->Fill(new SolidColorBrush(Color::FromRGB(100, 255, 255, 255)))
           ->Stroke(new SolidColorBrush(Color::FromRGB(0, 200, 200, 255)))
           ->StrokeThickness(2.0f);
    polygonRow->AddChild(diamond);

    // 五角星（简化版）
    auto* star = new Polygon();
    star->AddPoint(Point(30, 0));
    star->AddPoint(Point(37, 22));
    star->AddPoint(Point(60, 22));
    star->AddPoint(Point(42, 36));
    star->AddPoint(Point(49, 60));
    star->AddPoint(Point(30, 45));
    star->AddPoint(Point(11, 60));
    star->AddPoint(Point(18, 36));
    star->AddPoint(Point(0, 22));
    star->AddPoint(Point(23, 22));
    star->Fill(new SolidColorBrush(Color::FromRGB(255, 215, 0, 255)))
        ->Stroke(new SolidColorBrush(Color::FromRGB(200, 165, 0, 255)))
        ->StrokeThickness(2.0f);
    polygonRow->AddChild(star);

    polygonSection->AddChild(polygonRow);
    mainPanel->AddChild(polygonSection);

    // ========== 5. Path 演示 ==========
    auto* pathSection = new StackPanel();
    pathSection->SetOrient(Orientation::Vertical)->SetSpacing(10.0f);
    
    auto* pathTitle = new TextBlock();
    pathTitle->Text("5. Path - 复杂路径")->FontSize(18.0f)->FontWeight(fk::ui::FontWeight::SemiBold);
    pathSection->AddChild(pathTitle);

    auto* pathRow = new StackPanel();
    pathRow->SetOrient(Orientation::Horizontal)->SetSpacing(15.0f);

    // 贝塞尔曲线
    auto* bezier = new Path();
    bezier->MoveTo(0, 30)
          ->CubicTo(20, 0, 40, 60, 60, 30)
          ->Fill(new SolidColorBrush(Color::FromRGB(150, 200, 255, 255)))
          ->Stroke(new SolidColorBrush(Color::FromRGB(0, 100, 200, 255)))
          ->StrokeThickness(2.0f);
    pathRow->AddChild(bezier);

    // 心形
    auto* heart = new Path();
    heart->MoveTo(30, 15)
         ->CubicTo(30, 10, 25, 5, 20, 5)
         ->CubicTo(10, 5, 5, 15, 5, 25)
         ->CubicTo(5, 35, 15, 45, 30, 55)
         ->CubicTo(45, 45, 55, 35, 55, 25)
         ->CubicTo(55, 15, 50, 5, 40, 5)
         ->CubicTo(35, 5, 30, 10, 30, 15)
         ->Close()
         ->Fill(new SolidColorBrush(Color::FromRGB(255, 100, 120, 255)))
         ->Stroke(new SolidColorBrush(Color::FromRGB(200, 50, 70, 255)))
         ->StrokeThickness(2.0f);
    pathRow->AddChild(heart);

    pathSection->AddChild(pathRow);
    mainPanel->AddChild(pathSection);

    // 设置窗口内容
    window->Content(mainPanel);

    // 运行应用
    application->Run(window);
    return 0;
}
