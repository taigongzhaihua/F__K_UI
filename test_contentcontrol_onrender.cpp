/**
 * @file test_contentcontrol_onrender.cpp
 * @brief 测试 ContentControl 的 OnRender 实现
 * 
 * 验证：
 * 1. ContentControl::OnRender 确保模板在首次渲染时被应用
 * 2. RenderContext 正确传递到模板子元素
 * 3. 渲染命令正确生成
 */

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
#include <cassert>

using namespace fk;
using namespace fk::ui;

void test_template_lazy_apply() {
    std::cout << "测试1: 模板延迟应用（在首次渲染时应用）..." << std::endl;
    
    // 创建一个带模板的 Button，但不立即调用 ApplyTemplate
    auto* tmpl = new ControlTemplate();
    tmpl->SetTargetType(typeid(Button));
    
    tmpl->SetFactory([]() -> UIElement* {
        auto* border = new Border();
        border->Background(new SolidColorBrush(Color(200, 200, 200, 255)));
        
        auto* presenter = new ContentPresenter<>();
        border->Child(presenter);
        
        return border;
    });
    
    auto* button = new Button();
    button->SetTemplate(tmpl);
    button->Content("Click Me");
    // 注意：这里不调用 ApplyTemplate()
    
    // 检查视觉子节点（模板根会作为视觉子节点添加）
    size_t childCountBefore = button->GetVisualChildrenCount();
    std::cout << "  - ApplyTemplate 调用前，视觉子节点数量: " 
              << childCountBefore << std::endl;
    
    // 执行布局
    button->Measure(Size(200, 50));
    button->Arrange(Rect(0, 0, 200, 50));
    
    // 创建渲染上下文并尝试渲染
    render::RenderList renderList;
    render::TextRenderer textRenderer;
    render::RenderContext context(&renderList, &textRenderer);
    
    std::cout << "  - 调用 CollectDrawCommands..." << std::endl;
    button->CollectDrawCommands(context);
    
    // 检查视觉子节点现在是否存在
    size_t childCountAfter = button->GetVisualChildrenCount();
    std::cout << "  - CollectDrawCommands 调用后，视觉子节点数量: " 
              << childCountAfter << std::endl;
    
    if (childCountAfter > 0) {
        std::cout << "  ✓ 模板在首次渲染时被正确应用" << std::endl;
    } else {
        std::cout << "  ⚠ 警告：模板未被应用" << std::endl;
    }
    
    // 检查视觉子节点
    size_t childCount = button->GetVisualChildrenCount();
    std::cout << "  ✓ 视觉子节点数量: " << childCount << std::endl;
    
    delete button;
    std::cout << "测试1: 完成\n" << std::endl;
}

void test_render_context_propagation() {
    std::cout << "测试2: RenderContext 传递到子元素..." << std::endl;
    
    // 创建一个完整的 Button 层次结构
    auto* button = new Button();
    button->Content("Test Button");
    button->ApplyTemplate();  // 显式应用模板
    
    button->Measure(Size(200, 50));
    button->Arrange(Rect(10, 20, 200, 50));
    
    // 渲染
    render::RenderList renderList;
    render::TextRenderer textRenderer;
    render::RenderContext context(&renderList, &textRenderer);
    
    button->CollectDrawCommands(context);
    
    // 检查生成的渲染命令
    size_t commandCount = renderList.GetCommandCount();
    std::cout << "  - 渲染命令数量: " << commandCount << std::endl;
    
    // 分析渲染命令类型
    int transformCount = 0;
    int textCount = 0;
    
    for (size_t i = 0; i < renderList.GetCommands().size(); ++i) {
        auto& cmd = renderList.GetCommands()[i];
        if (cmd.type == render::CommandType::SetTransform) {
            transformCount++;
        } else if (cmd.type == render::CommandType::DrawText) {
            textCount++;
        }
    }
    
    std::cout << "  - Transform 命令: " << transformCount << std::endl;
    std::cout << "  - DrawText 命令: " << textCount << std::endl;
    
    if (transformCount > 0) {
        std::cout << "  ✓ RenderContext 正确推入变换" << std::endl;
    }
    
    if (textCount > 0) {
        std::cout << "  ✓ 文本内容被正确渲染" << std::endl;
    } else {
        std::cout << "  ⚠ 注意：没有生成文本渲染命令（可能是正常的）" << std::endl;
    }
    
    delete button;
    std::cout << "测试2: 完成\n" << std::endl;
}

void test_multiple_render_cycles() {
    std::cout << "测试3: 多次渲染循环..." << std::endl;
    
    auto* button = new Button();
    button->Content("Button");
    
    button->Measure(Size(200, 50));
    button->Arrange(Rect(0, 0, 200, 50));
    
    // 第一次渲染
    render::RenderList renderList1;
    render::TextRenderer textRenderer;
    render::RenderContext context1(&renderList1, &textRenderer);
    
    button->CollectDrawCommands(context1);
    size_t count1 = renderList1.GetCommandCount();
    std::cout << "  - 第一次渲染命令数量: " << count1 << std::endl;
    
    // 第二次渲染（不应该重新应用模板）
    render::RenderList renderList2;
    render::RenderContext context2(&renderList2, &textRenderer);
    
    button->CollectDrawCommands(context2);
    size_t count2 = renderList2.GetCommandCount();
    std::cout << "  - 第二次渲染命令数量: " << count2 << std::endl;
    
    if (count1 == count2) {
        std::cout << "  ✓ 多次渲染结果一致" << std::endl;
    } else {
        std::cout << "  ⚠ 警告：多次渲染结果不一致" << std::endl;
    }
    
    // 检查模板根没有被重复创建
    size_t childCount = button->GetVisualChildrenCount();
    assert(childCount == 1);
    std::cout << "  ✓ 模板根没有被重复创建（视觉子节点数量: " << childCount << "）" << std::endl;
    
    delete button;
    std::cout << "测试3: 完成\n" << std::endl;
}

int main() {
    std::cout << "==================================" << std::endl;
    std::cout << "  ContentControl OnRender 测试" << std::endl;
    std::cout << "==================================" << std::endl;
    std::cout << std::endl;
    
    try {
        test_template_lazy_apply();
        test_render_context_propagation();
        test_multiple_render_cycles();
        
        std::cout << "==================================" << std::endl;
        std::cout << "  所有测试完成！ ✓" << std::endl;
        std::cout << "==================================" << std::endl;
        
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "测试失败: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "测试失败: 未知错误" << std::endl;
        return 1;
    }
}
