#include "fk/ui/ControlTemplate.h"
#include "fk/ui/UIElement.h"
#include <stdexcept>

namespace fk::ui {

namespace {
    /**
     * @brief 递归克隆视觉树
     * 
     * 注意：UIElement::Clone() 方法应该处理自身的属性克隆
     * 这里只是简单调用，子元素的克隆由各个容器类型的 Clone() 实现决定
     */
    UIElement* CloneVisualTree(UIElement* source) {
        if (!source) return nullptr;
        
        // 使用虚方法克隆（派生类应实现完整的深拷贝，包括子元素）
        return source->Clone();
    }
    
    /**
     * @brief 递归设置视觉树中所有元素的 TemplatedParent
     */
    void SetTemplatedParentRecursive(UIElement* element, UIElement* templatedParent) {
        if (!element) return;
        
        element->SetTemplatedParent(templatedParent);
        
        // 递归处理所有逻辑子元素
        for (UIElement* child : element->GetLogicalChildren()) {
            SetTemplatedParentRecursive(child, templatedParent);
        }
    }
}

void ControlTemplate::SetTargetType(const std::type_info& type) {
    CheckSealed();
    targetType_ = &type;
}

void ControlTemplate::SetFactory(FactoryFunc factory) {
    CheckSealed();
    factory_ = std::move(factory);
}

void ControlTemplate::SetVisualTree(UIElement* root) {
    CheckSealed();
    visualTree_ = root;
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
    
    // TODO: 处理模板绑定（TemplateBinding）
    
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
    
    // 递归搜索所有逻辑子元素
    for (UIElement* child : element->GetLogicalChildren()) {
        UIElement* found = FindNameRecursive(name, child);
        if (found) {
            return found;
        }
    }
    
    return nullptr;
}

} // namespace fk::ui
