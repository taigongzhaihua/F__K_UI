#pragma once

#include <vector>
#include <memory>
#include <mutex>
#include <functional>

namespace fk {

/**
 * @brief 通用对象池，用于复用对象以提高性能
 * @tparam T 对象类型
 * 
 * 特性：
 * - 线程安全
 * - 自动扩展
 * - 支持自定义对象初始化和重置
 */
template<typename T>
class ObjectPool {
public:
    using CreateFunc = std::function<std::shared_ptr<T>()>;
    using ResetFunc = std::function<void(T*)>;

    /**
     * @brief 构造对象池
     * @param initialSize 初始大小
     * @param createFunc 对象创建函数（默认使用 std::make_shared<T>()）
     * @param resetFunc 对象重置函数（默认什么都不做）
     */
    explicit ObjectPool(size_t initialSize = 10, 
                       CreateFunc createFunc = nullptr,
                       ResetFunc resetFunc = nullptr)
        : createFunc_(createFunc ? createFunc : []() { return std::make_shared<T>(); })
        , resetFunc_(resetFunc ? resetFunc : [](T*) {})
        , totalCreated_(0)
        , totalAcquired_(0)
        , totalReleased_(0)
    {
        // 预分配对象
        std::lock_guard<std::mutex> lock(mutex_);
        for (size_t i = 0; i < initialSize; ++i) {
            pool_.push_back(createFunc_());
            totalCreated_++;
        }
    }

    /**
     * @brief 从池中获取对象
     * @return 对象的共享指针
     */
    std::shared_ptr<T> Acquire() {
        std::lock_guard<std::mutex> lock(mutex_);
        totalAcquired_++;

        if (pool_.empty()) {
            // 池为空，创建新对象
            totalCreated_++;
            return createFunc_();
        }

        // 从池中取出对象
        auto obj = pool_.back();
        pool_.pop_back();
        return obj;
    }

    /**
     * @brief 将对象归还到池中
     * @param obj 要归还的对象
     */
    void Release(std::shared_ptr<T> obj) {
        if (!obj) return;

        std::lock_guard<std::mutex> lock(mutex_);
        totalReleased_++;

        // 重置对象状态
        resetFunc_(obj.get());

        // 归还到池中
        pool_.push_back(obj);
    }

    /**
     * @brief 获取池中可用对象数量
     */
    size_t GetAvailableCount() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return pool_.size();
    }

    /**
     * @brief 获取已创建的总对象数
     */
    size_t GetTotalCreated() const {
        return totalCreated_;
    }

    /**
     * @brief 获取统计信息
     */
    struct Stats {
        size_t totalCreated;
        size_t totalAcquired;
        size_t totalReleased;
        size_t available;
        size_t inUse;
    };

    Stats GetStats() const {
        std::lock_guard<std::mutex> lock(mutex_);
        Stats stats;
        stats.totalCreated = totalCreated_;
        stats.totalAcquired = totalAcquired_;
        stats.totalReleased = totalReleased_;
        stats.available = pool_.size();
        stats.inUse = totalCreated_ - pool_.size();
        return stats;
    }

    /**
     * @brief 清空池中的所有对象
     */
    void Clear() {
        std::lock_guard<std::mutex> lock(mutex_);
        pool_.clear();
    }

    /**
     * @brief 预分配指定数量的对象
     * @param count 要预分配的对象数量
     */
    void Reserve(size_t count) {
        std::lock_guard<std::mutex> lock(mutex_);
        for (size_t i = 0; i < count; ++i) {
            pool_.push_back(createFunc_());
            totalCreated_++;
        }
    }

private:
    std::vector<std::shared_ptr<T>> pool_;
    CreateFunc createFunc_;
    ResetFunc resetFunc_;
    mutable std::mutex mutex_;
    
    // 统计信息
    size_t totalCreated_;
    size_t totalAcquired_;
    size_t totalReleased_;
};

/**
 * @brief RAII 辅助类，自动从池中获取和释放对象
 */
template<typename T>
class PooledObject {
public:
    PooledObject(ObjectPool<T>& pool)
        : pool_(pool)
        , object_(pool.Acquire())
    {
    }

    ~PooledObject() {
        if (object_) {
            pool_.Release(object_);
        }
    }

    // 禁止拷贝
    PooledObject(const PooledObject&) = delete;
    PooledObject& operator=(const PooledObject&) = delete;

    // 允许移动
    PooledObject(PooledObject&& other) noexcept
        : pool_(other.pool_)
        , object_(std::move(other.object_))
    {
    }

    PooledObject& operator=(PooledObject&& other) noexcept {
        if (this != &other) {
            if (object_) {
                pool_.Release(object_);
            }
            object_ = std::move(other.object_);
        }
        return *this;
    }

    T* operator->() { return object_.get(); }
    const T* operator->() const { return object_.get(); }
    T& operator*() { return *object_; }
    const T& operator*() const { return *object_; }
    T* Get() { return object_.get(); }
    const T* Get() const { return object_.get(); }

private:
    ObjectPool<T>& pool_;
    std::shared_ptr<T> object_;
};

} // namespace fk
