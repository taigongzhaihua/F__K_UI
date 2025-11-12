# F__K_UI Architecture Overview

## Introduction

F__K_UI is a modern C++ UI framework inspired by WPF, providing a comprehensive system for building desktop applications with declarative UI, data binding, and flexible layout.

## Module Organization

The framework is organized into 5 main modules:

```
F__K_UI/
├── app/        Application lifecycle and windowing
├── binding/    Dependency properties and data binding
├── core/       Core infrastructure and utilities
├── render/     Rendering pipeline and graphics
└── ui/         User interface elements and controls
```

## Overall Architecture Diagram

```
┌─────────────────────────────────────────────────────────────┐
│                     Application Layer                        │
│                  (app::Application, Window)                  │
└────────────────────────┬────────────────────────────────────┘
                         │
┌────────────────────────▼────────────────────────────────────┐
│                      UI Layer                                │
│         (Controls, Panels, Shapes, Visual Tree)              │
│  Button, TextBlock, Border, Image, StackPanel, Grid, etc.   │
└────────────────────────┬────────────────────────────────────┘
                         │
┌────────────────────────▼────────────────────────────────────┐
│                    Binding Layer                             │
│     (Dependency Properties, Data Binding, Validation)        │
│   DependencyObject, Binding, BindingExpression, etc.        │
└────────────────────────┬────────────────────────────────────┘
                         │
┌────────────────────────▼────────────────────────────────────┐
│                    Render Layer                              │
│        (Rendering Pipeline, OpenGL Backend, Text)            │
│      Renderer, GlRenderer, RenderBackend, TextRenderer      │
└────────────────────────┬────────────────────────────────────┘
                         │
┌────────────────────────▼────────────────────────────────────┐
│                     Core Layer                               │
│          (Threading, Events, Time, Logging)                  │
│         Dispatcher, Event, Clock, Timer, Logger             │
└─────────────────────────────────────────────────────────────┘
```

## Class Hierarchy and Relationships

### Complete Class Hierarchy

```
Core Infrastructure
└── core::Dispatcher (thread-safe message loop)
    └── core::DispatcherOperation (queued operation)
└── core::Event<T> (type-safe event system)
└── core::Clock (time tracking)
└── core::Timer (timer implementation)
└── core::Logger (logging interface)
    ├── core::ConsoleLogger
    ├── core::NullLogger
    └── core::LoggerManager

Binding System
└── binding::DependencyObject (property system base)
    ├── binding::ObservableObject (ViewModel base with INPC)
    └── ui::Visual (visual tree base)
        └── ui::UIElement (interactive element)
            └── ui::FrameworkElement<Derived> (layout support)
                ├── ui::Control<Derived> (templatable control)
                │   ├── ui::ContentControl<Derived> (single content)
                │   │   ├── ui::Button
                │   │   └── ui::Window
                │   └── ui::ItemsControl<Derived> (multiple items)
                ├── ui::Panel<Derived> (multi-child container)
                │   ├── ui::StackPanel
                │   └── ui::Grid
                ├── ui::Shape (vector graphics base)
                │   ├── ui::Rectangle
                │   └── ui::Ellipse
                ├── ui::TextBlock (text display)
                ├── ui::Border (decorator)
                └── ui::Image (bitmap display)

└── binding::DependencyProperty (property metadata)
└── binding::Binding (binding configuration)
    └── binding::TemplateBinding (template binding)
└── binding::MultiBinding (multi-source binding)
└── binding::BindingExpression (active binding)
└── binding::MultiBindingExpression (active multi-binding)
└── binding::BindingContext (manages bindings for object)
└── binding::BindingPath (property path parser)
└── binding::PropertyStore (value storage)
└── binding::PropertyAccessorRegistry (property accessors)
└── binding::RelativeSource (relative binding source)

Value Converters
└── binding::IValueConverter (converter interface)
    ├── binding::DefaultValueConverter
    └── binding::BooleanToStringConverter
└── binding::IMultiValueConverter (multi-value converter)

Validation
└── binding::ValidationRule (validation base)
    ├── binding::EmailValidationRule
    ├── binding::NotEmptyValidationRule
    ├── binding::RangeValidationRule
    ├── binding::StringLengthValidationRule
    └── binding::FunctionValidationRule

Interfaces
└── binding::INotifyPropertyChanged (property change notification)
└── binding::INotifyDataErrorInfo (error notification)

Rendering System
└── render::IRenderer (renderer interface)
    └── render::GlRenderer (OpenGL implementation)
└── render::Renderer (main renderer)
└── render::RenderBackend (platform abstraction)
    └── render::OpenGLRenderBackend
└── render::RenderContext (rendering context)
└── render::RenderHost (render host management)
└── render::RenderScene (scene management)
└── render::RenderTreeBuilder (builds render tree)
└── render::RenderCommandBuffer (command buffering)
└── render::RenderList (render list management)
└── render::TextRenderer (FreeType text rendering)
└── render::ColorUtils (color utilities)

UI Elements - Input Management
└── ui::InputManager (input event routing)
└── ui::FocusManager (focus management)
└── ui::RoutedEvent (routed event system)

UI Elements - Collections
└── ui::VisualCollection (visual children collection)
└── ui::ObservableCollection<T> (observable collection)

UI Elements - Transforms
└── ui::Transform (transform base)
    ├── ui::TranslateTransform (translation)
    ├── ui::ScaleTransform (scaling)
    ├── ui::RotateTransform (rotation)
    ├── ui::SkewTransform (skewing)
    ├── ui::MatrixTransform (matrix)
    └── ui::TransformGroup (composite)

UI Elements - Styles and Templates
└── ui::Style (style definition)
└── ui::Setter (property setter)
└── ui::SetterCollection (setter collection)
└── ui::FrameworkTemplate (template base)
    ├── ui::ControlTemplate (control template)
    └── ui::DataTemplate (data template)
└── ui::ResourceDictionary (resource storage)

UI Elements - Rendering
└── ui::DrawCommand (drawing command)

Application
└── app::Application (application singleton)
```

