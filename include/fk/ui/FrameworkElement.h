#pragma once

#include "fk/ui/UIElement.h"
#include "fk/ui/DependencyPropertyMacros.h"
#include "fk/ui/Thickness.h"
#include "fk/core/Event.h"

#include <any>
#include <utility>

namespace fk::ui {

enum class HorizontalAlignment {
    Left,
    Center,
    Right,
    Stretch
};

enum class VerticalAlignment {
    Top,
    Center,
    Bottom,
    Stretch
};

class FrameworkElement : public UIElement {
public:
    using UIElement::UIElement;

    FrameworkElement();
    ~FrameworkElement() override;

    // 依赖属性声明
    FK_DEPENDENCY_PROPERTY_DECLARE(Width, float);
    FK_DEPENDENCY_PROPERTY_DECLARE(Height, float);
    FK_DEPENDENCY_PROPERTY_DECLARE(MinWidth, float);
    FK_DEPENDENCY_PROPERTY_DECLARE(MinHeight, float);
    FK_DEPENDENCY_PROPERTY_DECLARE(MaxWidth, float);
    FK_DEPENDENCY_PROPERTY_DECLARE(MaxHeight, float);
    FK_DEPENDENCY_PROPERTY_DECLARE(HorizontalAlignment, HorizontalAlignment);
    FK_DEPENDENCY_PROPERTY_DECLARE(VerticalAlignment, VerticalAlignment);
    FK_DEPENDENCY_PROPERTY_DECLARE_REF(Margin, fk::Thickness);

public:
    static const binding::DependencyProperty& DataContextProperty();
    void SetDataContext(std::any value);

    template <typename T>
    void SetDataContext(T&& value) {
        SetDataContext(std::any(std::forward<T>(value)));
    }

    void ClearDataContext();
    [[nodiscard]] const std::any& GetDataContext() const noexcept;

    [[nodiscard]] bool IsInitialized() const noexcept { return isInitialized_; }
    [[nodiscard]] bool IsLoaded() const noexcept { return isLoaded_; }
    [[nodiscard]] bool HasAppliedTemplate() const noexcept { return isTemplateApplied_; }

    bool ApplyTemplate();

    [[nodiscard]] const Size& RenderSize() const noexcept { return renderSize_; }

    core::Event<FrameworkElement&> Initialized;
    core::Event<FrameworkElement&> Loaded;
    core::Event<FrameworkElement&> Unloaded;
    core::Event<FrameworkElement&> TemplateApplied;

protected:
    Size MeasureCore(const Size& availableSize) override;
    void ArrangeCore(const Rect& finalRect) override;

    virtual Size MeasureOverride(const Size& availableSize);
    virtual Size ArrangeOverride(const Size& finalSize);

    virtual void OnInitialized();
    virtual void OnLoaded();
    virtual void OnUnloaded();
    virtual void OnApplyTemplate();

    void OnAttachedToLogicalTree() override;
    void OnDetachedFromLogicalTree() override;
    void OnDataContextChanged(const std::any& oldValue, const std::any& newValue) override;

private:
    static binding::PropertyMetadata BuildDataContextMetadata();
    static void DataContextPropertyChanged(binding::DependencyObject& sender, const binding::DependencyProperty& property,
        const std::any& oldValue, const std::any& newValue);

    static bool ValidateOptionalLength(const std::any& value);
    static bool ValidateNonNegativeLength(const std::any& value);
    static bool ValidateNonNegativeOrInfiniteLength(const std::any& value);
    static bool ValidateHorizontalAlignment(const std::any& value);
    static bool ValidateVerticalAlignment(const std::any& value);
    static bool ValidateThickness(const std::any& value);

    static bool IsUnsetLength(float value);

    void UpdateLocalDataContextFromProperty(const std::any& value);
    void UpdatePropertyFromLocalDataContext(const std::any& value);

    void CoerceMaxToAtLeastMin();

    Size renderSize_{};
    bool isInitialized_{false};
    bool isLoaded_{false};
    bool isTemplateApplied_{false};
    bool isUpdatingDataContext_{false};
};

} // namespace fk::ui
