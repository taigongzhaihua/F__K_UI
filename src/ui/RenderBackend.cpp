#include "fk/ui/RenderBackend.h"
#include <glad/glad.h>
#include <cmath>

namespace fk::ui {

// ========== OpenGL 渲染后端实现 ==========

bool OpenGLRenderBackend::Initialize() {
    if (initialized_) {
        return true;
    }
    
    // OpenGL 已经在其他地方初始化（Application）
    // 这里只做一些额外设置
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    initialized_ = true;
    return true;
}

void OpenGLRenderBackend::Shutdown() {
    initialized_ = false;
}

void OpenGLRenderBackend::BeginFrame() {
    // 帧开始前的准备工作
}

void OpenGLRenderBackend::EndFrame() {
    // 帧结束后的清理工作
}

void OpenGLRenderBackend::Clear(const Color& color) {
    glClearColor(color.r, color.g, color.b, color.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void OpenGLRenderBackend::SetViewport(int x, int y, int width, int height) {
    glViewport(x, y, width, height);
}

void OpenGLRenderBackend::DrawRectangle(const Rect& rect, const Color& fill, const Color& stroke, float strokeWidth) {
    // 简化实现：使用立即模式（生产环境应使用 VBO/VAO）
    
    // 绘制填充
    if (fill.a > 0) {
        glColor4f(fill.r, fill.g, fill.b, fill.a);
        glBegin(GL_QUADS);
        glVertex2f(rect.x, rect.y);
        glVertex2f(rect.x + rect.width, rect.y);
        glVertex2f(rect.x + rect.width, rect.y + rect.height);
        glVertex2f(rect.x, rect.y + rect.height);
        glEnd();
    }
    
    // 绘制边框
    if (stroke.a > 0 && strokeWidth > 0) {
        glColor4f(stroke.r, stroke.g, stroke.b, stroke.a);
        glLineWidth(strokeWidth);
        glBegin(GL_LINE_LOOP);
        glVertex2f(rect.x, rect.y);
        glVertex2f(rect.x + rect.width, rect.y);
        glVertex2f(rect.x + rect.width, rect.y + rect.height);
        glVertex2f(rect.x, rect.y + rect.height);
        glEnd();
    }
}

void OpenGLRenderBackend::DrawRoundedRectangle(const Rect& rect, float radius, const Color& fill, const Color& stroke) {
    // 简化实现：绘制近似圆角矩形
    const int segments = 16;
    const float angleStep = 90.0f / segments * 3.14159f / 180.0f;
    
    if (fill.a > 0) {
        glColor4f(fill.r, fill.g, fill.b, fill.a);
        glBegin(GL_TRIANGLE_FAN);
        
        // 中心点
        glVertex2f(rect.x + rect.width / 2, rect.y + rect.height / 2);
        
        // 左上角
        for (int i = 0; i <= segments; ++i) {
            float angle = 180.0f * 3.14159f / 180.0f + i * angleStep;
            glVertex2f(rect.x + radius + radius * std::cos(angle),
                      rect.y + radius + radius * std::sin(angle));
        }
        
        // 左下角
        for (int i = 0; i <= segments; ++i) {
            float angle = 90.0f * 3.14159f / 180.0f + i * angleStep;
            glVertex2f(rect.x + radius + radius * std::cos(angle),
                      rect.y + rect.height - radius + radius * std::sin(angle));
        }
        
        // 右下角
        for (int i = 0; i <= segments; ++i) {
            float angle = 0.0f + i * angleStep;
            glVertex2f(rect.x + rect.width - radius + radius * std::cos(angle),
                      rect.y + rect.height - radius + radius * std::sin(angle));
        }
        
        // 右上角
        for (int i = 0; i <= segments; ++i) {
            float angle = 270.0f * 3.14159f / 180.0f + i * angleStep;
            glVertex2f(rect.x + rect.width - radius + radius * std::cos(angle),
                      rect.y + radius + radius * std::sin(angle));
        }
        
        glEnd();
    }
    
    // TODO: 绘制圆角边框
}

void OpenGLRenderBackend::DrawCircle(const Point& center, float radius, const Color& fill, const Color& stroke) {
    const int segments = 32;
    const float angleStep = 2.0f * 3.14159f / segments;
    
    // 绘制填充
    if (fill.a > 0) {
        glColor4f(fill.r, fill.g, fill.b, fill.a);
        glBegin(GL_TRIANGLE_FAN);
        glVertex2f(center.x, center.y);
        for (int i = 0; i <= segments; ++i) {
            float angle = i * angleStep;
            glVertex2f(center.x + radius * std::cos(angle),
                      center.y + radius * std::sin(angle));
        }
        glEnd();
    }
    
    // 绘制边框
    if (stroke.a > 0) {
        glColor4f(stroke.r, stroke.g, stroke.b, stroke.a);
        glBegin(GL_LINE_LOOP);
        for (int i = 0; i < segments; ++i) {
            float angle = i * angleStep;
            glVertex2f(center.x + radius * std::cos(angle),
                      center.y + radius * std::sin(angle));
        }
        glEnd();
    }
}

void OpenGLRenderBackend::DrawLine(const Point& start, const Point& end, const Color& color, float width) {
    glColor4f(color.r, color.g, color.b, color.a);
    glLineWidth(width);
    glBegin(GL_LINES);
    glVertex2f(start.x, start.y);
    glVertex2f(end.x, end.y);
    glEnd();
}

void OpenGLRenderBackend::DrawText(const Point& position, const std::string& text, const Color& color, float fontSize) {
    // TODO: 集成文本渲染（FreeType）
    // 临时占位实现
}

void OpenGLRenderBackend::DrawImage(const Rect& bounds, const std::string& imagePath) {
    // TODO: 集成图像渲染
    // 临时占位实现
}

void OpenGLRenderBackend::PushTransform(const Matrix3x2& transform) {
    glPushMatrix();
    
    // 转换为 OpenGL 4x4 矩阵（2D 仿射变换）
    float matrix[16] = {
        transform.m11, transform.m21, 0, 0,
        transform.m12, transform.m22, 0, 0,
        0, 0, 1, 0,
        transform.m31, transform.m32, 0, 1
    };
    
    glMultMatrixf(matrix);
}

void OpenGLRenderBackend::PopTransform() {
    glPopMatrix();
}

} // namespace fk::ui
