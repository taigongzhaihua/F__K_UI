#pragma once

#include "fk/ui/Panel.h"
#include "fk/ui/View.h"

namespace fk::ui {

enum class Orientation {
    Horizontal,
    Vertical
};

class StackPanel : public View<StackPanel, Panel<StackPanel>> {
public:
    using View::View;

    StackPanel();
    ~StackPanel() override;

    // 依赖属性声明
    static const binding::DependencyProperty& OrientationProperty();
    static const binding::DependencyProperty& SpacingProperty();

    // Getter: 获取方向 (无参重载,从依赖属性读取)
    [[nodiscard]] ui::Orientation Orientation() const;
    
    // Setter: 设置方向 (有参重载,写入依赖属性,返回Ptr支持链式调用)
    std::shared_ptr<StackPanel> Orientation(ui::Orientation orientation);

    // Getter: 获取间距
    [[nodiscard]] float Spacing() const;
    
    // Setter: 设置间距 (子元素之间的间距)
    std::shared_ptr<StackPanel> Spacing(float spacing);
    
    // 流式API: 添加子元素 (包装 Panel::AddChild)
    std::shared_ptr<StackPanel> AddChild(std::shared_ptr<UIElement> child) {
        this->PanelBase::AddChild(std::move(child));
        return this->Self();
    }
    
    // Getter: 获取所有子元素 (返回 span)
    [[nodiscard]] std::span<const std::shared_ptr<UIElement>> Children() const noexcept {
        return this->PanelBase::GetChildren();
    }
    
    // 流式API: 批量添加子元素 (接受初始化列表)
    std::shared_ptr<StackPanel> Children(std::initializer_list<std::shared_ptr<UIElement>> children) {
        for (auto& child : children) {
            this->PanelBase::AddChild(child);
        }
        return this->Self();
    }
    
    // 流式API: 批量添加子元素 (接受 vector)
    std::shared_ptr<StackPanel> Children(const std::vector<std::shared_ptr<UIElement>>& children) {
        for (const auto& child : children) {
            this->PanelBase::AddChild(child);
        }
        return this->Self();
    }
    
    // 流式API: 移除子元素 (包装 Panel::RemoveChild)
    std::shared_ptr<StackPanel> RemoveChild(UIElement* child) {
        this->PanelBase::RemoveChild(child);
        return this->Self();
    }
    
    // 流式API: 清空子元素 (包装 Panel::ClearChildren)
    std::shared_ptr<StackPanel> ClearChildren() {
        this->PanelBase::ClearChildren();
        return this->Self();
    }

protected:
    Size MeasureOverride(const Size& availableSize) override;
    Size ArrangeOverride(const Size& finalSize) override;

private:
    // 依赖属性元数据构建
    static binding::PropertyMetadata BuildOrientationMetadata();
    static binding::PropertyMetadata BuildSpacingMetadata();
    
    // 依赖属性变更回调
    static void OrientationPropertyChanged(
        binding::DependencyObject& sender,
        const binding::DependencyProperty& property,
        const std::any& oldValue,
        const std::any& newValue
    );
    
    static void SpacingPropertyChanged(
        binding::DependencyObject& sender,
        const binding::DependencyProperty& property,
        const std::any& oldValue,
        const std::any& newValue
    );
};
inline std::shared_ptr<StackPanel> stackPanel() {
    return std::make_shared<StackPanel>();
}
} // namespace fk::ui
