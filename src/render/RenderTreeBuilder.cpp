#include "fk/render/RenderTreeBuilder.h"
#include "fk/render/RenderScene.h"
#include "fk/render/RenderCommandBuffer.h"
#include "fk/render/RenderCommand.h"
#include "fk/render/ColorUtils.h"
#include "fk/ui/Button.h"  // 用于 ButtonBase 类型检查

#include <iostream>

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

    // 始终设置变换(即使偏移为0,确保状态正确)
    TransformPayload transformPayload;
    transformPayload.offsetX = currentOffsetX_;
    transformPayload.offsetY = currentOffsetY_;
    scene.CommandBuffer().AddCommand(
        RenderCommand(CommandType::SetTransform, transformPayload)
    );

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
    
    // 尝试将 Visual 转换为 ButtonBase
    const auto* button = dynamic_cast<const ui::detail::ButtonBase*>(&visual);
    if (button) {
        float borderThickness = button->GetBorderThickness();
        
        // 如果有边框,先绘制边框(作为底层)
        if (borderThickness > 0.0f) {
            RectanglePayload borderPayload;
            borderPayload.rect = ui::Rect(0, 0, bounds.width, bounds.height);
            borderPayload.color = ColorUtils::ParseColor(button->GetBorderBrush());
            borderPayload.color[3] *= opacity;
            borderPayload.cornerRadius = button->GetCornerRadius();
            
            scene.CommandBuffer().AddCommand(
                RenderCommand(CommandType::DrawRectangle, borderPayload)
            );
        }
        
        // 然后绘制背景(在边框之上,稍微缩小)
        if (borderThickness > 0.0f && borderThickness < bounds.width / 2 && borderThickness < bounds.height / 2) {
            // 有边框:绘制缩小的背景
            RectanglePayload bgPayload;
            bgPayload.rect = ui::Rect(
                borderThickness, 
                borderThickness,
                bounds.width - 2 * borderThickness,
                bounds.height - 2 * borderThickness
            );
            bgPayload.color = ColorUtils::ParseColor(button->GetActualBackground());
            bgPayload.color[3] *= opacity;
            bgPayload.cornerRadius = std::max(0.0f, button->GetCornerRadius() - borderThickness);
            
            scene.CommandBuffer().AddCommand(
                RenderCommand(CommandType::DrawRectangle, bgPayload)
            );
        } else {
            // 无边框:绘制完整背景
            RectanglePayload bgPayload;
            bgPayload.rect = ui::Rect(0, 0, bounds.width, bounds.height);
            bgPayload.color = ColorUtils::ParseColor(button->GetActualBackground());
            bgPayload.color[3] *= opacity;
            bgPayload.cornerRadius = button->GetCornerRadius();
            
            scene.CommandBuffer().AddCommand(
                RenderCommand(CommandType::DrawRectangle, bgPayload)
            );
        }
        
        return;
    }
    
    // 其他元素的默认渲染（占位实现）
    RectanglePayload rectPayload;
    rectPayload.rect = ui::Rect(0, 0, bounds.width, bounds.height); // 相对坐标
    rectPayload.color = {0.8f, 0.8f, 0.8f, opacity}; // 灰色占位
    rectPayload.cornerRadius = 0.0f;
    
    scene.CommandBuffer().AddCommand(
        RenderCommand(CommandType::DrawRectangle, rectPayload)
    );
}

} // namespace fk::render
