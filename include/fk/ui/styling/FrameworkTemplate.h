#pragma once

#include "fk/resources/ResourceDictionary.h"
#include <memory>
#include <functional>
#include <unordered_map>
#include <string>

namespace fk::ui {

using ResourceDictionary = fk::resources::ResourceDictionary;

// 前向声明
class UIElement;

/**
 * @brief 框架模板基类
 * 
 * 职责：
 * - 模板的密封机制
 * - 资源访问
 * 
 * WPF 对应：FrameworkTemplate
 */
class FrameworkTemplate {
public:
    FrameworkTemplate() = default;
    virtual ~FrameworkTemplate() = default;

    /**
     * @brief 密封模板（模板应用后不可修改）
     */
    void Seal();
    
    /**
     * @brief 检查是否已密封
     */
    bool IsSealed() const { return isSealed_; }
    
    /**
     * @brief 获取模板资源
     */
    ResourceDictionary* GetResources() { return resources_.get(); }

protected:
    /**
     * @brief 验证模板未密封（修改前调用）
     */
    void CheckSealed() const;

private:
    bool isSealed_{false};
    std::unique_ptr<ResourceDictionary> resources_;
};

} // namespace fk::ui
