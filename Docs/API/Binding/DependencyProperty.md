# DependencyProperty 文档

## 概述

`DependencyProperty` 是 FK_UI 框架的属性系统核心。它提供了比普通 C++ 成员变量更丰富的功能：数据绑定、属性变化通知、值验证、属性继承等。几乎所有 UI 元素的属性都是 DependencyProperty。

**命名空间：** `fk::binding`

**关键特性：**
- ✅ 支持数据绑定（单向、双向、单次）
- ✅ 属性变化通知（PropertyChanged 事件）
- ✅ 值验证（PropertyMetadata）
- ✅ 默认值管理
- ✅ 属性元数据

---

## 核心概念

### 依赖属性 vs 普通属性

| 特性 | 普通属性 | DependencyProperty |
|-----|--------|------------------|
| 数据绑定 | ❌ | ✅ |
| 属性变化通知 | ❌ | ✅ |
| 值验证 | 需手动实现 | 内置支持 |
| 内存占用 | 低（每个属性一个成员变量） | 高（只存储变更值） |
| 使用复杂度 | 简单 | 中等 |

### 属性存储模型

DependencyProperty 采用"稀疏存储"模型：
- 普通情况：属性使用默认值，不占用额外内存
- 被修改：只在被修改时才存储实际值
- 这样可以处理数千个属性而不会大幅增加内存

---

## 定义 DependencyProperty

### 基本定义方式

在 C++ 中，DependencyProperty 通过宏定义：

```cpp
// 在类声明中
class Button : public Control {
public:
    // 声明属性
    FK_PROPERTY(std::string, Content);
    FK_PROPERTY(Color, Background);
    FK_PROPERTY(Color, Foreground);
    FK_PROPERTY(bool, IsEnabled);
};
```

#### FK_PROPERTY 宏说明

```cpp
FK_PROPERTY(Type, PropertyName)
```

- **Type** - 属性类型（如 `std::string`、`Color`、`int` 等）
- **PropertyName** - 属性名称（CamelCase）
- **自动生成的方法：**
  - `Ptr PropertyName(Type value)` - 设置属性（返回 this 便于链式调用）
  - `Type PropertyName() const` - 获取属性
  - `DependencyProperty* PropertyNameProperty()` - 获取属性定义

### 带验证的属性定义

```cpp
class Slider : public Control {
public:
    // 值必须在 0-100 之间
    FK_PROPERTY(float, Value);
    
private:
    static bool ValidateValue(const std::any& value) {
        auto val = std::any_cast<float>(value);
        return val >= 0.0f && val <= 100.0f;
    }
};
```

### 属性变化回调

```cpp
class TextBlock : public Control {
private:
    // 当 Text 属性变化时的回调
    void OnTextChanged(const std::any& oldValue, const std::any& newValue) {
        std::string newText = std::any_cast<std::string>(newValue);
        InvalidateMeasure();  // 重新测量
        InvalidateRender();   // 重新渲染
    }
};
```

---

## 属性元数据

### PropertyMetadata 结构

```cpp
struct PropertyMetadata {
    std::any defaultValue;  // 默认值
    PropertyChangedCallback onChanged;  // 变化回调
    CoerceValueCallback onCoerce;  // 值强制转换
    ValidateValueCallback onValidate;  // 验证回调
    bool inherits = false;  // 是否继承到子元素
};
```

### 定义元数据

```cpp
auto metadata = PropertyMetadata{
    /* defaultValue */ std::any(std::string("")),
    /* onChanged */ [this](const std::any&, const std::any&) {
        InvalidateMeasure();
    },
    /* onCoerce */ nullptr,
    /* onValidate */ [](const std::any& val) {
        return std::any_cast<std::string>(val).length() <= 1000;
    }
};

// 在属性注册时使用
TextProperty.Register(metadata);
```

---

## 使用 DependencyProperty

### 获取和设置属性值

```cpp
// 设置属性（链式调用）
button->Content("Click Me")
      ->Background(Color::Blue());

// 获取属性值
std::string text = button->Content();
Color bg = button->Background();
```

### 默认值

每个 DependencyProperty 都有默认值。如果属性未被显式设置，返回默认值。

```cpp
Color defaultBg = button->Background();  // 返回默认颜色
```

### 属性变化事件

订阅属性变化事件：

```cpp
button->ContentProperty()->PropertyChanged() += 
    [](DependencyPropertyChangedEventArgs& args) {
        std::string newValue = std::any_cast<std::string>(args.NewValue);
        printf("Content 已变化为：%s\n", newValue.c_str());
    };
```

---

## 与数据绑定集成

### 简单绑定示例

