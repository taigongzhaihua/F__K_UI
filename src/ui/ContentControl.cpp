#include "fk/ui/ContentControl.h"
#include "fk/binding/DependencyProperty.h"
#include "fk/ui/Button.h"
#include "fk/ui/Window.h"
#include "fk/ui/TextBlock.h"
#include "fk/ui/ContentPresenter.h"
#include "fk/ui/ControlTemplate.h"

namespace fk::ui {

// 为 ContentControl 模板类注册依赖属性

template<typename Derived>
const binding::DependencyProperty& ContentControl<Derived>::ContentProperty() {
    static auto& property = binding::DependencyProperty::Register(
        "Content",
        typeid(std::any),
        typeid(ContentControl<Derived>),
        binding::PropertyMetadata{std::any()}
    );
    return property;
}

template<typename Derived>
const binding::DependencyProperty& ContentControl<Derived>::ContentTemplateProperty() {
    static auto& property = binding::DependencyProperty::Register(
        "ContentTemplate",
        typeid(DataTemplate*),
        typeid(ContentControl<Derived>),
        binding::PropertyMetadata{static_cast<DataTemplate*>(nullptr)}
    );
    return property;
}

template<typename Derived>
UIElement* ContentControl<Derived>::CreateTextBlockFromString(const std::string& text) {
    auto* textBlock = new TextBlock();
    textBlock->Text(text);
    
    // 继承字体属性
    textBlock->FontFamily(this->GetFontFamily());
    textBlock->FontSize(this->GetFontSize());
    textBlock->FontWeight(this->GetFontWeight());
    
    // 继承前景色
    if (this->GetForeground()) {
        textBlock->Foreground(this->GetForeground());
    }
    
    return textBlock;
}

template<typename Derived>
void ContentControl<Derived>::UpdateContentPresenter() {
    // 在模板根中查找 ContentPresenter
    auto* templateRoot = this->GetTemplateRoot();
    if (!templateRoot) {
        return;
    }
    
    // 递归查找 ContentPresenter
    ContentPresenter<>* presenter = FindContentPresenter<>(templateRoot);
    if (presenter) {
        // 更新 ContentPresenter 的 Content
        auto content = GetContent();
        
        // 如果 Content 是字符串，转换为 UIElement
        if (content.has_value()) {
            if (content.type() == typeid(const char*)) {
                const char* str = std::any_cast<const char*>(content);
                auto* textBlock = CreateTextBlockFromString(str);
                presenter->SetContent(std::any(textBlock));
            } else if (content.type() == typeid(std::string)) {
                std::string str = std::any_cast<std::string>(content);
                auto* textBlock = CreateTextBlockFromString(str);
                presenter->SetContent(std::any(textBlock));
            } else {
                presenter->SetContent(content);
            }
        }
        
        // 更新 ContentTemplate
        presenter->SetContentTemplate(GetContentTemplate());
    }
}

template<typename Derived>
template<typename T>
ContentPresenter<T>* ContentControl<Derived>::FindContentPresenter(UIElement* root) {
    if (!root) {
        return nullptr;
    }
    
    // 检查当前元素是否是 ContentPresenter
    auto* presenter = dynamic_cast<ContentPresenter<T>*>(root);
    if (presenter) {
        return presenter;
    }
    
    // 递归搜索逻辑子元素
    for (UIElement* child : root->GetLogicalChildren()) {
        auto* found = FindContentPresenter<T>(child);
        if (found) {
            return found;
        }
    }
    
    return nullptr;
}

} // namespace fk::ui

// 显式实例化需要的额外头文件
#include "fk/ui/ScrollViewer.h"

// 显式实例化 ContentControl 模板（必须在命名空间之外）
template class fk::ui::ContentControl<fk::ui::Button>;
template class fk::ui::ContentControl<fk::ui::Window>;
template class fk::ui::ContentControl<fk::ui::ScrollViewer>;
