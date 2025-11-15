#include "fk/ui/TextBlock.h"
#include "fk/ui/StackPanel.h"
#include "fk/ui/Brush.h"
#include "fk/render/RenderContext.h"
#include "fk/render/RenderList.h"
#include "fk/render/TextRenderer.h"
#include <iostream>

// 自定义 RenderContext 来追踪变换
class DebugRenderContext : public fk::render::RenderContext {
public:
    using fk::render::RenderContext::RenderContext;
    
    void PushTransform(float offsetX, float offsetY) {
        std::cout << "PushTransform: (" << offsetX << ", " << offsetY << ")" << std::endl;
        fk::render::RenderContext::PushTransform(offsetX, offsetY);
        auto transform = GetCurrentTransform();
        std::cout << "  Current累积变换: (" << transform.offsetX << ", " << transform.offsetY << ")" << std::endl;
    }
    
    void PopTransform() {
        std::cout << "PopTransform" << std::endl;
        fk::render::RenderContext::PopTransform();
        auto transform = GetCurrentTransform();
        std::cout << "  Current累积变换: (" << transform.offsetX << ", " << transform.offsetY << ")" << std::endl;
    }
    
    void DrawText(
        const fk::ui::Point& position,
        const std::string& text,
        const std::array<float, 4>& color,
        float fontSize,
        const std::string& fontFamily = "Arial",
        float maxWidth = 0.0f,
        bool textWrapping = false)
    {
        auto transform = GetCurrentTransform();
        auto globalPos = TransformPoint(position);
        std::cout << "DrawText: \"" << text << "\"" << std::endl;
        std::cout << "  局部坐标: (" << position.x << ", " << position.y << ")" << std::endl;
        std::cout << "  当前累积变换: (" << transform.offsetX << ", " << transform.offsetY << ")" << std::endl;
        std::cout << "  全局坐标: (" << globalPos.x << ", " << globalPos.y << ")" << std::endl;
        
        fk::render::RenderContext::DrawText(position, text, color, fontSize, fontFamily, maxWidth, textWrapping);
    }
};

int main() {
    std::cout << "=== 渲染管线调试测试 ===" << std::endl;
    
    // 创建元素
    auto* stackPanel = new fk::ui::StackPanel();
    stackPanel->SetOrientation(fk::ui::Orientation::Vertical);
    stackPanel->SetSpacing(10.0f);
    
    auto* text1 = new fk::ui::TextBlock();
    text1->SetText("第一个元素");
    text1->SetFontSize(32);
    text1->SetForeground(fk::ui::Brushes::Blue());
    text1->SetMargin(fk::Thickness(20));
    
    auto* text2 = new fk::ui::TextBlock();
    text2->SetText("第二个元素");
    text2->SetFontSize(16);
    text2->SetForeground(fk::ui::Brushes::Red());
    text2->SetMargin(fk::Thickness(20));
    
    auto* text3 = new fk::ui::TextBlock();
    text3->SetText("第三个元素");
    text3->SetFontSize(16);
    text3->SetForeground(fk::ui::Brushes::Green());
    text3->SetMargin(fk::Thickness(20));
    
    stackPanel->Children({text1, text2, text3});
    
    // 执行布局
    std::cout << "\n=== 布局阶段 ===" << std::endl;
    fk::ui::Size availableSize(800, 600);
    stackPanel->Measure(availableSize);
    stackPanel->Arrange(fk::ui::Rect(0, 0, 800, 600));
    
    std::cout << "\nStackPanel layoutRect: (" << stackPanel->GetLayoutRect().x << ", " 
              << stackPanel->GetLayoutRect().y << ", " 
              << stackPanel->GetLayoutRect().width << ", " 
              << stackPanel->GetLayoutRect().height << ")" << std::endl;
    
    std::cout << "Text1 layoutRect: (" << text1->GetLayoutRect().x << ", " 
              << text1->GetLayoutRect().y << ", " 
              << text1->GetLayoutRect().width << ", " 
              << text1->GetLayoutRect().height << ")" << std::endl;
    std::cout << "Text2 layoutRect: (" << text2->GetLayoutRect().x << ", " 
              << text2->GetLayoutRect().y << ", " 
              << text2->GetLayoutRect().width << ", " 
              << text2->GetLayoutRect().height << ")" << std::endl;
    std::cout << "Text3 layoutRect: (" << text3->GetLayoutRect().x << ", " 
              << text3->GetLayoutRect().y << ", " 
              << text3->GetLayoutRect().width << ", " 
              << text3->GetLayoutRect().height << ")" << std::endl;
    
    // 模拟渲染
    std::cout << "\n=== 渲染阶段 ===" << std::endl;
    fk::render::RenderList renderList;
    fk::render::TextRenderer textRenderer;
    DebugRenderContext context(&renderList, &textRenderer);
    
    std::cout << "\n开始收集绘制命令..." << std::endl;
    std::cout << "StackPanel 子元素数量: " << stackPanel->GetChildrenCount() << std::endl;
    std::cout << "StackPanel 可视子元素数量: " << stackPanel->GetVisualChildrenCount() << std::endl;
    
    for (size_t i = 0; i < stackPanel->GetVisualChildrenCount(); ++i) {
        auto* child = stackPanel->GetVisualChild(i);
        std::cout << "  可视子元素 " << i << ": " << (child ? "存在" : "null") << std::endl;
        if (child) {
            auto* uiChild = dynamic_cast<fk::ui::UIElement*>(child);
            if (uiChild) {
                auto rect = uiChild->GetLayoutRect();
                auto vis = uiChild->GetVisibility();
                std::cout << "    layoutRect: (" << rect.x << ", " << rect.y << ", " 
                          << rect.width << ", " << rect.height << ")" << std::endl;
                std::cout << "    visibility: " << static_cast<int>(vis) << " (0=Visible, 1=Hidden, 2=Collapsed)" << std::endl;
            }
        }
    }
    
    std::cout << "\n尝试手动在text1上调用CollectDrawCommands..." << std::endl;
    text1->CollectDrawCommands(context);
    
    std::cout << "\n现在对整个stackPanel调用CollectDrawCommands..." << std::endl;
    stackPanel->CollectDrawCommands(context);
    
    std::cout << "\n=== 总结 ===" << std::endl;
    std::cout << "如果文本的全局Y坐标相同，则说明渲染管线有问题" << std::endl;
    std::cout << "如果文本的全局Y坐标不同且符合布局，则渲染管线正常" << std::endl;
    
    delete stackPanel;
    
    return 0;
}
