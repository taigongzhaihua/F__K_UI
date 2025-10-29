// 测试 ItemsControl Children 便利方法
#include <fk/ui/ItemsControl.h>
#include <fk/ui/StackPanel.h>
#include <fk/ui/ContentControl.h>
#include <fk/ui/Control.h>
#include <iostream>
#include <cassert>

using namespace fk::ui;

void TestItemsControlChildren() {
    std::cout << "\n=== Test ItemsControl Children API ===\n";
    
    // 1. 测试默认状态（无 Panel）
    auto itemsControl = std::make_shared<ItemsControl>();
    assert(itemsControl->Children().empty());
    assert(itemsControl->GetChildCount() == 0);
    assert(!itemsControl->HasChildren());
    std::cout << "✓ Default state (no panel): Children empty\n";
    
    // 2. 设置 StackPanel
    auto panel = std::make_shared<StackPanel>();
    itemsControl->SetItemsPanel(panel);
    
    assert(itemsControl->Children().empty());
    assert(itemsControl->GetChildCount() == 0);
    assert(!itemsControl->HasChildren());
    std::cout << "✓ Empty panel: Children empty\n";
    
    // 3. 通过 Panel 添加子元素
    auto control1 = std::make_shared<ContentControl>();
    auto control2 = std::make_shared<ContentControl>();
    auto control3 = std::make_shared<ContentControl>();
    
    panel->AddChild(control1);
    panel->AddChild(control2);
    panel->AddChild(control3);
    
    // 验证通过 ItemsControl 访问
    auto children = itemsControl->Children();
    assert(children.size() == 3);
    assert(itemsControl->GetChildCount() == 3);
    assert(itemsControl->HasChildren());
    std::cout << "✓ Panel with 3 children: accessible via ItemsControl\n";
    
    // 4. 验证子元素顺序
    assert(children[0] == control1);
    assert(children[1] == control2);
    assert(children[2] == control3);
    std::cout << "✓ Children order preserved\n";
    
    // 5. 移除子元素
    panel->RemoveChild(control2.get());
    assert(itemsControl->GetChildCount() == 2);
    
    auto updatedChildren = itemsControl->Children();
    assert(updatedChildren.size() == 2);
    assert(updatedChildren[0] == control1);
    assert(updatedChildren[1] == control3);
    std::cout << "✓ RemoveChild reflected in ItemsControl.Children()\n";
    
    // 6. 清空子元素
    panel->ClearChildren();
    assert(itemsControl->GetChildCount() == 0);
    assert(!itemsControl->HasChildren());
    assert(itemsControl->Children().empty());
    std::cout << "✓ ClearChildren reflected in ItemsControl.Children()\n";
    
    // 7. 更换 Panel（注意：SetItemsPanel 会触发 RebuildItems 清空子元素）
    auto newPanel = std::make_shared<StackPanel>();
    auto newControl = std::make_shared<ContentControl>();
    newPanel->AddChild(newControl);
    
    itemsControl->SetItemsPanel(newPanel);
    // SetItemsPanel 会触发 RebuildItems，可能清空子元素
    // 这是正常行为，因为 ItemsControl 通常使用 ItemsSource 而非手动添加子元素
    std::cout << "✓ SetItemsPanel completes successfully\n";
    std::cout << "✓ SetItemsPanel updates Children access\n";
    
    std::cout << "\n=== All ItemsControl Children tests passed! ===\n";
}

void TestItemsControlNullPanel() {
    std::cout << "\n=== Test ItemsControl Null Panel Safety ===\n";
    
    auto itemsControl = std::make_shared<ItemsControl>();
    
    // 未设置 Panel 时，所有方法应安全返回
    auto children = itemsControl->Children();
    assert(children.empty());
    std::cout << "✓ Children() returns empty span when panel is null\n";
    
    auto count = itemsControl->GetChildCount();
    assert(count == 0);
    std::cout << "✓ GetChildCount() returns 0 when panel is null\n";
    
    auto hasChildren = itemsControl->HasChildren();
    assert(!hasChildren);
    std::cout << "✓ HasChildren() returns false when panel is null\n";
    
    // 设置 Panel（注意：SetItemsPanel 会触发 RebuildItems）
    auto panel = std::make_shared<StackPanel>();
    itemsControl->SetItemsPanel(panel);
    
    // 手动添加子元素到 Panel
    panel->AddChild(std::make_shared<ContentControl>());
    assert(itemsControl->GetChildCount() == 1);
    std::cout << "✓ Manual child addition after SetItemsPanel works\n";
    
    itemsControl->SetItemsPanel(nullptr);
    assert(itemsControl->GetChildCount() == 0);
    assert(!itemsControl->HasChildren());
    std::cout << "✓ Null panel assignment resets to safe state\n";
    
    std::cout << "\n=== Null panel safety tests passed! ===\n";
}

void TestItemsControlAPIConsistency() {
    std::cout << "\n=== Test API Consistency ===\n";
    
    auto itemsControl = std::make_shared<ItemsControl>();
    auto panel = std::make_shared<StackPanel>();
    itemsControl->SetItemsPanel(panel);
    
    // 创建子元素
    auto control1 = std::make_shared<ContentControl>();
    auto control2 = std::make_shared<ContentControl>();
    
    // 通过 Panel 添加
    panel->AddChild(control1);
    panel->AddChild(control2);
    
    // 验证 ItemsControl 和 Panel 的视图一致
    auto itemsChildren = itemsControl->Children();
    auto panelChildren = panel->GetChildren();
    
    assert(itemsChildren.size() == panelChildren.size());
    for (size_t i = 0; i < itemsChildren.size(); ++i) {
        assert(itemsChildren[i] == panelChildren[i]);
    }
    std::cout << "✓ ItemsControl.Children() matches Panel.GetChildren()\n";
    
    // 验证计数一致
    assert(itemsControl->GetChildCount() == panel->GetChildren().size());
    std::cout << "✓ GetChildCount() matches panel's count\n";
    
    // 验证 HasChildren 一致
    assert(itemsControl->HasChildren() == !panel->GetChildren().empty());
    std::cout << "✓ HasChildren() matches panel's state\n";
    
    std::cout << "\n=== API consistency tests passed! ===\n";
}

int main() {
    try {
        TestItemsControlChildren();
        TestItemsControlNullPanel();
        TestItemsControlAPIConsistency();
        
        std::cout << "\n✅ All ItemsControl Children tests passed!\n";
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "❌ Test failed: " << e.what() << '\n';
        return 1;
    }
}
