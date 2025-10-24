#pragma once

#include "fk/ui/DependencyObject.h"
#include "fk/binding/DependencyProperty.h"
#include "fk/core/Event.h"

#include <any>
#include <cstddef>

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

class UIElement : public DependencyObject {
public:
    using DependencyObject::DependencyObject;

    UIElement();
    ~UIElement() override;

    static const binding::DependencyProperty& VisibilityProperty();
    static const binding::DependencyProperty& IsEnabledProperty();
    static const binding::DependencyProperty& OpacityProperty();

    void SetVisibility(Visibility visibility);
    [[nodiscard]] Visibility GetVisibility() const;

    void SetIsEnabled(bool enabled);
    [[nodiscard]] bool IsEnabled() const;

    void SetOpacity(float value);
    [[nodiscard]] float Opacity() const;

    Size Measure(const Size& availableSize);
    void Arrange(const Rect& finalRect);

    void InvalidateMeasure();
    void InvalidateArrange();

    [[nodiscard]] bool IsMeasureValid() const noexcept { return isMeasureValid_; }
    [[nodiscard]] bool IsArrangeValid() const noexcept { return isArrangeValid_; }

    [[nodiscard]] const Size& DesiredSize() const noexcept { return desiredSize_; }
    [[nodiscard]] const Rect& LayoutSlot() const noexcept { return layoutSlot_; }

    core::Event<UIElement&> MeasureInvalidated;
    core::Event<UIElement&> ArrangeInvalidated;

protected:
    void OnAttachedToLogicalTree() override;
    void OnDetachedFromLogicalTree() override;

    virtual Size MeasureCore(const Size& availableSize);
    virtual void ArrangeCore(const Rect& finalRect);

    virtual void OnVisibilityChanged(Visibility oldValue, Visibility newValue);
    virtual void OnIsEnabledChanged(bool oldValue, bool newValue);
    virtual void OnOpacityChanged(float oldValue, float newValue);

    void SetDesiredSize(const Size& size) noexcept { desiredSize_ = size; }
    void SetLayoutSlot(const Rect& rect) noexcept { layoutSlot_ = rect; }

private:
    static binding::PropertyMetadata BuildVisibilityMetadata();
    static binding::PropertyMetadata BuildIsEnabledMetadata();
    static binding::PropertyMetadata BuildOpacityMetadata();
    static void VisibilityPropertyChanged(binding::DependencyObject& sender, const binding::DependencyProperty& property,
        const std::any& oldValue, const std::any& newValue);
    static void IsEnabledPropertyChanged(binding::DependencyObject& sender, const binding::DependencyProperty& property,
        const std::any& oldValue, const std::any& newValue);
    static void OpacityPropertyChanged(binding::DependencyObject& sender, const binding::DependencyProperty& property,
        const std::any& oldValue, const std::any& newValue);
    static bool ValidateOpacity(const std::any& value);

    Size desiredSize_{};
    Rect layoutSlot_{};
    bool isMeasureValid_{false};
    bool isArrangeValid_{false};
};

} // namespace fk::ui