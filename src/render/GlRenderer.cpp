#include "fk/render/GlRenderer.h"
#include "fk/render/RenderList.h"
#include "fk/render/RenderCommandBuffer.h"
#include "fk/render/RenderCommand.h"
#include "fk/render/TextRenderer.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdexcept>
#include <cmath>
#include <iostream>
#include <unordered_map>

namespace fk::render {

// 简单的顶点着色器
const char* vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 vTexCoord;
out vec2 vFragPos;

uniform vec2 uOffset;
uniform vec2 uViewport;

void main() {
    vec2 pos = aPos + uOffset;
    // 转换到 NDC (-1 到 1)
    vec2 ndc = (pos / uViewport) * 2.0 - 1.0;
    ndc.y = -ndc.y; // 翻转 Y 轴
    gl_Position = vec4(ndc, 0.0, 1.0);
    
    vTexCoord = aTexCoord;
    vFragPos = aTexCoord;  // 使用纹理坐标作为局部坐标 (0,0 到 w,h)
}
)";

// 支持圆角的片段着色器
const char* fragmentShaderSource = R"(
#version 330 core
in vec2 vTexCoord;
in vec2 vFragPos;

out vec4 FragColor;

uniform vec4 uColor;
uniform float uOpacity;
uniform float uCornerRadius;
uniform vec2 uRectSize;

// 计算到圆角的距离
float roundedBoxSDF(vec2 p, vec2 size, float radius) {
    vec2 d = abs(p) - size + radius;
    return length(max(d, 0.0)) + min(max(d.x, d.y), 0.0) - radius;
}

void main() {
    if (uCornerRadius > 0.0) {
        // 计算片段在矩形中的位置（中心为原点）
        vec2 center = uRectSize * 0.5;
        vec2 localPos = vFragPos - center;
        
        // 计算到圆角边界的距离
        float dist = roundedBoxSDF(localPos, uRectSize * 0.5, uCornerRadius);
        
        // 抗锯齿：使用平滑过渡
        float alpha = 1.0 - smoothstep(-0.5, 0.5, dist);
        
        FragColor = vec4(uColor.rgb, uColor.a * uOpacity * alpha);
        
        // 如果完全透明则丢弃片段
        if (FragColor.a < 0.01) discard;
    } else {
        FragColor = vec4(uColor.rgb, uColor.a * uOpacity);
    }
}
)";

// 文本渲染的顶点着色器
const char* textVertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec4 vertex; // <vec2 pos, vec2 tex>
out vec2 TexCoords;

uniform vec2 uOffset;
uniform vec2 uViewport;

void main() {
    vec2 pos = vertex.xy + uOffset;
    vec2 ndc = (pos / uViewport) * 2.0 - 1.0;
    ndc.y = -ndc.y;
    gl_Position = vec4(ndc, 0.0, 1.0);
    TexCoords = vertex.zw;
}
)";

// 文本渲染的片段着色器
const char* textFragmentShaderSource = R"(
#version 330 core
in vec2 TexCoords;
out vec4 color;

uniform sampler2D text;
uniform vec4 textColor;
uniform float uOpacity;

void main() {
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, TexCoords).r);
    color = vec4(textColor.rgb, textColor.a * uOpacity) * sampled;
}
)";

GlRenderer::GlRenderer() = default;

GlRenderer::~GlRenderer() {
    if (initialized_) {
        CleanupResources();
    }
}

