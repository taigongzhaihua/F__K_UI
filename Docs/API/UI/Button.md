# Button

## Overview

**Purpose**: Interactive button control that responds to pointer clicks

**Namespace**: `fk::ui`

**Inheritance**: `ContentControl<Button>` → `Control` → `FrameworkElement` → `UIElement` → `Visual` → `DependencyObject`

**Header**: `fk/ui/Button.h`

## Description

`Button` is a clickable control that can contain any content (text, images, or complex layouts). It provides visual feedback on hover and press, and fires a `Click` event when activated.

The button uses the CRTP pattern (`ContentControl<Button>`) for compile-time polymorphism and method chaining.

## Public Interface

### Constructor

```cpp
Button();
virtual ~Button() = default;
```

Creates a new button instance.

**Example**:
```cpp
auto button = std::make_shared<Button>();
```

### Events

#### Click
```cpp
core::Event<> Click;
```

Fired when the button is clicked (pointer pressed and released over the button).

**Usage**:
```cpp
button->Click += []() {
    std::cout << "Button clicked!" << std::endl;
};

// Or with disconnection:
auto connection = button->Click += []() { /* handler */ };
connection.Disconnect();
```

## Inherited Members

### From ContentControl

#### Content Management
```cpp
// Set content (from ContentControl)
auto textBlock = std::make_shared<TextBlock>();
button->SetContent(textBlock);

// Get content
auto content = button->GetContent();
```

See [ContentControl](ContentControl.md) for details.

### From Control

- **Template**: Control template support
- **Padding**: Internal padding for content
- **Background**, **Foreground**, **BorderBrush**, **BorderThickness**: Visual styling

See [Control](Control.md) for details.

### From FrameworkElement

- **Width**, **Height**: Size properties
- **MinWidth**, **MaxWidth**, **MinHeight**, **MaxHeight**: Size constraints
- **Margin**: External spacing
- **HorizontalAlignment**, **VerticalAlignment**: Alignment in parent
- **DataContext**: Inherited data context

See [FrameworkElement](FrameworkElement.md) for details.

### From UIElement

- **Visibility**: Visibility state
- **IsEnabled**: Enabled state
- **Opacity**: Transparency
- **RenderTransform**: Transformations
- **Clip**: Clipping region

See [UIElement](UIElement.md) for details.

## Protected Interface

### Event Handlers

```cpp
protected:
    void OnPointerPressed(PointerEventArgs& e) override;
    void OnPointerReleased(PointerEventArgs& e) override;
    void OnPointerEntered(PointerEventArgs& e) override;
    void OnPointerExited(PointerEventArgs& e) override;
```

Override these methods to customize pointer interaction behavior.

**Default Behavior**:
- `OnPointerPressed`: Sets `isPressed_` flag
- `OnPointerReleased`: Fires `Click` event if pressed over button
- `OnPointerEntered`: Visual feedback (hover state)
- `OnPointerExited`: Clears hover state

## Usage Examples

### Basic Text Button

```cpp
auto button = std::make_shared<Button>();

// Set text content (creates TextBlock automatically via ContentControl)
auto textBlock = std::make_shared<TextBlock>();
textBlock->SetText("Click Me");
button->SetContent(textBlock);

// Set size
button->SetValue(UIElement::WidthProperty(), 120.0);
button->SetValue(UIElement::HeightProperty(), 40.0);

// Handle click
button->Click += []() {
    std::cout << "Button clicked!" << std::endl;
};
```

### Styled Button

```cpp
auto button = std::make_shared<Button>();

// Set text
auto text = std::make_shared<TextBlock>();
text->SetText("Submit");
button->SetContent(text);

// Style the button
button->SetValue(Control::BackgroundProperty(), Color{0.0f, 0.5f, 1.0f, 1.0f});
button->SetValue(Control::ForegroundProperty(), Color{1.0f, 1.0f, 1.0f, 1.0f});
button->SetValue(Control::PaddingProperty(), Thickness(16, 8, 16, 8));
button->SetValue(Control::BorderThicknessProperty(), Thickness(2));
button->SetValue(Control::BorderBrushProperty(), Color{0.0f, 0.3f, 0.7f, 1.0f});

// Set corner radius (if supported by template)
button->SetValue(CornerRadiusProperty(), CornerRadius(4.0f));
```

### Button with Complex Content

```cpp
auto button = std::make_shared<Button>();

// Create complex content layout
auto panel = std::make_shared<StackPanel>();
panel->SetOrientation(Orientation::Horizontal);
panel->SetSpacing(8.0);

auto icon = std::make_shared<TextBlock>();
icon->SetText("📁");  // Icon
icon->SetFontSize(20.0);

auto label = std::make_shared<TextBlock>();
label->SetText("Open File");
label->SetFontSize(14.0);

panel->AddChild(icon);
panel->AddChild(label);

button->SetContent(panel);
button->SetValue(UIElement::WidthProperty(), 150.0);
button->SetValue(UIElement::HeightProperty(), 50.0);

button->Click += []() {
    // Open file dialog
};
```

