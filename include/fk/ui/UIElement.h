#pragma once

#include "fk/ui/DependencyObject.h"
#include "fk/ui/Visual.h"
#include "fk/ui/DependencyPropertyMacros.h"
#include "fk/binding/DependencyProperty.h"
#include "fk/core/Event.h"

#include <any>
#include <cstddef>

namespace fk::render {
class RenderHost;
}

namespace fk::ui {

struct Size {
    float width{0.0f};
    float height{0.0f};

    constexpr Size() = default;
    constexpr Size(float w, float h) : width(w), height(h) {}
};

struct Rect {
    float x{0.0f};
    float y{0.0f};
    float width{0.0f};
    float height{0.0f};

    constexpr Rect() = default;
    constexpr Rect(float left, float top, float w, float h)
        : x(left), y(top), width(w), height(h) {}
};

enum class Visibility {
    Visible,
    Hidden,
    Collapsed
};

class UIElement : public DependencyObject, public Visual {
public:
    using DependencyObject::DependencyObject;

    UIElement();
    ~UIElement() override;

    // 依赖属性（使用宏）
    FK_DEPENDENCY_PROPERTY_DECLARE(Visibility, fk::ui::Visibility)
    FK_DEPENDENCY_PROPERTY_DECLARE(IsEnabled, bool)
    FK_DEPENDENCY_PROPERTY_DECLARE(Opacity, float)
    FK_DEPENDENCY_PROPERTY_DECLARE(ClipToBounds, bool)

public:
    // GetVisibility() 和 GetOpacity() 同时实现 Visual 接口
    
    Size Measure(const Size& availableSize);
    void Arrange(const Rect& finalRect);

    void InvalidateMeasure();
    void InvalidateArrange();
    
    /**
     * @brief 标记元素的视觉呈现为无效，触发重新渲染
     * 
     * 当元素的视觉外观发生变化但布局不变时调用此方法。
     * 这会通知 RenderHost 在下一帧重新渲染此元素。
     */
    void InvalidateVisual();

    [[nodiscard]] bool IsMeasureValid() const noexcept { return isMeasureValid_; }
    [[nodiscard]] bool IsArrangeValid() const noexcept { return isArrangeValid_; }

    [[nodiscard]] const Size& DesiredSize() const noexcept { return desiredSize_; }
    [[nodiscard]] const Rect& LayoutSlot() const noexcept { return layoutSlot_; }

    core::Event<UIElement&> MeasureInvalidated;
    core::Event<UIElement&> ArrangeInvalidated;

    // Visual 接口实现
    Rect GetRenderBounds() const override;
    // GetOpacity() 和 GetVisibility() 已在上面声明
    std::vector<Visual*> GetVisualChildren() const override;
    bool HasRenderContent() const override;

protected:
    void OnAttachedToLogicalTree() override;
    void OnDetachedFromLogicalTree() override;

    virtual Size MeasureCore(const Size& availableSize);
    virtual void ArrangeCore(const Rect& finalRect);

    void SetDesiredSize(const Size& size) noexcept { desiredSize_ = size; }
    void SetLayoutSlot(const Rect& rect) noexcept { layoutSlot_ = rect; }

public:
    // 鼠标事件处理 (支持事件路由)
    // 返回 true 表示事件已处理，停止冒泡；返回 false 表示未处理，继续冒泡
    virtual bool OnMouseButtonDown(int button, double x, double y);
    virtual bool OnMouseButtonUp(int button, double x, double y);
    virtual bool OnMouseMove(double x, double y);
    virtual bool OnMouseWheel(double xoffset, double yoffset, double mouseX, double mouseY);
    virtual bool OnKeyDown(int key, int scancode, int mods);
    virtual bool OnKeyUp(int key, int scancode, int mods);
    virtual bool OnTextInput(unsigned int codepoint);
    
    // 命中测试 - 检查点是否在此元素的渲染边界内
    virtual bool HitTest(double x, double y) const;
    
    // 查找在指定位置的最上层子元素
    virtual UIElement* HitTestChildren(double x, double y);

    /**
     * @brief 获取此元素关联的 RenderHost（通过遍历父级链）
     */
    class render::RenderHost* GetRenderHost() const;

private:
    static bool ValidateOpacity(const std::any& value);

    Size desiredSize_{};
    Rect layoutSlot_{};
    bool isMeasureValid_{false};
    bool isArrangeValid_{false};
};

} // namespace fk::ui