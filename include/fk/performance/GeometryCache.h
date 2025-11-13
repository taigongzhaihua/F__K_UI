#pragma once

#include <memory>
#include <unordered_map>
#include <mutex>
#include <vector>

namespace fk {

/**
 * @brief 几何数据的缓存项
 */
struct GeometryCacheEntry {
    std::vector<float> vertices;        // 顶点数据
    std::vector<unsigned int> indices;  // 索引数据
    size_t vertexCount;
    size_t indexCount;
    size_t accessCount;                 // 访问次数
    std::chrono::steady_clock::time_point lastAccessTime;  // 最后访问时间

    GeometryCacheEntry()
        : vertexCount(0)
        , indexCount(0)
        , accessCount(0)
        , lastAccessTime(std::chrono::steady_clock::now())
    {
    }
};

/**
 * @brief 几何缓存，用于缓存常用的几何数据（如矩形、圆形等）
 * 
 * 特性：
 * - 线程安全
 * - LRU 淘汰策略
 * - 统计信息
 */
class GeometryCache {
public:
    static GeometryCache& Instance() {
        static GeometryCache instance;
        return instance;
    }

    /**
     * @brief 获取缓存的几何数据
     * @param key 缓存键
     * @return 缓存项指针，如果不存在则返回 nullptr
     */
    std::shared_ptr<GeometryCacheEntry> Get(const std::string& key);

    /**
     * @brief 添加或更新缓存
     * @param key 缓存键
     * @param vertices 顶点数据
     * @param indices 索引数据
     */
    void Put(const std::string& key, 
             const std::vector<float>& vertices,
             const std::vector<unsigned int>& indices);

    /**
     * @brief 检查缓存中是否存在指定键
     */
    bool Contains(const std::string& key) const;

    /**
     * @brief 清空缓存
     */
    void Clear();

    /**
     * @brief 设置最大缓存大小（字节）
     */
    void SetMaxCacheSize(size_t maxSize) {
        maxCacheSize_ = maxSize;
    }

    /**
     * @brief 获取当前缓存大小（字节）
     */
    size_t GetCurrentSize() const;

    /**
     * @brief 获取缓存统计信息
     */
    struct Stats {
        size_t totalRequests;   // 总请求数
        size_t cacheHits;       // 缓存命中数
        size_t cacheMisses;     // 缓存未命中数
        size_t evictions;       // 淘汰次数
        size_t currentEntries;  // 当前条目数
        size_t currentSize;     // 当前大小（字节）
        double hitRate;         // 命中率
    };

    Stats GetStats() const;

    /**
     * @brief 预缓存常用几何形状
     */
    void PreloadCommonGeometry();

private:
    GeometryCache()
        : maxCacheSize_(10 * 1024 * 1024)  // 默认 10MB
        , totalRequests_(0)
        , cacheHits_(0)
        , cacheMisses_(0)
        , evictions_(0)
    {
    }

    void EvictLRU();
    size_t CalculateEntrySize(const GeometryCacheEntry& entry) const;

    std::unordered_map<std::string, std::shared_ptr<GeometryCacheEntry>> cache_;
    mutable std::mutex mutex_;
    size_t maxCacheSize_;

    // 统计信息
    size_t totalRequests_;
    size_t cacheHits_;
    size_t cacheMisses_;
    size_t evictions_;
};

} // namespace fk
