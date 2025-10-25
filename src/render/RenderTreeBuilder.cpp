#include "fk/render/RenderTreeBuilder.h"
#include "fk/render/RenderScene.h"
#include "fk/render/RenderCommandBuffer.h"
#include "fk/render/RenderCommand.h"

namespace fk::render {

void RenderTreeBuilder::Rebuild(const ui::Visual& visualRoot, RenderScene& scene) {
    // 重置偏移
    currentOffsetX_ = 0.0f;
    currentOffsetY_ = 0.0f;

    // 开始遍历
    TraverseVisual(visualRoot, scene, 1.0f);
}

void RenderTreeBuilder::TraverseVisual(const ui::Visual& visual, RenderScene& scene, float parentOpacity) {
    // 检查可见性
    if (visual.GetVisibility() != ui::Visibility::Visible) {
        return;
    }

    // 计算有效不透明度
    float effectiveOpacity = parentOpacity * visual.GetOpacity();
    if (effectiveOpacity <= 0.0f) {
        return; // 完全透明，跳过
    }

    // 获取渲染边界
    ui::Rect bounds = visual.GetRenderBounds();
    
    // 如果宽高为0，跳过
    if (bounds.width <= 0.0f || bounds.height <= 0.0f) {
        return;
    }

    // 保存当前偏移
    float savedOffsetX = currentOffsetX_;
    float savedOffsetY = currentOffsetY_;

    // 应用当前元素的位置偏移
    currentOffsetX_ += bounds.x;
    currentOffsetY_ += bounds.y;

    // 设置变换
    if (bounds.x != 0.0f || bounds.y != 0.0f) {
        TransformPayload transformPayload;
        transformPayload.offsetX = currentOffsetX_;
        transformPayload.offsetY = currentOffsetY_;
        scene.CommandBuffer().AddCommand(
            RenderCommand(CommandType::SetTransform, transformPayload)
        );
    }

    // 如果不透明度 < 1，需要推入图层
    bool needLayer = effectiveOpacity < 1.0f;
    if (needLayer) {
        LayerPayload layerPayload;
        layerPayload.opacity = effectiveOpacity;
        scene.CommandBuffer().AddCommand(
            RenderCommand(CommandType::PushLayer, layerPayload)
        );
    }

    // 生成元素自身的渲染内容
    if (visual.HasRenderContent()) {
        GenerateRenderContent(visual, scene, effectiveOpacity);
    }

    // 递归处理子元素
    auto children = visual.GetVisualChildren();
    for (auto* child : children) {
        if (child) {
            TraverseVisual(*child, scene, effectiveOpacity);
        }
    }

    // 弹出图层
    if (needLayer) {
        scene.CommandBuffer().AddCommand(
            RenderCommand(CommandType::PopLayer, std::monostate{})
        );
    }

    // 恢复偏移
    currentOffsetX_ = savedOffsetX;
    currentOffsetY_ = savedOffsetY;
}

void RenderTreeBuilder::GenerateRenderContent(const ui::Visual& visual, RenderScene& scene, float opacity) {
    // 获取渲染边界（相对于父元素）
    ui::Rect bounds = visual.GetRenderBounds();
    
    // 暂时生成一个简单的矩形（占位实现）
    // 实际应该根据元素的背景、边框等属性生成相应的绘制命令
    RectanglePayload rectPayload;
    rectPayload.rect = ui::Rect(0, 0, bounds.width, bounds.height); // 相对坐标
    rectPayload.color = {0.8f, 0.8f, 0.8f, opacity}; // 灰色占位
    rectPayload.cornerRadius = 0.0f;
    
    scene.CommandBuffer().AddCommand(
        RenderCommand(CommandType::DrawRectangle, rectPayload)
    );
}

} // namespace fk::render
