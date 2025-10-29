#include "fk/render/RenderScene.h"
#include "fk/render/RenderList.h"

#include <utility>

namespace fk::render {

RenderScene::RenderScene()
    : commandBuffer_(std::make_unique<RenderCommandBuffer>()) {
}

RenderScene::~RenderScene() = default;

void RenderScene::Reset() {
    if (commandBuffer_) {
        commandBuffer_->Clear();
    }
}

RenderCommandBuffer& RenderScene::CommandBuffer() noexcept {
    return *commandBuffer_;
}

const RenderCommandBuffer& RenderScene::CommandBuffer() const noexcept {
    return *commandBuffer_;
}

std::unique_ptr<RenderList> RenderScene::GenerateRenderList() {
    auto list = std::make_unique<RenderList>();
    list->SetCommandBuffer(commandBuffer_.get()); // 传递原始指针
    return list;
}

} // namespace fk::render
