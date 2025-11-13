#include "fk/performance/GeometryCache.h"
#include <algorithm>
#include <cmath>

namespace fk {

std::shared_ptr<GeometryCacheEntry> GeometryCache::Get(const std::string& key) {
    std::lock_guard<std::mutex> lock(mutex_);
    totalRequests_++;

    auto it = cache_.find(key);
    if (it != cache_.end()) {
        cacheHits_++;
        it->second->accessCount++;
        it->second->lastAccessTime = std::chrono::steady_clock::now();
        return it->second;
    }

    cacheMisses_++;
    return nullptr;
}

void GeometryCache::Put(const std::string& key,
                        const std::vector<float>& vertices,
                        const std::vector<unsigned int>& indices) {
    std::lock_guard<std::mutex> lock(mutex_);

    auto entry = std::make_shared<GeometryCacheEntry>();
    entry->vertices = vertices;
    entry->indices = indices;
    entry->vertexCount = vertices.size();
    entry->indexCount = indices.size();
    entry->accessCount = 1;
    entry->lastAccessTime = std::chrono::steady_clock::now();

    // 检查缓存大小
    size_t entrySize = CalculateEntrySize(*entry);
    size_t currentSize = GetCurrentSize();

    while (currentSize + entrySize > maxCacheSize_ && !cache_.empty()) {
        EvictLRU();
        currentSize = GetCurrentSize();
    }

    cache_[key] = entry;
}

bool GeometryCache::Contains(const std::string& key) const {
    std::lock_guard<std::mutex> lock(mutex_);
    return cache_.find(key) != cache_.end();
}

void GeometryCache::Clear() {
    std::lock_guard<std::mutex> lock(mutex_);
    cache_.clear();
}

size_t GeometryCache::GetCurrentSize() const {
    size_t totalSize = 0;
    for (const auto& pair : cache_) {
        totalSize += CalculateEntrySize(*pair.second);
    }
    return totalSize;
}

GeometryCache::Stats GeometryCache::GetStats() const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    Stats stats;
    stats.totalRequests = totalRequests_;
    stats.cacheHits = cacheHits_;
    stats.cacheMisses = cacheMisses_;
    stats.evictions = evictions_;
    stats.currentEntries = cache_.size();
    stats.currentSize = GetCurrentSize();
    stats.hitRate = totalRequests_ > 0 ? 
        static_cast<double>(cacheHits_) / totalRequests_ : 0.0;

    return stats;
}

void GeometryCache::PreloadCommonGeometry() {
    // 预加载常用几何形状

    // 1. 单位矩形 (0,0) to (1,1)
    {
        std::vector<float> vertices = {
            0.0f, 0.0f,  // 左下
            1.0f, 0.0f,  // 右下
            1.0f, 1.0f,  // 右上
            0.0f, 1.0f   // 左上
        };
        std::vector<unsigned int> indices = {
            0, 1, 2,
            2, 3, 0
        };
        Put("rect_unit", vertices, indices);
    }

    // 2. 圆形 (32 个顶点)
    {
        const int segments = 32;
        std::vector<float> vertices;
        std::vector<unsigned int> indices;

        // 中心点
        vertices.push_back(0.0f);
        vertices.push_back(0.0f);

        // 圆周上的点
        for (int i = 0; i <= segments; ++i) {
            float angle = 2.0f * 3.14159265359f * i / segments;
            vertices.push_back(std::cos(angle));
            vertices.push_back(std::sin(angle));
        }

        // 索引
        for (int i = 1; i <= segments; ++i) {
            indices.push_back(0);
            indices.push_back(i);
            indices.push_back(i + 1);
        }

        Put("circle_32", vertices, indices);
    }

    // 3. 圆角矩形
    {
        const int cornerSegments = 8;
        std::vector<float> vertices;
        std::vector<unsigned int> indices;

        // 简化版：生成 4 个圆角
        // 这里只是示例，实际实现会更复杂
        vertices = {
            0.1f, 0.0f,
            0.9f, 0.0f,
            1.0f, 0.1f,
            1.0f, 0.9f,
            0.9f, 1.0f,
            0.1f, 1.0f,
            0.0f, 0.9f,
            0.0f, 0.1f
        };
        
        indices = {
            0, 1, 2,
            0, 2, 7,
            2, 3, 4,
            2, 4, 7,
            4, 5, 6,
            4, 6, 7
        };

        Put("rounded_rect_simple", vertices, indices);
    }
}

void GeometryCache::EvictLRU() {
    if (cache_.empty()) return;

    // 找到最久未使用的条目
    auto oldestIt = cache_.begin();
    auto oldestTime = oldestIt->second->lastAccessTime;

    for (auto it = cache_.begin(); it != cache_.end(); ++it) {
        if (it->second->lastAccessTime < oldestTime) {
            oldestTime = it->second->lastAccessTime;
            oldestIt = it;
        }
    }

    cache_.erase(oldestIt);
    evictions_++;
}

size_t GeometryCache::CalculateEntrySize(const GeometryCacheEntry& entry) const {
    return entry.vertices.size() * sizeof(float) +
           entry.indices.size() * sizeof(unsigned int) +
           sizeof(GeometryCacheEntry);
}

} // namespace fk
