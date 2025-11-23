#include "fk/render/TextRenderer.h"

#include <glad/glad.h>
#include <iostream>
#include <vector>
#include <cstring>

#include FT_COLOR_H
#include FT_GLYPH_H

namespace fk::render {

// COLR 层渲染辅助函数
static bool RenderCOLRLayers(FT_Face face, FT_UInt glyphIndex, int& outWidth, int& outHeight, 
                            int& outBearingX, int& outBearingY, std::vector<unsigned char>& outBuffer) {
    FT_LayerIterator iterator;
    FT_UInt layerGlyphIndex;
    FT_UInt layerColorIndex;
    FT_Bool haveLayer;
    
    iterator.p = NULL;
    
    // 首先获取所有层的边界来确定最终尺寸
    FT_BBox bbox;
    bbox.xMin = bbox.yMin = 32000;
    bbox.xMax = bbox.yMax = -32000;
    
    // 第一次遍历：计算边界
    while ((haveLayer = FT_Get_Color_Glyph_Layer(face, glyphIndex, &layerGlyphIndex, &layerColorIndex, &iterator))) {
        if (FT_Load_Glyph(face, layerGlyphIndex, FT_LOAD_DEFAULT) != 0) continue;
        
        FT_Glyph glyph;
        if (FT_Get_Glyph(face->glyph, &glyph) != 0) continue;
        
        FT_BBox layerBbox;
        FT_Glyph_Get_CBox(glyph, FT_GLYPH_BBOX_PIXELS, &layerBbox);
        FT_Done_Glyph(glyph);
        
        if (layerBbox.xMin < bbox.xMin) bbox.xMin = layerBbox.xMin;
        if (layerBbox.yMin < bbox.yMin) bbox.yMin = layerBbox.yMin;
        if (layerBbox.xMax > bbox.xMax) bbox.xMax = layerBbox.xMax;
        if (layerBbox.yMax > bbox.yMax) bbox.yMax = layerBbox.yMax;
    }
    
    if (bbox.xMin >= bbox.xMax || bbox.yMin >= bbox.yMax) {
        return false;
    }
    
    outWidth = bbox.xMax - bbox.xMin;
    outHeight = bbox.yMax - bbox.yMin;
    outBearingX = bbox.xMin;
    outBearingY = bbox.yMax;
    
    // 创建 RGBA 缓冲区
    outBuffer.resize(outWidth * outHeight * 4, 0);
    
    // 获取调色板
    FT_Palette_Data paletteData;
    FT_Palette_Data_Get(face, &paletteData);
    
    FT_Color* palette = nullptr;
    if (paletteData.num_palettes > 0) {
        FT_Palette_Select(face, 0, &palette);
    }
    
    // 第二次遍历：渲染每一层
    iterator.p = NULL;
    while ((haveLayer = FT_Get_Color_Glyph_Layer(face, glyphIndex, &layerGlyphIndex, &layerColorIndex, &iterator))) {
        if (FT_Load_Glyph(face, layerGlyphIndex, FT_LOAD_RENDER) != 0) continue;
        
        FT_Bitmap& bitmap = face->glyph->bitmap;
        if (bitmap.pixel_mode != FT_PIXEL_MODE_GRAY) continue;
        
        // 获取颜色
        unsigned char r = 0, g = 0, b = 0, a = 255;
        if (palette && layerColorIndex != 0xFFFF && layerColorIndex < paletteData.num_palette_entries) {
            FT_Color color = palette[layerColorIndex];
            r = color.red;
            g = color.green;
            b = color.blue;
            a = color.alpha;
        }
        
        // 混合到输出缓冲区
        int offsetX = face->glyph->bitmap_left - bbox.xMin;
        int offsetY = bbox.yMax - face->glyph->bitmap_top;
        
        for (unsigned int y = 0; y < bitmap.rows; y++) {
            for (unsigned int x = 0; x < bitmap.width; x++) {
                int dstX = offsetX + x;
                int dstY = offsetY + y;
                
                if (dstX < 0 || dstX >= outWidth || dstY < 0 || dstY >= outHeight) continue;
                
                unsigned char alpha = bitmap.buffer[y * bitmap.pitch + x];
                if (alpha == 0) continue;
                
                // Alpha blending
                float srcAlpha = (alpha / 255.0f) * (a / 255.0f);
                int idx = (dstY * outWidth + dstX) * 4;
                
                float dstAlpha = outBuffer[idx + 3] / 255.0f;
                float outAlpha = srcAlpha + dstAlpha * (1.0f - srcAlpha);
                
                if (outAlpha > 0) {
                    outBuffer[idx + 0] = static_cast<unsigned char>((r * srcAlpha + outBuffer[idx + 0] * dstAlpha * (1.0f - srcAlpha)) / outAlpha);
                    outBuffer[idx + 1] = static_cast<unsigned char>((g * srcAlpha + outBuffer[idx + 1] * dstAlpha * (1.0f - srcAlpha)) / outAlpha);
                    outBuffer[idx + 2] = static_cast<unsigned char>((b * srcAlpha + outBuffer[idx + 2] * dstAlpha * (1.0f - srcAlpha)) / outAlpha);
                    outBuffer[idx + 3] = static_cast<unsigned char>(outAlpha * 255.0f);
                }
            }
        }
    }
    
    return true;
}

TextRenderer::TextRenderer()
    : ftLibrary_(nullptr)
    , initialized_(false) {
}

TextRenderer::~TextRenderer() {
    // 清理字体
    for (auto& font : fonts_) {
        if (font && font->face) {
            // 清理字形纹理
            for (auto& pair : font->glyphs) {
                if (pair.second.textureID != 0) {
                    glDeleteTextures(1, &pair.second.textureID);
                }
            }
            FT_Done_Face(font->face);
        }
    }

    // 清理 FreeType 库
    if (ftLibrary_) {
        FT_Done_FreeType(ftLibrary_);
    }
}

bool TextRenderer::Initialize() {
    if (initialized_) {
        return true;
    }

    // 初始化 FreeType 库
    if (FT_Init_FreeType(&ftLibrary_)) {
        std::cerr << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
        return false;
    }

    initialized_ = true;
    std::cout << "TextRenderer initialized" << std::endl;
    return true;
}

int TextRenderer::LoadFont(const std::string& fontPath, unsigned int fontSize) {
    if (!initialized_) {
        std::cerr << "ERROR::FREETYPE: TextRenderer not initialized" << std::endl;
        return -1;
    }

    // Phase 5.0.3: 检查缓存
    FontCacheKey cacheKey{fontPath, fontSize};
    auto cacheIt = fontCache_.find(cacheKey);
    if (cacheIt != fontCache_.end()) {
        std::cout << "Font loaded from cache: " << fontPath << " (ID: " << cacheIt->second << ")" << std::endl;
        return cacheIt->second;
    }

    // 创建新的字体
    auto fontFace = std::make_unique<FontFace>();

    // 加载字体文件
    if (FT_New_Face(ftLibrary_, fontPath.c_str(), 0, &fontFace->face)) {
        std::cerr << "ERROR::FREETYPE: Failed to load font: " << fontPath << std::endl;
        return -1;
    }

    // 设置字体大小
    FT_Set_Pixel_Sizes(fontFace->face, 0, fontSize);
    
    // Phase 5.0.3: 存储字体信息
    fontFace->fontPath = fontPath;
    fontFace->fontSize = fontSize;

    // 先添加到fonts_向量,获取fontId
    int fontId = static_cast<int>(fonts_.size());
    fonts_.push_back(std::move(fontFace));
    
    // Phase 5.0.3: 添加到缓存
    fontCache_[cacheKey] = fontId;

    // 禁用字节对齐限制
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    // 预加载 ASCII 字符（失败是正常的，依赖 fallback）
    for (unsigned char c = 0; c < 128; c++) {
        LoadCharacter(c, fontId);
    }
    
    // Phase 5.0.3: 如果是第一个字体，设为默认
    if (defaultFontId_ == -1) {
        defaultFontId_ = fontId;
    }

    std::cout << "Font loaded: " << fontPath << " (ID: " << fontId << ")" << std::endl;
    return fontId;
}

bool TextRenderer::LoadCharacter(char32_t c, int fontId) {
    if (fontId < 0 || fontId >= fonts_.size()) {
        return false;
    }

    auto& font = fonts_[fontId];
    if (!font || !font->face) {
        return false;
    }

    // 检查是否已加载
    if (font->glyphs.find(c) != font->glyphs.end()) {
        return true;
    }

    // 先检查字体是否包含该字符
    FT_UInt glyph_index = FT_Get_Char_Index(font->face, c);
    if (glyph_index == 0) {
        // 字体不包含该字符（这是正常的，不是错误）
        return false;
    }

    // 检查是否为 COLR 字形
    FT_UInt glyphIndex = FT_Get_Char_Index(font->face, c);
    bool isColorGlyph = false;
    std::vector<unsigned char> colorBuffer;
    int colorWidth = 0, colorHeight = 0;
    int colorBearingX = 0, colorBearingY = 0;
    
    // 尝试使用 COLR 渲染
    if (FT_HAS_COLOR(font->face) && glyphIndex != 0) {
        if (RenderCOLRLayers(font->face, glyphIndex, colorWidth, colorHeight, 
                            colorBearingX, colorBearingY, colorBuffer)) {
            isColorGlyph = true;
        }
    }
    
    // 如果 COLR 失败，尝试 BGRA 位图
    if (!isColorGlyph) {
        FT_Error error = FT_Load_Char(font->face, c, FT_LOAD_COLOR | FT_LOAD_RENDER);
        if (error) {
            error = FT_Load_Char(font->face, c, FT_LOAD_RENDER);
            if (error) {
                return false;
            }
        }
        
        if (font->face->glyph->bitmap.pixel_mode == FT_PIXEL_MODE_BGRA) {
            isColorGlyph = true;
        }
    }

    // 生成纹理
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    
    // 根据像素格式选择纹理格式
    if (!colorBuffer.empty()) {
        // COLR 渲染的 RGBA 缓冲区
        glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RGBA,
            colorWidth,
            colorHeight,
            0,
            GL_RGBA,
            GL_UNSIGNED_BYTE,
            colorBuffer.data()
        );
    } else if (font->face->glyph->bitmap.pixel_mode == FT_PIXEL_MODE_BGRA) {
        // SBIX/CBDT 位图格式
        glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RGBA,
            font->face->glyph->bitmap.width,
            font->face->glyph->bitmap.rows,
            0,
            GL_BGRA,
            GL_UNSIGNED_BYTE,
            font->face->glyph->bitmap.buffer
        );
    } else {
        // 灰度字形
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RED,
            font->face->glyph->bitmap.width,
            font->face->glyph->bitmap.rows,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            font->face->glyph->bitmap.buffer
        );
    }

    // 设置纹理选项
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // 存储字形信息
    Glyph glyph{
        texture,
        !colorBuffer.empty() ? colorWidth : static_cast<int>(font->face->glyph->bitmap.width),
        !colorBuffer.empty() ? colorHeight : static_cast<int>(font->face->glyph->bitmap.rows),
        !colorBuffer.empty() ? colorBearingX : font->face->glyph->bitmap_left,
        !colorBuffer.empty() ? colorBearingY : font->face->glyph->bitmap_top,
        static_cast<int>(font->face->glyph->advance.x >> 6),  // 转换为像素 (1/64 像素)
        isColorGlyph  // 标记是否为彩色字形
    };

    font->glyphs[c] = glyph;
    return true;
}

