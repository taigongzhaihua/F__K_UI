#pragma once

#include <memory>
#include <unordered_map>
#include <mutex>
#include <string>
#include "../ui/DrawCommand.h"

namespace fk {

using Color = fk::ui::Color;
using Rect = fk::ui::Rect;

/**
 * @brief 材质数据
 */
struct Material {
    Color color;
    double opacity;
    std::string texturePath;
    bool hasTexture;

    Material()
        : color(Color::FromRGB(255, 255, 255))
        , opacity(1.0)
        , hasTexture(false)
    {
    }

    Material(const Color& c, double op = 1.0)
        : color(c)
        , opacity(op)
        , hasTexture(false)
    {
    }

    // 生成材质的哈希键
    std::string GetKey() const;

    bool operator==(const Material& other) const {
        return color == other.color && 
               opacity == other.opacity && 
               texturePath == other.texturePath &&
               hasTexture == other.hasTexture;
    }
};

/**
 * @brief 材质池，用于复用相同的材质对象
 * 
 * 特性：
 * - 线程安全
 * - 自动去重
 * - 引用计数
 */
class MaterialPool {
public:
    static MaterialPool& Instance() {
        static MaterialPool instance;
        return instance;
    }

    /**
     * @brief 获取或创建材质
     * @param material 材质数据
     * @return 材质的共享指针
     */
    std::shared_ptr<Material> GetOrCreate(const Material& material);

    /**
     * @brief 获取纯色材质
     */
    std::shared_ptr<Material> GetSolidColor(const Color& color, double opacity = 1.0);

    /**
     * @brief 清空池中未使用的材质
     */
    void Cleanup();

    /**
     * @brief 清空所有材质
     */
    void Clear();

    /**
     * @brief 获取池中材质数量
     */
    size_t GetMaterialCount() const;

    /**
     * @brief 获取统计信息
     */
    struct Stats {
        size_t totalMaterials;      // 总材质数
        size_t uniqueMaterials;     // 唯一材质数
        size_t duplicatesSaved;     // 避免创建的重复材质数
        size_t totalRequests;       // 总请求数
    };

    Stats GetStats() const;

private:
    MaterialPool()
        : totalRequests_(0)
        , duplicatesSaved_(0)
    {
    }

    std::unordered_map<std::string, std::shared_ptr<Material>> materials_;
    mutable std::mutex mutex_;

    // 统计信息
    size_t totalRequests_;
    size_t duplicatesSaved_;
};

} // namespace fk
