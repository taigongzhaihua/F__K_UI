#pragma once

#include <vector>
#include <memory>
#include <functional>
#include "../ui/DrawCommand.h"

namespace fk {

using Color = fk::ui::Color;
using Rect = fk::ui::Rect;

/**
 * @brief 渲染批次项
 */
struct RenderBatchItem {
    Rect bounds;
    Color color;
    double opacity;
    void* userData;  // 用户数据指针

    RenderBatchItem()
        : color(Color::FromRGB(255, 255, 255))
        , opacity(1.0)
        , userData(nullptr)
    {
    }
};

/**
 * @brief 渲染批次
 */
struct RenderBatch {
    std::vector<RenderBatchItem> items;
    std::string materialKey;  // 材质键（用于批处理相同材质的对象）
    size_t vertexCount;
    size_t indexCount;

    RenderBatch()
        : vertexCount(0)
        , indexCount(0)
    {
    }
};

/**
 * @brief 渲染批处理器，用于合并多个渲染命令以减少绘制调用
 * 
 * 特性：
 * - 按材质分组
 * - 自动合并相邻的相同材质对象
 * - 统计信息
 */
class RenderBatcher {
public:
    RenderBatcher();

    /**
     * @brief 开始新的批处理帧
     */
    void BeginFrame();

    /**
     * @brief 添加渲染项
     * @param item 渲染项
     * @param materialKey 材质键
     */
    void AddItem(const RenderBatchItem& item, const std::string& materialKey);

    /**
     * @brief 结束批处理帧并生成批次
     */
    void EndFrame();

    /**
     * @brief 获取生成的批次列表
     */
    const std::vector<RenderBatch>& GetBatches() const {
        return batches_;
    }

    /**
     * @brief 遍历所有批次
     */
    void ForEachBatch(std::function<void(const RenderBatch&)> callback) const;

    /**
     * @brief 清空批次数据
     */
    void Clear();

    /**
     * @brief 设置批次大小阈值
     */
    void SetBatchSizeThreshold(size_t threshold) {
        batchSizeThreshold_ = threshold;
    }

    /**
     * @brief 获取统计信息
     */
    struct Stats {
        size_t totalItems;          // 总渲染项数
        size_t totalBatches;        // 总批次数
        size_t itemsPerBatch;       // 平均每批次项数
        size_t drawCallsSaved;      // 节省的绘制调用数
        double batchingEfficiency;  // 批处理效率（0-1）
    };

    Stats GetStats() const;

private:
    void FlushCurrentBatch();

    std::vector<RenderBatch> batches_;
    RenderBatch currentBatch_;
    std::string currentMaterialKey_;
    size_t batchSizeThreshold_;

    // 统计信息
    size_t totalItems_;
    size_t totalItemsBeforeBatching_;
};

} // namespace fk
