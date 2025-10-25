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

void RenderHost::RenderFrame(const FrameContext& frameContext, const ui::Visual& visualRoot) {
    if (!renderer_) {
        return;
    }

    if (!renderPending_) {
        return;
    }

    if (!scene_) {
        scene_ = std::make_unique<RenderScene>();
    }

    scene_->Reset();
    BuildScene(visualRoot);

    // 生成渲染列表并传递给渲染器
    auto renderList = scene_->GenerateRenderList();

    renderer_->BeginFrame(frameContext);
    renderer_->Draw(*renderList);
    renderer_->EndFrame();

    renderPending_ = false;
    
    // 渲染完成后清除失效追踪
    ClearDirtyElements();
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

void RenderHost::BuildScene(const ui::Visual& visualRoot) {
    if (treeBuilder_ && scene_) {
        treeBuilder_->Rebuild(visualRoot, *scene_);
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
    
    // 调试输出
    // std::cout << "Element invalidated, requesting render" << std::endl;
}

void RenderHost::ClearDirtyElements() {
    dirtyElements_.clear();
}

} // namespace fk::render
