#pragma once

#include "fk/core/Event.h"
#include <vector>
#include <any>
#include <algorithm>

namespace fk::ui {

/**
 * @brief 集合变更操作类型
 */
enum class CollectionChangeAction {
    Add,        // 添加项
    Remove,     // 移除项
    Replace,    // 替换项
    Move,       // 移动项
    Reset       // 重置（清空或批量更改）
};

/**
 * @brief 集合变更事件参数
 */
struct CollectionChangedEventArgs {
    CollectionChangeAction action{CollectionChangeAction::Reset};
    std::vector<std::any> newItems;         // 新添加的项
    std::vector<std::any> oldItems;         // 移除的项
    int newStartingIndex{-1};               // 新项起始索引
    int oldStartingIndex{-1};               // 旧项起始索引
    
    CollectionChangedEventArgs() = default;
    explicit CollectionChangedEventArgs(CollectionChangeAction a) : action(a) {}
};

/**
 * @brief 可观察集合
 * 
 * 职责：
 * - 管理项集合
 * - 触发集合变更事件
 * - 支持添加、删除、清空等操作
 * 
 * WPF 对应：ObservableCollection<T>
 */
class ObservableCollection {
public:
    ObservableCollection() = default;
    ~ObservableCollection() = default;

    // ========== 集合操作 ==========
    
    /**
     * @brief 添加项
     */
    void Add(const std::any& item) {
        items_.push_back(item);
        
        CollectionChangedEventArgs args(CollectionChangeAction::Add);
        args.newItems.push_back(item);
        args.newStartingIndex = static_cast<int>(items_.size()) - 1;
        collectionChanged_(args);
    }
    
    /**
     * @brief 在指定位置插入项
     */
    void Insert(int index, const std::any& item) {
        if (index < 0 || index > static_cast<int>(items_.size())) {
            return;
        }
        
        items_.insert(items_.begin() + index, item);
        
        CollectionChangedEventArgs args(CollectionChangeAction::Add);
        args.newItems.push_back(item);
        args.newStartingIndex = index;
        collectionChanged_(args);
    }
    
    /**
     * @brief 移除指定位置的项
     */
    void RemoveAt(int index) {
        if (index < 0 || index >= static_cast<int>(items_.size())) {
            return;
        }
        
        std::any oldItem = items_[index];
        items_.erase(items_.begin() + index);
        
        CollectionChangedEventArgs args(CollectionChangeAction::Remove);
        args.oldItems.push_back(oldItem);
        args.oldStartingIndex = index;
        collectionChanged_(args);
    }
    
    /**
     * @brief 移除指定项
     */
    bool Remove(const std::any& item) {
        auto it = std::find_if(items_.begin(), items_.end(), [&](const std::any& a) {
            // 简化比较：只比较类型
            return a.type() == item.type();
        });
        
        if (it != items_.end()) {
            int index = static_cast<int>(std::distance(items_.begin(), it));
            RemoveAt(index);
            return true;
        }
        
        return false;
    }
    
    /**
     * @brief 清空集合
     */
    void Clear() {
        if (items_.empty()) {
            return;
        }
        
        items_.clear();
        
        CollectionChangedEventArgs args(CollectionChangeAction::Reset);
        collectionChanged_(args);
    }
    
    /**
     * @brief 获取指定位置的项
     */
    std::any operator[](int index) const {
        if (index >= 0 && index < static_cast<int>(items_.size())) {
            return items_[index];
        }
        return std::any();
    }
    
    /**
     * @brief 获取集合大小
     */
    int Count() const {
        return static_cast<int>(items_.size());
    }
    
    /**
     * @brief 检查集合是否为空
     */
    bool IsEmpty() const {
        return items_.empty();
    }
    
    /**
     * @brief 获取底层 vector（用于迭代）
     */
    const std::vector<std::any>& GetItems() const {
        return items_;
    }

    // ========== 事件 ==========
    
    /**
     * @brief 集合变更事件
     */
    core::Event<CollectionChangedEventArgs>& CollectionChanged() {
        return collectionChanged_;
    }

private:
    std::vector<std::any> items_;
    core::Event<CollectionChangedEventArgs> collectionChanged_;
};

} // namespace fk::ui
