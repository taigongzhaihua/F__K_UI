#pragma once

#include <memory>
#include "fk/render/RenderCommandBuffer.h"

namespace fk::render {

class RenderList;

class RenderScene {
public:
    RenderScene();
    ~RenderScene();

    RenderScene(RenderScene&&) noexcept = default;
    RenderScene& operator=(RenderScene&&) noexcept = default;
    RenderScene(const RenderScene&) = delete;
    RenderScene& operator=(const RenderScene&) = delete;

    void Reset();

    RenderCommandBuffer& CommandBuffer() noexcept;
    const RenderCommandBuffer& CommandBuffer() const noexcept;

    /**
     * @brief 生成渲染列表（用于传递给渲染器）
     */
    std::unique_ptr<RenderList> GenerateRenderList();

private:
    std::unique_ptr<RenderCommandBuffer> commandBuffer_;
};

} // namespace fk::render
