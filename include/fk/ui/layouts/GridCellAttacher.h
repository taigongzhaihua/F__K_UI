#pragma once

#include "fk/ui/base/UIElement.h"
#include <type_traits>

namespace fk::ui {

/**
 * @brief Grid 单元格位置包装器
 * 用于管道操作符风格的 API：element | cell(row, col)
 */
struct GridCellAttacher {
    int row{0};
    int col{0};
    int rowSpan{1};
    int colSpan{1};
    
    GridCellAttacher(int r, int c) : row(r), col(c) {}
    GridCellAttacher(int r, int c, int rs, int cs) 
        : row(r), col(c), rowSpan(rs), colSpan(cs) {}
    
    /**
     * @brief 璁剧疆琛岃法搴?
     */
    GridCellAttacher& RowSpan(int span) {
        rowSpan = span;
        return *this;
    }
    
    /**
     * @brief 璁剧疆鍒楄法搴?
     */
    GridCellAttacher& ColumnSpan(int span) {
        colSpan = span;
        return *this;
    }
    
    /**
     * @brief 应用到元素（保持类型，仅限 UIElement 派生类）
     */
    template<typename T, typename = std::enable_if_t<std::is_base_of<UIElement, T>::value>>
    T* ApplyTo(T* element) const {
        if (element) {
            UIElement* uiElement = static_cast<UIElement*>(element);
            ApplyToElement(uiElement);
        }
        return element;
    }
    
private:
    void ApplyToElement(UIElement* element) const;
};

/**
 * @brief 鍒涘缓 Grid 鍗曞厓鏍间綅缃寘瑁呭櫒
 * 
 * 鐢ㄦ硶锛?
 * @code
 * grid->Children({
 *     new Button()->Content("A") | cell(0, 0),
 *     new Button()->Content("B") | cell(0, 1),
 *     new Button()->Content("C") | cell(1, 0).RowSpan(2)
 * });
 * @endcode
 */
inline GridCellAttacher cell(int row, int col) {
    return GridCellAttacher(row, col);
}

/**
 * @brief 创建带跨度的 Grid 单元格位置包装器
 */
inline GridCellAttacher cell(int row, int col, int rowSpan, int colSpan) {
    return GridCellAttacher(row, col, rowSpan, colSpan);
}

/**
 * @brief 管道操作符：Element | GridCellAttacher (保持原始类型，仅限 UIElement 派生类)
 */
template<typename T, typename = std::enable_if_t<std::is_base_of<UIElement, T>::value>>
inline T* operator|(T* element, const GridCellAttacher& attacher) {
    return attacher.ApplyTo(element);
}

} // namespace fk::ui

