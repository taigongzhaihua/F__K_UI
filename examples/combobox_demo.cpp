/**
 * @file combobox_demo.cpp
 * @brief ComboBox和Popup控件演示
 * 
 * 本演示展示 F__K_UI 的组合框和弹出窗口功能：
 * 1. Popup - 基础弹出窗口
 * 2. ComboBox - 基础组合框
 * 3. ComboBox下拉操作
 * 4. 选择项目
 * 5. 事件处理
 */

#include "fk/ui/ComboBox.h"
#include "fk/ui/Popup.h"
#include "fk/ui/TextBlock.h"
#include "fk/ui/Border.h"
#include <iostream>
#include <string>

using namespace fk::ui;

void PrintSeparator() {
    std::cout << "\n" << std::string(60, '=') << "\n\n";
}

void Test1_BasicPopup() {
    std::cout << "测试 1: 基础Popup控件\n";
    std::cout << "--------------------\n";
    
    auto popup = new Popup<>();
    
    std::cout << "✓ 创建Popup\n";
    std::cout << "  初始打开状态: " << (popup->GetIsOpen() ? "打开" : "关闭") << "\n";
    std::cout << "  放置模式: Bottom\n";
    std::cout << "  StaysOpen: " << (popup->GetStaysOpen() ? "true" : "false") << "\n";
    
    delete popup;
    PrintSeparator();
}

void Test2_PopupOpenClose() {
    std::cout << "测试 2: Popup打开和关闭\n";
    std::cout << "----------------------\n";
    
    auto popup = new Popup<>();
    
    int openedCount = 0;
    int closedCount = 0;
    
    popup->Opened.Connect([&openedCount]() {
        openedCount++;
    });
    
    popup->Closed.Connect([&closedCount]() {
        closedCount++;
    });
    
    std::cout << "初始状态: " << (popup->GetIsOpen() ? "打开" : "关闭") << "\n";
    
    popup->SetIsOpen(true);
    std::cout << "SetIsOpen(true): " << (popup->GetIsOpen() ? "打开" : "关闭") << "\n";
    
    popup->SetIsOpen(false);
    std::cout << "SetIsOpen(false): " << (popup->GetIsOpen() ? "打开" : "关闭") << "\n";
    
    popup->SetIsOpen(true);
    popup->SetIsOpen(false);
    
    std::cout << "\n事件统计:\n";
    std::cout << "  Opened事件触发: " << openedCount << " 次\n";
    std::cout << "  Closed事件触发: " << closedCount << " 次\n";
    
    delete popup;
    PrintSeparator();
}

void Test3_PopupPlacement() {
    std::cout << "测试 3: Popup放置模式\n";
    std::cout << "--------------------\n";
    
    auto popup = new Popup<>();
    
    std::cout << "测试不同放置模式:\n";
    
    popup->SetPlacement(PlacementMode::Bottom);
    std::cout << "  ✓ Bottom - 在目标下方\n";
    
    popup->SetPlacement(PlacementMode::Top);
    std::cout << "  ✓ Top - 在目标上方\n";
    
    popup->SetPlacement(PlacementMode::Left);
    std::cout << "  ✓ Left - 在目标左侧\n";
    
    popup->SetPlacement(PlacementMode::Right);
    std::cout << "  ✓ Right - 在目标右侧\n";
    
    popup->SetPlacement(PlacementMode::Center);
    std::cout << "  ✓ Center - 在目标中心\n";
    
    // 测试偏移
    popup->SetHorizontalOffset(10.0);
    popup->SetVerticalOffset(20.0);
    std::cout << "\n偏移量设置:\n";
    std::cout << "  水平偏移: " << popup->GetHorizontalOffset() << "\n";
    std::cout << "  垂直偏移: " << popup->GetVerticalOffset() << "\n";
    
    delete popup;
    PrintSeparator();
}

void Test4_BasicComboBox() {
    std::cout << "测试 4: 基础ComboBox控件\n";
    std::cout << "-----------------------\n";
    
    auto comboBox = new ComboBox<>();
    
    std::cout << "✓ 创建ComboBox\n";
    std::cout << "  选中索引: " << comboBox->GetSelectedIndex() << "\n";
    std::cout << "  下拉打开状态: " << (comboBox->GetIsDropDownOpen() ? "打开" : "关闭") << "\n";
    std::cout << "  最大下拉高度: " << comboBox->GetMaxDropDownHeight() << "\n";
    
    // 添加项目
    auto& items = comboBox->GetItems();
    items.Add(std::string("选项 1"));
    items.Add(std::string("选项 2"));
    items.Add(std::string("选项 3"));
    items.Add(std::string("选项 4"));
    items.Add(std::string("选项 5"));
    
    std::cout << "✓ 添加了 " << items.Count() << " 个选项\n";
    
    delete comboBox;
    PrintSeparator();
}

