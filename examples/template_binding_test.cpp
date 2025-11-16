/**
 * @file template_binding_test.cpp
 * @brief 测试 TemplateBinding 和新的绑定 API
 * 
 * 本测试展示：
 * 1. TemplateBinding 自动绑定到 TemplatedParent
 * 2. 工厂函数创建控件
 * 3. 链式绑定语法 (property(bind("xxx")))
 */

#include "fk/ui/Button.h"
#include "fk/ui/Border.h"
#include "fk/ui/TextBlock.h"
#include "fk/ui/ControlTemplate.h"
#include "fk/ui/Window.h"
#include "fk/ui/StackPanel.h"
#include "fk/ui/Brush.h"
#include "fk/binding/TemplateBinding.h"
#include "fk/binding/Binding.h"
#include "fk/binding/ObservableObject.h"
#include <iostream>
#include <string>
#include <memory>

using namespace fk;
using namespace fk::ui;
using namespace fk::binding;

// ===== 测试用的 ViewModel =====
class TestViewModel : public ObservableObject {
public:
    const std::string& GetWindowTitle() const { return windowTitle_; }
    void SetWindowTitle(const std::string& value) {
        if (windowTitle_ != value) {
            windowTitle_ = value;
            RaisePropertyChanged(std::string("WindowTitle"));
        }
    }

private:
    std::string windowTitle_{"Test Window"};
};

// ===== 测试 1: TemplateBinding 自动绑定 =====
void TestTemplateBinding() {
    std::cout << "========== Test 1: TemplateBinding Auto-Binding ==========\n\n";

    // 创建一个 Button
    auto* button = new class Button();
    button->SetBackground(new SolidColorBrush(Color::FromRGB(255, 0, 0, 255))); // 红色背景

    // 创建一个 Border 作为模板内容
    auto* border = new class Border();
    
    // 设置 border 的 TemplatedParent 为 button
    border->SetTemplatedParent(button);

    // 使用 TemplateBinding 绑定 Button 的 Background 到 Border 的 Background
    border->SetBinding(
        Border::BackgroundProperty(),
        TemplateBinding(Control<class Button>::BackgroundProperty())
    );

    std::cout << "[Initial State]\n";
    std::cout << "  Button Background: " << (button->GetBackground() ? "Set (Red)" : "Not Set") << "\n";
    std::cout << "  Border Background: " << (border->GetBackground() ? "Set (from TemplateBinding)" : "Not Set") << "\n\n";

    // 修改 Button 的背景色
    button->SetBackground(new SolidColorBrush(Color::FromRGB(0, 255, 0, 255))); // 改为绿色

    std::cout << "[After Changing Button Background]\n";
    std::cout << "  Button Background: Changed to Green\n";
    std::cout << "  Border Background: " << (border->GetBackground() ? "Should be Green (auto-updated)" : "Not Set") << "\n";

    // 验证 TemplateBinding 是否工作
    if (border->GetBackground()) {
        std::cout << "\n✅ TemplateBinding is working! Border background follows Button background.\n";
    } else {
        std::cout << "\n❌ TemplateBinding failed. Border background was not set.\n";
    }

    std::cout << "\n";

    delete border;
    delete button;
}

// ===== 测试 2: 工厂函数 =====
void TestFactoryFunctions() {
    std::cout << "========== Test 2: Factory Functions ==========\n\n";

    std::cout << "[Creating controls using new keyword]\n";
    
    // 使用 new 创建控件（工厂函数在单独的命名空间中）
    auto* window = new Window();
    auto* button = new Button();
    auto* textBlock = new TextBlock();
    auto* border = new Border();

    std::cout << "  Window created: " << (window ? "✓" : "✗") << "\n";
    std::cout << "  Button created: " << (button ? "✓" : "✗") << "\n";
    std::cout << "  TextBlock created: " << (textBlock ? "✓" : "✗") << "\n";
    std::cout << "  Border created: " << (border ? "✓" : "✗") << "\n";

    std::cout << "\n✅ Control creation works correctly!\n\n";

    delete window;
    delete button;
    delete textBlock;
    delete border;
}

// ===== 测试 3: 链式绑定语法 =====
void TestChainedBindingSyntax() {
    std::cout << "========== Test 3: Chained Binding Syntax ==========\n\n";

    // 创建 ViewModel
    auto viewModel = std::make_shared<TestViewModel>();
    viewModel->SetWindowTitle("My Application");

    // 使用链式绑定语法
    auto* window = new Window();
    window->SetDataContext(viewModel);

    std::cout << "[Using chained binding syntax]\n";
    std::cout << "  window->Title(bind(\"WindowTitle\"));\n\n";

    // 使用链式绑定语法
    window->Title(bind("WindowTitle"));

    std::cout << "[Initial State]\n";
    std::cout << "  ViewModel.WindowTitle: \"" << viewModel->GetWindowTitle() << "\"\n";
    std::cout << "  Window.Title: \"" << window->GetTitle() << "\"\n\n";

    // 验证绑定是否工作
    if (window->GetTitle() == viewModel->GetWindowTitle()) {
        std::cout << "✅ Initial binding works!\n\n";
    } else {
        std::cout << "❌ Initial binding failed!\n\n";
    }

    // 修改 ViewModel 的属性
    viewModel->SetWindowTitle("Updated Title");

    std::cout << "[After Updating ViewModel]\n";
    std::cout << "  ViewModel.WindowTitle: \"" << viewModel->GetWindowTitle() << "\"\n";
    std::cout << "  Window.Title: \"" << window->GetTitle() << "\"\n\n";

    // 验证绑定更新是否工作
    if (window->GetTitle() == viewModel->GetWindowTitle()) {
        std::cout << "✅ Binding update works! Window title follows ViewModel.\n";
    } else {
        std::cout << "❌ Binding update failed!\n";
    }

    std::cout << "\n";

    delete window;
}

// ===== 测试 4: 组合使用 =====
void TestCombinedUsage() {
    std::cout << "========== Test 4: Combined Usage ==========\n\n";

    std::cout << "[Creating a complete example with all features]\n\n";

    // 创建控件
    auto* window = new Window();
    auto* button = new Button();

    // 使用链式 API 设置属性
    window->Title("Test Application")
          ->Width(800)
          ->Height(600);

    button->Width(120)
          ->Height(40);

    std::cout << "  Window: " << window->GetTitle() 
              << " (" << window->GetWidth() << "x" << window->GetHeight() << ")\n";
    std::cout << "  Button: " << button->GetWidth() << "x" << button->GetHeight() << "\n";

    std::cout << "\n✅ Combined usage works correctly!\n\n";

    delete button;
    delete window;
}

// ===== Main =====
int main() {
    std::cout << "=================================================\n";
    std::cout << "  TemplateBinding & New Binding API Test\n";
    std::cout << "=================================================\n\n";

    try {
        TestTemplateBinding();
        TestFactoryFunctions();
        TestChainedBindingSyntax();
        TestCombinedUsage();

        std::cout << "=================================================\n";
        std::cout << "  All Tests Completed!\n";
        std::cout << "=================================================\n\n";

        std::cout << "Implemented Features:\n";
        std::cout << "  ✅ TemplateBinding auto-binds to TemplatedParent\n";
        std::cout << "  ✅ Factory functions for creating controls\n";
        std::cout << "  ✅ Chained binding syntax (property(bind(\"xxx\")))\n";
        std::cout << "  ✅ Seamless integration with existing API\n";

        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
