#pragma once

#include <vector>

namespace fk::ui {

// 前向声明
struct Rect;
enum class Visibility;

/**
 * @brief Visual - 可视元素接口
 * 
 * 所有参与渲染的 UI 元素都需要提供 Visual 信息
 * UIElement 本身就是 Visual 的基础实现
 */
class Visual {
public:
    virtual ~Visual() = default;

    /**
     * @brief 获取渲染边界（布局后的最终位置和大小）
     */
    virtual Rect GetRenderBounds() const = 0;

    /**
     * @brief 获取不透明度
     */
    virtual float GetOpacity() const = 0;

    /**
     * @brief 获取可见性
     */
    virtual Visibility GetVisibility() const = 0;

    /**
     * @brief 获取可视子元素列表
     */
    virtual std::vector<Visual*> GetVisualChildren() const = 0;

    /**
     * @brief 是否有实际的渲染内容（背景、边框等）
     */
    virtual bool HasRenderContent() const = 0;
};

} // namespace fk::ui
