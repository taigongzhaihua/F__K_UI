/**
 * @file repeatbutton_demo.cpp
 * @brief RepeatButton 视觉状态测试程序
 * 
 * 用于测试 RepeatButton 的视觉状态是否正确跟随状态机变化
 */

#include "fk/app/Application.h"
#include "fk/ui/Window.h"
#include "fk/ui/StackPanel.h"
#include "fk/ui/RepeatButton.h"
#include "fk/ui/ScrollBar.h"
#include "fk/ui/TextBlock.h"
#include "fk/ui/Border.h"
#include "fk/ui/Brush.h"
#include <iostream>
#include <memory>

using namespace fk::ui;
using namespace fk::core;

int main()
{
    auto application = std::make_unique<fk::Application>();

    auto window = std::make_shared<Window>();
    window->Title("RepeatButton 视觉状态测试");
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
            ->Text("RepeatButton 视觉状态测试")
            ->FontSize(24)
            ->Foreground(new SolidColorBrush(Color::FromRGB(50, 50, 50, 255)))
            ->Margin(0.0f, 0.0f, 0.0f, 20.0f)
    );

    // ===== 点击计数显示 =====
    auto* countText = (new TextBlock())
        ->Text("点击次数: 0")
        ->FontSize(18)
        ->Foreground(new SolidColorBrush(Color::FromRGB(80, 80, 80, 255)))
        ->Margin(0.0f, 0.0f, 0.0f, 20.0f);
    mainPanel->AddChild(countText);

    static int clickCount = 0;

    // ===== 测试1：独立的 RepeatButton（应该正常工作） =====
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
            ->Text("独立 RepeatButton（应正常工作）:")
            ->FontSize(16)
            ->FontWeight(FontWeight::SemiBold)
            ->Margin(0.0f, 0.0f, 0.0f, 10.0f)
    );

    auto* repeatBtn1 = (new RepeatButton())
        ->Content("按住我")
        ->SetDelay(300)
        ->SetInterval(100)
        ->Background(new SolidColorBrush(Color::FromRGB(100, 149, 237, 255)))
        ->MouseOverBackground(Color::FromRGB(65, 105, 225, 255))
        ->PressedBackground(Color::FromRGB(0, 0, 139, 255))
        ->Margin(0.0f, 0.0f, 0.0f, 10.0f);
    
    repeatBtn1->Click += [countText]() {
        clickCount++;
        countText->Text("点击次数: " + std::to_string(clickCount));
        std::cout << "独立 RepeatButton clicked! Count: " << clickCount << std::endl;
    };
    
    group1->AddChild(repeatBtn1);
    section1->Child(group1);
    mainPanel->AddChild(section1);

    // ===== 测试2：ScrollBar 中的 RepeatButton（可能有问题） =====
    auto* section2 = (new Border())
        ->Background(new SolidColorBrush(Color::FromRGB(255, 255, 255, 255)))
        ->BorderBrush(new SolidColorBrush(Color::FromRGB(200, 200, 200, 255)))
        ->BorderThickness(1.0f)
        ->CornerRadius(5.0f)
        ->Padding(15.0f)
        ->Margin(0.0f, 0.0f, 0.0f, 20.0f);

    auto* group2 = (new StackPanel())
        ->SetOrient(Orientation::Horizontal);

    group2->AddChild(
        (new TextBlock())
            ->Text("ScrollBar（检查上下箭头按钮）:")
            ->FontSize(16)
            ->FontWeight(FontWeight::SemiBold)
            ->SetVAlign(VerticalAlignment::Center)
            ->Margin(0.0f, 0.0f, 20.0f, 0.0f)
    );

    // 垂直滚动条
    auto* vScrollBar = new ScrollBar();
    vScrollBar->SetOrientation(Orientation::Vertical);
    vScrollBar->Height(150);
    vScrollBar->Width(20);
    vScrollBar->SetMinimum(0);
    vScrollBar->SetMaximum(100);
    vScrollBar->SetValue(30);
    vScrollBar->SetViewportSize(20);
    
    vScrollBar->ValueChanged += [](float oldValue, float newValue) {
        std::cout << "ScrollBar Value: " << newValue << std::endl;
    };
    
    group2->AddChild(vScrollBar);

    // 水平滚动条
    auto* hScrollBar = new ScrollBar();
    hScrollBar->SetOrientation(Orientation::Horizontal);
    hScrollBar->Width(200);
    hScrollBar->Height(20);
    hScrollBar->SetMinimum(0);
    hScrollBar->SetMaximum(100);
    hScrollBar->SetValue(50);
    hScrollBar->SetViewportSize(30);
    hScrollBar->Margin(20.0f, 0.0f, 0.0f, 0.0f);
    hScrollBar->SetVAlign(VerticalAlignment::Center);
    
    group2->AddChild(hScrollBar);

    section2->Child(group2);
    mainPanel->AddChild(section2);

    // ===== 说明文字 =====
    mainPanel->AddChild(
        (new TextBlock())
            ->Text("测试说明:\n"
                   "1. 独立 RepeatButton 应该有正常的悬停/按下颜色变化\n"
                   "2. ScrollBar 的上下箭头按钮检查是否有相同的视觉反馈\n"
                   "3. 按住箭头按钮应该持续改变滚动条值")
            ->FontSize(12)
            ->Foreground(new SolidColorBrush(Color::FromRGB(120, 120, 120, 255)))
    );

    window->Content(mainPanel);
    application->Run(window);

    return 0;
}
