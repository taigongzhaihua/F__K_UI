#include "fk/render/RenderTreeBuilder.h"
#include "fk/render/RenderScene.h"
#include "fk/render/RenderCommandBuffer.h"
#include "fk/render/RenderCommand.h"
#include "fk/render/ColorUtils.h"
#include "fk/render/IRenderer.h"
#include "fk/ui/Button.h"  // 用于 ButtonBase 类型检查
#include "fk/ui/TextBlock.h"  // 用于 TextBlockBase 类型检查
// #include "fk/ui/TextBox.h"  // TODO: 暂时注释，未实现
// #include "fk/ui/ScrollBar.h"  // TODO: 暂时注释，未实现
// #include "fk/ui/ScrollViewer.h"  // TODO: 暂时注释，未实现
#include "fk/ui/UIElement.h"  // 用于 ClipToBounds
#include <iostream>

#include <iostream>

namespace fk::render {

void RenderTreeBuilder::Rebuild(const ui::Visual& visualRoot, RenderScene& scene, const FrameContext& frameContext) {
    // 重置偏移
    currentOffsetX_ = 0.0f;
    currentOffsetY_ = 0.0f;
    currentFrameTime_ = frameContext.elapsedSeconds;

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

    // 检查是否需要裁切
    const auto* uiElement = dynamic_cast<const ui::UIElement*>(&visual);
    bool needClip = uiElement && uiElement->GetClipToBounds();
    
    if (needClip) {
        ClipPayload clipPayload;
        clipPayload.clipRect = ui::Rect(0, 0, bounds.width, bounds.height);  // 相对于当前元素
        clipPayload.enabled = true;
        scene.CommandBuffer().AddCommand(
            RenderCommand(CommandType::SetClip, clipPayload)
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

    // 处理 ScrollViewer 的滚动条
    const auto* scrollViewer = dynamic_cast<const ui::detail::ScrollViewerBase*>(&visual);
    if (scrollViewer) {
        // 渲染水平滚动条
        auto* hScrollBar = scrollViewer->GetHorizontalScrollBar();
        if (hScrollBar && hScrollBar->GetVisibility() == ui::Visibility::Visible) {
            TraverseVisual(*hScrollBar, scene, effectiveOpacity);
        }
        
        // 渲染垂直滚动条
        auto* vScrollBar = scrollViewer->GetVerticalScrollBar();
        if (vScrollBar && vScrollBar->GetVisibility() == ui::Visibility::Visible) {
            TraverseVisual(*vScrollBar, scene, effectiveOpacity);
        }
    }

    // 弹出图层
    if (needLayer) {
        scene.CommandBuffer().AddCommand(
            RenderCommand(CommandType::PopLayer, std::monostate{})
        );
    }
    
    // 禁用裁切
    if (needClip) {
        ClipPayload clipPayload;
        clipPayload.enabled = false;
        scene.CommandBuffer().AddCommand(
            RenderCommand(CommandType::SetClip, clipPayload)
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
    
    // 尝试将 Visual 转换为 TextBoxBase
    const auto* textBox = dynamic_cast<const ui::detail::TextBoxBase*>(&visual);
    if (textBox) {
        const_cast<ui::detail::TextBoxBase*>(textBox)->PrepareForRender(currentFrameTime_);
        const float borderThickness = std::max(0.0f, textBox->GetBorderThickness());

        if (borderThickness > 0.0f) {
            RectanglePayload borderPayload;
            borderPayload.rect = ui::Rect(0, 0, bounds.width, bounds.height);
            borderPayload.color = ColorUtils::ParseColor(textBox->GetBorderBrush());
            borderPayload.color[3] *= opacity;
            borderPayload.cornerRadius = 2.0f;
            scene.CommandBuffer().AddCommand(RenderCommand(CommandType::DrawRectangle, borderPayload));
        }

        RectanglePayload backgroundPayload;
        if (borderThickness > 0.0f && borderThickness * 2 < bounds.width && borderThickness * 2 < bounds.height) {
            backgroundPayload.rect = ui::Rect(
                borderThickness,
                borderThickness,
                bounds.width - borderThickness * 2.0f,
                bounds.height - borderThickness * 2.0f);
        } else {
            backgroundPayload.rect = ui::Rect(0, 0, bounds.width, bounds.height);
        }
        backgroundPayload.color = ColorUtils::ParseColor(textBox->GetBackground());
        backgroundPayload.color[3] *= opacity;
        backgroundPayload.cornerRadius = std::max(0.0f, 2.0f - borderThickness);
        scene.CommandBuffer().AddCommand(RenderCommand(CommandType::DrawRectangle, backgroundPayload));

        if (textBox->HasSelection()) {
            auto selectionRects = textBox->GetSelectionRects();
            for (const auto& rect : selectionRects) {
                if (rect.width > 0.0f && rect.height > 0.0f) {
                    RectanglePayload selectionPayload;
                    selectionPayload.rect = rect;
                    selectionPayload.color = ColorUtils::ParseColor("#3399FF55");
                    selectionPayload.color[3] *= opacity;
                    selectionPayload.cornerRadius = 0.0f;
                    scene.CommandBuffer().AddCommand(RenderCommand(CommandType::DrawRectangle, selectionPayload));
                }
            }
        }

        if (textBox->ShouldShowCaret()) {
            RectanglePayload caretPayload;
            caretPayload.rect = textBox->GetCaretRect();
            caretPayload.color = ColorUtils::ParseColor(textBox->GetForeground());
            caretPayload.color[3] *= opacity;
            caretPayload.cornerRadius = 0.0f;
            scene.CommandBuffer().AddCommand(RenderCommand(CommandType::DrawRectangle, caretPayload));
        }

        return;
    }

    // 尝试将 Visual 转换为 ScrollBarBase
    const auto* scrollBar = dynamic_cast<const ui::detail::ScrollBarBase*>(&visual);
    if (scrollBar) {
        // 绘制 Track (轨道)
        RectanglePayload trackPayload;
        trackPayload.rect = ui::Rect(0, 0, bounds.width, bounds.height);
        trackPayload.color = ColorUtils::ParseColor(scrollBar->GetTrackBrush());
        trackPayload.color[3] *= opacity;
        trackPayload.cornerRadius = 4.0f;  // 轻微圆角
        
        scene.CommandBuffer().AddCommand(
            RenderCommand(CommandType::DrawRectangle, trackPayload)
        );
        
        // 绘制 Thumb (滑块)
        ui::Rect thumbBounds = scrollBar->GetThumbBounds();
        if (thumbBounds.width > 0 && thumbBounds.height > 0) {
            RectanglePayload thumbPayload;
            thumbPayload.rect = thumbBounds;  // 已经是相对于 ScrollBar 的坐标
            thumbPayload.color = ColorUtils::ParseColor(scrollBar->GetThumbBrush());
            thumbPayload.color[3] *= opacity;
            thumbPayload.cornerRadius = 4.0f;  // 圆角滑块
            
            scene.CommandBuffer().AddCommand(
                RenderCommand(CommandType::DrawRectangle, thumbPayload)
            );
        }
        
        return;
    }
    
    // 尝试将 Visual 转换为 TextBlockBase
    const auto* textBlock = dynamic_cast<const ui::detail::TextBlockBase*>(&visual);
    if (textBlock) {
        // 生成文本渲染命令
        TextPayload textPayload;
        textPayload.bounds = ui::Rect(0, 0, bounds.width, bounds.height);
        textPayload.color = ColorUtils::ParseColor(textBlock->GetForeground());
        textPayload.color[3] *= opacity;
        
        // 使用经过处理的文本(可能包含截断)
        const auto& wrappedLines = textBlock->GetWrappedLines();
        if (!wrappedLines.empty() && wrappedLines.size() == 1) {
            // 单行文本(可能被截断),使用处理后的文本
            textPayload.text = wrappedLines[0];
        } else {
            // 多行文本或没有处理过,使用原始文本
            // 让渲染器根据 textWrapping 和 maxWidth 自动换行
            textPayload.text = textBlock->GetText();
        }
        
        textPayload.fontSize = textBlock->GetFontSize();
        textPayload.fontFamily = textBlock->GetFontFamily();
        textPayload.fontId = 0;  // TODO: 通过 fontFamily 和 fontSize 获取实际 fontId
        textPayload.textWrapping = (textBlock->GetTextWrapping() == ui::TextWrapping::Wrap);
        textPayload.maxWidth = bounds.width;
        
        scene.CommandBuffer().AddCommand(
            RenderCommand(CommandType::DrawText, textPayload)
        );
        
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
