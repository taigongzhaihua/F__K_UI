# Phase 2.5: ItemsControl Integration with ItemContainerGenerator

## Overview

Integrate the ItemContainerGenerator into ItemsControl class to replace manual container management with a sophisticated, efficient system featuring container pooling and recycling.

## Implementation Status

✅ **COMPLETED**

## Changes Made

### 1. Added ItemContainerGenerator Include
```cpp
#include "fk/ui/ItemContainerGenerator.h"
```

### 2. Added Generator Member Variable
```cpp
private:
    ItemContainerGenerator generator_;  // Replaces itemContainers_ vector
```

### 3. Updated Constructor with Generator Configuration
```cpp
ItemsControl() : generator_(static_cast<Derived*>(this)) {
    // Configure generator with default factory
    generator_.SetContainerFactory([this](const std::any& item) -> UIElement* {
        return this->GetContainerForItem(item);
    });
    
    // Configure preparer
    generator_.SetContainerPreparer([this](UIElement* container, const std::any& item) {
        this->PrepareContainerForItem(container, item);
    });
    
    // Enable recycling
    generator_.SetEnableRecycling(true);
}
```

### 4. Added Generator Accessor
```cpp
ItemContainerGenerator& GetItemContainerGenerator() {
    return generator_;
}

const ItemContainerGenerator& GetItemContainerGenerator() const {
    return generator_;
}
```

### 5. Updated Container Management Methods

**GenerateContainers()**:
```cpp
virtual void GenerateContainers() {
    ClearItemContainers();
    
    if (items_) {
        std::vector<std::any> itemVector;
        for (const auto& item : items_->GetItems()) {
            itemVector.push_back(item);
        }
        
        auto containers = generator_.GenerateContainers(itemVector);
        // Containers automatically managed by generator
    }
}
```

**AddItemContainer()**:
```cpp
virtual void AddItemContainer(const std::any& item) {
    bool isNew = false;
    UIElement* container = generator_.GenerateContainer(item, isNew);
    if (container) {
        // isNew tells if container was reused or newly created
        this->InvalidateMeasure();
    }
}
```

**RemoveItemContainer()**:
```cpp
virtual void RemoveItemContainer(const std::any& item) {
    auto* container = generator_.ContainerFromItem(item);
    if (container) {
        generator_.RecycleContainer(container);  // Returns to pool
        this->InvalidateMeasure();
    }
}
```

**ClearItemContainers()**:
```cpp
virtual void ClearItemContainers() {
    generator_.RemoveAll();  // Clears all containers
    this->InvalidateMeasure();
}
```

## Benefits

### 1. Container Reuse
- Removed containers automatically return to pool
- New items reuse existing containers
- Reduces memory allocations significantly

### 2. Fast Lookups
- O(1) container lookup by item: `ContainerFromItem()`
- O(1) item lookup by container: `ItemFromContainer()`
- Bidirectional mapping via hash maps

### 3. Clean Separation
- Container management logic separated into generator
- ItemsControl focuses on high-level logic
- Generator handles low-level details

### 4. Extensibility
- Custom factories via `SetContainerFactory()`
- Custom preparers via `SetContainerPreparer()`
- Easy to override behavior

### 5. Virtualization Ready
- Incremental generation via `GenerateNext()`
- Position tracking built-in
- Ready for VirtualizingStackPanel

## Usage Example

```cpp
// Basic usage (automatic)
auto* itemsControl = new ItemsControl<ListBox>();
itemsControl->GetItems().Add(item1);
itemsControl->GetItems().Add(item2);
// Generator automatically creates containers

// Access generator
auto& gen = itemsControl->GetItemContainerGenerator();

// Custom factory (if needed)
gen.SetContainerFactory([](const std::any& item) {
    auto* container = new MyCustomContainer();
    return container;
});

// Lookup
auto* container = gen.ContainerFromItem(item1);
auto item = gen.ItemFromContainer(container);

// Check recycling stats
auto& pool = gen.GetContainerPool();
int poolSize = pool.size();
```

## Performance Characteristics

**Before (Manual Management)**:
- Container creation: Always new allocation
- Container removal: Immediate deletion
- Memory: Linear growth with item count
- Lookups: O(n) linear search

**After (With Generator)**:
- Container creation: Reuse from pool (O(1)) or new (O(1))
- Container removal: Return to pool (O(1))
- Memory: Pool size + active containers
- Lookups: O(1) hash map

## API Compatibility

### Preserved Methods
All existing ItemsControl virtual methods preserved:
- `IsItemItsOwnContainer()` - Still used by generator factory
- `GetContainerForItem()` - Called by default factory
- `PrepareContainerForItem()` - Called by default preparer

### New Methods
- `GetItemContainerGenerator()` - Access generator

### Changed Internals
- `itemContainers_` vector → `generator_` object
- Container storage managed internally by generator

## Integration Points

### With ObservableCollection
```cpp
items_->CollectionChanged() += [this](const CollectionChangedEventArgs& args) {
    // Add/Remove triggers generator methods
    OnItemsChanged(args);
};
```

### With Panel (Future)
```cpp
// Add generated containers to panel
auto* panel = GetItemsPanel();
for (auto* container : containers) {
    panel->AddChild(container);
}
```

### With DataTemplate
```cpp
// Generator uses ItemTemplate automatically
generator_.SetContainerFactory([this](const std::any& item) {
    auto* container = new ContentControl();
    container->SetContentTemplate(GetItemTemplate());
    return container;
});
```

## Next Steps

**Phase 2.6: ListBox Implementation**
- Create ListBox class deriving from ItemsControl
- Add SelectedItem/SelectedIndex properties
- Implement single/multiple selection modes
- Add keyboard navigation (Up/Down arrows)
- Visual selection indicators

**Future Enhancements**:
- Panel integration (add containers to visual tree)
- ItemsSource support for various collection types
- Template lookup from ResourceDictionary
- Incremental loading for large datasets

## Testing Recommendations

1. **Basic Generation**
   - Create ItemsControl with items
   - Verify containers generated
   - Check container-item mapping

2. **Container Recycling**
   - Add items, remove items
   - Verify containers recycled
   - Check pool size growth

3. **Performance**
   - Large item count (1000+)
   - Measure allocation count
   - Compare with/without recycling

4. **Custom Factories**
   - Set custom factory
   - Verify custom containers created
   - Check preparer called correctly

5. **Collection Changes**
   - Add/remove items dynamically
   - Verify UI updates correctly
   - Check no memory leaks

## Known Limitations

1. **Panel Integration**: TODO - Containers not yet added to visual tree
2. **ItemsSource**: TODO - Only supports Items collection currently
3. **Virtualization**: TODO - Needs VirtualizingStackPanel

## Files Modified

1. **include/fk/ui/ItemsControl.h**
   - Added ItemContainerGenerator include
   - Added generator_ member
   - Updated constructor with generator config
   - Added GetItemContainerGenerator() accessor
   - Updated container management methods
   - Removed itemContainers_ vector

## Code Statistics

- Lines added: ~40
- Lines removed: ~15
- Net change: +25 lines
- Complexity: Simplified (moved to generator)

---

**Phase 2.5 Complete!** ItemsControl now uses ItemContainerGenerator for efficient container management with automatic recycling. This provides a solid foundation for advanced features like UI virtualization and large dataset handling.
