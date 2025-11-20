#include "fk/render/RenderList.h"
#include <algorithm>
#include <unordered_set>

namespace fk::render {

RenderList::RenderList() {
    // 预分配合理的初始容量
    commands_.reserve(256);
    batches_.reserve(32);
}

RenderList::~RenderList() {
    Clear();
}

void RenderList::AddCommand(const RenderCommand& command) {
    commands_.push_back(command);
    optimized_ = false;
}

void RenderList::AddCommand(RenderCommand&& command) {
    commands_.push_back(std::move(command));
    optimized_ = false;
}

void RenderList::AddCommands(const std::vector<RenderCommand>& commands) {
    commands_.insert(commands_.end(), commands.begin(), commands.end());
    optimized_ = false;
}

void RenderList::Clear() {
    commands_.clear();
    batches_.clear();
    stats_ = RenderListStats{};
    optimized_ = false;
}

void RenderList::Reserve(size_t capacity) {
    commands_.reserve(capacity);
}

void RenderList::Optimize() {
    if (optimized_ || commands_.empty()) {
        return;
    }
    
    // 记录优化前的命令数
    size_t beforeCount = commands_.size();
    
    // 1. 去除重复命令
    RemoveDuplicates();
    
    // 2. 构建命令批次
    BuildBatches();
    
    // 3. 更新统计信息
    stats_.duplicatesRemoved = beforeCount - commands_.size();
    UpdateStats();
    
    optimized_ = true;
}

// ========== 私有方法实现 ==========

void RenderList::BuildBatches() {
    batches_.clear();
    
    if (commands_.empty()) {
        return;
    }
    
    // 构建批次：将相同类型的连续命令分组
    CommandBatch currentBatch;
    currentBatch.startIndex = 0;
    currentBatch.count = 1;
    currentBatch.type = commands_[0].type;
    
    for (size_t i = 1; i < commands_.size(); ++i) {
        if (CanBatch(commands_[i-1], commands_[i])) {
            // 可以批处理，增加当前批次计数
            currentBatch.count++;
        } else {
            // 不能批处理，保存当前批次，开始新批次
            batches_.push_back(currentBatch);
            
            currentBatch.startIndex = i;
            currentBatch.count = 1;
            currentBatch.type = commands_[i].type;
        }
    }
    
    // 添加最后一个批次
    batches_.push_back(currentBatch);
}

void RenderList::RemoveDuplicates() {
    if (commands_.size() <= 1) {
        return;
    }
    
    // 使用稳定去重算法：保留第一次出现的命令
    std::vector<RenderCommand> uniqueCommands;
    uniqueCommands.reserve(commands_.size());
    
    for (const auto& cmd : commands_) {
        // 检查是否与最后添加的命令重复
        // 注意：只检查最近的命令，避免 O(n^2) 复杂度
        if (uniqueCommands.empty() || !IsDuplicate(uniqueCommands.back(), cmd)) {
            uniqueCommands.push_back(cmd);
        }
    }
    
    commands_ = std::move(uniqueCommands);
}

void RenderList::UpdateStats() {
    stats_.totalCommands = commands_.size();
    stats_.batchCount = batches_.size();
    
    // 估算内存使用
    stats_.memoryUsed = commands_.capacity() * sizeof(RenderCommand) +
                       batches_.capacity() * sizeof(CommandBatch);
}

bool RenderList::CanBatch(const RenderCommand& a, const RenderCommand& b) const {
    // 相同类型的命令可以批处理
    if (a.type != b.type) {
        return false;
    }
    
    // 特殊情况：SetClip 和 SetTransform 不应该批处理
    // 因为它们会影响后续的绘制状态
    if (a.type == CommandType::SetClip || 
        a.type == CommandType::SetTransform ||
        a.type == CommandType::PushLayer ||
        a.type == CommandType::PopLayer) {
        return false;
    }
    
    return true;
}

bool RenderList::IsDuplicate(const RenderCommand& a, const RenderCommand& b) const {
    // 类型不同，肯定不重复
    if (a.type != b.type) {
        return false;
    }
    
    // 对于状态命令，检查是否设置相同的状态
    switch (a.type) {
        case CommandType::SetTransform: {
            auto* payloadA = std::get_if<TransformPayload>(&a.payload);
            auto* payloadB = std::get_if<TransformPayload>(&b.payload);
            if (payloadA && payloadB) {
                return payloadA->offsetX == payloadB->offsetX &&
                       payloadA->offsetY == payloadB->offsetY;
            }
            break;
        }
        
        case CommandType::SetClip: {
            auto* payloadA = std::get_if<ClipPayload>(&a.payload);
            auto* payloadB = std::get_if<ClipPayload>(&b.payload);
            if (payloadA && payloadB) {
                return payloadA->enabled == payloadB->enabled &&
                       payloadA->clipRect.x == payloadB->clipRect.x &&
                       payloadA->clipRect.y == payloadB->clipRect.y &&
                       payloadA->clipRect.width == payloadB->clipRect.width &&
                       payloadA->clipRect.height == payloadB->clipRect.height;
            }
            break;
        }
        
        case CommandType::DrawRectangle: {
            auto* payloadA = std::get_if<RectanglePayload>(&a.payload);
            auto* payloadB = std::get_if<RectanglePayload>(&b.payload);
            if (payloadA && payloadB) {
                // 矩形绘制命令：位置、大小、填充色、描边以及圆角相同才算重复
                return payloadA->rect.x == payloadB->rect.x &&
                       payloadA->rect.y == payloadB->rect.y &&
                       payloadA->rect.width == payloadB->rect.width &&
                       payloadA->rect.height == payloadB->rect.height &&
                       payloadA->fillColor == payloadB->fillColor &&
                       payloadA->strokeColor == payloadB->strokeColor &&
                       payloadA->strokeThickness == payloadB->strokeThickness &&
                       payloadA->cornerRadiusTopLeft == payloadB->cornerRadiusTopLeft &&
                       payloadA->cornerRadiusTopRight == payloadB->cornerRadiusTopRight &&
                       payloadA->cornerRadiusBottomRight == payloadB->cornerRadiusBottomRight &&
                       payloadA->cornerRadiusBottomLeft == payloadB->cornerRadiusBottomLeft &&
                       payloadA->strokeAlignment == payloadB->strokeAlignment &&
                       payloadA->aaWidth == payloadB->aaWidth;
            }
            break;
        }
        
        // 其他绘制命令通常不重复（位置不同）
        default:
            return false;
    }
    
    return false;
}

} // namespace fk::render
