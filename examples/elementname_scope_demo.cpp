/**
 * ElementName 作用域解析演示
 * 展示 ElementName 绑定如何通过向上遍历查找源元素
 */

#include "fk/app/Application.h"
#include "fk/ui/Window.h"
#include "fk/ui/TextBlock.h"
#include "fk/ui/StackPanel.h"
#include "fk/ui/Button.h"
#include "fk/ui/Border.h"
#include "fk/ui/Brush.h"
#include "fk/binding/Binding.h"
#include <iostream>
#include <memory>

using namespace fk;

int main() {
    std::cout << "╔═══════════════════════════════════════════════════════════╗\n";
    std::cout << "║         ElementName 作用域解析演示                       ║\n";
    std::cout << "║         展示向上遍历的查找机制                           ║\n";
    std::cout << "╚═══════════════════════════════════════════════════════════╝\n\n";

    try {
        auto app = std::make_shared<Application>();
        std::cout << "✓ Application 创建成功\n";

        auto window = app->CreateWindow();
        window->Title("ElementName 作用域解析")
              ->Width(700)
              ->Height(600);
        
        // 创建主容器
        auto* mainPanel = new ui::StackPanel();
        mainPanel->Name("mainPanel");
        
        // 标题
        auto* title = new ui::TextBlock();
        title->Text("ElementName 绑定的作用域解析");
        title->FontSize(24.0f);
        title->FontWeight(ui::FontWeight::Bold);
        title->TextAlignment(ui::TextAlignment::Center);
        title->Foreground(new ui::SolidColorBrush(ui::Color(0, 120, 215, 255)));
        mainPanel->AddChild(title);
        
        // 说明
        auto* desc = new ui::TextBlock();
        desc->Text("ElementName绑定使用向上遍历：从目标元素开始，\n沿逻辑树向上，在每个祖先中查找源元素");
        desc->FontSize(14.0f);
        desc->TextAlignment(ui::TextAlignment::Center);
        desc->Margin(0, 10, 0, 20);
        mainPanel->AddChild(desc);
        
        // ========== 示例1：兄弟元素绑定 ==========
        auto* example1 = new ui::Border();
        example1->BorderBrush(new ui::SolidColorBrush(ui::Color(0, 150, 0, 255)));
        example1->BorderThickness(2);
        example1->Padding(10);
        example1->Margin(10);
        
        auto* panel1 = new ui::StackPanel();
        
        auto* label1 = new ui::TextBlock();
        label1->Text("示例1：兄弟元素绑定");
        label1->FontSize(16.0f);
        label1->FontWeight(ui::FontWeight::Bold);
        panel1->AddChild(label1);
        
        auto* explanation1 = new ui::TextBlock();
        explanation1->Text("源按钮和目标文本是兄弟关系，通过共同父元素找到");
        explanation1->FontSize(12.0f);
        explanation1->Margin(0, 5, 0, 10);
        panel1->AddChild(explanation1);
        
        // 源按钮
        auto* sourceBtn1 = new ui::Button();
        sourceBtn1->Name("sourceButton1");
        sourceBtn1->Content("源按钮 (兄弟)");
        sourceBtn1->Width(150);
        sourceBtn1->Height(30);
        panel1->AddChild(sourceBtn1);
        
        // 目标文本（绑定到源按钮）
        auto* targetText1 = new ui::TextBlock();
        targetText1->Text("绑定结果: ");
        targetText1->FontSize(14.0f);
        targetText1->Margin(0, 10, 0, 0);
        
        // 创建ElementName绑定
        binding::Binding binding1;
        binding1.ElementName("sourceButton1").Path("Content");
        targetText1->SetBinding(ui::TextBlock::TextProperty(), std::move(binding1));
        panel1->AddChild(targetText1);
        
        example1->Child(panel1);
        mainPanel->AddChild(example1);
        
        // ========== 示例2：跨层级绑定 ==========
        auto* example2 = new ui::Border();
        example2->BorderBrush(new ui::SolidColorBrush(ui::Color(200, 100, 0, 255)));
        example2->BorderThickness(2);
        example2->Padding(10);
        example2->Margin(10);
        
        auto* panel2 = new ui::StackPanel();
        
        auto* label2 = new ui::TextBlock();
        label2->Text("示例2：跨层级绑定（叔伯关系）");
        label2->FontSize(16.0f);
        label2->FontWeight(ui::FontWeight::Bold);
        panel2->AddChild(label2);
        
        auto* explanation2 = new ui::TextBlock();
        explanation2->Text("源在左侧容器，目标在右侧容器，通过共同祖先找到");
        explanation2->FontSize(12.0f);
        explanation2->Margin(0, 5, 0, 10);
        panel2->AddChild(explanation2);
        
        // 左侧容器（包含源）
        auto* leftPanel = new ui::StackPanel();
        auto* sourceBtn2 = new ui::Button();
        sourceBtn2->Name("sharedSource");
        sourceBtn2->Content("共享源按钮");
        sourceBtn2->Width(150);
        sourceBtn2->Height(30);
        leftPanel->AddChild(sourceBtn2);
        panel2->AddChild(leftPanel);
        
        // 右侧容器（包含目标）
        auto* rightPanel = new ui::StackPanel();
        auto* targetText2 = new ui::TextBlock();
        targetText2->Text("绑定结果: ");
        targetText2->FontSize(14.0f);
        targetText2->Margin(0, 10, 0, 0);
        
        binding::Binding binding2;
        binding2.ElementName("sharedSource").Path("Content");
        targetText2->SetBinding(ui::TextBlock::TextProperty(), std::move(binding2));
        rightPanel->AddChild(targetText2);
        panel2->AddChild(rightPanel);
        
        example2->Child(panel2);
        mainPanel->AddChild(example2);
        
        // ========== 示例3：向上查找祖先 ==========
        auto* example3 = new ui::Border();
        example3->BorderBrush(new ui::SolidColorBrush(ui::Color(150, 0, 150, 255)));
        example3->BorderThickness(2);
        example3->Padding(10);
        example3->Margin(10);
        
        auto* panel3 = new ui::StackPanel();
        panel3->Name("ancestorPanel");
        
        auto* label3 = new ui::TextBlock();
        label3->Text("示例3：绑定到祖先元素");
        label3->FontSize(16.0f);
        label3->FontWeight(ui::FontWeight::Bold);
        panel3->AddChild(label3);
        
        auto* explanation3 = new ui::TextBlock();
        explanation3->Text("目标元素绑定到其祖先元素的Name属性");
        explanation3->FontSize(12.0f);
        explanation3->Margin(0, 5, 0, 10);
        panel3->AddChild(explanation3);
        
        // 嵌套容器
        auto* nestedPanel = new ui::StackPanel();
        auto* deepPanel = new ui::StackPanel();
        
        auto* targetText3 = new ui::TextBlock();
        targetText3->Text("祖先名称: ");
        targetText3->FontSize(14.0f);
        
        // 绑定到祖先
        binding::Binding binding3;
        binding3.ElementName("ancestorPanel").Path("Name");
        targetText3->SetBinding(ui::TextBlock::TextProperty(), std::move(binding3));
        deepPanel->AddChild(targetText3);
        
        nestedPanel->AddChild(deepPanel);
        panel3->AddChild(nestedPanel);
        
        example3->Child(panel3);
        mainPanel->AddChild(example3);
        
        window->Content(mainPanel);
        
        std::cout << "✓ UI 结构创建成功\n\n";
        
        // ========== 说明解析过程 ==========
        
        std::cout << "ElementName 绑定的解析过程：\n";
        std::cout << "========================================\n\n";
        
        std::cout << "示例1（兄弟元素）：\n";
        std::cout << "  树结构: panel1 → sourceBtn1, targetText1\n";
        std::cout << "  解析: targetText1 → 向上到 panel1\n";
        std::cout << "       → panel1.FindElementByName(\"sourceButton1\")\n";
        std::cout << "       → 找到 sourceBtn1 ✓\n\n";
        
        std::cout << "示例2（叔伯关系）：\n";
        std::cout << "  树结构: panel2 → leftPanel → sourceBtn2\n";
        std::cout << "                 → rightPanel → targetText2\n";
        std::cout << "  解析: targetText2 → 向上到 rightPanel → 没找到\n";
        std::cout << "       → 向上到 panel2\n";
        std::cout << "       → panel2.FindElementByName(\"sharedSource\")\n";
        std::cout << "       → 搜索整个子树，找到 sourceBtn2 ✓\n\n";
        
        std::cout << "示例3（祖先元素）：\n";
        std::cout << "  树结构: panel3 (name=\"ancestorPanel\")\n";
        std::cout << "           → nestedPanel → deepPanel → targetText3\n";
        std::cout << "  解析: targetText3 → 向上到 deepPanel → 没找到\n";
        std::cout << "       → 向上到 nestedPanel → 没找到\n";
        std::cout << "       → 向上到 panel3\n";
        std::cout << "       → panel3.FindElementByName(\"ancestorPanel\")\n";
        std::cout << "       → panel3 自己的名称匹配 ✓\n\n";
        
        std::cout << "========================================\n";
        std::cout << "关键特性：\n";
        std::cout << "  1. 从目标元素开始向上遍历\n";
        std::cout << "  2. 在每个祖先中调用 FindElementByName()\n";
        std::cout << "  3. FindElementByName() 会搜索该祖先的整个子树\n";
        std::cout << "  4. 找到第一个匹配就返回\n";
        std::cout << "  5. 可以找到：兄弟、叔伯、祖先等元素\n\n";
        
        std::cout << "与 FindName 的区别：\n";
        std::cout << "  FindName:     从元素向下搜索子树\n";
        std::cout << "  ElementName:  从元素向上遍历，在每个祖先中向下搜索\n\n";
        
        window->Show();
        std::cout << "✓ 窗口已显示\n";

        // 运行3秒
        std::cout << "\n运行窗口 3 秒...\n";
        int frameCount = 0;
        auto startTime = std::chrono::steady_clock::now();
        
        while (window->ProcessEvents()) {
            window->RenderFrame();
            frameCount++;
            
            auto now = std::chrono::steady_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(
                now - startTime
            ).count();
            
            if (elapsed >= 3) {
                window->Close();
                break;
            }
        }
        
        std::cout << "\n╔═══════════════════════════════════════════════════════════╗\n";
        std::cout << "║                   演示完成！                              ║\n";
        std::cout << "╚═══════════════════════════════════════════════════════════╝\n";
        std::cout << "\n✅ ElementName 绑定使用向上遍历机制\n";
        std::cout << "✅ 可以找到兄弟、叔伯、祖先等相关元素\n";
        std::cout << "✅ 通过逻辑树结构自然实现作用域隔离\n";

    } catch (const std::exception& e) {
        std::cerr << "\n✗ 错误: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
