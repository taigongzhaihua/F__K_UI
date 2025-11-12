# DependencyObject - Design Document

## Overview

**Module**: Binding

**Purpose**: Foundational base class providing dependency property system, data binding infrastructure, and logical tree management.

**WPF Equivalent**: `System.Windows.DependencyObject`

## Responsibilities

The `DependencyObject` class is responsible for:

1. **Property Storage**: Efficiently storing and retrieving property values
2. **Property Change Notification**: Notifying listeners when properties change
3. **Value Source Tracking**: Tracking where property values originate (local, binding, default)
4. **Data Binding Infrastructure**: Managing bindings and binding expressions
5. **DataContext Inheritance**: Propagating DataContext through the logical tree
6. **Logical Tree Management**: Maintaining parent-child relationships
7. **Element Name Resolution**: Supporting ElementName bindings

## Architecture

### Class Diagram

```
┌─────────────────────────────────┐
│      DependencyObject           │
├─────────────────────────────────┤
│ - propertyStore_: PropertyStore │
│ - bindingContext_: BindingContext│
│ - logicalParent_: DependencyObject*│
│ - logicalChildren_: vector<DependencyObject*>│
│ - elementName_: string          │
├─────────────────────────────────┤
│ + GetValue()                    │
│ + SetValue()                    │
│ + SetBinding()                  │
│ + SetDataContext()              │
│ + PropertyChanged: Event        │
└─────────────────────────────────┘
        △
        │ inherits
        │
┌───────┴──────────────────────┐
│ Visual, ObservableObject     │
│ (and other derived classes)  │
└──────────────────────────────┘
```

### Key Components

#### 1. PropertyStore
- Stores actual property values
- Uses hash map for O(1) property lookup
- Tracks value sources for each property
- Raises change notifications

#### 2. BindingContext
- Manages all active bindings for the object
- Creates and destroys BindingExpression instances
- Resolves binding sources (DataContext, ElementName, RelativeSource)
- Handles DataContext inheritance

#### 3. Logical Tree
- Parent pointer for upward traversal
- Children list for downward traversal
- Enables element name lookup
- Supports DataContext inheritance

## Design Decisions

### 1. Value Storage Strategy

**Decision**: Use `std::any` for type-erased value storage

**Rationale**:
- Allows properties of any type without templates
- Type safety through `DependencyProperty` metadata
- Performance acceptable for UI scenarios
- Alternative (`std::variant`) would limit supported types

**Trade-offs**:
- Runtime type checking overhead
- Heap allocation for large types
- Dynamic casts needed for access

### 2. Value Source Priority

**Decision**: Implement explicit value source priority (Local > Binding > Default)

**Rationale**:
- Matches WPF behavior
- Predictable and understandable for developers
- Allows local values to override bindings temporarily

**Implementation**:
```cpp
enum class ValueSource {
    Default = 0,    // Lowest priority
    Binding = 1,
    Local = 2       // Highest priority
};
```

### 3. Binding Management

**Decision**: Store bindings separately from values in `BindingContext`

**Rationale**:
- Bindings are complex objects with lifecycle
- Need to update when DataContext changes
- Separation of concerns (values vs. bindings)
- Easier debugging and introspection

### 4. Logical vs Visual Tree

**Decision**: Separate logical tree (DependencyObject) from visual tree (Visual)

**Rationale**:
- Not all DependencyObjects are visual (e.g., Style, Binding)
- Logical tree handles naming and DataContext
- Visual tree handles rendering and hit testing
- Matches WPF architecture

### 5. Thread Affinity

**Decision**: No built-in thread synchronization; objects are thread-affine

**Rationale**:
- UI operations are single-threaded
- Synchronization overhead not needed for most scenarios
- Users can use `Dispatcher` for cross-thread calls
- Matches WPF and most UI frameworks

## Implementation Details

### Memory Management

