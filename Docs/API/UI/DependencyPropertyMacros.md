# DependencyPropertyMacros - 依赖属性宏

## 概述

`DependencyPropertyMacros.h` 提供了一套宏来简化依赖属性的声明和实现,大幅减少样板代码。

**命名空间**: `fk::ui`  
**头文件**: `fk/ui/DependencyPropertyMacros.h`

## 核心功能

- **声明宏**: 在头文件中快速声明依赖属性
- **实现宏**: 在源文件中实现依赖属性
- **组合宏**: 一次性完成所有实现
- **类型支持**: 值类型和引用类型

## 宏列表

### 头文件声明宏

#### FK_DEPENDENCY_PROPERTY_DECLARE
```cpp
FK_DEPENDENCY_PROPERTY_DECLARE(PropertyName, ValueType)
```
声明值类型依赖属性(int、float、bool、enum 等)。

**生成的代码**:
```cpp
public:
    static const binding::DependencyProperty& PropertyNameProperty();
    void SetPropertyName(ValueType value);
    [[nodiscard]] ValueType GetPropertyName() const;

protected:
    virtual void OnPropertyNameChanged(ValueType oldValue, ValueType newValue);

private:
    static binding::PropertyMetadata BuildPropertyNameMetadata();
    static void PropertyNamePropertyChanged(/*...*/);
```

#### FK_DEPENDENCY_PROPERTY_DECLARE_REF
```cpp
FK_DEPENDENCY_PROPERTY_DECLARE_REF(PropertyName, ValueType)
```
声明引用类型依赖属性(std::string 等)。

**区别**: Getter 返回 `const ValueType&`,OnChanged 接受 `const ValueType&` 参数。

### 实现文件宏

#### FK_DEPENDENCY_PROPERTY_REGISTER
```cpp
FK_DEPENDENCY_PROPERTY_REGISTER(OwnerClass, PropertyName, ValueType)
```
实现 Property() 静态注册方法。

#### FK_DEPENDENCY_PROPERTY_IMPL
```cpp
FK_DEPENDENCY_PROPERTY_IMPL(OwnerClass, PropertyName, ValueType, DefaultValue)
```
实现 Getter/Setter 方法(值类型)。

#### FK_DEPENDENCY_PROPERTY_IMPL_REF
```cpp
FK_DEPENDENCY_PROPERTY_IMPL_REF(OwnerClass, PropertyName, ValueType)
```
实现 Getter/Setter 方法(引用类型)。

#### FK_DEPENDENCY_PROPERTY_CALLBACK
```cpp
FK_DEPENDENCY_PROPERTY_CALLBACK(OwnerClass, PropertyName, ValueType, DefaultValue)
```
实现属性改变回调函数(值类型)。

#### FK_DEPENDENCY_PROPERTY_CALLBACK_REF
```cpp
FK_DEPENDENCY_PROPERTY_CALLBACK_REF(OwnerClass, PropertyName, ValueType)
```
实现属性改变回调函数(引用类型)。

### 组合宏

#### FK_DEPENDENCY_PROPERTY_DEFINE
```cpp
FK_DEPENDENCY_PROPERTY_DEFINE(OwnerClass, PropertyName, ValueType, DefaultValue)
```
一次性实现值类型依赖属性(Register + Impl + Callback)。

#### FK_DEPENDENCY_PROPERTY_DEFINE_REF
```cpp
FK_DEPENDENCY_PROPERTY_DEFINE_REF(OwnerClass, PropertyName, ValueType)
```
一次性实现引用类型依赖属性。

## 使用示例

### 1. 基本值类型属性

**头文件 (MyControl.h)**:
```cpp
#include <fk/ui/Control.h>
#include <fk/ui/DependencyPropertyMacros.h>

class MyControl : public fk::ui::Control {
public:
    // 声明依赖属性
    FK_DEPENDENCY_PROPERTY_DECLARE(CornerRadius, float)
    FK_DEPENDENCY_PROPERTY_DECLARE(IsEnabled, bool)
};
```

