// Panel Children 依赖属性测试
#include "fk/ui/StackPanel.h"
#include "fk/ui/ContentControl.h"
#include "fk/ui/Window.h"

#include <iostream>

using namespace fk::ui;

int main() {
    try {
        std::cout << "=== Panel Children 依赖属性测试 ===" << std::endl;

        // 创建 StackPanel
        auto panel = std::make_shared<StackPanel>();
        
        // 测试 1: 初始状态
        std::cout << "\n测试 1: 初始状态" << std::endl;
        std::cout << "  子元素数量: " << panel->GetChildCount() << std::endl;
        std::cout << "  是否有子元素: " << (panel->HasChildren() ? "是" : "否") << std::endl;
        
        // 测试 2: 添加子元素
        std::cout << "\n测试 2: 添加子元素" << std::endl;
        auto control1 = std::make_shared<ContentControl>();
        auto control2 = std::make_shared<ContentControl>();
        
        panel->AddChild(control1);
        panel->AddChild(control2);
        
        std::cout << "  添加 2 个子元素后:" << std::endl;
        std::cout << "  子元素数量: " << panel->GetChildCount() << std::endl;
        std::cout << "  是否有子元素: " << (panel->HasChildren() ? "是" : "否") << std::endl;
        
        // 测试 3: 遍历子元素
        std::cout << "\n测试 3: 遍历子元素" << std::endl;
        auto children = panel->GetChildren();
        for (size_t i = 0; i < children.size(); ++i) {
            const auto& child = children[i];
            std::cout << "  子元素 " << i << ": " << typeid(*child).name() << std::endl;
        }
        
        // 测试 4: 移除子元素
        std::cout << "\n测试 4: 移除子元素" << std::endl;
        panel->RemoveChild(control1.get());
        std::cout << "  移除第一个子元素后:" << std::endl;
        std::cout << "  子元素数量: " << panel->GetChildCount() << std::endl;
        
        // 测试 5: 清空子元素
        std::cout << "\n测试 5: 清空子元素" << std::endl;
        panel->ClearChildren();
        std::cout << "  清空后:" << std::endl;
        std::cout << "  子元素数量: " << panel->GetChildCount() << std::endl;
        std::cout << "  是否有子元素: " << (panel->HasChildren() ? "是" : "否") << std::endl;
        
        // 测试 6: 依赖属性访问
        std::cout << "\n测试 6: 依赖属性访问" << std::endl;
        std::cout << "  ChildrenProperty 名称: " 
                  << PanelBase::ChildrenProperty().Name() << std::endl;
        std::cout << "  ChildrenProperty 所有者: " 
                  << PanelBase::ChildrenProperty().OwnerType().name() << std::endl;
        
        // 测试 7: 在 Window 中使用
        std::cout << "\n测试 7: 在 Window 中使用" << std::endl;
        auto window = std::make_shared<Window>();
        auto contentPanel = std::make_shared<StackPanel>();
        
        for (int i = 0; i < 3; ++i) {
            auto control = std::make_shared<ContentControl>();
            contentPanel->AddChild(control);
        }
        
        window->Content(contentPanel);
        std::cout << "  Window Content Panel 子元素数量: " 
                  << contentPanel->GetChildCount() << std::endl;
        
        // 测试 8: Fluent API - Children() getter
        std::cout << "\n测试 8: Fluent API - Children() getter" << std::endl;
        auto panel2 = std::make_shared<StackPanel>();
        panel2->AddChild(std::make_shared<ContentControl>());
        panel2->AddChild(std::make_shared<ContentControl>());
        
        auto childrenSpan = panel2->Children();
        std::cout << "  通过 Children() 获取: " << childrenSpan.size() << " 个子元素" << std::endl;
        
        // 测试 9: Fluent API - Children() setter
        std::cout << "\n测试 9: Fluent API - Children() setter" << std::endl;
        auto panel3 = std::make_shared<StackPanel>();
        
        UIElementCollection newChildren;
        for (int i = 0; i < 4; ++i) {
            newChildren.push_back(std::make_shared<ContentControl>());
        }
        
        panel3->Children(newChildren);
        std::cout << "  设置后子元素数量: " << panel3->GetChildCount() << std::endl;
        
        // 测试 10: Fluent API - Children() 替换集合
        std::cout << "\n测试 10: Fluent API - Children() 替换集合" << std::endl;
        UIElementCollection replaceChildren;
        replaceChildren.push_back(std::make_shared<ContentControl>());
        replaceChildren.push_back(std::make_shared<ContentControl>());
        replaceChildren.push_back(std::make_shared<ContentControl>());
        
        auto panel5 = std::make_shared<StackPanel>();
        panel5->AddChild(std::make_shared<ContentControl>());  // 先添加一个
        std::cout << "  替换前: " << panel5->GetChildCount() << " 个子元素" << std::endl;
        
        panel5->Children(replaceChildren);  // 替换为新集合
        std::cout << "  替换后: " << panel5->GetChildCount() << " 个子元素" << std::endl;
        
        std::cout << "\n✅ 所有测试通过！" << std::endl;
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ 测试失败: " << e.what() << std::endl;
        return 1;
    }
}
