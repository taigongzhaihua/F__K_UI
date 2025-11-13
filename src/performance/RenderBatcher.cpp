#include "fk/performance/RenderBatcher.h"

namespace fk {

RenderBatcher::RenderBatcher()
    : batchSizeThreshold_(100)
    , totalItems_(0)
    , totalItemsBeforeBatching_(0)
{
}

void RenderBatcher::BeginFrame() {
    Clear();
    totalItemsBeforeBatching_ = 0;
}

void RenderBatcher::AddItem(const RenderBatchItem& item, const std::string& materialKey) {
    totalItems_++;
    totalItemsBeforeBatching_++;

    // 如果材质改变或批次过大，刷新当前批次
    if (materialKey != currentMaterialKey_ || 
        currentBatch_.items.size() >= batchSizeThreshold_) {
        FlushCurrentBatch();
        currentMaterialKey_ = materialKey;
    }

    // 添加到当前批次
    currentBatch_.items.push_back(item);
    currentBatch_.materialKey = materialKey;
    
    // 更新顶点和索引计数（假设每个矩形 4 个顶点，6 个索引）
    currentBatch_.vertexCount += 4;
    currentBatch_.indexCount += 6;
}

void RenderBatcher::EndFrame() {
    FlushCurrentBatch();
}

void RenderBatcher::ForEachBatch(std::function<void(const RenderBatch&)> callback) const {
    for (const auto& batch : batches_) {
        callback(batch);
    }
}

void RenderBatcher::Clear() {
    batches_.clear();
    currentBatch_ = RenderBatch();
    currentMaterialKey_.clear();
}

RenderBatcher::Stats RenderBatcher::GetStats() const {
    Stats stats;
    stats.totalItems = totalItems_;
    stats.totalBatches = batches_.size();
    stats.itemsPerBatch = batches_.empty() ? 0 : totalItems_ / batches_.size();
    stats.drawCallsSaved = totalItemsBeforeBatching_ > batches_.size() ? 
        totalItemsBeforeBatching_ - batches_.size() : 0;
    stats.batchingEfficiency = totalItemsBeforeBatching_ > 0 ?
        1.0 - (static_cast<double>(batches_.size()) / totalItemsBeforeBatching_) : 0.0;
    
    return stats;
}

void RenderBatcher::FlushCurrentBatch() {
    if (!currentBatch_.items.empty()) {
        batches_.push_back(currentBatch_);
        currentBatch_ = RenderBatch();
    }
}

} // namespace fk