**源文件 (MyControl.cpp)**:
```cpp
#include "MyControl.h"

// 一次性实现
FK_DEPENDENCY_PROPERTY_DEFINE(MyControl, CornerRadius, float, 0.0f)
FK_DEPENDENCY_PROPERTY_DEFINE(MyControl, IsEnabled, bool, true)

// 实现元数据
fk::binding::PropertyMetadata MyControl::BuildCornerRadiusMetadata() {
    fk::binding::PropertyMetadata metadata(4.0f);  // 默认值 4.0
    metadata.propertyChangedCallback = CornerRadiusPropertyChanged;
    return metadata;
}

// 实现改变回调
void MyControl::OnCornerRadiusChanged(float oldValue, float newValue) {
    std::cout << "CornerRadius changed: " << oldValue << " -> " << newValue << "\n";
    InvalidateVisual();  // 触发重绘
}

// IsEnabled 的元数据和回调实现类似...
```

### 2. 引用类型属性

**头文件**:
```cpp
class MyControl : public fk::ui::Control {
public:
    FK_DEPENDENCY_PROPERTY_DECLARE_REF(Background, std::string)
    FK_DEPENDENCY_PROPERTY_DECLARE_REF(Text, std::string)
};
```

**源文件**:
```cpp
FK_DEPENDENCY_PROPERTY_DEFINE_REF(MyControl, Background, std::string)
FK_DEPENDENCY_PROPERTY_DEFINE_REF(MyControl, Text, std::string)

fk::binding::PropertyMetadata MyControl::BuildBackgroundMetadata() {
    fk::binding::PropertyMetadata metadata(std::string("#FFFFFF"));
    metadata.propertyChangedCallback = BackgroundPropertyChanged;
    return metadata;
}

void MyControl::OnBackgroundChanged(const std::string& oldValue, 
                                    const std::string& newValue) {
    std::cout << "Background: " << oldValue << " -> " << newValue << "\n";
    InvalidateVisual();
}
```

### 3. 手动分步实现

如果需要更多控制,可以使用独立的宏:

```cpp
// 注册
FK_DEPENDENCY_PROPERTY_REGISTER(MyControl, Opacity, float)

// 实现 Getter/Setter
FK_DEPENDENCY_PROPERTY_IMPL(MyControl, Opacity, float, 1.0f)

// 实现回调
FK_DEPENDENCY_PROPERTY_CALLBACK(MyControl, Opacity, float, 1.0f)

// 手动实现元数据
fk::binding::PropertyMetadata MyControl::BuildOpacityMetadata() {
    fk::binding::PropertyMetadata metadata(1.0f);
    metadata.propertyChangedCallback = OpacityPropertyChanged;
    
    // 添加验证
    metadata.validateValueCallback = [](const std::any& value) {
        float opacity = std::any_cast<float>(value);
        return opacity >= 0.0f && opacity <= 1.0f;
    };
    
    // 添加强制转换
    metadata.coerceValueCallback = [](fk::binding::DependencyObject&, 
                                       const std::any& value) {
        float opacity = std::any_cast<float>(value);
        return std::clamp(opacity, 0.0f, 1.0f);
    };
    
    return metadata;
}

// 手动实现改变回调
void MyControl::OnOpacityChanged(float oldValue, float newValue) {
    // 自定义逻辑
    if (newValue < 0.1f) {
        SetVisibility(fk::ui::Visibility::Collapsed);
    }
    InvalidateVisual();
}
```

### 4. 枚举类型属性

```cpp
enum class ClickMode {
    Release,
    Press,
    Hover
};

class MyButton : public fk::ui::Button {
public:
    FK_DEPENDENCY_PROPERTY_DECLARE(ClickMode, ClickMode)
};

// 实现
FK_DEPENDENCY_PROPERTY_DEFINE(MyButton, ClickMode, ClickMode, ClickMode::Release)

fk::binding::PropertyMetadata MyButton::BuildClickModeMetadata() {
    fk::binding::PropertyMetadata metadata(ClickMode::Release);
    metadata.propertyChangedCallback = ClickModePropertyChanged;
    return metadata;
}

void MyButton::OnClickModeChanged(ClickMode oldValue, ClickMode newValue) {
    // 处理点击模式改变
}
```

### 5. 完整示例 - 自定义按钮

