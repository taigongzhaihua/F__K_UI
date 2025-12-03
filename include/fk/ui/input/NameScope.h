#pragma once

#include <string>
#include <unordered_map>
#include <memory>

namespace fk::binding {
    class DependencyObject;
}

namespace fk::ui {

/**
 * @brief 命名作用域类 - 用于高效的元素名称查找
 * 
 * NameScope 提供 O(1) 的名称查找性能，通过内部哈希表（unordered_map）实现。
 * 这是对基本 FindName() 递归查找的性能优化方案。
 * 
 * 设计理念：
 * - 哈希表是实现手段，NameScope 是设计概念
 * - 提供清晰的作用域管理接口
 * - 自动维护名称索引
 * - 支持嵌套作用域
 * - 完全兼容WPF的命名作用域概念
 * 
 * 使用场景：
 * - 大型UI (> 1000 元素)
 * - 频繁查找 (每秒 > 10 次)
 * - 性能敏感的应用
 * 
 * 示例：
 * @code
 * // Window 自动创建 NameScope
 * auto* window = new Window();
 * 
 * auto* button = new Button();
 * button->Name("submitButton");
 * 
 * // O(1) 查找
 * auto* found = window->FindName("submitButton");
 * @endcode
 */
class NameScope {
public:
    NameScope() = default;
    ~NameScope() = default;
    
    // 禁止复制，允许移动
    NameScope(const NameScope&) = delete;
    NameScope& operator=(const NameScope&) = delete;
    NameScope(NameScope&&) = default;
    NameScope& operator=(NameScope&&) = default;
    
    /**
     * @brief 注册元素名称
     * 
     * @param name 元素名称
     * @param object 元素指针
     * @return 成功返回 true，如果名称已存在返回 false
     */
    bool RegisterName(const std::string& name, fk::binding::DependencyObject* object);
    
    /**
     * @brief 取消注册元素名称
     * 
     * @param name 要取消注册的元素名称
     */
    void UnregisterName(const std::string& name);
    
    /**
     * @brief 查找指定名称的元素
     * 
     * @param name 元素名称
     * @return 找到的元素指针，未找到返回 nullptr
     * 
     * 时间复杂度：O(1)
     */
    fk::binding::DependencyObject* FindName(const std::string& name) const;
    
    /**
     * @brief 更新元素名称
     * 
     * @param oldName 旧名称
     * @param newName 新名称
     * @param object 元素指针
     * @return 成功返回 true，如果新名称已存在返回 false
     * 
     * 用于元素重命名时自动维护索引
     */
    bool UpdateName(const std::string& oldName, const std::string& newName, 
                    fk::binding::DependencyObject* object);
    
    /**
     * @brief 检查名称是否已注册
     * 
     * @param name 要检查的名称
     * @return 已注册返回 true，否则返回 false
     */
    bool Contains(const std::string& name) const;
    
    /**
     * @brief 清空所有注册的名称
     */
    void Clear();
    
    /**
     * @brief 获取注册的名称数量
     * 
     * @return 名称数量
     */
    size_t GetCount() const { return names_.size(); }
    
private:
    // 名称到对象的哈希表映射 - O(1) 查找
    std::unordered_map<std::string, fk::binding::DependencyObject*> names_;
};

/**
 * @brief NameScope 附加属性键
 * 
 * 用于在 UIElement 上附加 NameScope 实例
 */
class NameScopeProperty {
public:
    static const std::string Key;
};

} // namespace fk::ui
