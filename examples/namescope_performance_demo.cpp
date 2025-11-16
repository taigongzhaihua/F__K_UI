/**
 * @file namescope_performance_demo.cpp
 * @brief 演示 NameScope 的性能优化功能
 * 
 * 此演示展示了：
 * 1. Window 自动创建 NameScope
 * 2. 手动为容器创建 NameScope
 * 3. FindName() vs FindNameFast() 的性能对比
 * 4. 嵌套 NameScope 的使用
 */

#include <iostream>
#include <chrono>
#include <fk/ui/Window.h>
#include <fk/ui/Button.h>
#include <fk/ui/TextBlock.h>
#include <fk/ui/StackPanel.h>
#include <fk/ui/Border.h>
#include <fk/ui/NameScope.h>

using namespace fk::ui;
using namespace std::chrono;

// ANSI颜色代码
#define RESET   "\033[0m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN    "\033[36m"

void printSeparator() {
    std::cout << "\n" << std::string(60, '=') << "\n\n";
}

void printTestHeader(const std::string& title) {
    std::cout << CYAN << "测试: " << title << RESET << "\n";
    std::cout << std::string(60, '-') << "\n";
}

/**
 * 测试1：Window 自动创建 NameScope
 */
void test1_window_auto_namescope() {
    printTestHeader("Window 自动创建 NameScope");
    
    std::cout << "创建窗口...\n";
    auto* window = new Window();
    std::cout << "设置标题...\n";
    window->Title("NameScope 测试窗口");
    
    // 检查 Window 是否自动创建了 NameScope
    auto* nameScope = window->GetNameScope();
    if (nameScope) {
        std::cout << GREEN << "✓ Window 自动创建了 NameScope" << RESET << "\n";
    } else {
        std::cout << "✗ Window 未创建 NameScope\n";
    }
    
    // 添加一些元素
    auto* panel = new StackPanel();
    
    auto* button1 = new Button();
    button1->Name("button1");
    button1->Content("按钮1");
    panel->AddChild(button1);
    
    auto* button2 = new Button();
    button2->Name("button2");
    button2->Content("按钮2");
    panel->AddChild(button2);
    
    auto* text = new TextBlock();
    text->Name("statusText");
    text->Text("状态文本");
    panel->AddChild(text);
    
    window->Content(panel);
    
    // 测试查找 - 应该使用 NameScope (O(1))
    std::cout << "\n查找测试:\n";
    
    auto* found1 = window->FindName("button1");
    if (found1) {
        std::cout << GREEN << "✓ 找到 'button1'" << RESET << "\n";
    }
    
    auto* found2 = window->FindName("button2");
    if (found2) {
        std::cout << GREEN << "✓ 找到 'button2'" << RESET << "\n";
    }
    
    auto* foundText = window->FindName("statusText");
    if (foundText) {
        std::cout << GREEN << "✓ 找到 'statusText'" << RESET << "\n";
    }
    
    // 测试 FindNameFast (使用 NameScope)
    std::cout << "测试 FindNameFast...\n";
    auto* foundFast = window->FindNameFast("button1");
    if (foundFast) {
        std::cout << GREEN << "✓ FindNameFast 成功查找 'button1'" << RESET << "\n";
    }
    
    std::cout << "测试1完成 (保留窗口对象)\n";
    // 注意：实际应用中应该正确管理内存，这里为了演示简化
}

/**
 * 测试2：手动创建 NameScope 以优化性能
 */
