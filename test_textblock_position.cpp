#include "fk/ui/StackPanel.h"
#include "fk/ui/TextBlock.h"
#include "fk/ui/Brush.h"
#include "fk/render/RenderContext.h"
#include "fk/render/RenderList.h"
#include "fk/render/TextRenderer.h"
#include "fk/render/RenderCommand.h"
#include <iostream>

using namespace fk;
using namespace fk::ui;

int main() {
    std::cout << "=== 测试 TextBlock 位置 ===" << std::endl;
    
    // 创建一个StackPanel，包含两个TextBlock
    auto* panel = new StackPanel();
    
    auto* text1 = new TextBlock();
    text1->Text("First Text");
    text1->FontSize(20);
    text1->Margin(Thickness(10));
    
    auto* text2 = new TextBlock();
    text2->Text("Second Text");
    text2->FontSize(20);
    text2->Margin(Thickness(10));
    
    panel->Children({text1, text2});
    
    // 测量和排列
    panel->Measure(Size(400, 300));
    panel->Arrange(Rect(0, 0, 400, 300));
    
    // 打印布局信息
    std::cout << "\n布局信息:" << std::endl;
    std::cout << "Panel layoutRect: (" 
              << panel->GetLayoutRect().x << ", " 
              << panel->GetLayoutRect().y << ", "
              << panel->GetLayoutRect().width << ", "
              << panel->GetLayoutRect().height << ")" << std::endl;
    
    std::cout << "Text1 layoutRect: (" 
              << text1->GetLayoutRect().x << ", " 
              << text1->GetLayoutRect().y << ", "
              << text1->GetLayoutRect().width << ", "
              << text1->GetLayoutRect().height << ")" << std::endl;
              
    std::cout << "Text2 layoutRect: (" 
              << text2->GetLayoutRect().x << ", " 
              << text2->GetLayoutRect().y << ", "
              << text2->GetLayoutRect().width << ", "
              << text2->GetLayoutRect().height << ")" << std::endl;
    
    // 生成渲染命令
    render::RenderList renderList;
    render::TextRenderer textRenderer;
    render::RenderContext context(&renderList, &textRenderer);
    
    panel->CollectDrawCommands(context);
    
    // 分析渲染命令
    std::cout << "\n=== 渲染命令分析 ===" << std::endl;
    std::cout << "总命令数: " << renderList.GetCommandCount() << std::endl;
    
    const auto& commands = renderList.GetCommands();
    for (size_t i = 0; i < commands.size(); ++i) {
        const auto& cmd = commands[i];
        if (cmd.type == render::CommandType::DrawText) {
            const auto& payload = std::get<render::TextPayload>(cmd.payload);
            std::cout << "\nDrawText 命令 #" << i << ":" << std::endl;
            std::cout << "  位置: (" << payload.bounds.x << ", " << payload.bounds.y << ")" << std::endl;
            std::cout << "  文本: '" << payload.text << "'" << std::endl;
        } else if (cmd.type == render::CommandType::SetTransform) {
            const auto& payload = std::get<render::TransformPayload>(cmd.payload);
            std::cout << "\nSetTransform 命令 #" << i << ":" << std::endl;
            std::cout << "  偏移: (" << payload.offsetX << ", " << payload.offsetY << ")" << std::endl;
        }
    }
    
    std::cout << "\n=== 预期结果 ===" << std::endl;
    std::cout << "First Text 应该在 (10, 10) 位置（考虑margin）" << std::endl;
    std::cout << "Second Text 应该在 First Text 下方，Y坐标应该 > First Text 的 Y + 高度" << std::endl;
    
    delete panel;
    
    return 0;
}
