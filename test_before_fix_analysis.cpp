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
    std::cout << "=== 分析修复前的实际渲染位置 ===" << std::endl;
    
    auto* button = new Button();
    button->Content("Click Me");
    button->ApplyTemplate();
    
    button->Measure(Size(200, 50));
    button->Arrange(Rect(0, 0, 200, 50));
    
    render::RenderList renderList;
    render::TextRenderer textRenderer;
    render::RenderContext context(&renderList, &textRenderer);
    
    button->CollectDrawCommands(context);
    
    std::cout << "\n=== RenderContext 生成的命令（payload 坐标）===" << std::endl;
    
    const auto& commands = renderList.GetCommands();
    float borderY = -1, textY = -1;
    
    for (size_t i = 0; i < commands.size(); ++i) {
        const auto& cmd = commands[i];
        
        if (cmd.type == render::CommandType::DrawRectangle) {
            const auto& payload = std::get<render::RectanglePayload>(cmd.payload);
            std::cout << "DrawRectangle (Border):" << std::endl;
            std::cout << "  payload.rect.y = " << payload.rect.y << std::endl;
            borderY = payload.rect.y;
        } else if (cmd.type == render::CommandType::DrawText) {
            const auto& payload = std::get<render::TextPayload>(cmd.payload);
            std::cout << "\nDrawText (TextBlock):" << std::endl;
            std::cout << "  payload.bounds.y = " << payload.bounds.y << std::endl;
            textY = payload.bounds.y;
        }
    }
    
    std::cout << "\n=== 修复前的着色器行为分析 ===" << std::endl;
    std::cout << "修复前着色器：vec2 pos = aPos + uOffset" << std::endl;
    std::cout << "\n关键问题：着色器在什么时候加 uOffset？" << std::endl;
    std::cout << "\n让我们追踪 SetTransform 命令：" << std::endl;
    
    float currentOffset = 0;
    bool foundBorder = false, foundText = false;
    float borderRenderY = -1, textRenderY = -1;
    
    for (size_t i = 0; i < commands.size(); ++i) {
        const auto& cmd = commands[i];
        
        if (cmd.type == render::CommandType::SetTransform) {
            const auto& payload = std::get<render::TransformPayload>(cmd.payload);
            currentOffset = payload.offsetY;
        } else if (cmd.type == render::CommandType::DrawRectangle && !foundBorder) {
            const auto& payload = std::get<render::RectanglePayload>(cmd.payload);
            borderRenderY = payload.rect.y + currentOffset;
            std::cout << "\nBorder 渲染：" << std::endl;
            std::cout << "  - payload.rect.y = " << payload.rect.y << std::endl;
            std::cout << "  - 当前 uOffset = " << currentOffset << std::endl;
            std::cout << "  - 着色器计算：pos.y = " << payload.rect.y << " + " << currentOffset << " = " << borderRenderY << std::endl;
            foundBorder = true;
        } else if (cmd.type == render::CommandType::DrawText && !foundText) {
            const auto& payload = std::get<render::TextPayload>(cmd.payload);
            textRenderY = payload.bounds.y + currentOffset;
            std::cout << "\nTextBlock 渲染：" << std::endl;
            std::cout << "  - payload.bounds.y = " << payload.bounds.y << std::endl;
            std::cout << "  - 当前 uOffset = " << currentOffset << std::endl;
            std::cout << "  - 着色器计算：pos.y = " << payload.bounds.y << " + " << currentOffset << " = " << textRenderY << std::endl;
            foundText = true;
        }
    }
    
    std::cout << "\n=== 结论 ===" << std::endl;
    std::cout << "修复前屏幕上的实际 y 坐标：" << std::endl;
    std::cout << "  Border:    y = " << borderRenderY << std::endl;
    std::cout << "  TextBlock: y = " << textRenderY << std::endl;
    
    if (borderRenderY > textRenderY) {
        std::cout << "\n✓ Border 的 y 坐标（" << borderRenderY << "）> TextBlock 的 y 坐标（" << textRenderY << "）" << std::endl;
        std::cout << "  这意味着 Border 在屏幕上位于 TextBlock **下方**" << std::endl;
        std::cout << "  用户的观察是正确的！" << std::endl;
    } else {
        std::cout << "\n✗ Border 的 y 坐标（" << borderRenderY << "）< TextBlock 的 y 坐标（" << textRenderY << "）" << std::endl;
        std::cout << "  这意味着 Border 在屏幕上位于 TextBlock **上方**" << std::endl;
    }
    
    std::cout << "\n修复后屏幕上的实际 y 坐标：" << std::endl;
    std::cout << "  Border:    y = " << borderY << " (payload 直接使用)" << std::endl;
    std::cout << "  TextBlock: y = " << textY << " (payload 直接使用)" << std::endl;
    
    return 0;
}