## Implementation Status Legend

- ✅ **已实现** - Fully implemented and tested
- 🔧 **需扩充** - Implemented but needs enhancement
- ⚠️ **假实现** - Stub/placeholder implementation
- ❌ **未实现** - Not yet implemented

## Module Responsibilities

### App Module

**Purpose**: Application lifecycle and top-level window management

**Classes (1)**:
- ✅ `Application` - Application singleton, window creation, message loop

**Responsibilities**:
- Application initialization and shutdown
- Window creation and management
- Message pump and event loop
- Application-wide resources

**Key Dependencies**: Core (Dispatcher), UI (Window)

---

### Core Module

**Purpose**: Fundamental infrastructure and utilities

**Classes (9)**:
- ✅ `Dispatcher` - Thread-safe message dispatching and invocation
- ✅ `DispatcherOperation` - Represents a queued operation
- ✅ `Event<T>` - Type-safe event system with connections
- ✅ `Clock` - Frame time tracking and timing
- ✅ `Timer` - Timer implementation with callbacks
- ✅ `Logger` - Logging interface
- ✅ `LoggerManager` - Manages logger instances
- ✅ `ConsoleLogger` - Console output logger
- ✅ `NullLogger` - No-op logger

**Responsibilities**:
- Thread synchronization and marshalling
- Event subscription and notification
- Time measurement and frame timing
- Logging infrastructure

**Key Dependencies**: None (base layer)

---

### Binding Module

**Purpose**: Dependency property system and data binding infrastructure

**Classes (25)**:

**Core Binding**:
- ✅ `DependencyObject` - Base class with dependency property support (120%)
- ✅ `DependencyProperty` - Property metadata and registration (110%)
- ✅ `Binding` - Data binding configuration
- ✅ `BindingExpression` - Active binding instance (115%)
- ✅ `BindingContext` - Manages bindings for an object
- ✅ `BindingPath` - Property path parsing and resolution
- ✅ `MultiBinding` - Multi-source binding configuration
- ✅ `MultiBindingExpression` - Active multi-binding instance
- 🔧 `TemplateBinding` - Template property binding (needs enhancement)
- 🔧 `RelativeSource` - Relative binding source specification (needs enhancement)

**Value Conversion**:
- ✅ `IValueConverter` - Value converter interface
- 🔧 `IMultiValueConverter` - Multi-value converter interface (basic impl)
- ✅ `DefaultValueConverter` - Default type conversion
- ✅ `BooleanToStringConverter` - Boolean to string conversion