void TextRenderer::MeasureText(
    const std::string& text,
    int fontId,
    int& outWidth,
    int& outHeight
) {
    outWidth = 0;
    outHeight = 0;

    if (fontId < 0 || fontId >= fonts_.size() || text.empty()) {
        return;
    }

    auto& font = fonts_[fontId];
    if (!font || !font->face) {
        return;
    }

    // 转换为 UTF-32
    auto utf32Text = Utf8ToUtf32(text);

    int x = 0;

    for (char32_t c : utf32Text) {
        // 使用 fallback 机制获取字形
        const Glyph* glyph = GetGlyphWithFallback(c, fontId);
        if (!glyph) {
            continue;
        }
        
        x += glyph->advance;
    }

    outWidth = x;
    // 使用字体的标准行高，而不是单个字形的高度
    // 这样可以保证文本的垂直对齐一致性
    outHeight = GetLineHeight(fontId);
}

int TextRenderer::GetLineHeight(int fontId) const {
    if (fontId < 0 || fontId >= fonts_.size()) {
        return 0;
    }

    auto& font = fonts_[fontId];
    if (!font || !font->face) {
        return 0;
    }

    return font->face->size->metrics.height >> 6;  // 转换为像素
}

unsigned int TextRenderer::RenderTextToTexture(
    const std::string& text,
    int fontId,
    const std::array<float, 4>& color,
    int& outWidth,
    int& outHeight
) {
    // TODO: 实现将文本渲染到单个纹理
    // 目前先返回 0,表示未实现
    outWidth = 0;
    outHeight = 0;
    return 0;
}