**MyButton.h**:
```cpp
#pragma once
#include <fk/ui/Button.h>
#include <fk/ui/DependencyPropertyMacros.h>

class MyButton : public fk::ui::ButtonBase {
public:
    MyButton();
    
    // 依赖属性声明
    FK_DEPENDENCY_PROPERTY_DECLARE(CornerRadius, float)
    FK_DEPENDENCY_PROPERTY_DECLARE_REF(Background, std::string)
    FK_DEPENDENCY_PROPERTY_DECLARE_REF(Foreground, std::string)
    FK_DEPENDENCY_PROPERTY_DECLARE(IsPressed, bool)
};
```

**MyButton.cpp**:
```cpp
#include "MyButton.h"

MyButton::MyButton() {
    // 构造函数初始化
}

// 实现所有依赖属性
FK_DEPENDENCY_PROPERTY_DEFINE(MyButton, CornerRadius, float, 0.0f)
FK_DEPENDENCY_PROPERTY_DEFINE_REF(MyButton, Background, std::string)
FK_DEPENDENCY_PROPERTY_DEFINE_REF(MyButton, Foreground, std::string)
FK_DEPENDENCY_PROPERTY_DEFINE(MyButton, IsPressed, bool, false)

// CornerRadius 元数据
fk::binding::PropertyMetadata MyButton::BuildCornerRadiusMetadata() {
    fk::binding::PropertyMetadata metadata(4.0f);
    metadata.propertyChangedCallback = CornerRadiusPropertyChanged;
    return metadata;
}

void MyButton::OnCornerRadiusChanged(float oldValue, float newValue) {
    InvalidateVisual();
}

// Background 元数据
fk::binding::PropertyMetadata MyButton::BuildBackgroundMetadata() {
    fk::binding::PropertyMetadata metadata(std::string("#CCCCCC"));
    metadata.propertyChangedCallback = BackgroundPropertyChanged;
    return metadata;
}

void MyButton::OnBackgroundChanged(const std::string& oldValue, 
                                   const std::string& newValue) {
    InvalidateVisual();
}

// Foreground 元数据
fk::binding::PropertyMetadata MyButton::BuildForegroundMetadata() {
    fk::binding::PropertyMetadata metadata(std::string("#000000"));
    metadata.propertyChangedCallback = ForegroundPropertyChanged;
    return metadata;
}

void MyButton::OnForegroundChanged(const std::string& oldValue,
                                   const std::string& newValue) {
    InvalidateVisual();
}

// IsPressed 元数据
fk::binding::PropertyMetadata MyButton::BuildIsPressedMetadata() {
    fk::binding::PropertyMetadata metadata(false);
    metadata.propertyChangedCallback = IsPressedPropertyChanged;
    return metadata;
}

void MyButton::OnIsPressedChanged(bool oldValue, bool newValue) {
    if (newValue) {
        SetBackground("#AAAAAA");  // 按下时颜色变深
    } else {
        SetBackground("#CCCCCC");  // 恢复原色
    }
}
```

## 最佳实践

### 1. 选择正确的宏

```cpp
// 值类型:使用 DECLARE 和 DEFINE
FK_DEPENDENCY_PROPERTY_DECLARE(Width, float)
FK_DEPENDENCY_PROPERTY_DEFINE(MyControl, Width, float, 0.0f)

// 引用类型:使用 DECLARE_REF 和 DEFINE_REF
FK_DEPENDENCY_PROPERTY_DECLARE_REF(Text, std::string)
FK_DEPENDENCY_PROPERTY_DEFINE_REF(MyControl, Text, std::string)
```

### 2. 始终实现元数据

```cpp
// 推荐:提供完整的元数据
fk::binding::PropertyMetadata MyControl::BuildWidthMetadata() {
    fk::binding::PropertyMetadata metadata(100.0f);  // 默认值
    metadata.propertyChangedCallback = WidthPropertyChanged;
    
    // 可选:添加验证
    metadata.validateValueCallback = [](const std::any& value) {
        return std::any_cast<float>(value) >= 0.0f;
    };
    
    return metadata;
}

// 避免:返回空元数据
```

### 3. 实现 OnChanged 回调

