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
    const RenderCommandBuffer* CommandBuffer() const { return commandBuffer_.get(); }

    /**
     * @brief 设置命令缓冲区
     */
    void SetCommandBuffer(std::unique_ptr<RenderCommandBuffer> buffer) {
        commandBuffer_ = std::move(buffer);
    }

    /**
     * @brief 检查是否为空
     */
    bool IsEmpty() const { return commandBuffer_ == nullptr; }

private:
    std::unique_ptr<RenderCommandBuffer> commandBuffer_;
};

} // namespace fk::render
