#pragma once

#include "fk/ui/FrameworkElement.h"
#include "fk/binding/DependencyProperty.h"
#include <string>
#include <memory>

namespace fk::ui {

// 前向声明
class RenderContext;

/**
 * @brief 图像拉伸模式
 */
enum class Stretch {
    None,               // 不拉伸，保持原始尺寸
    Fill,               // 填充，可能变形
    Uniform,            // 等比例缩放，保持宽高比，完全可见
    UniformToFill       // 等比例缩放，保持宽高比，填充满
};

// 前向声明
struct ImageData;

/**
 * @brief 图像控件
 * 
 * 职责：
 * - 显示图像资源
 * - 支持多种拉伸模式
 * - 自适应布局
 * 
 * WPF 对应：Image
 */
class Image : public FrameworkElement<Image> {
public:
    // ========== 依赖属性 ==========
    
    /// Source 属性：图像源路径
    static const binding::DependencyProperty& SourceProperty();
    
    /// Stretch 属性：拉伸模式
    static const binding::DependencyProperty& StretchProperty();

public:
    Image();
    virtual ~Image();

    // ========== 图像源 ==========
    
    /**
     * @brief 获取图像源路径
     */
    std::string GetSource() const {
        return GetValue<std::string>(SourceProperty());
    }
    
    /**
     * @brief 设置图像源路径
     */
    void SetSource(const std::string& path);
    
    /**
     * @brief 链式调用设置图像源
     */
    Image* Source(const std::string& path) {
        SetSource(path);
        return this;
    }
    
    /**
     * @brief 获取图像源路径（无参重载）
     */
    std::string Source() const {
        return GetSource();
    }

    // ========== 拉伸模式 ==========
    
    /**
     * @brief 获取拉伸模式
     */
    ui::Stretch GetStretch() const {
        return GetValue<ui::Stretch>(StretchProperty());
    }
    
    /**
     * @brief 设置拉伸模式
     */
    void SetStretch(ui::Stretch mode) {
        SetValue(StretchProperty(), mode);
        InvalidateMeasure();
    }
    
    /**
     * @brief 链式调用设置拉伸模式
     */
    Image* Stretch(ui::Stretch mode) {
        SetStretch(mode);
        return this;
    }
    
    /**
     * @brief 获取拉伸模式（无参重载）
     */
    ui::Stretch Stretch() const {
        return GetStretch();
    }

    // ========== 图像信息 ==========
    
    /**
     * @brief 获取原始图像宽度
     */
    float GetImageWidth() const;
    
    /**
     * @brief 获取原始图像高度
     */
    float GetImageHeight() const;
    
    /**
     * @brief 检查图像是否已加载
     */
    bool IsLoaded() const;

protected:
    /**
     * @brief 测量控件所需尺寸
     */
    Size MeasureOverride(const Size& availableSize) override;
    
    /**
     * @brief 排列控件位置和大小
     */
    Size ArrangeOverride(const Size& finalSize) override;
    
    /**
     * @brief 收集绘制命令
     */
    void CollectDrawCommands(RenderContext& context) override;

private:
    /**
     * @brief 加载图像
     */
    bool LoadImage(const std::string& path);
    
    /**
     * @brief 卸载图像
     */
    void UnloadImage();
    
    /**
     * @brief 计算渲染尺寸和位置
     * @param containerSize 容器尺寸
     * @return 渲染区域 (x, y, width, height)
     */
    Rect CalculateRenderBounds(const Size& containerSize) const;

private:
    std::unique_ptr<ImageData> imageData_;  // 图像数据（PIMPL模式）
};

} // namespace fk::ui
