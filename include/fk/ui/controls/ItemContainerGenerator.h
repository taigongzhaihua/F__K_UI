#pragma once

#include <any>
#include <vector>
#include <unordered_map>
#include <functional>
#include <memory>

namespace fk::ui {

// 前向声明
class UIElement;
template<typename Derived = void>
class ItemsControl;

/**
 * @brief 容器生成器状态
 */
enum class GeneratorStatus {
    NotStarted,      // 未开始生成
    GeneratingContainers,  // 正在生成容器
    ContainersGenerated   // 容器已生成
};

/**
 * @brief 容器生成位置
 */
struct GeneratorPosition {
    int index{-1};       // 项的索引
    int offset{0};       // 相对偏移
    
    GeneratorPosition() = default;
    GeneratorPosition(int idx, int off = 0) : index(idx), offset(off) {}
    
    bool operator==(const GeneratorPosition& other) const {
        return index == other.index && offset == other.offset;
    }
};

/**
 * @brief 项容器生成器
 * 
 * 职责：
 * - 管理项到容器的映射关系
 * - 实现容器的重用和回收机制
 * - 支持增量生成（虚拟化基础）
 * - 追踪容器的生成状态
 * 
 * 设计模式：
 * - Factory Pattern: 创建容器
 * - Object Pool: 容器重用
 * - Strategy Pattern: 可自定义容器创建策略
 */
class ItemContainerGenerator {
public:
    using ContainerFactory = std::function<UIElement*(const std::any& item)>;
    using ContainerPreparer = std::function<void(UIElement* container, const std::any& item)>;
    
    /**
     * @brief 构造函数
     * @param owner 拥有此生成器的 ItemsControl
     */
    explicit ItemContainerGenerator(ItemsControl<void>* owner);
    
    ~ItemContainerGenerator() = default;
    
    // ========== 状态查询 ==========
    
    /**
     * @brief 获取生成器状态
     */
    GeneratorStatus GetStatus() const { return status_; }
    
    /**
     * @brief 获取已生成容器的数量
     */
    int GetContainerCount() const { return static_cast<int>(containers_.size()); }
    
    // ========== 容器生成 ==========
    
    /**
     * @brief 为指定项生成或获取容器
     * @param item 数据项
     * @param isNewContainer 输出参数，表示是否新创建的容器
     * @return 容器UIElement指针
     */
    UIElement* GenerateContainer(const std::any& item, bool& isNewContainer);
    
    /**
     * @brief 批量生成容器
     * @param items 数据项列表
     * @return 生成的容器列表
     */
    std::vector<UIElement*> GenerateContainers(const std::vector<std::any>& items);
    
    /**
     * @brief 从位置开始生成容器
     * @param position 起始位置
     * @param direction 生成方向（1:向后, -1:向前）
     * @return 生成的容器
     */
    UIElement* GenerateNext(const GeneratorPosition& position, int direction = 1);
    
    // ========== 容器查找 ==========
    
    /**
     * @brief 根据项查找容器
     */
    UIElement* ContainerFromItem(const std::any& item) const;
    
    /**
     * @brief 根据索引查找容器
     */
    UIElement* ContainerFromIndex(int index) const;
    
    /**
     * @brief 根据容器查找项
     */
    std::any ItemFromContainer(UIElement* container) const;
    
    /**
     * @brief 根据容器查找索引
     */
    int IndexFromContainer(UIElement* container) const;
    
    // ========== 容器回收 ==========
    
    /**
     * @brief 回收容器到池中（用于虚拟化）
     * @param container 要回收的容器
     */
    void RecycleContainer(UIElement* container);
    
    /**
     * @brief 批量回收容器
     */
    void RecycleContainers(const std::vector<UIElement*>& containers);
    
    /**
     * @brief 清空容器池
     */
    void ClearContainerPool();
    
    /**
     * @brief 移除并释放容器
     * @param container 要移除的容器
     */
    void RemoveContainer(UIElement* container);
    
    /**
     * @brief 移除所有容器
     */
    void RemoveAll();
    
    // ========== 容器准备 ==========
    
    /**
     * @brief 准备容器（设置数据上下文等）
     * @param container 容器
     * @param item 数据项
     */
    void PrepareContainer(UIElement* container, const std::any& item);
    
    /**
     * @brief 清理容器（用于回收时）
     */
    void ClearContainer(UIElement* container);
    
    // ========== 策略配置 ==========
    
    /**
     * @brief 设置容器工厂函数
     */
    void SetContainerFactory(ContainerFactory factory) {
        containerFactory_ = std::move(factory);
    }
    
    /**
     * @brief 设置容器准备函数
     */
    void SetContainerPreparer(ContainerPreparer preparer) {
        containerPreparer_ = std::move(preparer);
    }
    
    /**
     * @brief 启用容器重用
     */
    void SetEnableRecycling(bool enable) { enableRecycling_ = enable; }
    
    /**
     * @brief 是否启用容器重用
     */
    bool IsRecyclingEnabled() const { return enableRecycling_; }
    
    // ========== 位置转换 ==========
    
    /**
     * @brief 从索引获取生成器位置
     */
    GeneratorPosition GeneratorPositionFromIndex(int itemIndex) const;
    
    /**
     * @brief 从生成器位置获取索引
     */
    int IndexFromGeneratorPosition(const GeneratorPosition& position) const;

private:
    /**
     * @brief 从池中获取可重用的容器
     */
    UIElement* GetRecycledContainer();
    
    /**
     * @brief 创建新容器
     */
    UIElement* CreateNewContainer(const std::any& item);
    
    /**
     * @brief 生成项到容器的键
     */
    std::string GenerateItemKey(const std::any& item) const;

private:
    ItemsControl<void>* owner_;                         // 拥有者
    GeneratorStatus status_{GeneratorStatus::NotStarted};  // 状态
    
    // 容器映射
    std::vector<UIElement*> containers_;                // 已生成容器列表
    std::unordered_map<std::string, UIElement*> itemToContainer_;  // 项->容器映射
    std::unordered_map<UIElement*, std::any> containerToItem_;     // 容器->项映射
    
    // 容器池（用于重用）
    std::vector<UIElement*> containerPool_;             // 回收的容器池
    bool enableRecycling_{true};                        // 是否启用重用
    
    // 策略函数
    ContainerFactory containerFactory_;                 // 容器工厂
    ContainerPreparer containerPreparer_;              // 容器准备器
    
    // 当前生成位置
    GeneratorPosition currentPosition_;
};

} // namespace fk::ui
