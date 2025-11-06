# 依赖属性宏使用指南

## 概述

`DependencyPropertyMacros.h` 提供了一套宏来大幅简化依赖属性的声明和实现，减少样板代码。

## 对比：使用宏前 vs 使用宏后

### 传统方式（手动编写）

**头文件 (.h)**：
```cpp
class ButtonBase : public ControlBase {
public:
    // 依赖属性声明
    static const binding::DependencyProperty& CornerRadiusProperty();
    
    // Getter/Setter
    void SetCornerRadius(float radius);
    float GetCornerRadius() const;

protected:
    // 属性变更回调
    virtual void OnCornerRadiusChanged(float oldValue, float newValue);

private:
    // 元数据构建
    static binding::PropertyMetadata BuildCornerRadiusMetadata();
    
    // 静态回调
    static void CornerRadiusPropertyChanged(binding::DependencyObject& sender,
        const binding::DependencyProperty& property, 
        const std::any& oldValue, const std::any& newValue);
};
```

**实现文件 (.cpp)** - 需要 ~80 行代码：
```cpp
// 1. 属性注册（~7 行）
const binding::DependencyProperty& ButtonBase::CornerRadiusProperty() {
    static const auto& property = binding::DependencyProperty::Register(
        "CornerRadius",
        typeid(float),
        typeid(ButtonBase),
        BuildCornerRadiusMetadata()
    );
    return property;
}

// 2. Setter（~3 行）
void ButtonBase::SetCornerRadius(float radius) {
    SetValue(CornerRadiusProperty(), radius);
}

// 3. Getter（~10 行）
float ButtonBase::GetCornerRadius() const {
    const auto& value = GetValue(CornerRadiusProperty());
    if (!value.has_value()) return 0.0f;
    try {
        return std::any_cast<float>(value);
    } catch (...) {
        return 0.0f;
    }
}

// 4. 静态回调（~12 行）
void ButtonBase::CornerRadiusPropertyChanged(binding::DependencyObject& sender,
    const binding::DependencyProperty& property, 
    const std::any& oldValue, const std::any& newValue) {
    auto* button = dynamic_cast<ButtonBase*>(&sender);
    if (!button) return;

    try {
        float oldRadius = oldValue.has_value() ? std::any_cast<float>(oldValue) : 0.0f;
        float newRadius = std::any_cast<float>(newValue);
        button->OnCornerRadiusChanged(oldRadius, newRadius);
    } catch (...) {}
}

// 5. 元数据构建（~5 行）
binding::PropertyMetadata ButtonBase::BuildCornerRadiusMetadata() {
    binding::PropertyMetadata metadata(4.0f);
    metadata.propertyChangedCallback = CornerRadiusPropertyChanged;
    return metadata;
}

// 6. 属性变更回调（~3 行）
void ButtonBase::OnCornerRadiusChanged(float oldValue, float newValue) {
    InvalidateVisual();
}
```

**总计**：头文件 ~15 行 + 实现文件 ~40 行 = **~55 行代码/属性**

---

### 使用宏（推荐方式）

**头文件 (.h)** - 只需 1 行：
```cpp
#include "fk/ui/DependencyPropertyMacros.h"

class ButtonBase : public ControlBase {
public:
    // 一行声明所有必要的方法！
    FK_DEPENDENCY_PROPERTY_DECLARE(CornerRadius, float)
};
```

**实现文件 (.cpp)** - 只需 ~10 行：
```cpp
// 一行定义 Register + Getter/Setter + Callback
FK_DEPENDENCY_PROPERTY_DEFINE(ButtonBase, CornerRadius, float, 0.0f)

// 手动实现 Metadata（可自定义默认值、验证等）
binding::PropertyMetadata ButtonBase::BuildCornerRadiusMetadata() {
    binding::PropertyMetadata metadata(4.0f);  // 自定义默认值
    metadata.propertyChangedCallback = CornerRadiusPropertyChanged;
    metadata.validateCallback = ValidateCornerRadius;  // 可选：添加验证
    return metadata;
}

// 手动实现 OnChanged（业务逻辑）
void ButtonBase::OnCornerRadiusChanged(float oldValue, float newValue) {
    InvalidateVisual();
}
```

