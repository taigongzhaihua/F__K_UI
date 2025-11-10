#pragma once

#include "fk/ui/FrameworkTemplate.h"
#include <functional>
#include <typeinfo>

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
    void SetTargetType(const std::type_info& type);
    
    /**
     * @brief 获取目标控件类型
     */
    const std::type_info* GetTargetType() const { return targetType_; }
    
    /**
     * @brief 设置视觉树工厂函数
     */
    void SetFactory(FactoryFunc factory);
    
    /**
     * @brief 实例化模板（为控件创建视觉树）
     */
    UIElement* Instantiate(UIElement* templatedParent);

private:
    const std::type_info* targetType_{nullptr};
    FactoryFunc factory_;
};

} // namespace fk::ui
