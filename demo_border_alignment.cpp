#include "fk/ui/Window.h"
#include "fk/app/Application.h"
#include "fk/ui/Border.h"
#include "fk/ui/Button.h"
#include "fk/ui/Grid.h"
#include "fk/ui/TextBlock.h"
#include "fk/ui/Brush.h"
#include "fk/ui/StackPanel.h"
#include "fk/ui/Thickness.h"

using namespace fk::ui;
using namespace fk;

// 辅助函数：创建一个演示用的 Border
Border* CreateDemoBorder(const std::string& text, HorizontalAlignment hAlign, VerticalAlignment vAlign) {
    auto border = new Border();
    border->SetWidth(180);
    border->SetHeight(120);
    border->SetBorderThickness(Thickness(2));
    border->SetBorderBrush(new SolidColorBrush(Color::Black())); // Black border
    border->SetBackground(new SolidColorBrush(Color::White())); // White background
    border->SetMargin(Thickness(5));

    auto button = new Button();
    button->SetContent(text);
    button->SetHorizontalAlignment(hAlign);
    button->SetVerticalAlignment(vAlign);
    // Note: Button background requires a template to render. 
    // For this demo, we rely on the Border's background.
    // button->SetBackground(new SolidColorBrush(Color(0.4f, 0.6f, 1.0f))); 
    // button->SetPadding(Thickness(10)); 

    // Wrap button content in a Border to simulate button appearance if needed, 
    // or just let the button be a text container for now.
    // Actually, let's wrap the text in a Border inside the Button to show the "Button" color
    auto innerBorder = new Border();
    innerBorder->SetBackground(new SolidColorBrush(Color(0.6f, 0.8f, 1.0f))); // Light Blue
    innerBorder->SetPadding(Thickness(10));
    innerBorder->SetCornerRadius(CornerRadius(4));
    auto tb = new TextBlock();
    tb->SetText(text);
    innerBorder->SetChild(tb);
    
    button->SetContent(innerBorder);

    border->SetChild(button);
    return border;
}

// 辅助函数：创建一个演示 Stretch 的 Border
Border* CreateStretchBorder(const std::string& text) {
    auto border = new Border();
    border->SetWidth(180);
    border->SetHeight(120);
    border->SetBorderThickness(Thickness(2));
    border->SetBorderBrush(new SolidColorBrush(Color::Black()));
    border->SetBackground(new SolidColorBrush(Color::White()));
    border->SetMargin(Thickness(5));

    auto button = new Button();
    button->SetHorizontalAlignment(HorizontalAlignment::Stretch);
    button->SetVerticalAlignment(VerticalAlignment::Stretch);

    auto innerBorder = new Border();
    innerBorder->SetBackground(new SolidColorBrush(Color(0.6f, 1.0f, 0.8f))); // Light Green
    innerBorder->SetPadding(Thickness(10));
    auto tb = new TextBlock();
    tb->SetText(text);
    innerBorder->SetChild(tb);
    
    button->SetContent(innerBorder);

    border->SetChild(button);
    return border;
}

// 辅助函数：创建一个演示尺寸限制的 Border
Border* CreateConstrainedBorder() {
    auto border = new Border();
    border->SetWidth(180);
    border->SetHeight(120);
    border->SetBorderThickness(Thickness(2));
    border->SetBorderBrush(new SolidColorBrush(Color::Red())); // Red border
    border->SetBackground(new SolidColorBrush(Color::White()));
    border->SetMargin(Thickness(5));

    auto button = new Button();
    // 强制按钮尺寸大于 Border
    button->SetWidth(250); 
    button->SetHeight(150);
    button->SetHorizontalAlignment(HorizontalAlignment::Center);
    button->SetVerticalAlignment(VerticalAlignment::Center);

    auto innerBorder = new Border();
    innerBorder->SetBackground(new SolidColorBrush(Color(1.0f, 0.8f, 0.6f))); // Light Orange
    innerBorder->SetPadding(Thickness(10));
    auto tb = new TextBlock();
    tb->SetText("超大内容将被裁剪\nLong Text Content");
    innerBorder->SetChild(tb);
    
    button->SetContent(innerBorder);

    border->SetChild(button);
    return border;
}

