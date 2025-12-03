#pragma once

#include "fk/ui/styling/FrameworkTemplate.h"
#include <functional>
#include <any>

namespace fk::ui {

/**
 * @brief 数据模板
 * 
 * 职责：
 * - 定义数据对象的可视化表示
 * - 为任意数据上下文实例化视觉树
 * 
 * WPF 对应：DataTemplate
 */
class DataTemplate : public FrameworkTemplate {
public:
    using FactoryFunc = std::function<UIElement*(const std::any&)>;
    
    DataTemplate() = default;
    virtual ~DataTemplate() = default;

    /**
     * @brief 设置视觉树工厂函数
     * @param factory 接收数据上下文，返回 UIElement
     */
    void SetFactory(FactoryFunc factory);
    
    /**
     * @brief 设置视觉树根节点（直接指定）
     */
    void SetVisualTree(UIElement* root);
    
    /**
     * @brief 获取视觉树根节点（模板定义，非实例）
     */
    UIElement* GetVisualTree() const { return visualTree_; }
    
    /**
     * @brief 实例化模板（为数据对象创建视觉树）
     * @param dataContext 数据上下文
     * @return 实例化的视觉树根节点（已设置 DataContext）
     */
    UIElement* Instantiate(const std::any& dataContext);
    
    /**
     * @brief 检查模板是否有效（有工厂或视觉树）
     */
    bool IsValid() const { return factory_ || visualTree_; }

private:
    FactoryFunc factory_;
    UIElement* visualTree_{nullptr};  // 视觉树定义（可选，优先使用 factory）
};

} // namespace fk::ui