**Validation**:
- ✅ `ValidationRule` - Validation rule base class
- ✅ `EmailValidationRule` - Email format validation
- ✅ `NotEmptyValidationRule` - Non-empty validation
- ✅ `RangeValidationRule` - Numeric range validation
- ✅ `StringLengthValidationRule` - String length validation
- ✅ `FunctionValidationRule` - Custom function validation

**Interfaces**:
- ✅ `INotifyPropertyChanged` - Property change notification interface
- ✅ `INotifyDataErrorInfo` - Error notification interface

**ViewModels**:
- ✅ `ObservableObject` - ViewModel base class

**Storage**:
- ✅ `PropertyStore` - Property value storage
- ✅ `PropertyAccessorRegistry` - Property accessor registration

**Responsibilities**:
- Property change notification and propagation
- Data binding between UI and data sources
- Value conversion and validation
- Property value storage and prioritization
- Logical tree and DataContext inheritance

**Key Dependencies**: Core (Event, Dispatcher)

---

### Render Module

**Purpose**: Rendering pipeline and graphics backend

**Classes (13)**:
- ✅ `IRenderer` - Renderer interface
- ✅ `Renderer` - Main renderer implementation (115%)
- ✅ `GlRenderer` - OpenGL-specific renderer
- ✅ `RenderBackend` - Platform abstraction (100%)
- ✅ `OpenGLRenderBackend` - OpenGL backend implementation
- ✅ `RenderContext` - Rendering context and state
- ✅ `RenderHost` - Manages render surface
- ✅ `RenderScene` - Scene graph management
- ✅ `RenderTreeBuilder` - Builds render tree from visual tree (110%)
- 🔧 `RenderCommandBuffer` - Command buffer for rendering (needs batching)
- 🔧 `RenderList` - Manages list of render commands (needs optimization)
- ✅ `TextRenderer` - Text rendering using FreeType
- ✅ `ColorUtils` - Color manipulation utilities

**Responsibilities**:
- Visual tree to render tree conversion
- OpenGL rendering and state management
- Text rendering with fonts
- Command buffering and batching
- Platform abstraction

**Key Dependencies**: Core (no direct UI dependencies at render level)

---

### UI Module

**Purpose**: User interface elements, controls, and visual tree

**Classes (50+)**:

**Base Classes (7)**:
- ✅ `Visual` - Visual tree node base (100%)
- ✅ `UIElement` - Interactive element with input and layout (110%)
- ✅ `FrameworkElement<Derived>` - Layout-aware element with sizing (115%)
- ✅ `Control<Derived>` - Templatable control base (95%)
- ✅ `ContentControl<Derived>` - Single-content host (95%)
- ✅ `Panel<Derived>` - Multi-child container base (95%)
- 🔧 `ItemsControl<Derived>` - Items collection control (85% - needs container generation)

**Layout Containers (2)**:
- ✅ `StackPanel` - Sequential layout (horizontal/vertical) (100%)
- ✅ `Grid` - Grid-based layout with rows/columns (95%)

**Controls (5)**:
- ✅ `Button` - Clickable button
- ✅ `TextBlock` - Text display (100%)
- ✅ `Border` - Border and background decorator (120%)
- ✅ `Image` - Bitmap image display (100%)
- ✅ `Window` - Top-level window

**Shapes (3)**:
- ⚠️ `Shape` - Vector shape base (stub only, 0%)
- ❌ `Rectangle` - Rectangle shape (not implemented)
- ❌ `Ellipse` - Ellipse/circle shape (not implemented)

**Transforms (7)**:
- ✅ `Transform` - Transform base (100%)
- ✅ `TranslateTransform` - Translation (100%)
- ✅ `ScaleTransform` - Scaling (100%)
- ✅ `RotateTransform` - Rotation (100%)
- ✅ `SkewTransform` - Skewing (100%)
- ✅ `MatrixTransform` - Matrix transformation (100%)
- ✅ `TransformGroup` - Composite transforms (100%)

