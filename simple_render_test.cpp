#include "fk/ui/TextBlock.h"
#include "fk/ui/StackPanel.h"
#include "fk/ui/Brush.h"
#include "fk/render/RenderContext.h"
#include "fk/render/RenderList.h"
#include "fk/render/TextRenderer.h"
#include <iostream>

int main() {
    std::cout << "=== 简单渲染测试 ===" << std::endl;
    
    // 创建一个TextBlock
    auto* text1 = new fk::ui::TextBlock();
    text1->SetText("测试文本");
    text1->SetFontSize(32);
    text1->SetForeground(fk::ui::Brushes::Blue());
    
    // 执行布局
    text1->Measure(fk::ui::Size(800, 600));
    text1->Arrange(fk::ui::Rect(10, 20, 400, 100));
    
    std::cout << "Text1 layoutRect: (" << text1->GetLayoutRect().x << ", " 
              << text1->GetLayoutRect().y << ", " 
              << text1->GetLayoutRect().width << ", " 
              << text1->GetLayoutRect().height << ")" << std::endl;
    std::cout << "Text1 renderSize: (" << text1->GetRenderSize().width << ", " 
              << text1->GetRenderSize().height << ")" << std::endl;
    std::cout << "Text1 visibility: " << static_cast<int>(text1->GetVisibility()) << std::endl;
    
    // 创建渲染上下文
    fk::render::RenderList renderList;
    fk::render::TextRenderer textRenderer;
    fk::render::RenderContext context(&renderList, &textRenderer);
    
    std::cout << "\n开始渲染..." << std::endl;
    
    // 测试DrawText
    fk::ui::Point testPos(50, 50);
    std::array<float, 4> testColor = {1.0f, 0.0f, 0.0f, 1.0f};
    context.DrawText(testPos, "直接调用DrawText", testColor, 24.0f);
    std::cout << "直接调用DrawText完成" << std::endl;
    
    // 测试CollectDrawCommands
    std::cout << "\n调用text1->CollectDrawCommands..." << std::endl;
    text1->CollectDrawCommands(context);
    std::cout << "CollectDrawCommands返回" << std::endl;
    
    // 检查生成的命令
    std::cout << "\n渲染命令数量: " << renderList.GetCommands().size() << std::endl;
    
    for (size_t i = 0; i < renderList.GetCommands().size(); ++i) {
        auto& cmd = renderList.GetCommands()[i];
        std::cout << "命令 " << i << ": type=" << static_cast<int>(cmd.type) << std::endl;
        if (cmd.type == fk::render::CommandType::DrawText) {
            auto& payload = std::get<fk::render::TextPayload>(cmd.payload);
            std::cout << "  DrawText: \"" << payload.text << "\" at (" 
                      << payload.bounds.x << ", " << payload.bounds.y << ")" << std::endl;
        } else if (cmd.type == fk::render::CommandType::SetTransform) {
            auto& payload = std::get<fk::render::TransformPayload>(cmd.payload);
            std::cout << "  SetTransform: offset=(" << payload.offsetX << ", " << payload.offsetY << ")" << std::endl;
        }
    }
    
    // 测试StackPanel
    std::cout << "\n\n=== 测试 StackPanel ===" << std::endl;
    renderList.Clear();
    
    auto* stackPanel = new fk::ui::StackPanel();
    stackPanel->SetOrientation(fk::ui::Orientation::Vertical);
    stackPanel->SetSpacing(10.0f);
    
    auto* text2 = new fk::ui::TextBlock();
    text2->SetText("第一个");
    text2->SetFontSize(24);
    text2->SetForeground(fk::ui::Brushes::Red());
    
    auto* text3 = new fk::ui::TextBlock();
    text3->SetText("第二个");
    text3->SetFontSize(24);
    text3->SetForeground(fk::ui::Brushes::Green());
    
    stackPanel->Children({text2, text3});
    
    stackPanel->Measure(fk::ui::Size(800, 600));
    stackPanel->Arrange(fk::ui::Rect(0, 0, 800, 600));
    
    std::cout << "StackPanel layoutRect: (" << stackPanel->GetLayoutRect().x << ", " 
              << stackPanel->GetLayoutRect().y << ")" << std::endl;
    std::cout << "Text2 layoutRect: (" << text2->GetLayoutRect().x << ", " 
              << text2->GetLayoutRect().y << ")" << std::endl;
    std::cout << "Text3 layoutRect: (" << text3->GetLayoutRect().x << ", " 
              << text3->GetLayoutRect().y << ")" << std::endl;
    
    fk::render::RenderContext context2(&renderList, &textRenderer);
    stackPanel->CollectDrawCommands(context2);
    
    std::cout << "\nStackPanel 渲染命令数量: " << renderList.GetCommands().size() << std::endl;
    
    for (size_t i = 0; i < renderList.GetCommands().size(); ++i) {
        auto& cmd = renderList.GetCommands()[i];
        std::cout << "命令 " << i << ": type=" << static_cast<int>(cmd.type) << std::endl;
        if (cmd.type == fk::render::CommandType::DrawText) {
            auto& payload = std::get<fk::render::TextPayload>(cmd.payload);
            std::cout << "  DrawText: \"" << payload.text << "\" at (" 
                      << payload.bounds.x << ", " << payload.bounds.y << ")" << std::endl;
        } else if (cmd.type == fk::render::CommandType::SetTransform) {
            auto& payload = std::get<fk::render::TransformPayload>(cmd.payload);
            std::cout << "  SetTransform: offset=(" << payload.offsetX << ", " << payload.offsetY << ")" << std::endl;
        }
    }
    
    delete stackPanel;
    delete text1;
    
    return 0;
}
