#pragma once

#include "fk/ui/Control.h"
#include "fk/ui/Brush.h"
#include <string>
#include <vector>
#include <memory>

namespace fk {
namespace ui {

// Forward declarations
class TextSelection;
class Caret;
class UndoRedoStack;

/**
 * @brief 文本输入框的抽象基类
 * 
 * 提供文本输入、光标管理、文本选择、撤销重做等共同功能。
 * 子类包括 TextBox（单行）、PasswordBox（密码）、RichTextBox（富文本）
 */
class TextBoxBase : public Control {
protected:
    // 构造函数（保护，只能被子类使用）
    TextBoxBase();

public:
    virtual ~TextBoxBase();

    // ========== 依赖属性 ==========
    
    /// Text 属性 - 文本内容
    static DependencyProperty* TextProperty;
    
    /// MaxLength 属性 - 最大长度（0 表示无限制）
    static DependencyProperty* MaxLengthProperty;
    
    /// IsReadOnly 属性 - 是否只读
    static DependencyProperty* IsReadOnlyProperty;
    
    /// CaretIndex 属性 - 光标位置
    static DependencyProperty* CaretIndexProperty;
    
    /// SelectionStart 属性 - 选择起始位置
    static DependencyProperty* SelectionStartProperty;
    
    /// SelectionLength 属性 - 选择长度
    static DependencyProperty* SelectionLengthProperty;
    
    /// CaretBrush 属性 - 光标画刷
    static DependencyProperty* CaretBrushProperty;
    
    /// SelectionBrush 属性 - 选择背景画刷
    static DependencyProperty* SelectionBrushProperty;

    // ========== 属性访问器 ==========
    
    std::string GetText() const;
    void SetText(const std::string& value);
    
    int GetMaxLength() const;
    void SetMaxLength(int value);
    
    bool GetIsReadOnly() const;
    void SetIsReadOnly(bool value);
    
    int GetCaretIndex() const;
    void SetCaretIndex(int value);
    
    int GetSelectionStart() const;
    void SetSelectionStart(int value);
    
    int GetSelectionLength() const;
    void SetSelectionLength(int value);
    
    Brush* GetCaretBrush() const;
    void SetCaretBrush(Brush* value);
    
    Brush* GetSelectionBrush() const;
    void SetSelectionBrush(Brush* value);

    // ========== 公共方法 ==========
    
    /// 获取选中的文本
    std::string GetSelectedText() const;
    
    /// 选择全部文本
    void SelectAll();
    
    /// 清除选择
    void ClearSelection();
    
    /// 剪切选中的文本到剪贴板
    void Cut();
    
    /// 复制选中的文本到剪贴板
    void Copy();
    
    /// 从剪贴板粘贴文本
    void Paste();
    
    /// 撤销上一次操作
    void Undo();
    
    /// 重做上一次撤销的操作
    void Redo();
    
    /// 是否可以撤销
    bool CanUndo() const;
    
    /// 是否可以重做
    bool CanRedo() const;

    // ========== 事件 ==========
    
    /// TextChanged 事件 - 文本改变时触发
    static RoutedEvent* TextChangedEvent;
    
    /// SelectionChanged 事件 - 选择改变时触发
    static RoutedEvent* SelectionChangedEvent;

protected:
    // ========== 虚方法（子类可重写）==========
    
    /// 获取显示的文本（例如 PasswordBox 返回遮罩字符）
    virtual std::string GetDisplayText() const;
    
    /// 验证输入（例如限制数字输入）
    virtual bool ValidateInput(const std::string& input);
    
    /// 处理文本改变
    virtual void OnTextChanged(const std::string& oldText, const std::string& newText);
    
    /// 处理选择改变
    virtual void OnSelectionChanged();

    // ========== 渲染相关 ==========
    
    void CollectDrawCommands(render::RenderContext& context) override;
    Size MeasureOverride(const Size& availableSize) override;
    Size ArrangeOverride(const Size& finalSize) override;
    
    /// 渲染文本内容
    virtual void RenderText(render::RenderContext& context);
    
    /// 渲染文本选择背景
    virtual void RenderSelection(render::RenderContext& context);
    
    /// 渲染光标
    virtual void RenderCaret(render::RenderContext& context);

    // ========== 事件处理 ==========
    
    void OnKeyDown(KeyEventArgs& e) override;
    void OnTextInput(TextInputEventArgs& e) override;
    void OnMouseDown(MouseButtonEventArgs& e) override;
    void OnMouseMove(MouseEventArgs& e) override;
    void OnMouseUp(MouseButtonEventArgs& e) override;
    void OnGotFocus(RoutedEventArgs& e) override;
    void OnLostFocus(RoutedEventArgs& e) override;

    // ========== 内部方法 ==========
    
    /// 插入文本到光标位置
    void InsertText(const std::string& text);
    
    /// 删除选中的文本
    void DeleteSelection();
    
    /// 删除光标后的字符（Delete 键）
    void DeleteForward();
    
    /// 删除光标前的字符（Backspace 键）
    void DeleteBackward();
    
    /// 移动光标
    void MoveCaretLeft(bool extendSelection = false);
    void MoveCaretRight(bool extendSelection = false);
    void MoveCaretToStart(bool extendSelection = false);
    void MoveCaretToEnd(bool extendSelection = false);
    
    /// 计算光标的屏幕位置
    Point CalculateCaretPosition(int index) const;
    
    /// 根据屏幕位置计算最近的文本索引
    int HitTestTextPosition(const Point& point) const;
    
    /// 更新光标位置（确保在有效范围内）
    void UpdateCaretIndex();
    
    /// 开始光标闪烁
    void StartCaretBlink();
    
    /// 停止光标闪烁
    void StopCaretBlink();

    // ========== 成员变量 ==========
    
    /// 文本内容（内部使用）
    std::string text_;
    
    /// 光标位置
    int caretIndex_;
    
    /// 选择起始位置
    int selectionStart_;
    
    /// 选择长度
    int selectionLength_;
    
    /// 光标是否可见
    bool caretVisible_;
    
    /// 是否正在选择文本（鼠标拖拽）
    bool isSelecting_;
    
    /// 撤销重做栈
    std::unique_ptr<UndoRedoStack> undoRedoStack_;
    
    // 注册依赖属性和事件
    static void RegisterDependencyProperties();
    static void RegisterEvents();
    
private:
    // 属性改变回调
    static void OnTextPropertyChanged(DependencyObject* d, const DependencyPropertyChangedEventArgs& e);
    static void OnCaretIndexPropertyChanged(DependencyObject* d, const DependencyPropertyChangedEventArgs& e);
    static void OnSelectionPropertyChanged(DependencyObject* d, const DependencyPropertyChangedEventArgs& e);
};

} // namespace ui
} // namespace fk
