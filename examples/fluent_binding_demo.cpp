/**
 * @file fluent_binding_demo.cpp
 * @brief 演示所有依赖属性的链式绑定接口
 * 
 * 展示了所有主要控件的依赖属性现在都支持 Binding 参数，
 * 可以使用链式语法进行数据绑定
 */

#include "fk/ui/Window.h"
#include "fk/ui/Button.h"
#include "fk/ui/TextBlock.h"
#include "fk/ui/Border.h"
#include "fk/ui/StackPanel.h"
#include "fk/binding/Binding.h"
#include "fk/binding/ObservableObject.h"
#include "fk/binding/ViewModelMacros.h"
#include <iostream>
#include <memory>

using namespace fk;
using namespace fk::ui;
using namespace fk::binding;

// ViewModel 用于演示绑定
class DemoViewModel : public ObservableObject {
public:
    // 字符串属性
    const std::string& GetTitle() const { return title_; }
    void SetTitle(const std::string& value) {
        if (title_ != value) {
            title_ = value;
            RaisePropertyChanged(std::string("Title"));
        }
    }
    
    const std::string& GetText() const { return text_; }
    void SetText(const std::string& value) {
        if (text_ != value) {
            text_ = value;
            RaisePropertyChanged(std::string("Text"));
        }
    }
    
    // 数值属性
    float GetWidth() const { return width_; }
    void SetWidth(float value) {
        if (width_ != value) {
            width_ = value;
            RaisePropertyChanged(std::string("Width"));
        }
    }
    
    float GetHeight() const { return height_; }
    void SetHeight(float value) {
        if (height_ != value) {
            height_ = value;
            RaisePropertyChanged(std::string("Height"));
        }
    }
    
    float GetFontSize() const { return fontSize_; }
    void SetFontSize(float value) {
        if (fontSize_ != value) {
            fontSize_ = value;
            RaisePropertyChanged(std::string("FontSize"));
        }
    }

private:
    std::string title_{"绑定演示窗口"};
    std::string text_{"这是通过绑定设置的文本"};
    float width_{800.0f};
    float height_{600.0f};
    float fontSize_{16.0f};
};

// 注册 ViewModel 属性
FK_VIEWMODEL_AUTO(DemoViewModel, Title, Text, Width, Height, FontSize)

