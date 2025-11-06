#include "fk/render/RenderHost.h"

#include <stdexcept>
#include <iostream>

#include "fk/render/IRenderer.h"
#include "fk/render/RenderScene.h"
#include "fk/render/RenderList.h"
#include "fk/render/RenderTreeBuilder.h"
#include "fk/ui/UIElement.h"

namespace fk::render {

RenderHost::RenderHost(RenderHostConfig config)
    : renderer_(std::move(config.renderer))
    , treeBuilder_(std::make_unique<RenderTreeBuilder>())
    , window_(config.window)
    , scene_() {
}

RenderHost::~RenderHost() = default;

void RenderHost::Initialize(const RendererInitParams& params) {
    if (!renderer_) {
        throw std::runtime_error("RenderHost requires a renderer");
    }

    renderer_->Initialize(params);
}

void RenderHost::RequestRender() {
    renderPending_ = true;
}

bool RenderHost::RenderFrame(const FrameContext& frameContext, const ui::Visual& visualRoot) {
    if (!renderer_) {
        return false;
    }

    if (!renderPending_) {
        return false;
    }

    // 消耗当前的渲染请求，但允许在构建过程中再次请求下一帧
    renderPending_ = false;

    if (!scene_) {
        scene_ = std::make_unique<RenderScene>();
    }

    scene_->Reset();
    BuildScene(visualRoot, frameContext);

    // 在构建过程中可能又产生了新的渲染请求（例如动画驱动），记录下来
    const bool requestAnotherFrame = renderPending_;

    // 生成渲染列表并传递给渲染器
    auto renderList = scene_->GenerateRenderList();

    renderer_->BeginFrame(frameContext);
    renderer_->Draw(*renderList);
    renderer_->EndFrame();

    // 如果在渲染过程中出现新的请求，保持挂起状态以便下一帧继续渲染
    renderPending_ = requestAnotherFrame;
    
    // 渲染完成后清除失效追踪
    ClearDirtyElements();
    
    return true;  // 返回 true 表示真的渲染了
}

void RenderHost::Resize(const Extent2D& newSize) {
    if (renderer_) {
        renderer_->Resize(newSize);
    }
}

void RenderHost::Shutdown() {
    if (renderer_) {
        renderer_->Shutdown();
    }
}

void RenderHost::BuildScene(const ui::Visual& visualRoot, const FrameContext& frameContext) {
    if (treeBuilder_ && scene_) {
        treeBuilder_->Rebuild(visualRoot, *scene_, frameContext);
    }
}

void RenderHost::InvalidateElement(ui::UIElement* element) {
    if (!element) {
        return;
    }
    
    // 将元素标记为脏
    dirtyElements_.insert(element);
    
    // 自动请求渲染
    RequestRender();
}

void RenderHost::ClearDirtyElements() {
    dirtyElements_.clear();
}

TextRenderer* RenderHost::GetTextRenderer() const {
    if (renderer_) {
        return renderer_->GetTextRenderer();
    }
    return nullptr;
}

} // namespace fk::render
