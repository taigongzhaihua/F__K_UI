#pragma once

namespace fk::ui {

/**
 * @brief 字体粗细
 */
enum class FontWeight {
    Thin = 100,
    ExtraLight = 200,
    Light = 300,
    Normal = 400,
    Medium = 500,
    SemiBold = 600,
    Bold = 700,
    ExtraBold = 800,
    Black = 900
};

/**
 * @brief 字体样式
 */
enum class FontStyle {
    Normal,
    Italic,
    Oblique
};

/**
 * @brief 文本对齐方式
 */
enum class TextAlignment {
    Left,
    Center,
    Right,
    Justify
};

/**
 * @brief 文本换行方式
 */
enum class TextWrapping {
    NoWrap,      // 不换行
    Wrap,        // 自动换行
    WrapWithOverflow  // 换行但允许溢出
};

} // namespace fk::ui
