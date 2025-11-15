#include "fk/ui/TextBlock.h"
#include "fk/ui/StackPanel.h"
#include "fk/ui/Brush.h"
#include "fk/render/RenderContext.h"
#include "fk/render/RenderList.h"
#include "fk/render/TextRenderer.h"
#include <iostream>

int main() {
    std::cout << "=== 测试 example/main.cpp 场景 ===" << std::endl;
    
    // 完全按照 example/main.cpp 创建元素
    auto* stackPanel = new fk::ui::StackPanel();
    
    auto* text1 = (new fk::ui::TextBlock())
                       ->Text("Hello, F K UI!")
                       ->FontSize(32)
                       ->Foreground(fk::ui::Brushes::Blue())
                       ->Margin(fk::Thickness(20));
    
    auto* text2 = (new fk::ui::TextBlock())
                       ->Text("This is a simple example of F K UI framework.")
                       ->FontSize(16)
                       ->Foreground(fk::ui::Brushes::DarkGray())
                       ->Margin(fk::Thickness(20));
    
    stackPanel->Children({text1, text2});
    
    // 执行布局
    fk::ui::Size availableSize(800, 600);
    stackPanel->Measure(availableSize);
    stackPanel->Arrange(fk::ui::Rect(0, 0, 800, 600));
    
    std::cout << "\n=== 布局结果 ===" << std::endl;
    std::cout << "StackPanel desired: (" << stackPanel->GetDesiredSize().width 
              << ", " << stackPanel->GetDesiredSize().height << ")" << std::endl;
    std::cout << "StackPanel layoutRect: (" << stackPanel->GetLayoutRect().x << ", " 
              << stackPanel->GetLayoutRect().y << ", " 
              << stackPanel->GetLayoutRect().width << ", " 
              << stackPanel->GetLayoutRect().height << ")" << std::endl;
    
    std::cout << "\nText1 \"" << text1->GetText() << "\":" << std::endl;
    std::cout << "  Margin: (" << text1->GetMargin().left << ", " << text1->GetMargin().top 
              << ", " << text1->GetMargin().right << ", " << text1->GetMargin().bottom << ")" << std::endl;
    std::cout << "  Desired: (" << text1->GetDesiredSize().width << ", " << text1->GetDesiredSize().height << ")" << std::endl;
    std::cout << "  LayoutRect: (" << text1->GetLayoutRect().x << ", " << text1->GetLayoutRect().y 
              << ", " << text1->GetLayoutRect().width << ", " << text1->GetLayoutRect().height << ")" << std::endl;
    std::cout << "  RenderSize: (" << text1->GetRenderSize().width << ", " << text1->GetRenderSize().height << ")" << std::endl;
    
    std::cout << "\nText2 \"" << text2->GetText() << "\":" << std::endl;
    std::cout << "  Margin: (" << text2->GetMargin().left << ", " << text2->GetMargin().top 
              << ", " << text2->GetMargin().right << ", " << text2->GetMargin().bottom << ")" << std::endl;
    std::cout << "  Desired: (" << text2->GetDesiredSize().width << ", " << text2->GetDesiredSize().height << ")" << std::endl;
    std::cout << "  LayoutRect: (" << text2->GetLayoutRect().x << ", " << text2->GetLayoutRect().y 
              << ", " << text2->GetLayoutRect().width << ", " << text2->GetLayoutRect().height << ")" << std::endl;
    std::cout << "  RenderSize: (" << text2->GetRenderSize().width << ", " << text2->GetRenderSize().height << ")" << std::endl;
    
    // 检查重叠
    auto rect1 = text1->GetLayoutRect();
    auto rect2 = text2->GetLayoutRect();
    float text1_bottom = rect1.y + rect1.height;
    float text2_top = rect2.y;
    float gap = text2_top - text1_bottom;
    
    std::cout << "\n=== 间距分析 ===" << std::endl;
    std::cout << "Text1底部: " << text1_bottom << std::endl;
    std::cout << "Text2顶部: " << text2_top << std::endl;
    std::cout << "实际间距: " << gap << " 像素" << std::endl;
    
    if (gap < 0) {
        std::cout << "❌ 警告：元素重叠！重叠了 " << -gap << " 像素" << std::endl;
    } else if (gap == 0) {
        std::cout << "⚠️  元素紧贴，没有间距" << std::endl;
    } else if (gap < 5) {
        std::cout << "✅ 间距很小，元素几乎紧贴" << std::endl;
    } else if (gap > 50) {
        std::cout << "❌ 警告：间距过大！" << std::endl;
    } else {
        std::cout << "✅ 间距正常" << std::endl;
    }
    
    // 模拟渲染
    std::cout << "\n=== 渲染命令 ===" << std::endl;
    fk::render::RenderList renderList;
    fk::render::TextRenderer textRenderer;
    fk::render::RenderContext context(&renderList, &textRenderer);
    
    stackPanel->CollectDrawCommands(context);
    
    std::cout << "生成了 " << renderList.GetCommands().size() << " 个渲染命令" << std::endl;
    
    float lastTextY = -1000.0f;
    for (size_t i = 0; i < renderList.GetCommands().size(); ++i) {
        auto& cmd = renderList.GetCommands()[i];
        if (cmd.type == fk::render::CommandType::DrawText) {
            auto& payload = std::get<fk::render::TextPayload>(cmd.payload);
            std::cout << "  DrawText[" << i << "]: \"" << payload.text.substr(0, 20);
            if (payload.text.length() > 20) std::cout << "...";
            std::cout << "\" at Y=" << payload.bounds.y << std::endl;
            
            if (lastTextY >= 0 && std::abs(payload.bounds.y - lastTextY) < 1.0f) {
                std::cout << "    ❌ 警告：与上一个文本Y坐标相同或几乎相同！" << std::endl;
            }
            lastTextY = payload.bounds.y;
        }
    }
    
    delete stackPanel;
    
    return 0;
}
