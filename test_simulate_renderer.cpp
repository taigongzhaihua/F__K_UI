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

int main() {
    std::cout << "=== 模拟渲染器处理命令 ===" << std::endl;
    
    // 模拟example/main.cpp的场景
    auto* panel = new StackPanel();
    auto* button = new Button();
    button->Content("Click Me");
    panel->AddChild(button);
    
    panel->Measure(Size(800, 600));
    panel->Arrange(Rect(0, 0, 800, 600));
    
    std::cout << "\n布局信息：" << std::endl;
    std::cout << "Button layoutRect: (" << button->GetLayoutRect().x << ", " 
              << button->GetLayoutRect().y << ", "
              << button->GetLayoutRect().width << ", "
              << button->GetLayoutRect().height << ")" << std::endl;
    
    if (button->GetVisualChildrenCount() > 0) {
        auto* border = dynamic_cast<Border*>(button->GetVisualChild(0));
        if (border) {
            std::cout << "Border layoutRect: (" << border->GetLayoutRect().x << ", " 
                      << border->GetLayoutRect().y << ")" << std::endl;
            
            auto* cp = dynamic_cast<UIElement*>(border->GetChild());
            if (cp) {
                std::cout << "ContentPresenter layoutRect: (" << cp->GetLayoutRect().x << ", " 
                          << cp->GetLayoutRect().y << ")" << std::endl;
                          
                if (cp->GetVisualChildrenCount() > 0) {
                    auto* tb = dynamic_cast<TextBlock*>(cp->GetVisualChild(0));
                    if (tb) {
                        std::cout << "TextBlock layoutRect: (" << tb->GetLayoutRect().x << ", " 
                                  << tb->GetLayoutRect().y << ")" << std::endl;
                    }
                }
            }
        }
    }
    
    render::RenderList renderList;
    render::TextRenderer textRenderer;
    render::RenderContext context(&renderList, &textRenderer);
    
    panel->CollectDrawCommands(context);
    
    std::cout << "\n=== 模拟渲染器执行（模拟着色器行为：pos = aPos + uOffset）===" << std::endl;
    
    const auto& commands = renderList.GetCommands();
    float uOffsetX = 0, uOffsetY = 0;
    
    for (size_t i = 0; i < commands.size(); ++i) {
        const auto& cmd = commands[i];
        
        if (cmd.type == render::CommandType::SetTransform) {
            const auto& payload = std::get<render::TransformPayload>(cmd.payload);
            uOffsetX = payload.offsetX;
            uOffsetY = payload.offsetY;
            std::cout << "命令 #" << i << ": SetTransform -> uOffset = (" 
                      << uOffsetX << ", " << uOffsetY << ")" << std::endl;
                      
        } else if (cmd.type == render::CommandType::DrawRectangle) {
            const auto& payload = std::get<render::RectanglePayload>(cmd.payload);
            float finalX = payload.rect.x + uOffsetX;
            float finalY = payload.rect.y + uOffsetY;
            std::cout << "命令 #" << i << ": DrawRectangle" << std::endl;
            std::cout << "  payload坐标: (" << payload.rect.x << ", " << payload.rect.y << ")" << std::endl;
            std::cout << "  uOffset: (" << uOffsetX << ", " << uOffsetY << ")" << std::endl;
            std::cout << "  **最终渲染位置**: (" << finalX << ", " << finalY << ")" << std::endl;
            
        } else if (cmd.type == render::CommandType::DrawText) {
            const auto& payload = std::get<render::TextPayload>(cmd.payload);
            float finalX = payload.bounds.x + uOffsetX;
            float finalY = payload.bounds.y + uOffsetY;
            std::cout << "命令 #" << i << ": DrawText '" << payload.text << "'" << std::endl;
            std::cout << "  payload坐标: (" << payload.bounds.x << ", " << payload.bounds.y << ")" << std::endl;
            std::cout << "  uOffset: (" << uOffsetX << ", " << uOffsetY << ")" << std::endl;
            std::cout << "  **最终渲染位置**: (" << finalX << ", " << finalY << ")" << std::endl;
        }
    }
    
    return 0;
}