**总计**：头文件 1 行 + 实现文件 ~10 行 = **~11 行代码/属性**

**代码减少**：从 55 行 → 11 行，**减少 80%**！

---

## 宏选择指南

### 1. 类型判断

**值类型** → 使用普通版本：
- `float`, `int`, `bool`, `enum`
- 宏：`FK_DEPENDENCY_PROPERTY_DECLARE` + `FK_DEPENDENCY_PROPERTY_DEFINE`

**引用类型** → 使用 `_REF` 版本：
- `std::string`, `Brush`, 或其他需要返回 `const&` 的类型
- 宏：`FK_DEPENDENCY_PROPERTY_DECLARE_REF` + `FK_DEPENDENCY_PROPERTY_DEFINE_REF`

### 2. 决策树

```
你的属性是什么类型？
│
├─ 值类型 (float, int, bool, enum)
│   │
│   ├─ 头文件 .h:
│   │   FK_DEPENDENCY_PROPERTY_DECLARE(PropertyName, ValueType)
│   │
│   └─ 实现文件 .cpp:
│       FK_DEPENDENCY_PROPERTY_DEFINE(OwnerClass, PropertyName, ValueType, DefaultValue)
│       + 手动实现 BuildMetadata() 和 OnChanged()
│
└─ 引用类型 (std::string, Brush)
    │
    ├─ 头文件 .h:
    │   FK_DEPENDENCY_PROPERTY_DECLARE_REF(PropertyName, ValueType)
    │
    └─ 实现文件 .cpp:
        FK_DEPENDENCY_PROPERTY_DEFINE_REF(OwnerClass, PropertyName, ValueType)
        + 手动实现 BuildMetadata() 和 OnChanged()
```

---

## 完整示例

### 示例 1：值类型属性（float）

**Button.h**：
```cpp
#include "fk/ui/DependencyPropertyMacros.h"

class ButtonBase : public ControlBase {
public:
    FK_DEPENDENCY_PROPERTY_DECLARE(CornerRadius, float)
    FK_DEPENDENCY_PROPERTY_DECLARE(BorderThickness, float)
    FK_DEPENDENCY_PROPERTY_DECLARE(IsPressed, bool)
};
```

**Button.cpp**：
```cpp
// CornerRadius
FK_DEPENDENCY_PROPERTY_DEFINE(ButtonBase, CornerRadius, float, 0.0f)

binding::PropertyMetadata ButtonBase::BuildCornerRadiusMetadata() {
    binding::PropertyMetadata metadata(4.0f);  // Fluent 默认圆角
    metadata.propertyChangedCallback = CornerRadiusPropertyChanged;
    metadata.validateCallback = ValidateCornerRadius;
    return metadata;
}

void ButtonBase::OnCornerRadiusChanged(float oldValue, float newValue) {
    InvalidateVisual();
}

// BorderThickness
FK_DEPENDENCY_PROPERTY_DEFINE(ButtonBase, BorderThickness, float, 0.0f)

binding::PropertyMetadata ButtonBase::BuildBorderThicknessMetadata() {
    binding::PropertyMetadata metadata(1.0f);
    metadata.propertyChangedCallback = BorderThicknessPropertyChanged;
    return metadata;
}

void ButtonBase::OnBorderThicknessChanged(float oldValue, float newValue) {
    InvalidateVisual();
}

// IsPressed
FK_DEPENDENCY_PROPERTY_DEFINE(ButtonBase, IsPressed, bool, false)

binding::PropertyMetadata ButtonBase::BuildIsPressedMetadata() {
    binding::PropertyMetadata metadata(false);
    metadata.propertyChangedCallback = IsPressedPropertyChanged;
    return metadata;
}

void ButtonBase::OnIsPressedChanged(bool oldValue, bool newValue) {
    InvalidateVisual();
}
```

---

### 示例 2：引用类型属性（std::string）

**Button.h**：
```cpp
#include "fk/ui/DependencyPropertyMacros.h"

class ButtonBase : public ControlBase {
public:
    FK_DEPENDENCY_PROPERTY_DECLARE_REF(Background, std::string)
    FK_DEPENDENCY_PROPERTY_DECLARE_REF(Foreground, std::string)
};
```

