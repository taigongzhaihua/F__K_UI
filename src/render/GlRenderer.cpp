#include "fk/render/GlRenderer.h"
#include "fk/render/RenderList.h"
#include "fk/render/RenderCommandBuffer.h"
#include "fk/render/RenderCommand.h"
#include "fk/render/TextRenderer.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdexcept>
#include <cmath>
#include <algorithm>
#include <iostream>
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

uniform vec2 uViewport;

void main() {
    // aPos 已经是全局坐标，不需要再加 uOffset
    vec2 pos = aPos;
    // 转换到 NDC (-1 到 1)
    vec2 ndc = (pos / uViewport) * 2.0 - 1.0;
    ndc.y = -ndc.y; // 翻转 Y 轴
    gl_Position = vec4(ndc, 0.0, 1.0);
    
    vTexCoord = aTexCoord;
    vFragPos = aTexCoord;  // 使用纹理坐标作为局部坐标 (0,0 到 w,h)
}
)";

// Border 的片段着色器（圆形圆角，四个独立半径）
const char* borderFragmentShaderSource = R"(
#version 330 core
in vec2 vTexCoord;
in vec2 vFragPos;

out vec4 FragColor;

uniform vec4 uColor;          // 填充色
uniform vec4 uStrokeColor;    // 描边色
uniform float uStrokeWidth;   // 描边宽度（像素）
uniform vec2 uStrokeAlignment; // x = strokeInset, y = strokeOutset
uniform float uAAWidth;       // 抗锯齿宽度
uniform float uOpacity;
uniform vec4 uCornerRadius;   // 四个圆角：x=topLeft, y=topRight, z=bottomRight, w=bottomLeft
uniform vec2 uRectSize;

// 圆形圆角的 SDF
float roundedBoxSDF(vec2 p, vec2 size, vec4 radius) {
    // 根据象限选择对应的圆角半径
    float r;
    if (p.x > 0.0) {
        if (p.y > 0.0) {
            r = radius.z; // 右下
        } else {
            r = radius.y; // 右上
        }
    } else {
        if (p.y > 0.0) {
            r = radius.w; // 左下
        } else {
            r = radius.x; // 左上
        }
    }
    
    vec2 d = abs(p) - size + r;
    return length(max(d, 0.0)) + min(max(d.x, d.y), 0.0) - r;
}

void main() {
    vec2 center = uRectSize * 0.5;
    vec2 localPos = vFragPos - center;
    float dist = roundedBoxSDF(localPos, uRectSize * 0.5, uCornerRadius);

    float aa = max(uAAWidth, 0.0001);
    float strokeInset = uStrokeAlignment.x;
    float strokeOutset = uStrokeAlignment.y;

    // 判断是否有描边
    bool hasStroke = (strokeInset + strokeOutset) > 0.0001;
    
    // 计算最终颜色（使用 alpha 混合而不是叠加）
    vec4 color;
    
    if (hasStroke) {
        // 有描边：精确计算边框区域
        float fillEdge = dist + strokeInset;     // 填充边界（向内）
        float strokeEdge = dist - strokeOutset;  // 描边外边界（向外）
        
        // 填充区域：只在 fillEdge 内侧，平滑过渡
        float fillAlpha = smoothstep(aa, -aa, fillEdge);
        
        // 描边区域：在 [strokeEdge, fillEdge] 之间
        float strokeOuterAlpha = smoothstep(aa, -aa, strokeEdge);
        float strokeInnerAlpha = smoothstep(aa, -aa, fillEdge);
        float strokeAlpha = strokeOuterAlpha - strokeInnerAlpha;
        
        // 使用 mix 在填充和描边之间插值，避免混色
        // 当 fillAlpha 接近 1 时完全使用填充色，接近 0 时使用描边色
        vec3 finalRGB = mix(uStrokeColor.rgb, uColor.rgb, fillAlpha);
        float finalAlpha = max(uColor.a * fillAlpha, uStrokeColor.a * strokeAlpha);
        
        color = vec4(finalRGB, finalAlpha);
    } else {
        // 无描边：只渲染填充和其抗锯齿
        float fillAlpha = smoothstep(aa, -aa, dist);
        color = vec4(uColor.rgb, uColor.a * fillAlpha);
    }
    
    color *= uOpacity;

    if (color.a < 0.001) discard;
    FragColor = color;
}
)";

