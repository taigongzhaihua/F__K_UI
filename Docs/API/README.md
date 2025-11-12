# F__K_UI API Reference

Complete API reference for all classes in the F__K_UI framework.

## Module Index

### [App Module](App/) - Application and Window Management
- [Application](App/Application.md) - Application singleton and lifecycle

### [Binding Module](Binding/) - Data Binding and Dependency Properties
Core binding system classes:
- [DependencyObject](Binding/DependencyObject.md) - Base class with dependency property support
- [DependencyProperty](Binding/DependencyProperty.md) - Property metadata and registration
- [Binding](Binding/Binding.md) - Data binding configuration
- [BindingExpression](Binding/BindingExpression.md) - Active binding instance
- [BindingContext](Binding/BindingContext.md) - Binding context management
- [BindingPath](Binding/BindingPath.md) - Property path parsing

Advanced binding:
- [MultiBinding](Binding/MultiBinding.md) - Multi-source binding
- [MultiBindingExpression](Binding/MultiBindingExpression.md) - Multi-binding instance
- [TemplateBinding](Binding/TemplateBinding.md) - Template property binding

Value converters:
- [IValueConverter](Binding/IValueConverter.md) - Value converter interface
- [IMultiValueConverter](Binding/IMultiValueConverter.md) - Multi-value converter interface
- [ValueConverters](Binding/ValueConverters.md) - Built-in converters

Validation:
- [ValidationRule](Binding/ValidationRule.md) - Validation rule base
- [INotifyDataErrorInfo](Binding/INotifyDataErrorInfo.md) - Error notification interface

Property change notification:
- [INotifyPropertyChanged](Binding/INotifyPropertyChanged.md) - Property change interface
- [ObservableObject](Binding/ObservableObject.md) - ViewModel base class

Utilities:
- [PropertyStore](Binding/PropertyStore.md) - Property value storage
- [PropertyAccessors](Binding/PropertyAccessors.md) - Property access helpers

### [Core Module](Core/) - Core Infrastructure
- [Dispatcher](Core/Dispatcher.md) - Thread-safe message dispatching
- [Event](Core/Event.md) - Type-safe event system
- [Clock](Core/Clock.md) - Time and frame tracking
- [Timer](Core/Timer.md) - Timer implementation
- [Logger](Core/Logger.md) - Logging infrastructure

### [Render Module](Render/) - Rendering System
- [Renderer](Render/Renderer.md) - Main renderer
- [IRenderer](Render/IRenderer.md) - Renderer interface
- [GlRenderer](Render/GlRenderer.md) - OpenGL renderer implementation
- [RenderBackend](Render/RenderBackend.md) - Platform abstraction
- [RenderContext](Render/RenderContext.md) - Rendering context
- [RenderHost](Render/RenderHost.md) - Render host management
- [RenderScene](Render/RenderScene.md) - Scene management
- [RenderTreeBuilder](Render/RenderTreeBuilder.md) - Render tree construction
- [RenderCommandBuffer](Render/RenderCommandBuffer.md) - Command buffering
- [RenderList](Render/RenderList.md) - Render list management
- [TextRenderer](Render/TextRenderer.md) - Text rendering (FreeType)
- [ColorUtils](Render/ColorUtils.md) - Color utilities

### [UI Module](UI/) - User Interface Elements

#### Base Classes
- [Visual](UI/Visual.md) - Visual tree node base
- [UIElement](UI/UIElement.md) - Interactive element base
- [FrameworkElement](UI/FrameworkElement.md) - Layout-aware element base
- [Control](UI/Control.md) - Templatable control base
- [ContentControl](UI/ContentControl.md) - Single-content control base

#### Layout Containers
- [Panel](UI/Panel.md) - Multi-child container base
- [StackPanel](UI/StackPanel.md) - Sequential layout
- [Grid](UI/Grid.md) - Grid-based layout
- [VisualCollection](UI/VisualCollection.md) - Visual children collection

#### Controls
- [Button](UI/Button.md) - Button control
- [TextBlock](UI/TextBlock.md) - Text display
- [Border](UI/Border.md) - Border and background decorator
- [Image](UI/Image.md) - Image display
- [ItemsControl](UI/ItemsControl.md) - Items collection control
- [Window](UI/Window.md) - Top-level window

#### Shapes
- [Shape](UI/Shape.md) - Shape base class
- [Rectangle](UI/Rectangle.md) - Rectangle shape
- [Ellipse](UI/Ellipse.md) - Ellipse/circle shape

#### Styles and Templates
- [Style](UI/Style.md) - Style definition
- [Setter](UI/Setter.md) - Property setter
- [FrameworkTemplate](UI/FrameworkTemplate.md) - Template base
- [ControlTemplate](UI/ControlTemplate.md) - Control template
- [DataTemplate](UI/DataTemplate.md) - Data template
- [ResourceDictionary](UI/ResourceDictionary.md) - Resource storage

#### Transforms
- [Transform](UI/Transform.md) - Transform base class
- [TranslateTransform](UI/TranslateTransform.md) - Translation
- [ScaleTransform](UI/ScaleTransform.md) - Scaling
- [RotateTransform](UI/RotateTransform.md) - Rotation
- [SkewTransform](UI/SkewTransform.md) - Skewing
- [MatrixTransform](UI/MatrixTransform.md) - Matrix transformation
- [TransformGroup](UI/TransformGroup.md) - Transform composition

#### Input Management
- [InputManager](UI/InputManager.md) - Input event routing
- [FocusManager](UI/FocusManager.md) - Focus management

#### Collections
- [ObservableCollection](UI/ObservableCollection.md) - Observable collection

#### Rendering
- [DrawCommand](UI/DrawCommand.md) - Drawing commands

#### Primitives and Enums
- [Primitives](UI/Primitives.md) - Basic types (Point, Size, Rect, etc.)
- [Enums](UI/Enums.md) - Common enumerations
- [TextEnums](UI/TextEnums.md) - Text-related enumerations
- [Alignment](UI/Alignment.md) - Alignment enumerations
- [Thickness](UI/Thickness.md) - Thickness structure
- [CornerRadius](UI/CornerRadius.md) - Corner radius structure

#### Routed Events
- [RoutedEvent](UI/RoutedEvent.md) - Routed event system

## Documentation Conventions

Each class documentation includes:

1. **Overview** - Class purpose and responsibilities
2. **Inheritance** - Base classes and derived classes
3. **Properties** - Dependency properties and regular properties
4. **Methods** - Public methods with parameters and return values
5. **Events** - Available events
6. **Usage Examples** - Code examples demonstrating usage
7. **Related Classes** - Links to related documentation

## Quick Links

- [Getting Started Guide](../GettingStarted.md)
- [Development Guide](../Development.md)
- [Architecture Documentation](../Designs/)
- [Implementation Status](../Implementation-Status.md)