void GlRenderer::Initialize(const RendererInitParams& params) {
    if (initialized_) {
        throw std::runtime_error("GlRenderer already initialized");
    }

    // 初始化 GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        throw std::runtime_error("Failed to initialize GLAD");
    }

    viewportSize_ = params.initialSize;

    // 设置视口
    glViewport(0, 0, viewportSize_.width, viewportSize_.height);

    // 启用混合
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // 启用多重采样抗锯齿 (MSAA)
    glEnable(GL_MULTISAMPLE);

    // 初始化着色器和缓冲区
    InitializeShaders();
    InitializeBuffers();

    // 初始化文本渲染器
    textRenderer_ = std::make_unique<TextRenderer>();
    if (!textRenderer_->Initialize()) {
        std::cerr << "WARNING: Failed to initialize TextRenderer" << std::endl;
    }

    // 设置初始 uniform 值
    glUseProgram(shaderProgram_);
    
    // 初始化投影矩阵
    int projLoc = glGetUniformLocation(shaderProgram_, "uProjection");
    float projection[16] = {
        2.0f / viewportSize_.width, 0.0f, 0.0f, 0.0f,
        0.0f, -2.0f / viewportSize_.height, 0.0f, 0.0f,
        0.0f, 0.0f, -1.0f, 0.0f,
        -1.0f, 1.0f, 0.0f, 1.0f
    };
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, projection);
    
    // 初始化偏移为 (0,0)
    int offsetLoc = glGetUniformLocation(shaderProgram_, "uOffset");
    glUniform2f(offsetLoc, 0.0f, 0.0f);

    initialized_ = true;
}

void GlRenderer::Resize(const Extent2D& size) {
    viewportSize_ = size;
    glViewport(0, 0, size.width, size.height);
}

void GlRenderer::BeginFrame(const FrameContext& ctx) {
    currentFrame_ = ctx;

    // 清空颜色缓冲区
    glClearColor(
        ctx.clearColor[0],
        ctx.clearColor[1],
        ctx.clearColor[2],
        ctx.clearColor[3]
    );
    glClear(GL_COLOR_BUFFER_BIT);

    // 重置状态
    currentOffsetX_ = 0.0f;
    currentOffsetY_ = 0.0f;
    layerStack_.clear();
    layerStack_.push_back({1.0f}); // 根图层不透明度为 1

    // 绑定着色器程序
    glUseProgram(shaderProgram_);

    // 设置视口 uniform
    int viewportLoc = glGetUniformLocation(shaderProgram_, "uViewport");
    glUniform2f(viewportLoc, 
        static_cast<float>(viewportSize_.width), 
        static_cast<float>(viewportSize_.height));
    
    // 设置初始偏移为 0
    int offsetLoc = glGetUniformLocation(shaderProgram_, "uOffset");
    glUniform2f(offsetLoc, 0.0f, 0.0f);
}

void GlRenderer::Draw(const RenderList& list) {
    if (!initialized_) {
        return;
    }

    const RenderCommandBuffer* buffer = list.CommandBuffer();
    if (!buffer || buffer->IsEmpty()) {
        return;
    }

    // 执行所有渲染命令
    const auto& commands = buffer->GetCommands();
    for (const auto& cmd : commands) {
        ExecuteCommand(cmd);
    }
}

void GlRenderer::EndFrame() {
    // OpenGL 自动交换缓冲区由 GLFW 处理
    // 这里只需要解绑资源
    glUseProgram(0);
    glBindVertexArray(0);
}

void GlRenderer::Shutdown() {
    if (initialized_) {
        CleanupResources();
        initialized_ = false;
    }
}

void GlRenderer::ExecuteCommand(const RenderCommand& cmd) {
    switch (cmd.type) {
        case CommandType::SetClip:
            if (std::holds_alternative<ClipPayload>(cmd.payload)) {
                ApplyClip(std::get<ClipPayload>(cmd.payload));
            }
            break;

        case CommandType::SetTransform:
            if (std::holds_alternative<TransformPayload>(cmd.payload)) {
                ApplyTransform(std::get<TransformPayload>(cmd.payload));
            }
            break;

        case CommandType::DrawRectangle:
            if (std::holds_alternative<RectanglePayload>(cmd.payload)) {
                DrawRectangle(std::get<RectanglePayload>(cmd.payload));
            }
            break;

        case CommandType::DrawText:
            if (std::holds_alternative<TextPayload>(cmd.payload)) {
                DrawText(std::get<TextPayload>(cmd.payload));
            }
            break;

        case CommandType::DrawImage:
            if (std::holds_alternative<ImagePayload>(cmd.payload)) {
                DrawImage(std::get<ImagePayload>(cmd.payload));
            }
            break;

        case CommandType::PushLayer:
            if (std::holds_alternative<LayerPayload>(cmd.payload)) {
                PushLayer(std::get<LayerPayload>(cmd.payload));
            }
            break;

        case CommandType::PopLayer:
            PopLayer();
            break;
    }
}