**Button.cpp**：
```cpp
// Background
FK_DEPENDENCY_PROPERTY_DEFINE_REF(ButtonBase, Background, std::string)

binding::PropertyMetadata ButtonBase::BuildBackgroundMetadata() {
    binding::PropertyMetadata metadata(std::string("#0078D4"));  // Fluent Accent Blue
    metadata.propertyChangedCallback = BackgroundPropertyChanged;
    metadata.validateCallback = ValidateColor;
    return metadata;
}

void ButtonBase::OnBackgroundChanged(const std::string& oldValue, const std::string& newValue) {
    InvalidateVisual();
}

// Foreground
FK_DEPENDENCY_PROPERTY_DEFINE_REF(ButtonBase, Foreground, std::string)

binding::PropertyMetadata ButtonBase::BuildForegroundMetadata() {
    binding::PropertyMetadata metadata(std::string("#FFFFFF"));
    metadata.propertyChangedCallback = ForegroundPropertyChanged;
    return metadata;
}

void ButtonBase::OnForegroundChanged(const std::string& oldValue, const std::string& newValue) {
    InvalidateVisual();
}
```

---

## 宏展开详解

### FK_DEPENDENCY_PROPERTY_DECLARE

**输入**：
```cpp
FK_DEPENDENCY_PROPERTY_DECLARE(CornerRadius, float)
```

**展开为**：
```cpp
public:
    static const binding::DependencyProperty& CornerRadiusProperty();
    void SetCornerRadius(float value);
    [[nodiscard]] float GetCornerRadius() const;

protected:
    virtual void OnCornerRadiusChanged(float oldValue, float newValue);

private:
    static binding::PropertyMetadata BuildCornerRadiusMetadata();
    static void CornerRadiusPropertyChanged(binding::DependencyObject& sender,
        const binding::DependencyProperty& property, const std::any& oldValue, const std::any& newValue);
```

---

### FK_DEPENDENCY_PROPERTY_DEFINE

**输入**：
```cpp
FK_DEPENDENCY_PROPERTY_DEFINE(ButtonBase, CornerRadius, float, 0.0f)
```

**展开为** 3 个部分（~30 行代码）：

1. **Register**：
```cpp
const binding::DependencyProperty& ButtonBase::CornerRadiusProperty() {
    static const auto& property = binding::DependencyProperty::Register(
        "CornerRadius",
        typeid(float),
        typeid(ButtonBase),
        BuildCornerRadiusMetadata()
    );
    return property;
}
```

2. **Getter/Setter**：
```cpp
void ButtonBase::SetCornerRadius(float value) {
    SetValue(CornerRadiusProperty(), value);
}

float ButtonBase::GetCornerRadius() const {
    const auto& value = GetValue(CornerRadiusProperty());
    if (!value.has_value()) return 0.0f;
    try {
        return std::any_cast<float>(value);
    } catch (...) {
        return 0.0f;
    }
}
```

3. **Callback**：
```cpp
void ButtonBase::CornerRadiusPropertyChanged(binding::DependencyObject& sender,
    const binding::DependencyProperty& property, const std::any& oldValue, const std::any& newValue) {
    auto* obj = dynamic_cast<ButtonBase*>(&sender);
    if (!obj) return;

    try {
        float oldVal = oldValue.has_value() ? std::any_cast<float>(oldValue) : 0.0f;
        float newVal = std::any_cast<float>(newValue);
        obj->OnCornerRadiusChanged(oldVal, newVal);
    } catch (...) {}
}
```

---

## 高级用法

### 1. 分步使用宏（更灵活）

如果你需要更精细的控制，可以分别使用单独的宏：

```cpp
// 只生成 Register
FK_DEPENDENCY_PROPERTY_REGISTER(ButtonBase, CornerRadius, float)

// 只生成 Getter/Setter
FK_DEPENDENCY_PROPERTY_IMPL(ButtonBase, CornerRadius, float, 0.0f)

// 只生成 Callback
FK_DEPENDENCY_PROPERTY_CALLBACK(ButtonBase, CornerRadius, float, 0.0f)
```

