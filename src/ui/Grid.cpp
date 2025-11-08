#include "fk/ui/Grid.h"

#include <algorithm>
#include <any>
#include <cmath>
#include <numeric>
#include <utility>

namespace fk::ui {

FK_DEPENDENCY_PROPERTY_DEFINE_REF(Grid, RowDefinitions, RowDefinitionCollection)
FK_DEPENDENCY_PROPERTY_DEFINE_REF(Grid, ColumnDefinitions, ColumnDefinitionCollection)

Grid::Grid() = default;

Grid::~Grid() = default;

std::shared_ptr<Grid> Grid::AddRow(RowDefinition row) {
    RowDefinitionCollection rows = GetRowDefinitions();
    rows.push_back(std::move(row));
    SetRowDefinitions(std::move(rows));
    return this->Self();
}

std::shared_ptr<Grid> Grid::AddColumn(ColumnDefinition column) {
    ColumnDefinitionCollection columns = GetColumnDefinitions();
    columns.push_back(std::move(column));
    SetColumnDefinitions(std::move(columns));
    return this->Self();
}

std::shared_ptr<Grid> Grid::ClearRows() {
    SetRowDefinitions(RowDefinitionCollection{});
    return this->Self();
}

std::shared_ptr<Grid> Grid::ClearColumns() {
    SetColumnDefinitions(ColumnDefinitionCollection{});
    return this->Self();
}

const binding::DependencyProperty& Grid::RowProperty() {
    static const auto& property = binding::DependencyProperty::RegisterAttached(
        "Row",
        typeid(int),
        typeid(Grid),
        [] {
            binding::PropertyMetadata metadata;
            metadata.defaultValue = 0;
            return metadata;
        }());
    return property;
}

const binding::DependencyProperty& Grid::ColumnProperty() {
    static const auto& property = binding::DependencyProperty::RegisterAttached(
        "Column",
        typeid(int),
        typeid(Grid),
        [] {
            binding::PropertyMetadata metadata;
            metadata.defaultValue = 0;
            return metadata;
        }());
    return property;
}

const binding::DependencyProperty& Grid::RowSpanProperty() {
    static const auto& property = binding::DependencyProperty::RegisterAttached(
        "RowSpan",
        typeid(int),
        typeid(Grid),
        [] {
            binding::PropertyMetadata metadata;
            metadata.defaultValue = 1;
            return metadata;
        }());
    return property;
}

const binding::DependencyProperty& Grid::ColumnSpanProperty() {
    static const auto& property = binding::DependencyProperty::RegisterAttached(
        "ColumnSpan",
        typeid(int),
        typeid(Grid),
        [] {
            binding::PropertyMetadata metadata;
            metadata.defaultValue = 1;
            return metadata;
        }());
    return property;
}

void Grid::SetRow(UIElement* element, int row) {
    if (!element) {
        return;
    }
    element->SetValue(RowProperty(), std::max(0, row));
}

void Grid::SetColumn(UIElement* element, int column) {
    if (!element) {
        return;
    }
    element->SetValue(ColumnProperty(), std::max(0, column));
}

void Grid::SetRowSpan(UIElement* element, int span) {
    if (!element) {
        return;
    }
    element->SetValue(RowSpanProperty(), std::max(1, span));
}

void Grid::SetColumnSpan(UIElement* element, int span) {
    if (!element) {
        return;
    }
    element->SetValue(ColumnSpanProperty(), std::max(1, span));
}

int Grid::GetRow(const UIElement* element) {
    if (!element) {
        return 0;
    }
    const auto& value = element->GetValue(RowProperty());
    if (!value.has_value()) {
        return 0;
    }
    try {
        return std::max(0, std::any_cast<int>(value));
    } catch (...) {
        return 0;
    }
}

int Grid::GetColumn(const UIElement* element) {
    if (!element) {
        return 0;
    }
    const auto& value = element->GetValue(ColumnProperty());
    if (!value.has_value()) {
        return 0;
    }
    try {
        return std::max(0, std::any_cast<int>(value));
    } catch (...) {
        return 0;
    }
}

int Grid::GetRowSpan(const UIElement* element) {
    if (!element) {
        return 1;
    }
    const auto& value = element->GetValue(RowSpanProperty());
    if (!value.has_value()) {
        return 1;
    }
    try {
        return std::max(1, std::any_cast<int>(value));
    } catch (...) {
        return 1;
    }
}

int Grid::GetColumnSpan(const UIElement* element) {
    if (!element) {
        return 1;
    }
    const auto& value = element->GetValue(ColumnSpanProperty());
    if (!value.has_value()) {
        return 1;
    }
    try {
        return std::max(1, std::any_cast<int>(value));
    } catch (...) {
        return 1;
    }
}

Size Grid::MeasureOverride(const Size& availableSize) {
    const auto children = ChildSpan();

    for (const auto& child : children) {
        if (!child) {
            continue;
        }
        MeasureChild(*child, availableSize);
    }

    const auto rowCount = ResolveRowCount(children);
    const auto columnCount = ResolveColumnCount(children);

    auto desiredInfo = CalculateDesiredSizes(children, rowCount, columnCount);

    auto rowSizes = ResolveRowSizes(availableSize.height, desiredInfo.rowHeights, false);
    auto columnSizes = ResolveColumnSizes(availableSize.width, desiredInfo.columnWidths, false);

    const float totalHeight = std::accumulate(rowSizes.begin(), rowSizes.end(), 0.0f);
    const float totalWidth = std::accumulate(columnSizes.begin(), columnSizes.end(), 0.0f);

    Size desired{
        std::min(totalWidth, availableSize.width),
        std::min(totalHeight, availableSize.height)
    };

    return desired;
}

Size Grid::ArrangeOverride(const Size& finalSize) {
    const auto children = ChildSpan();

    const auto rowCount = ResolveRowCount(children);
    const auto columnCount = ResolveColumnCount(children);

    auto desiredInfo = CalculateDesiredSizes(children, rowCount, columnCount);

    auto rowSizes = ResolveRowSizes(finalSize.height, desiredInfo.rowHeights, true);
    auto columnSizes = ResolveColumnSizes(finalSize.width, desiredInfo.columnWidths, true);

    std::vector<float> rowOffsets(rowCount, 0.0f);
    std::vector<float> columnOffsets(columnCount, 0.0f);

    float accumulated = 0.0f;
    for (std::size_t i = 0; i < rowCount; ++i) {
        rowOffsets[i] = accumulated;
        accumulated += rowSizes[i];
    }

    accumulated = 0.0f;
    for (std::size_t i = 0; i < columnCount; ++i) {
        columnOffsets[i] = accumulated;
        accumulated += columnSizes[i];
    }

    for (const auto& child : children) {
        if (!child) {
            continue;
        }

        const auto rowIndex = static_cast<std::size_t>(NormalizeIndex(GetRow(child.get()), rowCount));
        const auto columnIndex = static_cast<std::size_t>(NormalizeIndex(GetColumn(child.get()), columnCount));
        const auto rowSpan = static_cast<std::size_t>(NormalizeSpan(GetRowSpan(child.get())));
        const auto columnSpan = static_cast<std::size_t>(NormalizeSpan(GetColumnSpan(child.get())));

        const auto rowEnd = std::min<std::size_t>(rowIndex + rowSpan, rowCount);
        const auto columnEnd = std::min<std::size_t>(columnIndex + columnSpan, columnCount);

        if (rowEnd <= rowIndex || columnEnd <= columnIndex) {
            continue;
        }

        float rectHeight = 0.0f;
        for (std::size_t r = rowIndex; r < rowEnd; ++r) {
            rectHeight += rowSizes[r];
        }

        float rectWidth = 0.0f;
        for (std::size_t c = columnIndex; c < columnEnd; ++c) {
            rectWidth += columnSizes[c];
        }

        const float rectY = rowOffsets[rowIndex];
        const float rectX = columnOffsets[columnIndex];

        ArrangeChild(*child, Rect{ rectX, rectY, rectWidth, rectHeight });
    }

    return finalSize;
}

void Grid::OnRowDefinitionsChanged(const RowDefinitionCollection&, const RowDefinitionCollection&) {
    InvalidateMeasure();
    InvalidateArrange();
}

void Grid::OnColumnDefinitionsChanged(const ColumnDefinitionCollection&, const ColumnDefinitionCollection&) {
    InvalidateMeasure();
    InvalidateArrange();
}

binding::PropertyMetadata Grid::BuildRowDefinitionsMetadata() {
    binding::PropertyMetadata metadata;
    metadata.defaultValue = RowDefinitionCollection{};
    metadata.propertyChangedCallback = &Grid::RowDefinitionsPropertyChanged;
    return metadata;
}

binding::PropertyMetadata Grid::BuildColumnDefinitionsMetadata() {
    binding::PropertyMetadata metadata;
    metadata.defaultValue = ColumnDefinitionCollection{};
    metadata.propertyChangedCallback = &Grid::ColumnDefinitionsPropertyChanged;
    return metadata;
}

Grid::GridDesiredInfo Grid::CalculateDesiredSizes(std::span<const std::shared_ptr<UIElement>> children,
    std::size_t rowCount,
    std::size_t columnCount) const {
    GridDesiredInfo info;
    info.rowHeights.assign(rowCount, 0.0f);
    info.columnWidths.assign(columnCount, 0.0f);

    for (const auto& child : children) {
        if (!child) {
            continue;
        }

        const auto desired = child->DesiredSize();

        const auto rowIndex = static_cast<std::size_t>(NormalizeIndex(GetRow(child.get()), rowCount));
        const auto columnIndex = static_cast<std::size_t>(NormalizeIndex(GetColumn(child.get()), columnCount));
        const auto rowSpan = static_cast<std::size_t>(NormalizeSpan(GetRowSpan(child.get())));
        const auto columnSpan = static_cast<std::size_t>(NormalizeSpan(GetColumnSpan(child.get())));

        const auto rowEnd = std::min<std::size_t>(rowIndex + rowSpan, rowCount);
        const auto columnEnd = std::min<std::size_t>(columnIndex + columnSpan, columnCount);

        if (rowEnd <= rowIndex || columnEnd <= columnIndex) {
            continue;
        }

        const auto rowCountForChild = rowEnd - rowIndex;
        const auto columnCountForChild = columnEnd - columnIndex;

        const float perRow = rowCountForChild > 0 ? desired.height / static_cast<float>(rowCountForChild) : desired.height;
        const float perColumn = columnCountForChild > 0 ? desired.width / static_cast<float>(columnCountForChild) : desired.width;

        for (std::size_t r = rowIndex; r < rowEnd; ++r) {
            info.rowHeights[r] = std::max(info.rowHeights[r], perRow);
        }

        for (std::size_t c = columnIndex; c < columnEnd; ++c) {
            info.columnWidths[c] = std::max(info.columnWidths[c], perColumn);
        }
    }

    const auto& rowDefs = GetRowDefinitions();
    if (!rowDefs.empty()) {
        for (std::size_t i = 0; i < rowCount; ++i) {
            const auto& def = rowDefs[i];
            if (def.height.unit == GridUnitType::Pixel) {
                info.rowHeights[i] = std::max(info.rowHeights[i], def.height.value);
            }
            info.rowHeights[i] = ClampWithLimits(info.rowHeights[i], def.minHeight, def.maxHeight);
        }
    }

    const auto& columnDefs = GetColumnDefinitions();
    if (!columnDefs.empty()) {
        for (std::size_t i = 0; i < columnCount; ++i) {
            const auto& def = columnDefs[i];
            if (def.width.unit == GridUnitType::Pixel) {
                info.columnWidths[i] = std::max(info.columnWidths[i], def.width.value);
            }
            info.columnWidths[i] = ClampWithLimits(info.columnWidths[i], def.minWidth, def.maxWidth);
        }
    }

    return info;
}

std::vector<float> Grid::ResolveRowSizes(float availableHeight,
    const std::vector<float>& desiredRowHeights,
    bool useAvailableSpace) const {
    const auto rowCount = desiredRowHeights.size();
    std::vector<float> result(rowCount, 0.0f);

    const auto& rowDefs = GetRowDefinitions();

    float totalFixed = 0.0f;
    float totalStarWeight = 0.0f;
    std::vector<std::size_t> starIndices;
    starIndices.reserve(rowCount);

    for (std::size_t i = 0; i < rowCount; ++i) {
        const auto def = rowDefs.empty() ? RowDefinition{} : rowDefs[i];
        const float desired = desiredRowHeights[i];

        switch (def.height.unit) {
        case GridUnitType::Pixel: {
            const float value = ClampWithLimits(def.height.value, def.minHeight, def.maxHeight);
            result[i] = value;
            totalFixed += value;
            break;
        }
        case GridUnitType::Auto: {
            const float value = ClampWithLimits(desired, def.minHeight, def.maxHeight);
            result[i] = value;
            totalFixed += value;
            break;
        }
        case GridUnitType::Star: {
            if (useAvailableSpace) {
                starIndices.push_back(i);
                totalStarWeight += std::max(def.height.value, 0.0f);
                result[i] = ClampWithLimits(0.0f, def.minHeight, def.maxHeight);
            } else {
                const float value = ClampWithLimits(desired, def.minHeight, def.maxHeight);
                result[i] = value;
                totalFixed += value;
            }
            break;
        }
        }
    }

    if (!useAvailableSpace) {
        return result;
    }

    float remaining = availableHeight - totalFixed;
    if (remaining < 0.0f) {
        remaining = 0.0f;
    }

    if (starIndices.empty()) {
        return result;
    }

    if (totalStarWeight <= 0.0f) {
        totalStarWeight = static_cast<float>(starIndices.size());
    }

    for (auto index : starIndices) {
        const auto def = rowDefs.empty() ? RowDefinition{} : rowDefs[index];
        float weight = def.height.value;
        if (weight <= 0.0f) {
            weight = 1.0f;
        }

        float value = remaining > 0.0f ? remaining * (weight / totalStarWeight) : 0.0f;
        value = ClampWithLimits(value, def.minHeight, def.maxHeight);
        result[index] = value;
    }

    return result;
}

std::vector<float> Grid::ResolveColumnSizes(float availableWidth,
    const std::vector<float>& desiredColumnWidths,
    bool useAvailableSpace) const {
    const auto columnCount = desiredColumnWidths.size();
    std::vector<float> result(columnCount, 0.0f);

    const auto& columnDefs = GetColumnDefinitions();

    float totalFixed = 0.0f;
    float totalStarWeight = 0.0f;
    std::vector<std::size_t> starIndices;
    starIndices.reserve(columnCount);

    for (std::size_t i = 0; i < columnCount; ++i) {
        const auto def = columnDefs.empty() ? ColumnDefinition{} : columnDefs[i];
        const float desired = desiredColumnWidths[i];

        switch (def.width.unit) {
        case GridUnitType::Pixel: {
            const float value = ClampWithLimits(def.width.value, def.minWidth, def.maxWidth);
            result[i] = value;
            totalFixed += value;
            break;
        }
        case GridUnitType::Auto: {
            const float value = ClampWithLimits(desired, def.minWidth, def.maxWidth);
            result[i] = value;
            totalFixed += value;
            break;
        }
        case GridUnitType::Star: {
            if (useAvailableSpace) {
                starIndices.push_back(i);
                totalStarWeight += std::max(def.width.value, 0.0f);
                result[i] = ClampWithLimits(0.0f, def.minWidth, def.maxWidth);
            } else {
                const float value = ClampWithLimits(desired, def.minWidth, def.maxWidth);
                result[i] = value;
                totalFixed += value;
            }
            break;
        }
        }
    }

    if (!useAvailableSpace) {
        return result;
    }

    float remaining = availableWidth - totalFixed;
    if (remaining < 0.0f) {
        remaining = 0.0f;
    }

    if (starIndices.empty()) {
        return result;
    }

    if (totalStarWeight <= 0.0f) {
        totalStarWeight = static_cast<float>(starIndices.size());
    }

    for (auto index : starIndices) {
        const auto def = columnDefs.empty() ? ColumnDefinition{} : columnDefs[index];
        float weight = def.width.value;
        if (weight <= 0.0f) {
            weight = 1.0f;
        }

        float value = remaining > 0.0f ? remaining * (weight / totalStarWeight) : 0.0f;
        value = ClampWithLimits(value, def.minWidth, def.maxWidth);
        result[index] = value;
    }

    return result;
}

std::size_t Grid::ResolveRowCount(std::span<const std::shared_ptr<UIElement>> children) const {
    const auto& defs = GetRowDefinitions();
    if (!defs.empty()) {
        return defs.size();
    }

    std::size_t maxIndex = 0;
    bool hasChild = false;

    for (const auto& child : children) {
        if (!child) {
            continue;
        }
        const int row = std::max(0, GetRow(child.get()));
        const int span = NormalizeSpan(GetRowSpan(child.get()));
        const std::size_t end = static_cast<std::size_t>(row) + static_cast<std::size_t>(span);
        if (end == 0) {
            continue;
        }
        hasChild = true;
        const std::size_t last = end - 1;
        if (last > maxIndex) {
            maxIndex = last;
        }
    }

    return hasChild ? maxIndex + 1 : 1;
}

std::size_t Grid::ResolveColumnCount(std::span<const std::shared_ptr<UIElement>> children) const {
    const auto& defs = GetColumnDefinitions();
    if (!defs.empty()) {
        return defs.size();
    }

    std::size_t maxIndex = 0;
    bool hasChild = false;

    for (const auto& child : children) {
        if (!child) {
            continue;
        }
        const int column = std::max(0, GetColumn(child.get()));
        const int span = NormalizeSpan(GetColumnSpan(child.get()));
        const std::size_t end = static_cast<std::size_t>(column) + static_cast<std::size_t>(span);
        if (end == 0) {
            continue;
        }
        hasChild = true;
        const std::size_t last = end - 1;
        if (last > maxIndex) {
            maxIndex = last;
        }
    }

    return hasChild ? maxIndex + 1 : 1;
}

int Grid::NormalizeIndex(int index, std::size_t count) {
    if (count == 0) {
        return 0;
    }
    if (index < 0) {
        return 0;
    }
    const auto maxIndex = static_cast<int>(count - 1);
    if (index > maxIndex) {
        return maxIndex;
    }
    return index;
}

int Grid::NormalizeSpan(int span) {
    return span < 1 ? 1 : span;
}

float Grid::ClampWithLimits(float value, float minValue, float maxValue) {
    if (value < minValue) {
        value = minValue;
    }
    if (std::isfinite(maxValue) && value > maxValue) {
        value = maxValue;
    }
    return value;
}

} // namespace fk::ui
