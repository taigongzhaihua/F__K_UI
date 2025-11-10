#pragma once

namespace fk::ui {

/**
 * @brief 水平对齐方式
 */
enum class HorizontalAlignment {
    Left,      // 左对齐
    Center,    // 居中
    Right,     // 右对齐
    Stretch    // 拉伸填充
};

/**
 * @brief 垂直对齐方式
 */
enum class VerticalAlignment {
    Top,       // 顶部对齐
    Center,    // 居中
    Bottom,    // 底部对齐
    Stretch    // 拉伸填充
};

} // namespace fk::ui