// Rectangle 的片段着色器(椭圆圆角,radiusX 和 radiusY)
const char* rectangleFragmentShaderSource = R"(
#version 330 core
// 片段着色器：椭圆圆角矩形（描边仅向内），改进的抗锯齿处理
// 使用说明（单位需统一，例如像素）：
// - vFragPos: 片段在与 uRectSize 同一坐标空间的本地位置（例如像素坐标）
// - uRectSize: 矩形完整尺寸（宽，高）
// - uCornerRadiusXY: 椭圆圆角半轴 (a,b)（半轴）
// - uStrokeWidth: 描边宽度（像素），全部放到内侧
// - uAAWidth: 抗锯齿最小宽度（像素），建议 0.5~1.0
// - 推荐在渲染端使用预乘 alpha 与 glBlendFuncSeparate(GL_ONE, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA)

in vec2 vTexCoord;
in vec2 vFragPos;

out vec4 FragColor;

uniform vec4 uColor;           // 填充色（非预乘）
uniform vec4 uStrokeColor;     // 描边色（非预乘）
uniform float uStrokeWidth;    // 描边宽度（像素），向内
uniform float uAAWidth;        // 抗锯齿最小宽度（像素）
uniform float uOpacity;        // 全局不透明度乘子
uniform vec2 uCornerRadiusXY;  // 椭圆半轴 (a,b)
uniform vec2 uRectSize;        // 矩形完整尺寸 (w,h)

const int NEWTON_ITERS = 12;
const float EPS = 1e-6;

// 计算点 p（以矩形中心为原点）到带椭圆圆角矩形边界的有符号距离
// 正值 = 在外部；负值 = 在内部
float sdRoundedRectEllipse(vec2 p, vec2 halfSize, vec2 radius) {
    vec2 r = min(radius, halfSize);
    vec2 q = abs(p);
    vec2 box = halfSize - r;

    if (q.x > halfSize.x || q.y > halfSize.y) {
        vec2 outside = max(q - halfSize, vec2(0.0));
        return length(outside);
    }

    vec2 d = q - box;

    if (d.x <= 0.0 && d.y <= 0.0) {
        float dx = halfSize.x - q.x;
        float dy = halfSize.y - q.y;
        return -min(dx, dy);
    }

    if (d.x <= 0.0) {
        return -(halfSize.y - q.y);
    }
    if (d.y <= 0.0) {
        return -(halfSize.x - q.x);
    }

    vec2 u = d;
    if (r.x < EPS || r.y < EPS) {
        return length(u) - min(r.x, r.y);
    }

    float a = r.x;
    float b = r.y;
    // 初始角度猜测：把点映射到单位椭圆后取极角
    float theta = atan(u.y / b, u.x / a);
    theta = clamp(theta, 1e-6, 1.57079632679 - 1e-6);

    for (int i = 0; i < NEWTON_ITERS; ++i) {
        float cs = cos(theta);
        float sn = sin(theta);
        float ex = a * cs;
        float ey = b * sn;
        float H = a * sn * (ex - u.x) - b * cs * (ey - u.y);
        float cs2 = cs * cs;
        float sn2 = sn * sn;
        float Hprime = (a * a - b * b) * (cs2 - sn2) - a * u.x * cs - b * u.y * sn;
        if (abs(Hprime) < 1e-8) break;
        theta -= H / Hprime;
        theta = clamp(theta, 1e-8, 1.57079632679 - 1e-8);
    }

    vec2 ellipsePt = vec2(a * cos(theta), b * sin(theta));
    float dist = length(u - ellipsePt);
    float norm = (u.x * u.x) / (a * a) + (u.y * u.y) / (b * b);
    return (norm <= 1.0) ? -dist : dist;
}

