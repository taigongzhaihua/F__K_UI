#include "fk/ui/Renderer.h"
#include "fk/ui/RenderBackend.h"

namespace fk::ui {

Renderer::Renderer() = default;

Renderer::~Renderer() = default;

void Renderer::SetBackend(std::unique_ptr<RenderBackend> backend) {
    backend_ = std::move(backend);
    if (backend_) {
        backend_->Initialize();
    }
}

void Renderer::Submit(const DrawCommand& command) {
    commandQueue_.push_back(command);
}

void Renderer::Submit(const std::vector<DrawCommand>& commands) {
    commandQueue_.insert(commandQueue_.end(), commands.begin(), commands.end());
}

void Renderer::Clear() {
    commandQueue_.clear();
}

void Renderer::RenderFrame() {
    if (!backend_) {
        return;
    }
    
    backend_->BeginFrame();
    
    // 执行所有绘制命令
    for (const auto& cmd : commandQueue_) {
        // 应用变换
        if (cmd.transform.m11 != 1.0f || cmd.transform.m12 != 0.0f ||
            cmd.transform.m21 != 0.0f || cmd.transform.m22 != 1.0f ||
            cmd.transform.m31 != 0.0f || cmd.transform.m32 != 0.0f) {
            backend_->PushTransform(cmd.transform);
        }
        
        // 根据命令类型执行绘制
        switch (cmd.type) {
            case DrawCommandType::Rectangle:
                backend_->DrawRectangle(cmd.bounds, cmd.fillColor, cmd.strokeColor, cmd.strokeWidth);
                break;
                
            case DrawCommandType::RoundedRect:
                backend_->DrawRoundedRectangle(cmd.bounds, cmd.radius, cmd.fillColor, cmd.strokeColor);
                break;
                
            case DrawCommandType::Circle:
                backend_->DrawCircle(cmd.position, cmd.radius, cmd.fillColor, cmd.strokeColor);
                break;
                
            case DrawCommandType::Line: {
                Point end(cmd.bounds.x, cmd.bounds.y);
                backend_->DrawLine(cmd.position, end, cmd.strokeColor, cmd.strokeWidth);
                break;
            }
                
            case DrawCommandType::Text:
                backend_->DrawText(cmd.position, cmd.text, cmd.fillColor, cmd.fontSize);
                break;
                
            case DrawCommandType::Image:
                backend_->DrawImage(cmd.bounds, cmd.imagePath);
                break;
                
            case DrawCommandType::Custom:
                if (cmd.customDraw) {
                    cmd.customDraw();
                }
                break;
        }
        
        // 恢复变换
        if (cmd.transform.m11 != 1.0f || cmd.transform.m12 != 0.0f ||
            cmd.transform.m21 != 0.0f || cmd.transform.m22 != 1.0f ||
            cmd.transform.m31 != 0.0f || cmd.transform.m32 != 0.0f) {
            backend_->PopTransform();
        }
    }
    
    backend_->EndFrame();
    
    // 清空命令队列
    commandQueue_.clear();
}

void Renderer::SetViewport(const Size& size) {
    viewport_ = size;
    if (backend_) {
        backend_->SetViewport(0, 0, static_cast<int>(size.width), static_cast<int>(size.height));
    }
}

void Renderer::DrawRectangle(const Rect& rect, const Color& fill, const Color& stroke, float strokeWidth) {
    Submit(DrawCommand::Rectangle(rect, fill, stroke, strokeWidth));
}

void Renderer::DrawRoundedRectangle(const Rect& rect, float radius, const Color& fill, const Color& stroke) {
    Submit(DrawCommand::RoundedRectangle(rect, radius, fill, stroke));
}

void Renderer::DrawCircle(const Point& center, float radius, const Color& fill, const Color& stroke) {
    Submit(DrawCommand::Circle(center, radius, fill, stroke));
}

void Renderer::DrawLine(const Point& start, const Point& end, const Color& color, float width) {
    Submit(DrawCommand::Line(start, end, color, width));
}

void Renderer::DrawText(const Point& position, const std::string& text, const Color& color, float fontSize) {
    Submit(DrawCommand::Text(position, text, color, fontSize));
}

void Renderer::DrawImage(const Rect& bounds, const std::string& path) {
    Submit(DrawCommand::Image(bounds, path));
}

} // namespace fk::ui