std::u32string TextRenderer::Utf8ToUtf32(const std::string& utf8) {
    std::u32string result;
    result.reserve(utf8.size()); // 预分配空间
    
    size_t i = 0;
    while (i < utf8.size()) {
        char32_t codepoint = 0;
        unsigned char c = static_cast<unsigned char>(utf8[i]);
        
        if (c <= 0x7F) {
            // 1字节字符 (ASCII)
            codepoint = c;
            i += 1;
        } else if ((c & 0xE0) == 0xC0) {
            // 2字节字符
            if (i + 1 >= utf8.size()) break;
            codepoint = ((c & 0x1F) << 6) | (utf8[i + 1] & 0x3F);
            i += 2;
        } else if ((c & 0xF0) == 0xE0) {
            // 3字节字符
            if (i + 2 >= utf8.size()) break;
            codepoint = ((c & 0x0F) << 12) | ((utf8[i + 1] & 0x3F) << 6) | (utf8[i + 2] & 0x3F);
            i += 3;
        } else if ((c & 0xF8) == 0xF0) {
            // 4字节字符
            if (i + 3 >= utf8.size()) break;
            codepoint = ((c & 0x07) << 18) | ((utf8[i + 1] & 0x3F) << 12) | 
                       ((utf8[i + 2] & 0x3F) << 6) | (utf8[i + 3] & 0x3F);
            i += 4;
        } else {
            // 无效的UTF-8序列，跳过
            i += 1;
            continue;
        }
        
        // 过滤变体选择符和其他不可见字符
        // U+FE00-U+FE0F: Variation Selectors (变体选择符)
        // U+E0100-U+E01EF: Variation Selectors Supplement
        // U+200B-U+200D: Zero Width Space, Zero Width Joiner, etc.
        bool isVariationSelector = (codepoint >= 0xFE00 && codepoint <= 0xFE0F) ||
                                   (codepoint >= 0xE0100 && codepoint <= 0xE01EF) ||
                                   (codepoint >= 0x200B && codepoint <= 0x200D);
        
        if (!isVariationSelector) {
            result.push_back(codepoint);
        }
    }
    
    return result;
}

