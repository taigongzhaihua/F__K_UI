#pragma once

#include <memory>

namespace fk::render {

class RenderCommandBuffer;

/**
 * @brief 渲染列表 - 包含本帧所有渲染命令
 * 
 * RenderList是RenderScene构建完成后生成的最终渲染数据，
 * 传递给IRenderer进行实际绘制
 */
class RenderList {
public:
    RenderList() = default;
    ~RenderList() = default;

    /**
     * @brief 获取命令缓冲区（只读）
     */
    const RenderCommandBuffer* CommandBuffer() const { return commandBuffer_; }

    /**
     * @brief 设置命令缓冲区（接受原始指针，不获取所有权）
     */
    void SetCommandBuffer(const RenderCommandBuffer* buffer) {
        commandBuffer_ = buffer;
    }

    /**
     * @brief 检查是否为空
     */
    bool IsEmpty() const { return commandBuffer_ == nullptr; }

private:
    const RenderCommandBuffer* commandBuffer_ = nullptr;
};

} // namespace fk::render
