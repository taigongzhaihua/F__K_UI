/**
 * @file star_test.cpp
 * @brief 五角星单独测试
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
    std::cout << "=== 五角星测试 ===\n";

    auto application = std::make_unique<fk::Application>();

    // 创建窗口
    auto window = std::make_shared<Window>();
    window->Title("Star Test - 五角星测试")
          ->Width(400)
          ->Height(400)
          ->Background(new SolidColorBrush(Color::FromRGB(255, 255, 255, 255)));  // 白色背景

    auto* panel = new StackPanel();
    panel->SetOrient(Orientation::Vertical)
         ->Spacing(20.0f)
         ->Margin(20.0f);

    // 标题
    auto* title = new TextBlock();
    title->Text("五角星测试")
         ->FontSize(24.0f)
         ->Foreground(new SolidColorBrush(Color::FromRGB(0, 0, 0, 255)));
    panel->AddChild(title);

    // 大号五角星 - 放大10倍
    auto* bigStar = new Polygon();
    bigStar->AddPoint(Point(150, 50));   // 顶点
    bigStar->AddPoint(Point(170, 120));  // 右上内
    bigStar->AddPoint(Point(250, 120));  // 右上外
    bigStar->AddPoint(Point(190, 180));  // 右下内
    bigStar->AddPoint(Point(220, 260));  // 右下外
    bigStar->AddPoint(Point(150, 210));  // 底部
    bigStar->AddPoint(Point(80, 260));   // 左下外
    bigStar->AddPoint(Point(110, 180));  // 左下内
    bigStar->AddPoint(Point(50, 120));   // 左上外
    bigStar->AddPoint(Point(130, 120));  // 左上内
    bigStar->Fill(new SolidColorBrush(Color::FromRGB(255, 215, 0, 255)))  // 金色
            ->Stroke(new SolidColorBrush(Color::FromRGB(255, 0, 0, 255)))  // 红色描边
            ->StrokeThickness(3.0f);
    panel->AddChild(bigStar);

    window->Content(panel);

    application->Run(window);
    return 0;
}
