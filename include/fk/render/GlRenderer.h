#pragma once

#include "fk/render/IRenderer.h"
#include <memory>
#include <unordered_map>
#include <vector>

namespace fk::render {

class RenderList;
class TextRenderer;

/**
 * @brief OpenGL 渲染器实现
 */
class GlRenderer : public IRenderer {
public:
    GlRenderer();
    ~GlRenderer() override;

    // IRenderer 接口实现
    void Initialize(const RendererInitParams& params) override;
    void Resize(const Extent2D& size) override;
    void BeginFrame(const FrameContext& ctx) override;
    void Draw(const RenderList& list) override;
    void EndFrame() override;
    void Shutdown() override;

    /**
     * @brief 获取 TextRenderer 实例(用于文本测量)
     */
    TextRenderer* GetTextRenderer() const { return textRenderer_.get(); }
    
    /**
     * @brief 检查渲染器是否已初始化
     */
    bool IsInitialized() const { return initialized_; }

private:
    /**
     * @brief 执行单个渲染命令
     */
    void ExecuteCommand(const struct RenderCommand& cmd);

    /**
     * @brief 设置裁剪区域
     */
    void ApplyClip(const struct ClipPayload& payload);

    /**
     * @brief 设置变换矩阵
     */
    void ApplyTransform(const struct TransformPayload& payload);

    /**
     * @brief 绘制矩形
     */
    void DrawRectangle(const struct RectanglePayload& payload);

    /**
     * @brief 绘制文本（占位）
     */
    void DrawText(const struct TextPayload& payload);

    /**
     * @brief 绘制图像（占位）
     */
    void DrawImage(const struct ImagePayload& payload);

    /**
     * @brief 绘制多边形
     */
    void DrawPolygon(const struct PolygonPayload& payload);

    /**
     * @brief 绘制路径
     */
    void DrawPath(const struct PathPayload& payload);

    /**
     * @brief 推入透明度图层
     */
    void PushLayer(const struct LayerPayload& payload);

    /**
     * @brief 弹出透明度图层
     */
    void PopLayer();

    /**
     * @brief 初始化着色器程序
     */
    void InitializeShaders();

    /**
     * @brief 初始化顶点缓冲区
     */
    void InitializeBuffers();

    /**
     * @brief 清理资源
     */
    void CleanupResources();

    // OpenGL 资源
    unsigned int borderShaderProgram_{0};     // Border 着色器（圆形圆角）
    unsigned int rectangleShaderProgram_{0};  // Rectangle 着色器（椭圆圆角）
    unsigned int simpleShaderProgram_{0};     // 简单着色器（无SDF，用于多边形）
    unsigned int pathAAShaderProgram_{0};     // Path抗锯齿着色器（边缘羽化）
    unsigned int textShaderProgram_{0};       // 文本渲染着色器
    unsigned int vao_{0};
    unsigned int vbo_{0};
    unsigned int pathAAVAO_{0};  // Path抗锯齿 VAO
    unsigned int pathAAVBO_{0};  // Path抗锯齿 VBO
    unsigned int textVAO_{0};  // 文本渲染 VAO
    unsigned int textVBO_{0};  // 文本渲染 VBO
    
    // 文本渲染器
    std::unique_ptr<TextRenderer> textRenderer_;
    
    // 状态
    Extent2D viewportSize_{};
    FrameContext currentFrame_{};
    
    // 变换栈
    float currentOffsetX_{0.0f};
    float currentOffsetY_{0.0f};
    
    // 透明度栈
    struct LayerState {
        float opacity{1.0f};
    };
    std::vector<LayerState> layerStack_;
    
    bool initialized_{false};
};

} // namespace fk::render
