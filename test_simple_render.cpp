#include "fk/ui/ContentControl.h"
#include "fk/ui/TextBlock.h"
#include "fk/ui/Border.h"
#include "fk/ui/ControlTemplate.h"
#include "fk/ui/ContentPresenter.h"
#include "fk/ui/Brush.h"
#include "fk/render/RenderContext.h"
#include "fk/render/RenderList.h"
#include "fk/render/TextRenderer.h"
#include <iostream>

using namespace fk;
using namespace fk::ui;

// 使用 Window 作为测试（它是 ContentControl 的派生类，已经显式实例化）
#include "fk/ui/Window.h"

// 追踪 OnRender 调用的 TextBlock
class TrackedTextBlock : public TextBlock {
public:
    void OnRender(render::RenderContext& context) override {
        std::cout << "  -> TrackedTextBlock::OnRender 被调用！" << std::endl;
        std::cout << "     RenderContext 地址: " << &context << std::endl;
        std::cout << "     文本: \"" << GetText() << "\"" << std::endl;
        std::cout << "     当前变换偏移: (" << context.GetCurrentTransform().offsetX 
                  << ", " << context.GetCurrentTransform().offsetY << ")" << std::endl;
        TextBlock::OnRender(context);
    }
};

int main() {
    std::cout << "=== 简单渲染测试：确保模板实例化后获取 RenderContext ===" << std::endl << std::endl;
    
    // 创建自定义模板
    auto* tmpl = new ControlTemplate();
    tmpl->SetTargetType(typeid(Window));
    
    std::cout << "1. 设置模板工厂..." << std::endl;
    tmpl->SetFactory([]() -> UIElement* {
        std::cout << "   [模板工厂被调用！]" << std::endl;
        
        auto* border = new Border();
        border->Background(new SolidColorBrush(Color(200, 200, 200, 255)));
        border->Padding(5.0f);
        
        auto* presenter = new ContentPresenter<>();
        border->Child(presenter);
        
        std::cout << "   [模板视觉树创建完成]" << std::endl;
        return border;
    });
    
    // 创建 ContentControl（使用 Window）
    std::cout << "\n2. 创建 Window..." << std::endl;
    auto* control = new Window();
    control->SetTemplate(tmpl);
    
    // 设置追踪的 TextBlock 作为 Content
    std::cout << "\n2.5. 设置 Content..." << std::endl;
    auto* text = new TrackedTextBlock();
    text->Text("Hello World");
    control->Content(text);
    std::cout << "   Content 已设置" << std::endl;
    
    std::cout << "   Window 视觉子节点数量: " << control->GetVisualChildrenCount() << std::endl;
    
    // 检查模板根
    if (control->GetVisualChildrenCount() > 0) {
        auto* templateRoot = control->GetVisualChild(0);
        std::cout << "   模板根类型: " << typeid(*templateRoot).name() << std::endl;
        std::cout << "   模板根视觉子节点数量: " << templateRoot->GetVisualChildrenCount() << std::endl;
        
        if (templateRoot->GetVisualChildrenCount() > 0) {
            auto* child = templateRoot->GetVisualChild(0);
            std::cout << "   模板根的第一个子节点类型: " << typeid(*child).name() << std::endl;
            std::cout << "   该子节点（ContentPresenter）的视觉子节点数量: " << child->GetVisualChildrenCount() << std::endl;
            
            // 检查 ContentPresenter 的内容
            auto* presenter = dynamic_cast<ContentPresenter<>*>(child);
            if (presenter) {
                try {
                    auto presenterContent = presenter->GetContent();
                    std::cout << "   ContentPresenter 的 Content 是否有值: " << (presenterContent.has_value() ? "是" : "否") << std::endl;
                    if (presenterContent.has_value()) {
                        std::cout << "   Content 类型: " << presenterContent.type().name() << std::endl;
                    }
                } catch (const std::exception& e) {
                    std::cout << "   获取 ContentPresenter Content 时出错: " << e.what() << std::endl;
                }
            }
        }
    }
    
    // 布局
    std::cout << "\n3. 执行布局..." << std::endl;
    control->Measure(Size(200, 50));
    control->Arrange(Rect(10, 20, 200, 50));
    std::cout << "   布局完成" << std::endl;
    
    // 渲染
    std::cout << "\n4. 创建 RenderContext 并渲染..." << std::endl;
    render::RenderList renderList;
    render::TextRenderer textRenderer;
    render::RenderContext context(&renderList, &textRenderer);
    std::cout << "   RenderContext 地址: " << &context << std::endl;
    
    std::cout << "\n5. 调用 CollectDrawCommands..." << std::endl;
    control->CollectDrawCommands(context);
    
    // 检查结果
    std::cout << "\n6. 检查结果:" << std::endl;
    std::cout << "   视觉子节点数量: " << control->GetVisualChildrenCount() << std::endl;
    std::cout << "   渲染命令数量: " << renderList.GetCommandCount() << std::endl;
    
    // 统计命令
    int transformCount = 0;
    int textCount = 0;
    for (const auto& cmd : renderList.GetCommands()) {
        if (cmd.type == render::CommandType::SetTransform) transformCount++;
        if (cmd.type == render::CommandType::DrawText) textCount++;
    }
    
    std::cout << "   Transform 命令: " << transformCount << std::endl;
    std::cout << "   DrawText 命令: " << textCount << std::endl;
    
    std::cout << "\n结论:" << std::endl;
    if (textCount > 0) {
        std::cout << "✓ 成功！模板实例化后 TextBlock 正确获取到 RenderContext 并生成了渲染命令" << std::endl;
    } else {
        std::cout << "✗ 失败：没有生成文本渲染命令" << std::endl;
    }
    
    delete control;
    return 0;
}
