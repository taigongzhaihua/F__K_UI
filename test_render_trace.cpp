#include "fk/ui/Button.h"
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

// 自定义 TextBlock 来追踪 OnRender 调用
class TrackedTextBlock : public TextBlock {
public:
    void OnRender(render::RenderContext& context) override {
        std::cout << "    -> TrackedTextBlock::OnRender 被调用" << std::endl;
        std::cout << "       RenderContext 地址: " << &context << std::endl;
        std::cout << "       当前变换: " << context.GetCurrentTransform().offsetX 
                  << ", " << context.GetCurrentTransform().offsetY << std::endl;
        std::cout << "       文本: " << GetText() << std::endl;
        TextBlock::OnRender(context);
    }
};

int main() {
    std::cout << "=== 渲染追踪测试 ===" << std::endl << std::endl;
    
    std::cout << "测试: 确保模板实例化后正确获取 RenderContext" << std::endl;
    
    // 创建自定义模板，使用追踪的 TextBlock
    auto* tmpl = new ControlTemplate();
    tmpl->SetTargetType(typeid(Button));
    
    tmpl->SetFactory([]() -> UIElement* {
        std::cout << "  [模板工厂] 创建视觉树..." << std::endl;
        
        auto* border = new Border();
        border->Background(new SolidColorBrush(Color(200, 200, 200, 255)));
        border->Padding(10.0f);
        
        auto* presenter = new ContentPresenter<>();
        border->Child(presenter);
        
        std::cout << "  [模板工厂] 视觉树创建完成" << std::endl;
        return border;
    });
    
    // 创建 Button，立即替换默认模板
    auto* button = new Button();
    // Button 构造时已经设置了默认模板，我们需要替换它
    button->SetTemplate(tmpl);
    // 重新应用模板以使用我们的自定义模板
    button->ApplyTemplate();
    
    // 使用追踪的 TextBlock 作为 Content
    auto* trackedText = new TrackedTextBlock();
    trackedText->Text("Tracked Content");
    button->Content(trackedText);
    
    std::cout << "\n步骤 1: Button 创建完成" << std::endl;
    std::cout << "  视觉子节点数量: " << button->GetVisualChildrenCount() << std::endl;
    std::cout << "  模板是否为我们的自定义模板: " << (button->GetTemplate() == tmpl ? "是" : "否") << std::endl;
    
    // 执行布局
    std::cout << "\n步骤 2: 执行 Measure/Arrange" << std::endl;
    button->Measure(Size(200, 50));
    button->Arrange(Rect(10, 20, 200, 50));
    std::cout << "  布局完成" << std::endl;
    std::cout << "  Button 布局矩形: (" << button->GetLayoutRect().x << ", " 
              << button->GetLayoutRect().y << ")" << std::endl;
    
    // 创建渲染上下文
    std::cout << "\n步骤 3: 创建 RenderContext 并开始渲染" << std::endl;
    render::RenderList renderList;
    render::TextRenderer textRenderer;
    render::RenderContext context(&renderList, &textRenderer);
    std::cout << "  RenderContext 地址: " << &context << std::endl;
    
    // 执行渲染
    std::cout << "\n步骤 4: 调用 CollectDrawCommands" << std::endl;
    button->CollectDrawCommands(context);
    
    std::cout << "\n步骤 5: 检查结果" << std::endl;
    std::cout << "  视觉子节点数量: " << button->GetVisualChildrenCount() << std::endl;
    std::cout << "  渲染命令数量: " << renderList.GetCommandCount() << std::endl;
    
    // 分析渲染命令
    int transformCount = 0;
    int textCount = 0;
    
    for (size_t i = 0; i < renderList.GetCommands().size(); ++i) {
        auto& cmd = renderList.GetCommands()[i];
        if (cmd.type == render::CommandType::SetTransform) {
            transformCount++;
            auto& payload = std::get<render::TransformPayload>(cmd.payload);
            std::cout << "  命令 " << i << ": Transform (" 
                      << payload.offsetX << ", " << payload.offsetY << ")" << std::endl;
        } else if (cmd.type == render::CommandType::DrawText) {
            textCount++;
            auto& payload = std::get<render::TextPayload>(cmd.payload);
            std::cout << "  命令 " << i << ": DrawText \"" << payload.text << "\" at (" 
                      << payload.bounds.x << ", " << payload.bounds.y << ")" << std::endl;
        }
    }
    
    std::cout << "\n总结:" << std::endl;
    std::cout << "  Transform 命令: " << transformCount << std::endl;
    std::cout << "  DrawText 命令: " << textCount << std::endl;
    
    if (textCount > 0) {
        std::cout << "  ✓ 模板实例化后正确获取到 RenderContext 并生成了文本渲染命令" << std::endl;
    } else {
        std::cout << "  ⚠ 警告: 没有生成文本渲染命令" << std::endl;
    }
    
    delete button;
    
    return 0;
}
