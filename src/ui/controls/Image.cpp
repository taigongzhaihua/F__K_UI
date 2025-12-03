#include "fk/ui/controls/Image.h"
#include "fk/ui/graphics/Primitives.h"
#include "fk/render/DrawCommand.h"
#include "fk/render/RenderContext.h"
#include "fk/binding/DependencyProperty.h"

// 集成图像加载�?
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <algorithm>
#include <iostream>

// OpenGL headers
#include <glad/glad.h>

namespace fk::ui {

// ========== 图像数据结构（PIMPL�?==========

struct ImageData {
    unsigned int textureId = 0;   // OpenGL 纹理 ID
    int width = 0;                // 图像宽度
    int height = 0;               // 图像高度
    int channels = 0;             // 通道�?
    bool loaded = false;          // 是否已加�?
};

// ========== 依赖属性定�?==========

const binding::DependencyProperty& Image::SourceProperty() {
    static auto& prop = binding::DependencyProperty::Register(
        "Source",
        typeid(std::string),
        typeid(Image),
        {std::string()}
    );
    return prop;
}

const binding::DependencyProperty& Image::StretchProperty() {
    static auto& prop = binding::DependencyProperty::Register(
        "Stretch",
        typeid(ui::Stretch),
        typeid(Image),
        {ui::Stretch::Uniform}
    );
    return prop;
}

// ========== 构�?析构 ==========

Image::Image() : imageData_(std::make_unique<ImageData>()) {
}

Image::~Image() {
    UnloadImage();
}

// ========== 图像加载 ==========

void Image::SetSource(const std::string& path) {
    SetValue(SourceProperty(), path);
    
    UnloadImage();
    
    if (!path.empty()) {
        if (LoadImage(path)) {
            InvalidateMeasure();
            InvalidateVisual();
        }
    }
}

bool Image::LoadImage(const std::string& path) {
    // 使用 stb_image 加载图像
    int width, height, channels;
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &channels, 0);
    
    if (!data) {
        std::cerr << "Failed to load image: " << path << std::endl;
        std::cerr << "Reason: " << stbi_failure_reason() << std::endl;
        return false;
    }
    
    // 创建 OpenGL 纹理
    glGenTextures(1, &imageData_->textureId);
    glBindTexture(GL_TEXTURE_2D, imageData_->textureId);
    
    // 设置纹理参数
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    // 上传纹理数据
    GLenum format;
    switch (channels) {
        case 1: format = GL_RED; break;
        case 2: format = GL_RG; break;
        case 3: format = GL_RGB; break;
        case 4: format = GL_RGBA; break;
        default:
            stbi_image_free(data);
            return false;
    }
    
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    
    // 生成 mipmap（可选）
    glGenerateMipmap(GL_TEXTURE_2D);
    
    // 释放图像数据
    stbi_image_free(data);
    
    // 保存图像信息
    imageData_->width = width;
    imageData_->height = height;
    imageData_->channels = channels;
    imageData_->loaded = true;
    
    std::cout << "Successfully loaded image: " << path 
              << " (" << width << "x" << height << ", " << channels << " channels)" << std::endl;
    
    return true;
}

void Image::UnloadImage() {
    if (imageData_->loaded && imageData_->textureId != 0) {
        // 删除 OpenGL 纹理
        glDeleteTextures(1, &imageData_->textureId);
        
        imageData_->textureId = 0;
        imageData_->loaded = false;
        imageData_->width = 0;
        imageData_->height = 0;
        imageData_->channels = 0;
    }
}

// ========== 图像信息查询 ==========

float Image::GetImageWidth() const {
    return static_cast<float>(imageData_->width);
}

float Image::GetImageHeight() const {
    return static_cast<float>(imageData_->height);
}

bool Image::IsLoaded() const {
    return imageData_->loaded;
}

// ========== 布局测量 ==========