```cpp
// 推荐:在 OnChanged 中处理副作用
void MyControl::OnWidthChanged(float oldValue, float newValue) {
    InvalidateVisual();        // 触发重绘
    InvalidateMeasure();       // 触发重新测量
    
    // 触发事件
    SizeChanged.Raise(*this, oldValue, newValue);
}

// 避免:空实现
void MyControl::OnWidthChanged(float oldValue, float newValue) {
    // 什么都不做
}
```

### 4. 使用组合宏简化代码

```cpp
// 推荐:使用组合宏
FK_DEPENDENCY_PROPERTY_DEFINE(MyControl, Width, float, 0.0f)

// 避免:手动写所有宏(除非需要定制)
FK_DEPENDENCY_PROPERTY_REGISTER(MyControl, Width, float)
FK_DEPENDENCY_PROPERTY_IMPL(MyControl, Width, float, 0.0f)
FK_DEPENDENCY_PROPERTY_CALLBACK(MyControl, Width, float, 0.0f)
```

## 常见问题

### Q1: 值类型和引用类型如何选择?

```cpp
// 值类型:小型 POD 类型
FK_DEPENDENCY_PROPERTY_DECLARE(Width, float)
FK_DEPENDENCY_PROPERTY_DECLARE(IsEnabled, bool)
FK_DEPENDENCY_PROPERTY_DECLARE(ClickMode, ClickMode)  // enum

// 引用类型:大型类型或需要避免复制
FK_DEPENDENCY_PROPERTY_DECLARE_REF(Text, std::string)
FK_DEPENDENCY_PROPERTY_DECLARE_REF(Items, std::vector<int>)
```

### Q2: 如何添加属性验证?

```cpp
fk::binding::PropertyMetadata MyControl::BuildOpacityMetadata() {
    fk::binding::PropertyMetadata metadata(1.0f);
    metadata.propertyChangedCallback = OpacityPropertyChanged;
    
    // 添加验证
    metadata.validateValueCallback = [](const std::any& value) {
        float opacity = std::any_cast<float>(value);
        return opacity >= 0.0f && opacity <= 1.0f;  // 只接受 [0, 1]
    };
    
    return metadata;
}
```

### Q3: 如何实现属性继承?

```cpp
class BaseControl : public fk::ui::Control {
public:
    FK_DEPENDENCY_PROPERTY_DECLARE(FontSize, float)
};

class DerivedControl : public BaseControl {
    // 自动继承 FontSize 属性
    // 可以重写 OnFontSizeChanged
protected:
    void OnFontSizeChanged(float oldValue, float newValue) override {
        BaseControl::OnFontSizeChanged(oldValue, newValue);
        // 添加自定义逻辑
    }
};
```

### Q4: 宏展开后的代码是什么?

```cpp
// 宏:
FK_DEPENDENCY_PROPERTY_DECLARE(Width, float)

// 展开为:
public:
    static const binding::DependencyProperty& WidthProperty();
    void SetWidth(float value);
    [[nodiscard]] float GetWidth() const;
protected:
    virtual void OnWidthChanged(float oldValue, float newValue);
private:
    static binding::PropertyMetadata BuildWidthMetadata();
    static void WidthPropertyChanged(/*...*/);
```

## 性能考虑

### 1. 默认值
```cpp
// 推荐:使用合理的默认值
FK_DEPENDENCY_PROPERTY_IMPL(MyControl, Width, float, 100.0f)

// 避免:总是使用 0
FK_DEPENDENCY_PROPERTY_IMPL(MyControl, Width, float, 0.0f)
```

### 2. OnChanged 性能
```cpp
// 推荐:仅在必要时 InvalidateVisual
void MyControl::OnWidthChanged(float oldValue, float newValue) {
    if (std::abs(newValue - oldValue) > 0.01f) {
        InvalidateVisual();
    }
}

// 避免:总是重绘
void MyControl::OnWidthChanged(float oldValue, float newValue) {
    InvalidateVisual();  // 即使变化很小也重绘
}
```

## 相关文档

- [DependencyProperty.md](../Binding/DependencyProperty.md) - 依赖属性系统
- [BindingMacros.md](BindingMacros.md) - 绑定宏
- [DependencyObject.md](../Binding/DependencyObject.md) - 依赖对象基类
