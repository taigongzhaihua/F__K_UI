#pragma once

#include "fk/ui/styling/FrameworkTemplate.h"
#include <functional>
#include <typeinfo>
#include <vector>
#include <memory>

// 前向声明动画命名空间的类
namespace fk::animation {
    class VisualStateGroup;
}

namespace fk::ui {

// 前向声明
template<typename> class Control;

/**
 * @brief 控件模板
 * 
 * 职责：
 * - 定义控件的可视化结构
 * - 为特定控件类型实例化视觉树
 * 
 * WPF 对应：ControlTemplate
 */
class ControlTemplate : public FrameworkTemplate {
public:
    using FactoryFunc = std::function<UIElement*()>;
    
    ControlTemplate() = default;
    virtual ~ControlTemplate() = default;

    /**
     * @brief 设置目标控件类型
     */
    ControlTemplate* SetTargetType(const std::type_info& type);
    
    /**
     * @brief 获取目标控件类型
     */
    const std::type_info* GetTargetType() const { return targetType_; }
    
    /**
     * @brief 设置视觉树工厂函数
     */
    ControlTemplate* SetFactory(FactoryFunc factory);
    
    /**
     * @brief 设置视觉树根节点（直接指定）
     */
    ControlTemplate* SetVisualTree(UIElement* root);
    
    /**
     * @brief 获取视觉树根节点（模板定义，非实例）
     */
    UIElement* GetVisualTree() const { return visualTree_; }
    
    /**
     * @brief 实例化模板（为控件创建视觉树）
     * 
     * @param templatedParent 应用此模板的控件
     * @return 实例化的视觉树根节点
     */
    UIElement* Instantiate(UIElement* templatedParent);
    
    /**
     * @brief 检查模板是否有效（有工厂或视觉树）
     */
    bool IsValid() const { return factory_ || visualTree_; }
    
    /**
     * @brief 在模板实例中查找命名元素
     * 
     * @param name 元素名称
     * @param root 模板实例化的根节点
     * @return 找到的元素，未找到返回 nullptr
     */
    static UIElement* FindName(const std::string& name, UIElement* root);
    
    /**
     * @brief 添加视觉状态组（用于在模板中定义视觉状态）
     * 
     * @param group 要添加的状态组
     * @return this（支持链式调用）
     */
    ControlTemplate* AddVisualStateGroup(std::shared_ptr<animation::VisualStateGroup> group);
    
    /**
     * @brief 获取模板中定义的所有视觉状态组
     * 
     * @return 视觉状态组的列表
     */
    const std::vector<std::shared_ptr<animation::VisualStateGroup>>& GetVisualStateGroups() const { 
        return visualStateGroups_; 
    }
    
    /**
     * @brief 检查模板是否定义了视觉状态
     * 
     * @return 如果有定义视觉状态组则返回true
     */
    bool HasVisualStates() const { 
        return !visualStateGroups_.empty(); 
    }

private:
    /**
     * @brief 递归查找命名元素
     */
    static UIElement* FindNameRecursive(const std::string& name, UIElement* element);

    const std::type_info* targetType_{nullptr};
    FactoryFunc factory_;
    UIElement* visualTree_{nullptr};  // 视觉树定义（可选，优先使用 factory）
    std::vector<std::shared_ptr<animation::VisualStateGroup>> visualStateGroups_;  // 模板中定义的视觉状态组
};

} // namespace fk::ui