const Glyph* TextRenderer::GetGlyph(char32_t c, int fontId) {
    if (fontId < 0 || fontId >= fonts_.size()) {
        return nullptr;
    }

    auto& font = fonts_[fontId];
    if (!font || !font->face) {
        return nullptr;
    }

    // 如果字形未加载,尝试加载
    if (font->glyphs.find(c) == font->glyphs.end()) {
        LoadCharacter(c, fontId);
    }

    auto it = font->glyphs.find(c);
    return (it != font->glyphs.end()) ? &it->second : nullptr;
}

// ========== Phase 5.0.3 新增方法 ==========

void TextRenderer::SetDefaultFont(int fontId) {
    if (fontId >= 0 && fontId < fonts_.size()) {
        defaultFontId_ = fontId;
    }
}

void TextRenderer::AddFallbackFont(int fontId) {
    if (fontId >= 0 && fontId < fonts_.size()) {
        fallbackFonts_.push_back(fontId);
    }
}

void TextRenderer::ClearFallbackFonts() {
    fallbackFonts_.clear();
}

const Glyph* TextRenderer::GetGlyphWithFallback(char32_t c, int fontId) {
    // 尝试主字体
    const Glyph* glyph = GetGlyph(c, fontId);
    if (glyph) {
        return glyph;
    }
    
    // 尝试回退字体
    for (int fallbackId : fallbackFonts_) {
        glyph = GetGlyph(c, fallbackId);
        if (glyph) {
            return glyph;
        }
    }
    
    // 尝试默认字体
    if (defaultFontId_ != -1 && defaultFontId_ != fontId) {
        glyph = GetGlyph(c, defaultFontId_);
        if (glyph) {
            return glyph;
        }
    }
    
    return nullptr;
}

