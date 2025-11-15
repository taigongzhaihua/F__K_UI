#include "fk/ui/ContentControl.h"
#include "fk/binding/DependencyProperty.h"
#include "fk/ui/Button.h"
#include "fk/ui/Window.h"
#include "fk/ui/TextBlock.h"

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

} // namespace fk::ui

// 显式实例化需要的额外头文件
#include "fk/ui/ScrollViewer.h"

// 显式实例化 ContentControl 模板（必须在命名空间之外）
template class fk::ui::ContentControl<fk::ui::Button>;
template class fk::ui::ContentControl<fk::ui::Window>;
template class fk::ui::ContentControl<fk::ui::ScrollViewer>;