int main() {
    std::cout << "=== 链式绑定接口演示 ===\n\n";
    
    // 创建 ViewModel
    auto viewModel = std::make_shared<DemoViewModel>();
    
    std::cout << "[1] Window 属性绑定\n";
    auto* window = new Window();
    window->DataContext(viewModel);
    
    // Window 支持的绑定
    window->Title(bind("Title"))        // 标题绑定
          ->Width(bind("Width"))         // 宽度绑定
          ->Height(bind("Height"))       // 高度绑定
          ->Left(100.0f)                 // 位置可以直接设置
          ->Top(100.0f);
    
    std::cout << "    ✓ Window 属性已绑定\n";
    std::cout << "      - Title: 绑定到 ViewModel.Title\n";
    std::cout << "      - Width: 绑定到 ViewModel.Width\n";
    std::cout << "      - Height: 绑定到 ViewModel.Height\n";
    std::cout << "      - Left/Top: 直接设置值\n\n";
    
    std::cout << "[2] TextBlock 属性绑定\n";
    auto* textBlock = new TextBlock();
    textBlock->DataContext(viewModel);
    
    // TextBlock 支持的绑定
    textBlock->Text(bind("Text"))           // 文本内容绑定
             ->FontSize(bind("FontSize"))    // 字体大小绑定
             ->FontFamily("Arial")           // 字体可以直接设置
             ->Width(200.0f);
    
    std::cout << "    ✓ TextBlock 属性已绑定\n";
    std::cout << "      - Text: 绑定到 ViewModel.Text\n";
    std::cout << "      - FontSize: 绑定到 ViewModel.FontSize\n";
    std::cout << "      - FontFamily: 直接设置为 'Arial'\n";
    std::cout << "      - Width: 直接设置为 200\n\n";
    
    std::cout << "[3] Border 属性绑定\n";
    auto* border = new Border();
    border->DataContext(viewModel);
    
    // Border 支持的绑定（虽然这里用直接值演示）
    border->Width(bind("Width"))             // 尺寸可以绑定
          ->Height(100.0f)                   // 或直接设置
          ->BorderThickness(2.0f)
          ->CornerRadius(5.0f);
    
    std::cout << "    ✓ Border 属性已配置\n";
    std::cout << "      - Width: 绑定到 ViewModel.Width\n";
    std::cout << "      - Height: 直接设置为 100\n";
    std::cout << "      - BorderThickness: 2.0\n";
    std::cout << "      - CornerRadius: 5.0\n\n";
    
    std::cout << "[4] Button 属性绑定\n";
    auto* button = new Button();
    button->DataContext(viewModel);
    
    // Button 继承自 Control，支持所有 Control 和 FrameworkElement 的绑定
    button->Width(bind("Width"))          // 宽度绑定
          ->Height(50.0f)                 // 高度直接设置
          ->MinWidth(100.0f)              // 最小宽度
          ->MaxWidth(400.0f);             // 最大宽度
    
    std::cout << "    ✓ Button 属性已配置\n";
    std::cout << "      - Width: 绑定到 ViewModel.Width\n";
    std::cout << "      - Height: 直接设置为 50\n";
    std::cout << "      - MinWidth/MaxWidth: 设置约束\n\n";
    
    std::cout << "=== 支持绑定的属性类别 ===\n\n";
    
    std::cout << "[FrameworkElement 尺寸属性]\n";
    std::cout << "  - Width(Binding)           : 宽度绑定\n";
    std::cout << "  - Height(Binding)          : 高度绑定\n";
    std::cout << "  - MinWidth(Binding)        : 最小宽度绑定\n";
    std::cout << "  - MaxWidth(Binding)        : 最大宽度绑定\n";
    std::cout << "  - MinHeight(Binding)       : 最小高度绑定\n";
    std::cout << "  - MaxHeight(Binding)       : 最大高度绑定\n\n";
    
    std::cout << "[Control 外观属性]\n";
    std::cout << "  - Foreground(Binding)      : 前景色绑定\n";
    std::cout << "  - Background(Binding)      : 背景色绑定\n";
    std::cout << "  - BorderBrush(Binding)     : 边框画刷绑定\n\n";
    
    std::cout << "[Window 专有属性]\n";
    std::cout << "  - Title(Binding)           : 标题绑定\n";
    std::cout << "  - Left(Binding)            : 左侧位置绑定\n";
    std::cout << "  - Top(Binding)             : 顶部位置绑定\n\n";
    
    std::cout << "[Border 专有属性]\n";
    std::cout << "  - Background(Binding)      : 背景绑定\n";
    std::cout << "  - BorderBrush(Binding)     : 边框画刷绑定\n\n";
    
    std::cout << "[TextBlock 专有属性]\n";
    std::cout << "  - Text(Binding)            : 文本内容绑定\n";
    std::cout << "  - FontFamily(Binding)      : 字体绑定\n";
    std::cout << "  - FontSize(Binding)        : 字体大小绑定\n\n";
    
    std::cout << "=== 使用方式 ===\n\n";
    std::cout << "1. 链式调用：\n";
    std::cout << "   button->Width(bind(\"Width\"))->Height(50)->Background(...);\n\n";
    
    std::cout << "2. 混合绑定和直接值：\n";
    std::cout << "   element->Width(bind(\"Width\"))  // 绑定\n";
    std::cout << "          ->Height(100.0f);         // 直接设置\n\n";
    
    std::cout << "3. 所有依赖属性都支持：\n";
    std::cout << "   - PropertyName(value)      : 设置直接值\n";
    std::cout << "   - PropertyName(binding)    : 设置绑定\n";
    std::cout << "   - PropertyName()           : 获取当前值\n\n";
    
    std::cout << "=== 演示完成 ===\n";
    std::cout << "所有主要控件的依赖属性现在都支持链式绑定接口！\n";
    
    // 清理
    delete window;
    delete textBlock;
    delete border;
    delete button;
    
    return 0;
}
