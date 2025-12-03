/**
 * @file ComboBox.h
 * @brief ComboBox 组合框控件
 * 
 * 职责：
 * - 提供下拉选择框UI控件
 * - 继承自ItemsControl，支持项目集合
 * - 集成Popup实现下拉功能
 * - 单选模式，显示选中项
 * - 支持键盘导航
 * 
 * WPF 对应：ComboBox
 */

#pragma once

#include "fk/ui/controls/ItemsControl.h"
#include "fk/ui/controls/Popup.h"
#include "fk/binding/DependencyProperty.h"
#include <any>

namespace fk::ui {

/**
 * @brief ComboBox 组合框控件
 * 
 * 提供下拉列表选择功能，用户可以从列表中选择一个项目。
 * 结合了文本框和下拉列表的特性。
 */
template<typename Derived = void>
class ComboBox : public ItemsControl<typename std::conditional<std::is_void_v<Derived>, ComboBox<>, Derived>::type> {
private:
    using Base = ItemsControl<typename std::conditional<std::is_void_v<Derived>, ComboBox<>, Derived>::type>;
    using ActualDerived = typename std::conditional<std::is_void_v<Derived>, ComboBox<>, Derived>::type;
    
public:
    ComboBox() : Base(), popup_(nullptr) {
        // 创建内部Popup
        popup_ = new Popup<>();
        popup_->SetStaysOpen(false);  // 点击外部自动关闭
        popup_->SetPlacementTarget(this);
        popup_->SetPlacement(PlacementMode::Bottom);
    }
    
    virtual ~ComboBox() {
        if (popup_) {
            delete popup_;
        }
    }
    
    // ========== 依赖属性 ==========
    
    /// SelectedItem 属性：当前选中项
    static const binding::DependencyProperty& SelectedItemProperty();
    
    /// SelectedIndex 属性：选中项索引
    static const binding::DependencyProperty& SelectedIndexProperty();
    
    /// IsDropDownOpen 属性：下拉列表是否打开
    static const binding::DependencyProperty& IsDropDownOpenProperty();
    
    /// MaxDropDownHeight 属性：下拉列表最大高度
    static const binding::DependencyProperty& MaxDropDownHeightProperty();
    
    // ========== SelectedItem 属性 ==========
    
    std::any GetSelectedItem() const {
        return this->GetValue(SelectedItemProperty());
    }
    
    void SetSelectedItem(const std::any& value) {
        this->SetValue(SelectedItemProperty(), value);
    }
    
    ActualDerived* SelectedItem(const std::any& value) {
        SetSelectedItem(value);
        return static_cast<ActualDerived*>(this);
    }
    std::any SelectedItem() const { return GetSelectedItem(); }
    
    // ========== SelectedIndex 属性 ==========
    
    int GetSelectedIndex() const {
        return this->template GetValue<int>(SelectedIndexProperty());
    }
    
    void SetSelectedIndex(int value) {
        this->SetValue(SelectedIndexProperty(), value);
        SyncSelectedItem(value);
    }
    
    ActualDerived* SelectedIndex(int value) {
        SetSelectedIndex(value);
        return static_cast<ActualDerived*>(this);
    }
    int SelectedIndex() const { return GetSelectedIndex(); }
    
    // ========== IsDropDownOpen 属性 ==========
    
    bool GetIsDropDownOpen() const {
        return this->template GetValue<bool>(IsDropDownOpenProperty());
    }
    
    void SetIsDropDownOpen(bool value) {
        this->SetValue(IsDropDownOpenProperty(), value);
        if (popup_) {
            popup_->SetIsOpen(value);
        }
    }
    
    ActualDerived* IsDropDownOpen(bool value) {
        SetIsDropDownOpen(value);
        return static_cast<ActualDerived*>(this);
    }
    bool IsDropDownOpen() const { return GetIsDropDownOpen(); }
    
    // ========== MaxDropDownHeight 属性 ==========
    
    double GetMaxDropDownHeight() const {
        return this->template GetValue<double>(MaxDropDownHeightProperty());
    }
    
