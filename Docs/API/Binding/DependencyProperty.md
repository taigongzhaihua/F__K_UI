# DependencyProperty

## 概览

**目的**：定义依赖属性的元数据和行为

**命名空间**：`fk::binding`

**头文件**：`fk/binding/DependencyProperty.h`

## 描述

`DependencyProperty` 定义依赖属性的元数据，包括默认值、变更回调、强制回调等。它是依赖属性系统的核心。

## 公共接口

### 注册

#### Register
```cpp
template<typename TOwner, typename TValue>
static DependencyProperty Register(
    const std::string& name,
    const PropertyMetadata& metadata
);
```

注册一个新的依赖属性。

**示例**：
```cpp
class MyElement : public UIElement {
public:
    static const DependencyProperty& MyPropertyProperty() {
        static auto prop = DependencyProperty::Register<MyElement, int>(
            "MyProperty",
            PropertyMetadata::Create(
                0,  // 默认值
                [](DependencyObject* d, const DependencyPropertyChangedEventArgs& e) {
                    // 变更回调
                    auto element = static_cast<MyElement*>(d);
                    element->OnMyPropertyChanged(e);
                }
            )
        );
        return prop;
    }
};
```

### 属性信息

#### Name / OwnerType / PropertyType
```cpp
const std::string& Name() const;
const std::type_info& OwnerType() const;
const std::type_info& PropertyType() const;
```

获取属性的名称和类型信息。

### 元数据

#### Metadata
```cpp
const PropertyMetadata& Metadata() const;
```

获取属性元数据。

## PropertyMetadata

### 创建元数据

```cpp
static PropertyMetadata Create(
    const std::any& defaultValue,
    PropertyChangedCallback changedCallback = nullptr,
    CoerceValueCallback coerceCallback = nullptr
);
```

**参数**：
- `defaultValue` - 属性的默认值
- `changedCallback` - 属性变更时的回调
- `coerceCallback` - 值强制转换回调

### 元数据选项

```cpp
PropertyMetadata& Inherits(bool inherits);
PropertyMetadata& AffectsMeasure(bool affects);
PropertyMetadata& AffectsArrange(bool affects);
PropertyMetadata& AffectsRender(bool affects);
```

设置属性的特性：
- `Inherits` - 沿逻辑树继承
- `AffectsMeasure` - 影响测量
- `AffectsArrange` - 影响排列
- `AffectsRender` - 影响渲染

## 使用示例

### 基本属性注册
```cpp
static const DependencyProperty& WidthProperty() {
    static auto prop = DependencyProperty::Register<UIElement, double>(
        "Width",
        PropertyMetadata::Create(NAN)  // 默认为NaN
    );
    return prop;
}
```

### 带变更回调
```cpp
static const DependencyProperty& BackgroundProperty() {
    static auto prop = DependencyProperty::Register<Control, Color>(
        "Background",
        PropertyMetadata::Create(
            Colors::Transparent,
            [](DependencyObject* d, const DependencyPropertyChangedEventArgs& e) {
                auto control = static_cast<Control*>(d);
                control->InvalidateVisual();  // 触发重绘
            }
        )
    );
    return prop;
}
```

### 影响布局的属性
```cpp
static const DependencyProperty& PaddingProperty() {
    static auto prop = DependencyProperty::Register<FrameworkElement, Thickness>(
        "Padding",
        PropertyMetadata::Create(Thickness(0))
            .AffectsMeasure(true)  // 影响测量
    );
    return prop;
}
```

### 可继承属性
```cpp
static const DependencyProperty& FontSizeProperty() {
    static auto prop = DependencyProperty::Register<Control, double>(
        "FontSize",
        PropertyMetadata::Create(12.0)
            .Inherits(true)  // 沿逻辑树继承
    );
    return prop;
}
```

### 带强制回调
```cpp
static const DependencyProperty& OpacityProperty() {
    static auto prop = DependencyProperty::Register<UIElement, double>(
        "Opacity",
        PropertyMetadata::Create(
            1.0,
            nullptr,  // 无变更回调
            [](DependencyObject* d, const std::any& value) -> std::any {
                // 强制值在0-1之间
                double opacity = std::any_cast<double>(value);
                return std::clamp(opacity, 0.0, 1.0);
            }
        )
    );
    return prop;
}
```

## 相关类

- [DependencyObject](DependencyObject.md) - 使用依赖属性的基类
- [PropertyMetadata](#) - 属性元数据

## 另请参阅

- [设计文档](../../Design/Binding/DependencyProperty.md)
