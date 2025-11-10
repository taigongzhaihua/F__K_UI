#pragma once

#include "fk/ui/FrameworkTemplate.h"
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
     * @brief 实例化模板（为数据对象创建视觉树）
     * @param dataContext 数据上下文
     */
    UIElement* Instantiate(const std::any& dataContext);

private:
    FactoryFunc factory_;
};

} // namespace fk::ui
