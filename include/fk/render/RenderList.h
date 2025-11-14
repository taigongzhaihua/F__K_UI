#pragma once

#include "fk/render/RenderCommand.h"
#include <vector>
#include <memory>
#include <cstddef>

namespace fk::render {

class RenderCommandBuffer;

/**
 * @brief 命令批次信息
 */
struct CommandBatch {
    size_t startIndex{0};     // 批次起始命令索引
    size_t count{0};          // 批次命令数量
    CommandType type;         // 批次命令类型
};

/**
 * @brief 渲染列表统计信息
 */
struct RenderListStats {
    size_t totalCommands{0};      // 总命令数
    size_t batchCount{0};         // 批次数量
    size_t duplicatesRemoved{0};  // 去重的命令数
    size_t memoryUsed{0};         // 内存使用（字节）
};

/**
 * @brief 渲染列表 - 包含本帧所有渲染命令
 * 
 * Phase 5.0.2 增强版：
 * - 直接存储 RenderCommand
 * - 命令批处理优化
 * - 命令去重
 * - 内存池管理
 * - 性能统计
 */
class RenderList {
public:
    RenderList();
    ~RenderList();

    /**
     * @brief 添加渲染命令
     */
    void AddCommand(const RenderCommand& command);
    
    /**
     * @brief 添加渲染命令（移动语义）
     */
    void AddCommand(RenderCommand&& command);
    
    /**
     * @brief 批量添加命令
     */
    void AddCommands(const std::vector<RenderCommand>& commands);

    /**
     * @brief 获取所有命令（只读）
     */
    const std::vector<RenderCommand>& GetCommands() const { return commands_; }
    
    /**
     * @brief 获取命令批次（只读）
     */
    const std::vector<CommandBatch>& GetBatches() const { return batches_; }

    /**
     * @brief 清空所有命令
     */
    void Clear();
    
    /**
     * @brief 检查是否为空
     */
    bool IsEmpty() const { return commands_.empty(); }
    
    /**
     * @brief 获取命令数量
     */
    size_t GetCommandCount() const { return commands_.size(); }

    /**
     * @brief 优化命令列表（批处理、去重）
     * 
     * 应在所有命令添加完成后调用
     */
    void Optimize();
    
    /**
     * @brief 获取统计信息
     */
    const RenderListStats& GetStats() const { return stats_; }
    
    /**
     * @brief 预分配内存
     * @param capacity 预分配的命令容量
     */
    void Reserve(size_t capacity);

    // ========== 兼容旧接口 ==========
    
    /**
     * @brief 获取命令缓冲区（只读）- 旧接口兼容
     */
    const RenderCommandBuffer* CommandBuffer() const { return commandBuffer_; }

    /**
     * @brief 设置命令缓冲区（接受原始指针，不获取所有权）- 旧接口兼容
     */
    void SetCommandBuffer(const RenderCommandBuffer* buffer) {
        commandBuffer_ = buffer;
    }

private:
    /**
     * @brief 构建命令批次
     */
    void BuildBatches();
    
    /**
     * @brief 去除重复命令
     */
    void RemoveDuplicates();
    
    /**
     * @brief 更新统计信息
     */
    void UpdateStats();
    
    /**
     * @brief 检查两个命令是否可以批处理
     */
    bool CanBatch(const RenderCommand& a, const RenderCommand& b) const;
    
    /**
     * @brief 检查两个命令是否重复
     */
    bool IsDuplicate(const RenderCommand& a, const RenderCommand& b) const;

private:
    std::vector<RenderCommand> commands_;       // 命令列表
    std::vector<CommandBatch> batches_;         // 命令批次
    RenderListStats stats_;                     // 统计信息
    
    // 旧接口兼容
    const RenderCommandBuffer* commandBuffer_{nullptr};
    
    // 优化标志
    bool optimized_{false};
};

} // namespace fk::render