void main() {
    // 半尺寸坐标系（以矩形中心为原点）
    vec2 half = uRectSize * 0.5;
    vec2 p = vFragPos - half;

    // 计算有符号距离（正外，负内）
    float dist = sdRoundedRectEllipse(p, half, uCornerRadiusXY);

    // 基于像素导数自适应 AA：fwidth(dist) 表示每像素的变化量
    float pixelF = fwidth(dist);

    // 计算 halfAA：smoothstep 的半宽（使总过渡宽度 = 2*halfAA）
    // 但我们在后面保证过渡宽度不会把描边整体“扩张”超过期望
    float halfAA = max(0.5 * uAAWidth, 0.5 * pixelF); // 最小值受 uAAWidth 控制
    halfAA = max(halfAA, 0.25); // 保证至少有少量 AA 避免完全锯齿（可调整）

    // 描边仅向内：描边范围 dist ∈ [inner, outer]，outer = 0，inner = -strokeW
    float strokeW = max(uStrokeWidth, 0.0);
    float inner = -strokeW;
    float outer = 0.0;

    // 为避免 AA 导致描边“膨胀”，为内外边界分别限制半宽：
    // - outerHalfAA 用于外边界（0），允许轻微的外部采样 AA（但不放大描边）
    // - innerHalfAA 用于内边界（-strokeW），不得过大（以免把内边缘向外模糊过多）
    float outerHalfAA = halfAA;
    // 限制 innerHalfAA 不超过 strokeW 的一半（避免内边界平滑把描边宽度缩小/变形）
    float innerHalfAA = min(halfAA, max(0.5 * strokeW, 0.5 * halfAA));

    // 当没有描边时 innerHalfAA 无效，避免 NaN：确保 innerHalfAA >= 0
    innerHalfAA = max(innerHalfAA, 0.0);

    // outerMask: dist <= outer -> 1（内侧）；使用窄过渡 outerHalfAA
    float outerMask = 1.0 - smoothstep(outer - outerHalfAA, outer + outerHalfAA, dist);
    // innerMask: dist <= inner -> 1（填充区域）；使用窄过渡 innerHalfAA
    float innerMask = 1.0 - smoothstep(inner - innerHalfAA, inner + innerHalfAA, dist);

    // 描边掩码 = outerMask - innerMask（限制到 [0,1]）
    float strokeMask = clamp(outerMask - innerMask, 0.0, 1.0);
    float fillMask = clamp(innerMask, 0.0, 1.0);

    // 使用预乘 alpha 风格的组合能减少边缘暗边（渲染端应启用相应混合）
    // 但 uColor/uStrokeColor 是非预乘，因此先将 rgb * alpha 做为预乘结果
    vec3 strokePremulRGB = uStrokeColor.rgb * uStrokeColor.a;
    vec3 fillPremulRGB   = uColor.rgb * uColor.a;

    // 最终预乘颜色
    vec3 outPremulRGB = strokePremulRGB * strokeMask + fillPremulRGB * fillMask;
    float outA = uStrokeColor.a * strokeMask + uColor.a * fillMask;

    // 若你不想使用预乘，可以把下面结果除以 alpha（当 outA > 0）
    vec3 outRGB = (outA > 0.0) ? (outPremulRGB / outA) : vec3(0.0);

    // 最终颜色与全局不透明度相乘
    vec4 outColor = vec4(outRGB, outA) * uOpacity;

    // 小透明度阈值剪枝
    if (outColor.a <= 1e-5) discard;
    FragColor = outColor;
}
)";

// 文本渲染的顶点着色器
const char* textVertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec4 vertex; // <vec2 pos, vec2 tex>
out vec2 TexCoords;

uniform vec2 uViewport;

void main() {
    // vertex.xy 已经是全局坐标，不需要再加 uOffset
    vec2 pos = vertex.xy;
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

    // 注意：不再需要初始化 uniform 值
    // 每个 Draw 调用会在使用着色器前设置所需的 uniform

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

    // 设置 OpenGL 状态（2D渲染）
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // 注意：不再在 BeginFrame 设置着色器程序
    // 每个 Draw 调用会根据需要切换着色器（Border 或 Rectangle）
}

