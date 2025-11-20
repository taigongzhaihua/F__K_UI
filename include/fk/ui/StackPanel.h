#pragma once

#include "fk/ui/Panel.h"
#include "fk/ui/Enums.h"
#include "fk/binding/DependencyProperty.h"

namespace fk::ui {

/**
 * @brief 堆栈面板
 * 
 * 职责：
 * - 按方向（水平/垂直）堆叠子元素
 * 
 * WPF 对应：StackPanel
 */
class StackPanel : public Panel<StackPanel> {
public:
    // ========== 依赖属性 ==========
    
    /// Orientation 属性：堆叠方向
    static const binding::DependencyProperty& OrientationProperty();
    
    /// Spacing 属性：子元素间距
    static const binding::DependencyProperty& SpacingProperty();

public:
    StackPanel() = default;
    virtual ~StackPanel() = default;

    // ========== 方向 ==========
    
    Orientation GetOrientation() const { return GetValue<enum ui::Orientation>(OrientationProperty()); }
    void SetOrientation(Orientation value) { SetValue(OrientationProperty(), value); }
    
    // 流式 API：使用 SetOrientation 避免与枚举类型名称冲突
    StackPanel* SetOrient(enum Orientation value) {
        SetOrientation(value);
        return this;
    }
    
    // ========== 间距 ==========
    
    float GetSpacing() const { return GetValue<float>(SpacingProperty()); }
    void SetSpacing(float value) { SetValue(SpacingProperty(), value); }
    
    StackPanel* Spacing(float value) {
        SetSpacing(value);
        return this;
    }
    float Spacing() const { return GetSpacing(); }

protected:
    Size MeasureOverride(const Size& availableSize) override;
    Size ArrangeOverride(const Size& finalSize) override;
    void OnRender(render::RenderContext& context) override;
};

} // namespace fk::ui
