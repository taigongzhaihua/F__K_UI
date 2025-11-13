#include "fk/ui/ItemContainerGenerator.h"
#include "fk/ui/ItemsControl.h"
#include "fk/ui/ContentControl.h"
#include "fk/ui/UIElement.h"
#include <sstream>
#include <typeinfo>
#include <cstring>

namespace fk::ui {

ItemContainerGenerator::ItemContainerGenerator(ItemsControl<void>* owner)
    : owner_(owner) {
}

// ========== 容器生成 ==========

UIElement* ItemContainerGenerator::GenerateContainer(const std::any& item, bool& isNewContainer) {
    status_ = GeneratorStatus::GeneratingContainers;
    
    // 检查项是否已有容器
    std::string itemKey = GenerateItemKey(item);
    auto it = itemToContainer_.find(itemKey);
    if (it != itemToContainer_.end()) {
        isNewContainer = false;
        return it->second;
    }
    
    // 尝试从池中获取可重用的容器
    UIElement* container = nullptr;
    if (enableRecycling_ && !containerPool_.empty()) {
        container = GetRecycledContainer();
        if (container) {
            isNewContainer = false;
            // 准备容器以供新项使用
            ClearContainer(container);
        }
    }
    
    // 如果没有可重用的容器，创建新容器
    if (!container) {
        container = CreateNewContainer(item);
        isNewContainer = true;
    }
    
    if (container) {
        // 建立映射关系
        itemToContainer_[itemKey] = container;
        containerToItem_[container] = item;
        containers_.push_back(container);
        
        // 准备容器
        PrepareContainer(container, item);
    }
    
    status_ = GeneratorStatus::ContainersGenerated;
    return container;
}

std::vector<UIElement*> ItemContainerGenerator::GenerateContainers(
    const std::vector<std::any>& items) {
    
    std::vector<UIElement*> result;
    result.reserve(items.size());
    
    for (const auto& item : items) {
        bool isNew = false;
        UIElement* container = GenerateContainer(item, isNew);
        if (container) {
            result.push_back(container);
        }
    }
    
    return result;
}

UIElement* ItemContainerGenerator::GenerateNext(
    const GeneratorPosition& position, int direction) {
    
    int index = IndexFromGeneratorPosition(position) + direction;
    if (index < 0 || index >= static_cast<int>(containers_.size())) {
        return nullptr;
    }
    
    currentPosition_ = GeneratorPositionFromIndex(index);
    return containers_[index];
}

// ========== 容器查找 ==========

UIElement* ItemContainerGenerator::ContainerFromItem(const std::any& item) const {
    std::string itemKey = GenerateItemKey(item);
    auto it = itemToContainer_.find(itemKey);
    return (it != itemToContainer_.end()) ? it->second : nullptr;
}

UIElement* ItemContainerGenerator::ContainerFromIndex(int index) const {
    if (index < 0 || index >= static_cast<int>(containers_.size())) {
        return nullptr;
    }
    return containers_[index];
}

std::any ItemContainerGenerator::ItemFromContainer(UIElement* container) const {
    auto it = containerToItem_.find(container);
    return (it != containerToItem_.end()) ? it->second : std::any{};
}

int ItemContainerGenerator::IndexFromContainer(UIElement* container) const {
    for (size_t i = 0; i < containers_.size(); ++i) {
        if (containers_[i] == container) {
            return static_cast<int>(i);
        }
    }
    return -1;
}

// ========== 容器回收 ==========

void ItemContainerGenerator::RecycleContainer(UIElement* container) {
    if (!container || !enableRecycling_) {
        return;
    }
    
    // 从映射中移除
    auto itemIt = containerToItem_.find(container);
    if (itemIt != containerToItem_.end()) {
        std::string itemKey = GenerateItemKey(itemIt->second);
        itemToContainer_.erase(itemKey);
        containerToItem_.erase(itemIt);
    }
    
    // 从容器列表中移除
    auto it = std::find(containers_.begin(), containers_.end(), container);
    if (it != containers_.end()) {
        containers_.erase(it);
    }
    
    // 清理容器
    ClearContainer(container);
    
    // 添加到池中
    containerPool_.push_back(container);
}

void ItemContainerGenerator::RecycleContainers(
    const std::vector<UIElement*>& containers) {
    
    for (auto* container : containers) {
        RecycleContainer(container);
    }
}

void ItemContainerGenerator::ClearContainerPool() {
    // 释放池中的容器
    for (auto* container : containerPool_) {
        delete container;
    }
    containerPool_.clear();
}

void ItemContainerGenerator::RemoveContainer(UIElement* container) {
    if (!container) return;
    
    // 从映射中移除
    auto itemIt = containerToItem_.find(container);
    if (itemIt != containerToItem_.end()) {
        std::string itemKey = GenerateItemKey(itemIt->second);
        itemToContainer_.erase(itemKey);
        containerToItem_.erase(itemIt);
    }
    
    // 从容器列表中移除
    auto it = std::find(containers_.begin(), containers_.end(), container);
    if (it != containers_.end()) {
        containers_.erase(it);
    }
    
    // 释放容器
    delete container;
}

void ItemContainerGenerator::RemoveAll() {
    // 释放所有容器
    for (auto* container : containers_) {
        try {
            delete container;
        } catch (const std::bad_any_cast& e) {
            // 忽略删除时的any_cast错误
            // 这可能发生在DataContext包含已失效的any对象时
        } catch (...) {
            // 忽略其他删除错误
        }
    }
    
    // 清空所有集合
    containers_.clear();
    itemToContainer_.clear();
    containerToItem_.clear();
    
    // 清空池
    ClearContainerPool();
    
    status_ = GeneratorStatus::NotStarted;
}

// ========== 容器准备 ==========

void ItemContainerGenerator::PrepareContainer(UIElement* container, const std::any& item) {
    if (!container) return;
    
    try {
        // 设置数据上下文
        container->SetDataContext(item);
        
        // 调用自定义准备器
        if (containerPreparer_) {
            containerPreparer_(container, item);
        }
        
        // 如果所有者有准备逻辑，也调用
        if (owner_) {
            owner_->PrepareContainerForItem(container, item);
        }
    } catch (const std::bad_any_cast& e) {
        // 忽略any_cast错误，这可能发生在item包含已失效的对象时
    } catch (...) {
        // 忽略其他准备错误
    }
}

void ItemContainerGenerator::ClearContainer(UIElement* container) {
    if (!container) return;
    
    // 清除数据上下文
    container->SetDataContext(std::any{});
    
    // TODO: 清除其他与数据项相关的属性
}

// ========== 位置转换 ==========

GeneratorPosition ItemContainerGenerator::GeneratorPositionFromIndex(int itemIndex) const {
    if (itemIndex < 0 || itemIndex >= static_cast<int>(containers_.size())) {
        return GeneratorPosition{-1, 0};
    }
    return GeneratorPosition{itemIndex, 0};
}

int ItemContainerGenerator::IndexFromGeneratorPosition(const GeneratorPosition& position) const {
    return position.index + position.offset;
}

// ========== 私有方法 ==========

UIElement* ItemContainerGenerator::GetRecycledContainer() {
    if (containerPool_.empty()) {
        return nullptr;
    }
    
    UIElement* container = containerPool_.back();
    containerPool_.pop_back();
    return container;
}

UIElement* ItemContainerGenerator::CreateNewContainer(const std::any& item) {
    // 优先使用自定义工厂
    if (containerFactory_) {
        return containerFactory_(item);
    }
    
    // 使用所有者的容器创建逻辑
    if (owner_) {
        UIElement* container = owner_->GetContainerForItem(item);
        if (container) {
            return container;
        }
    }
    
    // 默认：创建 ContentControl 作为容器
    // TODO: 需要实现 ContentControl 的无参构造
    // return new ContentControl();
    
    return nullptr;
}

std::string ItemContainerGenerator::GenerateItemKey(const std::any& item) const {
    // 简化实现：使用类型信息和地址生成键
    // 更好的实现应该支持值比较
    
    std::ostringstream oss;
    
    // 检查是否为空
    if (!item.has_value()) {
        oss << "empty_any_" << &item;
        return oss.str();
    }
    
    // 添加类型信息
    oss << item.type().name();
    
    // 尝试获取指针值
    try {
        // 如果是指针类型
        if (item.type() == typeid(void*) || 
            item.type() == typeid(UIElement*) ||
            std::string(item.type().name()).find("*") != std::string::npos) {
            
            // 获取指针地址
            const void* ptr = nullptr;
            if (item.type() == typeid(UIElement*)) {
                ptr = std::any_cast<UIElement*>(item);
            } else {
                // 通用指针处理
                std::memcpy(&ptr, &item, sizeof(void*));
            }
            oss << "_" << ptr;
        } else {
            // 对于值类型，使用 any 的地址作为标识
            oss << "_" << &item;
        }
    } catch (...) {
        // 失败时使用 any 地址
        oss << "_" << &item;
    }
    
    return oss.str();
}

} // namespace fk::ui
