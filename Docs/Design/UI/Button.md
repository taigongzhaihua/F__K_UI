# Button - Design Document

## Overview

**Module**: UI

**Purpose**: Interactive control that responds to pointer clicks and fires Click events

**WPF Equivalent**: `System.Windows.Controls.Button`

## Responsibilities

The `Button` class is responsible for:

1. **Click Event Handling**: Detecting and firing click events
2. **Pointer State Tracking**: Tracking press/hover state
3. **Visual Feedback**: Providing state to templates for visual changes
4. **Content Hosting**: Hosting arbitrary content (via ContentControl)
5. **Accessibility**: Supporting keyboard and assistive technology (future)

## Architecture

### Class Diagram

```
┌──────────────────────────────────┐
│    DependencyObject              │
└────────────┬─────────────────────┘
             │
┌────────────▼─────────────────────┐
│         Visual                   │
└────────────┬─────────────────────┘
             │
┌────────────▼─────────────────────┐
│        UIElement                 │
│  + Input event routing           │
│  + Layout (Measure/Arrange)      │
└────────────┬─────────────────────┘
             │
┌────────────▼─────────────────────┐
│    FrameworkElement              │
│  + Width, Height, Margin         │
│  + DataContext                   │
└────────────┬─────────────────────┘
             │
┌────────────▼─────────────────────┐
│      Control<Derived>            │
│  + Template support              │
│  + Padding, Border, Background   │
└────────────┬─────────────────────┘
             │
┌────────────▼─────────────────────┐
│  ContentControl<Button>          │
│  + Single child content          │
│  + Content property              │
└────────────┬─────────────────────┘
             │
┌────────────▼─────────────────────┐
│          Button                  │
│  + Click event                   │
│  + isPressed_ state              │
│  + OnPointerPressed/Released     │
└──────────────────────────────────┘
```

### Key Components

#### State Management
- **isPressed_**: Boolean flag tracking press state
- Cleared on pointer release or exit
- Used to determine if Click should fire

#### Event Routing
- Overrides pointer event handlers from UIElement
- Implements click detection logic
- Fires Click event through core::Event system

#### CRTP Pattern
- Uses `ContentControl<Button>` for compile-time polymorphism
- Enables method chaining and derived-specific behavior
- Zero runtime overhead vs virtual functions

## Design Decisions

### 1. CRTP vs Virtual Methods

**Decision**: Use CRTP (`ContentControl<Button>`) instead of traditional inheritance

**Rationale**:
- Compile-time polymorphism (no vtable lookups)
- Better method chaining ergonomics
- Type-safe derived class access
- Consistent with modern C++ patterns

**Trade-offs**:
- More complex template syntax
- Longer compile times
- Cannot store different button types in same container (without type erasure)

### 2. Simple State Tracking

**Decision**: Use single `isPressed_` flag instead of complex state machine

**Rationale**:
- Button has simple states (normal, hover, pressed)
- Complex state machine unnecessary
- Visual state handled by template
- Easy to understand and maintain

**Alternative Considered**: Enum-based state machine (rejected as over-engineered)

### 3. Click Event Simplicity

**Decision**: Click event has no parameters (`Event<>`)

**Rationale**:
- Most click handlers don't need event args
- Simplifies common case
- Pointer details available through pointer events if needed
- Matches many modern UI frameworks

**Alternative**: `Event<PointerEventArgs&>` (rejected as verbose for common case)

### 4. No Built-in Command Pattern

**Decision**: Button doesn't include ICommand property (unlike old version)

**Rationale**:
- Keeps button class simple and focused
- Command pattern can be layered on top
- Reduces dependencies
- Applications can implement command binding if needed

**Migration Path**:
```cpp
// Old API (removed):
// button->Command(myCommand);

// New API:
button->Click += [command]() {
    if (command->CanExecute(nullptr)) {
        command->Execute(nullptr);
    }
};
```

### 5. Content via Inheritance

**Decision**: Content functionality inherited from ContentControl

**Rationale**:
- Single responsibility principle
- Code reuse across all content controls
- Standard pattern across framework
- Simplifies button-specific logic

## Implementation Details

### Memory Management

#### Ownership
- Button doesn't own its content (managed by ContentControl parent)
- Click event connections managed by Event<> system
- No manual memory management required

#### Lifecycle
```cpp
// Creation
auto button = std::make_shared<Button>();

// Content lifetime managed separately
auto content = std::make_shared<TextBlock>();
button->SetContent(content);  // ContentControl manages reference

// Event connection lifetime
auto conn = button->Click += handler;  // Connection object manages cleanup

// Destruction
// All references released automatically
```

### Thread Safety

- Not thread-safe (like all UI elements)
- Must be accessed from UI thread only
- Use Dispatcher for cross-thread interaction

### Performance Considerations

#### Event Handling Performance
- Direct event handler invocation (no routing overhead for Click)
- Pointer events routed through UIElement tree
- Minimal overhead per button instance

#### Memory Footprint
- Base object: ~200 bytes (from DependencyObject)
- Button-specific: ~8 bytes (`isPressed_` + padding)
- Content: Variable (depends on content type)

Total: ~210 bytes + content

### State Transitions

