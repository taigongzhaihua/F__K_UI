#pragma once

#include "fk/ui/ItemsControl.h"
#include "fk/binding/DependencyProperty.h"
#include <any>
#include <functional>

namespace fk::ui {

/**
 * @brief ListBox selection mode enumeration
 */
enum class SelectionMode {
    Single,    // Single selection only
    Multiple,  // Multiple selection allowed
    Extended   // Extended selection (Shift+Click, Ctrl+Click)
};

/**
 * @brief ListBox control with selection support
 * 
 * Responsibilities:
 * - Display list of items
 * - Single/Multiple selection support
 * - Keyboard navigation (Up/Down/Home/End)
 * - Selection changed events
 * - Mouse click selection
 * 
 * Template parameter: Derived - Derived class type (CRTP)
 * Inherits: ItemsControl<Derived>
 */
template<typename Derived = void>
class ListBox : public ItemsControl<typename std::conditional<std::is_void_v<Derived>, ListBox<>, Derived>::type> {
private:
    using Base = ItemsControl<typename std::conditional<std::is_void_v<Derived>, ListBox<>, Derived>::type>;
    using ActualDerived = typename std::conditional<std::is_void_v<Derived>, ListBox<>, Derived>::type;
    
public:
    // ========== Dependency Properties ==========
    
    /// SelectedItem property: Currently selected item
    static const binding::DependencyProperty& SelectedItemProperty();
    
    /// SelectedIndex property: Index of selected item
    static const binding::DependencyProperty& SelectedIndexProperty();
    
    /// SelectionMode property: Selection mode (Single/Multiple/Extended)
    static const binding::DependencyProperty& SelectionModeProperty();

public:
    ListBox() : Base() {
        // Subscribe to mouse and keyboard events
        // TODO: Connect to actual event system when available
        // this->MouseDown += [this](auto& sender, auto& e) { OnMouseDownInternal(e); };
        // this->KeyDown += [this](auto& sender, auto& e) { OnKeyDownInternal(e); };
    }
    
    virtual ~ListBox() = default;

    // ========== Selected Item ==========
    
    std::any GetSelectedItem() const { 
        return this->template GetValue<std::any>(SelectedItemProperty()); 
    }
    
    void SetSelectedItem(const std::any& item) { 
        this->SetValue(SelectedItemProperty(), item);
    }
    
    ActualDerived* SelectedItem(const std::any& item) {
        SetSelectedItem(item);
        return static_cast<ActualDerived*>(this);
    }
    std::any SelectedItem() const { return GetSelectedItem(); }

    // ========== Selected Index ==========
    
    int GetSelectedIndex() const { 
        return this->template GetValue<int>(SelectedIndexProperty()); 
    }
    
    void SetSelectedIndex(int index) { 
        this->SetValue(SelectedIndexProperty(), index);
    }
    
    ActualDerived* SelectedIndex(int index) {
        SetSelectedIndex(index);
        return static_cast<ActualDerived*>(this);
    }
    int SelectedIndex() const { return GetSelectedIndex(); }

    // ========== Selection Mode ==========
    
    ::fk::ui::SelectionMode GetSelectionMode() const { 
        return this->template GetValue<::fk::ui::SelectionMode>(SelectionModeProperty()); 
    }
    
    void SetSelectionMode(::fk::ui::SelectionMode mode) { 
        this->SetValue(SelectionModeProperty(), mode);
    }
    
    ActualDerived* SelectionMode(::fk::ui::SelectionMode mode) {
        SetSelectionMode(mode);
        return static_cast<ActualDerived*>(this);
    }
    ::fk::ui::SelectionMode SelectionMode() const { return GetSelectionMode(); }

    // ========== Selection Management ==========
    
    /**
     * @brief Select item by index
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
        
        // Update visual state of containers
        UpdateSelectionVisuals();
        
        // Raise selection changed event
        OnSelectionChanged();
    }
    
    /**
     * @brief Select item by value
     */
    virtual void SelectItem(const std::any& item) {
        auto& items = this->GetItems();
        for (size_t i = 0; i < items.Count(); ++i) {
            // TODO: Proper equality comparison
            if (items[i].type() == item.type()) {
                SelectItemByIndex(static_cast<int>(i));
                return;
            }
        }
        
        ClearSelection();
    }
    
    /**
     * @brief Clear selection
     */
    virtual void ClearSelection() {
        SetSelectedIndex(-1);
        SetSelectedItem(std::any{});
        UpdateSelectionVisuals();
        OnSelectionChanged();
    }
    
