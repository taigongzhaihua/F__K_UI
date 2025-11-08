#pragma once

#include "fk/ui/Panel.h"
#include "fk/ui/View.h"
#include "fk/ui/DependencyPropertyMacros.h"

#include <algorithm>
#include <cstddef>
#include <limits>
#include <memory>
#include <span>
#include <utility>
#include <vector>

namespace fk::ui {

enum class GridUnitType {
    Auto,
    Pixel,
    Star
};

struct GridLength {
    constexpr GridLength(float v = 1.0f, GridUnitType u = GridUnitType::Star) noexcept
        : value(v), unit(u) {}

    static constexpr GridLength Auto() noexcept { return GridLength(0.0f, GridUnitType::Auto); }
    static constexpr GridLength Pixel(float pixels) noexcept { return GridLength(pixels, GridUnitType::Pixel); }
    static constexpr GridLength Star(float weight = 1.0f) noexcept { return GridLength(weight, GridUnitType::Star); }

    float value;
    GridUnitType unit;
};

struct RowDefinition {
    constexpr RowDefinition(GridLength h = GridLength::Star(),
        float min = 0.0f,
        float max = std::numeric_limits<float>::infinity()) noexcept
        : height(h), minHeight(min), maxHeight(max) {}

    GridLength height;
    float minHeight;
    float maxHeight;
};

struct ColumnDefinition {
    constexpr ColumnDefinition(GridLength w = GridLength::Star(),
        float min = 0.0f,
        float max = std::numeric_limits<float>::infinity()) noexcept
        : width(w), minWidth(min), maxWidth(max) {}

    GridLength width;
    float minWidth;
    float maxWidth;
};

using RowDefinitionCollection = std::vector<RowDefinition>;
using ColumnDefinitionCollection = std::vector<ColumnDefinition>;

struct GridCellPlacement {
    int row;
    int column;
    int rowSpan;
    int columnSpan;
};

inline GridCellPlacement cell(int row, int column, int rowSpan = 1, int columnSpan = 1) {
    return GridCellPlacement{row, column, rowSpan, columnSpan};
}

class Grid : public View<Grid, Panel<Grid>> {
public:
    using View::View;

    Grid();
    ~Grid() override;

    FK_DEPENDENCY_PROPERTY_DECLARE_REF(RowDefinitions, RowDefinitionCollection)
    FK_DEPENDENCY_PROPERTY_DECLARE_REF(ColumnDefinitions, ColumnDefinitionCollection)

public:
    std::shared_ptr<Grid> RowDefinitions(const RowDefinitionCollection& rows) {
        SetRowDefinitions(rows);
        return this->Self();
    }

    std::shared_ptr<Grid> RowDefinitions(RowDefinitionCollection&& rows) {
        SetRowDefinitions(std::move(rows));
        return this->Self();
    }

    std::shared_ptr<Grid> RowDefinitions(binding::Binding binding) {
        this->SetBinding(RowDefinitionsProperty(), std::move(binding));
        return this->Self();
    }

    std::shared_ptr<Grid> ColumnDefinitions(const ColumnDefinitionCollection& columns) {
        SetColumnDefinitions(columns);
        return this->Self();
    }

    std::shared_ptr<Grid> ColumnDefinitions(ColumnDefinitionCollection&& columns) {
        SetColumnDefinitions(std::move(columns));
        return this->Self();
    }

    std::shared_ptr<Grid> ColumnDefinitions(binding::Binding binding) {
        this->SetBinding(ColumnDefinitionsProperty(), std::move(binding));
        return this->Self();
    }

    std::shared_ptr<Grid> AddRow(RowDefinition row);
    std::shared_ptr<Grid> AddColumn(ColumnDefinition column);
    std::shared_ptr<Grid> ClearRows();
    std::shared_ptr<Grid> ClearColumns();

    static const binding::DependencyProperty& RowProperty();
    static const binding::DependencyProperty& ColumnProperty();
    static const binding::DependencyProperty& RowSpanProperty();
    static const binding::DependencyProperty& ColumnSpanProperty();

    static void SetRow(UIElement* element, int row);
    static void SetColumn(UIElement* element, int column);
    static void SetRowSpan(UIElement* element, int span);
    static void SetColumnSpan(UIElement* element, int span);

    static int GetRow(const UIElement* element);
    static int GetColumn(const UIElement* element);
    static int GetRowSpan(const UIElement* element);
    static int GetColumnSpan(const UIElement* element);

protected:
    Size MeasureOverride(const Size& availableSize) override;
    Size ArrangeOverride(const Size& finalSize) override;

private:
    struct GridDesiredInfo {
        std::vector<float> rowHeights;
        std::vector<float> columnWidths;
    };

    [[nodiscard]] GridDesiredInfo CalculateDesiredSizes(std::span<const std::shared_ptr<UIElement>> children,
        std::size_t rowCount,
        std::size_t columnCount) const;

    [[nodiscard]] std::vector<float> ResolveRowSizes(float availableHeight,
        const std::vector<float>& desiredRowHeights,
        bool useAvailableSpace) const;

    [[nodiscard]] std::vector<float> ResolveColumnSizes(float availableWidth,
        const std::vector<float>& desiredColumnWidths,
        bool useAvailableSpace) const;

    [[nodiscard]] std::size_t ResolveRowCount(std::span<const std::shared_ptr<UIElement>> children) const;
    [[nodiscard]] std::size_t ResolveColumnCount(std::span<const std::shared_ptr<UIElement>> children) const;

    [[nodiscard]] static int NormalizeIndex(int index, std::size_t count);
    [[nodiscard]] static int NormalizeSpan(int span);
    [[nodiscard]] static float ClampWithLimits(float value, float minValue, float maxValue);
};

inline std::shared_ptr<Grid> grid() {
    return std::make_shared<Grid>();
}

inline std::shared_ptr<UIElement> operator|(GridCellPlacement placement, std::shared_ptr<UIElement> element) {
    if (!element) {
        return element;
    }
    Grid::SetRow(element.get(), placement.row);
    Grid::SetColumn(element.get(), placement.column);
    Grid::SetRowSpan(element.get(), placement.rowSpan);
    Grid::SetColumnSpan(element.get(), placement.columnSpan);
    return element;
}

inline std::shared_ptr<UIElement> operator|(std::shared_ptr<UIElement> element, GridCellPlacement placement) {
    if (!element) {
        return element;
    }
    Grid::SetRow(element.get(), placement.row);
    Grid::SetColumn(element.get(), placement.column);
    Grid::SetRowSpan(element.get(), placement.rowSpan);
    Grid::SetColumnSpan(element.get(), placement.columnSpan);
    return element;
}

} // namespace fk::ui
