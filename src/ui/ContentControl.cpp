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
    
    // 递归搜索视觉子元素（而非逻辑子元素）
    // 在模板中，ContentPresenter 是视觉树的一部分
    size_t childCount = root->GetVisualChildrenCount();
    for (size_t i = 0; i < childCount; ++i) {
        auto* visualChild = root->GetVisualChild(i);
        if (visualChild) {
            auto* uiChild = dynamic_cast<UIElement*>(visualChild);
            if (uiChild) {
                auto* found = FindContentPresenter<T>(uiChild);
                if (found) {
                    return found;
                }
            }
        }
    }
    
    return nullptr;
}

template<typename Derived>
void ContentControl<Derived>::OnRender(render::RenderContext& context) {
    // ContentControl 在 OnRender 中的职责：
    // 
    // 根据用户需求："ContentControl 应该在 OnRender 中先将模板实例化得到一个 UIElement，
    // 然后调用 UIElement 的 OnRender，父元素需要通过这种方式向下传递 rendercontext"
    //
    // 理解：在渲染时确保模板已经应用。如果模板尚未应用，先应用它。
    // 然后子元素的渲染会由 UIElement::CollectDrawCommands 中的递归机制自动完成。
    
    // 确保模板已应用（如果有模板但还没有实例化）
    auto* tmpl = this->GetTemplate();
    UIElement* templateRoot = this->GetTemplateRoot();
    
    if (tmpl && tmpl->IsValid() && !templateRoot) {
        // 有模板但还没有实例化：立即应用模板
        // 这确保了在第一次渲染时模板会被正确设置
        this->ApplyTemplate();
        templateRoot = this->GetTemplateRoot();
    }
    
    // ContentControl 本身不绘制任何内容
    // 子元素（模板根或内容元素）的渲染会由 UIElement::CollectDrawCommands 
    // 中的 Visual::CollectDrawCommands(context) 自动完成
    // 
    // 这保证了：
    // 1. 模板根或内容元素的 CollectDrawCommands 会被调用
    // 2. 它们的 OnRender 会在正确的坐标系下被调用
    // 3. RenderContext 被正确传递到整个视觉树
    
    (void)context; // ContentControl 本身不绘制，避免未使用参数警告
}

} // namespace fk::ui

// 显式实例化需要的额外头文件
#include "fk/ui/ToggleButton.h"

// 显式实例化 ContentControl 模板（必须在命名空间之外）
template class fk::ui::ContentControl<fk::ui::Button>;
template class fk::ui::ContentControl<fk::ui::Window>;
template class fk::ui::ContentControl<fk::ui::ToggleButton>;
