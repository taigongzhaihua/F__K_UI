#include "fk/ui/StackPanel.h"
#include "fk/ui/Button.h"
#include "fk/ui/TextBlock.h"
#include "fk/ui/Border.h"
#include "fk/ui/Brush.h"
#include "fk/render/RenderContext.h"
#include "fk/render/RenderList.h"
#include "fk/render/TextRenderer.h"
#include "fk/render/RenderCommand.h"
#include <iostream>
#include <iomanip>

using namespace fk;
using namespace fk::ui;

void PrintRenderCommands(const render::RenderList& renderList) {
    const auto& commands = renderList.GetCommands();
    std::cout << "\n========== 渲染命令详情 ==========" << std::endl;
    std::cout << "总命令数: " << commands.size() << std::endl;
    
    float currentX = 0, currentY = 0;
    
    for (size_t i = 0; i < commands.size(); ++i) {
        const auto& cmd = commands[i];
        
        if (cmd.type == render::CommandType::SetTransform) {
            const auto& payload = std::get<render::TransformPayload>(cmd.payload);
            currentX = payload.offsetX;
            currentY = payload.offsetY;
            std::cout << "\n[" << std::setw(3) << i << "] SetTransform: "
                      << "offset=(" << currentX << ", " << currentY << ")" << std::endl;
        }
        else if (cmd.type == render::CommandType::DrawText) {
            const auto& payload = std::get<render::TextPayload>(cmd.payload);
            std::cout << "\n[" << std::setw(3) << i << "] DrawText:" << std::endl;
            std::cout << "      文本: \"" << payload.text << "\"" << std::endl;
            std::cout << "      位置: (" << payload.bounds.x << ", " << payload.bounds.y << ")" << std::endl;
            std::cout << "      当前变换: (" << currentX << ", " << currentY << ")" << std::endl;
            
            // 检查位置是否匹配当前变换
            if (payload.bounds.x == 0 && payload.bounds.y == 0) {
                std::cout << "      ⚠️  警告: 文本位置在(0,0)，当前变换为(" << currentX << ", " << currentY << ")" << std::endl;
            } else if (std::abs(payload.bounds.x - currentX) < 0.01f && 
                       std::abs(payload.bounds.y - currentY) < 0.01f) {
                std::cout << "      ✓ 位置与当前变换匹配" << std::endl;
            }
        }
        else if (cmd.type == render::CommandType::DrawRectangle) {
            const auto& payload = std::get<render::RectanglePayload>(cmd.payload);
            std::cout << "\n[" << std::setw(3) << i << "] DrawRectangle: "
                      << "rect=(" << payload.rect.x << ", " << payload.rect.y << ", "
                      << payload.rect.width << ", " << payload.rect.height << ")" << std::endl;
        }
    }
    std::cout << "\n========== 命令列表结束 ==========\n" << std::endl;
}