```cpp
// ViewModel 中
class ItemViewModel : public ObservableObject {
public:
    FK_PROPERTY(std::string, ItemName);
    FK_PROPERTY(bool, IsSelected);
};

// View 中
auto vm = std::make_shared<ItemViewModel>();
auto textBlock = ui::textBlock()
    ->Bind(binding::Bind(vm->ItemNameProperty()));
// 当 ViewModel 的 ItemName 变化时，TextBlock 自动更新
```

### 双向绑定

```cpp
auto textBox = ui::textBox()
    ->Bind(binding::Bind(vm->ItemNameProperty())
               ->TwoWay());
// TextBox 变化时更新 ViewModel，ViewModel 变化时更新 TextBox
```

---

## 完整示例

### 示例 1：定义自定义控件的属性

```cpp
// CustomProgressBar.h
class CustomProgressBar : public Control {
public:
    FK_PROPERTY(float, Value);      // 当前值
    FK_PROPERTY(float, Minimum);    // 最小值
    FK_PROPERTY(float, Maximum);    // 最大值
    FK_PROPERTY(Color, FillColor);  // 填充颜色
    
    CustomProgressBar()
        ->Minimum(0.0f)
        ->Maximum(100.0f)
        ->Value(0.0f)
        ->FillColor(Color::Green());

private:
    bool ValidateValue(const std::any& value) {
        auto v = std::any_cast<float>(value);
        auto min = Minimum();
        auto max = Maximum();
        return v >= min && v <= max;
    }
};
```

### 示例 2：属性变化回调

```cpp
class DataGrid : public Control {
public:
    FK_PROPERTY(std::vector<std::string>, ItemsSource);
    
    DataGrid() {
        // 当 ItemsSource 变化时重新加载数据
        ItemsSourceProperty()->PropertyChanged() +=
            [this](DependencyPropertyChangedEventArgs&) {
                ReloadData();
            };
    }
    
private:
    void ReloadData() {
        auto items = ItemsSource();
        // 更新 UI 显示新数据
    }
};
```

### 示例 3：验证属性值

```cpp
class AgeControl : public TextBox {
private:
    bool ValidateAge(const std::any& value) {
        try {
            auto age = std::any_cast<int>(value);
            return age >= 0 && age <= 150;
        } catch (...) {
            return false;  // 无法转换为 int
        }
    }
};
```

---

## 常见问题

### Q1：DependencyProperty 和普通属性有何区别？
**A：** DependencyProperty 支持数据绑定、属性通知、验证等高级功能，但占用更多内存和处理时间。选择取决于是否需要这些功能。

### Q2：如何监听属性变化？
**A：** 使用 PropertyChanged 事件或属性的回调函数：
```cpp
property->PropertyChanged() += [](auto& args) {
    // 处理变化
};
```

### Q3：如何创建只读属性？
**A：** 当前框架中，DependencyProperty 的访问权限主要通过类的访问控制（private/public）管理。

### Q4：属性继承是什么意思？
**A：** 如果一个属性设置为 `inherits = true`，子元素会自动继承父元素的属性值（如 Foreground、FontSize）。

### Q5：如何为属性设置默认值？
**A：** 在 PropertyMetadata 中指定 defaultValue：
```cpp
PropertyMetadata{
    /* defaultValue */ std::any(100.0f),
    ...
};
```

---

## 性能考虑

- **属性访问：** DependencyProperty 的访问速度比普通成员变量略慢
- **内存占用：** 大量属性时，稀疏存储模型能显著节省内存
- **绑定开销：** 频繁绑定/解绑可能有性能影响

---

## 最佳实践

1. **对 UI 属性使用 DependencyProperty：**
   ```cpp
   // ✅ 推荐
   FK_PROPERTY(Color, Background);
   
   // ⚠️ 避免
   Color m_background;
   ```

2. **为可绑定属性提供元数据：**
   ```cpp
   // ✅ 推荐：指定默认值和变化回调
   PropertyMetadata metadata{
       /* defaultValue */ std::any(Color::White()),
       /* onChanged */ [this](const std::any&, const std::any&) {
           InvalidateRender();
       }
   };
   ```

3. **使用验证防止无效值：**
   ```cpp
   // ✅ 推荐
   PropertyMetadata metadata{
       /* onValidate */ [](const std::any& val) {
           return std::any_cast<float>(val) >= 0.0f;
       }
   };
   ```

---

## 另见

- [ICommand.md](../Core/ICommand.md) - 命令系统
- [Button.md](../UI/Button.md) - Button 的属性绑定示例
- [ObservableObject.md](ObservableObject.md) - ViewModel 属性（待完成）
- [Binding.md](Binding.md) - 数据绑定机制（待完成）