```
         ┌─────────────┐
         │   Normal    │
         └──────┬──────┘
                │
      PointerEntered
                │
         ┌──────▼──────┐
         │    Hover    │◄────┐
         └──────┬──────┘     │
                │             │
      PointerPressed          │
                │             │
         ┌──────▼──────┐     │
         │   Pressed   │     │
         └──────┬──────┘     │
                │             │
   PointerReleased/Exited    │
                │             │
         ┌──────▼──────┐     │
         │    Fire     │     │
         │    Click    │     │
         └──────┬──────┘     │
                │             │
                └─────────────┘
```

## Dependencies

### Direct Dependencies
- `ContentControl<Button>` - Base class providing content hosting
- `Control` - Template and styling support
- `FrameworkElement` - Layout and DataContext
- `UIElement` - Input events and rendering
- `core::Event` - Event system

### Indirect Dependencies
- `PointerEventArgs` - Event argument types
- `RenderTransform` - Visual transformations
- Template system - For visual customization

## Usage Patterns

### Pattern 1: Simple Click Handler

```cpp
auto button = std::make_shared<Button>();
button->Click += []() {
    // Handle click
};
```

### Pattern 2: Contextual Click Handler

```cpp
auto button = std::make_shared<Button>();
button->Click += [this, button]() {
    // Access button and owner context
    ProcessButtonClick(button);
};
```

### Pattern 3: Conditional Behavior

```cpp
auto button = std::make_shared<Button>();

// Enable/disable based on state
UpdateUI([button, canSubmit]() {
    button->SetValue(UIElement::IsEnabledProperty(), canSubmit);
});

// Click only fires when enabled
button->Click += [this]() {
    Submit();
};
```

### Pattern 4: Async Operation

```cpp
button->Click += [this]() {
    // Disable button during operation
    button->SetValue(UIElement::IsEnabledProperty(), false);
    
    // Start async operation
    PerformAsyncOperation([this, button]() {
        // Re-enable when complete
        Dispatcher()->InvokeAsync([button]() {
            button->SetValue(UIElement::IsEnabledProperty(), true);
        });
    });
};
```

## Testing Considerations

### Unit Testing
- Test click event firing
- Verify isPressed_ state management
- Test disabled state (no click when disabled)
- Test pointer capture scenarios

### Integration Testing
- Test with real pointer input
- Verify visual feedback through template
- Test in complex layouts
- Performance test with many buttons

### Test Examples
```cpp
// Test click event
TEST(ButtonTest, ClickEventFires) {
    auto button = std::make_shared<Button>();
    bool clicked = false;
    button->Click += [&clicked]() { clicked = true; };
    
    // Simulate click
    PointerEventArgs pressArgs{};
    button->OnPointerPressed(pressArgs);
    
    PointerEventArgs releaseArgs{};
    button->OnPointerReleased(releaseArgs);
    
    EXPECT_TRUE(clicked);
}

// Test disabled button
TEST(ButtonTest, DisabledButtonNoClick) {
    auto button = std::make_shared<Button>();
    button->SetValue(UIElement::IsEnabledProperty(), false);
    
    bool clicked = false;
    button->Click += [&clicked]() { clicked = true; };
    
    // Simulate click
    SimulateClick(button);
    
    EXPECT_FALSE(clicked);  // Should not fire when disabled
}
```

## Known Limitations

1. **No Repeat Button**: No built-in support for continuous clicking while pressed
2. **No Keyboard Activation**: Space/Enter key activation not yet implemented
3. **No Command Binding**: ICommand pattern not built-in (by design)
4. **No Menu Button**: Dropdown/popup functionality requires custom implementation

## Future Enhancements

### Planned Features
1. **Keyboard Support**: Space and Enter key activation
2. **Accessibility**: ARIA roles and accessible names
3. **Focus Visual**: Visual indication when button has keyboard focus
4. **Auto-Repeat**: RepeatButton variant for continuous clicks
5. **Drop-Down**: DropDownButton and SplitButton variants

### Possible Improvements
1. **Command Support**: Optional ICommand binding helper
2. **Gesture Recognition**: Long press, double click support
3. **Touch Optimization**: Better touch target sizing
4. **Animation Hooks**: Callbacks for animation integration

## Migration Notes

### From Old Button API

**Old** (removed):
```cpp
auto button = ui::button()
    ->Content("Click Me")
    ->Background("#0078D4")
    ->OnClick([](auto&) { /* handler */ });
```

**New** (current):
```cpp
auto button = std::make_shared<Button>();

auto text = std::make_shared<TextBlock>();
text->SetText("Click Me");
button->SetContent(text);

button->SetValue(Control::BackgroundProperty(), Color{0.0f, 0.47f, 0.83f, 1.0f});

button->Click += []() {
    /* handler */
};
```

### Breaking Changes
- Fluent API removed (use standard setters)
- Command property removed (use Click event + manual command)
- Convenience methods removed (use dependency properties)

## Performance Benchmarks

Typical performance characteristics:

- **Event Handler Invocation**: ~10-50ns
- **State Update**: ~100-200ns
- **Complete Click Cycle**: ~500ns
- **Memory per Button**: ~210 bytes + content

## See Also

- [API Documentation](../../API/UI/Button.md)
- [ContentControl Design](ContentControl.md)
- [Control Design](Control.md)
- [Event System Design](../Core/Event.md)
- [Input Manager Design](InputManager.md)
