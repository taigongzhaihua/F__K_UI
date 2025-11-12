# F__K_UI Documentation

Welcome to the F__K_UI framework documentation! This directory contains comprehensive guides and references for developing with F__K_UI.

## 📚 Documentation Structure

### Getting Started
- **[Getting Started Guide](GettingStarted.md)** - Your first steps with F__K_UI
  - Installation and setup
  - Your first application
  - Core concepts and examples
  - Common patterns

### Development
- **[Development Guide](Development.md)** - For contributors and advanced users
  - Project structure
  - Architecture overview
  - Adding new controls
  - Code style and conventions
  - Debugging tips

### API Reference
- **[API Documentation](API/README.md)** - Complete API reference
  - Module-by-module documentation
  - Class references
  - Property and method details

### Architecture & Design
- **[Architecture Refactoring](Designs/Architecture-Refactoring.md)** - Framework architecture
- **[UI Refactor Architecture](Designs/UI-Refactor-Architecture.md)** - Detailed class design
- **[Memory Management Strategy](Designs/Memory-Management-Strategy.md)** - Memory handling
- **[Window Implementation](Designs/Window-Implementation.md)** - Window system design

### API Design Documents
- **[Chainable Content API](Designs/ChainableContentAPI.md)** - Fluent API design
- **[Children Batch API](Designs/ChildrenBatchAPI.md)** - Batch operations
- **[Nested Chaining Style](Designs/NestedChainingStyle.md)** - Chaining patterns

### Status & Progress
- **[Implementation Status](Implementation-Status.md)** - Current progress and completion tracking

## 🚀 Quick Navigation

### I'm New to F__K_UI
Start here: [Getting Started Guide](GettingStarted.md)

### I Want to Build an App
1. Read [Getting Started](GettingStarted.md)
2. Explore examples in `/examples` directory
3. Check [API Reference](API/README.md) for specific controls

### I Want to Contribute
1. Read [Development Guide](Development.md)
2. Check [Implementation Status](Implementation-Status.md) for open tasks
3. Review [Architecture](Designs/Architecture-Refactoring.md)

### I'm Looking for Specific Information
- **Controls**: See [API/UI](API/UI/)
- **Data Binding**: See [API/Binding](API/Binding/)
- **Rendering**: See [API/Render](API/Render/)
- **Application**: See [API/App](API/App/)

## 🎯 Framework Overview

F__K_UI is a modern C++ UI framework inspired by WPF, featuring:

### Core Features
- **Declarative UI**: Fluent, chainable API for building interfaces
- **Data Binding**: Comprehensive dependency property and binding system
- **Flexible Layout**: StackPanel, Grid, Canvas, and custom panels
- **Rich Controls**: Button, TextBlock, TextBox, Image, Border, and more
- **Transform System**: Rotate, scale, translate, skew, and matrix transforms
- **Event System**: Mouse, keyboard, and focus management
- **Modern Rendering**: OpenGL-based rendering pipeline

### Current Status (82% Complete)
- ✅ **Dependency Properties**: Full implementation with change notification
- ✅ **Data Binding**: Two-way binding, converters, and expressions
- ✅ **Visual Tree**: Complete parent-child hierarchy management
- ✅ **Layout System**: Measure/Arrange with constraint-based layout
- ✅ **Core Controls**: Button, TextBlock, Border, Image, Grid, StackPanel
- ✅ **Transform System**: Complete 2D transformation support
- ✅ **Input Management**: Mouse, keyboard, and focus navigation
- ✅ **Rendering**: Full OpenGL backend with text and image support
- ⏳ **Styles/Templates**: Planned for Phase 2

## 📦 Module Overview

### App Module (`fk::app`)
Application lifecycle and window management
- `Application` - Application singleton
- `Window` - Top-level windows

### Core Module (`fk::core`)
Fundamental utilities and infrastructure
- `DependencyObject` - Base for objects with dependency properties
- `Dispatcher` - Thread-safe event dispatching
- `Event<T>` - Type-safe event system

### Binding Module (`fk::binding`)
Data binding and dependency property system
- `DependencyProperty` - Property metadata
- `Binding` - Binding configuration
- `BindingExpression` - Active binding
- `ObservableObject` - ViewModel base

### UI Module (`fk::ui`)
Visual elements and controls
- **Elements**: `Visual`, `UIElement`, `FrameworkElement`
- **Containers**: `Panel`, `StackPanel`, `Grid`, `Canvas`
- **Controls**: `Control`, `ContentControl`, `Button`, `TextBox`
- **Display**: `TextBlock`, `Border`, `Image`
- **Collections**: `ItemsControl`, `ScrollViewer`

### Render Module (`fk::render`)
Rendering pipeline and graphics
- `IRenderer` - Renderer interface
- `GlRenderer` - OpenGL implementation
- `RenderCommand` - Drawing commands
- `TextRenderer` - Font rendering

## 🔧 Technology Stack

- **Language**: C++17/20
- **Build**: CMake 3.20+
- **Graphics**: OpenGL 3.3+
- **Windowing**: GLFW 3.x
- **Images**: stb_image
- **Fonts**: FreeType

## 📖 Example Code

### Hello World
```cpp
#include "fk/app/Application.h"
#include "fk/ui/Window.h"
#include "fk/ui/Button.h"

int main() {
    auto app = app::Application::Create();
    auto window = app->CreateWindow();
    window->Title("Hello F__K_UI")->Width(800)->Height(600);
    
    auto button = window->SetContent<Button>();
    button->Content("Click Me!")->Width(200)->Height(50);
    
    return app->Run();
}
```

### Layout Example
```cpp
auto stack = window->SetContent<StackPanel>();
stack->Orientation(Orientation::Vertical)->Spacing(10);

stack->AddChild<Button>()->Content("Button 1");
stack->AddChild<Button>()->Content("Button 2");
stack->AddChild<TextBlock>()->Text("Some text");
```

### Transform Example
```cpp
auto rotateTransform = new RotateTransform(45.0f, 100.0f, 100.0f);
element->SetRenderTransform(rotateTransform);
```

## 🛠️ Building the Framework

```bash
# Clone
git clone https://github.com/taigongzhaihua/F__K_UI.git
cd F__K_UI

# Build
mkdir build && cd build
cmake ..
cmake --build . -j8

# Run examples
./hello_world
./phase1_enhancement_demo
```

## 📈 Roadmap

### ✅ Phase 1 (82% - Complete)
Core framework with essential controls and transforms

### 🔄 Phase 2 (In Progress)
Styles, templates, and advanced controls

### 🎯 Phase 3 (Planned)
Animations, themes, and advanced features

See [Implementation Status](Implementation-Status.md) for detailed progress.

## 💡 Getting Help

- **Start with**: [Getting Started Guide](GettingStarted.md)
- **API Questions**: Check [API Documentation](API/README.md)
- **Architecture**: Review [Design Documents](Designs/)
- **Bugs/Issues**: Open an issue on GitHub

## 🤝 Contributing

Contributions are welcome! See the [Development Guide](Development.md) for:
- Code style guidelines
- Architecture overview
- Adding new features
- Testing procedures

---

**Happy Coding! 🎉**

*Documentation last updated: November 2025*
