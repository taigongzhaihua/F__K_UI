#include "fk/ui/TextBlock.h"
#include "fk/ui/StackPanel.h"
#include "fk/ui/Brush.h"
#include "fk/render/RenderContext.h"
#include "fk/render/RenderList.h"
#include "fk/render/TextRenderer.h"
#include "fk/render/RenderCommand.h"
#include <iostream>

using namespace fk;
using namespace fk::ui;

int main() {
    std::cout << "===== 测试：如果DrawText不应用变换会怎样 =====" << std::endl;
    
    auto* panel = new StackPanel();
    
    auto* text1 = new TextBlock();
    text1->Text("First");
    text1->Margin(Thickness(50, 100, 0, 0));  // 明显的偏移
    
    auto* text2 = new TextBlock();
    text2->Text("Second");
    text2->Margin(Thickness(50, 200, 0, 0));  // 更大的偏移
    
    panel->Children({text1, text2});
    
    panel->Measure(Size(400, 400));
    panel->Arrange(Rect(0, 0, 400, 400));
    
    std::cout << "\n布局信息：" << std::endl;
    std::cout << "Text1 layoutRect: (" << text1->GetLayoutRect().x << ", " 
              << text1->GetLayoutRect().y << ")" << std::endl;
    std::cout << "Text2 layoutRect: (" << text2->GetLayoutRect().x << ", " 
              << text2->GetLayoutRect().y << ")" << std::endl;
    
    // 生成渲染命令
    render::RenderList renderList;
    render::TextRenderer textRenderer;
    render::RenderContext context(&renderList, &textRenderer);
    
    panel->CollectDrawCommands(context);
    
    // 分析DrawText命令的位置
    std::cout << "\nDrawText命令分析：" << std::endl;
    const auto& commands = renderList.GetCommands();
    for (size_t i = 0; i < commands.size(); ++i) {
        const auto& cmd = commands[i];
        if (cmd.type == render::CommandType::DrawText) {
            const auto& payload = std::get<render::TextPayload>(cmd.payload);
            std::cout << "  文本 \"" << payload.text << "\" 在位置: ("
                      << payload.bounds.x << ", " << payload.bounds.y << ")" << std::endl;
        }
    }
    
    std::cout << "\n预期结果：" << std::endl;
    std::cout << "  如果变换正确应用，First应该在(50, 100)左右，Second应该在(50, 200+高度)左右" << std::endl;
    std::cout << "  如果变换没有应用，两者都会在(0, 0)或很小的坐标" << std::endl;
    
    // 检查是否所有文本都在(0,0)附近
    bool allAtZero = true;
    for (const auto& cmd : commands) {
        if (cmd.type == render::CommandType::DrawText) {
            const auto& payload = std::get<render::TextPayload>(cmd.payload);
            if (payload.bounds.x > 1.0f || payload.bounds.y > 1.0f) {
                allAtZero = false;
                break;
            }
        }
    }
    
    if (allAtZero) {
        std::cout << "\n❌ 错误：所有TextBlock都在(0,0)附近！变换没有被应用！" << std::endl;
    } else {
        std::cout << "\n✓ 正确：TextBlock位置已被正确变换！" << std::endl;
    }
    
    delete panel;
    
    return 0;
}
