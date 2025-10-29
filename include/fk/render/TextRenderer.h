#pragma once

#include <ft2build.h>
#include FT_FREETYPE_H

#include <string>
#include <memory>
#include <unordered_map>
#include <array>
#include <vector>

namespace fk::render {

// 字形信息
struct Glyph {
    unsigned int textureID;  // OpenGL 纹理 ID
    int width;              // 字形宽度
    int height;             // 字形高度
    int bearingX;           // 字形水平偏移
    int bearingY;           // 字形垂直偏移
    int advance;            // 水平前进值
};

// 字体信息
struct FontFace {
    FT_Face face;
    std::unordered_map<char32_t, Glyph> glyphs;  // 字符到字形的映射
};

/**
 * @brief 文本渲染器
 * 
 * 使用 FreeType 加载字体并生成字形纹理
 * 支持 UTF-8 编码的文本渲染
 */
class TextRenderer {
public:
    TextRenderer();
    ~TextRenderer();

    // 禁止拷贝
    TextRenderer(const TextRenderer&) = delete;
    TextRenderer& operator=(const TextRenderer&) = delete;

    /**
     * @brief 初始化 FreeType 库
     * @return 是否成功
     */
    bool Initialize();

    /**
     * @brief 加载字体
     * @param fontPath 字体文件路径
     * @param fontSize 字体大小(像素)
     * @return 字体 ID,失败返回 -1
     */
    int LoadFont(const std::string& fontPath, unsigned int fontSize);

    /**
     * @brief 渲染文本到纹理
     * @param text UTF-8 编码的文本
     * @param fontId 字体 ID
     * @param color RGBA 颜色数组
     * @param outWidth 输出纹理宽度
     * @param outHeight 输出纹理高度
     * @return OpenGL 纹理 ID,失败返回 0
     */
    unsigned int RenderTextToTexture(
        const std::string& text,
        int fontId,
        const std::array<float, 4>& color,
        int& outWidth,
        int& outHeight
    );

    /**
     * @brief 测量文本尺寸
     * @param text UTF-8 编码的文本
     * @param fontId 字体 ID
     * @param outWidth 输出宽度
     * @param outHeight 输出高度
     */
    void MeasureText(
        const std::string& text,
        int fontId,
        int& outWidth,
        int& outHeight
    );

    /**
     * @brief 获取字体的行高
     * @param fontId 字体 ID
     * @return 行高(像素)
     */
    int GetLineHeight(int fontId) const;

    /**
     * @brief 获取字符字形
     * @param c 字符码点
     * @param fontId 字体 ID
     * @return 字形指针,如果不存在返回 nullptr
     */
    const Glyph* GetGlyph(char32_t c, int fontId);

    /**
     * @brief UTF-8 转 UTF-32
     * @param utf8 UTF-8 字符串
     * @return UTF-32 字符数组
     */
    std::u32string Utf8ToUtf32(const std::string& utf8);

private:
    /**
     * @brief 加载单个字符的字形
     * @param c 字符码点
     * @param fontId 字体 ID
     * @return 是否成功
     */
    bool LoadCharacter(char32_t c, int fontId);

private:
    FT_Library ftLibrary_;
    std::vector<std::unique_ptr<FontFace>> fonts_;
    bool initialized_;
};

} // namespace fk::render
