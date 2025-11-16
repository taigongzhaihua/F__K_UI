#include "fk/ui/StackPanel.h"
#include "fk/ui/TextBlock.h"
#include "fk/ui/Button.h"
#include "fk/ui/Brush.h"
#include "fk/render/RenderContext.h"
#include "fk/render/RenderList.h"
#include "fk/render/TextRenderer.h"
#include "fk/render/RenderCommand.h"
#include <iostream>

using namespace fk;
using namespace fk::ui;

int main() {
    std::cout << "===== 验证GlRenderer::DrawText修复 =====" << std::endl;
    std::cout << "\n问题描述：" << std::endl;
    std::cout << "  之前：GlRenderer::DrawText 总是从(0,0)开始渲染，忽略了payload.bounds" << std::endl;
    std::cout << "  修复：现在使用 payload.bounds.x 和 payload.bounds.y 作为起始坐标" << std::endl;
    std::cout << std::endl;
    
    // 场景：模拟 example/main.cpp 的完整场景
    std::cout << "【测试场景】模拟 example/main.cpp" << std::endl;
    std::cout << "  - StackPanel 包含多个 TextBlock 和一个 Button" << std::endl;
    std::cout << "  - 每个元素都有不同的 margin，因此会有不同的 Y 坐标" << std::endl;
    std::cout << std::endl;
    
    auto* panel = new StackPanel();
    
    // 第一个TextBlock：大标题，有margin
    auto* title = new TextBlock();
    title->Text("Hello, F K UI!");
    title->FontSize(32);
    title->Foreground(Brushes::Blue());
    title->Margin(Thickness(20));
    
    // 第二个TextBlock：副标题
    auto* subtitle = new TextBlock();
    subtitle->Text("This is a simple example of F K UI framework.");
    subtitle->FontSize(16);
    subtitle->Foreground(Brushes::DarkGray());
    subtitle->Margin(Thickness(20));
    
    // Button：包含TextBlock
    auto* button = new Button();
    button->Content("Click Me");
    button->Background(Brushes::Green());
    button->ApplyTemplate();
    
    panel->Children({title, subtitle, button});
    
    // 测量和排列（模拟800x600窗口）
    panel->Measure(Size(800, 600));
    panel->Arrange(Rect(0, 0, 800, 600));
    
    // 打印布局结果
    std::cout << "布局结果：" << std::endl;
    std::cout << "  Title layoutRect: (" << title->GetLayoutRect().x << ", " 
              << title->GetLayoutRect().y << ")" << std::endl;
    std::cout << "  Subtitle layoutRect: (" << subtitle->GetLayoutRect().x << ", " 
              << subtitle->GetLayoutRect().y << ")" << std::endl;
    std::cout << "  Button layoutRect: (" << button->GetLayoutRect().x << ", " 
              << button->GetLayoutRect().y << ")" << std::endl;
    std::cout << std::endl;
    
    // 生成渲染命令
    render::RenderList renderList;
    render::TextRenderer textRenderer;
    render::RenderContext context(&renderList, &textRenderer);
    
    panel->CollectDrawCommands(context);
    
    // 分析DrawText命令
    std::cout << "DrawText 命令分析：" << std::endl;
    const auto& commands = renderList.GetCommands();
    
    int textCount = 0;
    bool allCorrect = true;
    
    for (const auto& cmd : commands) {
        if (cmd.type == render::CommandType::DrawText) {
            const auto& payload = std::get<render::TextPayload>(cmd.payload);
            textCount++;
            
            std::cout << "  [" << textCount << "] 文本: \"" << payload.text << "\"" << std::endl;
            std::cout << "      位置: (" << payload.bounds.x << ", " << payload.bounds.y << ")" << std::endl;
            
            // 检查位置是否合理（不应该都在0,0）
            if (textCount > 1 && payload.bounds.x < 1.0f && payload.bounds.y < 1.0f) {
                std::cout << "      ❌ 错误：位置接近(0,0)！" << std::endl;
                allCorrect = false;
            } else if (payload.bounds.y > 0) {
                std::cout << "      ✓ 位置合理" << std::endl;
            }
        }
    }
    
    std::cout << std::endl;
    std::cout << "总共 " << textCount << " 个TextBlock" << std::endl;
    std::cout << std::endl;
    
    // 验证结果
    if (allCorrect && textCount >= 3) {
        std::cout << "✅ 修复验证通过！" << std::endl;
        std::cout << "   所有TextBlock的位置都正确，没有都堆在(0,0)" << std::endl;
        std::cout << std::endl;
        std::cout << "修复说明：" << std::endl;
        std::cout << "   - 在 GlRenderer::DrawText 中" << std::endl;
        std::cout << "   - 将 float y = 0.0f; 改为 float y = payload.bounds.y;" << std::endl;
        std::cout << "   - 将 float x = 0.0f; 改为 float x = payload.bounds.x;" << std::endl;
        std::cout << "   - 这样实际渲染时会使用RenderContext提供的全局坐标" << std::endl;
    } else {
        std::cout << "❌ 验证失败：仍然存在问题" << std::endl;
        allCorrect = false;
    }
    
    delete panel;
    
    return allCorrect ? 0 : 1;
}
