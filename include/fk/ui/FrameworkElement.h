#pragma once

#include "fk/ui/UIElement.h"
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

    static const binding::DependencyProperty& WidthProperty();
    static const binding::DependencyProperty& HeightProperty();
    static const binding::DependencyProperty& MinWidthProperty();
    static const binding::DependencyProperty& MinHeightProperty();
    static const binding::DependencyProperty& MaxWidthProperty();
    static const binding::DependencyProperty& MaxHeightProperty();
    static const binding::DependencyProperty& HorizontalAlignmentProperty();
    static const binding::DependencyProperty& VerticalAlignmentProperty();
    static const binding::DependencyProperty& MarginProperty();
    static const binding::DependencyProperty& DataContextProperty();

    void SetWidth(float value);
    [[nodiscard]] float GetWidth() const;

    void SetHeight(float value);
    [[nodiscard]] float GetHeight() const;

    void SetMinWidth(float value);
    [[nodiscard]] float GetMinWidth() const;

    void SetMinHeight(float value);
    [[nodiscard]] float GetMinHeight() const;

    void SetMaxWidth(float value);
    [[nodiscard]] float GetMaxWidth() const;

    void SetMaxHeight(float value);
    [[nodiscard]] float GetMaxHeight() const;

    void SetHorizontalAlignment(HorizontalAlignment alignment);
    [[nodiscard]] HorizontalAlignment GetHorizontalAlignment() const;

    void SetVerticalAlignment(VerticalAlignment alignment);
    [[nodiscard]] VerticalAlignment GetVerticalAlignment() const;

    void SetMargin(const fk::Thickness& margin);
    [[nodiscard]] const fk::Thickness& GetMargin() const;

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
    static binding::PropertyMetadata BuildWidthMetadata();
    static binding::PropertyMetadata BuildHeightMetadata();
    static binding::PropertyMetadata BuildMinWidthMetadata();
    static binding::PropertyMetadata BuildMinHeightMetadata();
    static binding::PropertyMetadata BuildMaxWidthMetadata();
    static binding::PropertyMetadata BuildMaxHeightMetadata();
    static binding::PropertyMetadata BuildHorizontalAlignmentMetadata();
    static binding::PropertyMetadata BuildVerticalAlignmentMetadata();
    static binding::PropertyMetadata BuildMarginMetadata();
    static binding::PropertyMetadata BuildDataContextMetadata();

    static void LengthPropertyChanged(binding::DependencyObject& sender, const binding::DependencyProperty& property,
        const std::any& oldValue, const std::any& newValue);
    static void MinMaxLengthPropertyChanged(binding::DependencyObject& sender, const binding::DependencyProperty& property,
        const std::any& oldValue, const std::any& newValue);
    static void AlignmentPropertyChanged(binding::DependencyObject& sender, const binding::DependencyProperty& property,
        const std::any& oldValue, const std::any& newValue);
    static void MarginPropertyChanged(binding::DependencyObject& sender, const binding::DependencyProperty& property,
        const std::any& oldValue, const std::any& newValue);
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
