/**
 * @file textbox_wrapping_demo.cpp
 * @brief TextBox 自动换行和多行输入功能演示
 * 
 * 展示功能：
 * 1. TextWrapping - 自动换行（文本超出宽度时折行）
 * 2. AcceptsReturn - 多行输入（按 Enter 插入换行符）
 */

#include "fk/app/Application.h"
#include "fk/ui/Window.h"
#include "fk/ui/StackPanel.h"
#include "fk/ui/TextBox.h"
#include "fk/ui/TextBlock.h"
#include "fk/ui/Button.h"

#include <iostream>

using namespace fk;
using namespace fk::ui;

int main() {
    Application application;

    // 创建主窗口
    auto window = ui::window()
        ->Title("TextBox 换行功能演示")
        ->Width(600.0f)
        ->Height(500.0f);

    // 创建主面板
    auto mainPanel = ui::stackPanel()
        ->Orientation(Orientation::Vertical);

    // ============================================================
    // 示例 1: 单行 TextBox（默认，无换行）
    // ============================================================
    auto title1 = ui::textBlock()
        ->Text("1. 单行输入框（无换行）")
        ->FontSize(16.0f)
        ->Foreground("#333333");
    title1->SetMargin(fk::Thickness{0.0f, 0.0f, 0.0f, 10.0f});

    auto textBox1 = ui::textBox()
        ->Width(550.0f)
        ->Height(35.0f)
        ->PlaceholderText("输入文本...（超出宽度会自动滚动，不换行）")
        ->BorderBrush("#CCCCCC")
        ->BorderThickness(1.0f)
        ->Background("#FFFFFF")
        ->Margin(fk::Thickness{0.0f, 0.0f, 0.0f, 20.0f});

    // ============================================================
    // 示例 2: 自动换行 TextBox（TextWrapping::Wrap）
    // ============================================================
    auto title2 = ui::textBlock()
        ->Text("2. 自动换行输入框（TextWrapping::Wrap）")
        ->FontSize(16.0f)
        ->Foreground("#333333");
    title2->SetMargin(fk::Thickness{0.0f, 0.0f, 0.0f, 10.0f});

    auto textBox2 = ui::textBox()
        ->Width(550.0f)
        ->PlaceholderText("输入长文本...（超出宽度自动折行显示）")
        ->BorderBrush("#CCCCCC")
        ->BorderThickness(1.0f)
        ->Background("#FFFFFF")
        ->TextWrapping(TextWrapping::Wrap)  // 🎯 启用自动换行
        ->Margin(fk::Thickness{0.0f, 0.0f, 0.0f, 20.0f});

    // ============================================================
    // 示例 3: 多行输入 TextBox（AcceptsReturn）
    // ============================================================
    auto title3 = ui::textBlock()
        ->Text("3. 多行输入框（AcceptsReturn + TextWrapping）")
        ->FontSize(16.0f)
        ->Foreground("#333333");
    title3->SetMargin(fk::Thickness{0.0f, 0.0f, 0.0f, 10.0f});

    auto desc3 = ui::textBlock()
        ->Text("按 Enter 键插入换行符，支持多行文本编辑")
        ->FontSize(12.0f)
        ->Foreground("#666666");
    desc3->SetMargin(fk::Thickness{0.0f, 0.0f, 0.0f, 10.0f});

    auto textBox3 = ui::textBox()
        ->Width(550.0f)
        ->Height(150.0f)
        ->PlaceholderText("输入多行文本...\n按 Enter 键可以换行")
        ->BorderBrush("#CCCCCC")
        ->BorderThickness(1.0f)
        ->Background("#FFFFFF")
        ->AcceptsReturn(true)               // 🎯 允许 Enter 键插入换行
        ->TextWrapping(TextWrapping::Wrap)  // 🎯 启用自动换行
        ->Margin(fk::Thickness{0.0f, 0.0f, 0.0f, 10.0f});

    // ============================================================
    // 控制按钮：显示文本内容
    // ============================================================
    auto buttonPanel = ui::stackPanel()
        ->Orientation(Orientation::Horizontal)
        ->Margin(fk::Thickness{0.0f, 10.0f, 0.0f, 0.0f});

    auto showButton = ui::button()
        ->Content("显示所有输入框的内容")
        ->Width(200.0f)
        ->Height(35.0f);

    // 按钮点击事件
    showButton->Click += [textBox1, textBox2, textBox3](auto&) {
        std::cout << "========================================" << std::endl;
        std::cout << "输入框内容：" << std::endl;
        std::cout << "----------------------------------------" << std::endl;
        
        std::cout << "单行输入框：" << std::endl;
        std::cout << textBox1->GetText() << std::endl;
        std::cout << "----------------------------------------" << std::endl;
        
        std::cout << "自动换行输入框：" << std::endl;
        std::cout << textBox2->GetText() << std::endl;
        std::cout << "----------------------------------------" << std::endl;
        
        std::cout << "多行输入框：" << std::endl;
        std::cout << textBox3->GetText() << std::endl;
        std::cout << "========================================" << std::endl;
    };

    buttonPanel->AddChild(showButton);

    // ============================================================
    // 组装界面
    // ============================================================
    mainPanel->AddChild(title1);
    mainPanel->AddChild(textBox1);
    
    mainPanel->AddChild(title2);
    mainPanel->AddChild(textBox2);
    
    mainPanel->AddChild(title3);
    mainPanel->AddChild(desc3);
    mainPanel->AddChild(textBox3);
    
    mainPanel->AddChild(buttonPanel);

    window->SetContent(mainPanel);

    std::cout << "TextBox 换行功能演示" << std::endl;
    std::cout << "--------------------------------------------" << std::endl;
    std::cout << "1. 单行输入框：不换行，超出宽度自动滚动" << std::endl;
    std::cout << "2. 自动换行：文本超出宽度时自动折行显示" << std::endl;
    std::cout << "3. 多行输入：按 Enter 键插入换行符" << std::endl;
    std::cout << "--------------------------------------------" << std::endl;
    std::cout << "点击按钮查看输入内容（控制台输出）" << std::endl;

    application.Run(window);
    return 0;
}