void GlRenderer::ApplyClip(const ClipPayload& payload) {
    // TODO: 实现裁剪区域
    // 可以使用 glScissor 或模板缓冲区
}

void GlRenderer::ApplyTransform(const TransformPayload& payload) {
    currentOffsetX_ = payload.offsetX;
    currentOffsetY_ = payload.offsetY;

    // 更新 uniform
    int offsetLoc = glGetUniformLocation(shaderProgram_, "uOffset");
    glUniform2f(offsetLoc, currentOffsetX_, currentOffsetY_);
}

void GlRenderer::DrawRectangle(const RectanglePayload& payload) {
    // 绑定 VAO
    glBindVertexArray(vao_);

    // 设置颜色 uniform
    int colorLoc = glGetUniformLocation(shaderProgram_, "uColor");
    glUniform4f(colorLoc, 
        payload.color[0], 
        payload.color[1], 
        payload.color[2], 
        payload.color[3]);

    // 设置不透明度（考虑图层栈）
    float effectiveOpacity = layerStack_.empty() ? 1.0f : layerStack_.back().opacity;
    int opacityLoc = glGetUniformLocation(shaderProgram_, "uOpacity");
    glUniform1f(opacityLoc, effectiveOpacity);

    // 设置圆角半径
    int cornerRadiusLoc = glGetUniformLocation(shaderProgram_, "uCornerRadius");
    glUniform1f(cornerRadiusLoc, payload.cornerRadius);

    // 设置矩形尺寸（用于圆角计算）
    int rectSizeLoc = glGetUniformLocation(shaderProgram_, "uRectSize");
    glUniform2f(rectSizeLoc, payload.rect.width, payload.rect.height);

    // 构建矩形顶点（两个三角形，每个顶点包含位置和纹理坐标）
    float x = payload.rect.x;
    float y = payload.rect.y;
    float w = payload.rect.width;
    float h = payload.rect.height;

    // 顶点格式：x, y, u, v (位置 + 纹理坐标)
    float vertices[] = {
        // 第一个三角形
        x,     y,      0.0f, 0.0f,  // 左上
        x + w, y,      w,    0.0f,  // 右上
        x,     y + h,  0.0f, h,     // 左下
        
        // 第二个三角形
        x + w, y,      w,    0.0f,  // 右上
        x + w, y + h,  w,    h,     // 右下
        x,     y + h,  0.0f, h      // 左下
    };

    // 更新顶点数据
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

    // 绘制
    glDrawArrays(GL_TRIANGLES, 0, 6);

    // 解绑
    glBindVertexArray(0);
}