**Styles and Templates (7)**:
- ⚠️ `Style` - Style definition (0% - not implemented)
- ✅ `Setter` - Property setter
- 🔧 `SetterCollection` - Collection of setters (basic impl)
- ⚠️ `FrameworkTemplate` - Template base (stub only)
- ⚠️ `ControlTemplate` - Control visual template (stub only)
- ⚠️ `DataTemplate` - Data presentation template (stub only)
- ✅ `ResourceDictionary` - Resource storage (100%)

**Input Management (3)**:
- ✅ `InputManager` - Input event routing and hit testing (98%)
- ✅ `FocusManager` - Keyboard focus management (98%)
- ✅ `RoutedEvent` - Routed event system

**Collections (2)**:
- ✅ `VisualCollection` - Visual children collection (105%)
- ✅ `ObservableCollection<T>` - Observable collection with notifications

**Rendering (1)**:
- ✅ `DrawCommand` - Drawing command structure (110%)

**Primitives and Types (6)**:
- ✅ `Primitives` - Point, Size, Rect, Matrix3x2, Color
- ✅ `Thickness` - Four-sided thickness
- ✅ `CornerRadius` - Corner radius specification
- ✅ `Enums` - Visibility, Orientation, Alignment enums
- ✅ `TextEnums` - Text-specific enums
- ✅ `Alignment` - Alignment enumerations

**Responsibilities**:
- Visual tree construction and management
- Layout (measure and arrange)
- Input event handling and routing
- Rendering via DrawCommands
- Control templating and styling
- Focus and keyboard navigation

**Key Dependencies**: Binding (DependencyObject, properties), Core (Event, Dispatcher), Render (DrawCommand)

---

## Key Design Patterns

### 1. Dependency Property System

**Pattern**: Attached Behavior with Metadata

**Implementation**:
```
DependencyObject
    └── PropertyStore (maps DependencyProperty -> value)
    └── BindingContext (maps DependencyProperty -> BindingExpression)

DependencyProperty
    └── PropertyMetadata (default value, callbacks, coercion)
```

**Flow**:
1. Property registered with metadata
2. Values stored in PropertyStore
3. Changes trigger callbacks and events
4. Bindings automatically update on changes

### 2. Visual Tree / Logical Tree

**Pattern**: Composite with dual hierarchies

**Visual Tree**: Rendering and hit-testing
```
Visual
    └── children_: vector<Visual*>
    └── parent_: Visual*
```

**Logical Tree**: DataContext inheritance and element lookup
```
DependencyObject
    └── logicalChildren_: vector<DependencyObject*>
    └── logicalParent_: DependencyObject*
```

### 3. Data Binding

**Pattern**: Observer with bidirectional propagation

**Components**:
```
Source (ViewModel)
    └── INotifyPropertyChanged
    
Binding Configuration
    └── Source, Path, Mode, Converter

BindingExpression (Active)
    └── Subscribes to source changes
    └── Updates target property
    └── Updates source on TwoWay
```

**Flow**:
1. Binding created with source/path
2. BindingExpression resolves source object
3. Subscribes to PropertyChanged
4. Updates flow based on BindingMode

### 4. CRTP for Type-Safe Derivation

**Pattern**: Curiously Recurring Template Pattern

**Example**:
```cpp
template<typename Derived>
class FrameworkElement : public UIElement {
    Derived* This() { return static_cast<Derived*>(this); }
};

class Button : public ContentControl<Button> {
    // Inherits type-safe methods via CRTP
};
```

**Benefits**:
- Compile-time polymorphism
- No vtable overhead
- Type-safe method chaining

### 5. Template System

**Pattern**: Strategy with delayed instantiation

```
Control
    └── Template: ControlTemplate

ControlTemplate
    └── VisualTreeFactory: function

Apply Template:
    1. Call VisualTreeFactory
    2. Set as visual child
    3. Apply TemplateBindings
```

### 6. Routed Events

**Pattern**: Event bubbling and tunneling

**Flow**:
```
Target Element (Direct)
    ↓ Tunnel (PreviewXxx)
Root → Parent → ... → Target
    ↑ Bubble (Xxx)
Target → ... → Parent → Root
```

### 7. Layout System

**Pattern**: Two-pass constraint-based layout

**Measure Pass**: Calculate desired sizes
```
Parent.Measure(availableSize)
    → Child.Measure(childConstraint)
        → Child.DesiredSize
```