TextLayout TextRenderer::CalculateTextLayout(
    const std::string& text,
    int fontId,
    float maxWidth
) {
    TextLayout layout;
    
    if (fontId < 0 || fontId >= fonts_.size() || text.empty()) {
        return layout;
    }
    
    auto& font = fonts_[fontId];
    if (!font || !font->face) {
        return layout;
    }
    
    // 转换为 UTF-32
    auto utf32Text = Utf8ToUtf32(text);
    
    if (maxWidth <= 0) {
        // 不换行，单行文本
        layout.lines.push_back(utf32Text);
        
        int lineWidth = 0;
        for (char32_t c : utf32Text) {
            const Glyph* glyph = GetGlyphWithFallback(c, fontId);
            if (glyph) {
                lineWidth += glyph->advance;
            }
        }
        
        layout.lineWidths.push_back(lineWidth);
        layout.totalWidth = lineWidth;
        layout.totalHeight = GetLineHeight(fontId);
    } else {
        // 换行模式
        std::u32string currentLine;
        int currentWidth = 0;
        int lineHeight = GetLineHeight(fontId);
        
        for (size_t i = 0; i < utf32Text.length(); ++i) {
            char32_t c = utf32Text[i];
            
            // 处理换行符
            if (c == U'\n') {
                layout.lines.push_back(currentLine);
                layout.lineWidths.push_back(currentWidth);
                layout.totalWidth = std::max(layout.totalWidth, currentWidth);
                
                currentLine.clear();
                currentWidth = 0;
                continue;
            }
            
            const Glyph* glyph = GetGlyphWithFallback(c, fontId);
            if (!glyph) {
                continue;
            }
            
            // 检查是否需要换行
            if (currentWidth + glyph->advance > maxWidth && !currentLine.empty()) {
                layout.lines.push_back(currentLine);
                layout.lineWidths.push_back(currentWidth);
                layout.totalWidth = std::max(layout.totalWidth, currentWidth);
                
                currentLine.clear();
                currentWidth = 0;
            }
            
            currentLine += c;
            currentWidth += glyph->advance;
        }
        
        // 添加最后一行
        if (!currentLine.empty()) {
            layout.lines.push_back(currentLine);
            layout.lineWidths.push_back(currentWidth);
            layout.totalWidth = std::max(layout.totalWidth, currentWidth);
        }
        
        layout.totalHeight = static_cast<int>(layout.lines.size()) * lineHeight;
    }
    
    return layout;
}

void TextRenderer::MeasureTextMultiline(
    const std::string& text,
    int fontId,
    float maxWidth,
    int& outWidth,
    int& outHeight
) {
    auto layout = CalculateTextLayout(text, fontId, maxWidth);
    outWidth = layout.totalWidth;
    outHeight = layout.totalHeight;
}

} // namespace fk::render
