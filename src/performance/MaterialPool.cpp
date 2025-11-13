#include "fk/performance/MaterialPool.h"
#include <sstream>
#include <iomanip>

namespace fk {

std::string Material::GetKey() const {
    std::ostringstream oss;
    oss << "C" << std::hex << std::setfill('0')
        << std::setw(2) << static_cast<int>(color.r * 255)
        << std::setw(2) << static_cast<int>(color.g * 255)
        << std::setw(2) << static_cast<int>(color.b * 255)
        << std::setw(2) << static_cast<int>(color.a * 255)
        << "_O" << std::fixed << std::setprecision(2) << opacity;
    
    if (hasTexture) {
        oss << "_T" << texturePath;
    }
    
    return oss.str();
}

std::shared_ptr<Material> MaterialPool::GetOrCreate(const Material& material) {
    std::lock_guard<std::mutex> lock(mutex_);
    totalRequests_++;

    std::string key = material.GetKey();
    auto it = materials_.find(key);
    
    if (it != materials_.end()) {
        // 找到现有材质
        duplicatesSaved_++;
        return it->second;
    }

    // 创建新材质
    auto newMaterial = std::make_shared<Material>(material);
    materials_[key] = newMaterial;
    return newMaterial;
}

std::shared_ptr<Material> MaterialPool::GetSolidColor(const Color& color, double opacity) {
    Material mat(color, opacity);
    return GetOrCreate(mat);
}

void MaterialPool::Cleanup() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // 移除只有一个引用（即池自己持有的引用）的材质
    auto it = materials_.begin();
    while (it != materials_.end()) {
        if (it->second.use_count() == 1) {
            it = materials_.erase(it);
        } else {
            ++it;
        }
    }
}

void MaterialPool::Clear() {
    std::lock_guard<std::mutex> lock(mutex_);
    materials_.clear();
}

size_t MaterialPool::GetMaterialCount() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return materials_.size();
}

MaterialPool::Stats MaterialPool::GetStats() const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    Stats stats;
    stats.totalMaterials = materials_.size();
    stats.uniqueMaterials = materials_.size();
    stats.duplicatesSaved = duplicatesSaved_;
    stats.totalRequests = totalRequests_;
    
    return stats;
}

} // namespace fk
