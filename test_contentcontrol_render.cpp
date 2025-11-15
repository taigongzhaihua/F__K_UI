#include "fk/ui/Button.h"
#include "fk/ui/ContentControl.h"
#include "fk/ui/TextBlock.h"
#include "fk/ui/Border.h"
#include "fk/ui/ControlTemplate.h"
#include "fk/ui/ContentPresenter.h"
#include "fk/ui/Brush.h"
#include "fk/render/RenderContext.h"
#include "fk/render/RenderList.h"
#include <iostream>

using namespace fk;
using namespace fk::ui;

void printVisualTree(Visual* node, int depth = 0) {
    if (!node) return;
    
    std::string indent(depth * 2, ' ');
    std::cout << indent << "- " << typeid(*node).name();
    
    auto* elem = dynamic_cast<UIElement*>(node);
    if (elem) {
        auto renderSize = elem->GetRenderSize();
        std::cout << " [" << renderSize.width << "x" << renderSize.height << "]";
    }
    std::cout << std::endl;
    
    for (size_t i = 0; i < node->GetVisualChildrenCount(); ++i) {
        printVisualTree(node->GetVisualChild(i), depth + 1);
    }
}

int main() {
    std::cout << "=== ContentControl 渲染测试 ===" << std::endl << std::endl;
    
    // 测试 1: Button 带默认模板
    std::cout << "测试 1: Button 带模板" << std::endl;
    auto* button = new Button();
    button->Content("Click Me");
    button->ApplyTemplate();
    button->Measure(Size(200, 50));
    button->Arrange(Rect(0, 0, 200, 50));
    
    std::cout << "视觉树结构：" << std::endl;
    printVisualTree(button);
    
    render::RenderList renderList;
    render::RenderContext context(&renderList);
    button->CollectDrawCommands(context);
    
    std::cout << "绘制命令数量: " << renderList.GetCommandCount() << std::endl;
    std::cout << std::endl;
    
    delete button;
    
    // 测试 2: 纯 TextBlock（无模板）
    std::cout << "测试 2: 纯 TextBlock（对比）" << std::endl;
    auto* text = new TextBlock();
    text->Text("Hello");
    text->Measure(Size(200, 50));
    text->Arrange(Rect(0, 0, 200, 50));
    
    std::cout << "视觉树结构：" << std::endl;
    printVisualTree(text);
    
    render::RenderList renderList2;
    render::RenderContext context2(&renderList2);
    text->CollectDrawCommands(context2);
    
    std::cout << "绘制命令数量: " << renderList2.GetCommandCount() << std::endl;
    std::cout << std::endl;
    
    delete text;
    
    return 0;
}
