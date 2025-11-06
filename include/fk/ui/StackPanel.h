#pragma once

#include "fk/ui/Panel.h"
#include "fk/ui/View.h"
#include "fk/ui/Enums.h"
#include "fk/ui/BindingMacros.h"
#include "fk/ui/DependencyPropertyMacros.h"

namespace fk::ui {

class StackPanel : public View<StackPanel, Panel<StackPanel>> {
public:
    using View::View;

    StackPanel();
    ~StackPanel() override;

    // 依赖属性（使用宏）
    FK_DEPENDENCY_PROPERTY_DECLARE(Orientation, ui::Orientation)
    FK_DEPENDENCY_PROPERTY_DECLARE(Spacing, float)

public:
    // 流式 API 重载（返回 shared_ptr）
    std::shared_ptr<StackPanel> Orientation(ui::Orientation orientation) {
        SetOrientation(orientation);
        return this->Self();
    }
    
    std::shared_ptr<StackPanel> Orientation(binding::Binding binding) {
        SetBinding(OrientationProperty(), std::move(binding));
        return this->Self();
    }

    std::shared_ptr<StackPanel> Spacing(float spacing) {
        SetSpacing(spacing);
        return this->Self();
    }
    
    std::shared_ptr<StackPanel> Spacing(binding::Binding binding) {
        SetBinding(SpacingProperty(), std::move(binding));
        return this->Self();
    }
    
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
};
inline std::shared_ptr<StackPanel> stackPanel() {
    return std::make_shared<StackPanel>();
}
} // namespace fk::ui
