#pragma once

#include "fk/ui/Panel.h"
#include "fk/ui/View.h"

namespace fk::ui {

enum class Orientation {
    Horizontal,
    Vertical
};

class StackPanel : public View<StackPanel, Panel> {
public:
    using View::View;

    StackPanel();
    ~StackPanel() override;

    // 依赖属性声明
    static const binding::DependencyProperty& OrientationProperty();

    // Getter: 获取方向 (无参重载,从依赖属性读取)
    [[nodiscard]] ui::Orientation Orientation() const;
    
    // Setter: 设置方向 (有参重载,写入依赖属性,返回Ptr支持链式调用)
    std::shared_ptr<StackPanel> Orientation(ui::Orientation orientation);
    
    // 流式API: 添加子元素 (包装 Panel::AddChild)
    std::shared_ptr<StackPanel> AddChild(std::shared_ptr<UIElement> child) {
        Panel::AddChild(std::move(child));
        return Self();
    }
    
    // 流式API: 移除子元素 (包装 Panel::RemoveChild)
    std::shared_ptr<StackPanel> RemoveChild(UIElement* child) {
        Panel::RemoveChild(child);
        return Self();
    }
    
    // 流式API: 清空子元素 (包装 Panel::ClearChildren)
    std::shared_ptr<StackPanel> ClearChildren() {
        Panel::ClearChildren();
        return Self();
    }

protected:
    Size MeasureOverride(const Size& availableSize) override;
    Size ArrangeOverride(const Size& finalSize) override;

private:
    // 依赖属性元数据构建
    static binding::PropertyMetadata BuildOrientationMetadata();
    
    // 依赖属性变更回调
    static void OrientationPropertyChanged(
        binding::DependencyObject& sender,
        const binding::DependencyProperty& property,
        const std::any& oldValue,
        const std::any& newValue
    );
};

} // namespace fk::ui
