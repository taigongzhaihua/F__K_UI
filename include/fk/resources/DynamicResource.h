#pragma once

#include "fk/resources/ThemeManager.h"
#include <string>
#include <functional>
#include <any>

namespace fk::resources {

/**
 * @brief 动态资源引用
 * 
 * 职责：
 * - 引用主题中的资源
 * - 自动响应主题切换
 * - 支持资源变更回调
 * 
 * 继承：无
 * WPF 对应：DynamicResource 标记扩展
 */
template<typename T>
class DynamicResource {
public:
    using ValueChangedCallback = std::function<void(const T&)>;

    /**
     * @brief 构造函数
     * @param key 资源键
     */
    explicit DynamicResource(const std::string& key) 
        : resourceKey_(key), currentValue_(T{}) {
        
        // 订阅主题变更事件
        ThemeManager::Instance().ThemeChanged.Connect(
            [this](std::shared_ptr<Theme> oldTheme, std::shared_ptr<Theme> newTheme) {
                OnThemeChanged(oldTheme, newTheme);
            }
        );
        
        // 初始化当前值
        UpdateValue();
    }
    
    ~DynamicResource() = default;

    /**
     * @brief 获取当前值
     */
    T GetValue() const {
        return currentValue_;
    }
    
    /**
     * @brief 获取资源键
     */
    std::string GetResourceKey() const {
        return resourceKey_;
    }
    
    /**
     * @brief 设置值变更回调
     */
    void SetValueChangedCallback(ValueChangedCallback callback) {
        valueChangedCallback_ = callback;
    }
    
    /**
     * @brief 刷新资源值
     */
    void Refresh() {
        UpdateValue();
    }
    
    /**
     * @brief 隐式转换为值类型
     */
    operator T() const {
        return currentValue_;
    }

private:
    void UpdateValue() {
        T newValue = ThemeManager::Instance().FindResource<T>(resourceKey_);
        
        // 检查值是否变更
        if (!ValuesEqual(currentValue_, newValue)) {
            currentValue_ = newValue;
            
            // 触发回调
            if (valueChangedCallback_) {
                valueChangedCallback_(currentValue_);
            }
        }
    }
    
    void OnThemeChanged(std::shared_ptr<Theme> oldTheme, std::shared_ptr<Theme> newTheme) {
        UpdateValue();
    }
    
    // 值比较辅助函数
    template<typename U>
    bool ValuesEqual(const U& a, const U& b) const {
        // 默认使用 operator==
        return a == b;
    }

    std::string resourceKey_;
    T currentValue_;
    ValueChangedCallback valueChangedCallback_;
};

/**
 * @brief 创建动态资源的辅助函数
 */
template<typename T>
std::shared_ptr<DynamicResource<T>> MakeDynamicResource(const std::string& key) {
    return std::make_shared<DynamicResource<T>>(key);
}

} // namespace fk::resources