void GlRenderer::Draw(const RenderList& list) {
    if (!initialized_) {
        return;
    }

    if (list.IsEmpty()) {
        return;
    }

    // 直接使用 RenderList 的 GetCommands() 方法
    const auto& commands = list.GetCommands();
    
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
    if (payload.enabled) {
        // 启用裁切
        glEnable(GL_SCISSOR_TEST);
        
        // 计算裁切区域 (需要转换为窗口坐标系)
        // OpenGL 的裁切坐标系原点在左下角,Y 轴向上
        float x = currentOffsetX_ + payload.clipRect.x;
        float y = viewportSize_.height - (currentOffsetY_ + payload.clipRect.y + payload.clipRect.height);
        float width = payload.clipRect.width;
        float height = payload.clipRect.height;
        
        // 确保裁切区域有效
        if (width > 0 && height > 0) {
            glScissor(
                static_cast<GLint>(x),
                static_cast<GLint>(y),
                static_cast<GLsizei>(width),
                static_cast<GLsizei>(height)
            );
        }
    } else {
        // 禁用裁切
        glDisable(GL_SCISSOR_TEST);
    }
}

void GlRenderer::ApplyTransform(const TransformPayload& payload) {
    currentOffsetX_ = payload.offsetX;
    currentOffsetY_ = payload.offsetY;

    // uOffset 已经从着色器中移除（坐标已经是全局的）
    // SetTransform 命令保留用于其他目的或兼容性，但不再设置 uOffset
    // 不再需要更新 uniform
}

