#include "fk/ui/Button.h"
#include "fk/ui/StackPanel.h"
#include "fk/ui/TextBlock.h"
#include "fk/ui/Border.h"
#include "fk/ui/Brush.h"
#include "fk/render/RenderContext.h"
#include "fk/render/RenderList.h"
#include "fk/render/TextRenderer.h"
#include "fk/render/RenderCommand.h"
#include <iostream>

using namespace fk;
using namespace fk::ui;

// 计算全局坐标的辅助函数
void printGlobalCoordinates(UIElement* element, float parentX, float parentY, const std::string& name, int depth = 0) {
    if (!element) return;
    
    std::string indent(depth * 2, ' ');
    auto layoutRect = element->GetLayoutRect();
    float globalX = parentX + layoutRect.x;
    float globalY = parentY + layoutRect.y;
    
    std::cout << indent << name << ":" << std::endl;
    std::cout << indent << "  局部坐标: (" << layoutRect.x << ", " << layoutRect.y << ")" << std::endl;
    std::cout << indent << "  全局坐标: (" << globalX << ", " << globalY << ")" << std::endl;
    
    // 递归打印子元素
    if (auto* button = dynamic_cast<Button*>(element)) {
        if (button->GetVisualChildrenCount() > 0) {
            auto* border = dynamic_cast<Border*>(button->GetVisualChild(0));
            if (border) {
                printGlobalCoordinates(border, globalX, globalY, "Border", depth + 1);
                
                auto* cp = dynamic_cast<UIElement*>(border->GetChild());
                if (cp) {
                    auto cpRect = cp->GetLayoutRect();
                    printGlobalCoordinates(cp, globalX + cpRect.x, globalY + cpRect.y, "ContentPresenter", depth + 2);
                    
                    if (cp->GetVisualChildrenCount() > 0) {
                        auto* tb = dynamic_cast<TextBlock*>(cp->GetVisualChild(0));
                        if (tb) {
                            auto tbRect = tb->GetLayoutRect();
                            printGlobalCoordinates(tb, globalX + cpRect.x + tbRect.x, globalY + cpRect.y + tbRect.y, "TextBlock", depth + 3);
                        }
                    }
                }
            }
        }
    }
}

int main() {
    std::cout << "=== 完整场景中的全局坐标分析 ===" << std::endl;
    
    // 创建和 example/main.cpp 类似的场景
    auto* panel = new StackPanel();
    
    auto* textBlock1 = new TextBlock();
    textBlock1->Text("Hello, F K UI!")
              ->FontSize(32)
              ->Foreground(Brushes::Blue())
              ->Margin(Thickness(20));
    
    auto* textBlock2 = new TextBlock();
    textBlock2->Text("This is a simple example of F K UI framework.")
              ->FontSize(16)
              ->Foreground(Brushes::DarkGray())
              ->Margin(Thickness(20));
    
    auto* button = new Button();
    button->Content("Click Me");
    
    panel->AddChild(textBlock1);
    panel->AddChild(textBlock2);
    panel->AddChild(button);
    
    panel->Measure(Size(800, 600));
    panel->Arrange(Rect(0, 0, 800, 600));
    
    std::cout << "\n=== 布局后的全局坐标 ===" << std::endl;
    printGlobalCoordinates(button, 0, button->GetLayoutRect().y, "Button", 0);
    
    // 生成渲染命令
    render::RenderList renderList;
    render::TextRenderer textRenderer;
    render::RenderContext context(&renderList, &textRenderer);
    
    panel->CollectDrawCommands(context);
    
    std::cout << "\n=== 渲染命令中的坐标 ===" << std::endl;
    
    const auto& commands = renderList.GetCommands();
    for (size_t i = 0; i < commands.size(); ++i) {
        const auto& cmd = commands[i];
        
        if (cmd.type == render::CommandType::DrawRectangle) {
            const auto& payload = std::get<render::RectanglePayload>(cmd.payload);
            if (payload.rect.y > 100) {  // 只显示 Button 的矩形
                std::cout << "DrawRectangle (Button的Border):" << std::endl;
                std::cout << "  payload 坐标: (" << payload.rect.x << ", " << payload.rect.y << ")" << std::endl;
                std::cout << "  这就是 Border 的全局渲染位置" << std::endl;
            }
        } else if (cmd.type == render::CommandType::DrawText) {
            const auto& payload = std::get<render::TextPayload>(cmd.payload);
            if (payload.text == "Click Me") {
                std::cout << "\nDrawText (Button内的TextBlock):" << std::endl;
                std::cout << "  payload 坐标: (" << payload.bounds.x << ", " << payload.bounds.y << ")" << std::endl;
                std::cout << "  这就是 TextBlock 的全局渲染位置" << std::endl;
            }
        }
    }
    
    std::cout << "\n=== 分析 ===" << std::endl;
    std::cout << "在这个完整场景中：" << std::endl;
    std::cout << "- Button 位于 y=117.6（由 StackPanel 布局决定）" << std::endl;
    std::cout << "- Border 全局坐标：y=117.6（与 Button 相同）" << std::endl;
    std::cout << "- TextBlock 全局坐标：y=117.6+6=123.6（相对 Button 偏移 6）" << std::endl;
    std::cout << "\n修复前，如果有双重变换：" << std::endl;
    std::cout << "- TextBlock 会被渲染在 y=123.6+6=129.6（错误）" << std::endl;
    std::cout << "\n所以问题不是'border的坐标比textblock大'" << std::endl;
    std::cout << "而是：TextBlock 的 y 坐标被错误地加了两次偏移" << std::endl;
    
    return 0;
}
