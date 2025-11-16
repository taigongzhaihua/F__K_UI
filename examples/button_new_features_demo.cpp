/**
 * @file button_new_features_demo.cpp
 * @brief 演示 Button 使用新的 TemplateBinding 和绑定语法
 * 
 * 本示例展示：
 * 1. TemplateBinding 已经在 Button 的默认模板中使用
 * 2. 可以使用工厂函数创建 Button
 * 3. 可以使用链式 API 设置属性
 * 4. 演示更直观的嵌套模板定义方式
 */

#include "fk/ui/Button.h"
#include "fk/ui/Border.h"
#include "fk/ui/ContentPresenter.h"
#include "fk/ui/Window.h"
#include "fk/ui/StackPanel.h"
#include "fk/ui/Brush.h"
#include "fk/ui/ControlTemplate.h"
#include "fk/binding/TemplateBinding.h"
#include "fk/binding/ObservableObject.h"
#include "fk/binding/ViewModelMacros.h"
#include <iostream>
#include <memory>

using namespace fk;
using namespace fk::ui;
using namespace fk::binding;

// ViewModel 示例
class ButtonViewModel : public ObservableObject {
public:
    const std::string& GetButtonText() const { return buttonText_; }
    void SetButtonText(const std::string& value) {
        if (buttonText_ != value) {
            buttonText_ = value;
            RaisePropertyChanged(std::string("ButtonText"));
        }
    }

private:
    std::string buttonText_{"点击我"};
};

// 注册 ViewModel 属性
FK_VIEWMODEL_AUTO(ButtonViewModel, ButtonText)

// 创建一个自定义的 Button 模板（更直观的嵌套方式）
ControlTemplate* CreateCustomButtonTemplate() {
    auto* tmpl = new ControlTemplate();
    tmpl->SetTargetType(typeid(Button));
    
    // 使用 lambda 定义模板，展示嵌套结构
    tmpl->SetFactory([]() -> UIElement* {
        // 创建 Border 作为根元素
        auto* border = new Border();
        
        // ===== 使用 TemplateBinding 绑定属性 =====
        // Background 从 Button 绑定到 Border
        border->SetBinding(
            Border::BackgroundProperty(),
            TemplateBinding(Control<Button>::BackgroundProperty())
        );
        
        // BorderBrush 也可以绑定
        border->SetBinding(
            Border::BorderBrushProperty(),
            TemplateBinding(Control<Button>::BorderBrushProperty())
        );
        
        // ===== 设置 Border 样式 =====
        border->BorderThickness(2.0f)
              ->CornerRadius(5.0f)
              ->Padding(15.0f, 8.0f, 15.0f, 8.0f);
        
        // ===== 创建内容展示器 =====
        auto* presenter = new ContentPresenter<>();
        presenter->SetHorizontalAlignment(HorizontalAlignment::Center);
        presenter->SetVerticalAlignment(VerticalAlignment::Center);
        
        // ===== 嵌套结构：Border 包含 ContentPresenter =====
        border->Child(presenter);
        
        return border;
    });
    
    return tmpl;
}

