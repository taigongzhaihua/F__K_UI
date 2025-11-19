#include "fk/render/TextRenderer.h"

#include <glad/glad.h>
#include <iostream>

namespace fk::render {

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

    // 预加载 ASCII 字符
    for (unsigned char c = 0; c < 128; c++) {
        if (!LoadCharacter(c, fontId)) {
            std::cerr << "ERROR::FREETYPE: Failed to load character: " << c << std::endl;
        }
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

    // 加载字符字形
    if (FT_Load_Char(font->face, c, FT_LOAD_RENDER)) {
        return false;
    }

    // 生成纹理
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
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

    // 设置纹理选项
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // 存储字形信息
    Glyph glyph{
        texture,
        static_cast<int>(font->face->glyph->bitmap.width),
        static_cast<int>(font->face->glyph->bitmap.rows),
        font->face->glyph->bitmap_left,
        font->face->glyph->bitmap_top,
        static_cast<int>(font->face->glyph->advance.x >> 6)  // 转换为像素 (1/64 像素)
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
        // 确保字符已加载
        if (font->glyphs.find(c) == font->glyphs.end()) {
            LoadCharacter(c, fontId);
        }

        auto it = font->glyphs.find(c);
        if (it == font->glyphs.end()) {
            continue;
        }

        const Glyph& glyph = it->second;
        x += glyph.advance;
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
        
        result.push_back(codepoint);
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
        if (!LoadCharacter(c, fontId)) {
            return nullptr;
        }
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