    void SetMaxDropDownHeight(double value) {
        this->SetValue(MaxDropDownHeightProperty(), value);
    }
    
    ActualDerived* MaxDropDownHeight(double value) {
        SetMaxDropDownHeight(value);
        return static_cast<ActualDerived*>(this);
    }
    double MaxDropDownHeight() const { return GetMaxDropDownHeight(); }
    
    // ========== 公共方法 ==========
    
    /**
     * @brief 打开下拉列表
     */
    void OpenDropDown() {
        SetIsDropDownOpen(true);
    }
    
    /**
     * @brief 关闭下拉列表
     */
    void CloseDropDown() {
        SetIsDropDownOpen(false);
    }
    
    /**
     * @brief 切换下拉列表状态
     */
    void ToggleDropDown() {
        SetIsDropDownOpen(!GetIsDropDownOpen());
    }
    
    /**
     * @brief 选择指定索引的项目
     */
    virtual void SelectItemByIndex(int index) {
        if (index < 0 || index >= static_cast<int>(this->GetItems().Count())) {
            ClearSelection();
            return;
        }
        
        auto& items = this->GetItems();
        std::any item = items[index];
        
        SetSelectedIndex(index);
        SetSelectedItem(item);
        
        // 关闭下拉列表
        CloseDropDown();
        
        // 触发选择变更事件
        OnSelectionChanged();
    }
    
    /**
     * @brief 清除选择
     */
    virtual void ClearSelection() {
        SetSelectedIndex(-1);
        SetSelectedItem(std::any{});
        OnSelectionChanged();
    }
    
    /**
     * @brief 获取Popup控件
     */
    Popup<>* GetPopup() const {
        return popup_;
    }
    
    // ========== 事件 ==========
    
    /// SelectionChanged 事件：选择变更时触发
    core::Event<> SelectionChanged;
    
    /// DropDownOpened 事件：下拉列表打开时触发
    core::Event<> DropDownOpened;
    
    /// DropDownClosed 事件：下拉列表关闭时触发
    core::Event<> DropDownClosed;
    
protected:
    /**
     * @brief 选择变更事件处理
     */
    virtual void OnSelectionChanged() {
        SelectionChanged();
    }
    
    /**
     * @brief 同步SelectedItem到指定索引的项目
     */
    void SyncSelectedItem(int index) {
        if (index < 0 || index >= static_cast<int>(this->GetItems().Count())) {
            this->SetValue(SelectedItemProperty(), std::any{});
        } else {
            auto& items = this->GetItems();
            std::any item = items[index];
            this->SetValue(SelectedItemProperty(), item);
        }
    }
    
private:
    Popup<>* popup_;  // 下拉Popup
};

// ========== 依赖属性实现 ==========

template<typename Derived>
const binding::DependencyProperty& ComboBox<Derived>::SelectedItemProperty() {
    static auto& property = binding::DependencyProperty::Register(
        "SelectedItem",
        typeid(std::any),
        typeid(ComboBox<Derived>),
        binding::PropertyMetadata{std::any{}}
    );
    return property;
}

template<typename Derived>
const binding::DependencyProperty& ComboBox<Derived>::SelectedIndexProperty() {
    static auto& property = binding::DependencyProperty::Register(
        "SelectedIndex",
        typeid(int),
        typeid(ComboBox<Derived>),
        binding::PropertyMetadata{-1}
    );
    return property;
}

template<typename Derived>
const binding::DependencyProperty& ComboBox<Derived>::IsDropDownOpenProperty() {
    static auto& property = binding::DependencyProperty::Register(
        "IsDropDownOpen",
        typeid(bool),
        typeid(ComboBox<Derived>),
        binding::PropertyMetadata{false}
    );
    return property;
}

template<typename Derived>
const binding::DependencyProperty& ComboBox<Derived>::MaxDropDownHeightProperty() {
    static auto& property = binding::DependencyProperty::Register(
        "MaxDropDownHeight",
        typeid(double),
        typeid(ComboBox<Derived>),
        binding::PropertyMetadata{200.0}
    );
    return property;
}

} // namespace fk::ui