void GlRenderer::DrawRectangle(const RectanglePayload& payload) {
    // 绑定 VAO
    glBindVertexArray(vao_);

    const float width = payload.rect.width;
    const float height = payload.rect.height;
    const float minDimension = std::min(width, height);
    const float halfMinDimension = std::max(0.0f, minDimension * 0.5f);

    // 根据是否有椭圆圆角选择着色器
    bool hasEllipseCorners = (payload.radiusX > 0.0001f || payload.radiusY > 0.0001f);
    unsigned int shaderProgram = hasEllipseCorners ? rectangleShaderProgram_ : borderShaderProgram_;
    glUseProgram(shaderProgram);

    // 设置视口 uniform
    int viewportLoc = glGetUniformLocation(shaderProgram, "uViewport");
    glUniform2f(viewportLoc, 
        static_cast<float>(viewportSize_.width), 
        static_cast<float>(viewportSize_.height));

    // 设置填充颜色 uniform
    int colorLoc = glGetUniformLocation(shaderProgram, "uColor");
    glUniform4f(colorLoc, 
        payload.fillColor[0], 
        payload.fillColor[1], 
        payload.fillColor[2], 
        payload.fillColor[3]);

    // 设置不透明度（考虑图层栈）
    float effectiveOpacity = layerStack_.empty() ? 1.0f : layerStack_.back().opacity;
    int opacityLoc = glGetUniformLocation(shaderProgram, "uOpacity");
    glUniform1f(opacityLoc, effectiveOpacity);

    // 设置矩形尺寸（用于圆角计算）
    int rectSizeLoc = glGetUniformLocation(shaderProgram, "uRectSize");
    glUniform2f(rectSizeLoc, width, height);

    if (hasEllipseCorners) {
        // Rectangle 着色器：只设置椭圆圆角半径
        int cornerRadiusXYLoc = glGetUniformLocation(shaderProgram, "uCornerRadiusXY");
        glUniform2f(cornerRadiusXYLoc, payload.radiusX, payload.radiusY);
    } else {
        // Border 着色器：设置四个独立圆角半径
        // 设置圆角半径（需要检查相邻圆角之和不超过对应边长）
        // 参考 CSS 规范：https://www.w3.org/TR/css-backgrounds-3/#corner-overlap
        float topLeft = std::max(0.0f, payload.cornerRadiusTopLeft);
        float topRight = std::max(0.0f, payload.cornerRadiusTopRight);
        float bottomRight = std::max(0.0f, payload.cornerRadiusBottomRight);
        float bottomLeft = std::max(0.0f, payload.cornerRadiusBottomLeft);
        
        // 计算每条边上相邻圆角的和
        float topSum = topLeft + topRight;      // 上边
        float rightSum = topRight + bottomRight; // 右边
        float bottomSum = bottomRight + bottomLeft; // 下边
        float leftSum = bottomLeft + topLeft;    // 左边
        
        // 计算缩放因子：如果相邻圆角之和超过边长，需要按比例缩小
        float scaleX = 1.0f;
        float scaleY = 1.0f;
        
        if (topSum > width && topSum > 0.0f) {
            scaleX = std::min(scaleX, width / topSum);
        }
        if (bottomSum > width && bottomSum > 0.0f) {
            scaleX = std::min(scaleX, width / bottomSum);
        }
        if (leftSum > height && leftSum > 0.0f) {
            scaleY = std::min(scaleY, height / leftSum);
        }
        if (rightSum > height && rightSum > 0.0f) {
            scaleY = std::min(scaleY, height / rightSum);
        }
        
        // 使用最严格的缩放因子（水平和垂直方向取最小值）
        float scale = std::min(scaleX, scaleY);
        
        // 应用缩放并限制在合理范围内
        float clampedTopLeft = std::clamp(topLeft * scale, 0.0f, halfMinDimension);
        float clampedTopRight = std::clamp(topRight * scale, 0.0f, halfMinDimension);
        float clampedBottomRight = std::clamp(bottomRight * scale, 0.0f, halfMinDimension);
        float clampedBottomLeft = std::clamp(bottomLeft * scale, 0.0f, halfMinDimension);
        
        int cornerRadiusLoc = glGetUniformLocation(shaderProgram, "uCornerRadius");
        glUniform4f(cornerRadiusLoc, clampedTopLeft, clampedTopRight, clampedBottomRight, clampedBottomLeft);
    }

    // 构建矩形顶点（两个三角形，每个顶点包含位置和纹理坐标）
    float x = payload.rect.x;
    float y = payload.rect.y;
    float w = width;
    float h = height;

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

    // 计算描边对齐（inside/outside/center）
    float insetFactor = 0.5f;
    float outsetFactor = 0.5f;
    switch (payload.strokeAlignment) {
        case StrokeAlignment::Inside:
            insetFactor = 1.0f;
            outsetFactor = 0.0f;
            break;
        case StrokeAlignment::Outside:
            insetFactor = 0.0f;
            outsetFactor = 1.0f;
            break;
        case StrokeAlignment::Center:
        default:
            insetFactor = 0.5f;
            outsetFactor = 0.5f;
            break;
    }

    float strokeInset = std::min(payload.strokeThickness * insetFactor, halfMinDimension);
    float strokeOutset = std::max(payload.strokeThickness * outsetFactor, 0.0f);

    // 设置描边相关 uniform
    int strokeColorLoc = glGetUniformLocation(shaderProgram, "uStrokeColor");
    glUniform4f(strokeColorLoc,
        payload.strokeColor[0],
        payload.strokeColor[1],
        payload.strokeColor[2],
        payload.strokeColor[3]);

    int strokeWidthLoc = glGetUniformLocation(shaderProgram, "uStrokeWidth");
    glUniform1f(strokeWidthLoc, payload.strokeThickness);

    int strokeAlignLoc = glGetUniformLocation(shaderProgram, "uStrokeAlignment");
    glUniform2f(strokeAlignLoc, strokeInset, strokeOutset);

    int aaLoc = glGetUniformLocation(shaderProgram, "uAAWidth");
    float aaWidth = std::clamp(payload.aaWidth, 0.1f, 2.0f);
    glUniform1f(aaLoc, aaWidth);

    // 单次绘制：片段着色器基于 SDF 计算填充与描边
    glDrawArrays(GL_TRIANGLES, 0, 6);
    // 如果有描边，则先绘制外部（描边）填充，再绘制内矩形填充（这样可以正确支持圆角描边）

        // 无描边，直接绘制填充矩形
        glDrawArrays(GL_TRIANGLES, 0, 6);
    

    // 解绑
    glBindVertexArray(0);
}

