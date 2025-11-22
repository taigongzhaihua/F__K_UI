/**
 * @file subpath_fill_test.cpp
 * @brief 测试一个Path内多个子路径分别填充不同颜色
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
    std::cout << "=== SubPath Fill Test ===\n";

    auto application = std::make_unique<fk::Application>();

    auto window = std::make_shared<Window>();
    window->Title("SubPath Fill Test")
          ->Width(700)
          ->Height(500)
          ->Background(new SolidColorBrush(Color::FromRGB(240, 240, 240, 255)));

    auto* panel = new StackPanel();
    panel->SetOrient(Orientation::Vertical)
         ->Spacing(20.0f)
         ->Margin(30.0f);
    window->Content(panel);

    // 标题
    auto* title = new TextBlock();
    title->Text("一个Path内多个子路径分别着色")->FontSize(20.0f);
    panel->AddChild(title);

    // 示例1: 三个正方形,不同填充和描边
    auto* label1 = new TextBlock();
    label1->Text("示例1: 三个正方形(独立填充+独立描边)")->FontSize(14.0f);
    panel->AddChild(label1);

    auto* path1 = new Path();
    path1->Width(600)->Height(100);
    // 红色正方形,黑色粗边
    path1->MoveTo(10, 10)
         ->SetSubPathFill(1.0f, 0.0f, 0.0f, 0.8f)
         ->SetSubPathStroke(0.0f, 0.0f, 0.0f, 1.0f, 3.0f)
         ->LineTo(60, 10)
         ->LineTo(60, 60)
         ->LineTo(10, 60)
         ->Close();
    // 绿色正方形,蓝色细边
    path1->MoveTo(80, 10)
         ->SetSubPathFill(0.0f, 1.0f, 0.0f, 0.8f)
         ->SetSubPathStroke(0.0f, 0.0f, 1.0f, 1.0f, 2.0f)
         ->LineTo(130, 10)
         ->LineTo(130, 60)
         ->LineTo(80, 60)
         ->Close();
    // 蓝色正方形,红色虚化边
    path1->MoveTo(150, 10)
         ->SetSubPathFill(0.0f, 0.0f, 1.0f, 0.8f)
         ->SetSubPathStroke(1.0f, 0.0f, 0.0f, 0.6f, 4.0f)
         ->LineTo(200, 10)
         ->LineTo(200, 60)
         ->LineTo(150, 60)
         ->Close();
    
    panel->AddChild(path1);

    // 示例2: 多形状组合,每个形状不同粗细的描边
    auto* label2 = new TextBlock();
    label2->Text("示例2: 多形状组合(不同描边粗细)")->FontSize(14.0f);
    panel->AddChild(label2);

    auto* path2 = new Path();
    path2->Width(600)->Height(120);
    
    // 黄色圆形,细边
    path2->MoveTo(50, 60)
         ->SetSubPathFill(1.0f, 1.0f, 0.0f, 0.8f)
         ->SetSubPathStroke(0.8f, 0.6f, 0.0f, 1.0f, 1.0f);
    for (int i = 0; i < 8; ++i) {
        float angle = i * 3.14159f * 2 / 8;
        float x = 50 + 30 * std::cos(angle);
        float y = 60 + 30 * std::sin(angle);
        path2->LineTo(x, y);
    }
    path2->Close();
    
    // 橙色三角形,中等边
    path2->MoveTo(120, 80)
         ->SetSubPathFill(1.0f, 0.6f, 0.0f, 0.8f)
         ->SetSubPathStroke(0.8f, 0.3f, 0.0f, 1.0f, 2.5f)
         ->LineTo(160, 20)
         ->LineTo(200, 80)
         ->Close();
    
    // 紫色矩形,粗边
    path2->MoveTo(220, 30)
         ->SetSubPathFill(0.6f, 0.2f, 0.8f, 0.8f)
         ->SetSubPathStroke(0.4f, 0.1f, 0.6f, 1.0f, 4.0f)
         ->LineTo(280, 30)
         ->LineTo(280, 90)
         ->LineTo(220, 90)
         ->Close();
    
    panel->AddChild(path2);

    // 示例3: 对比 - 有描边vs无描边
    auto* label3 = new TextBlock();
    label3->Text("示例3: 对比效果(左侧无描边,右侧有描边)")->FontSize(14.0f);
    panel->AddChild(label3);

    auto* path3 = new Path();
    path3->Width(600)->Height(100);
    
    // 左侧:红色圆形,无描边
    path3->MoveTo(70, 50)
         ->SetSubPathFill(1.0f, 0.0f, 0.0f, 0.7f)
         ->SetSubPathStroke(0.0f, 0.0f, 0.0f, 0.0f, 0.0f); // 透明描边=无描边
    for (int i = 0; i < 16; ++i) {
        float angle = i * 3.14159f * 2 / 16;
        float x = 70 + 30 * std::cos(angle);
        float y = 50 + 30 * std::sin(angle);
        path3->LineTo(x, y);
    }
    path3->Close();
    
    // 右侧:蓝色圆形,白色粗描边
    path3->MoveTo(200, 50)
         ->SetSubPathFill(0.0f, 0.0f, 1.0f, 0.7f)
         ->SetSubPathStroke(1.0f, 1.0f, 1.0f, 1.0f, 4.0f); // 白色粗边
    for (int i = 0; i < 16; ++i) {
        float angle = i * 3.14159f * 2 / 16;
        float x = 200 + 30 * std::cos(angle);
        float y = 50 + 30 * std::sin(angle);
        path3->LineTo(x, y);
    }
    path3->Close();
    
    panel->AddChild(path3);

    application->Run(window);
    return 0;
}
