#include "fk/app/Application.h"
#include "fk/ui/Window.h"
#include "fk/ui/TextBlock.h"
#include "fk/ui/Brush.h"
#include "fk/ui/StackPanel.h"
#include "fk/ui/CheckBox.h"
#include "fk/ui/Thickness.h"
#include "fk/core/Logger.h"

#include <iostream>

int main(int argc, char **argv)
{
    fk::Application app;
    auto window = std::make_shared<fk::ui::Window>();
    window->Title("CheckBox 测试")
        ->Width(400)
        ->Height(300)
        ->Background(new fk::ui::SolidColorBrush(240, 245, 255));

    // 创建一个简单的 StackPanel 布局
    auto panel = new fk::ui::StackPanel();
    panel->Margin(fk::ui::Thickness(20));

    // 标题
    auto title = new fk::ui::TextBlock();
    title->Text("CheckBox 控件演示")
        ->FontSize(20)
        ->Margin(fk::ui::Thickness(0, 0, 0, 20));
    panel->AddChild(title);

    // CheckBox 1 - 已选中
    auto chk1 = new fk::ui::CheckBox();
    chk1->Name("chkNotifications")
        ->Content(
            (new fk::ui::TextBlock())
                ->Text("启用通知")
                ->FontSize(14))
        ->IsChecked(true)
        ->Margin(fk::ui::Thickness(0, 5, 0, 5));
    
    chk1->Checked += [](std::optional<bool>)
    {
        std::cout << "✓ 通知已启用" << std::endl;
    };
    chk1->Unchecked += [](std::optional<bool>)
    {
        std::cout << "✗ 通知已禁用" << std::endl;
    };
    panel->AddChild(chk1);

    // CheckBox 2 - 未选中
    auto chk2 = new fk::ui::CheckBox();
    chk2->Name("chkAutoUpdate")
        ->Content(
            (new fk::ui::TextBlock())
                ->Text("自动更新")
                ->FontSize(14))
        ->IsChecked(false)
        ->Margin(fk::ui::Thickness(0, 5, 0, 5));
    
    chk2->Checked += [](std::optional<bool>)
    {
        std::cout << "✓ 自动更新已启用" << std::endl;
    };
    chk2->Unchecked += [](std::optional<bool>)
    {
        std::cout << "✗ 自动更新已禁用" << std::endl;
    };
    panel->AddChild(chk2);

    // CheckBox 3 - 三态，不确定状态
    auto chk3 = new fk::ui::CheckBox();
    chk3->Name("chkAnalytics")
        ->Content(
            (new fk::ui::TextBlock())
                ->Text("发送分析数据（三态）")
                ->FontSize(14))
        ->IsThreeState(true)
        ->IsChecked(std::nullopt)
        ->Margin(fk::ui::Thickness(0, 5, 0, 5));
    
    chk3->Checked += [](std::optional<bool>)
    {
        std::cout << "✓ 分析数据: 启用" << std::endl;
    };
    chk3->Unchecked += [](std::optional<bool>)
    {
        std::cout << "✗ 分析数据: 禁用" << std::endl;
    };
    chk3->Indeterminate += [](std::optional<bool>)
    {
        std::cout << "? 分析数据: 稍后询问" << std::endl;
    };
    panel->AddChild(chk3);

    window->SetContent(panel);

    std::cout << "\n=== CheckBox 测试程序 ===" << std::endl;
    std::cout << "点击复选框测试功能：" << std::endl;
    std::cout << "- 勾选/取消勾选" << std::endl;
    std::cout << "- 三态切换（第三个复选框）" << std::endl;
    std::cout << "==========================\n" << std::endl;

    app.Run(window);
    return 0;
}
