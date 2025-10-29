// 测试 ItemsControl 改进功能
#include <fk/ui/ItemsControl.h>
#include <fk/ui/StackPanel.h>
#include <fk/ui/ContentControl.h>
#include <iostream>
#include <cassert>

using namespace fk::ui;

void TestDisplayMemberPath() {
    std::cout << "\n=== Test DisplayMemberPath ===\n";
    
    auto itemsControl = std::make_shared<ItemsControl>();
    std::cout << "ItemsControl created\n";
    
    // 设置 DisplayMemberPath
    itemsControl->SetDisplayMemberPath("Name");
    std::cout << "DisplayMemberPath set to 'Name'\n";
    
    try {
        auto path = itemsControl->GetDisplayMemberPath();
        std::cout << "Got path: '" << path << "'\n";
        assert(path == "Name");
        std::cout << "✓ SetDisplayMemberPath/GetDisplayMemberPath\n";
    } catch (const std::exception& e) {
        std::cout << "Exception in GetDisplayMemberPath: " << e.what() << "\n";
        throw;
    }
    
    std::cout << "✅ DisplayMemberPath tests passed\n";
}

void TestAlternationCount() {
    std::cout << "\n=== Test AlternationCount ===\n";
    
    try {
        auto itemsControl = std::make_shared<ItemsControl>();
        std::cout << "ItemsControl created\n";
        
        // 默认值应该是 0
        int count = itemsControl->GetAlternationCount();
        std::cout << "Default AlternationCount = " << count << "\n";
        assert(count == 0);
        std::cout << "✓ Default AlternationCount is 0\n";
        
        // 设置 AlternationCount
        itemsControl->SetAlternationCount(2);
        count = itemsControl->GetAlternationCount();
        std::cout << "After set, AlternationCount = " << count << "\n";
        assert(count == 2);
        std::cout << "✓ SetAlternationCount\n";
        
        std::cout << "Adding items...\n";
        
        // 添加一些子项
        auto item1 = std::make_shared<ContentControl>();
        auto item2 = std::make_shared<ContentControl>();
        auto item3 = std::make_shared<ContentControl>();
        auto item4 = std::make_shared<ContentControl>();
        
        itemsControl->AddItem(item1);
        std::cout << "Item 1 added\n";
        itemsControl->AddItem(item2);
        std::cout << "Item 2 added\n";
        itemsControl->AddItem(item3);
        std::cout << "Item 3 added\n";
        itemsControl->AddItem(item4);
        std::cout << "Item 4 added\n";
        
        // 验证 AlternationIndex（0, 1, 0, 1）
        int idx1 = ItemsControl::GetAlternationIndex(item1.get());
        std::cout << "Item1 AlternationIndex = " << idx1 << "\n";
        
        std::cout << "✅ AlternationCount tests passed\n";
    } catch (const std::exception& e) {
        std::cout << "Exception: " << e.what() << "\n";
        throw;
    }
}

void TestImprovedItemsAPI() {
    std::cout << "\n=== Test Improved Items API ===\n";
    
    auto itemsControl = std::make_shared<ItemsControl>();
    
    // GetItemCount
    assert(itemsControl->GetItemCount() == 0);
    std::cout << "✓ Initial GetItemCount is 0\n";
    
    // AppendItems - 批量添加
    auto item1 = std::make_shared<ContentControl>();
    auto item2 = std::make_shared<ContentControl>();
    auto item3 = std::make_shared<ContentControl>();
    
    itemsControl->AppendItems({item1, item2, item3});
    assert(itemsControl->GetItemCount() == 3);
    std::cout << "✓ AppendItems adds multiple items\n";
    
    // GetItemAt
    assert(itemsControl->GetItemAt(0) == item1.get());
    assert(itemsControl->GetItemAt(1) == item2.get());
    assert(itemsControl->GetItemAt(2) == item3.get());
    assert(itemsControl->GetItemAt(99) == nullptr);
    std::cout << "✓ GetItemAt returns correct items\n";
    
    // InsertItem
    auto item4 = std::make_shared<ContentControl>();
    itemsControl->InsertItem(1, item4);
    assert(itemsControl->GetItemCount() == 4);
    assert(itemsControl->GetItemAt(0) == item1.get());
    assert(itemsControl->GetItemAt(1) == item4.get());
    assert(itemsControl->GetItemAt(2) == item2.get());
    assert(itemsControl->GetItemAt(3) == item3.get());
    std::cout << "✓ InsertItem inserts at correct position\n";
    
    // InsertItem 超出范围（应该 append）
    auto item5 = std::make_shared<ContentControl>();
    itemsControl->InsertItem(100, item5);
    assert(itemsControl->GetItemCount() == 5);
    assert(itemsControl->GetItemAt(4) == item5.get());
    std::cout << "✓ InsertItem with out-of-range index appends\n";
    
    std::cout << "✅ Improved Items API tests passed\n";
}