#### Ownership Model
- **Parent -> Child**: Raw pointers (parent doesn't own children)
- **Public API**: Shared pointers for objects passed across API boundaries
- **Bindings**: Shared pointers (managed lifetime)

#### Lifecycle
```cpp
// Parent manages child lifetime
panel->AddChild(button);  // Button's lifetime managed elsewhere

// Bindings kept alive by BindingContext
auto binding = Binding("Property");
element->SetBinding(Property, binding);  // BindingContext holds reference
```

#### Cleanup
- Destructor clears all bindings automatically
- Children not automatically destroyed (managed externally)
- Event connections use Connection objects for RAII cleanup

### Thread Safety

#### Thread Affinity
- Objects are created on and belong to a specific thread
- All operations must occur on the owning thread
- No internal synchronization (performance)

#### Cross-Thread Access
```cpp
// Wrong - crashes or undefined behavior
std::thread([element]() {
    element->SetValue(Property, value);  // ERROR!
}).detach();

// Correct - use Dispatcher
std::thread([element]() {
    element->GetDispatcher()->InvokeAsync([element]() {
        element->SetValue(Property, value);  // OK
    });
}).detach();
```

### Performance Considerations

#### Property Access Performance
- **Read**: O(1) hash lookup
- **Write**: O(1) + change notification overhead
- **Binding**: Additional overhead for source resolution and updates

#### Optimization Strategies
- Property values cached until invalidated
- Binding updates batched when possible
- Change notifications short-circuited when value unchanged
- Lazy binding expression creation

#### Memory Footprint
Per DependencyObject:
- PropertyStore: ~32 bytes + stored values
- BindingContext: ~64 bytes + active bindings
- Logical tree: ~24 bytes (parent + children vector)
- Element name: ~32 bytes (if set)

Total: ~150-200 bytes minimum per object

## Dependencies

### Direct Dependencies
- `PropertyStore` - Value storage
- `BindingContext` - Binding management
- `DependencyProperty` - Property metadata
- `BindingExpression` - Active binding instance
- `core::Event` - Event system

### Indirect Dependencies
- `INotifyPropertyChanged` - Source object interface
- `Dispatcher` - Thread marshalling (for derived classes)

## Usage Patterns

### Pattern 1: Simple Property Access

```cpp
// Define property
static const DependencyProperty& MyPropertyProperty();

// Implement wrappers
void SetMyProperty(const T& value) {
    SetValue(MyPropertyProperty(), value);
}

T GetMyProperty() const {
    return GetValue<T>(MyPropertyProperty());
}
```

### Pattern 2: Property Changed Callbacks

```cpp
class MyElement : public DependencyObject {
protected:
    void OnPropertyChanged(const DependencyProperty& property,
                          const std::any& oldValue,
                          const std::any& newValue,
                          ValueSource oldSource,
                          ValueSource newSource) override {
        DependencyObject::OnPropertyChanged(property, oldValue, newValue, oldSource, newSource);
        
        if (&property == &MyPropertyProperty()) {
            // Handle property change
            HandleMyPropertyChanged(oldValue, newValue);
        }
    }
};
```

### Pattern 3: Two-Way Binding

```cpp
// ViewModel
class ViewModel : public ObservableObject {
public:
    FK_PROPERTY(std::string, Text, "")
};

// View
auto binding = Binding("Text");
binding.SetMode(BindingMode::TwoWay);
textBox->SetBinding(TextBox::TextProperty(), binding);

// Changes flow both ways:
// textBox.Text -> ViewModel.Text (when user types)
// ViewModel.Text -> textBox.Text (when ViewModel changes)
```

### Pattern 4: DataContext Inheritance

```cpp
// Set DataContext on parent
window->SetDataContext(viewModel);

// All descendants inherit automatically
// Bindings in children resolve to window's DataContext
button->SetBinding(Button::ContentProperty(), Binding("ButtonText"));
textBlock->SetBinding(TextBlock::TextProperty(), Binding("StatusText"));
```

## Testing Considerations

### Unit Testing
- Mock `PropertyStore` for isolated testing
- Test property change notifications
- Verify value source priority
- Test binding lifecycle

### Integration Testing
- Test DataContext inheritance
- Verify ElementName resolution
- Test binding updates across tree
- Performance benchmarks for large trees

## Known Limitations

1. **No Property Coercion**: Unlike WPF, values aren't coerced automatically
2. **No Property Validation**: Validation must be done explicitly
3. **No Animation Support**: Property animations not yet implemented
4. **Limited Multi-Binding**: Only basic multi-binding scenarios supported

## Future Enhancements

### Planned Features
1. **Property Coercion**: Automatic value coercion/clamping
2. **Property Validation**: Built-in validation framework
3. **Property Metadata Inheritance**: Inherit metadata from base classes
4. **Attached Properties**: Properties attachable to any DependencyObject
5. **Property Triggers**: Automatic actions on property changes
6. **Expression Binding**: Compile-time binding expressions
7. **Weak References**: Weak parent/child references to prevent leaks

### Performance Improvements
1. **Inline Storage**: Store small values inline (avoid std::any overhead)
2. **Property Batching**: Batch multiple property changes
3. **Deferred Updates**: Delay non-critical updates
4. **Value Caching**: Cache computed values

## Migration Notes

### From Old API
The refactored API changes:

**Old**:
```cpp
button->Background("#FF0000");  // Convenience method
```

**New**:
```cpp
button->SetValue(Button::BackgroundProperty(), "#FF0000");  // Explicit
// Or with convenience wrapper:
button->SetBackground("#FF0000");
```

### Breaking Changes
- Property accessors now use dependency property system
- Convenience methods may need updating
- Binding syntax unchanged

## See Also

- [API Documentation](../../API/Binding/DependencyObject.md)
- [PropertyStore Design](PropertyStore.md)
- [BindingContext Design](BindingContext.md)
- [DependencyProperty Design](DependencyProperty.md)
- [Architecture Overview](../Designs/Architecture-Refactoring.md)
