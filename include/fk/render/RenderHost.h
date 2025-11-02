#pragma once

#include <memory>
#include <unordered_set>

#include "fk/render/IRenderer.h"

namespace fk::ui {
class Visual;
class Window;
class UIElement;
}

namespace fk::render {

class RenderScene;
class RenderTreeBuilder;

struct RenderHostConfig {
    std::shared_ptr<ui::Window> window;
    std::unique_ptr<IRenderer> renderer;
};

class RenderHost {
public:
    explicit RenderHost(RenderHostConfig config);
    ~RenderHost();

    RenderHost(RenderHost&&) noexcept = delete;
    RenderHost& operator=(RenderHost&&) noexcept = delete;
    RenderHost(const RenderHost&) = delete;
    RenderHost& operator=(const RenderHost&) = delete;

    void Initialize(const RendererInitParams& params);
    void RequestRender();
    bool RenderFrame(const FrameContext& frameContext, const ui::Visual& visualRoot);
    void Resize(const Extent2D& newSize);
    void Shutdown();

    /**
     * @brief 标记某个元素需要重绘（自动失效追踪）
     * @param element 需要重绘的元素
     * 
     * 当元素调用 InvalidateVisual() 时会调用此方法。
     * RenderHost 会记录所有失效的元素，并在下一帧统一重绘。
     */
    void InvalidateElement(ui::UIElement* element);
    
    /**
     * @brief 清除失效追踪（通常在渲染完成后调用）
     */
    void ClearDirtyElements();

    [[nodiscard]] bool IsRenderPending() const noexcept { return renderPending_; }
    [[nodiscard]] bool HasDirtyElements() const noexcept { return !dirtyElements_.empty(); }

private:
    void BuildScene(const ui::Visual& visualRoot);

    std::unique_ptr<IRenderer> renderer_;
    std::unique_ptr<RenderTreeBuilder> treeBuilder_;
    std::unique_ptr<RenderScene> scene_;
    std::weak_ptr<ui::Window> window_;
    bool renderPending_{false};
    
    // 失效追踪：记录需要重绘的元素
    std::unordered_set<ui::UIElement*> dirtyElements_;
};

} // namespace fk::render
