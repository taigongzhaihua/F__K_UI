#include "fk/ui/styling/ControlTemplate.h"
#include "fk/ui/base/UIElement.h"
#include "fk/ui/controls/Control.h"
#include "fk/animation/VisualStateGroup.h"
#include <stdexcept>
#include <iostream>

namespace fk::ui {

namespace {
    /**
     * @brief 递归克隆视觉�?
     * 
     * 注意：UIElement::Clone() 方法应该处理自身的属性克�?
     * 这里只是简单调用，子元素的克隆由各个容器类型的 Clone() 实现决定
     */
    UIElement* CloneVisualTree(UIElement* source) {
        if (!source) return nullptr;
        
        // 使用虚方法克隆（派生类应实现完整的深拷贝，包括子元素�?
        return source->Clone();
    }
    
    /**
     * @brief 递归设置视觉树中所有元素的 TemplatedParent
     * 
     * 注意：当遇到有自己模板的 Control 时，只设置该 Control �?TemplatedParent�?
     * 不递归进入其内部，因为�?Control 会在自己�?OnApplyTemplate 中正确设�?
     * 其模板元素的 TemplatedParent�?
     */
    void SetTemplatedParentRecursive(UIElement* element, UIElement* templatedParent) {
        if (!element) return;
        
        element->SetTemplatedParent(templatedParent);
        
        // 检查这个元素是否是一个有自己模板�?Control
        // 如果是，不递归进入其模板子元素，让它自己处�?
        if (element->HasOwnTemplate()) {
            // 这个 Control 有自己的模板，它会在自己�?OnApplyTemplate �?
            // 正确设置其模板元素的 TemplatedParent
            return;
        }
        
        // 递归处理所有逻辑子元�?
        for (UIElement* child : element->GetLogicalChildren()) {
            SetTemplatedParentRecursive(child, templatedParent);
        }
    }
}

ControlTemplate* ControlTemplate::SetTargetType(const std::type_info& type) {
    CheckSealed();
    targetType_ = &type;
    return this;
}

ControlTemplate* ControlTemplate::SetFactory(FactoryFunc factory) {
    CheckSealed();
    factory_ = std::move(factory);
    return this;
}

ControlTemplate* ControlTemplate::SetVisualTree(UIElement* root) {
    CheckSealed();
    visualTree_ = root;
    return this;
}

UIElement* ControlTemplate::Instantiate(UIElement* templatedParent) {
    UIElement* root = nullptr;
    
    // 优先使用工厂函数
    if (factory_) {
        root = factory_();
    } 
    // 如果没有工厂，但有视觉树定义，克隆它
    else if (visualTree_) {
        root = CloneVisualTree(visualTree_);
    }
    
    if (!root) {
        return nullptr;
    }
    
    // 设置 TemplatedParent 关联
    SetTemplatedParentRecursive(root, templatedParent);
    
    // TemplateBinding 会在下次 UpdateTarget 时自动解析到正确�?TemplatedParent
    // 不需要手动重新激活，因为 BindingExpression �?TemplateBinding 会每次重新解析源
    
    return root;
}

UIElement* ControlTemplate::FindName(const std::string& name, UIElement* root) {
    if (!root || name.empty()) {
        return nullptr;
    }
    
    return FindNameRecursive(name, root);
}

UIElement* ControlTemplate::FindNameRecursive(const std::string& name, UIElement* element) {
    if (!element) {
        return nullptr;
    }
    
    // 检查当前元素的名称
    if (element->GetName() == name) {
        return element;
    }
    
    // 递归搜索所有逻辑子元�?
    for (UIElement* child : element->GetLogicalChildren()) {
        UIElement* found = FindNameRecursive(name, child);
        if (found) {
            return found;
        }
    }
    
    return nullptr;
}

ControlTemplate* ControlTemplate::AddVisualStateGroup(std::shared_ptr<animation::VisualStateGroup> group) {
    CheckSealed();
    if (group) {
        // 先移除同名的状态组（避免重复）
        std::string groupName = group->GetName();
        visualStateGroups_.erase(
            std::remove_if(visualStateGroups_.begin(), visualStateGroups_.end(),
                [&groupName](const std::shared_ptr<animation::VisualStateGroup>& existing) {
                    return existing && existing->GetName() == groupName;
                }),
            visualStateGroups_.end()
        );
        // 然后添加新的状态组
        visualStateGroups_.push_back(group);
    }
    return this;
}

} // namespace fk::ui
