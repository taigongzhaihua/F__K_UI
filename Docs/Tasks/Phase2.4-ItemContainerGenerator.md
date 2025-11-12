# Phase 2.4: ItemContainerGenerator Implementation

## Overview

Implementation of ItemContainerGenerator with container reuse and recycling capabilities to support efficient ItemsControl rendering and enable future UI virtualization.

## Implementation Status: ✅ COMPLETE

### Files Created

1. **include/fk/ui/ItemContainerGenerator.h** (273 lines)
   - Complete generator class interface
   - Container reuse/recycling support
   - Position tracking for incremental generation
   
2. **src/ui/ItemContainerGenerator.cpp** (296 lines)
   - Full implementation with container pooling
   - Item-to-container mapping
   - Recycling logic

## Features Implemented

### Core Functionality

1. **Container Generation**
   - `GenerateContainer()` - Generate or reuse container for single item
   - `GenerateContainers()` - Batch generation for multiple items
   - `GenerateNext()` - Incremental generation from position
   - Automatic detection of new vs. reused containers

2. **Container Lookup**
   - `ContainerFromItem()` - Find container for data item
   - `ContainerFromIndex()` - Find container by index
   - `ItemFromContainer()` - Find data item for container
   - `IndexFromContainer()` - Find index for container
   - Bidirectional mapping support

3. **Container Recycling**
   - `RecycleContainer()` - Return container to pool for reuse
   - `RecycleContainers()` - Batch recycling
   - `ClearContainerPool()` - Clear all recycled containers
   - `RemoveContainer()` - Remove and delete container
   - `RemoveAll()` - Remove all containers

4. **Container Preparation**
   - `PrepareContainer()` - Setup container for item (DataContext, etc.)
   - `ClearContainer()` - Clean container for recycling
   - Customizable via `SetContainerPreparer()`

5. **Strategy Configuration**
   - `SetContainerFactory()` - Custom container creation
   - `SetContainerPreparer()` - Custom container preparation
   - `SetEnableRecycling()` - Enable/disable recycling
   - Flexible and extensible design

### Technical Details

**Container Pooling**:
```cpp
// Recycling workflow
1. Container becomes unnecessary → RecycleContainer()
2. Container cleared → ClearContainer()
3. Container added to pool → containerPool_.push_back()
4. New item needs container → GetRecycledContainer()
5. Container reused → PrepareContainer() with new item
```

**Mapping System**:
```cpp
// Bidirectional mappings
itemToContainer_: map<string, UIElement*>   // Item key -> Container
containerToItem_: map<UIElement*, any>      // Container -> Item
containers_: vector<UIElement*>             // Ordered list
```

**Item Key Generation**:
- Uses type information + pointer address
- Handles both pointer and value types
- Fallback to std::any address for safety

### Integration Points

**With ItemsControl**:
```cpp
class ItemsControl {
    ItemContainerGenerator generator_;
    
    void OnItemsSourceChanged() {
        auto containers = generator_.GenerateContainers(items);
        // Add containers to panel
    }
};
```

**With DataTemplate**:
```cpp
generator_.SetContainerFactory([this](const any& item) {
    auto* container = new ContentControl();
    if (itemTemplate_) {
        container->SetContentTemplate(itemTemplate_);
    }
    return container;
});
```

**With Virtualization (Future)**:
```cpp
// Viewport changed - recycle off-screen containers
for (auto* container : offScreenContainers) {
    generator_.RecycleContainer(container);
}

// Generate new containers for visible items
auto newContainers = generator_.GenerateContainers(visibleItems);
```

## Usage Examples

### Basic Usage

```cpp
// Create generator
ItemContainerGenerator generator(itemsControl);

// Set custom factory
generator.SetContainerFactory([](const any& item) {
    return new MyCustomContainer();
});

// Generate containers for items
vector<any> items = GetDataItems();
auto containers = generator.GenerateContainers(items);

// Lookup
auto* container = generator.ContainerFromItem(items[0]);
auto item = generator.ItemFromContainer(container);
```

