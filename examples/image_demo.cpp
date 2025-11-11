#include "fk/app/Application.h"
#include "fk/ui/Window.h"
#include "fk/ui/StackPanel.h"
#include "fk/ui/Image.h"
#include "fk/ui/TextBlock.h"
#include "fk/ui/Border.h"

using namespace fk;
using namespace fk::ui;

int main() {
    Application app;
    
    // 创建主窗口（由 Application 管理生命周期）
    auto* mainWindow = app.CreateWindow();
    mainWindow->Title("Image Control Demo")
              ->Width(800.0f)
              ->Height(400.0f);
    
    // 创建垂直布局面板
    auto* panel = new StackPanel();
    panel->Orientation(Orientation::Vertical)
         ->Spacing(10.0f)
         ->Margin(Thickness(20));
    
    // 标题
    auto* title = new TextBlock();
    title->Text("Image Control Test")
         ->FontSize(24.0f)
         ->Margin(Thickness(0, 0, 0, 20));
    
    // Image with Stretch::None
    auto* image1 = new Image();
    image1->Source("test_image.png")
          ->Stretch(Stretch::None)
          ->Width(200.0f)
          ->Height(200.0f);
    
    auto* border1 = new Border();
    border1->Child(image1)
           ->BorderThickness(1.0f)
           ->Margin(Thickness(0, 0, 0, 10));
    
    auto* label1 = new TextBlock();
    label1->Text("Stretch::None (200x200)")
          ->Margin(Thickness(0, 0, 0, 20));
    
    // Image with Stretch::Uniform
    auto* image2 = new Image();
    image2->Source("test_image.png")
          ->Stretch(Stretch::Uniform)
          ->Width(300.0f)
          ->Height(150.0f);
    
    auto* border2 = new Border();
    border2->Child(image2)
           ->BorderThickness(1.0f)
           ->Margin(Thickness(0, 0, 0, 10));
    
    auto* label2 = new TextBlock();
    label2->Text("Stretch::Uniform (300x150)");
    
    // 添加所有元素到面板
    panel->AddChild(title);
    panel->AddChild(border1);
    panel->AddChild(label1);
    panel->AddChild(border2);
    panel->AddChild(label2);
    
    // 设置窗口内容
    mainWindow->SetContent(panel);
    
    // 运行应用程序（CreateWindow 返回的指针由 Application 管理）
    app.Run(app.GetMainWindow());
    return 0;
}
