#include "fk/ui/Grid.h"
#include <algorithm>
#include <unordered_map>

namespace fk::ui {

// 附加属性存储（简化实现，使用全局映射）
static std::unordered_map<UIElement*, int> g_rowMap;
static std::unordered_map<UIElement*, int> g_columnMap;
static std::unordered_map<UIElement*, int> g_rowSpanMap;
static std::unordered_map<UIElement*, int> g_columnSpanMap;

void Grid::AddRowDefinition(const RowDefinition& row) {
    rowDefinitions_.push_back(row);
    InvalidateMeasure();
}

void Grid::AddColumnDefinition(const ColumnDefinition& col) {
    columnDefinitions_.push_back(col);
    InvalidateMeasure();
}

Grid* Grid::RowDefinitions(std::initializer_list<RowDefinition> rows) {
    for (const auto& row : rows) {
        AddRowDefinition(row);
    }
    return this;
}

Grid* Grid::ColumnDefinitions(std::initializer_list<ColumnDefinition> cols) {
    for (const auto& col : cols) {
        AddColumnDefinition(col);
    }
    return this;
}

void Grid::SetRow(UIElement* element, int row) {
    if (element) g_rowMap[element] = row;
}

int Grid::GetRow(UIElement* element) {
    if (element && g_rowMap.count(element)) {
        return g_rowMap[element];
    }
    return 0;
}

void Grid::SetColumn(UIElement* element, int col) {
    if (element) g_columnMap[element] = col;
}

int Grid::GetColumn(UIElement* element) {
    if (element && g_columnMap.count(element)) {
        return g_columnMap[element];
    }
    return 0;
}

void Grid::SetRowSpan(UIElement* element, int span) {
    if (element) g_rowSpanMap[element] = span;
}

int Grid::GetRowSpan(UIElement* element) {
    if (element && g_rowSpanMap.count(element)) {
        return g_rowSpanMap[element];
    }
    return 1;
}

void Grid::SetColumnSpan(UIElement* element, int span) {
    if (element) g_columnSpanMap[element] = span;
}

int Grid::GetColumnSpan(UIElement* element) {
    if (element && g_columnSpanMap.count(element)) {
        return g_columnSpanMap[element];
    }
    return 1;
}

Size Grid::MeasureOverride(const Size& availableSize) {
    // 简化实现：三遍测量
    // 1. 测量所有 Auto 和 Pixel 尺寸的行列
    // 2. 分配剩余空间给 Star 行列
    // 3. 测量所有子元素
    
    if (rowDefinitions_.empty()) {
        rowDefinitions_.push_back(RowDefinition::Star());
    }
    if (columnDefinitions_.empty()) {
        columnDefinitions_.push_back(ColumnDefinition::Star());
    }
    
    // 第一遍：处理 Pixel 和 Auto
    MeasureAutoRows(availableSize);
    MeasureAutoCols(availableSize);
    
    // 计算剩余空间
    float usedHeight = 0;
    float usedWidth = 0;
    for (auto& row : rowDefinitions_) {
        if (row.type != RowDefinition::SizeType::Star) {
            usedHeight += row.actualHeight;
        }
    }
    for (auto& col : columnDefinitions_) {
        if (col.type != ColumnDefinition::SizeType::Star) {
            usedWidth += col.actualWidth;
        }
    }
    
    // 第二遍：分配 Star 空间
    DistributeStarRows(availableSize.height - usedHeight);
    DistributeStarCols(availableSize.width - usedWidth);
    
    // 计算总尺寸
    float totalHeight = 0;
    float totalWidth = 0;
    for (const auto& row : rowDefinitions_) {
        totalHeight += row.actualHeight;
    }
    for (const auto& col : columnDefinitions_) {
        totalWidth += col.actualWidth;
    }
    
    return Size(totalWidth, totalHeight);
}

Size Grid::ArrangeOverride(const Size& finalSize) {
    // 计算每个单元格的位置
    std::vector<float> rowOffsets(rowDefinitions_.size() + 1, 0);
    std::vector<float> colOffsets(columnDefinitions_.size() + 1, 0);
    
    for (size_t i = 0; i < rowDefinitions_.size(); ++i) {
        rowOffsets[i + 1] = rowOffsets[i] + rowDefinitions_[i].actualHeight;
    }
    for (size_t i = 0; i < columnDefinitions_.size(); ++i) {
        colOffsets[i + 1] = colOffsets[i] + columnDefinitions_[i].actualWidth;
    }
    
    // 排列子元素
    for (auto* child : children_) {
        if (child && child->GetVisibility() != Visibility::Collapsed) {
            int row = GetRow(child);
            int col = GetColumn(child);
            int rowSpan = GetRowSpan(child);
            int colSpan = GetColumnSpan(child);
            
            // 确保索引有效
            row = std::max(0, std::min(row, static_cast<int>(rowDefinitions_.size()) - 1));
            col = std::max(0, std::min(col, static_cast<int>(columnDefinitions_.size()) - 1));
            
            float x = colOffsets[col];
            float y = rowOffsets[row];
            float width = colOffsets[std::min(col + colSpan, static_cast<int>(columnDefinitions_.size()))] - x;
            float height = rowOffsets[std::min(row + rowSpan, static_cast<int>(rowDefinitions_.size()))] - y;
            
            child->Arrange(Rect(x, y, width, height));
        }
    }
    
    return finalSize;
}

void Grid::MeasureAutoRows(const Size& availableSize) {
    for (auto& row : rowDefinitions_) {
        if (row.type == RowDefinition::SizeType::Pixel) {
            row.actualHeight = row.value;
        } else if (row.type == RowDefinition::SizeType::Auto) {
            row.actualHeight = 0; // 后续根据内容计算
        }
    }
}

void Grid::MeasureAutoCols(const Size& availableSize) {
    for (auto& col : columnDefinitions_) {
        if (col.type == ColumnDefinition::SizeType::Pixel) {
            col.actualWidth = col.value;
        } else if (col.type == ColumnDefinition::SizeType::Auto) {
            col.actualWidth = 0; // 后续根据内容计算
        }
    }
}

void Grid::DistributeStarRows(float availableHeight) {
    float totalStars = 0;
    for (const auto& row : rowDefinitions_) {
        if (row.type == RowDefinition::SizeType::Star) {
            totalStars += row.value;
        }
    }
    
    if (totalStars > 0 && availableHeight > 0) {
        float heightPerStar = availableHeight / totalStars;
        for (auto& row : rowDefinitions_) {
            if (row.type == RowDefinition::SizeType::Star) {
                row.actualHeight = row.value * heightPerStar;
            }
        }
    }
}

void Grid::DistributeStarCols(float availableWidth) {
    float totalStars = 0;
    for (const auto& col : columnDefinitions_) {
        if (col.type == ColumnDefinition::SizeType::Star) {
            totalStars += col.value;
        }
    }
    
    if (totalStars > 0 && availableWidth > 0) {
        float widthPerStar = availableWidth / totalStars;
        for (auto& col : columnDefinitions_) {
            if (col.type == ColumnDefinition::SizeType::Star) {
                col.actualWidth = col.value * widthPerStar;
            }
        }
    }
}

} // namespace fk::ui
