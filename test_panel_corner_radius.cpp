/**
 * @file test_panel_corner_radius.cpp
 * @brief 测试 Panel (StackPanel/Grid) 的 Background 和 CornerRadius 功能
 */

#include "fk/ui/Window.h"
#include "fk/ui/StackPanel.h"
#include "fk/ui/Grid.h"
#include "fk/ui/TextBlock.h"
#include "fk/ui/Border.h"
#include "fk/ui/Brush.h"
#include "fk/render/RenderList.h"
#include <iostream>
#include <iomanip>

using namespace fk;
using namespace fk::ui;
using namespace fk::render;

void PrintRectangleCommands(const RenderList& renderList) {
    const auto& commands = renderList.GetCommandBuffer().GetCommands();
    
    std::cout << "\n=== 渲染命令列表 ===\n";
    for (size_t i = 0; i < commands.size(); ++i) {
        const auto& cmd = commands[i];
        
        if (cmd.type == CommandType::DrawRectangle) {
            const auto& payload = std::get<RectanglePayload>(cmd.payload);
            std::cout << "命令 #" << i << ": DrawRectangle\n";
            std::cout << "  位置: (" << payload.rect.x << ", " << payload.rect.y << ")\n";
            std::cout << "  尺寸: " << payload.rect.width << " x " << payload.rect.height << "\n";
            std::cout << "  填充色: (" 
                     << payload.fillColor[0] << ", " 
                     << payload.fillColor[1] << ", " 
                     << payload.fillColor[2] << ", " 
                     << payload.fillColor[3] << ")\n";
            std::cout << "  圆角: topLeft=" << payload.cornerRadius.topLeft
                     << ", topRight=" << payload.cornerRadius.topRight
                     << ", bottomRight=" << payload.cornerRadius.bottomRight
                     << ", bottomLeft=" << payload.cornerRadius.bottomLeft << "\n\n";
        }
    }
}

int main() {
    std::cout << "=== 测试 Panel Background 和 CornerRadius ===\n\n";
    
    // 测试 1: StackPanel 带统一圆角
    {
        std::cout << "--- 测试 1: StackPanel 带统一圆角 ---\n";
        
        auto window = new Window();
        auto stackPanel = (new StackPanel())
            ->Background(new SolidColorBrush(Color::FromHex("#E0E0E0")))
            ->CornerRadius(10.0f)  // 统一圆角
            ->Width(200)
            ->Height(150)
            ->Children({
                (new TextBlock())->Text("Item 1"),
                (new TextBlock())->Text("Item 2")
            });
        
        window->SetContent(stackPanel);
        window->Measure(Size(800, 600));
        window->Arrange(Rect(0, 0, 800, 600));
        
        RenderList renderList;
        window->CollectDrawCommands(renderList);
        
        PrintRectangleCommands(renderList);
        
        delete window;
    }
    
    // 测试 2: Grid 带四角不同圆角
    {
        std::cout << "--- 测试 2: Grid 带四角不同圆角 ---\n";
        
        auto window = new Window();
        auto grid = (new Grid())
            ->Background(new SolidColorBrush(Color::FromHex("#FFE4B5")))
            ->CornerRadius(CornerRadius(20, 10, 5, 15))  // 四角不同
            ->Width(200)
            ->Height(150)
            ->Children({
                (new TextBlock())->Text("Grid Content")
            });
        
        window->SetContent(grid);
        window->Measure(Size(800, 600));
        window->Arrange(Rect(0, 0, 800, 600));
        
        RenderList renderList;
        window->CollectDrawCommands(renderList);
        
        PrintRectangleCommands(renderList);
        
        delete window;
    }
    
    // 测试 3: Border 带四角不同圆角
    {
        std::cout << "--- 测试 3: Border 带四角不同圆角 ---\n";
        
        auto window = new Window();
        auto border = (new Border())
            ->Background(new SolidColorBrush(Color::FromHex("#ADD8E6")))
            ->BorderBrush(new SolidColorBrush(Color::FromHex("#4682B4")))
            ->BorderThickness(Thickness(2))
            ->CornerRadius(CornerRadius(30, 15, 10, 5))  // 四角不同
            ->Width(200)
            ->Height(150)
            ->Child((new TextBlock())->Text("Border Content"));
        
        window->SetContent(border);
        window->Measure(Size(800, 600));
        window->Arrange(Rect(0, 0, 800, 600));
        
        RenderList renderList;
        window->CollectDrawCommands(renderList);
        
        PrintRectangleCommands(renderList);
        
        delete window;
    }
    
    // 测试 4: 嵌套测试 - Border 中包含 StackPanel
    {
        std::cout << "--- 测试 4: 嵌套 Border 和 StackPanel ---\n";
        
        auto window = new Window();
        auto border = (new Border())
            ->Background(new SolidColorBrush(Color::FromHex("#F0F0F0")))
            ->CornerRadius(CornerRadius(15, 15, 5, 5))
            ->Padding(Thickness(10))
            ->Width(250)
            ->Height(200)
            ->Child(
                (new StackPanel())
                    ->Background(new SolidColorBrush(Color::FromHex("#FFF8DC")))
                    ->CornerRadius(5.0f)
                    ->Children({
                        (new TextBlock())->Text("Header"),
                        (new TextBlock())->Text("Body"),
                        (new TextBlock())->Text("Footer")
                    })
            );
        
        window->SetContent(border);
        window->Measure(Size(800, 600));
        window->Arrange(Rect(0, 0, 800, 600));
        
        RenderList renderList;
        window->CollectDrawCommands(renderList);
        
        PrintRectangleCommands(renderList);
        
        delete window;
    }
    
    std::cout << "=== 所有测试完成 ===\n";
    return 0;
}
