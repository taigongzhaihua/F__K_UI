// 测试 Panel CRTP 模板链式调用
#include "fk/ui/StackPanel.h"
#include "fk/ui/ContentControl.h"
#include "fk/ui/Window.h"

#include <iostream>

using namespace fk::ui;

int main() {
    std::cout << "=== Panel CRTP 链式调用测试 ===" << std::endl;
    
    // 测试 1: StackPanel 的 Children() 返回 StackPanel*
    std::cout << "\n测试 1: StackPanel->Children() 返回类型" << std::endl;
    auto panel = std::make_shared<StackPanel>();
    
    UIElementCollection children;
    children.push_back(std::make_shared<ContentControl>());
    children.push_back(std::make_shared<ContentControl>());
    
    // 链式调用：Children() 返回 StackPanel*，可以继续调用 Orientation()
    auto result = panel->Children(children)
                       ->Orientation(Orientation::Horizontal);
    
    std::cout << "  ✅ Children() 返回 shared_ptr<StackPanel>" << std::endl;
    std::cout << "  ✅ 可以链式调用 Orientation()" << std::endl;
    std::cout << "  子元素数量: " << panel->GetChildCount() << std::endl;
    
    // 测试 2: 更复杂的链式调用
    std::cout << "\n测试 2: 复杂链式调用" << std::endl;
    UIElementCollection moreChildren;
    for (int i = 0; i < 3; ++i) {
        moreChildren.push_back(std::make_shared<ContentControl>());
    }
    
    auto panel2 = std::make_shared<StackPanel>();
    panel2->Children(moreChildren)
          ->Orientation(Orientation::Vertical)
          ->Width(300.0f)
          ->Height(400.0f);
    
    panel2->SetOpacity(0.8f);  // SetOpacity 返回 void，单独调用
    
    std::cout << "  ✅ Children()->Orientation()->Width()->Height() + SetOpacity()" << std::endl;
    std::cout << "  子元素数量: " << panel2->GetChildCount() << std::endl;
    std::cout << "  宽度: " << panel2->Width() << std::endl;
    std::cout << "  高度: " << panel2->Height() << std::endl;
    std::cout << "  不透明度: " << panel2->GetOpacity() << std::endl;
    
    // 测试 3: 在 Window 中使用
    std::cout << "\n测试 3: 在 Window 中使用链式调用" << std::endl;
    auto window = std::make_shared<Window>();
    
    UIElementCollection windowChildren;
    for (int i = 0; i < 4; ++i) {
        windowChildren.push_back(std::make_shared<ContentControl>());
    }
    
    auto contentPanel = std::make_shared<StackPanel>();
    contentPanel->Children(windowChildren)
                ->Orientation(Orientation::Vertical);
    
    window->Content(contentPanel);
    window->Title("CRTP 链式调用测试")
          ->Width(800)
          ->Height(600);
    
    std::cout << "  ✅ 创建带有链式配置的窗口" << std::endl;
    std::cout << "  窗口标题: " << window->Title() << std::endl;
    std::cout << "  窗口大小: " << window->Width() << "x" << window->Height() << std::endl;
    std::cout << "  内容面板子元素: " << contentPanel->GetChildCount() << std::endl;
    
    // 测试 4: 对比旧式写法
    std::cout << "\n测试 4: 对比传统写法 vs 链式调用" << std::endl;
    
    // 传统写法
    std::cout << "  传统写法:" << std::endl;
    auto panel3 = std::make_shared<StackPanel>();
    UIElementCollection c1;
    c1.push_back(std::make_shared<ContentControl>());
    panel3->Children(c1);
    panel3->Orientation(Orientation::Horizontal);
    panel3->Width(200.0f);
    std::cout << "    需要多行分别调用" << std::endl;
    
    // 链式调用
    std::cout << "  链式调用:" << std::endl;
    UIElementCollection c2;
    c2.push_back(std::make_shared<ContentControl>());
    auto panel4 = std::make_shared<StackPanel>();
    panel4->Children(c2)->Orientation(Orientation::Horizontal)->Width(200.0f);
    std::cout << "    一行完成所有配置" << std::endl;
    
    std::cout << "\n✅ 所有 CRTP 链式调用测试通过！" << std::endl;
    return 0;
}
