#include "fk/render/RenderContext.h"
#include "fk/render/RenderList.h"
#include "fk/render/TextRenderer.h"
#include <algorithm>
#include <cmath>
#include <iostream>

namespace fk::render {

RenderContext::RenderContext(RenderList* renderList, TextRenderer* textRenderer)
    : renderList_(renderList)
    , textRenderer_(textRenderer)
    , currentOpacity_(1.0f)
{
    // 初始化默认状态
    currentTransform_ = TransformState{};
    currentClip_ = ClipState{ui::Rect{0, 0, 10000, 10000}, false}; // 无裁剪
}

// ========== 变换管理 ==========

void RenderContext::PushTransform(float offsetX, float offsetY) {
    // 保存当前状态
    transformStack_.push(currentTransform_);
    
    // 累积变换
    currentTransform_.offsetX += offsetX;
    currentTransform_.offsetY += offsetY;
    
    // 应用变换到渲染命令
    ApplyCurrentTransform();
}

void RenderContext::PopTransform() {
    if (!transformStack_.empty()) {
        currentTransform_ = transformStack_.top();
        transformStack_.pop();
        
        // 恢复之前的变换
        ApplyCurrentTransform();
    }
}

TransformState RenderContext::GetCurrentTransform() const {
    return currentTransform_;
}

ui::Point RenderContext::TransformPoint(const ui::Point& point) const {
    return ui::Point{
        point.x + currentTransform_.offsetX,
        point.y + currentTransform_.offsetY
    };
}

ui::Rect RenderContext::TransformRect(const ui::Rect& rect) const {
    return ui::Rect{
        rect.x + currentTransform_.offsetX,
        rect.y + currentTransform_.offsetY,
        rect.width,
        rect.height
    };
}

// ========== 裁剪管理 ==========

void RenderContext::PushClip(const ui::Rect& clipRect) {
    // 保存当前状态
    clipStack_.push(currentClip_);
    
    // 变换裁剪矩形到全局坐标
    ui::Rect globalClip = TransformRect(clipRect);
    
    // 与当前裁剪求交集
    if (currentClip_.enabled) {
        float x1 = std::max(currentClip_.clipRect.x, globalClip.x);
        float y1 = std::max(currentClip_.clipRect.y, globalClip.y);
        float x2 = std::min(
            currentClip_.clipRect.x + currentClip_.clipRect.width,
            globalClip.x + globalClip.width
        );
        float y2 = std::min(
            currentClip_.clipRect.y + currentClip_.clipRect.height,
            globalClip.y + globalClip.height
        );
        
        currentClip_.clipRect = ui::Rect{x1, y1, std::max(0.0f, x2 - x1), std::max(0.0f, y2 - y1)};
    } else {
        currentClip_.clipRect = globalClip;
    }
    
    currentClip_.enabled = true;
    
    // 应用裁剪到渲染命令
    ApplyCurrentClip();
}

void RenderContext::PopClip() {
    if (!clipStack_.empty()) {
        currentClip_ = clipStack_.top();
        clipStack_.pop();
        
        // 恢复之前的裁剪
        ApplyCurrentClip();
    }
}

ui::Rect RenderContext::GetCurrentClip() const {
    return currentClip_.clipRect;
}

bool RenderContext::IsClipped(const ui::Rect& rect) const {
    if (!currentClip_.enabled) {
        return false;
    }
    
    // 变换矩形到全局坐标
    ui::Rect globalRect = TransformRect(rect);
    
    // 检查是否完全在裁剪区域外
    if (globalRect.x + globalRect.width <= currentClip_.clipRect.x ||
        globalRect.x >= currentClip_.clipRect.x + currentClip_.clipRect.width ||
        globalRect.y + globalRect.height <= currentClip_.clipRect.y ||
        globalRect.y >= currentClip_.clipRect.y + currentClip_.clipRect.height) {
        return true;
    }
    
    return false;
}

// ========== 图层管理 ==========

void RenderContext::PushLayer(float opacity) {
    // 保存当前状态
    layerStack_.push(LayerState{currentOpacity_});
    
    // 累积透明度
    currentOpacity_ *= opacity;
    
    // 生成 PushLayer 命令
    if (renderList_) {
        LayerPayload payload;
        payload.opacity = opacity;
        renderList_->AddCommand(RenderCommand(CommandType::PushLayer, payload));
    }
}

void RenderContext::PopLayer() {
    if (!layerStack_.empty()) {
        currentOpacity_ = layerStack_.top().opacity;
        layerStack_.pop();
        
        // 生成 PopLayer 命令
        if (renderList_) {
            renderList_->AddCommand(RenderCommand(CommandType::PopLayer, std::monostate{}));
        }
    }
}

float RenderContext::GetCurrentOpacity() const {
    return currentOpacity_;
}

// ========== 绘制 API ==========

void RenderContext::DrawBorder(
    const ui::Rect& rect,
    const std::array<float, 4>& fillColor,
    const std::array<float, 4>& strokeColor,
    float strokeWidth,
    float cornerRadiusTopLeft,
    float cornerRadiusTopRight,
    float cornerRadiusBottomRight,
    float cornerRadiusBottomLeft,
    StrokeAlignment strokeAlignment,
    float aaWidth)
{
    // 检查是否被裁剪
    if (IsClipped(rect)) {
        return;
    }
    
    if (!renderList_) {
        return;
    }
    
    // 变换到全局坐标
    ui::Rect globalRect = TransformRect(rect);
    
    // 应用透明度
    auto finalFillColor = ApplyOpacity(fillColor);
    auto finalStrokeColor = ApplyOpacity(strokeColor);
    
    // 生成绘制命令
    RectanglePayload payload;
    payload.rect = globalRect;
    payload.fillColor = finalFillColor;
    payload.strokeColor = finalStrokeColor;
    payload.strokeThickness = strokeWidth;
    payload.cornerRadiusTopLeft = cornerRadiusTopLeft;
    payload.cornerRadiusTopRight = cornerRadiusTopRight;
    payload.cornerRadiusBottomRight = cornerRadiusBottomRight;
    payload.cornerRadiusBottomLeft = cornerRadiusBottomLeft;
    payload.strokeAlignment = strokeAlignment;
    payload.aaWidth = aaWidth;
    
    renderList_->AddCommand(RenderCommand(CommandType::DrawRectangle, payload));
}

void RenderContext::DrawRectangle(
    const ui::Rect& rect,
    const std::array<float, 4>& fillColor,
    const std::array<float, 4>& strokeColor,
    float strokeWidth,
    float radiusX,
    float radiusY,
    StrokeAlignment strokeAlignment,
    float aaWidth)
{
    // 检查是否被裁剪
    if (IsClipped(rect)) {
        return;
    }
    
    if (!renderList_) {
        return;
    }
    
    // 变换到全局坐标
    ui::Rect globalRect = TransformRect(rect);
    
    // 应用透明度
    auto finalFillColor = ApplyOpacity(fillColor);
    auto finalStrokeColor = ApplyOpacity(strokeColor);
    
    // 生成绘制命令 - 使用椭圆圆角
    RectanglePayload payload;
    payload.rect = globalRect;
    payload.fillColor = finalFillColor;
    payload.strokeColor = finalStrokeColor;
    payload.strokeThickness = strokeWidth;
    payload.cornerRadiusTopLeft = 0.0f;   // 不使用独立圆角
    payload.cornerRadiusTopRight = 0.0f;
    payload.cornerRadiusBottomRight = 0.0f;
    payload.cornerRadiusBottomLeft = 0.0f;
    payload.radiusX = radiusX;            // 使用椭圆圆角
    payload.radiusY = radiusY;
    payload.strokeAlignment = strokeAlignment;
    payload.aaWidth = aaWidth;
    
    renderList_->AddCommand(RenderCommand(CommandType::DrawRectangle, payload));
}

void RenderContext::DrawText(
    const ui::Rect& bounds,
    const std::string& text,
    const std::array<float, 4>& color,
    float fontSize,
    const std::string& fontFamily,
    float maxWidth,
    bool textWrapping)
{
    if (text.empty() || !renderList_) {
        return;
    }
    
    // 检查是否被裁剪
    if (IsClipped(bounds)) {
        return;
    }
    
    // 变换到全局坐标
    ui::Rect globalBounds = TransformRect(bounds);
    
    // 应用透明度
    auto finalColor = ApplyOpacity(color);
    
    // Phase 5.0.5: 生成完整的文本绘制命令,包含边界用于裁剪
    TextPayload payload;
    payload.bounds = globalBounds; // 使用完整边界
    payload.color = finalColor;
    payload.text = text;
    payload.fontSize = fontSize;
    payload.fontFamily = fontFamily;
    payload.maxWidth = maxWidth > 0 ? maxWidth : globalBounds.width; // 如果没指定maxWidth则使用bounds宽度
    payload.textWrapping = textWrapping;
    
    renderList_->AddCommand(RenderCommand(CommandType::DrawText, payload));
}

void RenderContext::DrawEllipse(
    const ui::Rect& bounds,
    const std::array<float, 4>& fillColor,
    const std::array<float, 4>& strokeColor,
    float strokeWidth)
{
    // 检查是否被裁剪
    if (IsClipped(bounds)) {
        return;
    }
    
    if (!renderList_) {
        return;
    }
    
    // 椭圆就是圆角矩形，其中 radiusX 和 radiusY 等于矩形宽高的一半
    // 这样可以使用高质量的 SDF 着色器绘制，而不是多边形近似
    DrawRectangle(
        bounds,
        fillColor,
        strokeColor,
        strokeWidth,
        bounds.width / 2.0f,  // radiusX = 宽度的一半
        bounds.height / 2.0f  // radiusY = 高度的一半
    );
}

void RenderContext::DrawLine(
    const ui::Point& start,
    const ui::Point& end,
    const std::array<float, 4>& color,
    float width)
{
    if (!renderList_) {
        return;
    }
    
    // 变换到全局坐标
    ui::Point globalStart = TransformPoint(start);
    ui::Point globalEnd = TransformPoint(end);
    
    // 应用透明度
    auto finalColor = ApplyOpacity(color);
    
    // 使用多边形绘制线条（2个点）
    PolygonPayload payload;
    payload.points = {globalStart, globalEnd};
    payload.strokeColor = finalColor;
    payload.strokeThickness = width;
    payload.filled = false;
    
    renderList_->AddCommand(RenderCommand(CommandType::DrawPolygon, payload));
}

void RenderContext::DrawPolygon(
    const std::vector<ui::Point>& points,
    const std::array<float, 4>& fillColor,
    const std::array<float, 4>& strokeColor,
    float strokeWidth)
{
    if (points.empty() || !renderList_) {
        return;
    }
    
    // 变换所有点到全局坐标
    std::vector<ui::Point> globalPoints;
    globalPoints.reserve(points.size());
    for (const auto& point : points) {
        globalPoints.push_back(TransformPoint(point));
    }
    
    // 应用透明度
    auto finalFillColor = ApplyOpacity(fillColor);
    auto finalStrokeColor = ApplyOpacity(strokeColor);
    
    // 生成绘制命令
    PolygonPayload payload;
    payload.points = globalPoints;
    payload.fillColor = finalFillColor;
    payload.strokeColor = finalStrokeColor;
    payload.strokeThickness = strokeWidth;
    payload.filled = true;
    
    renderList_->AddCommand(RenderCommand(CommandType::DrawPolygon, payload));
}

void RenderContext::DrawPath(
    const std::vector<PathSegment>& segments,
    const std::array<float, 4>& fillColor,
    const std::array<float, 4>& strokeColor,
    float strokeWidth)
{
    if (segments.empty() || !renderList_) {
        return;
    }
    
    // 变换所有路径段到全局坐标
    std::vector<PathSegment> globalSegments;
    globalSegments.reserve(segments.size());
    
    for (const auto& segment : segments) {
        PathSegment globalSegment = segment;
        
        // 根据段类型,只变换真正的坐标点
        switch (segment.type) {
            case PathSegmentType::MoveTo:
            case PathSegmentType::LineTo:
                // 单个终点
                for (auto& point : globalSegment.points) {
                    point = TransformPoint(point);
                }
                break;
                
            case PathSegmentType::QuadraticBezierTo:
                // 控制点 + 终点
                for (auto& point : globalSegment.points) {
                    point = TransformPoint(point);
                }
                break;
                
            case PathSegmentType::CubicBezierTo:
                // 两个控制点 + 终点
                for (auto& point : globalSegment.points) {
                    point = TransformPoint(point);
                }
                break;
                
            case PathSegmentType::ArcTo:
                // ArcTo 的点布局:
                // [0] = (radiusX, radiusY) - 半径,不需要变换(目前只有平移变换)
                // [1] = (xAxisRotation, 0) - 角度,不变换
                // [2] = (largeArcFlag, sweepFlag) - 标志,不变换
                // [3] = end point - 终点,需要完整变换
                if (globalSegment.points.size() >= 4) {
                    // points[0] (半径) 和 points[1] (角度) 和 points[2] (标志) 保持不变
                    globalSegment.points[3] = TransformPoint(globalSegment.points[3]); // 终点
                }
                break;
                
            case PathSegmentType::Close:
                // 无参数
                break;
        }
        
        globalSegments.push_back(globalSegment);
    }
    
    // 应用透明度
    auto finalFillColor = ApplyOpacity(fillColor);
    auto finalStrokeColor = ApplyOpacity(strokeColor);
    
    // 生成绘制命令
    PathPayload payload;
    payload.segments = globalSegments;
    payload.fillColor = finalFillColor;
    payload.strokeColor = finalStrokeColor;
    payload.strokeThickness = strokeWidth;
    payload.filled = true;
    
    renderList_->AddCommand(RenderCommand(CommandType::DrawPath, payload));
}

void RenderContext::DrawImage(
    const ui::Rect& bounds,
    uint32_t textureId,
    const std::array<float, 4>& tint)
{
    // 检查是否被裁剪
    if (IsClipped(bounds)) {
        return;
    }
    
    if (!renderList_) {
        return;
    }
    
    // 变换到全局坐标
    ui::Rect globalBounds = TransformRect(bounds);
    
    // 应用透明度到着色
    auto finalTint = ApplyOpacity(tint);
    
    // 生成绘制命令
    ImagePayload payload;
    payload.destRect = globalBounds;
    payload.textureId = textureId;
    
    renderList_->AddCommand(RenderCommand(CommandType::DrawImage, payload));
}

// ========== 文本度量 ==========

ui::Size RenderContext::MeasureText(
    const std::string& text,
    float fontSize,
    const std::string& fontFamily)
{
    if (!textRenderer_ || text.empty()) {
        // 简单估算
        return ui::Size{text.length() * fontSize * 0.6f, fontSize * 1.2f};
    }
    
    // Phase 5.0.5: 使用 TextRenderer 进行精确度量
    // 使用默认字体 ID（TextRenderer 会处理字体加载和缓存）
    int fontId = textRenderer_->GetDefaultFont();
    if (fontId < 0) {
        // 如果没有默认字体，尝试加载一个
        // 使用跨平台的字体路径
        #ifdef _WIN32
            fontId = textRenderer_->LoadFont("C:/Windows/Fonts/arial.ttf", static_cast<unsigned int>(fontSize));
        #elif __APPLE__
            fontId = textRenderer_->LoadFont("/System/Library/Fonts/Helvetica.ttc", static_cast<unsigned int>(fontSize));
        #else
            fontId = textRenderer_->LoadFont("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", static_cast<unsigned int>(fontSize));
        #endif
        
        if (fontId < 0) {
            // 仍然失败，使用简单估算
            return ui::Size{text.length() * fontSize * 0.6f, fontSize * 1.2f};
        }
    }
    
    int width = 0, height = 0;
    textRenderer_->MeasureText(text, fontId, width, height);
    
    return ui::Size{static_cast<float>(width), static_cast<float>(height)};
}

// ========== 私有辅助方法 ==========

void RenderContext::ApplyCurrentTransform() {
    if (!renderList_) {
        return;
    }
    
    // 生成变换命令
    TransformPayload payload;
    payload.offsetX = currentTransform_.offsetX;
    payload.offsetY = currentTransform_.offsetY;
    
    renderList_->AddCommand(RenderCommand(CommandType::SetTransform, payload));
}

void RenderContext::ApplyCurrentClip() {
    if (!renderList_) {
        return;
    }
    
    // 生成裁剪命令
    ClipPayload payload;
    payload.clipRect = currentClip_.clipRect;
    payload.enabled = currentClip_.enabled;
    
    renderList_->AddCommand(RenderCommand(CommandType::SetClip, payload));
}

std::array<float, 4> RenderContext::ApplyOpacity(const std::array<float, 4>& color) const {
    return {
        color[0],
        color[1],
        color[2],
        color[3] * currentOpacity_  // 应用透明度
    };
}

} // namespace fk::render
