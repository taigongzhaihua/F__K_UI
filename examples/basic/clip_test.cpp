#include <fk/app/Application.h>
#include <fk/ui/Window.h>
#include <fk/ui/Grid.h>
#include <fk/ui/Border.h>
#include <fk/ui/StackPanel.h>
#include <fk/ui/TextBlock.h>
#include <fk/ui/Brush.h>
#include <fk/ui/GridCellAttacher.h>

using namespace fk;
using namespace fk::ui;

int main() {
    Application app;

    auto window = std::make_shared<Window>();
    window->SetTitle("Clipping Test - 裁剪测试");
    window->SetBackground(new SolidColorBrush(240, 240, 240, 255));

    auto grid = std::make_shared<Grid>();
    grid->Rows("1*, 1*");
    grid->Columns("1*");

    // 第一个测试：StackPanel with ClipToBounds = true，嵌套超大子元素
    auto panel1 = std::make_shared<StackPanel>();
    panel1->SetBackground(new SolidColorBrush(200, 200, 255, 255));
    panel1->SetWidth(300);
    panel1->SetHeight(150);
    panel1->SetClipToBounds(true);  // 启用裁剪
    
    auto innerBorder1 = std::make_shared<Border>();
    innerBorder1->SetBackground(new SolidColorBrush(255, 100, 100, 255));
    innerBorder1->SetWidth(400);  // 超出父元素宽度
    innerBorder1->SetHeight(200); // 超出父元素高度
    
    auto text1 = std::make_shared<TextBlock>();
    text1->SetText("CLIPPED: This panel and border exceed parent bounds");
    text1->SetForeground(new SolidColorBrush(255, 255, 255, 255));
    text1->SetFontSize(16);
    innerBorder1->SetChild(text1.get());
    
    panel1->AddChild(innerBorder1.get());
    cell(0, 0).ApplyTo(panel1.get());

    // 第二个测试：StackPanel without ClipToBounds (default)
    auto panel2 = std::make_shared<StackPanel>();
    panel2->SetBackground(new SolidColorBrush(255, 200, 200, 255));
    panel2->SetWidth(300);
    panel2->SetHeight(150);
    // panel2->SetClipToBounds(false); // 默认不裁剪
    
    auto innerBorder2 = std::make_shared<Border>();
    innerBorder2->SetBackground(new SolidColorBrush(100, 255, 100, 255));
    innerBorder2->SetWidth(400);  // 超出父元素宽度
    innerBorder2->SetHeight(200); // 超出父元素高度
    
    auto text2 = std::make_shared<TextBlock>();
    text2->SetText("NOT CLIPPED: This overflows parent bounds");
    text2->SetForeground(new SolidColorBrush(0, 0, 0, 255));
    text2->SetFontSize(16);
    innerBorder2->SetChild(text2.get());
    
    panel2->AddChild(innerBorder2.get());
    cell(1, 0).ApplyTo(panel2.get());

    grid->AddChild(panel1.get());
    grid->AddChild(panel2.get());
    
    window->SetContent(std::any(grid.get()));
    app.Run(window);

    return 0;
}
