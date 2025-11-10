#pragma once

#include <vector>
#include <cstddef>

namespace fk::ui {

// 前向声明
class Visual;

/**
 * @brief Visual 子节点集合管理器
 * 
 * 职责：
 * - 封装 Visual 的子节点集合
 * - 提供类型安全的操作接口
 * - 维护父子关系的一致性
 * 
 * 继承：无（集合辅助类）
 */
class VisualCollection {
public:
    using Iterator = std::vector<Visual*>::iterator;
    using ConstIterator = std::vector<Visual*>::const_iterator;

    /**
     * @brief 构造函数
     * @param owner 拥有此集合的 Visual 对象
     */
    explicit VisualCollection(Visual* owner);
    
    ~VisualCollection() = default;

    // 禁止拷贝
    VisualCollection(const VisualCollection&) = delete;
    VisualCollection& operator=(const VisualCollection&) = delete;

    // ========== 集合操作 ==========
    
    /**
     * @brief 添加子节点到末尾
     * @param child 要添加的子节点
     */
    void Add(Visual* child);
    
    /**
     * @brief 在指定位置插入子节点
     * @param index 插入位置
     * @param child 要插入的子节点
     */
    void Insert(size_t index, Visual* child);
    
    /**
     * @brief 移除指定子节点
     * @param child 要移除的子节点
     */
    void Remove(Visual* child);
    
    /**
     * @brief 按索引移除子节点
     * @param index 要移除的索引
     */
    void RemoveAt(size_t index);
    
    /**
     * @brief 清空所有子节点
     */
    void Clear();

    // ========== 查询操作 ==========
    
    /**
     * @brief 获取子节点数量
     */
    size_t Count() const { return children_.size(); }
    
    /**
     * @brief 检查集合是否为空
     */
    bool IsEmpty() const { return children_.empty(); }
    
    /**
     * @brief 索引访问
     * @param index 索引
     * @return 子节点指针
     */
    Visual* operator[](size_t index) const;
    
    /**
     * @brief 查找子节点的索引
     * @param child 要查找的子节点
     * @return 索引，如果未找到返回 -1
     */
    int IndexOf(Visual* child) const;
    
    /**
     * @brief 检查是否包含指定子节点
     * @param child 要检查的子节点
     * @return 是否包含
     */
    bool Contains(Visual* child) const;

    // ========== 迭代器支持 ==========
    
    /**
     * @brief 获取开始迭代器
     */
    Iterator begin() { return children_.begin(); }
    
    /**
     * @brief 获取结束迭代器
     */
    Iterator end() { return children_.end(); }
    
    /**
     * @brief 获取常量开始迭代器
     */
    ConstIterator begin() const { return children_.begin(); }
    
    /**
     * @brief 获取常量结束迭代器
     */
    ConstIterator end() const { return children_.end(); }
    
    /**
     * @brief 获取常量开始迭代器
     */
    ConstIterator cbegin() const { return children_.cbegin(); }
    
    /**
     * @brief 获取常量结束迭代器
     */
    ConstIterator cend() const { return children_.cend(); }

private:
    Visual* owner_;                  // 拥有此集合的 Visual 对象
    std::vector<Visual*> children_;  // 子节点列表
};

} // namespace fk::ui
