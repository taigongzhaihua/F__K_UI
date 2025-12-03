#pragma once

#include "fk/ui/text/TextBoxBase.h"

namespace fk {
namespace ui {

/**
 * @brief 单行文本输入框
 * 
 * 提供单行文本编辑功能，支持：
 * - 文本输入和编辑
 * - 光标管理和闪烁
 * - 文本选择（鼠标和键盘）
 * - 剪切/复制/粘贴
 * - 撤销/重做
 * - 最大长度限制
 * - 只读模式
 */
class TextBox : public TextBoxBase {
public:
    TextBox();
    virtual ~TextBox();

    // ========== CRTP 链式调用 API ==========
    
    TextBox& Text(const std::string& value) {
        SetText(value);
        return *this;
    }
    
    TextBox& MaxLength(int value) {
        SetMaxLength(value);
        return *this;
    }
    
    TextBox& IsReadOnly(bool value) {
        SetIsReadOnly(value);
        return *this;
    }
    
    TextBox& Width(float value) {
        SetWidth(value);
        return *this;
    }
    
    TextBox& Height(float value) {
        SetHeight(value);
        return *this;
    }

protected:
    // ========== 重写基类方法 ==========
    
    /// 获取显示的文本（单行文本直接返回）
    std::string GetDisplayText() const override;
    
    /// 测量控件大小
    Size MeasureOverride(const Size& availableSize) override;
    
    /// 安排控件布局
    Size ArrangeOverride(const Size& finalSize) override;
    
    /// 渲染文本（单行文本，处理水平滚动）
    void RenderText(render::RenderContext& context) override;

private:
    /// 水平滚动偏移（用于显示超出视口的文本）
    float scrollOffset_;
    
    /// 更新滚动偏移以确保光标可见
    void UpdateScrollOffset();
    
    /// 计算文本的可见区域
    std::string GetVisibleText(float& startOffset) const;
};

} // namespace ui
} // namespace fk