int main() {
    std::cout << "===== TextBlock 位置可视化检查 =====" << std::endl;
    
    // 场景1: 简单的StackPanel + TextBlocks
    std::cout << "\n【场景1】StackPanel + 两个TextBlock" << std::endl;
    {
        auto* panel = new StackPanel();
        
        auto* text1 = new TextBlock();
        text1->Text("Hello World");
        text1->FontSize(20);
        text1->Margin(Thickness(10));
        
        auto* text2 = new TextBlock();
        text2->Text("Second Line");
        text2->FontSize(20);
        text2->Margin(Thickness(10));
        
        panel->Children({text1, text2});
        
        panel->Measure(Size(400, 300));
        panel->Arrange(Rect(0, 0, 400, 300));
        
        std::cout << "布局结果:" << std::endl;
        std::cout << "  Text1 layoutRect: (" << text1->GetLayoutRect().x << ", " 
                  << text1->GetLayoutRect().y << ")" << std::endl;
        std::cout << "  Text2 layoutRect: (" << text2->GetLayoutRect().x << ", " 
                  << text2->GetLayoutRect().y << ")" << std::endl;
        
        render::RenderList renderList;
        render::TextRenderer textRenderer;
        render::RenderContext context(&renderList, &textRenderer);
        
        panel->CollectDrawCommands(context);
        PrintRenderCommands(renderList);
        
        delete panel;
    }
    
    // 场景2: Button + TextBlock（模拟example/main.cpp）
    std::cout << "\n【场景2】Button + TextBlock（带模板）" << std::endl;
    {
        auto* button = new Button();
        button->Content("Click Me");
        button->Background(Brushes::Green());
        button->Margin(Thickness(20, 100, 0, 0));  // 明显的偏移
        
        button->ApplyTemplate();
        button->Measure(Size(400, 300));
        button->Arrange(Rect(20, 100, 200, 50));
        
        std::cout << "布局结果:" << std::endl;
        std::cout << "  Button layoutRect: (" << button->GetLayoutRect().x << ", " 
                  << button->GetLayoutRect().y << ")" << std::endl;
        
        // 找到模板内的TextBlock
        if (button->GetVisualChildrenCount() > 0) {
            auto* border = dynamic_cast<Border*>(button->GetVisualChild(0));
            if (border && border->GetChild()) {
                auto* presenter = dynamic_cast<UIElement*>(border->GetChild());
                if (presenter && presenter->GetVisualChildrenCount() > 0) {
                    auto* textBlock = dynamic_cast<TextBlock*>(presenter->GetVisualChild(0));
                    if (textBlock) {
                        std::cout << "  内部TextBlock layoutRect: (" 
                                  << textBlock->GetLayoutRect().x << ", " 
                                  << textBlock->GetLayoutRect().y << ")" << std::endl;
                    }
                }
            }
        }
        
        render::RenderList renderList;
        render::TextRenderer textRenderer;
        render::RenderContext context(&renderList, &textRenderer);
        
        button->CollectDrawCommands(context);
        PrintRenderCommands(renderList);
        
        delete button;
    }
    
    // 场景3: 模拟完整的example/main.cpp场景
    std::cout << "\n【场景3】完整场景：StackPanel + TextBlocks + Button" << std::endl;
    {
        auto* panel = new StackPanel();
        
        auto* text1 = new TextBlock();
        text1->Text("Hello, F K UI!");
        text1->FontSize(32);
        text1->Foreground(Brushes::Blue());
        text1->Margin(Thickness(20));
        
        auto* text2 = new TextBlock();
        text2->Text("This is a simple example.");
        text2->FontSize(16);
        text2->Margin(Thickness(20));
        
        auto* button = new Button();
        button->Content("Click Me");
        button->Background(Brushes::Green());
        button->ApplyTemplate();
        
        panel->Children({text1, text2, button});
        
        panel->Measure(Size(800, 600));
        panel->Arrange(Rect(0, 0, 800, 600));
        
        std::cout << "布局结果:" << std::endl;
        std::cout << "  Text1 layoutRect: (" << text1->GetLayoutRect().x << ", " 
                  << text1->GetLayoutRect().y << ")" << std::endl;
        std::cout << "  Text2 layoutRect: (" << text2->GetLayoutRect().x << ", " 
                  << text2->GetLayoutRect().y << ")" << std::endl;
        std::cout << "  Button layoutRect: (" << button->GetLayoutRect().x << ", " 
                  << button->GetLayoutRect().y << ")" << std::endl;
        
        render::RenderList renderList;
        render::TextRenderer textRenderer;
        render::RenderContext context(&renderList, &textRenderer);
        
        panel->CollectDrawCommands(context);
        PrintRenderCommands(renderList);
        
        delete panel;
    }
    
    std::cout << "\n===== 测试完成 =====" << std::endl;
    std::cout << "\n分析结论：" << std::endl;
    std::cout << "如果所有DrawText命令的位置都是(0, 0)，则存在问题。" << std::endl;
    std::cout << "如果DrawText命令的位置与layoutRect匹配，则代码正确。" << std::endl;
    
    return 0;
}
