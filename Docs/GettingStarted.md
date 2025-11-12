# Getting Started with F__K_UI

## Introduction

F__K_UI is a modern C++ UI framework inspired by WPF, focusing on clean APIs and powerful functionality. This guide will help you get started with building your first application.

## Prerequisites

- **C++17 or later** compiler (MSVC, GCC, or Clang)
- **CMake 3.20+**
- **OpenGL 3.3+** support
- **Windows, Linux, or macOS** (primary support for Windows)

## Installation

### 1. Clone the Repository

```bash
git clone https://github.com/taigongzhaihua/F__K_UI.git
cd F__K_UI
```

### 2. Build the Project

#### Windows (PowerShell)

```powershell
mkdir build
cd build
cmake ..
cmake --build . -j 8
```

#### Linux/macOS

```bash
mkdir build
cd build
cmake ..
make -j8
```

## Your First Application

### Hello World Example

Create a simple window with a button:

```cpp
#include "fk/app/Application.h"
#include "fk/ui/Window.h"
#include "fk/ui/Button.h"

int main() {
    // Create application
    auto app = app::Application::Create();
    
    // Create main window
    auto window = app->CreateWindow();
    window->Title("My First F__K_UI App")
          ->Width(800)
          ->Height(600);
    
    // Add a button
    auto button = window->SetContent<Button>();
    button->Content("Click Me!")
          ->Width(200)
          ->Height(50);
    
    // Run the application
    return app->Run();
}
```

### Understanding the Code

1. **Application Creation**: `Application::Create()` initializes the framework
2. **Window Setup**: Create and configure a window using fluent API
3. **Content Addition**: Use `SetContent<>()` to set window content
4. **Run Loop**: `app->Run()` starts the message loop

## Core Concepts

### 1. Fluent API (Method Chaining)

F__K_UI uses a fluent, chainable API for clean code:

```cpp
button->Content("Submit")
      ->Width(100)
      ->Height(40)
      ->Background(Colors::Blue)
      ->Foreground(Colors::White);
```

### 2. Visual Tree

UI elements form a parent-child hierarchy:

```cpp
auto panel = window->SetContent<StackPanel>();
panel->AddChild<Button>()->Content("Button 1");
panel->AddChild<Button>()->Content("Button 2");
panel->AddChild<TextBlock>()->Text("Some text");
```

### 3. Layout System

Panels automatically arrange children:

- **StackPanel**: Arranges children vertically or horizontally
- **Grid**: Flexible grid-based layout
- **Canvas**: Absolute positioning

```cpp
auto stack = window->SetContent<StackPanel>();
stack->Orientation(Orientation::Vertical)
     ->Spacing(10);
```

### 4. Dependency Properties

Properties that support binding and change notification:

```cpp
// Set property
button->Width(200);

// Get property
double w = button->Width();
```

### 5. Data Binding

Connect UI to data sources:

```cpp
// Create binding
auto binding = new Binding("PropertyName");
binding->Source(dataObject);

// Apply binding
element->SetBinding(UIElement::WidthProperty, binding);
```

## Layout Examples

### StackPanel Layout

```cpp
auto stack = window->SetContent<StackPanel>();
stack->Orientation(Orientation::Vertical)
     ->Spacing(10)
     ->Padding(Thickness(20));

stack->AddChild<Button>()->Content("Top Button")->Height(40);
stack->AddChild<Button>()->Content("Middle Button")->Height(40);
stack->AddChild<Button>()->Content("Bottom Button")->Height(40);
```

### Grid Layout

```cpp
auto grid = window->SetContent<Grid>();

// Define rows and columns
grid->RowDefinitions()->Add(new RowDefinition(100));  // Fixed height
grid->RowDefinitions()->Add(new RowDefinition());     // Fill remaining
grid->ColumnDefinitions()->Add(new ColumnDefinition()); // Fill
grid->ColumnDefinitions()->Add(new ColumnDefinition()); // Fill

// Add elements with grid positions
auto btn1 = grid->AddChild<Button>();
btn1->Content("Top Left");
Grid::SetRow(btn1, 0);
Grid::SetColumn(btn1, 0);

auto btn2 = grid->AddChild<Button>();
btn2->Content("Top Right");
Grid::SetRow(btn2, 0);
Grid::SetColumn(btn2, 1);
```

## Styling Elements

### Colors and Brushes

```cpp
element->Background(Colors::Blue);
element->Foreground(Colors::White);
element->BorderBrush(Colors::Gray);
element->BorderThickness(Thickness(2));
```

### Transformations

```cpp
// Rotate element
auto rotateTransform = new RotateTransform(45.0f, 100.0f, 100.0f);
element->SetRenderTransform(rotateTransform);

// Scale element
auto scaleTransform = new ScaleTransform(1.5f, 1.5f);
element->SetRenderTransform(scaleTransform);

// Translate element
auto translateTransform = new TranslateTransform(50.0f, 50.0f);
element->SetRenderTransform(translateTransform);
```

## Event Handling

### Click Events

```cpp
button->Click([](const RoutedEventArgs& e) {
    std::cout << "Button clicked!" << std::endl;
});
```

### Mouse Events

```cpp
element->MouseEnter([](const MouseEventArgs& e) {
    std::cout << "Mouse entered" << std::endl;
});

element->MouseLeave([](const MouseEventArgs& e) {
    std::cout << "Mouse left" << std::endl;
});
```

### Keyboard Events

```cpp
element->KeyDown([](const KeyEventArgs& e) {
    if (e.Key == Key::Enter) {
        std::cout << "Enter key pressed" << std::endl;
    }
});
```

## Advanced Features

### Focus Management

```cpp
// Set focus
element->Focus();

// Check if focused
if (element->IsFocused()) {
    // ...
}

// Keyboard navigation (Arrow keys automatically work)
```

### Image Loading

```cpp
auto image = panel->AddChild<Image>();
image->Source("path/to/image.png")
     ->Width(200)
     ->Height(200)
     ->Stretch(Stretch::Uniform);
```

### Collections and ItemsControl

```cpp
auto itemsControl = panel->AddChild<SimpleItemsControl>();
auto& items = itemsControl->GetItems();

// Add items
items.Add("Item 1");
items.Add("Item 2");
items.Add("Item 3");

// Listen to changes
items.CollectionChanged([](const CollectionChangedEventArgs& args) {
    if (args.Action == NotifyCollectionChangedAction::Add) {
        std::cout << "Item added!" << std::endl;
    }
});
```

## Running Examples

The repository includes several example applications:

```powershell
# Hello World
.\hello_world.exe

# Button Example
.\button_example.exe

# Image Demo
.\image_demo.exe

# Phase 1 Enhancement Demo (comprehensive)
.\phase1_enhancement_demo.exe
```

## Next Steps

- Explore the [API Documentation](API/README.md)
- Read about [Architecture](Designs/Architecture-Refactoring.md)
- Check [Implementation Status](Implementation-Status.md)
- Study the examples in the `examples/` directory

## Troubleshooting

### Build Errors

- Ensure you have C++17 support enabled
- Check that all dependencies (GLFW, FreeType, stb_image) are found by CMake
- Try cleaning the build directory and rebuilding

### Runtime Errors

- Verify OpenGL 3.3+ support on your system
- Check that image paths are correct and files exist
- Enable debug logging to see detailed messages

## Getting Help

- Check existing examples for reference
- Review the API documentation
- Open an issue on GitHub for bugs or questions

---

**Happy Coding! 🎉**
