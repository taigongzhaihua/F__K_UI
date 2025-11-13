#pragma once

#include "fk/resources/Theme.h"
#include "fk/core/Event.h"
#include <memory>
#include <unordered_map>
#include <mutex>

namespace fk::resources {

/**
 * @brief 主题管理器（单例）
 * 
 * 职责：
 * - 管理应用程序主题
 * - 支持主题注册和切换
 * - 提供全局主题访问
 * - 主题切换事件通知
 * 
 * 继承：无
 * WPF 对应：无直接对应（第三方库功能）
 */
class ThemeManager {
public:
    /**
     * @brief 获取单例实例
     */
    static ThemeManager& Instance() {
        static ThemeManager instance;
        return instance;
    }

    // 禁止拷贝和赋值
    ThemeManager(const ThemeManager&) = delete;
    ThemeManager& operator=(const ThemeManager&) = delete;

    // ========== 主题管理 ==========
    
    /**
     * @brief 注册主题
     */
    void RegisterTheme(std::shared_ptr<Theme> theme) {
        if (!theme) return;
        
        std::lock_guard<std::mutex> lock(mutex_);
        themes_[theme->GetName()] = theme;
    }
    
    /**
     * @brief 取消注册主题
     */
    void UnregisterTheme(const std::string& themeName) {
        std::lock_guard<std::mutex> lock(mutex_);
        themes_.erase(themeName);
    }
    
    /**
     * @brief 获取主题
     */
    std::shared_ptr<Theme> GetTheme(const std::string& themeName) const {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = themes_.find(themeName);
        return (it != themes_.end()) ? it->second : nullptr;
    }
    
    /**
     * @brief 检查主题是否存在
     */
    bool HasTheme(const std::string& themeName) const {
        std::lock_guard<std::mutex> lock(mutex_);
        return themes_.find(themeName) != themes_.end();
    }
    
    /**
     * @brief 获取所有主题名称
     */
    std::vector<std::string> GetThemeNames() const {
        std::lock_guard<std::mutex> lock(mutex_);
        std::vector<std::string> names;
        names.reserve(themes_.size());
        for (const auto& [name, _] : themes_) {
            names.push_back(name);
        }
        return names;
    }

    // ========== 当前主题 ==========
    
    /**
     * @brief 设置当前主题
     */
    bool SetCurrentTheme(const std::string& themeName) {
        auto theme = GetTheme(themeName);
        if (!theme) {
            return false;
        }
        
        std::shared_ptr<Theme> oldTheme;
        {
            std::lock_guard<std::mutex> lock(mutex_);
            oldTheme = currentTheme_;
            currentTheme_ = theme;
        }
        
        // 在锁外触发主题变更事件（避免死锁）
        ThemeChanged(oldTheme, theme);
        
        return true;
    }
    
    /**
     * @brief 获取当前主题
     */
    std::shared_ptr<Theme> GetCurrentTheme() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return currentTheme_;
    }
    
    /**
     * @brief 获取当前主题名称
     */
    std::string GetCurrentThemeName() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return currentTheme_ ? currentTheme_->GetName() : "";
    }

    // ========== 资源查找 ==========
    
    /**
     * @brief 从当前主题查找资源
     */
    template<typename T>
    T FindResource(const std::string& key) const {
        std::lock_guard<std::mutex> lock(mutex_);
        
        if (currentTheme_) {
            if (currentTheme_->ContainsResource(key)) {
                return currentTheme_->GetResource<T>(key);
            }
        }
        
        return T{};  // 返回默认值
    }
    
    /**
     * @brief 从当前主题查找资源（带默认值）
     */
    template<typename T>
    T FindResourceOrDefault(const std::string& key, const T& defaultValue) const {
        std::lock_guard<std::mutex> lock(mutex_);
        
        if (currentTheme_ && currentTheme_->ContainsResource(key)) {
            return currentTheme_->GetResource<T>(key);
        }
        
        return defaultValue;
    }
    
    /**
     * @brief 检查当前主题是否包含资源
     */
    bool ContainsResource(const std::string& key) const {
        std::lock_guard<std::mutex> lock(mutex_);
        return currentTheme_ && currentTheme_->ContainsResource(key);
    }

    // ========== 预定义主题 ==========
    
    /**
     * @brief 创建并注册默认的浅色主题
     */
    void CreateDefaultLightTheme();
    
    /**
     * @brief 创建并注册默认的深色主题
     */
    void CreateDefaultDarkTheme();

    // ========== 事件 ==========
    
    /**
     * @brief 主题切换事件
     * 参数：(oldTheme, newTheme)
     */
    core::Event<std::shared_ptr<Theme>, std::shared_ptr<Theme>> ThemeChanged;

private:
    ThemeManager() = default;
    ~ThemeManager() = default;

    mutable std::mutex mutex_;
    std::unordered_map<std::string, std::shared_ptr<Theme>> themes_;
    std::shared_ptr<Theme> currentTheme_;
};

} // namespace fk::resources