### 2. 自定义验证

```cpp
binding::PropertyMetadata ButtonBase::BuildCornerRadiusMetadata() {
    binding::PropertyMetadata metadata(4.0f);
    metadata.propertyChangedCallback = CornerRadiusPropertyChanged;
    
    // 添加自定义验证
    metadata.validateCallback = [](const std::any& value) {
        try {
            float radius = std::any_cast<float>(value);
            return radius >= 0.0f && radius <= 100.0f;  // 限制范围
        } catch (...) {
            return false;
        }
    };
    
    return metadata;
}
```

### 3. 复杂的属性变更逻辑

```cpp
void ButtonBase::OnBackgroundChanged(const std::string& oldValue, const std::string& newValue) {
    // 复杂的业务逻辑
    if (IsMouseOver()) {
        // 特殊处理
    }
    
    // 通知子元素
    NotifyChildrenOfBackgroundChange();
    
    // 触发重绘
    InvalidateVisual();
}
```

---

## 迁移指南

### 步骤 1：更新头文件

**旧代码**：
```cpp
class ButtonBase {
public:
    static const binding::DependencyProperty& CornerRadiusProperty();
    void SetCornerRadius(float radius);
    float GetCornerRadius() const;
protected:
    virtual void OnCornerRadiusChanged(float oldValue, float newValue);
private:
    static binding::PropertyMetadata BuildCornerRadiusMetadata();
    static void CornerRadiusPropertyChanged(...);
};
```

**新代码**：
```cpp
#include "fk/ui/DependencyPropertyMacros.h"

class ButtonBase {
public:
    FK_DEPENDENCY_PROPERTY_DECLARE(CornerRadius, float)
};
```

### 步骤 2：更新实现文件

**旧代码** (~40 行)：
```cpp
const binding::DependencyProperty& ButtonBase::CornerRadiusProperty() { ... }
void ButtonBase::SetCornerRadius(float radius) { ... }
float ButtonBase::GetCornerRadius() const { ... }
void ButtonBase::CornerRadiusPropertyChanged(...) { ... }
binding::PropertyMetadata ButtonBase::BuildCornerRadiusMetadata() { ... }
void ButtonBase::OnCornerRadiusChanged(float oldValue, float newValue) { ... }
```

**新代码** (~10 行)：
```cpp
FK_DEPENDENCY_PROPERTY_DEFINE(ButtonBase, CornerRadius, float, 0.0f)

binding::PropertyMetadata ButtonBase::BuildCornerRadiusMetadata() {
    binding::PropertyMetadata metadata(4.0f);
    metadata.propertyChangedCallback = CornerRadiusPropertyChanged;
    return metadata;
}

void ButtonBase::OnCornerRadiusChanged(float oldValue, float newValue) {
    InvalidateVisual();
}
```

### 步骤 3：编译测试

```bash
cmake --build build -j8
```

---

## 优势总结

### 1. 代码减少
- **80% 减少**：从 ~55 行/属性 → ~11 行/属性
- Button 类 9 个属性：从 ~495 行 → ~99 行

### 2. 一致性
- 所有属性使用相同的模式
- 减少复制粘贴错误

### 3. 可维护性
- 修改宏定义即可影响所有属性
- 集中管理样板代码

### 4. 可读性
- 头文件更简洁
- 实现文件重点突出业务逻辑

### 5. 类型安全
- 编译期检查
- 类型信息自动推导

---

## 注意事项

1. **必须手动实现**：
   - `BuildXxxMetadata()` - 定义默认值、验证等
   - `OnXxxChanged()` - 属性变更的业务逻辑

2. **宏的局限性**：
   - 不能为属性添加额外的重载
   - 特殊需求时可以退回手动实现

3. **调试**：
   - 宏展开后的代码可以通过编译器查看
   - 使用 `-E` 选项查看预处理结果

---

## 结论

`DependencyPropertyMacros.h` 大幅简化了依赖属性的编写，将重复的样板代码从 ~55 行减少到 ~11 行（减少 80%），同时保持了完全的灵活性和类型安全性。推荐在所有新代码中使用，旧代码可以逐步迁移。