void test2_manual_namescope() {
    printTestHeader("手动为容器创建 NameScope");
    
    auto* window = new Window();
    
    auto* mainPanel = new StackPanel();
    
    // 手动为大型面板创建 NameScope
    mainPanel->CreateNameScope();
    
    auto* scope = mainPanel->GetNameScope();
    if (scope) {
        std::cout << GREEN << "✓ 成功为 StackPanel 创建 NameScope" << RESET << "\n";
    }
    
    // 添加许多子元素
    const int childCount = 20;
    std::cout << "\n添加 " << childCount << " 个子元素...\n";
    
    for (int i = 0; i < childCount; ++i) {
        auto* button = new Button();
        button->Name("button_" + std::to_string(i));
        button->Content("按钮 " + std::to_string(i));
        mainPanel->AddChild(button);
    }
    
    window->Content(mainPanel);
    
    // 测试查找
    std::cout << "\n查找测试:\n";
    auto* found = mainPanel->FindName("button_10");
    if (found) {
        std::cout << GREEN << "✓ 找到 'button_10'" << RESET << "\n";
    }
    
    auto* foundFast = mainPanel->FindNameFast("button_15");
    if (foundFast) {
        std::cout << GREEN << "✓ FindNameFast 找到 'button_15'" << RESET << "\n";
    }
    
    // 简化内存管理
}

/**
 * 测试3：性能对比
 */
void test3_performance_comparison() {
    printTestHeader("性能对比: FindName vs FindNameFast");
    
    auto* window = new Window();
    auto* panel = new StackPanel();
    
    // 创建大量元素（模拟大型UI）
    const int elementCount = 100;
    std::cout << "创建 " << elementCount << " 个元素...\n\n";
    
    for (int i = 0; i < elementCount; ++i) {
        auto* button = new Button();
        button->Name("element_" + std::to_string(i));
        button->Content("Element " + std::to_string(i));
        panel->AddChild(button);
    }
    
    window->Content(panel);
    
    // 没有 NameScope 的情况 - 递归查找
    std::cout << YELLOW << "场景1: 无 NameScope (递归查找)" << RESET << "\n";
    auto start1 = high_resolution_clock::now();
    for (int i = 0; i < 10; ++i) {
        window->FindName("element_50");
    }
    auto end1 = high_resolution_clock::now();
    auto duration1 = duration_cast<microseconds>(end1 - start1).count();
    std::cout << "10次查找耗时: " << duration1 << " 微秒\n";
    std::cout << "平均每次: " << duration1 / 10.0 << " 微秒\n";
    
    // 现在为 panel 创建 NameScope
    panel->CreateNameScope();
    std::cout << "\n" << YELLOW << "场景2: 有 NameScope (哈希表查找)" << RESET << "\n";
    
    auto start2 = high_resolution_clock::now();
    for (int i = 0; i < 10; ++i) {
        panel->FindNameFast("element_50");
    }
    auto end2 = high_resolution_clock::now();
    auto duration2 = duration_cast<microseconds>(end2 - start2).count();
    std::cout << "10次查找耗时: " << duration2 << " 微秒\n";
    std::cout << "平均每次: " << duration2 / 10.0 << " 微秒\n";
    
    // 计算提升
    if (duration2 > 0) {
        double speedup = static_cast<double>(duration1) / duration2;
        std::cout << "\n" << GREEN << "性能提升: " << speedup << "x" << RESET << "\n";
    }
    
    // 简化内存管理
}

/**
 * 测试4：嵌套 NameScope
 */
void test4_nested_namescopes() {
    printTestHeader("嵌套 NameScope");
    
    auto* window = new Window();
    
    // 主面板
    auto* mainPanel = new StackPanel();
    
    // Header 区域 - 有自己的 NameScope
    auto* headerPanel = new StackPanel();
    headerPanel->CreateNameScope();
    
    auto* headerButton = new Button();
    headerButton->Name("actionButton");
    headerButton->Content("Header 动作");
    headerPanel->AddChild(headerButton);
    
    // Content 区域 - 有自己的 NameScope
    auto* contentPanel = new StackPanel();
    contentPanel->CreateNameScope();
    
    auto* contentButton = new Button();
    contentButton->Name("actionButton");  // 相同名称！
    contentButton->Content("Content 动作");
    contentPanel->AddChild(contentButton);
    
    mainPanel->AddChild(headerPanel);
    mainPanel->AddChild(contentPanel);
    window->Content(mainPanel);
    
    std::cout << "创建了两个区域，都有名为 'actionButton' 的按钮\n\n";
    
    // 从不同作用域查找
    auto* foundInHeader = headerPanel->FindName("actionButton");
    if (foundInHeader && foundInHeader == headerButton) {
        std::cout << GREEN << "✓ Header 区域找到正确的 actionButton" << RESET << "\n";
    }
    
    auto* foundInContent = contentPanel->FindName("actionButton");
    if (foundInContent && foundInContent == contentButton) {
        std::cout << GREEN << "✓ Content 区域找到正确的 actionButton" << RESET << "\n";
    }
    
    // 验证它们是不同的按钮
    if (foundInHeader != foundInContent) {
        std::cout << GREEN << "✓ 两个区域的按钮是独立的（不同实例）" << RESET << "\n";
    }
    
    // 简化内存管理
}

