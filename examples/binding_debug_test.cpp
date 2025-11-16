/**
 * @file binding_debug_test.cpp
 * @brief 调试绑定系统
 */

#include "fk/ui/Window.h"
#include "fk/binding/Binding.h"
#include "fk/binding/ObservableObject.h"
#include "fk/binding/ViewModelMacros.h"
#include <iostream>
#include <string>
#include <memory>

using namespace fk;
using namespace fk::ui;
using namespace fk::binding;

class TestViewModel : public ObservableObject {
public:
    const std::string& GetWindowTitle() const { return windowTitle_; }
    void SetWindowTitle(const std::string& value) {
        if (windowTitle_ != value) {
            std::cout << "[ViewModel] SetWindowTitle: \"" << windowTitle_ 
                      << "\" -> \"" << value << "\"\n";
            windowTitle_ = value;
            RaisePropertyChanged(std::string("WindowTitle"));
            std::cout << "[ViewModel] PropertyChanged event raised for 'WindowTitle'\n";
        }
    }

private:
    std::string windowTitle_{"Initial Title"};
};

// 注册 ViewModel 属性
FK_VIEWMODEL_AUTO(TestViewModel, WindowTitle)

int main() {
    std::cout << "=== Binding Debug Test ===\n\n";

    // 创建 ViewModel (使用 shared_ptr 以便正确管理生命周期)
    auto viewModel = std::make_shared<TestViewModel>();
    std::cout << "[1] Created ViewModel with WindowTitle = \"" << viewModel->GetWindowTitle() << "\"\n\n";

    // 创建 Window
    auto* window = new Window();
    std::cout << "[2] Created Window\n";
    std::cout << "    Initial Window.Title = \"" << window->GetTitle() << "\"\n\n";

    // 设置 DataContext (传递 shared_ptr)
    window->SetDataContext(viewModel);
    std::cout << "[3] Set Window.DataContext to ViewModel (as shared_ptr)\n";
    std::cout << "    DataContext has value: " << (window->GetDataContext().has_value() ? "Yes" : "No") << "\n\n";

    // 创建绑定
    std::cout << "[4] Creating binding with path 'WindowTitle'\n";
    Binding titleBinding;
    titleBinding.Path("WindowTitle");
    std::cout << "    Binding.Path = \"" << titleBinding.GetPath() << "\"\n";
    std::cout << "    Binding.HasExplicitSource = " << (titleBinding.HasExplicitSource() ? "Yes" : "No") << "\n\n";

    // 设置绑定
    std::cout << "[5] Setting binding on Window.TitleProperty()\n";
    window->SetBinding(Window::TitleProperty(), titleBinding);
    std::cout << "    Binding set\n\n";

    // 检查绑定后的值
    std::cout << "[6] Checking Window.Title after binding\n";
    std::cout << "    Window.Title = \"" << window->GetTitle() << "\"\n";
    std::cout << "    Expected: \"" << viewModel->GetWindowTitle() << "\"\n";
    std::cout << "    Match: " << (window->GetTitle() == viewModel->GetWindowTitle() ? "✓" : "✗") << "\n\n";

    // 修改 ViewModel
    std::cout << "[7] Updating ViewModel.WindowTitle\n";
    viewModel->SetWindowTitle("Updated Title");
    std::cout << "\n";

    // 检查 Window 是否更新
    std::cout << "[8] Checking Window.Title after ViewModel update\n";
    std::cout << "    Window.Title = \"" << window->GetTitle() << "\"\n";
    std::cout << "    Expected: \"" << viewModel->GetWindowTitle() << "\"\n";
    std::cout << "    Match: " << (window->GetTitle() == viewModel->GetWindowTitle() ? "✓" : "✗") << "\n\n";

    // 检查绑定表达式
    auto binding = window->GetBinding(Window::TitleProperty());
    std::cout << "[9] Binding Expression Info\n";
    std::cout << "    HasBinding: " << (binding ? "Yes" : "No") << "\n";
    if (binding) {
        std::cout << "    IsActive: " << (binding->IsActive() ? "Yes" : "No") << "\n";
        std::cout << "    Definition.Path: \"" << binding->Definition().GetPath() << "\"\n";
    }
    std::cout << "\n";

    std::cout << "=== Test Complete ===\n";

    delete window;
    return 0;
}
