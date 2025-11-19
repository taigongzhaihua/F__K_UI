#pragma once

#include "fk/ui/Panel.h"
#include "fk/ui/GridCellAttacher.h"
#include <vector>
#include <string>
#include <limits>

namespace fk::ui {

/**
 * @brief 网格行定义
 */
struct RowDefinition {
    enum class SizeType { Auto, Pixel, Star };
    
    SizeType type{SizeType::Auto};
    float value{1.0f};
    float actualHeight{0.0f};
    
    // 约束
    float minHeight{0.0f};
    float maxHeight{std::numeric_limits<float>::infinity()};
    
    RowDefinition() = default;
    RowDefinition(SizeType t, float v = 1.0f) : type(t), value(v) {}
    
    static RowDefinition Auto() { return RowDefinition(SizeType::Auto, 0); }
    static RowDefinition Pixel(float pixels) { return RowDefinition(SizeType::Pixel, pixels); }
    static RowDefinition Star(float stars = 1.0f) { return RowDefinition(SizeType::Star, stars); }
    
    // 流式 API 设置约束
    RowDefinition& MinHeight(float min) { minHeight = min; return *this; }
    RowDefinition& MaxHeight(float max) { maxHeight = max; return *this; }
};

/**
 * @brief 网格列定义
 */
struct ColumnDefinition {
    enum class SizeType { Auto, Pixel, Star };
    
    SizeType type{SizeType::Auto};
    float value{1.0f};
    float actualWidth{0.0f};
    
    // 约束
    float minWidth{0.0f};
    float maxWidth{std::numeric_limits<float>::infinity()};
    
    ColumnDefinition() = default;
    ColumnDefinition(SizeType t, float v = 1.0f) : type(t), value(v) {}
    
    static ColumnDefinition Auto() { return ColumnDefinition(SizeType::Auto, 0); }
    static ColumnDefinition Pixel(float pixels) { return ColumnDefinition(SizeType::Pixel, pixels); }
    static ColumnDefinition Star(float stars = 1.0f) { return ColumnDefinition(SizeType::Star, stars); }
    
    // 流式 API 设置约束
    ColumnDefinition& MinWidth(float min) { minWidth = min; return *this; }
    ColumnDefinition& MaxWidth(float max) { maxWidth = max; return *this; }
};

/**
 * @brief 网格面板
 * 
 * 职责：
 * - 按行列网格布局子元素
 * - 支持星号、自动、像素三种尺寸模式
 * 
 * WPF 对应：Grid
 */
class Grid : public Panel<Grid> {
public:
    Grid() = default;
    virtual ~Grid() = default;

    // ========== 行列定义 ==========
    
    Grid* AddRowDefinition(const RowDefinition& row);
    Grid* AddColumnDefinition(const ColumnDefinition& col);
    
    Grid* RowDefinitions(std::initializer_list<RowDefinition> rows);
    Grid* ColumnDefinitions(std::initializer_list<ColumnDefinition> cols);
    
    // 字符串解析便捷方法
    Grid* Rows(const std::string& spec);
    Grid* Columns(const std::string& spec);
    
    const std::vector<RowDefinition>& GetRowDefinitions() const { return rowDefinitions_; }
    const std::vector<ColumnDefinition>& GetColumnDefinitions() const { return columnDefinitions_; }

    // ========== 附加属性 DependencyProperty ==========
    
    static const binding::DependencyProperty& RowProperty();
    static const binding::DependencyProperty& ColumnProperty();
    static const binding::DependencyProperty& RowSpanProperty();
    static const binding::DependencyProperty& ColumnSpanProperty();
    
    // ========== 附加属性访问器 ==========
    
    static void SetRow(UIElement* element, int row);
    static int GetRow(UIElement* element);
    
    static void SetColumn(UIElement* element, int col);
    static int GetColumn(UIElement* element);
    
    static void SetRowSpan(UIElement* element, int span);
    static int GetRowSpan(UIElement* element);
    
    static void SetColumnSpan(UIElement* element, int span);
    static int GetColumnSpan(UIElement* element);

protected:
    Size MeasureOverride(const Size& availableSize) override;
    Size ArrangeOverride(const Size& finalSize) override;
    void OnRender(render::RenderContext& context) override;

private:
    std::vector<RowDefinition> rowDefinitions_;
    std::vector<ColumnDefinition> columnDefinitions_;
    
    // 性能优化缓存
    mutable bool measureCacheValid_{false};
    mutable Size cachedAvailableSize_;
    mutable Size cachedDesiredSize_;
    
    // 内部辅助方法
    void MeasureAutoRows(const Size& availableSize);
    void MeasureAutoCols(const Size& availableSize);
    void DistributeStarRows(float availableHeight);
    void DistributeStarCols(float availableWidth);
    
    // 字符串解析辅助
    static std::vector<RowDefinition> ParseRowSpec(const std::string& spec);
    static std::vector<ColumnDefinition> ParseColumnSpec(const std::string& spec);
    
    // 应用约束
    static float ApplyConstraints(float value, float minValue, float maxValue);
};

} // namespace fk::ui