int main() {
    auto app = std::make_shared<Application>();
    auto window = std::make_shared<Window>();
    window->SetTitle("Border Alignment & Layout Demo");
    window->SetWidth(1000);
    window->SetHeight(800);
    window->SetBackground(new SolidColorBrush(Color(0.8f, 0.8f, 0.8f))); // Darker gray background

    // 主布局
    auto mainGrid = new Grid();
    mainGrid->AddRowDefinition(RowDefinition::Pixel(50)); // 标题
    mainGrid->AddRowDefinition(RowDefinition::Star(1));   // 内容

    // 标题
    auto title = new TextBlock();
    title->SetText("Border Layout & Alignment Demo");
    title->SetFontSize(24);
    title->SetForeground(new SolidColorBrush(Color::Black())); // Explicit black text
    title->SetHorizontalAlignment(HorizontalAlignment::Center);
    title->SetVerticalAlignment(VerticalAlignment::Center);
    
    mainGrid->AddChild(title);
    title->Row(0)->Column(0);

    // 内容区域 Grid (4x4)
    auto contentGrid = new Grid();
    contentGrid->SetMargin(Thickness(20));
    
    // 定义行
    contentGrid->AddRowDefinition(RowDefinition::Star(1));
    contentGrid->AddRowDefinition(RowDefinition::Star(1));
    contentGrid->AddRowDefinition(RowDefinition::Star(1));
    contentGrid->AddRowDefinition(RowDefinition::Star(1));

    // 定义列
    contentGrid->AddColumnDefinition(ColumnDefinition::Star(1));
    contentGrid->AddColumnDefinition(ColumnDefinition::Star(1));
    contentGrid->AddColumnDefinition(ColumnDefinition::Star(1));
    contentGrid->AddColumnDefinition(ColumnDefinition::Star(1));

    // 第一行：顶部对齐
    auto tl = CreateDemoBorder("Top Left", HorizontalAlignment::Left, VerticalAlignment::Top);
    contentGrid->AddChild(tl); tl->Row(0)->Column(0);
    
    auto tc = CreateDemoBorder("Top Center", HorizontalAlignment::Center, VerticalAlignment::Top);
    contentGrid->AddChild(tc); tc->Row(0)->Column(1);
    
    auto tr = CreateDemoBorder("Top Right", HorizontalAlignment::Right, VerticalAlignment::Top);
    contentGrid->AddChild(tr); tr->Row(0)->Column(2);
    
    auto ts = CreateStretchBorder("Stretch / Top");
    contentGrid->AddChild(ts); ts->Row(0)->Column(3);
    
    // 第二行：居中对齐
    auto cl = CreateDemoBorder("Center Left", HorizontalAlignment::Left, VerticalAlignment::Center);
    contentGrid->AddChild(cl); cl->Row(1)->Column(0);
    
    auto cc = CreateDemoBorder("Center Center", HorizontalAlignment::Center, VerticalAlignment::Center);
    contentGrid->AddChild(cc); cc->Row(1)->Column(1);
    
    auto cr = CreateDemoBorder("Center Right", HorizontalAlignment::Right, VerticalAlignment::Center);
    contentGrid->AddChild(cr); cr->Row(1)->Column(2);
    
    // 第三行：底部对齐
    auto bl = CreateDemoBorder("Bottom Left", HorizontalAlignment::Left, VerticalAlignment::Bottom);
    contentGrid->AddChild(bl); bl->Row(2)->Column(0);
    
    auto bc = CreateDemoBorder("Bottom Center", HorizontalAlignment::Center, VerticalAlignment::Bottom);
    contentGrid->AddChild(bc); bc->Row(2)->Column(1);
    
    auto br = CreateDemoBorder("Bottom Right", HorizontalAlignment::Right, VerticalAlignment::Bottom);
    contentGrid->AddChild(br); br->Row(2)->Column(2);

    // 第四行：特殊情况
    auto fs = CreateStretchBorder("Full Stretch");
    contentGrid->AddChild(fs); fs->Row(3)->Column(0);
    
    // 混合 Stretch
    auto vStretch = CreateDemoBorder("V Stretch", HorizontalAlignment::Center, VerticalAlignment::Stretch);
    contentGrid->AddChild(vStretch); vStretch->Row(3)->Column(1);

    auto hStretch = CreateDemoBorder("H Stretch", HorizontalAlignment::Stretch, VerticalAlignment::Center);
    contentGrid->AddChild(hStretch); hStretch->Row(3)->Column(2);

    // 尺寸限制测试
    auto constrained = CreateConstrainedBorder();
    contentGrid->AddChild(constrained); constrained->Row(3)->Column(3);

    mainGrid->AddChild(contentGrid);
    contentGrid->Row(1)->Column(0);

    window->Content(mainGrid);
    window->Show();
    app->Run(window);

    return 0;
}