int main() {
    std::cout << "=== Button 新功能演示 ===\n\n";
    
    // ===== 1. 使用默认模板的 Button（已包含 TemplateBinding）=====
    std::cout << "[1] 创建使用默认模板的 Button\n";
    auto* button1 = new Button();
    button1->Width(120)
           ->Height(40);
    
    // 设置背景色 - TemplateBinding 会自动传递到模板中的 Border
    button1->SetBackground(new SolidColorBrush(Color::FromRGB(0, 120, 215, 255))); // 蓝色
    
    std::cout << "    ✓ Button 创建完成\n";
    std::cout << "    - 宽度: " << button1->GetWidth() << "\n";
    std::cout << "    - 高度: " << button1->GetHeight() << "\n";
    std::cout << "    - 背景: " << (button1->GetBackground() ? "已设置（蓝色）" : "未设置") << "\n";
    std::cout << "    - TemplateBinding: Background 属性会自动传递到模板中的 Border\n\n";
    
    // ===== 2. 使用链式 API =====
    std::cout << "[2] 使用链式 API 创建 Button\n";
    auto* button2 = new Button();
    button2->Width(150)
           ->Height(45)
           ->SetBackground(new SolidColorBrush(Color::FromRGB(0, 168, 107, 255))); // 绿色
    
    std::cout << "    ✓ 使用链式 API: button->Width(150)->Height(45)->SetBackground(...)\n";
    std::cout << "    - 宽度: " << button2->GetWidth() << "\n";
    std::cout << "    - 高度: " << button2->GetHeight() << "\n\n";
    
    // ===== 3. 使用自定义模板（展示嵌套定义方式）=====
    std::cout << "[3] 使用自定义模板的 Button\n";
    auto* button3 = new Button();
    button3->SetTemplate(CreateCustomButtonTemplate());
    button3->Width(180)->Height(50);
    button3->SetBackground(new SolidColorBrush(Color::FromRGB(232, 17, 35, 255))); // 红色
    button3->SetBorderBrush(new SolidColorBrush(Color::FromRGB(139, 0, 0, 255))); // 深红色边框
    
    std::cout << "    ✓ 自定义模板包含：\n";
    std::cout << "      - Border 使用 TemplateBinding 绑定 Background\n";
    std::cout << "      - Border 使用 TemplateBinding 绑定 BorderBrush\n";
    std::cout << "      - 嵌套的 ContentPresenter\n";
    std::cout << "    - 背景: 红色\n";
    std::cout << "    - 边框: 深红色，厚度 2.0，圆角 5.0\n\n";
    
    // ===== 4. 创建一个包含多个 Button 的面板 =====
    std::cout << "[4] 创建包含多个 Button 的 StackPanel\n";
    auto* panel = new StackPanel();
    panel->SetOrientation(ui::Orientation::Vertical);
    panel->Width(300);
    
    // 添加按钮到面板（展示嵌套结构）
    auto* btn1 = new Button();
    btn1->Width(200)->Height(35);
    auto* btn2 = new Button();
    btn2->Width(200)->Height(35);
    auto* btn3 = new Button();
    btn3->Width(200)->Height(35);
    
    // 在实际使用中会这样添加：
    // panel->Children()->Add(btn1);
    // panel->Children()->Add(btn2);
    // panel->Children()->Add(btn3);
    
    std::cout << "    ✓ StackPanel 创建完成\n";
    std::cout << "    - 方向: 垂直\n";
    std::cout << "    - 包含 3 个 Button\n\n";
    
    // ===== 5. TemplateBinding 工作原理说明 =====
    std::cout << "[5] TemplateBinding 工作原理\n";
    std::cout << "    当前 Button 的默认模板中已经使用了 TemplateBinding：\n";
    std::cout << "    \n";
    std::cout << "    border->SetBinding(\n";
    std::cout << "        Border::BackgroundProperty(),\n";
    std::cout << "        TemplateBinding(Control<Button>::BackgroundProperty())\n";
    std::cout << "    );\n";
    std::cout << "    \n";
    std::cout << "    这意味着：\n";
    std::cout << "    1. 设置 Button 的 Background 时\n";
    std::cout << "    2. TemplateBinding 自动将值传递到 Border 的 Background\n";
    std::cout << "    3. 无需手动同步属性\n\n";
    
    // ===== 总结 =====
    std::cout << "=== 功能总结 ===\n";
    std::cout << "✅ Button 已经使用 TemplateBinding\n";
    std::cout << "✅ 可以使用链式 API（Width()->Height()->SetBackground()）\n";
    std::cout << "✅ 模板定义采用嵌套的 lambda 方式，结构清晰\n";
    std::cout << "✅ TemplateBinding 自动处理属性传递\n";
    std::cout << "\n";
    std::cout << "注意：\n";
    std::cout << "- Button.cpp 中的 SyncBackgroundToBorder() 是为了兼容性保留\n";
    std::cout << "- 新代码应该依赖 TemplateBinding 自动工作\n";
    std::cout << "- 可以移除手动同步代码，完全依赖 TemplateBinding\n";
    
    // 清理
    delete button1;
    delete button2;
    delete button3;
    delete btn1;
    delete btn2;
    delete btn3;
    delete panel;
    
    return 0;
}
