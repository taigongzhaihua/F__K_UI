# DependencyObject

## Overview

**Purpose**: Base class for objects that support dependency properties, data binding, and logical tree relationships.

**Namespace**: `fk::binding`

**Inheritance**: None (root base class)

**Header**: `fk/binding/DependencyObject.h`

## Description

`DependencyObject` is the foundational base class for the F__K_UI property system. It provides infrastructure for:

- **Dependency Properties**: Properties with metadata, change notification, and value coercion
- **Data Binding**: Automatic synchronization between properties and data sources
- **Logical Tree**: Parent-child relationships for element name resolution and DataContext inheritance
- **Value Sources**: Tracking where property values come from (local, binding, default, etc.)

All UI elements and many framework classes derive from `DependencyObject`.

## Public Interface

### Property Value Management

#### GetValue
```cpp
const std::any& GetValue(const DependencyProperty& property) const;

template<typename T>
T GetValue(const DependencyProperty& property) const;
```
Gets the current effective value of a dependency property.

**Parameters**:
- `property`: The dependency property to query

**Returns**: The property value (typed or as `std::any`)

**Example**:
```cpp
double width = element->GetValue<double>(UIElement::WidthProperty());
```

#### SetValue
```cpp
void SetValue(const DependencyProperty& property, std::any value);

template<typename T>
void SetValue(const DependencyProperty& property, T&& value);

void SetValue(const DependencyProperty& property, Binding binding);
```
Sets the local value of a dependency property.

**Parameters**:
- `property`: The dependency property to set
- `value`: The new value (or Binding for data binding)

**Example**:
```cpp
element->SetValue(UIElement::WidthProperty(), 200.0);
// Or with binding:
element->SetValue(UIElement::WidthProperty(), Binding("Width"));
```

#### ClearValue
```cpp
void ClearValue(const DependencyProperty& property);
```
Clears the local value, reverting to default or inherited value.

#### GetValueSource
```cpp
ValueSource GetValueSource(const DependencyProperty& property) const;
```
Returns where the current value comes from (Local, Default, Binding, etc.).

### Data Binding

#### SetBinding
```cpp
void SetBinding(const DependencyProperty& property, Binding binding);
void SetBinding(const DependencyProperty& property, MultiBinding binding);
```
Establishes a data binding on a property.

**Example**:
```cpp
auto binding = Binding("UserName");
binding.SetMode(BindingMode::TwoWay);
element->SetBinding(TextBox::TextProperty(), binding);
```

#### ClearBinding
```cpp
void ClearBinding(const DependencyProperty& property);
```
Removes the binding from a property.

#### GetBinding
```cpp
std::shared_ptr<BindingExpression> GetBinding(const DependencyProperty& property) const;
```
Gets the active binding expression for a property (if any).

#### UpdateSource
```cpp
void UpdateSource(const DependencyProperty& property);
```
Manually triggers UpdateSource for a two-way binding.

### DataContext Management

#### SetDataContext
```cpp
void SetDataContext(std::any value);

template<typename T>
void SetDataContext(T&& value);
```
Sets the data context for this element and its descendants.

**Example**:
```cpp
auto viewModel = std::make_shared<MyViewModel>();
window->SetDataContext(viewModel);
```

#### GetDataContext
```cpp
const std::any& GetDataContext() const noexcept;
bool HasDataContext() const noexcept;
```
Gets the data context or checks if one is set.

#### ClearDataContext
```cpp
void ClearDataContext();
```
Clears the local data context.

#### SetDataContextParent
```cpp
void SetDataContextParent(DependencyObject* parent);
```
Internal method to set up DataContext inheritance.

### Logical Tree Management

#### Element Naming
```cpp
void SetElementName(std::string name);
const std::string& GetElementName() const noexcept;
```
Sets/gets the element's name for ElementName binding.

#### Parent-Child Relationships
```cpp
void SetLogicalParent(DependencyObject* parent);
DependencyObject* GetLogicalParent() const noexcept;

void AddLogicalChild(DependencyObject* child);
void RemoveLogicalChild(DependencyObject* child);
const std::vector<DependencyObject*>& GetLogicalChildren() const noexcept;
```
Manages logical parent-child relationships.

#### Element Lookup
```cpp
DependencyObject* FindElementByName(std::string_view name);
const DependencyObject* FindElementByName(std::string_view name) const;
```
Searches the logical tree for an element by name.

### Binding Context

```cpp
BindingContext& GetBindingContext() noexcept;
const BindingContext& GetBindingContext() const noexcept;
```
Gets the binding context managing all bindings for this object.

## Events

### PropertyChanged
```cpp
PropertyChangedEvent PropertyChanged;
```
**Signature**: `Event<const DependencyProperty&, const std::any& oldValue, const std::any& newValue, ValueSource oldSource, ValueSource newSource>`

Fired when any dependency property value changes.

