#include "fk/app/Application.h"
#include "fk/ui/Window.h"
#include "fk/ui/layouts/StackPanel.h"
#include "fk/ui/buttons/RadioButton.h"
#include "fk/ui/text/TextBlock.h"
#include "fk/ui/controls/Border.h"
#include "fk/ui/graphics/Brush.h"
#include <iostream>
#include <memory>

using namespace fk::ui;
using namespace fk::core;

/**
 * RadioButton 示例程序
 * 
 * 功能展示：
 * 1. 基本的 RadioButton 分组
 * 2. 多个独立的 RadioButton 组
 * 3. 命名分组（跨容器）
 * 4. 选中状态监听
 */

int main()
{
    auto application = std::make_unique<fk::Application>();

    auto window = std::make_shared<Window>();
    window->Title("RadioButton 示例");
    window->Width(600);
    window->Height(500);

    // 主容器
    auto* mainPanel = (new StackPanel())
        ->SetOrient(Orientation::Vertical)
        ->Padding(20.0f)
        ->Background(new SolidColorBrush(Color::FromRGB(250, 250, 250, 255)));

    // ===== 标题 =====
    mainPanel->AddChild(
        (new TextBlock())
            ->Text("RadioButton 控件示例")
            ->FontSize(24)
            ->Foreground(new SolidColorBrush(Color::FromRGB(50, 50, 50, 255)))
            ->Margin(0.0f, 0.0f, 0.0f, 20.0f)
    );

    // ===== 示例1：基本的单选组（同一容器内自动分组） =====
    auto* section1 = (new Border())
        ->Background(new SolidColorBrush(Color::FromRGB(255, 255, 255, 255)))
        ->BorderBrush(new SolidColorBrush(Color::FromRGB(200, 200, 200, 255)))
        ->BorderThickness(1.0f)
        ->CornerRadius(5.0f)
        ->Padding(15.0f)
        ->Margin(0.0f, 0.0f, 0.0f, 20.0f);

    auto* group1 = (new StackPanel())
        ->SetOrient(Orientation::Vertical);

    group1->AddChild(
        (new TextBlock())
            ->Text("选择你喜欢的颜色：")
            ->FontSize(16)
            ->FontWeight(FontWeight::SemiBold)
            ->Margin(0.0f, 0.0f, 0.0f, 10.0f)
    );

    auto* radio1_1 = (new RadioButton())
        ->Content("红色")
        ->IsChecked(true)  // 默认选中
        ->Margin(0.0f, 0.0f, 0.0f, 8.0f);

    auto* radio1_2 = (new RadioButton())
        ->Content("绿色")
        ->Margin(0.0f, 0.0f, 0.0f, 8.0f);

    auto* radio1_3 = (new RadioButton())
        ->Content("蓝色")
        ->Margin(0.0f, 0.0f, 0.0f, 8.0f);

    // 添加选中事件监听
    radio1_1->Checked += [](std::optional<bool> checked) {
        std::cout << "选择了红色" << std::endl;
    };
    radio1_2->Checked += [](std::optional<bool> checked) {
        std::cout << "选择了绿色" << std::endl;
    };
    radio1_3->Checked += [](std::optional<bool> checked) {
        std::cout << "选择了蓝色" << std::endl;
    };

    group1->AddChild(radio1_1);
    group1->AddChild(radio1_2);
    group1->AddChild(radio1_3);
    section1->Child(group1);
    mainPanel->AddChild(section1);

    // ===== 示例2：第二个独立的单选组 =====
    auto* section2 = (new Border())
        ->Background(new SolidColorBrush(Color::FromRGB(255, 255, 255, 255)))
        ->BorderBrush(new SolidColorBrush(Color::FromRGB(200, 200, 200, 255)))
        ->BorderThickness(1.0f)
        ->CornerRadius(5.0f)
        ->Padding(15.0f)
        ->Margin(0.0f, 0.0f, 0.0f, 20.0f);

    auto* group2 = (new StackPanel())
        ->SetOrient(Orientation::Vertical);

    group2->AddChild(
        (new TextBlock())
            ->Text("选择你的年龄段：")
            ->FontSize(16)
            ->FontWeight(FontWeight::SemiBold)
            ->Margin(0.0f, 0.0f, 0.0f, 10.0f)
    );

    auto* radio2_1 = (new RadioButton())
        ->Content("18岁以下")
        ->Margin(0.0f, 0.0f, 0.0f, 8.0f);

    auto* radio2_2 = (new RadioButton())
        ->Content("18-30岁")
        ->IsChecked(true)
        ->Margin(0.0f, 0.0f, 0.0f, 8.0f);

    auto* radio2_3 = (new RadioButton())
        ->Content("30-50岁")
        ->Margin(0.0f, 0.0f, 0.0f, 8.0f);

    auto* radio2_4 = (new RadioButton())
        ->Content("50岁以上");

    radio2_1->Checked += [](std::optional<bool> checked) {
        std::cout << "选择了：18岁以下" << std::endl;
    };
    radio2_2->Checked += [](std::optional<bool> checked) {
        std::cout << "选择了：18-30岁" << std::endl;
    };
    radio2_3->Checked += [](std::optional<bool> checked) {
        std::cout << "选择了：30-50岁" << std::endl;
    };
    radio2_4->Checked += [](std::optional<bool> checked) {
        std::cout << "选择了：50岁以上" << std::endl;
    };

    group2->AddChild(radio2_1);
    group2->AddChild(radio2_2);
    group2->AddChild(radio2_3);
    group2->AddChild(radio2_4);
    section2->Child(group2);
    mainPanel->AddChild(section2);

    // ===== 示例3：使用 GroupName 进行跨容器分组 =====
    auto* section3 = (new Border())
        ->Background(new SolidColorBrush(Color::FromRGB(255, 255, 255, 255)))
        ->BorderBrush(new SolidColorBrush(Color::FromRGB(200, 200, 200, 255)))
        ->BorderThickness(1.0f)
        ->CornerRadius(5.0f)
        ->Padding(15.0f);

    auto* group3 = (new StackPanel())
        ->SetOrient(Orientation::Vertical);

    group3->AddChild(
        (new TextBlock())
            ->Text("选择你喜欢的编程语言（使用命名分组）：")
            ->FontSize(16)
            ->FontWeight(FontWeight::SemiBold)
            ->Margin(0.0f, 0.0f, 0.0f, 10.0f)
    );

    // 创建两个子容器，但使用相同的 GroupName
    auto* subPanel1 = (new StackPanel())
        ->SetOrient(Orientation::Vertical);

    auto* radio3_1 = new RadioButton();
    radio3_1->Content("C++");
    radio3_1->GroupName("language");
    radio3_1->IsChecked(true);
    radio3_1->Margin(0.0f, 0.0f, 0.0f, 8.0f);

    auto* radio3_2 = new RadioButton();
    radio3_2->Content("Python");
    radio3_2->GroupName("language");
    radio3_2->Margin(0.0f, 0.0f, 0.0f, 8.0f);

    subPanel1->AddChild(radio3_1);
    subPanel1->AddChild(radio3_2);

    auto* subPanel2 = (new StackPanel())
        ->SetOrient(Orientation::Vertical);

    auto* radio3_3 = new RadioButton();
    radio3_3->Content("JavaScript");
    radio3_3->GroupName("language");
    radio3_3->Margin(0.0f, 0.0f, 0.0f, 8.0f);

    auto* radio3_4 = new RadioButton();
    radio3_4->Content("Rust");
    radio3_4->GroupName("language");

    subPanel2->AddChild(radio3_3);
    subPanel2->AddChild(radio3_4);

    radio3_1->Checked += [](std::optional<bool> checked) {
        std::cout << "选择了语言：C++" << std::endl;
    };
    radio3_2->Checked += [](std::optional<bool> checked) {
        std::cout << "选择了语言：Python" << std::endl;
    };
    radio3_3->Checked += [](std::optional<bool> checked) {
        std::cout << "选择了语言：JavaScript" << std::endl;
    };
    radio3_4->Checked += [](std::optional<bool> checked) {
        std::cout << "选择了语言：Rust" << std::endl;
    };

    group3->AddChild(subPanel1);
    group3->AddChild(
        (new TextBlock())
            ->Text("或者：")
            ->Margin(10.0f, 5.0f, 10.0f, 5.0f)
    );
    group3->AddChild(subPanel2);
    section3->Child(group3);
    mainPanel->AddChild(section3);

    // 设置窗口内容
    window->Content(mainPanel);

    application->Run(window);
    return 0;
}