### Data-Bound Button

```cpp
// ViewModel
class MyViewModel : public ObservableObject {
public:
    FK_PROPERTY(std::string, ButtonLabel, "Submit")
    FK_PROPERTY(bool, CanSubmit, true)
    
    void Submit() {
        std::cout << "Submitting..." << std::endl;
    }
};

auto viewModel = std::make_shared<MyViewModel>();

// Create button
auto button = std::make_shared<Button>();

// Bind content to ViewModel
auto contentBinding = Binding("ButtonLabel");
auto textBlock = std::make_shared<TextBlock>();
textBlock->SetBinding(TextBlock::TextProperty(), contentBinding);
button->SetContent(textBlock);

// Bind IsEnabled to ViewModel
auto enabledBinding = Binding("CanSubmit");
button->SetBinding(UIElement::IsEnabledProperty(), enabledBinding);

// Handle click
button->Click += [viewModel]() {
    viewModel->Submit();
};

// Set DataContext
button->SetDataContext(viewModel);
```

### Disabled Button

```cpp
auto button = std::make_shared<Button>();
auto text = std::make_shared<TextBlock>();
text->SetText("Disabled");
button->SetContent(text);

// Disable the button
button->SetValue(UIElement::IsEnabledProperty(), false);

// Click event won't fire when disabled
button->Click += []() {
    // This won't be called
};
```

### Button with Transform

```cpp
auto button = std::make_shared<Button>();
auto text = std::make_shared<TextBlock>();
text->SetText("Rotated");
button->SetContent(text);

// Apply rotation transform
auto rotateTransform = std::make_shared<RotateTransform>(15.0f);
button->SetValue(UIElement::RenderTransformProperty(), rotateTransform);
```

## Visual States

The button's visual appearance changes based on its state:

| State | Condition | Visual Feedback |
|-------|-----------|-----------------|
| Normal | Default state | Standard appearance |
| Hover | Pointer over button | Highlight (via template) |
| Pressed | Pointer pressed down | Pressed appearance |
| Disabled | `IsEnabled = false` | Grayed out (via template) |

Visual feedback is typically handled by the control template. The button class manages the state transitions.

## Event Flow

Click event sequence:

1. **PointerPressed**: User presses pointer over button
   - `OnPointerPressed()` called
   - `isPressed_` flag set to `true`

2. **PointerReleased**: User releases pointer
   - `OnPointerReleased()` called
   - If pointer still over button AND `isPressed_` is `true`:
     - `Click` event fired
   - `isPressed_` flag cleared

3. **PointerExited**: If pointer leaves before release
   - Click is cancelled (no event fired)

## Customization

### Custom Button Behavior

```cpp
class MyButton : public Button {
public:
    MyButton() : Button() {}
    
protected:
    void OnPointerPressed(PointerEventArgs& e) override {
        Button::OnPointerPressed(e);
        // Custom logic
        std::cout << "Custom press behavior" << std::endl;
    }
    
    void OnPointerReleased(PointerEventArgs& e) override {
        // Custom logic before click
        if (validateBeforeClick()) {
            Button::OnPointerReleased(e);
        }
    }
    
private:
    bool validateBeforeClick() {
        // Custom validation
        return true;
    }
};
```

### Custom Visual Styling

Visual styling is controlled by the Control template and Style system:

```cpp
// TODO: Add example when Style/Template system is fully documented
```

## Performance Considerations

- **Event Handlers**: Keep click handlers lightweight; use async operations for heavy work
- **Complex Content**: Avoid overly complex content hierarchies (impacts layout performance)
- **Many Buttons**: Use templates and styles to share visual resources

## Accessibility

Consider adding accessibility properties:

```cpp
// Set accessible name (future feature)
// button->SetAccessibleName("Submit Form Button");

// Set accessible description
// button->SetAccessibleDescription("Submits the registration form");
```

## Related Classes

- [ContentControl](ContentControl.md) - Base class for content hosting
- [Control](Control.md) - Templatable control base
- [FrameworkElement](FrameworkElement.md) - Layout and sizing
- [UIElement](UIElement.md) - Input and rendering
- [TextBlock](TextBlock.md) - Common content type
- [StackPanel](StackPanel.md) - For complex button content
- [ControlTemplate](ControlTemplate.md) - Visual customization

## See Also

- [Design Document](../../Design/UI/Button.md)
- [Input Events Guide](../../Development.md#input-events)
- [Getting Started - Controls](../../GettingStarted.md#controls)
