#pragma once

#include <any>
#include <string>
#include <unordered_map>
#include <memory>
#include <vector>

namespace fk::resources {

/**
 * @brief 资源字典
 * 
 * 职责：
 * - 存储和检索资源（样式、模板、画刷等）
 * - 支持资源继承（MergedDictionaries）
 * 
 * WPF 对应：ResourceDictionary
 */
class ResourceDictionary {
public:
    ResourceDictionary() = default;
    virtual ~ResourceDictionary() = default;

    /**
     * @brief 添加资源
     */
    void Add(const std::string& key, const std::any& value) {
        resources_[key] = value;
    }
    
    /**
     * @brief 获取资源
     */
    template<typename T>
    T Get(const std::string& key) const {
        auto it = resources_.find(key);
        if (it != resources_.end()) {
            return std::any_cast<T>(it->second);
        }
        
        // 在合并的字典中查找
        for (const auto& merged : mergedDictionaries_) {
            if (merged->Contains(key)) {
                return merged->Get<T>(key);
            }
        }
        
        return T{};  // 返回默认值
    }
    
    /**
     * @brief 检查是否包含资源
     */
    bool Contains(const std::string& key) const {
        if (resources_.find(key) != resources_.end()) {
            return true;
        }
        
        for (const auto& merged : mergedDictionaries_) {
            if (merged->Contains(key)) {
                return true;
            }
        }
        
        return false;
    }
    
    /**
     * @brief 移除资源
     */
    void Remove(const std::string& key) {
        resources_.erase(key);
    }
    
    /**
     * @brief 清空资源
     */
    void Clear() {
        resources_.clear();
    }
    
    /**
     * @brief 获取资源数量
     */
    size_t Count() const {
        return resources_.size();
    }
    
    /**
     * @brief 添加合并的资源字典
     */
    void AddMergedDictionary(std::shared_ptr<ResourceDictionary> dictionary) {
        if (dictionary) {
            mergedDictionaries_.push_back(dictionary);
        }
    }
    
    /**
     * @brief 获取合并的字典列表
     */
    const std::vector<std::shared_ptr<ResourceDictionary>>& GetMergedDictionaries() const {
        return mergedDictionaries_;
    }

private:
    std::unordered_map<std::string, std::any> resources_;
    std::vector<std::shared_ptr<ResourceDictionary>> mergedDictionaries_;
};

} // namespace fk::resources
