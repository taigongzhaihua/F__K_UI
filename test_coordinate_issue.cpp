#include "fk/ui/Button.h"
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

int main() {
    std::cout << "=== 测试 Border 和 TextBlock 的 Y 坐标 ===" << std::endl;
    
    auto* button = new Button();
    button->Content("Click Me");
    button->ApplyTemplate();
    
    // 测量和排列
    button->Measure(Size(200, 50));
    button->Arrange(Rect(0, 0, 200, 50));
    
    // 打印布局信息
    std::cout << "\nButton layoutRect: " 
              << button->GetLayoutRect().x << ", " 
              << button->GetLayoutRect().y << std::endl;
    
    if (button->GetVisualChildrenCount() > 0) {
        auto* border = dynamic_cast<Border*>(button->GetVisualChild(0));
        if (border) {
            std::cout << "Border layoutRect: " 
                      << border->GetLayoutRect().x << ", " 
                      << border->GetLayoutRect().y << std::endl;
            
            auto* child = border->GetChild();
            if (child) {
                auto* uiChild = dynamic_cast<UIElement*>(child);
                std::cout << "ContentPresenter layoutRect: " 
                          << uiChild->GetLayoutRect().x << ", " 
                          << uiChild->GetLayoutRect().y << std::endl;
                
                if (uiChild->GetVisualChildrenCount() > 0) {
                    auto* textBlock = dynamic_cast<TextBlock*>(uiChild->GetVisualChild(0));
                    if (textBlock) {
                        std::cout << "TextBlock layoutRect: " 
                                  << textBlock->GetLayoutRect().x << ", " 
                                  << textBlock->GetLayoutRect().y << std::endl;
                    }
                }
            }
        }
    }
    
    // 生成渲染命令并查看坐标
    render::RenderList renderList;
    render::TextRenderer textRenderer;
    render::RenderContext context(&renderList, &textRenderer);
    
    button->CollectDrawCommands(context);
    
    std::cout << "\n=== 渲染命令 ===" << std::endl;
    std::cout << "命令总数: " << renderList.GetCommandCount() << std::endl;
    
    const auto& commands = renderList.GetCommands();
    for (size_t i = 0; i < commands.size(); ++i) {
        const auto& cmd = commands[i];
        if (cmd.type == render::CommandType::DrawRectangle) {
            const auto& payload = std::get<render::RectanglePayload>(cmd.payload);
            std::cout << "\nDrawRectangle 命令 #" << i << ":" << std::endl;
            std::cout << "  rect: (" << payload.rect.x << ", " << payload.rect.y 
                      << ", " << payload.rect.width << ", " << payload.rect.height << ")" << std::endl;
        } else if (cmd.type == render::CommandType::DrawText) {
            const auto& payload = std::get<render::TextPayload>(cmd.payload);
            std::cout << "\nDrawText 命令 #" << i << ":" << std::endl;
            std::cout << "  position: (" << payload.bounds.x << ", " << payload.bounds.y << ")" << std::endl;
            std::cout << "  text: '" << payload.text << "'" << std::endl;
        } else if (cmd.type == render::CommandType::SetTransform) {
            const auto& payload = std::get<render::TransformPayload>(cmd.payload);
            std::cout << "\nSetTransform 命令 #" << i << ":" << std::endl;
            std::cout << "  offset: (" << payload.offsetX << ", " << payload.offsetY << ")" << std::endl;
        }
    }
    
    return 0;
}