Size Image::MeasureOverride(const Size& availableSize) {
    if (!IsLoaded()) {
        return Size(0, 0);
    }
    
    float imgWidth = GetImageWidth();
    float imgHeight = GetImageHeight();
    
    auto stretch = GetStretch();
    
    // None: 使用原始尺寸
    if (stretch == Stretch::None) {
        return Size(imgWidth, imgHeight);
    }
    
    // 无限空间，使用原始尺�?
    if (availableSize.width == std::numeric_limits<float>::infinity() &&
        availableSize.height == std::numeric_limits<float>::infinity()) {
        return Size(imgWidth, imgHeight);
    }
    
    // Fill: 填充可用空间
    if (stretch == Stretch::Fill) {
        float width = availableSize.width != std::numeric_limits<float>::infinity() 
            ? availableSize.width : imgWidth;
        float height = availableSize.height != std::numeric_limits<float>::infinity() 
            ? availableSize.height : imgHeight;
        return Size(width, height);
    }
    
    // Uniform / UniformToFill: 保持宽高�?
    float aspectRatio = imgWidth / imgHeight;
    
    float availWidth = availableSize.width != std::numeric_limits<float>::infinity() 
        ? availableSize.width : imgWidth;
    float availHeight = availableSize.height != std::numeric_limits<float>::infinity() 
        ? availableSize.height : imgHeight;
    
    float widthByHeight = availHeight * aspectRatio;
    float heightByWidth = availWidth / aspectRatio;
    
    if (stretch == Stretch::Uniform) {
        // Uniform: 完全可见，可能有空白
        if (widthByHeight <= availWidth) {
            return Size(widthByHeight, availHeight);
        } else {
            return Size(availWidth, heightByWidth);
        }
    } else { // Stretch::UniformToFill
        // UniformToFill: 填充满，可能裁剪
        if (widthByHeight >= availWidth) {
            return Size(widthByHeight, availHeight);
        } else {
            return Size(availWidth, heightByWidth);
        }
    }
}

Size Image::ArrangeOverride(const Size& finalSize) {
    return finalSize;
}

// ========== 渲染计算 ==========

Rect Image::CalculateRenderBounds(const Size& containerSize) const {
    if (!IsLoaded()) {
        return Rect(0, 0, 0, 0);
    }
    
    float imgWidth = GetImageWidth();
    float imgHeight = GetImageHeight();
    
    auto stretch = GetStretch();
    
    // None: 左上角对齐，原始尺寸
    if (stretch == Stretch::None) {
        return Rect(0, 0, imgWidth, imgHeight);
    }
    
    // Fill: 填充整个容器
    if (stretch == Stretch::Fill) {
        return Rect(0, 0, containerSize.width, containerSize.height);
    }
    
    // Uniform / UniformToFill: 保持宽高�?
    float aspectRatio = imgWidth / imgHeight;
    float containerAspect = containerSize.width / containerSize.height;
    
    float renderWidth, renderHeight, x, y;
    
    if (stretch == Stretch::Uniform) {
        // Uniform: 完全可见
        if (aspectRatio > containerAspect) {
            // 图像更宽，以宽度为准
            renderWidth = containerSize.width;
            renderHeight = renderWidth / aspectRatio;
            x = 0;
            y = (containerSize.height - renderHeight) / 2.0f;
        } else {
            // 图像更高，以高度为准
            renderHeight = containerSize.height;
            renderWidth = renderHeight * aspectRatio;
            x = (containerSize.width - renderWidth) / 2.0f;
            y = 0;
        }
    } else { // UniformToFill
        // UniformToFill: 填充�?
        if (aspectRatio < containerAspect) {
            // 图像更窄，以宽度为准
            renderWidth = containerSize.width;
            renderHeight = renderWidth / aspectRatio;
            x = 0;
            y = (containerSize.height - renderHeight) / 2.0f;
        } else {
            // 图像更宽，以高度为准
            renderHeight = containerSize.height;
            renderWidth = renderHeight * aspectRatio;
            x = (containerSize.width - renderWidth) / 2.0f;
            y = 0;
        }
    }
    
    return Rect(x, y, renderWidth, renderHeight);
}

// ========== 渲染 ==========

void Image::OnRender(render::RenderContext& context) {
    // Phase 5.0.5: Image 渲染将在后续实现（Phase 5.0.6�?
    // 当前使用新的 RenderContext API，需要更新实�?
    // TODO: 使用 context.DrawImage() 替代旧的 DrawCommand::GetRenderer()->Submit()
    
    if (!IsLoaded()) {
        return;
    }
    
    // 暂时跳过渲染，等�?Phase 5.0.6 完成 Image 控件的新 API 适配
}

} // namespace fk::ui
