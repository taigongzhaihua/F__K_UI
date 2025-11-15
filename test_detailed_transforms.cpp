#include "fk/ui/Button.h"
#include "fk/ui/TextBlock.h"
#include "fk/ui/Border.h"
#include "fk/ui/Brush.h"
#include "fk/render/RenderContext.h"
#include "fk/render/RenderList.h"
#include "fk/render/TextRenderer.h"
#include "fk/render/RenderCommand.h"
#include <iostream>
#include <stack>

using namespace fk;
using namespace fk::ui;

int main() {
    std::cout << "=== 详细分析变换栈 ===" << std::endl;
    
    auto* button = new Button();
    button->Content("Click Me");
    button->ApplyTemplate();
    
    button->Measure(Size(200, 50));
    button->Arrange(Rect(0, 0, 200, 50));
    
    render::RenderList renderList;
    render::TextRenderer textRenderer;
    render::RenderContext context(&renderList, &textRenderer);
    
    button->CollectDrawCommands(context);
    
    const auto& commands = renderList.GetCommands();
    
    // 模拟渲染器的变换栈处理
    std::stack<std::pair<float, float>> transformStack;
    float currentX = 0, currentY = 0;
    int depth = 0;
    
    std::cout << "\n模拟渲染器执行：\n" << std::endl;
    
    for (size_t i = 0; i < commands.size(); ++i) {
        const auto& cmd = commands[i];
        
        if (cmd.type == render::CommandType::SetTransform) {
            const auto& payload = std::get<render::TransformPayload>(cmd.payload);
            
            // 检查这是 push 还是 pop
            // SetTransform实际上设置的是绝对变换，不是增量
            float newX = payload.offsetX;
            float newY = payload.offsetY;
            
            std::string indent(depth * 2, ' ');
            
            if (newX > currentX || newY > currentY) {
                // 这是一个push（增加了偏移）
                std::cout << indent << "命令 #" << i << ": Push变换 (" 
                          << newX - currentX << ", " << newY - currentY << ")" << std::endl;
                std::cout << indent << "  -> 累积变换现在是 (" << newX << ", " << newY << ")" << std::endl;
                transformStack.push({currentX, currentY});
                depth++;
            } else {
                // 这是一个pop（恢复之前的状态）
                depth--;
                std::cout << indent << "命令 #" << i << ": Pop变换" << std::endl;
                std::cout << indent << "  -> 累积变换恢复到 (" << newX << ", " << newY << ")" << std::endl;
            }
            
            currentX = newX;
            currentY = newY;
            
        } else if (cmd.type == render::CommandType::DrawRectangle) {
            const auto& payload = std::get<render::RectanglePayload>(cmd.payload);
            std::string indent(depth * 2, ' ');
            std::cout << indent << "命令 #" << i << ": DrawRectangle" << std::endl;
            std::cout << indent << "  位置(全局坐标): (" << payload.rect.x << ", " << payload.rect.y << ")" << std::endl;
            std::cout << indent << "  当前累积变换: (" << currentX << ", " << currentY << ")" << std::endl;
            std::cout << indent << "  实际渲染位置应该是: (" << payload.rect.x << ", " << payload.rect.y << ")" << std::endl;
            
        } else if (cmd.type == render::CommandType::DrawText) {
            const auto& payload = std::get<render::TextPayload>(cmd.payload);
            std::string indent(depth * 2, ' ');
            std::cout << indent << "命令 #" << i << ": DrawText '" << payload.text << "'" << std::endl;
            std::cout << indent << "  位置(全局坐标): (" << payload.bounds.x << ", " << payload.bounds.y << ")" << std::endl;
            std::cout << indent << "  当前累积变换: (" << currentX << ", " << currentY << ")" << std::endl;
            std::cout << indent << "  实际渲染位置应该是: (" << payload.bounds.x << ", " << payload.bounds.y << ")" << std::endl;
        }
    }
    
    std::cout << "\n=== 关键发现 ===" << std::endl;
    std::cout << "RenderContext已经在调用DrawRectangle和DrawText之前" << std::endl;
    std::cout << "应用了TransformRect和TransformPoint，" << std::endl;
    std::cout << "所以payload中的坐标已经是全局坐标了。" << std::endl;
    std::cout << "\n因此SetTransform命令可能是给渲染器用来管理其他状态的，" << std::endl;
    std::cout << "但绘制命令中的坐标已经是最终位置。" << std::endl;
    
    return 0;
}