**Example**:
```cpp
element->PropertyChanged += [](const DependencyProperty& prop, 
                                const std::any& oldVal, 
                                const std::any& newVal,
                                ValueSource oldSrc,
                                ValueSource newSrc) {
    std::cout << "Property changed!" << std::endl;
};
```

### BindingChanged
```cpp
BindingChangedEvent BindingChanged;
```
**Signature**: `Event<const DependencyProperty&, const std::shared_ptr<BindingExpression>& old, const std::shared_ptr<BindingExpression>& new>`

Fired when a binding is added or removed from a property.

### DataContextChanged
```cpp
DataContextChangedEvent DataContextChanged;
```
**Signature**: `Event<const std::any& oldValue, const std::any& newValue>`

Fired when the DataContext changes.

## Protected Interface

### Virtual Callbacks

```cpp
protected:
    virtual void OnPropertyChanged(const DependencyProperty& property, 
                                   const std::any& oldValue, 
                                   const std::any& newValue, 
                                   ValueSource oldSource, 
                                   ValueSource newSource);
    
    virtual void OnBindingChanged(const DependencyProperty& property, 
                                  const std::shared_ptr<BindingExpression>& oldBinding, 
                                  const std::shared_ptr<BindingExpression>& newBinding);
    
    virtual void OnDataContextChanged(const std::any& oldValue, 
                                      const std::any& newValue);
```

Override these methods in derived classes to respond to changes.

### Property Store Access

```cpp
protected:
    PropertyStore& MutablePropertyStore() noexcept;
    const PropertyStore& GetPropertyStore() const noexcept;
```

Direct access to the underlying property storage (for advanced scenarios).

## Usage Examples

### Basic Property Usage

```cpp
// Create an element
auto element = std::make_shared<UIElement>();

// Set properties
element->SetValue(UIElement::WidthProperty(), 200.0);
element->SetValue(UIElement::HeightProperty(), 100.0);

// Get properties
double width = element->GetValue<double>(UIElement::WidthProperty());

// Clear value (revert to default)
element->ClearValue(UIElement::WidthProperty());
```

### Data Binding

```cpp
// Create ViewModel
class MyViewModel : public ObservableObject {
public:
    FK_PROPERTY(std::string, Title, "Hello")
    FK_PROPERTY(double, Width, 300.0)
};

auto viewModel = std::make_shared<MyViewModel>();

// Set DataContext
window->SetDataContext(viewModel);

// Bind properties
auto titleBinding = Binding("Title");
textBlock->SetBinding(TextBlock::TextProperty(), titleBinding);

auto widthBinding = Binding("Width");
widthBinding.SetMode(BindingMode::TwoWay);
element->SetBinding(UIElement::WidthProperty(), widthBinding);

// When ViewModel.Title changes, TextBlock.Text updates automatically
// When element width changes, ViewModel.Width updates (TwoWay)
```

### Logical Tree and Element Names

```cpp
// Set element names
button->SetElementName("SubmitButton");
textBox->SetElementName("UserInput");

// Find elements by name
auto button = window->FindElementByName("SubmitButton");
auto textBox = window->FindElementByName("UserInput");

// Bind to another element
auto binding = Binding("Text");
binding.SetElementName("UserInput");
textBlock->SetBinding(TextBlock::TextProperty(), binding);
```

### Property Change Notification

```cpp
element->PropertyChanged += [](const DependencyProperty& prop,
                               const std::any& oldVal,
                               const std::any& newVal,
                               ValueSource oldSrc,
                               ValueSource newSrc) {
    if (&prop == &UIElement::WidthProperty()) {
        double oldWidth = std::any_cast<double>(oldVal);
        double newWidth = std::any_cast<double>(newVal);
        std::cout << "Width changed from " << oldWidth 
                  << " to " << newWidth << std::endl;
    }
};
```

## Value Source Priority

Property values can come from multiple sources. The priority order is:

1. **Local** - Directly set via `SetValue()`
2. **Binding** - From an active data binding
3. **Default** - The default value from property metadata

When multiple sources exist, the highest priority source wins.

## Thread Safety

- `DependencyObject` is NOT thread-safe by default
- Property access should occur on the object's associated thread (usually UI thread)
- Use `Dispatcher` to marshal calls from other threads

## Performance Considerations

- Property access is optimized with hash-based lookup
- Bindings add overhead - use sparingly for frequently updated properties
- Property change notifications have a cost - minimize unnecessary changes

## Related Classes

- [DependencyProperty](DependencyProperty.md) - Property metadata
- [Binding](Binding.md) - Binding configuration
- [BindingExpression](BindingExpression.md) - Active binding
- [PropertyStore](PropertyStore.md) - Value storage
- [ObservableObject](ObservableObject.md) - ViewModel base

## See Also

- [Design Document](../../Design/Binding/DependencyObject.md)
- [Data Binding Guide](../../GettingStarted.md#data-binding)
- [Dependency Property System](../../Development.md#dependency-properties)
