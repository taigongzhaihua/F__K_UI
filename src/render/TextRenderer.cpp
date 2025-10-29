#include "fk/render/TextRenderer.h"

#include <glad/glad.h>
#include <iostream>
#include <codecvt>
#include <locale>

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

    // 创建新的字体
    auto fontFace = std::make_unique<FontFace>();

    // 加载字体文件
    if (FT_New_Face(ftLibrary_, fontPath.c_str(), 0, &fontFace->face)) {
        std::cerr << "ERROR::FREETYPE: Failed to load font: " << fontPath << std::endl;
        return -1;
    }

    // 设置字体大小
    FT_Set_Pixel_Sizes(fontFace->face, 0, fontSize);

    // 先添加到fonts_向量,获取fontId
    int fontId = static_cast<int>(fonts_.size());
    fonts_.push_back(std::move(fontFace));

    // 禁用字节对齐限制
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    // 预加载 ASCII 字符
    for (unsigned char c = 0; c < 128; c++) {
        if (!LoadCharacter(c, fontId)) {
            std::cerr << "ERROR::FREETYPE: Failed to load character: " << c << std::endl;
        }
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
    int maxHeight = 0;

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
        maxHeight = std::max(maxHeight, glyph.height);
    }

    outWidth = x;
    outHeight = maxHeight > 0 ? maxHeight : GetLineHeight(fontId);
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
    try {
        std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> converter;
        return converter.from_bytes(utf8);
    } catch (const std::exception& e) {
        std::cerr << "ERROR::UTF8: Conversion failed: " << e.what() << std::endl;
        return U"";
    }
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

} // namespace fk::render
