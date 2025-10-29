// Panel Children() Fluent API 使用示例
#include "fk/ui/StackPanel.h"
#include "fk/ui/ContentControl.h"
#include "fk/ui/Window.h"

#include <iostream>

using namespace fk::ui;

int main() {
    std::cout << "=== Panel Children() Fluent API 示例 ===" << std::endl;
    
    // 方式 1: 传统 AddChild 方式
    std::cout << "\n方式 1: 传统 AddChild" << std::endl;
    auto panel1 = std::make_shared<StackPanel>();
    panel1->AddChild(std::make_shared<ContentControl>());
    panel1->AddChild(std::make_shared<ContentControl>());
    panel1->AddChild(std::make_shared<ContentControl>());
    std::cout << "  子元素数量: " << panel1->GetChildCount() << std::endl;
    
    // 方式 2: Children() getter - 读取子元素
    std::cout << "\n方式 2: Children() getter" << std::endl;
    auto panel2 = std::make_shared<StackPanel>();
    panel2->AddChild(std::make_shared<ContentControl>());
    panel2->AddChild(std::make_shared<ContentControl>());
    
    auto children = panel2->Children();  // 返回 span<const shared_ptr<UIElement>>
    std::cout << "  获取到 " << children.size() << " 个子元素" << std::endl;
    for (size_t i = 0; i < children.size(); ++i) {
        std::cout << "    子元素 " << i << ": " << typeid(*children[i]).name() << std::endl;
    }
    
    // 方式 3: Children() setter - 批量设置子元素
    std::cout << "\n方式 3: Children() setter - 批量设置" << std::endl;
    auto panel3 = std::make_shared<StackPanel>();
    
    // 准备子元素集合
    UIElementCollection newChildren;
    for (int i = 0; i < 5; ++i) {
        newChildren.push_back(std::make_shared<ContentControl>());
    }
    
    // 一次性设置所有子元素
    panel3->Children(newChildren);
    std::cout << "  批量设置后子元素数量: " << panel3->GetChildCount() << std::endl;
    
    // 方式 4: Children() setter - 替换现有子元素
    std::cout << "\n方式 4: Children() setter - 替换" << std::endl;
    auto panel4 = std::make_shared<StackPanel>();
    
    // 先添加一些子元素
    panel4->AddChild(std::make_shared<ContentControl>());
    panel4->AddChild(std::make_shared<ContentControl>());
    std::cout << "  初始子元素数量: " << panel4->GetChildCount() << std::endl;
    
    // 用新集合替换
    UIElementCollection replacements;
    for (int i = 0; i < 3; ++i) {
        replacements.push_back(std::make_shared<ContentControl>());
    }
    panel4->Children(replacements);
    std::cout << "  替换后子元素数量: " << panel4->GetChildCount() << std::endl;
    
    // 方式 5: 混合使用
    std::cout << "\n方式 5: 混合使用 Children() 和 AddChild()" << std::endl;
    auto panel5 = std::make_shared<StackPanel>();
    
    // 先批量设置
    UIElementCollection initial;
    initial.push_back(std::make_shared<ContentControl>());
    initial.push_back(std::make_shared<ContentControl>());
    panel5->Children(initial);
    std::cout << "  批量设置后: " << panel5->GetChildCount() << " 个" << std::endl;
    
    // 再单独添加
    panel5->AddChild(std::make_shared<ContentControl>());
    panel5->AddChild(std::make_shared<ContentControl>());
    std::cout << "  追加后: " << panel5->GetChildCount() << " 个" << std::endl;
    
    // 方式 6: 在实际场景中使用
    std::cout << "\n方式 6: 实际场景 - Window 中使用" << std::endl;
    auto window = std::make_shared<Window>();
    auto mainPanel = std::make_shared<StackPanel>();
    
    // 创建多个控件
    UIElementCollection controls;
    for (int i = 0; i < 4; ++i) {
        auto ctrl = std::make_shared<ContentControl>();
        controls.push_back(ctrl);
    }
    
    // 批量设置到 Panel
    mainPanel->Children(controls);
    
    // 设置为窗口内容
    window->Content(mainPanel);
    
    std::cout << "  窗口内容 Panel 的子元素数量: " 
              << mainPanel->GetChildCount() << std::endl;
    
    // 方式 7: 清空再重新设置
    std::cout << "\n方式 7: 清空再重新设置" << std::endl;
    auto panel7 = std::make_shared<StackPanel>();
    panel7->AddChild(std::make_shared<ContentControl>());
    panel7->AddChild(std::make_shared<ContentControl>());
    std::cout << "  设置前: " << panel7->GetChildCount() << " 个" << std::endl;
    
    // 清空
    panel7->Children(UIElementCollection{});  // 空集合
    std::cout << "  清空后: " << panel7->GetChildCount() << " 个" << std::endl;
    
    // 重新设置
    UIElementCollection fresh;
    fresh.push_back(std::make_shared<ContentControl>());
    panel7->Children(std::move(fresh));  // 移动语义
    std::cout << "  重新设置后: " << panel7->GetChildCount() << " 个" << std::endl;
    
    std::cout << "\n✅ 所有示例运行完成！" << std::endl;
    return 0;
}