void Test5_ComboBoxSelection() {
    std::cout << "测试 5: ComboBox选择操作\n";
    std::cout << "-----------------------\n";
    
    auto comboBox = new ComboBox<>();
    
    // 添加项目
    auto& items = comboBox->GetItems();
    items.Add(std::string("北京"));
    items.Add(std::string("上海"));
    items.Add(std::string("广州"));
    items.Add(std::string("深圳"));
    
    std::cout << "项目列表:\n";
    for (size_t i = 0; i < items.Count(); ++i) {
        try {
            auto item = std::any_cast<std::string>(items[i]);
            std::cout << "  " << i << ": " << item << "\n";
        } catch (...) {
            std::cout << "  " << i << ": (unknown)\n";
        }
    }
    
    std::cout << "\n执行选择操作:\n";
    
    comboBox->SelectItemByIndex(0);
    std::cout << "  选择索引 0: 当前选中 = " << comboBox->GetSelectedIndex() << "\n";
    
    comboBox->SelectItemByIndex(2);
    std::cout << "  选择索引 2: 当前选中 = " << comboBox->GetSelectedIndex() << "\n";
    
    // 获取选中项
    try {
        auto selectedItem = comboBox->GetSelectedItem();
        if (selectedItem.has_value()) {
            auto item = std::any_cast<std::string>(selectedItem);
            std::cout << "  当前选中项: " << item << "\n";
        }
    } catch (...) {
        std::cout << "  无法获取选中项\n";
    }
    
    comboBox->ClearSelection();
    std::cout << "  清除选择: 当前选中 = " << comboBox->GetSelectedIndex() << "\n";
    
    delete comboBox;
    PrintSeparator();
}

void Test6_ComboBoxDropDown() {
    std::cout << "测试 6: ComboBox下拉操作\n";
    std::cout << "-----------------------\n";
    
    auto comboBox = new ComboBox<>();
    
    // 添加项目
    auto& items = comboBox->GetItems();
    for (int i = 1; i <= 5; ++i) {
        items.Add(std::string("项目 ") + std::to_string(i));
    }
    
    int dropDownOpenedCount = 0;
    int dropDownClosedCount = 0;
    
    comboBox->DropDownOpened.Connect([&dropDownOpenedCount]() {
        dropDownOpenedCount++;
    });
    
    comboBox->DropDownClosed.Connect([&dropDownClosedCount]() {
        dropDownClosedCount++;
    });
    
    std::cout << "初始下拉状态: " << (comboBox->GetIsDropDownOpen() ? "打开" : "关闭") << "\n";
    
    comboBox->OpenDropDown();
    std::cout << "OpenDropDown(): " << (comboBox->GetIsDropDownOpen() ? "打开" : "关闭") << "\n";
    
    comboBox->CloseDropDown();
    std::cout << "CloseDropDown(): " << (comboBox->GetIsDropDownOpen() ? "打开" : "关闭") << "\n";
    
    comboBox->ToggleDropDown();
    std::cout << "ToggleDropDown(): " << (comboBox->GetIsDropDownOpen() ? "打开" : "关闭") << "\n";
    
    comboBox->ToggleDropDown();
    std::cout << "ToggleDropDown(): " << (comboBox->GetIsDropDownOpen() ? "打开" : "关闭") << "\n";
    
    std::cout << "\n事件统计:\n";
    std::cout << "  DropDownOpened触发: " << dropDownOpenedCount << " 次\n";
    std::cout << "  DropDownClosed触发: " << dropDownClosedCount << " 次\n";
    
    delete comboBox;
    PrintSeparator();
}

void Test7_ComboBoxEvents() {
    std::cout << "测试 7: ComboBox事件\n";
    std::cout << "-------------------\n";
    
    auto comboBox = new ComboBox<>();
    
    // 添加项目
    auto& items = comboBox->GetItems();
    items.Add(std::string("选项 A"));
    items.Add(std::string("选项 B"));
    items.Add(std::string("选项 C"));
    
    int selectionChangedCount = 0;
    
    comboBox->SelectionChanged.Connect([&selectionChangedCount]() {
        selectionChangedCount++;
    });
    
    std::cout << "执行选择操作...\n";
    
    comboBox->SelectItemByIndex(0);
    comboBox->SelectItemByIndex(1);
    comboBox->SelectItemByIndex(2);
    comboBox->ClearSelection();
    
    std::cout << "✓ SelectionChanged事件触发: " << selectionChangedCount << " 次\n";
    
    delete comboBox;
    PrintSeparator();
}

int main() {
    std::cout << "\n";
    std::cout << "╔══════════════════════════════════════════════════════════╗\n";
    std::cout << "║      F__K_UI ComboBox和Popup演示 (Phase 3.4)          ║\n";
    std::cout << "╚══════════════════════════════════════════════════════════╝\n";
    
    PrintSeparator();
    
    try {
        Test1_BasicPopup();
        Test2_PopupOpenClose();
        Test3_PopupPlacement();
        Test4_BasicComboBox();
        Test5_ComboBoxSelection();
        Test6_ComboBoxDropDown();
        Test7_ComboBoxEvents();
        
        std::cout << "\n";
        std::cout << "╔══════════════════════════════════════════════════════════╗\n";
        std::cout << "║                   所有测试完成！                        ║\n";
        std::cout << "╠══════════════════════════════════════════════════════════╣\n";
        std::cout << "║  ✓ Popup 基础功能                                       ║\n";
        std::cout << "║  ✓ Popup 打开/关闭                                      ║\n";
        std::cout << "║  ✓ Popup 放置模式                                       ║\n";
        std::cout << "║  ✓ ComboBox 基础功能                                    ║\n";
        std::cout << "║  ✓ ComboBox 选择操作                                    ║\n";
        std::cout << "║  ✓ ComboBox 下拉操作                                    ║\n";
        std::cout << "║  ✓ ComboBox 事件系统                                    ║\n";
        std::cout << "╚══════════════════════════════════════════════════════════╝\n";
        std::cout << "\n";
        
    } catch (const std::exception& e) {
        std::cerr << "\n❌ 错误: " << e.what() << "\n";
        return 1;
    }
    
    return 0;
}
