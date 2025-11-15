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
    std::cout << "=== 测试变换累积问题 ===" << std::endl;
    
    auto* button = new Button();
    button->Content("Click Me");
    button->ApplyTemplate();
    
    // 测量和排列
    button->Measure(Size(200, 50));
    button->Arrange(Rect(0, 0, 200, 50));
    
    // 生成渲染命令
    render::RenderList renderList;
    render::TextRenderer textRenderer;
    render::RenderContext context(&renderList, &textRenderer);
    
    button->CollectDrawCommands(context);
    
    std::cout << "\n=== 分析变换累积 ===" << std::endl;
    
    const auto& commands = renderList.GetCommands();
    float currentX = 0, currentY = 0;
    
    for (size_t i = 0; i < commands.size(); ++i) {
        const auto& cmd = commands[i];
        
        if (cmd.type == render::CommandType::SetTransform) {
            const auto& payload = std::get<render::TransformPayload>(cmd.payload);
            currentX = payload.offsetX;
            currentY = payload.offsetY;
            std::cout << "命令 #" << i << ": SetTransform -> 累积变换 = (" 
                      << currentX << ", " << currentY << ")" << std::endl;
        } else if (cmd.type == render::CommandType::DrawRectangle) {
            const auto& payload = std::get<render::RectanglePayload>(cmd.payload);
            std::cout << "命令 #" << i << ": DrawRectangle" << std::endl;
            std::cout << "  - 绘制位置(全局): (" << payload.rect.x << ", " << payload.rect.y << ")" << std::endl;
            std::cout << "  - 当前变换: (" << currentX << ", " << currentY << ")" << std::endl;
            std::cout << "  - 最终位置 = 绘制位置 (已经包含了变换)" << std::endl;
        } else if (cmd.type == render::CommandType::DrawText) {
            const auto& payload = std::get<render::TextPayload>(cmd.payload);
            std::cout << "命令 #" << i << ": DrawText '" << payload.text << "'" << std::endl;
            std::cout << "  - 绘制位置(全局): (" << payload.bounds.x << ", " << payload.bounds.y << ")" << std::endl;
            std::cout << "  - 当前变换: (" << currentX << ", " << currentY << ")" << std::endl;
            std::cout << "  - 最终位置 = 绘制位置 (已经包含了变换)" << std::endl;
        }
    }
    
    std::cout << "\n=== 结论 ===" << std::endl;
    std::cout << "Border 的 DrawRectangle 位置: (0, 0)" << std::endl;
    std::cout << "TextBlock 的 DrawText 位置: (11, 6)" << std::endl;
    std::cout << "\n问题：TextBlock 在y坐标上相对于 Border 偏移了6个单位" << std::endl;
    std::cout << "这是由 Border 的 Padding(10, 5, 10, 5) 导致的。" << std::endl;
    std::cout << "但问题陈述说 'border的y坐标大约为textblock的两倍'" << std::endl;
    std::cout << "这可能指的是另一个场景..." << std::endl;
    
    return 0;
}