void GlRenderer::DrawText(const TextPayload& payload) {
    if (!textRenderer_) {
        return;
    }

    // 为每个字体大小加载单独的字体
    static std::unordered_map<unsigned int, int> fontCache;
    unsigned int fontSizeKey = static_cast<unsigned int>(payload.fontSize);
    
    int fontId = -1;
    auto it = fontCache.find(fontSizeKey);
    if (it != fontCache.end()) {
        fontId = it->second;
    } else {
        // 加载新的字体大小
        fontId = textRenderer_->LoadFont("C:/Windows/Fonts/msyh.ttc", fontSizeKey);
        if (fontId < 0) {
            fontId = textRenderer_->LoadFont("C:/Windows/Fonts/simhei.ttf", fontSizeKey);
        }
        if (fontId < 0) {
            std::cerr << "Failed to load any font for size " << fontSizeKey << "!" << std::endl;
            return;
        }
        fontCache[fontSizeKey] = fontId;
    }
    
    // 保存当前着色器程序
    GLint currentProgram;
    glGetIntegerv(GL_CURRENT_PROGRAM, &currentProgram);
    
    // 使用文本着色器
    glUseProgram(textShaderProgram_);
    
    // 设置 uniforms
    glUniform4f(glGetUniformLocation(textShaderProgram_, "textColor"), 
                payload.color[0], payload.color[1], payload.color[2], payload.color[3]);
    glUniform1f(glGetUniformLocation(textShaderProgram_, "uOpacity"), 1.0f);
    glUniform2f(glGetUniformLocation(textShaderProgram_, "uViewport"), 
                viewportSize_.width, viewportSize_.height);
    // 使用当前的全局变换偏移,而不是 payload.bounds
    glUniform2f(glGetUniformLocation(textShaderProgram_, "uOffset"), 
                currentOffsetX_, currentOffsetY_);
    
    glActiveTexture(GL_TEXTURE0);
    glUniform1i(glGetUniformLocation(textShaderProgram_, "text"), 0);
    glBindVertexArray(textVAO_);
    
    // 保存混合状态
    GLboolean blendEnabled = glIsEnabled(GL_BLEND);
    GLint blendSrc, blendDst;
    glGetIntegerv(GL_BLEND_SRC_ALPHA, &blendSrc);
    glGetIntegerv(GL_BLEND_DST_ALPHA, &blendDst);
    
    // 启用混合
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // 将 UTF-8 文本转换为 UTF-32
    std::u32string utf32Text = textRenderer_->Utf8ToUtf32(payload.text);
    
    // 渲染每个字符
    float x = 0.0f;  // 相对于 offset 的位置
    float y = 0.0f;
    
    for (char32_t c : utf32Text) {
        const auto* glyph = textRenderer_->GetGlyph(c, fontId);
        if (!glyph) {
            continue; // 跳过无法加载的字符
        }
        
        float xpos = x + glyph->bearingX;
        float ypos = y + (payload.fontSize - glyph->bearingY); // 基线对齐
        float w = glyph->width;
        float h = glyph->height;
        
        // 更新 VBO (翻转纹理 V 坐标以匹配 FreeType 的上下翻转)
        float vertices[6][4] = {
            { xpos,     ypos + h,   0.0f, 1.0f },
            { xpos,     ypos,       0.0f, 0.0f },
            { xpos + w, ypos,       1.0f, 0.0f },
            
            { xpos,     ypos + h,   0.0f, 1.0f },
            { xpos + w, ypos,       1.0f, 0.0f },
            { xpos + w, ypos + h,   1.0f, 1.0f }
        };
        
        // 绑定字形纹理
        glBindTexture(GL_TEXTURE_2D, glyph->textureID);
        
        // 更新 VBO 内容
        glBindBuffer(GL_ARRAY_BUFFER, textVBO_);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        
        // 渲染四边形
        glDrawArrays(GL_TRIANGLES, 0, 6);
        
        // 前进到下一个字形位置 (advance 已经是像素单位)
        x += glyph->advance;
    }
    
    // 恢复状态
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    
    // 恢复混合状态
    if (!blendEnabled) {
        glDisable(GL_BLEND);
    }
    if (blendEnabled) {
        glBlendFunc(blendSrc, blendDst);
    }
    
    // 恢复着色器程序
    glUseProgram(currentProgram);
}

void GlRenderer::DrawImage(const ImagePayload& payload) {
    // TODO: 实现图像渲染
    // 需要纹理管理
}

void GlRenderer::PushLayer(const LayerPayload& payload) {
    LayerState state;
    state.opacity = payload.opacity;
    
    // 如果有父图层，累乘不透明度
    if (!layerStack_.empty()) {
        state.opacity *= layerStack_.back().opacity;
    }
    
    layerStack_.push_back(state);
}

void GlRenderer::PopLayer() {
    if (!layerStack_.empty()) {
        layerStack_.pop_back();
    }
}

