/**
 * @file scrollviewer_demo.cpp
 * @brief ScrollViewer 控件示例程序
 * 
 * 功能展示：
 * 1. 基本的垂直滚动
 * 2. 水平+垂直滚动
 * 3. 不同的 ScrollBarVisibility 设置
 * 4. 编程方式滚动
 */

#include "fk/app/Application.h"
#include "fk/ui/Window.h"
#include "fk/ui/layouts/StackPanel.h"
#include "fk/ui/text/TextBlock.h"
#include "fk/ui/controls/Border.h"
#include "fk/ui/buttons/Button.h"
#include "fk/ui/graphics/Brush.h"
#include "fk/ui/layouts/Grid.h"
#include "fk/ui/scrolling/ScrollViewer.h"
#include <iostream>
#include <memory>
#include <sstream>

using namespace fk::ui;
using namespace fk::core;

int main()
{
    auto application = std::make_unique<fk::Application>();

    auto window = std::make_shared<Window>();
    window->Title("ScrollViewer 示例");
    window->Width(800);
    window->Height(600);

    // 主布局：水平两列
    auto* mainGrid = new Grid();
    mainGrid->ColumnDefinitions({
        ColumnDefinition::Star(1),
        ColumnDefinition::Star(1)
    });
    mainGrid->Padding(10.0f);
    mainGrid->Background(new SolidColorBrush(Color::FromRGB(240, 240, 240, 255)));

    // ===== 左侧：垂直滚动示例 =====
    auto* leftPanel = (new StackPanel())
        ->SetOrient(Orientation::Vertical)
        ->Margin(5.0f);

    leftPanel->AddChild(
        (new TextBlock())
            ->Text("垂直滚动 (Visible)")
            ->FontSize(18)
            ->FontWeight(FontWeight::Bold)
            ->Margin(0.0f, 0.0f, 0.0f, 10.0f)
    );

    // 创建 ScrollViewer
    auto* scrollViewer1 = new ScrollViewer();
    scrollViewer1->SetVerticalScrollBarVisibility(ScrollBarVisibility::Auto);
    scrollViewer1->SetHorizontalScrollBarVisibility(ScrollBarVisibility::Auto);
    scrollViewer1->Height(400);
    scrollViewer1->SetBackground(new SolidColorBrush(Color::FromRGB(255, 255, 255, 255)));

    // 创建长内容
    auto* contentPanel1 = (new StackPanel())
        ->SetOrient(Orientation::Vertical)
        ->Padding(10.0f);

    // 添加很多项目来测试滚动
    for (int i = 1; i <= 30; ++i) {
        std::ostringstream oss;
        oss << "项目 " << i << " - 这是一段测试文本";
        
        auto* item = (new Border())
            ->Background(new SolidColorBrush(
                i % 2 == 0 
                    ? Color::FromRGB(245, 245, 245, 255) 
                    : Color::FromRGB(255, 255, 255, 255)))
            ->BorderBrush(new SolidColorBrush(Color::FromRGB(220, 220, 220, 255)))
            ->BorderThickness(1.0f)
            ->Padding(10.0f)
            ->Margin(0.0f, 0.0f, 0.0f, 5.0f);

        item->Child(
            (new TextBlock())
                ->Text(oss.str())
                ->FontSize(14)
        );

        contentPanel1->AddChild(item);
    }

    scrollViewer1->Content(contentPanel1);

    // 监听滚动事件
    scrollViewer1->ScrollChanged += [](const ScrollChangedEventArgs& e) {
        std::cout << "垂直滚动: Offset=" << e.verticalOffset 
                  << ", Extent=" << e.extentHeight 
                  << ", Viewport=" << e.viewportHeight << std::endl;
    };

    leftPanel->AddChild(scrollViewer1);

    // 滚动控制按钮
    auto* buttonPanel1 = (new StackPanel())
        ->SetOrient(Orientation::Horizontal)
        ->Margin(0.0f, 10.0f, 0.0f, 0.0f);

    auto* btnTop = (new Button())
        ->Content("到顶部")
        ->Width(80)
        ->Height(30)
        ->Margin(0.0f, 0.0f, 10.0f, 0.0f);
    btnTop->Click += [scrollViewer1]() {
        scrollViewer1->ScrollToTop();
    };
    buttonPanel1->AddChild(btnTop);

    auto* btnBottom = (new Button())
        ->Content("到底部")
        ->Width(80)
        ->Height(30)
        ->Margin(0.0f, 0.0f, 10.0f, 0.0f);
    btnBottom->Click += [scrollViewer1]() {
        scrollViewer1->ScrollToBottom();
    };
    buttonPanel1->AddChild(btnBottom);

    auto* btnPageDown = (new Button())
        ->Content("下一页")
        ->Width(80)
        ->Height(30);
    btnPageDown->Click += [scrollViewer1]() {
        scrollViewer1->PageDown();
    };
    buttonPanel1->AddChild(btnPageDown);

    leftPanel->AddChild(buttonPanel1);

    Grid::SetColumn(leftPanel, 0);
    mainGrid->AddChild(leftPanel);

    // ===== 右侧：双向滚动示例 =====
    auto* rightPanel = (new StackPanel())
        ->SetOrient(Orientation::Vertical)
        ->Margin(5.0f);

    rightPanel->AddChild(
        (new TextBlock())
            ->Text("双向滚动 (Auto)")
            ->FontSize(18)
            ->FontWeight(FontWeight::Bold)
            ->Margin(0.0f, 0.0f, 0.0f, 10.0f)
    );

    // 创建双向 ScrollViewer
    auto* scrollViewer2 = new ScrollViewer();
    scrollViewer2->SetVerticalScrollBarVisibility(ScrollBarVisibility::Auto);
    scrollViewer2->SetHorizontalScrollBarVisibility(ScrollBarVisibility::Auto);
    scrollViewer2->Height(400);
    scrollViewer2->SetBackground(new SolidColorBrush(Color::FromRGB(255, 255, 255, 255)));

    // 创建宽且长的内容
    auto* contentPanel2 = (new StackPanel())
        ->SetOrient(Orientation::Vertical)
        ->Padding(10.0f)
        ->Width(600);  // 比视口宽，触发水平滚动

    for (int i = 1; i <= 20; ++i) {
        std::ostringstream oss;
        oss << "第 " << i << " 行 - 这是一段很长的文本内容，用于测试水平滚动功能。"
            << "当文本超出容器宽度时，应该显示水平滚动条。";

        auto* item = (new Border())
            ->Background(new SolidColorBrush(
                i % 2 == 0 
                    ? Color::FromRGB(230, 240, 255, 255) 
                    : Color::FromRGB(255, 255, 255, 255)))
            ->BorderBrush(new SolidColorBrush(Color::FromRGB(180, 200, 230, 255)))
            ->BorderThickness(1.0f)
            ->Padding(10.0f)
            ->Margin(0.0f, 0.0f, 0.0f, 5.0f);

        item->Child(
            (new TextBlock())
                ->Text(oss.str())
                ->FontSize(14)
        );

        contentPanel2->AddChild(item);
    }

    scrollViewer2->Content(contentPanel2);

    // 监听滚动事件
    scrollViewer2->ScrollChanged += [](const ScrollChangedEventArgs& e) {
        std::cout << "双向滚动: H=" << e.horizontalOffset 
                  << ", V=" << e.verticalOffset << std::endl;
    };

    rightPanel->AddChild(scrollViewer2);

    // 滚动状态显示
    auto* statusText = (new TextBlock())
        ->Text("滚动状态：等待滚动...")
        ->FontSize(12)
        ->Margin(0.0f, 10.0f, 0.0f, 0.0f);

    scrollViewer2->ScrollChanged += [statusText](const ScrollChangedEventArgs& e) {
        std::ostringstream oss;
        oss << "H: " << static_cast<int>(e.horizontalOffset) 
            << "/" << static_cast<int>(e.extentWidth - e.viewportWidth)
            << "  V: " << static_cast<int>(e.verticalOffset)
            << "/" << static_cast<int>(e.extentHeight - e.viewportHeight);
        statusText->Text(oss.str());
    };

    rightPanel->AddChild(statusText);

    // 控制按钮
    auto* buttonPanel2 = (new StackPanel())
        ->SetOrient(Orientation::Horizontal)
        ->Margin(0.0f, 10.0f, 0.0f, 0.0f);

    auto* btnHome = (new Button())
        ->Content("起始")
        ->Width(60)
        ->Height(30)
        ->Margin(0.0f, 0.0f, 10.0f, 0.0f);
    btnHome->Click += [scrollViewer2]() {
        scrollViewer2->ScrollToHome();
    };
    buttonPanel2->AddChild(btnHome);

    auto* btnEnd = (new Button())
        ->Content("结束")
        ->Width(60)
        ->Height(30)
        ->Margin(0.0f, 0.0f, 10.0f, 0.0f);
    btnEnd->Click += [scrollViewer2]() {
        scrollViewer2->ScrollToEnd();
    };
    buttonPanel2->AddChild(btnEnd);

    auto* btnLeft = (new Button())
        ->Content("◀")
        ->Width(40)
        ->Height(30)
        ->Margin(0.0f, 0.0f, 5.0f, 0.0f);
    btnLeft->Click += [scrollViewer2]() {
        scrollViewer2->PageLeft();
    };
    buttonPanel2->AddChild(btnLeft);

    auto* btnRight = (new Button())
        ->Content("▶")
        ->Width(40)
        ->Height(30);
    btnRight->Click += [scrollViewer2]() {
        scrollViewer2->PageRight();
    };
    buttonPanel2->AddChild(btnRight);

    rightPanel->AddChild(buttonPanel2);

    Grid::SetColumn(rightPanel, 1);
    mainGrid->AddChild(rightPanel);

    window->Content(mainGrid);

    application->Run(window);

    return 0;
}