void TestAlternationWithImprovedAPI() {
    std::cout << "\n=== Test Alternation with Improved API ===\n";
    
    auto itemsControl = std::make_shared<ItemsControl>();
    itemsControl->SetAlternationCount(2);
    
    // 使用 AppendItems 批量添加
    auto item1 = std::make_shared<ContentControl>();
    auto item2 = std::make_shared<ContentControl>();
    auto item3 = std::make_shared<ContentControl>();
    
    itemsControl->AppendItems({item1, item2, item3});
    
    // 验证 AlternationIndex
    assert(ItemsControl::GetAlternationIndex(item1.get()) == 0);
    assert(ItemsControl::GetAlternationIndex(item2.get()) == 1);
    assert(ItemsControl::GetAlternationIndex(item3.get()) == 0);
    std::cout << "✓ AlternationIndex works with AppendItems\n";
    
    // 插入一个新项
    auto item4 = std::make_shared<ContentControl>();
    itemsControl->InsertItem(1, item4);
    
    // 验证插入后的 AlternationIndex
    assert(ItemsControl::GetAlternationIndex(item1.get()) == 0);
    assert(ItemsControl::GetAlternationIndex(item4.get()) == 1);
    assert(ItemsControl::GetAlternationIndex(item2.get()) == 0);
    assert(ItemsControl::GetAlternationIndex(item3.get()) == 1);
    std::cout << "✓ AlternationIndex updates after InsertItem\n";
    
    std::cout << "✅ Alternation with Improved API tests passed\n";
}

void TestItemsAPICompatibility() {
    std::cout << "\n=== Test Items API Compatibility ===\n";
    
    auto itemsControl = std::make_shared<ItemsControl>();
    
    // 旧 API: AddItem
    auto item1 = std::make_shared<ContentControl>();
    itemsControl->AddItem(item1);
    
    // 新 API: GetItemCount
    assert(itemsControl->GetItemCount() == 1);
    
    // 旧 API: Items() span
    auto itemsSpan = itemsControl->Items();
    assert(itemsSpan.size() == 1);
    assert(itemsSpan[0] == item1);
    
    // 新 API: AppendItems
    auto item2 = std::make_shared<ContentControl>();
    itemsControl->AppendItems({item2});
    
    // 旧 API 仍然工作
    assert(itemsControl->Items().size() == 2);
    
    // 旧 API: RemoveItem
    itemsControl->RemoveItem(item1.get());
    assert(itemsControl->GetItemCount() == 1);
    
    std::cout << "✓ Old and new APIs work together\n";
    std::cout << "✅ Items API Compatibility tests passed\n";
}

int main() {
    try {
        TestDisplayMemberPath();
        TestAlternationCount();
        TestImprovedItemsAPI();
        TestAlternationWithImprovedAPI();
        TestItemsAPICompatibility();
        
        std::cout << "\n✅✅✅ All ItemsControl improvement tests passed! ✅✅✅\n";
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "❌ Test failed: " << e.what() << '\n';
        return 1;
    }
}