**Arrange Pass**: Position elements
```
Parent.Arrange(finalRect)
    → Child.Arrange(childRect)
        → Child.RenderSize
```

## Data Flow Diagrams

### Binding Update Flow

```
ViewModel Property Changes
    ↓
INotifyPropertyChanged.PropertyChanged
    ↓
BindingExpression receives notification
    ↓
BindingExpression.UpdateTarget()
    ↓
DependencyObject.SetValue() [Binding source]
    ↓
PropertyStore updates value
    ↓
DependencyObject.PropertyChanged event
    ↓
UIElement.InvalidateVisual()
    ↓
Re-render
```

### Input Event Flow

```
Platform Input (Mouse/Keyboard)
    ↓
RenderBackend captures event
    ↓
InputManager.ProcessInput()
    ↓
Hit Testing (with transforms)
    ↓
Find target UIElement
    ↓
Raise Routed Event (Preview + Bubble)
    ↓
Element event handlers
    ↓
Update visual state if needed
```

### Render Pipeline

```
Visual Tree
    ↓
Layout Pass (Measure + Arrange)
    ↓
RenderTreeBuilder traversal
    ↓
Generate DrawCommands
    ↓
RenderCommandBuffer
    ↓
GlRenderer processes commands
    ↓
OpenGL draw calls
    ↓
Frame buffer → Screen
```

## Threading Model

### UI Thread Affinity

All UI objects are single-threaded and must be accessed from the UI thread only.

```
Background Thread
    ↓
Dispatcher.InvokeAsync(action)
    ↓
Message Queue
    ↓
UI Thread processes action
    ↓
Safe to access UI objects
```

### Dispatcher Operations

```cpp
// From background thread
std::thread([dispatcher, element]() {
    // Do work...
    
    dispatcher->InvokeAsync([element]() {
        // Update UI (now on UI thread)
        element->SetValue(Property, value);
    });
}).detach();
```

## Memory Management Strategy

### Ownership Rules

1. **Visual Tree**: Parent doesn't own children (weak references)
2. **Logical Tree**: Parent doesn't own children (weak references)
3. **Shared Ownership**: Public API uses `shared_ptr`
4. **Bindings**: BindingContext owns BindingExpressions
5. **Event Connections**: Connection objects manage lifetime

### Lifecycle

```
Application owns Windows
Windows hold shared_ptr to root content
Content holds shared_ptr to children
Bindings keep source alive while active
Event connections auto-disconnect on destruction
```

## Performance Characteristics

### Property Access
- Get: O(1) hash lookup
- Set: O(1) + notification overhead
- Binding: Additional source resolution

### Layout
- Measure: O(n) where n = element count
- Arrange: O(n)
- Invalidation: O(1) flag set, deferred recompute

### Rendering
- Build render tree: O(n)
- Draw commands: O(n)
- OpenGL calls: Batched where possible

### Memory Footprint
- DependencyObject: ~200 bytes
- UIElement: +~100 bytes
- Control: +~50 bytes
- Per binding: ~100 bytes

## Extension Points

### Custom Controls
1. Inherit from `Control<MyControl>`
2. Override `OnRender()` for custom drawing
3. Override layout methods if needed
4. Define dependency properties

### Custom Panels
1. Inherit from `Panel<MyPanel>`
2. Override `MeasureOverride()`
3. Override `ArrangeOverride()`

### Custom Binding Converters
1. Implement `IValueConverter`
2. Implement `Convert()` and `ConvertBack()`

### Custom Validation
1. Inherit from `ValidationRule`
2. Override `Validate()`

## Future Architecture Enhancements

1. **Animation System**: Timeline-based property animations
2. **Composition Layer**: Off-thread rendering and animations
3. **Resource Management**: Texture atlases, shader compilation
4. **Accessibility**: Screen reader support, keyboard navigation
5. **Touch Input**: Gesture recognition, multi-touch
6. **3D Support**: 3D transforms and camera
7. **Hardware Acceleration**: GPU-accelerated effects

## See Also

- [Getting Started Guide](GettingStarted.md)
- [Development Guide](Development.md)
- [API Reference](API/README.md)
- [Implementation Status](Implementation-Status.md)