/**
 * 测试5：NameScope API 完整性
 */
void test5_namescope_api() {
    printTestHeader("NameScope API 测试");
    
    auto* window = new Window();
    auto* panel = new StackPanel();
    panel->CreateNameScope();
    
    auto* scope = panel->GetNameScope();
    if (!scope) {
        std::cout << "✗ 无法获取 NameScope\n";
        delete window;
        return;
    }
    
    // 手动注册名称
    auto* button1 = new Button();
    button1->Name("testButton1");
    
    bool registered = scope->RegisterName("testButton1", button1);
    if (registered) {
        std::cout << GREEN << "✓ 手动注册名称成功" << RESET << "\n";
    }
    
    // 检查是否包含
    if (scope->Contains("testButton1")) {
        std::cout << GREEN << "✓ Contains() 方法工作正常" << RESET << "\n";
    }
    
    // 查找
    auto* found = scope->FindName("testButton1");
    if (found == button1) {
        std::cout << GREEN << "✓ FindName() 找到正确的元素" << RESET << "\n";
    }
    
    // 更新名称
    bool updated = scope->UpdateName("testButton1", "renamedButton", button1);
    if (updated && scope->Contains("renamedButton") && !scope->Contains("testButton1")) {
        std::cout << GREEN << "✓ UpdateName() 成功更新名称" << RESET << "\n";
    }
    
    // 取消注册
    scope->UnregisterName("renamedButton");
    if (!scope->Contains("renamedButton")) {
        std::cout << GREEN << "✓ UnregisterName() 成功移除名称" << RESET << "\n";
    }
    
    // 获取数量
    std::cout << "\nNameScope 中的名称数量: " << scope->GetCount() << "\n";
    
    // 清空
    scope->Clear();
    if (scope->GetCount() == 0) {
        std::cout << GREEN << "✓ Clear() 成功清空所有名称" << RESET << "\n";
    }
    
    // button1 会被自动管理，不需要手动删除
    // 简化内存管理
}

int main() {
    std::cout << MAGENTA;
    std::cout << "╔═══════════════════════════════════════════════════╗\n";
    std::cout << "║     NameScope 性能优化功能演示                   ║\n";
    std::cout << "║     (混合方案: NameScope + 哈希表)               ║\n";
    std::cout << "╚═══════════════════════════════════════════════════╝\n";
    std::cout << RESET;
    
    printSeparator();
    test1_window_auto_namescope();
    
    printSeparator();
    test2_manual_namescope();
    
    printSeparator();
    test3_performance_comparison();
    
    printSeparator();
    test4_nested_namescopes();
    
    printSeparator();
    test5_namescope_api();
    
    printSeparator();
    
    std::cout << GREEN;
    std::cout << "╔═══════════════════════════════════════════════════╗\n";
    std::cout << "║  ✓ 所有测试完成！                                ║\n";
    std::cout << "║                                                   ║\n";
    std::cout << "║  关键特性：                                       ║\n";
    std::cout << "║  • Window 自动创建 NameScope                      ║\n";
    std::cout << "║  • 手动创建 NameScope 以优化性能                  ║\n";
    std::cout << "║  • FindNameFast() 使用 O(1) 哈希表查找           ║\n";
    std::cout << "║  • 支持嵌套 NameScope                             ║\n";
    std::cout << "║  • 完整的 NameScope API                           ║\n";
    std::cout << "╚═══════════════════════════════════════════════════╝\n";
    std::cout << RESET;
    
    return 0;
}