void GlRenderer::DrawText(const TextPayload& payload) {
    if (!textRenderer_) {
        return;
    }

    // Phase 5.0.4: 改进字体加载 - 使用字体族名称和跨平台路径
    // 为每个字体大小加载单独的字体
    static std::unordered_map<std::string, int> fontCache; // 使用 "family_size" 作为键
    std::string fontKey = payload.fontFamily + "_" + std::to_string(static_cast<unsigned int>(payload.fontSize));
    
    int fontId = -1;
    auto it = fontCache.find(fontKey);
    if (it != fontCache.end()) {
        fontId = it->second;
    } else {
        // Phase 5.0.4: 跨平台字体路径
        std::vector<std::string> fontPaths;
        
        #ifdef _WIN32
            // Windows 字体路径
            fontPaths = {
                "C:/Windows/Fonts/msyh.ttc",      // 微软雅黑
                "C:/Windows/Fonts/simhei.ttf",    // 黑体
                "C:/Windows/Fonts/arial.ttf",     // Arial
                "C:/Windows/Fonts/times.ttf"      // Times
            };
        #elif __APPLE__
            // macOS 字体路径
            fontPaths = {
                "/System/Library/Fonts/PingFang.ttc",           // 苹方
                "/System/Library/Fonts/Helvetica.ttc",          // Helvetica
                "/Library/Fonts/Arial Unicode.ttf"               // Arial Unicode
            };
        #else
            // Linux 字体路径
            fontPaths = {
                "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
                "/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf",
                "/usr/share/fonts/opentype/noto/NotoSansCJK-Regular.ttc"
            };
        #endif
        
        // 尝试加载字体
        unsigned int fontSizeKey = static_cast<unsigned int>(payload.fontSize);
        for (const auto& path : fontPaths) {
            fontId = textRenderer_->LoadFont(path, fontSizeKey);
            if (fontId >= 0) {
                break;
            }
        }
        
        if (fontId < 0) {
            std::cerr << "Failed to load any font for size " << fontSizeKey << "!" << std::endl;
            // Phase 5.0.4: 尝试使用默认字体
            fontId = textRenderer_->GetDefaultFont();
            if (fontId < 0) {
                return;
            }
        }
        fontCache[fontKey] = fontId;
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
    // uOffset 已经从文本着色器中移除（坐标已经是全局的）
    // 不再需要设置 uOffset
    
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
    
    // 处理文本换行
    std::vector<std::string> lines;
    
    // 首先按硬换行符 \n 分割文本
    std::vector<std::string> hardLines;
    {
        std::string currentHardLine;
        for (char c : payload.text) {
            if (c == '\n') {
                hardLines.push_back(currentHardLine);
                currentHardLine.clear();
            } else {
                currentHardLine += c;
            }
        }
        if (!currentHardLine.empty() || payload.text.empty() || payload.text.back() == '\n') {
            hardLines.push_back(currentHardLine);
        }
    }
    
    // 然后对每个硬换行的行应用自动换行
    if (payload.textWrapping && payload.maxWidth > 0.0f) {
        // 自动换行:按字符分割文本
        float maxLineWidth = payload.maxWidth;
        
        for (const auto& hardLine : hardLines) {
            if (hardLine.empty()) {
                lines.push_back("");  // 保留空行
                continue;
            }
            
            std::string currentLine;
            float currentLineWidth = 0.0f;
            
            std::u32string utf32Text = textRenderer_->Utf8ToUtf32(hardLine);
            std::string utf8Buffer;
            
            for (char32_t c : utf32Text) {
                const auto* glyph = textRenderer_->GetGlyph(c, fontId);
                if (!glyph) continue;
                
                // 检查是否需要自动换行
                if (currentLineWidth + glyph->advance > maxLineWidth && !currentLine.empty()) {
                    lines.push_back(currentLine);
                    currentLine.clear();
                    currentLineWidth = 0.0f;
                }
                
                // 将 UTF-32 字符转换为 UTF-8 并添加到当前行
            if (c < 0x80) {
                utf8Buffer = std::string(1, static_cast<char>(c));
            } else if (c < 0x800) {
                utf8Buffer = std::string{
                    static_cast<char>(0xC0 | (c >> 6)),
                    static_cast<char>(0x80 | (c & 0x3F))
                };
            } else if (c < 0x10000) {
                utf8Buffer = std::string{
                    static_cast<char>(0xE0 | (c >> 12)),
                    static_cast<char>(0x80 | ((c >> 6) & 0x3F)),
                    static_cast<char>(0x80 | (c & 0x3F))
                };
            } else {
                utf8Buffer = std::string{
                    static_cast<char>(0xF0 | (c >> 18)),
                    static_cast<char>(0x80 | ((c >> 12) & 0x3F)),
                    static_cast<char>(0x80 | ((c >> 6) & 0x3F)),
                    static_cast<char>(0x80 | (c & 0x3F))
                };
            }
            
                currentLine += utf8Buffer;
                currentLineWidth += glyph->advance;
            }
            
            // 添加当前硬行的最后一个软行
            if (!currentLine.empty()) {
                lines.push_back(currentLine);
            }
        }
    } else {
        // 不自动换行:只处理硬换行符
        lines = hardLines;
    }
    
    // 渲染每一行
    float lineHeight = payload.fontSize * 1.2f;
    float y = payload.bounds.y;  // 修复：使用payload中的全局Y坐标
    float boundsBottom = payload.bounds.y + payload.bounds.height;
    
    for (const auto& line : lines) {
        // 如果当前行完全超出 bounds 底部，停止渲染后续行
        if (y >= boundsBottom) {
            break;
        }
        // 将 UTF-8 文本转换为 UTF-32
        std::u32string utf32Text = textRenderer_->Utf8ToUtf32(line);
        
        // Phase 5.0.4: 计算行宽用于对齐
        float lineWidth = 0.0f;
        for (char32_t c : utf32Text) {
            const auto* glyph = textRenderer_->GetGlyph(c, fontId);
            if (glyph) {
                lineWidth += glyph->advance;
            }
        }
        
        // Phase 5.0.4: 根据对齐方式计算起始 X 位置
        float x = payload.bounds.x;  // 修复：使用payload中的全局X坐标
        float maxWidth = payload.maxWidth > 0.0f ? payload.maxWidth : lineWidth;
        
        // TODO: 使用 payload.textAlignment (需要在 TextPayload 中添加)
        // 暂时默认左对齐
        // if (alignment == TextAlignment::Center) {
        //     x += (maxWidth - lineWidth) / 2.0f;
        // } else if (alignment == TextAlignment::Right) {
        //     x += maxWidth - lineWidth;
        // }
        
        for (char32_t c : utf32Text) {
            const auto* glyph = textRenderer_->GetGlyph(c, fontId);
            if (!glyph) {
                continue; // 跳过无法加载的字符
            }
            
            float xpos = x + glyph->bearingX;
            float ypos = y + (payload.fontSize - glyph->bearingY); // 基线对齐
            float w = glyph->width;
            float h = glyph->height;
            
            // 裁剪检查：跳过完全在 bounds 外的字符
            float charRight = xpos + w;
            float charBottom = ypos + h;
            float boundsRight = payload.bounds.x + payload.bounds.width;
            float boundsBottom = payload.bounds.y + payload.bounds.height;
            
            // 如果字符完全在右侧或底部之外，跳过
            if (xpos >= boundsRight || ypos >= boundsBottom) {
                x += glyph->advance;
                continue;
            }
            
            // 如果字符完全在左侧或顶部之外，跳过
            if (charRight <= payload.bounds.x || charBottom <= payload.bounds.y) {
                x += glyph->advance;
                continue;
            }
            
            // 计算裁剪后的字符区域和纹理坐标
            float renderXPos = xpos;
            float renderYPos = ypos;
            float renderW = w;
            float renderH = h;
            float texLeft = 0.0f;
            float texTop = 0.0f;
            float texRight = 1.0f;
            float texBottom = 1.0f;
            
            // 左边裁剪
            if (renderXPos < payload.bounds.x) {
                float clipAmount = payload.bounds.x - renderXPos;
                texLeft = clipAmount / w;
                renderXPos = payload.bounds.x;
                renderW -= clipAmount;
            }
            
            // 右边裁剪
            if (renderXPos + renderW > boundsRight) {
                float clipAmount = (renderXPos + renderW) - boundsRight;
                texRight = 1.0f - (clipAmount / w);
                renderW -= clipAmount;
            }
            
            // 顶部裁剪
            if (renderYPos < payload.bounds.y) {
                float clipAmount = payload.bounds.y - renderYPos;
                texTop = clipAmount / h;
                renderYPos = payload.bounds.y;
                renderH -= clipAmount;
            }
            
            // 底部裁剪
            if (renderYPos + renderH > boundsBottom) {
                float clipAmount = (renderYPos + renderH) - boundsBottom;
                texBottom = 1.0f - (clipAmount / h);
                renderH -= clipAmount;
            }
            
            // 如果裁剪后没有可见区域，跳过
            if (renderW <= 0 || renderH <= 0) {
                x += glyph->advance;
                continue;
            }
            
            // 更新 VBO (翻转纹理 V 坐标以匹配 FreeType 的上下翻转，并应用裁剪)
            float vertices[6][4] = {
                { renderXPos,           renderYPos + renderH,   texLeft,  texBottom },
                { renderXPos,           renderYPos,             texLeft,  texTop },
                { renderXPos + renderW, renderYPos,             texRight, texTop },
                
                { renderXPos,           renderYPos + renderH,   texLeft,  texBottom },
                { renderXPos + renderW, renderYPos,             texRight, texTop },
                { renderXPos + renderW, renderYPos + renderH,   texRight, texBottom }
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
        
        // 移动到下一行
        y += lineHeight;
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
    // 编译顶点着色器（Border 和 Rectangle 共享）
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

    // === 编译 Border 片段着色器（圆形圆角）===
    unsigned int borderFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(borderFragmentShader, 1, &borderFragmentShaderSource, nullptr);
    glCompileShader(borderFragmentShader);

    glGetShaderiv(borderFragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(borderFragmentShader, 512, nullptr, infoLog);
        throw std::runtime_error(std::string("Border fragment shader compilation failed: ") + infoLog);
    }

    // 链接 Border 着色器程序
    borderShaderProgram_ = glCreateProgram();
    glAttachShader(borderShaderProgram_, vertexShader);
    glAttachShader(borderShaderProgram_, borderFragmentShader);
    glLinkProgram(borderShaderProgram_);

    glGetProgramiv(borderShaderProgram_, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(borderShaderProgram_, 512, nullptr, infoLog);
        throw std::runtime_error(std::string("Border shader program linking failed: ") + infoLog);
    }

    glDeleteShader(borderFragmentShader);

    // === 编译 Rectangle 片段着色器（椭圆圆角）===
    unsigned int rectangleFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(rectangleFragmentShader, 1, &rectangleFragmentShaderSource, nullptr);
    glCompileShader(rectangleFragmentShader);

    glGetShaderiv(rectangleFragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(rectangleFragmentShader, 512, nullptr, infoLog);
        throw std::runtime_error(std::string("Rectangle fragment shader compilation failed: ") + infoLog);
    }

    // 链接 Rectangle 着色器程序
    rectangleShaderProgram_ = glCreateProgram();
    glAttachShader(rectangleShaderProgram_, vertexShader);
    glAttachShader(rectangleShaderProgram_, rectangleFragmentShader);
    glLinkProgram(rectangleShaderProgram_);

    glGetProgramiv(rectangleShaderProgram_, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(rectangleShaderProgram_, 512, nullptr, infoLog);
        throw std::runtime_error(std::string("Rectangle shader program linking failed: ") + infoLog);
    }

    glDeleteShader(rectangleFragmentShader);
    glDeleteShader(vertexShader);

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

    if (rectangleShaderProgram_ != 0) {
        glDeleteProgram(rectangleShaderProgram_);
        rectangleShaderProgram_ = 0;
    }

    if (borderShaderProgram_ != 0) {
        glDeleteProgram(borderShaderProgram_);
        borderShaderProgram_ = 0;
    }
}

} // namespace fk::render
