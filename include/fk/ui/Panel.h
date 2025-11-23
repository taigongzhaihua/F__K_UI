#pragma once

#include "fk/ui/FrameworkElement.h"
#include "fk/ui/CornerRadius.h"
#include "fk/binding/DependencyProperty.h"
#include <vector>
#include <memory>

namespace fk::ui {

// 前向声明
class Brush;

/**
 * @brief 面板基类（CRTP 模式）
 * 
 * 职责：
 * - 子元素管理
 * - 布局逻辑
 * 
 * 模板参数：Derived - 派生类类型（CRTP）
 * 继承：FrameworkElement<Derived>
 */
template<typename Derived>
class Panel : public FrameworkElement<Derived> {
public:
    // ========== 依赖属性 ==========
    
    /// Background 属性：背景画刷
    static const binding::DependencyProperty& BackgroundProperty();
    
    /// CornerRadius 属性：圆角半径
    static const binding::DependencyProperty& CornerRadiusProperty();
    
    /// ClipToBounds 属性：是否裁剪到边界
    static const binding::DependencyProperty& ClipToBoundsProperty();

public:
    Panel() = default;
    virtual ~Panel() = default;

    // ========== 外观 ==========
    
    Brush* GetBackground() const { return this->template GetValue<Brush*>(BackgroundProperty()); }
    void SetBackground(Brush* value) { this->SetValue(BackgroundProperty(), value); }
    
    Derived* Background(Brush* brush) {
        SetBackground(brush);
        return static_cast<Derived*>(this);
    }
    Brush* Background() const { return GetBackground(); }
    
    // ========== 圆角 ==========
    
    ui::CornerRadius GetCornerRadius() const { return this->template GetValue<ui::CornerRadius>(CornerRadiusProperty()); }
    void SetCornerRadius(const ui::CornerRadius& value) { this->SetValue(CornerRadiusProperty(), value); }
    
    Derived* CornerRadius(float uniform) {
        SetCornerRadius(ui::CornerRadius(uniform));
        return static_cast<Derived*>(this);
    }
    Derived* CornerRadius(float topLeft, float topRight, float bottomRight, float bottomLeft) {
        SetCornerRadius(ui::CornerRadius(topLeft, topRight, bottomRight, bottomLeft));
        return static_cast<Derived*>(this);
    }
    ui::CornerRadius CornerRadius() const { return GetCornerRadius(); }
    
    // ========== 裁剪控制 ==========
    
    bool GetClipToBounds() const { return this->template GetValue<bool>(ClipToBoundsProperty()); }
    void SetClipToBounds(bool value) { this->SetValue(ClipToBoundsProperty(), value); }
    
    Derived* ClipToBounds(bool value) {
        SetClipToBounds(value);
        return static_cast<Derived*>(this);
    }
    bool ClipToBounds() const { return GetClipToBounds(); }

    // ========== 子元素集合 ==========
    
    /**
     * @brief 添加子元素（并获取其所有权）
     */
    Derived* AddChild(UIElement* child) {
        if (child) {
            children_.push_back(child);
            this->AddVisualChild(child);
            this->TakeOwnership(child);  // 获取子对象所有权
            this->InvalidateMeasure();
        }
        return static_cast<Derived*>(this);
    }
    
    /**
     * @brief 批量添加子元素（链式调用）
     */
    Derived* Children(std::initializer_list<UIElement*> children) {
        for (auto* child : children) {
            AddChild(child);
        }
        return static_cast<Derived*>(this);
    }
    
    /**
     * @brief 移除子元素
     */
    void RemoveChild(UIElement* child) {
        auto it = std::find(children_.begin(), children_.end(), child);
        if (it != children_.end()) {
            children_.erase(it);
            this->RemoveVisualChild(child);
            this->InvalidateMeasure();
        }
    }
    
    /**
     * @brief 清空子元素
     */
    void ClearChildren() {
        for (auto* child : children_) {
            this->RemoveVisualChild(child);
        }
        children_.clear();
        this->InvalidateMeasure();
    }
    
    /**
     * @brief 获取子元素集合
     */
    const std::vector<UIElement*>& GetChildren() const { return children_; }
    
    const std::vector<UIElement*>& Children() const { return GetChildren(); }
    
    /**
     * @brief 获取子元素数量
     */
    size_t GetChildrenCount() const { return children_.size(); }
    
    // ========== 逻辑树遍历（覆写 UIElement）==========
    
    /**
     * @brief 返回子元素集合（用于模板递归遍历）
     */
    std::vector<UIElement*> GetLogicalChildren() const override {
        return children_;
    }

protected:
    /**
     * @brief 测量所有子元素
     */
    void MeasureChildren(const Size& availableSize) {
        for (auto* child : children_) {
            if (child && child->GetVisibility() != Visibility::Collapsed) {
                child->Measure(availableSize);
            }
        }
    }
    
    /**
     * @brief 排列所有子元素
     */
    void ArrangeChildren() {
        for (auto* child : children_) {
            if (child && child->GetVisibility() != Visibility::Collapsed) {
                // 派生类的 ArrangeOverride 应该调用 child->Arrange()
            }
        }
    }
    
    // ========== 裁剪系统（新增）==========
    
    /**
     * @brief Panel的裁剪由ClipToBounds属性控制
     */
    bool ShouldClipToBounds() const override {
        return GetClipToBounds();
    }

    // 子元素集合
    std::vector<UIElement*> children_;
};

} // namespace fk::ui
