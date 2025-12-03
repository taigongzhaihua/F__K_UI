#pragma once

namespace fk::ui {

/**
 * @brief 布局方向枚举
 * 用于 StackPanel, ScrollBar 等控件
 */
enum class Orientation {
    Horizontal,  // 水平
    Vertical     // 垂直
};

/**
 * @brief 滚动条可见性枚举
 * 用于 ScrollViewer 控制滚动条的显示方式
 * 
 * WPF 对应：System.Windows.Controls.ScrollBarVisibility
 */
enum class ScrollBarVisibility {
    Disabled,  ///< 禁用滚动，滚动条不显示，内容不可滚动
    Auto,      ///< 自动，仅当内容超出视口时显示滚动条
    Hidden,    ///< 隐藏滚动条，但内容可以滚动（如通过鼠标滚轮）
    Visible    ///< 始终显示滚动条
};

} // namespace fk::ui
