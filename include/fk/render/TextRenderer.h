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
    bool isColor;           // 是否为彩色字形（emoji）
};

// 字体信息
struct FontFace {
    FT_Face face;
    std::unordered_map<char32_t, Glyph> glyphs;  // 字符到字形的映射
    std::string fontPath;                         // 字体文件路径（用于缓存）
    unsigned int fontSize;                        // 字体大小
};

// 字体缓存键（路径 + 大小）
struct FontCacheKey {
    std::string path;
    unsigned int size;
    
    bool operator==(const FontCacheKey& other) const {
        return path == other.path && size == other.size;
    }
};

// 字体缓存键哈希函数
struct FontCacheKeyHash {
    std::size_t operator()(const FontCacheKey& key) const {
        return std::hash<std::string>()(key.path) ^ (std::hash<unsigned int>()(key.size) << 1);
    }
};

// 多行文本布局信息
struct TextLayout {
    std::vector<std::u32string> lines;  // 分行的文本
    std::vector<int> lineWidths;        // 每行宽度
    int totalWidth{0};                  // 总宽度
    int totalHeight{0};                 // 总高度
};

/**
 * @brief 文本渲染器（Phase 5.0.3 增强版）
 * 
 * 使用 FreeType 加载字体并生成字形纹理
 * 支持 UTF-8 编码的文本渲染
 * 
 * Phase 5.0.3 新增功能：
 * - 字体缓存（避免重复加载）
 * - 默认字体设置
 * - 字体回退机制
 * - 多行文本布局
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
     * @brief 加载字体（带缓存）
     * @param fontPath 字体文件路径
     * @param fontSize 字体大小(像素)
     * @return 字体 ID,失败返回 -1
     */
    int LoadFont(const std::string& fontPath, unsigned int fontSize);
    
    /**
     * @brief 设置默认字体
     * @param fontId 字体 ID
     */
    void SetDefaultFont(int fontId);
    
    /**
     * @brief 获取默认字体 ID
     */
    int GetDefaultFont() const { return defaultFontId_; }
    
    /**
     * @brief 添加回退字体
     * @param fontId 字体 ID
     * 
     * 当主字体找不到字符时，会依次尝试回退字体
     */
    void AddFallbackFont(int fontId);
    
    /**
     * @brief 清除回退字体列表
     */
    void ClearFallbackFonts();

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
     * @brief 获取字符字形（带回退）
     * @param c 字符码点
     * @param fontId 字体 ID
     * @return 字形指针,如果不存在返回 nullptr
     */
    const Glyph* GetGlyphWithFallback(char32_t c, int fontId);

    /**
     * @brief UTF-8 转 UTF-32
     * @param utf8 UTF-8 字符串
     * @return UTF-32 字符数组
     */
    std::u32string Utf8ToUtf32(const std::string& utf8);
    
    /**
     * @brief 计算多行文本布局
     * @param text UTF-8 编码的文本
     * @param fontId 字体 ID
     * @param maxWidth 最大宽度（0 表示不换行）
     * @return 文本布局信息
     */
    TextLayout CalculateTextLayout(
        const std::string& text,
        int fontId,
        float maxWidth = 0.0f
    );
    
    /**
     * @brief 测量多行文本尺寸
     * @param text UTF-8 编码的文本
     * @param fontId 字体 ID
     * @param maxWidth 最大宽度（0 表示不换行）
     * @param outWidth 输出总宽度
     * @param outHeight 输出总高度
     */
    void MeasureTextMultiline(
        const std::string& text,
        int fontId,
        float maxWidth,
        int& outWidth,
        int& outHeight
    );

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
    
    // Phase 5.0.3 新增成员
    std::unordered_map<FontCacheKey, int, FontCacheKeyHash> fontCache_;  // 字体缓存
    int defaultFontId_{-1};                                               // 默认字体
    std::vector<int> fallbackFonts_;                                     // 回退字体列表
};

} // namespace fk::render