    /**
     * @brief Check if item is selected
     */
    virtual bool IsItemSelected(int index) const {
        return index == GetSelectedIndex();
    }

protected:
    // ========== Event Handlers ==========
    
    /**
     * @brief Handle mouse down on list item
     * TODO: Implement when event system is available
     */
    // virtual void OnMouseDownInternal(MouseButtonEventArgs& e) {
    //     // Find which container was clicked
    //     // TODO: Hit testing to find container
    // }
    
    /**
     * @brief Handle keyboard navigation
     * TODO: Implement when event system is available
     */
    // virtual void OnKeyDownInternal(KeyEventArgs& e) {
    //     int currentIndex = GetSelectedIndex();
    //     int itemCount = static_cast<int>(this->GetItems().Count());
    //     
    //     if (itemCount == 0) return;
    //     
    //     int newIndex = currentIndex;
    //     
    //     switch (e.Key) {
    //         case Key::Up:
    //         case Key::Left:
    //             newIndex = (currentIndex <= 0) ? 0 : currentIndex - 1;
    //             e.Handled = true;
    //             break;
    //             
    //         case Key::Down:
    //         case Key::Right:
    //             newIndex = (currentIndex >= itemCount - 1) ? itemCount - 1 : currentIndex + 1;
    //             e.Handled = true;
    //             break;
    //             
    //         case Key::Home:
    //             newIndex = 0;
    //             e.Handled = true;
    //             break;
    //             
    //         case Key::End:
    //             newIndex = itemCount - 1;
    //             e.Handled = true;
    //             break;
    //             
    //         case Key::PageUp:
    //             newIndex = std::max(0, currentIndex - 10);
    //             e.Handled = true;
    //             break;
    //             
    //         case Key::PageDown:
    //             newIndex = std::min(itemCount - 1, currentIndex + 10);
    //             e.Handled = true;
    //             break;
    //             
    //         default:
    //             break;
    //     }
    //     
    //     if (newIndex != currentIndex) {
    //         SelectItemByIndex(newIndex);
    //         ScrollIntoView(newIndex);
    //     }
    // }
    
    /**
     * @brief Selection changed event handler
     */
    virtual void OnSelectionChanged() {
        // Derived classes can override
        // Fire SelectionChanged event
    }
    
    /**
     * @brief Update visual state of all containers to reflect selection
     */
    virtual void UpdateSelectionVisuals() {
        auto& generator = this->GetItemContainerGenerator();
        int selectedIndex = GetSelectedIndex();
        
        for (size_t i = 0; i < this->GetItems().Count(); ++i) {
            auto* container = generator.ContainerFromIndex(static_cast<int>(i));
            if (container) {
                UpdateContainerSelectionState(container, static_cast<int>(i) == selectedIndex);
            }
        }
    }
    
    /**
     * @brief Update single container's selection visual state
     */
    virtual void UpdateContainerSelectionState(UIElement* container, bool isSelected) {
        // TODO: Apply selection visual (highlight, different background, etc.)
        // This could use VisualStateManager or direct property changes
        // For now, placeholder
    }
    
    /**
     * @brief Scroll to make item at index visible
     */
    virtual void ScrollIntoView(int index) {
        // TODO: Implement scrolling logic
        // Requires ScrollViewer integration
    }
    
private:
    // Selection state
    // Note: SelectedItem and SelectedIndex stored in dependency properties
};

// ========== Dependency Property Implementations ==========

template<typename Derived>
const binding::DependencyProperty& ListBox<Derived>::SelectedItemProperty() {
    static auto& property = binding::DependencyProperty::Register(
        "SelectedItem",
        typeid(std::any),
        typeid(ListBox<Derived>),
        binding::PropertyMetadata{std::any{}}
    );
    return property;
}

template<typename Derived>
const binding::DependencyProperty& ListBox<Derived>::SelectedIndexProperty() {
    static auto& property = binding::DependencyProperty::Register(
        "SelectedIndex",
        typeid(int),
        typeid(ListBox<Derived>),
        binding::PropertyMetadata{std::any(-1)}
    );
    return property;
}

template<typename Derived>
const binding::DependencyProperty& ListBox<Derived>::SelectionModeProperty() {
    static auto& property = binding::DependencyProperty::Register(
        "SelectionMode",
        typeid(::fk::ui::SelectionMode),
        typeid(ListBox<Derived>),
        binding::PropertyMetadata{std::any(::fk::ui::SelectionMode::Single)}
    );
    return property;
}

} // namespace fk::ui
