#include "fk/ui/VisualCollection.h"
#include "fk/ui/Visual.h"
#include <algorithm>
#include <stdexcept>

namespace fk::ui {

VisualCollection::VisualCollection(Visual* owner) 
    : owner_(owner) {
}

void VisualCollection::Add(Visual* child) {
    if (!child) {
        throw std::invalid_argument("Cannot add null child to VisualCollection");
    }
    
    if (!owner_) {
        throw std::runtime_error("VisualCollection has no owner");
    }
    
    // 如果子节点已有父节点，先从旧父节点移除
    if (child->visualParent_ && child->visualParent_ != owner_) {
        child->visualParent_->RemoveVisualChild(child);
    }
    
    // 避免重复添加
    if (child->visualParent_ == owner_) {
        return;
    }
    
    // 添加到集合并设置父指针
    children_.push_back(child);
    owner_->visualChildren_.push_back(child);
    child->visualParent_ = owner_;
}

void VisualCollection::Insert(size_t index, Visual* child) {
    if (!child) {
        throw std::invalid_argument("Cannot insert null child to VisualCollection");
    }
    
    if (!owner_) {
        throw std::runtime_error("VisualCollection has no owner");
    }
    
    if (index > children_.size()) {
        throw std::out_of_range("Index out of range in VisualCollection::Insert");
    }
    
    // 如果子节点已有父节点，先从旧父节点移除
    if (child->visualParent_ && child->visualParent_ != owner_) {
        child->visualParent_->RemoveVisualChild(child);
    }
    
    // 避免重复添加
    if (child->visualParent_ == owner_) {
        return;
    }
    
    // 插入到指定位置并设置父指针
    children_.insert(children_.begin() + index, child);
    owner_->visualChildren_.insert(owner_->visualChildren_.begin() + index, child);
    child->visualParent_ = owner_;
}

void VisualCollection::Remove(Visual* child) {
    if (!child || !owner_) {
        return;
    }
    
    auto it = std::find(children_.begin(), children_.end(), child);
    if (it != children_.end()) {
        children_.erase(it);
        
        // 从 owner 的子节点列表中移除
        auto ownerIt = std::find(owner_->visualChildren_.begin(), 
                                 owner_->visualChildren_.end(), child);
        if (ownerIt != owner_->visualChildren_.end()) {
            owner_->visualChildren_.erase(ownerIt);
        }
        
        // 清除父指针
        child->visualParent_ = nullptr;
    }
}

void VisualCollection::RemoveAt(size_t index) {
    if (index >= children_.size()) {
        throw std::out_of_range("Index out of range in VisualCollection::RemoveAt");
    }
    
    if (!owner_) {
        throw std::runtime_error("VisualCollection has no owner");
    }
    
    Visual* child = children_[index];
    children_.erase(children_.begin() + index);
    
    // 从 owner 的子节点列表中移除
    auto ownerIt = std::find(owner_->visualChildren_.begin(), 
                             owner_->visualChildren_.end(), child);
    if (ownerIt != owner_->visualChildren_.end()) {
        owner_->visualChildren_.erase(ownerIt);
    }
    
    // 清除父指针
    if (child) {
        child->visualParent_ = nullptr;
    }
}

void VisualCollection::Clear() {
    // 移除所有子节点
    while (!children_.empty()) {
        RemoveAt(0);
    }
}

Visual* VisualCollection::operator[](size_t index) const {
    if (index >= children_.size()) {
        throw std::out_of_range("Index out of range in VisualCollection::operator[]");
    }
    return children_[index];
}

int VisualCollection::IndexOf(Visual* child) const {
    auto it = std::find(children_.begin(), children_.end(), child);
    if (it != children_.end()) {
        return static_cast<int>(std::distance(children_.begin(), it));
    }
    return -1;
}

bool VisualCollection::Contains(Visual* child) const {
    return std::find(children_.begin(), children_.end(), child) != children_.end();
}

} // namespace fk::ui