void GlRenderer::InitializeShaders() {
    // 编译顶点着色器
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);

    // 检查编译错误
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
        throw std::runtime_error(std::string("Vertex shader compilation failed: ") + infoLog);
    }

    // 编译片段着色器
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
        throw std::runtime_error(std::string("Fragment shader compilation failed: ") + infoLog);
    }

    // 链接着色器程序
    shaderProgram_ = glCreateProgram();
    glAttachShader(shaderProgram_, vertexShader);
    glAttachShader(shaderProgram_, fragmentShader);
    glLinkProgram(shaderProgram_);

    glGetProgramiv(shaderProgram_, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram_, 512, nullptr, infoLog);
        throw std::runtime_error(std::string("Shader program linking failed: ") + infoLog);
    }

    // 删除着色器（已经链接到程序中）
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // === 初始化文本着色器 ===
    
    // 编译文本顶点着色器
    unsigned int textVertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(textVertexShader, 1, &textVertexShaderSource, nullptr);
    glCompileShader(textVertexShader);

    glGetShaderiv(textVertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(textVertexShader, 512, nullptr, infoLog);
        throw std::runtime_error(std::string("Text vertex shader compilation failed: ") + infoLog);
    }

    // 编译文本片段着色器
    unsigned int textFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(textFragmentShader, 1, &textFragmentShaderSource, nullptr);
    glCompileShader(textFragmentShader);

    glGetShaderiv(textFragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(textFragmentShader, 512, nullptr, infoLog);
        throw std::runtime_error(std::string("Text fragment shader compilation failed: ") + infoLog);
    }

    // 链接文本着色器程序
    textShaderProgram_ = glCreateProgram();
    glAttachShader(textShaderProgram_, textVertexShader);
    glAttachShader(textShaderProgram_, textFragmentShader);
    glLinkProgram(textShaderProgram_);

    glGetProgramiv(textShaderProgram_, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(textShaderProgram_, 512, nullptr, infoLog);
        throw std::runtime_error(std::string("Text shader program linking failed: ") + infoLog);
    }

    glDeleteShader(textVertexShader);
    glDeleteShader(textFragmentShader);
}

void GlRenderer::InitializeBuffers() {
    // 创建 VAO 和 VBO
    glGenVertexArrays(1, &vao_);
    glGenBuffers(1, &vbo_);

    glBindVertexArray(vao_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);

    // 分配缓冲区空间（足够容纳一个矩形的顶点）
    // 每个顶点：(x, y, u, v) = 4 floats
    // 6个顶点（2个三角形）
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 24, nullptr, GL_DYNAMIC_DRAW);

    // 配置顶点属性
    // location = 0: 位置 (x, y)
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // location = 1: 纹理坐标 (u, v)
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // === 初始化文本渲染缓冲区 ===
    glGenVertexArrays(1, &textVAO_);
    glGenBuffers(1, &textVBO_);
    
    glBindVertexArray(textVAO_);
    glBindBuffer(GL_ARRAY_BUFFER, textVBO_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, nullptr, GL_DYNAMIC_DRAW);
    
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void GlRenderer::CleanupResources() {
    if (textVBO_ != 0) {
        glDeleteBuffers(1, &textVBO_);
        textVBO_ = 0;
    }

    if (textVAO_ != 0) {
        glDeleteVertexArrays(1, &textVAO_);
        textVAO_ = 0;
    }

    if (vbo_ != 0) {
        glDeleteBuffers(1, &vbo_);
        vbo_ = 0;
    }

    if (vao_ != 0) {
        glDeleteVertexArrays(1, &vao_);
        vao_ = 0;
    }

    if (textShaderProgram_ != 0) {
        glDeleteProgram(textShaderProgram_);
        textShaderProgram_ = 0;
    }

    if (shaderProgram_ != 0) {
        glDeleteProgram(shaderProgram_);
        shaderProgram_ = 0;
    }
}

} // namespace fk::render
