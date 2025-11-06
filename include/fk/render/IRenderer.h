#pragma once

#include <array>
#include <cstdint>
#include <string>

namespace fk::render {

struct Extent2D {
    std::uint32_t width{0};
    std::uint32_t height{0};
};

struct RendererInitParams {
    void* nativeSurfaceHandle{nullptr};
    Extent2D initialSize{};
    float pixelRatio{1.0f};
    bool enableDebugLayer{false};
    std::string rendererName;
};

struct FrameContext {
    double elapsedSeconds{0.0};
    double deltaSeconds{0.0};
    std::array<float, 4> clearColor{0.0f, 0.0f, 0.0f, 0.0f};
    std::uint64_t frameIndex{0};
};

class RenderList;
class TextRenderer;

class IRenderer {
public:
    virtual ~IRenderer() = default;

    virtual void Initialize(const RendererInitParams& params) = 0;
    virtual void Resize(const Extent2D& size) = 0;
    virtual void BeginFrame(const FrameContext& ctx) = 0;
    virtual void Draw(const RenderList& list) = 0;
    virtual void EndFrame() = 0;
    virtual void Shutdown() = 0;
    
    /**
     * @brief 获取 TextRenderer 实例(用于文本测量等)
     * @return TextRenderer 指针,如果渲染器不支持则返回 nullptr
     */
    virtual TextRenderer* GetTextRenderer() const { return nullptr; }
};

} // namespace fk::render
