#include "fk/ui/layouts/Grid.h"
#include "fk/ui/graphics/Brush.h"
#include "fk/render/RenderContext.h"
#include <algorithm>
#include <limits>
#include <sstream>
#include <cctype>
#include <iostream>

namespace fk::ui {

// ========== 模板显式实例�?==========
template class FrameworkElement<Grid>;
template class Panel<Grid>;

// ========== DependencyProperty 注册 ==========

const binding::DependencyProperty& Grid::RowProperty() {
    static auto& property = binding::DependencyProperty::RegisterAttached(
        "Row",
        typeid(int),
        typeid(Grid),
        {0}
    );
    return property;
}

const binding::DependencyProperty& Grid::ColumnProperty() {
    static auto& property = binding::DependencyProperty::RegisterAttached(
        "Column",
        typeid(int),
        typeid(Grid),
        {0}
    );
    return property;
}

const binding::DependencyProperty& Grid::RowSpanProperty() {
    static auto& property = binding::DependencyProperty::RegisterAttached(
        "RowSpan",
        typeid(int),
        typeid(Grid),
        {1}
    );
    return property;
}

const binding::DependencyProperty& Grid::ColumnSpanProperty() {
    static auto& property = binding::DependencyProperty::RegisterAttached(
        "ColumnSpan",
        typeid(int),
        typeid(Grid),
        {1}
    );
    return property;
}

// ========== 行列定义管理 ==========

Grid* Grid::AddRowDefinition(const RowDefinition& row) {
    rowDefinitions_.push_back(row);
    measureCacheValid_ = false;
    InvalidateMeasure();
    return this;
}

Grid* Grid::AddColumnDefinition(const ColumnDefinition& col) {
    columnDefinitions_.push_back(col);
    measureCacheValid_ = false;
    InvalidateMeasure();
    return this;
}

Grid* Grid::RowDefinitions(std::initializer_list<RowDefinition> rows) {
    for (const auto& row : rows) {
        rowDefinitions_.push_back(row);
    }
    measureCacheValid_ = false;
    InvalidateMeasure();
    return this;
}

Grid* Grid::ColumnDefinitions(std::initializer_list<ColumnDefinition> cols) {
    for (const auto& col : cols) {
        columnDefinitions_.push_back(col);
    }
    measureCacheValid_ = false;
    InvalidateMeasure();
    return this;
}

Grid* Grid::Rows(const std::string& spec) {
    auto rows = ParseRowSpec(spec);
    for (const auto& row : rows) {
        rowDefinitions_.push_back(row);
    }
    measureCacheValid_ = false;
    InvalidateMeasure();
    return this;
}

Grid* Grid::Columns(const std::string& spec) {
    auto cols = ParseColumnSpec(spec);
    for (const auto& col : cols) {
        columnDefinitions_.push_back(col);
    }
    measureCacheValid_ = false;
    InvalidateMeasure();
    return this;
}

// ========== 附加属性访问器 ==========

void Grid::SetRow(UIElement* element, int row) {
    if (element) {
        element->SetValue(RowProperty(), row);
    }
}

int Grid::GetRow(UIElement* element) {
    if (element) {
        auto value = element->GetValue(RowProperty());
        if (value.has_value()) {
            return std::any_cast<int>(value);
        }
    }
    return 0;
}

void Grid::SetColumn(UIElement* element, int col) {
    if (element) {
        element->SetValue(ColumnProperty(), col);
    }
}

int Grid::GetColumn(UIElement* element) {
    if (element) {
        auto value = element->GetValue(ColumnProperty());
        if (value.has_value()) {
            return std::any_cast<int>(value);
        }
    }
    return 0;
}

void Grid::SetRowSpan(UIElement* element, int span) {
    if (element) {
        element->SetValue(RowSpanProperty(), std::max(1, span));
    }
}

int Grid::GetRowSpan(UIElement* element) {
    if (element) {
        auto value = element->GetValue(RowSpanProperty());
        if (value.has_value()) {
            return std::max(1, std::any_cast<int>(value));
        }
    }
    return 1;
}

void Grid::SetColumnSpan(UIElement* element, int span) {
    if (element) {
        element->SetValue(ColumnSpanProperty(), std::max(1, span));
    }
}

int Grid::GetColumnSpan(UIElement* element) {
    if (element) {
        auto value = element->GetValue(ColumnSpanProperty());
        if (value.has_value()) {
            return std::max(1, std::any_cast<int>(value));
        }
    }
    return 1;
}

// ========== 布局算法 ==========

Size Grid::MeasureOverride(const Size& availableSize) {
    // 性能优化：检查缓�?
    if (measureCacheValid_ && 
        cachedAvailableSize_.width == availableSize.width &&
        cachedAvailableSize_.height == availableSize.height) {
        return cachedDesiredSize_;
    }
    
    // 自动创建默认 1* 行列
    if (rowDefinitions_.empty()) {
        rowDefinitions_.push_back(RowDefinition::Star());
    }
    if (columnDefinitions_.empty()) {
        columnDefinitions_.push_back(ColumnDefinition::Star());
    }
    
    // 第一遍：处理 Pixel 尺寸
    for (auto& row : rowDefinitions_) {
        if (row.type == RowDefinition::SizeType::Pixel) {
            row.actualHeight = ApplyConstraints(row.value, row.minHeight, row.maxHeight);
        } else {
            row.actualHeight = 0;
        }
    }
    for (auto& col : columnDefinitions_) {
        if (col.type == ColumnDefinition::SizeType::Pixel) {
            col.actualWidth = ApplyConstraints(col.value, col.minWidth, col.maxWidth);
        } else {
            col.actualWidth = 0;
        }
    }
    
    // 第二遍：测量 Auto 尺寸（基于子元素内容�?
    MeasureAutoRows(availableSize);
    MeasureAutoCols(availableSize);
    
    // 计算已使用的空间
    float usedHeight = 0;
    float usedWidth = 0;
    for (const auto& row : rowDefinitions_) {
        if (row.type != RowDefinition::SizeType::Star) {
            usedHeight += row.actualHeight;
        }
    }
    for (const auto& col : columnDefinitions_) {
        if (col.type != ColumnDefinition::SizeType::Star) {
            usedWidth += col.actualWidth;
        }
    }
    
    // 第三遍：分配 Star 空间（带约束�?
    float remainingHeight = std::max(0.0f, availableSize.height - usedHeight);
    float remainingWidth = std::max(0.0f, availableSize.width - usedWidth);
    
    // std::cout << "Grid::MeasureOverride remainingHeight=" << remainingHeight << " remainingWidth=" << remainingWidth << std::endl;

    DistributeStarRows(remainingHeight);
    DistributeStarCols(remainingWidth);
    
    // 第四遍：测量 Star �?列中的子元素
    // 关键修复：必须测�?Star 单元格中的子元素，否则它们的 DesiredSize �?0
    for (auto* child : children_) {
        if (child && child->GetVisibility() != Visibility::Collapsed) {
            int row = GetRow(child);
            int col = GetColumn(child);
            int rowSpan = GetRowSpan(child);
            int colSpan = GetColumnSpan(child);
            
            // 索引边界检�?
            row = std::clamp(row, 0, static_cast<int>(rowDefinitions_.size()) - 1);
            col = std::clamp(col, 0, static_cast<int>(columnDefinitions_.size()) - 1);
            int rowEnd = std::min(row + rowSpan, static_cast<int>(rowDefinitions_.size()));
            int colEnd = std::min(col + colSpan, static_cast<int>(columnDefinitions_.size()));
            
            // 检查是否在 Star 行或 Star 列中
            bool inStarRow = false;
            bool inStarCol = false;
            for (int r = row; r < rowEnd; ++r) {
                if (rowDefinitions_[r].type == RowDefinition::SizeType::Star) {
                    inStarRow = true;
                    break;
                }
            }
            for (int c = col; c < colEnd; ++c) {
                if (columnDefinitions_[c].type == ColumnDefinition::SizeType::Star) {
                    inStarCol = true;
                    break;
                }
            }
            
            // 如果�?Star 行或列中，现在才测量
            if (inStarRow || inStarCol) {
                // 计算可用空间（Star 空间已分配）
                float cellWidth = 0;
                float cellHeight = 0;
                for (int c = col; c < colEnd; ++c) {
                    cellWidth += columnDefinitions_[c].actualWidth;
                }
                for (int r = row; r < rowEnd; ++r) {
                    cellHeight += rowDefinitions_[r].actualHeight;
                }
                
                // std::cout << "Grid::MeasureOverride StarChild cell=" << cellWidth << "x" << cellHeight << std::endl;

                Size childConstraint(cellWidth, cellHeight);
                child->Measure(childConstraint);
            }
        }
    }
    
    // 计算总尺�?
    float totalHeight = 0;
    float totalWidth = 0;
    for (const auto& row : rowDefinitions_) {
        totalHeight += row.actualHeight;
    }
    for (const auto& col : columnDefinitions_) {
        totalWidth += col.actualWidth;
    }
    
    Size desiredSize(totalWidth, totalHeight);
    
    // Grid 应该填满父容�?与StackPanel不同)
    // 如果availableSize是有限的,返回它而非内容尺寸
    if (std::isfinite(availableSize.width)) {
        desiredSize.width = availableSize.width;
    }
    if (std::isfinite(availableSize.height)) {
        desiredSize.height = availableSize.height;
    }
    
    // 缓存结果
    cachedAvailableSize_ = availableSize;
    cachedDesiredSize_ = desiredSize;
    measureCacheValid_ = true;
    
    return desiredSize;
}

Size Grid::ArrangeOverride(const Size& finalSize) {
    // 重新分配Star行列以适应finalSize
    // 计算已使用的固定空间(Auto和Pixel)
    float usedHeight = 0;
    float usedWidth = 0;
    for (const auto& row : rowDefinitions_) {
        if (row.type != RowDefinition::SizeType::Star) {
            usedHeight += row.actualHeight;
        }
    }
    for (const auto& col : columnDefinitions_) {
        if (col.type != ColumnDefinition::SizeType::Star) {
            usedWidth += col.actualWidth;
        }
    }
    
    // 重新分配Star空间
    float remainingHeight = std::max(0.0f, finalSize.height - usedHeight);
    float remainingWidth = std::max(0.0f, finalSize.width - usedWidth);
    DistributeStarRows(remainingHeight);
    DistributeStarCols(remainingWidth);
    
    // 重新测量Star�?列中的子元素,因为Star尺寸可能在Arrange阶段改变
    // 注意:只有在子元素的HorizontalAlignment==Stretch或VerticalAlignment==Stretch时才需要重新测�?
    for (auto* child : children_) {
        if (child && child->GetVisibility() != Visibility::Collapsed) {
            int row = GetRow(child);
            int col = GetColumn(child);
            int rowSpan = GetRowSpan(child);
            int colSpan = GetColumnSpan(child);
            
            row = std::clamp(row, 0, static_cast<int>(rowDefinitions_.size()) - 1);
            col = std::clamp(col, 0, static_cast<int>(columnDefinitions_.size()) - 1);
            int rowEnd = std::min(row + rowSpan, static_cast<int>(rowDefinitions_.size()));
            int colEnd = std::min(col + colSpan, static_cast<int>(columnDefinitions_.size()));
            
            // 检查是否在Star行或Star列中
            bool inStarRow = false;
            bool inStarCol = false;
            for (int r = row; r < rowEnd; ++r) {
                if (rowDefinitions_[r].type == RowDefinition::SizeType::Star) {
                    inStarRow = true;
                    break;
                }
            }
            for (int c = col; c < colEnd; ++c) {
                if (columnDefinitions_[c].type == ColumnDefinition::SizeType::Star) {
                    inStarCol = true;
                    break;
                }
            }
            
            // 如果在Star行或列中,并且子元素是Stretch对齐,才用新的单元格尺寸重新测�?
            bool needRemeasure = false;
            float cellWidth = std::numeric_limits<float>::infinity();
            float cellHeight = std::numeric_limits<float>::infinity();
            
            if (inStarCol && child->GetHorizontalAlignment() == HorizontalAlignment::Stretch) {
                cellWidth = 0;
                for (int c = col; c < colEnd; ++c) {
                    cellWidth += columnDefinitions_[c].actualWidth;
                }
                needRemeasure = true;
            }
            
            if (inStarRow && child->GetVerticalAlignment() == VerticalAlignment::Stretch) {
                cellHeight = 0;
                for (int r = row; r < rowEnd; ++r) {
                    cellHeight += rowDefinitions_[r].actualHeight;
                }
                needRemeasure = true;
            }
            
            if (needRemeasure) {
                child->Measure(Size(cellWidth, cellHeight));
            }
        }
    }
    
    // 计算每个单元格的偏移位置
    std::vector<float> rowOffsets(rowDefinitions_.size() + 1, 0);
    std::vector<float> colOffsets(columnDefinitions_.size() + 1, 0);
    
    for (size_t i = 0; i < rowDefinitions_.size(); ++i) {
        rowOffsets[i + 1] = rowOffsets[i] + rowDefinitions_[i].actualHeight;
    }
    for (size_t i = 0; i < columnDefinitions_.size(); ++i) {
        colOffsets[i + 1] = colOffsets[i] + columnDefinitions_[i].actualWidth;
    }
    
    // 获取 Padding（子元素需要排列在内容区域�?
    auto padding = GetPadding();
    
    // 排列子元素（支持对齐和边距）
    for (auto* child : children_) {
        if (child && child->GetVisibility() != Visibility::Collapsed) {
            // 获取 Grid 附加属�?
            int row = GetRow(child);
            int col = GetColumn(child);
            int rowSpan = GetRowSpan(child);
            int colSpan = GetColumnSpan(child);
            
            // 索引边界检�?
            row = std::clamp(row, 0, static_cast<int>(rowDefinitions_.size()) - 1);
            col = std::clamp(col, 0, static_cast<int>(columnDefinitions_.size()) - 1);
            int rowEnd = std::min(row + rowSpan, static_cast<int>(rowDefinitions_.size()));
            int colEnd = std::min(col + colSpan, static_cast<int>(columnDefinitions_.size()));
            
            // 计算单元格范�?
            float cellX = colOffsets[col];
            float cellY = rowOffsets[row];
            float cellWidth = colOffsets[colEnd] - cellX;
            float cellHeight = rowOffsets[rowEnd] - cellY;
            
            // 获取子元素的 Margin �?Alignment
            auto margin = child->GetMargin();
            auto hAlign = child->GetHorizontalAlignment();
            auto vAlign = child->GetVerticalAlignment();
            Size childDesired = child->GetDesiredSize();
            
            // 减去 Margin
            float availableWidth = std::max(0.0f, cellWidth - margin.left - margin.right);
            float availableHeight = std::max(0.0f, cellHeight - margin.top - margin.bottom);
            
            // 根据 HorizontalAlignment 计算宽度�?X 位置
            float childWidth, childX;
            switch (hAlign) {
                case HorizontalAlignment::Stretch:
                    childWidth = availableWidth;
                    childX = cellX + margin.left;
                    break;
                case HorizontalAlignment::Left:
                    childWidth = std::min(childDesired.width, availableWidth);
                    childX = cellX + margin.left;
                    break;
                case HorizontalAlignment::Center:
                    childWidth = std::min(childDesired.width, availableWidth);
                    childX = cellX + margin.left + (availableWidth - childWidth) / 2.0f;
                    break;
                case HorizontalAlignment::Right:
                    childWidth = std::min(childDesired.width, availableWidth);
                    childX = cellX + margin.left + availableWidth - childWidth;
                    break;
                default:
                    childWidth = availableWidth;
                    childX = cellX + margin.left;
                    break;
            }
            
            // 根据 VerticalAlignment 计算高度�?Y 位置
            float childHeight, childY;
            switch (vAlign) {
                case VerticalAlignment::Stretch:
                    childHeight = availableHeight;
                    childY = cellY + margin.top;
                    break;
                case VerticalAlignment::Top:
                    childHeight = std::min(childDesired.height, availableHeight);
                    childY = cellY + margin.top;
                    break;
                case VerticalAlignment::Center:
                    childHeight = std::min(childDesired.height, availableHeight);
                    childY = cellY + margin.top + (availableHeight - childHeight) / 2.0f;
                    break;
                case VerticalAlignment::Bottom:
                    childHeight = std::min(childDesired.height, availableHeight);
                    childY = cellY + margin.top + availableHeight - childHeight;
                    break;
                default:
                    childHeight = availableHeight;
                    childY = cellY + margin.top;
                    break;
            }
            
            // 加上 Padding 偏移（子元素坐标相对�?Panel �?(0,0)�?
            child->Arrange(Rect(padding.left + childX, padding.top + childY, childWidth, childHeight));
        }
    }
    
    return finalSize;
}

void Grid::MeasureAutoRows(const Size& availableSize) {
    // 测量所有子元素以确�?Auto 行的高度
    for (auto* child : children_) {
        if (child && child->GetVisibility() != Visibility::Collapsed) {
            int row = GetRow(child);
            int rowSpan = GetRowSpan(child);
            
            // 索引边界检�?
            row = std::clamp(row, 0, static_cast<int>(rowDefinitions_.size()) - 1);
            int rowEnd = std::min(row + rowSpan, static_cast<int>(rowDefinitions_.size()));
            
            // 仅处理单行的 Auto 元素（多行跨度更复杂，简化处理）
            if (rowSpan == 1 && rowDefinitions_[row].type == RowDefinition::SizeType::Auto) {
                // 给子元素无限高度空间进行测量
                Size childConstraint(availableSize.width, std::numeric_limits<float>::infinity());
                child->Measure(childConstraint);
                
                Size childDesired = child->GetDesiredSize();
                auto margin = child->GetMargin();
                float requiredHeight = childDesired.height + margin.top + margin.bottom;
                
                // 应用约束
                requiredHeight = ApplyConstraints(requiredHeight, 
                                                 rowDefinitions_[row].minHeight, 
                                                 rowDefinitions_[row].maxHeight);
                
                // 更新行高（取最大值）
                rowDefinitions_[row].actualHeight = std::max(
                    rowDefinitions_[row].actualHeight, 
                    requiredHeight
                );
            }
        }
    }
    
    // 确保 Auto 行符合约�?
    for (auto& row : rowDefinitions_) {
        if (row.type == RowDefinition::SizeType::Auto) {
            row.actualHeight = ApplyConstraints(row.actualHeight, row.minHeight, row.maxHeight);
        }
    }
}

void Grid::MeasureAutoCols(const Size& availableSize) {
    // 测量所有子元素以确�?Auto 列的宽度
    for (auto* child : children_) {
        if (child && child->GetVisibility() != Visibility::Collapsed) {
            int col = GetColumn(child);
            int colSpan = GetColumnSpan(child);
            
            // 索引边界检�?
            col = std::clamp(col, 0, static_cast<int>(columnDefinitions_.size()) - 1);
            int colEnd = std::min(col + colSpan, static_cast<int>(columnDefinitions_.size()));
            
            // 仅处理单列的 Auto 元素
            if (colSpan == 1 && columnDefinitions_[col].type == ColumnDefinition::SizeType::Auto) {
                // 给子元素无限宽度空间进行测量
                Size childConstraint(std::numeric_limits<float>::infinity(), availableSize.height);
                child->Measure(childConstraint);
                
                Size childDesired = child->GetDesiredSize();
                auto margin = child->GetMargin();
                float requiredWidth = childDesired.width + margin.left + margin.right;
                
                // 应用约束
                requiredWidth = ApplyConstraints(requiredWidth, 
                                                columnDefinitions_[col].minWidth, 
                                                columnDefinitions_[col].maxWidth);
                
                // 更新列宽（取最大值）
                columnDefinitions_[col].actualWidth = std::max(
                    columnDefinitions_[col].actualWidth, 
                    requiredWidth
                );
            }
        }
    }
    
    // 确保 Auto 列符合约�?
    for (auto& col : columnDefinitions_) {
        if (col.type == ColumnDefinition::SizeType::Auto) {
            col.actualWidth = ApplyConstraints(col.actualWidth, col.minWidth, col.maxWidth);
        }
    }
}

void Grid::DistributeStarRows(float availableHeight) {
    // 计算�?Star 权重
    float totalStars = 0;
    std::vector<size_t> starIndices;
    for (size_t i = 0; i < rowDefinitions_.size(); ++i) {
        if (rowDefinitions_[i].type == RowDefinition::SizeType::Star) {
            totalStars += rowDefinitions_[i].value;
            starIndices.push_back(i);
        }
    }
    
    if (totalStars <= 0 || availableHeight <= 0 || starIndices.empty()) {
        return;
    }
    
    // 按比例分配空间（带约束支持）
    float remainingHeight = availableHeight;
    float remainingStars = totalStars;
    
    // 多遍分配，处理约�?
    for (int pass = 0; pass < 2 && !starIndices.empty(); ++pass) {
        float heightPerStar = remainingHeight / remainingStars;
        
        auto it = starIndices.begin();
        while (it != starIndices.end()) {
            size_t i = *it;
            auto& row = rowDefinitions_[i];
            float idealHeight = row.value * heightPerStar;
            float constrainedHeight = ApplyConstraints(idealHeight, row.minHeight, row.maxHeight);
            
            // 如果受约束限制，固定该行并重新分�?
            if (std::abs(constrainedHeight - idealHeight) > 0.01f) {
                row.actualHeight = constrainedHeight;
                remainingHeight -= constrainedHeight;
                remainingStars -= row.value;
                it = starIndices.erase(it);
            } else {
                // 即使没有触发约束，也暂时记录计算出的高度
                // 但不从列表中移除，因为后续可能会因为其他行被固定而重新分�?
                row.actualHeight = idealHeight;
                ++it;
            }
        }
    }
}

void Grid::DistributeStarCols(float availableWidth) {
    // 计算�?Star 权重
    float totalStars = 0;
    std::vector<size_t> starIndices;
    for (size_t i = 0; i < columnDefinitions_.size(); ++i) {
        if (columnDefinitions_[i].type == ColumnDefinition::SizeType::Star) {
            totalStars += columnDefinitions_[i].value;
            starIndices.push_back(i);
        }
    }
    
    if (totalStars <= 0 || availableWidth <= 0 || starIndices.empty()) {
        return;
    }
    
    // 按比例分配空间（带约束支持）
    float remainingWidth = availableWidth;
    float remainingStars = totalStars;
    
    // 多遍分配，处理约�?
    for (int pass = 0; pass < 2 && !starIndices.empty(); ++pass) {
        float widthPerStar = remainingWidth / remainingStars;
        
        auto it = starIndices.begin();
        while (it != starIndices.end()) {
            size_t i = *it;
            auto& col = columnDefinitions_[i];
            float idealWidth = col.value * widthPerStar;
            float constrainedWidth = ApplyConstraints(idealWidth, col.minWidth, col.maxWidth);
            
            // 如果受约束限制，固定该列并重新分�?
            if (std::abs(constrainedWidth - idealWidth) > 0.01f) {
                col.actualWidth = constrainedWidth;
                remainingWidth -= constrainedWidth;
                remainingStars -= col.value;
                it = starIndices.erase(it);
            } else {
                col.actualWidth = idealWidth;
                ++it;
            }
        }
    }
}

// ========== 辅助方法 ==========

float Grid::ApplyConstraints(float value, float minValue, float maxValue) {
    return std::clamp(value, minValue, maxValue);
}

std::vector<RowDefinition> Grid::ParseRowSpec(const std::string& spec) {
    std::vector<RowDefinition> rows;
    std::istringstream stream(spec);
    std::string token;
    
    while (std::getline(stream, token, ',')) {
        // 移除首尾空格
        size_t start = token.find_first_not_of(" \t");
        size_t end = token.find_last_not_of(" \t");
        if (start == std::string::npos) continue;
        token = token.substr(start, end - start + 1);
        
        if (token.empty()) continue;
        
        // 解析不同类型
        if (token == "Auto" || token == "auto") {
            rows.push_back(RowDefinition::Auto());
        } else if (token == "*") {
            rows.push_back(RowDefinition::Star(1.0f));
        } else if (token.back() == '*') {
            // 解析 "2*" 格式
            std::string numStr = token.substr(0, token.length() - 1);
            try {
                float stars = numStr.empty() ? 1.0f : std::stof(numStr);
                rows.push_back(RowDefinition::Star(stars));
            } catch (...) {
                rows.push_back(RowDefinition::Star(1.0f));
            }
        } else {
            // 解析数字（像素）
            try {
                float pixels = std::stof(token);
                rows.push_back(RowDefinition::Pixel(pixels));
            } catch (...) {
                rows.push_back(RowDefinition::Auto());
            }
        }
    }
    
    return rows;
}

std::vector<ColumnDefinition> Grid::ParseColumnSpec(const std::string& spec) {
    std::vector<ColumnDefinition> cols;
    std::istringstream stream(spec);
    std::string token;
    
    while (std::getline(stream, token, ',')) {
        // 移除首尾空格
        size_t start = token.find_first_not_of(" \t");
        size_t end = token.find_last_not_of(" \t");
        if (start == std::string::npos) continue;
        token = token.substr(start, end - start + 1);
        
        if (token.empty()) continue;
        
        // 解析不同类型
        if (token == "Auto" || token == "auto") {
            cols.push_back(ColumnDefinition::Auto());
        } else if (token == "*") {
            cols.push_back(ColumnDefinition::Star(1.0f));
        } else if (token.back() == '*') {
            // 解析 "2*" 格式
            std::string numStr = token.substr(0, token.length() - 1);
            try {
                float stars = numStr.empty() ? 1.0f : std::stof(numStr);
                cols.push_back(ColumnDefinition::Star(stars));
            } catch (...) {
                cols.push_back(ColumnDefinition::Star(1.0f));
            }
        } else {
            // 解析数字（像素）
            try {
                float pixels = std::stof(token);
                cols.push_back(ColumnDefinition::Pixel(pixels));
            } catch (...) {
                cols.push_back(ColumnDefinition::Auto());
            }
        }
    }
    
    return cols;
}

// ========== 全局辅助函数（供 UIElement 流式方法调用�?=========

void SetGridRow(UIElement* element, int row) {
    Grid::SetRow(element, row);
}

void SetGridColumn(UIElement* element, int col) {
    Grid::SetColumn(element, col);
}

void SetGridRowSpan(UIElement* element, int span) {
    Grid::SetRowSpan(element, span);
}

void SetGridColumnSpan(UIElement* element, int span) {
    Grid::SetColumnSpan(element, span);
}

void Grid::OnRender(render::RenderContext& context) {
    // 辅助函数：将 Brush 转换�?RenderContext 颜色格式
    auto brushToColor = [](Brush* brush) -> std::array<float, 4> {
        if (!brush) return {{0.0f, 0.0f, 0.0f, 0.0f}};  // 透明
        if (auto solidBrush = dynamic_cast<SolidColorBrush*>(brush)) {
            auto color = solidBrush->GetColor();
            return {{color.r, color.g, color.b, color.a}};
        }
        return {{0.0f, 0.0f, 0.0f, 0.0f}};
    };
    
    // 获取背景画刷
    auto background = GetBackground();
    if (!background) {
        return; // 没有背景，不需要绘�?
    }
    
    // 背景应该覆盖整个布局区域（包括Padding�?
    auto layoutRect = GetLayoutRect();
    Rect rect(0, 0, layoutRect.width, layoutRect.height);
    
    // 获取圆角
    auto cornerRadius = GetCornerRadius();
    
    // 转换为颜�?
    std::array<float, 4> fillColor = brushToColor(background);
    std::array<float, 4> strokeColor = {{0.0f, 0.0f, 0.0f, 0.0f}}; // 无边�?
    
    // 绘制背景矩形
    context.DrawBorder(rect, fillColor, strokeColor, 0.0f,
                      cornerRadius.topLeft, cornerRadius.topRight,
                      cornerRadius.bottomRight, cornerRadius.bottomLeft);
}

} // namespace fk::ui
