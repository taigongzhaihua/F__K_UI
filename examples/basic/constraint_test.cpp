#include <fk/app/Application.h>
#include <fk/ui/Window.h>
#include <fk/ui/Grid.h>
#include <fk/ui/CheckBox.h>
#include <fk/ui/RadioButton.h>
#include <fk/ui/StackPanel.h>
#include <fk/ui/Border.h>
#include <fk/ui/Brush.h>

using namespace fk;
using namespace fk::ui;

int main() {
    Application app;

    auto window = std::make_shared<Window>();
    window->SetTitle("CheckBox/RadioButton Size Constraint Test");
    window->SetBackground(new SolidColorBrush(240, 240, 240, 255));

    auto mainPanel = std::make_shared<StackPanel>();
    mainPanel->SetOrient(Orientation::Vertical);
    mainPanel->SetSpacing(20);
    mainPanel->SetMargin(Thickness(20));

    // 测试1：StackPanel宽度300，CheckBox没有显式Width
    auto testPanel1 = std::make_shared<StackPanel>();
    testPanel1->SetOrient(Orientation::Vertical);
    testPanel1->SetSpacing(10);
    testPanel1->SetBackground(new SolidColorBrush(200, 220, 255, 255));
    testPanel1->SetWidth(300);  // 父元素限制宽度
    testPanel1->SetPadding(Thickness(10));

    auto cb1 = std::make_shared<CheckBox>();
    cb1->SetContent(std::string("This is a very long checkbox text that should wrap or be constrained by parent width"));
    // 注意：没有设置Width，应该受父元素300宽度限制
    
    auto cb2 = std::make_shared<CheckBox>();
    cb2->SetContent(std::string("Short text"));
    
    testPanel1->AddChild(cb1.get());
    testPanel1->AddChild(cb2.get());

    // 测试2：StackPanel宽度200，RadioButton没有显式Width
    auto testPanel2 = std::make_shared<StackPanel>();
    testPanel2->SetOrient(Orientation::Vertical);
    testPanel2->SetSpacing(10);
    testPanel2->SetBackground(new SolidColorBrush(255, 220, 200, 255));
    testPanel2->SetWidth(200);  // 更小的父元素宽度
    testPanel2->SetPadding(Thickness(10));

    auto rb1 = std::make_shared<RadioButton>();
    rb1->SetContent(std::string("This is a very long radio button text that should be constrained"));
    
    auto rb2 = std::make_shared<RadioButton>();
    rb2->SetContent(std::string("Short"));
    
    testPanel2->AddChild(rb1.get());
    testPanel2->AddChild(rb2.get());

    // 测试3：CheckBox有显式Width（应该忽略父元素约束）
    auto testPanel3 = std::make_shared<StackPanel>();
    testPanel3->SetOrient(Orientation::Vertical);
    testPanel3->SetSpacing(10);
    testPanel3->SetBackground(new SolidColorBrush(200, 255, 200, 255));
    testPanel3->SetWidth(200);
    testPanel3->SetPadding(Thickness(10));

    auto cb3 = std::make_shared<CheckBox>();
    cb3->SetContent(std::string("Explicit width 400"));
    cb3->SetWidth(400);  // 显式宽度，超出父元素
    
    testPanel3->AddChild(cb3.get());

    mainPanel->AddChild(testPanel1.get());
    mainPanel->AddChild(testPanel2.get());
    mainPanel->AddChild(testPanel3.get());

    window->SetContent(std::any(mainPanel.get()));
    app.Run(window);

    return 0;
}
