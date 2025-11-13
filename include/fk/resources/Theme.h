#pragma once

#include "fk/ui/ResourceDictionary.h"
#include <string>
#include <memory>

namespace fk::resources {

/**
 * @brief 主题类
 * 
 * 职责：
 * - 定义应用程序的视觉主题
 * - 包含颜色、字体、样式等资源
 * - 支持主题切换
 * 
 * 继承：无
 * WPF 对应：Theme（第三方库概念）
 */
class Theme {
public:
    Theme(const std::string& name) : name_(name) {
        resources_ = std::make_shared<ui::ResourceDictionary>();
    }
    
    virtual ~Theme() = default;

    // ========== 主题属性 ==========
    
    /**
     * @brief 获取主题名称
     */
    std::string GetName() const { return name_; }
    
    /**
     * @brief 获取主题描述
     */
    std::string GetDescription() const { return description_; }
    void SetDescription(const std::string& value) { description_ = value; }
    
    /**
     * @brief 获取主题作者
     */
    std::string GetAuthor() const { return author_; }
    void SetAuthor(const std::string& value) { author_ = value; }
    
    /**
     * @brief 获取主题版本
     */
    std::string GetVersion() const { return version_; }
    void SetVersion(const std::string& value) { version_ = value; }

    // ========== 资源管理 ==========
    
    /**
     * @brief 获取主题的资源字典
     */
    std::shared_ptr<ui::ResourceDictionary> GetResources() const {
        return resources_;
    }
    
    /**
     * @brief 添加资源
     */
    template<typename T>
    void AddResource(const std::string& key, const T& value) {
        resources_->Add(key, value);
    }
    
    /**
     * @brief 获取资源
     */
    template<typename T>
    T GetResource(const std::string& key) const {
        return resources_->Get<T>(key);
    }
    
    /**
     * @brief 检查是否包含资源
     */
    bool ContainsResource(const std::string& key) const {
        return resources_->Contains(key);
    }

    // ========== 主题类型 ==========
    
    enum class ThemeType {
        Light,      // 浅色主题
        Dark,       // 深色主题
        Custom      // 自定义主题
    };
    
    ThemeType GetThemeType() const { return themeType_; }
    void SetThemeType(ThemeType value) { themeType_ = value; }

protected:
    std::string name_;
    std::string description_;
    std::string author_;
    std::string version_{"1.0.0"};
    ThemeType themeType_{ThemeType::Light};
    std::shared_ptr<ui::ResourceDictionary> resources_;
};

} // namespace fk::resources