### With Recycling

```cpp
// Enable recycling
generator.SetEnableRecycling(true);

// Item removed from view
generator.RecycleContainer(container);

// Item comes into view - reuses recycled container
bool isNew = false;
auto* reusedContainer = generator.GenerateContainer(newItem, isNew);
// isNew == false if container was recycled
```

### Incremental Generation

```cpp
// Start from position
GeneratorPosition pos(0, 0);

// Generate next container
while (auto* container = generator.GenerateNext(pos, 1)) {
    // Process container
    panel->AddChild(container);
    pos = generator.GeneratorPositionFromIndex(
        generator.IndexFromContainer(container) + 1
    );
}
```

## Benefits Achieved

✅ **Container Reuse**: Reduces memory allocations via pooling  
✅ **Efficient Mapping**: Fast bidirectional item-container lookup  
✅ **Flexible Strategy**: Customizable factory and preparer functions  
✅ **Virtualization Ready**: Incremental generation and recycling support  
✅ **Clean Separation**: Generator logic separated from ItemsControl  
✅ **Type Safe**: Proper use of std::any and RTTI  

## Design Patterns

1. **Factory Pattern**: ContainerFactory for container creation
2. **Object Pool**: Container recycling for memory efficiency
3. **Strategy Pattern**: Pluggable factory and preparer strategies
4. **Mediator Pattern**: Generator mediates item-container relationships

## Performance Characteristics

**Container Generation**:
- First time: O(1) - Create new container
- With recycling: O(1) - Reuse from pool
- Batch generation: O(n) where n = item count

**Container Lookup**:
- ContainerFromItem: O(1) average via hash map
- IndexFromContainer: O(n) - Linear search (could be optimized)
- ItemFromContainer: O(1) average via hash map

**Memory**:
- Container pool size configurable
- Trade-off: memory vs. allocation cost
- Pool should be cleared periodically for long-running apps

## Future Enhancements

### Phase 3 Enhancements

**UI Virtualization**:
- Integrate with VirtualizingStackPanel
- Dynamic container generation based on viewport
- Automatic recycling of off-screen items
- Estimated size calculations

**Performance Optimizations**:
- Index-based container lookup (add reverse index)
- Batch preparation operations
- Async container generation for large datasets
- Container warmup (pre-generate idle time)

**Advanced Features**:
- Container type selection based on item type
- Multiple container pools (by type)
- Container state preservation across recycling
- Generation statistics and debugging

## Testing Recommendations

### Unit Tests

```cpp
TEST(ItemContainerGenerator, BasicGeneration) {
    // Test container generation
}

TEST(ItemContainerGenerator, ContainerRecycling) {
    // Test recycling workflow
}

TEST(ItemContainerGenerator, BidirectionalMapping) {
    // Test item-container lookups
}

TEST(ItemContainerGenerator, CustomFactory) {
    // Test factory customization
}
```

### Integration Tests

```cpp
TEST(ItemsControl, GeneratorIntegration) {
    // Test generator with ItemsControl
}

TEST(ListView, VirtualizationWithGenerator) {
    // Test with virtualization
}
```

## Next Steps

**Phase 2.5**: Integrate ItemContainerGenerator into ItemsControl
- Update ItemsControl to use generator
- Replace manual container management
- Add generator property accessor

**Phase 2.6**: ListBox Implementation
- Create ListBox class using ItemsControl
- Add selection support
- Keyboard navigation

**Phase 3**: Advanced Controls & Virtualization
- VirtualizingStackPanel
- Large dataset performance testing
- Scrolling viewport integration

## Conclusion

Phase 2.4 complete! ItemContainerGenerator provides a robust foundation for efficient item-to-container management, enabling future virtualization and performance optimizations. The design is flexible, extensible, and follows established design patterns.

---

**Status**: ✅ IMPLEMENTED  
**Lines of Code**: ~570 (273 header + 296 implementation)  
**Dependencies**: ItemsControl, UIElement, ContentControl  
**Next Phase**: 2.5 - ItemsControl Generator Integration
