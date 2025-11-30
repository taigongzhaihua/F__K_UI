# DependencyObject

## 概览

| 属性 | 值 |
|------|-----|
| **命名空间** | `fk::binding` |
| **头文件** | `fk/binding/DependencyObject.h` |
| **源文件** | `src/binding/DependencyObject.cpp` |

## 描述

`DependencyObject` 是 F__K_UI 属性系统的核心基类，负责：

- **依赖属性系统**：支持具有元数据、变更通知和值验证的属性
- **数据绑定**：属性和数据源之间的自动同步
- **逻辑树**：父子关系管理，支持元素名称解析和 DataContext 继承
- **值来源追踪**：跟踪属性值来源（本地、绑定、样式、默认值等）

所有 UI 元素和许多框架类都派生自 `DependencyObject`。

## 类定义

```cpp
namespace fk::binding {

class DependencyObject {
public:
    // 事件类型别名
    using PropertyChangedEvent = core::Event<const DependencyProperty&, const std::any&, const std::any&, ValueSource, ValueSource>;
    using BindingChangedEvent = core::Event<const DependencyProperty&, const std::shared_ptr<BindingExpression>&, const std::shared_ptr<BindingExpression>&>;
    using DataContextChangedEvent = core::Event<const std::any&, const std::any&>;

    virtual ~DependencyObject();

    // 属性值管理
    const std::any& GetValue(const DependencyProperty& property) const;
    template<typename T> T GetValue(const DependencyProperty& property) const;
    void SetValue(const DependencyProperty& property, std::any value);
    template<typename T> void SetValue(const DependencyProperty& property, T&& value);
    void SetValue(const DependencyProperty& property, Binding binding);
    void ClearValue(const DependencyProperty& property);
    ValueSource GetValueSource(const DependencyProperty& property) const;

    // 绑定管理
    void SetBinding(const DependencyProperty& property, const Binding& binding);
    void SetBinding(const DependencyProperty& property, MultiBinding binding);
    void ClearBinding(const DependencyProperty& property);
    std::shared_ptr<BindingExpression> GetBinding(const DependencyProperty& property) const;
    void UpdateSource(const DependencyProperty& property);

    // 数据上下文
    void SetDataContext(std::any value);
    template<typename T> void SetDataContext(T&& value);
    void ClearDataContext();
    const std::any& GetDataContext() const noexcept;
    bool HasDataContext() const noexcept;
    void SetDataContextParent(DependencyObject* parent);

    // 元素名称
    void SetElementName(std::string name);
    const std::string& GetElementName() const noexcept;

    // 逻辑树
    void SetLogicalParent(DependencyObject* parent);
    DependencyObject* GetLogicalParent() const noexcept;
    void AddLogicalChild(DependencyObject* child);
    void RemoveLogicalChild(DependencyObject* child);
    const std::vector<DependencyObject*>& GetLogicalChildren() const noexcept;

    // 元素查找
    DependencyObject* FindElementByName(std::string_view name);
    const DependencyObject* FindElementByName(std::string_view name) const;
    virtual const DependencyProperty* FindProperty(const std::string& propertyName) const;

    // 绑定上下文访问
    BindingContext& GetBindingContext() noexcept;
    const BindingContext& GetBindingContext() const noexcept;

    // 事件
    PropertyChangedEvent PropertyChanged;
    BindingChangedEvent BindingChanged;
    DataContextChangedEvent DataContextChanged;

protected:
    DependencyObject();

    virtual void OnPropertyChanged(const DependencyProperty& property, const std::any& oldValue, const std::any& newValue, ValueSource oldSource, ValueSource newSource);
    virtual void OnBindingChanged(const DependencyProperty& property, const std::shared_ptr<BindingExpression>& oldBinding, const std::shared_ptr<BindingExpression>& newBinding);
    virtual void OnDataContextChanged(const std::any& oldValue, const std::any& newValue);

    PropertyStore& MutablePropertyStore() noexcept;
    const PropertyStore& GetPropertyStore() const noexcept;

private:
    BindingContext bindingContext_;
    PropertyStore propertyStore_;
    DependencyObject* logicalParent_{nullptr};
    std::vector<DependencyObject*> logicalChildren_{};
    std::string elementName_{};
    std::shared_ptr<MultiBindingExpression> activeMultiBinding_;

    friend class BindingExpression;
    friend class MultiBindingExpression;
};

} // namespace fk::binding
```

## 构造函数与析构函数

### DependencyObject()

```cpp
protected:
DependencyObject();
```

受保护的构造函数，初始化绑定上下文和属性存储。

**说明**：
- 创建 `BindingContext` 并连接 `DataContextChanged` 事件
- 初始化 `PropertyStore` 并设置值变更回调
- 派生类应调用此构造函数

### ~DependencyObject()

```cpp
virtual ~DependencyObject();
```

虚析构函数，支持多态删除。

---

## 属性值管理方法

### GetValue

```cpp
const std::any& GetValue(const DependencyProperty& property) const;

template<typename T>
T GetValue(const DependencyProperty& property) const;
```

获取依赖属性的当前有效值。

**参数**：
| 参数 | 类型 | 描述 |
|------|------|------|
| `property` | `const DependencyProperty&` | 要查询的依赖属性 |

**返回值**：属性值（作为 `std::any` 或类型化值）

**示例**：
```cpp
// 获取原始值
const std::any& value = element->GetValue(UIElement::WidthProperty());

// 获取类型化值
double width = element->GetValue<double>(UIElement::WidthProperty());
```

### SetValue

```cpp
void SetValue(const DependencyProperty& property, std::any value);

template<typename T>
std::enable_if_t<!std::is_same_v<std::decay_t<T>, Binding>, void>
SetValue(const DependencyProperty& property, T&& value);

void SetValue(const DependencyProperty& property, Binding binding);
```

设置依赖属性的值。

**参数**：
| 参数 | 类型 | 描述 |
|------|------|------|
| `property` | `const DependencyProperty&` | 要设置的依赖属性 |
| `value` | `std::any` / `T&&` / `Binding` | 新值或绑定配置 |

**异常**：
- 如果值类型与属性类型不匹配，抛出 `std::invalid_argument`
- 如果值验证失败，抛出 `std::invalid_argument`

**说明**：
- 设置本地值（`ValueSource::Local`）
- 如果传入 `Binding` 对象，内部调用 `SetBinding()`
- 触发 `PropertyChanged` 事件

**示例**：
```cpp
// 设置值
element->SetValue(UIElement::WidthProperty(), 200.0);

// 设置绑定
element->SetValue(UIElement::WidthProperty(), fk::bind("Width"));
```

### ClearValue

```cpp
void ClearValue(const DependencyProperty& property);
```

清除属性的本地值。

**参数**：
| 参数 | 类型 | 描述 |
|------|------|------|
| `property` | `const DependencyProperty&` | 要清除的依赖属性 |

**说明**：
- 清除本地值后，属性回退到其他值来源（绑定、样式、继承、默认值）
- 如果值发生变化，触发 `PropertyChanged` 事件

**示例**：
```cpp
element->ClearValue(UIElement::WidthProperty());
// 现在宽度使用默认值或继承值
```

### GetValueSource

```cpp
ValueSource GetValueSource(const DependencyProperty& property) const;
```

获取属性当前值的来源。

**参数**：
| 参数 | 类型 | 描述 |
|------|------|------|
| `property` | `const DependencyProperty&` | 要查询的依赖属性 |

**返回值**：`ValueSource` 枚举值

**示例**：
```cpp
auto source = element->GetValueSource(UIElement::WidthProperty());
switch (source) {
    case fk::ValueSource::Local:
        std::cout << "本地设置的值" << std::endl;
        break;
    case fk::ValueSource::Binding:
        std::cout << "绑定的值" << std::endl;
        break;
    case fk::ValueSource::Default:
        std::cout << "默认值" << std::endl;
        break;
    // ...
}
```

---

## 绑定管理方法

### SetBinding

```cpp
void SetBinding(const DependencyProperty& property, const Binding& binding);
void SetBinding(const DependencyProperty& property, MultiBinding binding);
```

设置属性的数据绑定。

**参数**：
| 参数 | 类型 | 描述 |
|------|------|------|
| `property` | `const DependencyProperty&` | 要绑定的依赖属性 |
| `binding` | `const Binding&` / `MultiBinding` | 绑定配置 |

**说明**：
- 如果属性已有绑定，先分离旧绑定
- 创建并激活 `BindingExpression` 或 `MultiBindingExpression`
- 触发 `BindingChanged` 事件

**示例**：
```cpp
// 单一绑定
fk::Binding binding;
binding.Path("Username").Mode(fk::BindingMode::TwoWay);
textBox->SetBinding(TextBox::TextProperty(), binding);

// 多绑定
fk::MultiBinding multi;
multi.AddBinding(fk::bind("FirstName"));
multi.AddBinding(fk::bind("LastName"));
multi.Converter(std::make_shared<FullNameConverter>());
textBlock->SetBinding(TextBlock::TextProperty(), multi);
```

### ClearBinding

```cpp
void ClearBinding(const DependencyProperty& property);
```

清除属性的绑定。

**参数**：
| 参数 | 类型 | 描述 |
|------|------|------|
| `property` | `const DependencyProperty&` | 要清除绑定的依赖属性 |

**说明**：
- 分离并移除绑定表达式
- 属性值回退到本地值或默认值
- 触发 `BindingChanged` 事件

**示例**：
```cpp
textBox->ClearBinding(TextBox::TextProperty());
```

### GetBinding

```cpp
std::shared_ptr<BindingExpression> GetBinding(const DependencyProperty& property) const;
```

获取属性的绑定表达式。

**参数**：
| 参数 | 类型 | 描述 |
|------|------|------|
| `property` | `const DependencyProperty&` | 要查询的依赖属性 |

**返回值**：绑定表达式智能指针，如果没有绑定返回 `nullptr`

**示例**：
```cpp
auto expr = textBox->GetBinding(TextBox::TextProperty());
if (expr && expr->IsActive()) {
    std::cout << "有活动的绑定" << std::endl;
}
```

### UpdateSource

```cpp
void UpdateSource(const DependencyProperty& property);
```

显式触发绑定的源更新。

**参数**：
| 参数 | 类型 | 描述 |
|------|------|------|
| `property` | `const DependencyProperty&` | 要更新源的依赖属性 |

**说明**：
- 仅对 `UpdateSourceTrigger::Explicit` 模式的绑定有效
- 内部调用 `BindingExpression::UpdateSourceExplicitly()`

**示例**：
```cpp
// 配置显式更新模式
fk::Binding binding;
binding.Path("Email")
       .Mode(fk::BindingMode::TwoWay)
       .SetUpdateSourceTrigger(fk::UpdateSourceTrigger::Explicit);
textBox->SetBinding(TextBox::TextProperty(), binding);

// 用户点击保存按钮时更新
saveButton->OnClick([textBox]() {
    textBox->UpdateSource(TextBox::TextProperty());
});
```

---

## 数据上下文方法

### SetDataContext

```cpp
void SetDataContext(std::any value);

template<typename T>
void SetDataContext(T&& value);
```

设置数据上下文。

**参数**：
| 参数 | 类型 | 描述 |
|------|------|------|
| `value` | `std::any` / `T&&` | 数据上下文值（通常是 ViewModel） |

**说明**：
- 设置本地数据上下文
- 子元素自动继承（除非有自己的本地数据上下文）
- 触发 `DataContextChanged` 事件

**示例**：
```cpp
auto viewModel = std::make_shared<MyViewModel>();
window->SetDataContext(viewModel);
// 所有子元素现在可以绑定到 viewModel 的属性
```

### ClearDataContext

```cpp
void ClearDataContext();
```

清除本地数据上下文。

**说明**：
- 清除后，元素使用从父级继承的数据上下文
- 触发 `DataContextChanged` 事件（如果有效值变化）

**示例**：
```cpp
childElement->ClearDataContext();
// 现在使用父级的数据上下文
```

### GetDataContext

```cpp
const std::any& GetDataContext() const noexcept;
```

获取当前有效的数据上下文。

**返回值**：数据上下文的常量引用

**示例**：
```cpp
const auto& dc = element->GetDataContext();
if (dc.has_value()) {
    auto* vm = std::any_cast<MyViewModel*>(dc);
    // 使用 viewModel
}
```

### HasDataContext

```cpp
bool HasDataContext() const noexcept;
```

检查是否有有效的数据上下文。

**返回值**：如果有数据上下文返回 `true`

### SetDataContextParent

```cpp
void SetDataContextParent(DependencyObject* parent);
```

设置数据上下文的父级（用于继承）。

**参数**：
| 参数 | 类型 | 描述 |
|------|------|------|
| `parent` | `DependencyObject*` | 父级对象，可以为 `nullptr` |

**说明**：通常在设置逻辑父级时自动调用

---

## 元素名称方法

### SetElementName

```cpp
void SetElementName(std::string name);
```

设置元素的名称（用于 `ElementName` 绑定）。

**参数**：
| 参数 | 类型 | 描述 |
|------|------|------|
| `name` | `std::string` | 元素名称 |

**示例**：
```cpp
slider->SetElementName("volumeSlider");

// 其他元素可以通过名称绑定
fk::Binding binding;
binding.ElementName("volumeSlider").Path("Value");
textBlock->SetBinding(TextBlock::TextProperty(), binding);
```

### GetElementName

```cpp
const std::string& GetElementName() const noexcept;
```

获取元素名称。

**返回值**：元素名称的常量引用

---

## 逻辑树方法

### SetLogicalParent

```cpp
void SetLogicalParent(DependencyObject* parent);
```

设置逻辑父级。

**参数**：
| 参数 | 类型 | 描述 |
|------|------|------|
| `parent` | `DependencyObject*` | 父级对象，可以为 `nullptr` |

**说明**：
- 从旧父级的子列表中移除
- 添加到新父级的子列表
- 更新数据上下文继承

**示例**：
```cpp
childElement->SetLogicalParent(parentElement);
```

### GetLogicalParent

```cpp
DependencyObject* GetLogicalParent() const noexcept;
```

获取逻辑父级。

**返回值**：父级对象指针，如果没有返回 `nullptr`

### AddLogicalChild

```cpp
void AddLogicalChild(DependencyObject* child);
```

添加逻辑子元素。

**参数**：
| 参数 | 类型 | 描述 |
|------|------|------|
| `child` | `DependencyObject*` | 子元素 |

**说明**：内部调用 `child->SetLogicalParent(this)`

**示例**：
```cpp
parent->AddLogicalChild(child);
// 等同于 child->SetLogicalParent(parent)
```

### RemoveLogicalChild

```cpp
void RemoveLogicalChild(DependencyObject* child);
```

移除逻辑子元素。

**参数**：
| 参数 | 类型 | 描述 |
|------|------|------|
| `child` | `DependencyObject*` | 子元素 |

**说明**：内部调用 `child->SetLogicalParent(nullptr)`

### GetLogicalChildren

```cpp
const std::vector<DependencyObject*>& GetLogicalChildren() const noexcept;
```

获取所有逻辑子元素。

**返回值**：子元素指针向量的常量引用

**示例**：
```cpp
for (auto* child : parent->GetLogicalChildren()) {
    // 处理子元素
}
```

---

## 元素查找方法

### FindElementByName

```cpp
DependencyObject* FindElementByName(std::string_view name);
const DependencyObject* FindElementByName(std::string_view name) const;
```

在逻辑树中按名称查找元素。

**参数**：
| 参数 | 类型 | 描述 |
|------|------|------|
| `name` | `std::string_view` | 要查找的元素名称 |

**返回值**：找到的元素指针，如果未找到返回 `nullptr`

**说明**：递归搜索当前元素及其所有子元素

**示例**：
```cpp
auto* slider = window->FindElementByName("volumeSlider");
if (slider) {
    // 使用找到的元素
}
```

### FindProperty

```cpp
virtual const DependencyProperty* FindProperty(const std::string& propertyName) const;
```

按名称查找依赖属性。

**参数**：
| 参数 | 类型 | 描述 |
|------|------|------|
| `propertyName` | `const std::string&` | 属性名称 |

**返回值**：找到的属性指针，如果未找到返回 `nullptr`

**说明**：使用全局注册表按对象的实际类型查找属性

---

## 绑定上下文访问方法

### GetBindingContext

```cpp
BindingContext& GetBindingContext() noexcept;
const BindingContext& GetBindingContext() const noexcept;
```

获取绑定上下文。

**返回值**：`BindingContext` 的引用

**说明**：用于高级场景，直接访问数据上下文管理

---

## 事件

### PropertyChanged

```cpp
using PropertyChangedEvent = core::Event<const DependencyProperty&, const std::any&, const std::any&, ValueSource, ValueSource>;
PropertyChangedEvent PropertyChanged;
```

任何依赖属性值变更时触发的事件。

**参数**：
| 参数 | 描述 |
|------|------|
| 第一个参数 | 变更的属性 |
| 第二个参数 | 旧值 |
| 第三个参数 | 新值 |
| 第四个参数 | 旧值来源 |
| 第五个参数 | 新值来源 |

**示例**：
```cpp
element->PropertyChanged += [](const fk::DependencyProperty& prop,
                               const std::any& oldVal,
                               const std::any& newVal,
                               fk::ValueSource oldSource,
                               fk::ValueSource newSource) {
    std::cout << "属性 " << prop.Name() << " 变更" << std::endl;
};
```

### BindingChanged

```cpp
using BindingChangedEvent = core::Event<const DependencyProperty&, const std::shared_ptr<BindingExpression>&, const std::shared_ptr<BindingExpression>&>;
BindingChangedEvent BindingChanged;
```

绑定变更时触发的事件。

**参数**：
| 参数 | 描述 |
|------|------|
| 第一个参数 | 绑定变更的属性 |
| 第二个参数 | 旧的绑定表达式（可能为 `nullptr`） |
| 第三个参数 | 新的绑定表达式（可能为 `nullptr`） |

**示例**：
```cpp
element->BindingChanged += [](const fk::DependencyProperty& prop,
                              const std::shared_ptr<fk::BindingExpression>& oldExpr,
                              const std::shared_ptr<fk::BindingExpression>& newExpr) {
    if (newExpr) {
        std::cout << "属性 " << prop.Name() << " 绑定已设置" << std::endl;
    } else {
        std::cout << "属性 " << prop.Name() << " 绑定已清除" << std::endl;
    }
};
```

### DataContextChanged

```cpp
using DataContextChangedEvent = core::Event<const std::any&, const std::any&>;
DataContextChangedEvent DataContextChanged;
```

数据上下文变更时触发的事件。

**参数**：
| 参数 | 描述 |
|------|------|
| 第一个参数 | 旧的数据上下文 |
| 第二个参数 | 新的数据上下文 |

**示例**：
```cpp
element->DataContextChanged += [](const std::any& oldDC, const std::any& newDC) {
    if (newDC.has_value()) {
        std::cout << "数据上下文已设置" << std::endl;
    } else {
        std::cout << "数据上下文已清除" << std::endl;
    }
};
```

---

## 受保护的虚方法

### OnPropertyChanged

```cpp
virtual void OnPropertyChanged(const DependencyProperty& property, const std::any& oldValue, const std::any& newValue, ValueSource oldSource, ValueSource newSource);
```

属性变更时的虚回调，派生类可重写以添加自定义逻辑。

### OnBindingChanged

```cpp
virtual void OnBindingChanged(const DependencyProperty& property, const std::shared_ptr<BindingExpression>& oldBinding, const std::shared_ptr<BindingExpression>& newBinding);
```

绑定变更时的虚回调。

### OnDataContextChanged

```cpp
virtual void OnDataContextChanged(const std::any& oldValue, const std::any& newValue);
```

数据上下文变更时的虚回调。

---

## 值优先级

属性值按以下优先级确定（从高到低）：

| 优先级 | 来源 | 描述 |
|--------|------|------|
| 1 | `Local` | 通过 `SetValue()` 设置的本地值 |
| 2 | `Binding` | 绑定计算的值 |
| 3 | `Style` | 样式设置的值 |
| 4 | `Inherited` | 从父元素继承的值 |
| 5 | `Default` | 属性元数据中定义的默认值 |

---

## 使用示例

### 基本属性操作

```cpp
#include "fk/binding/DependencyObject.h"
#include "fk/ui/UIElement.h"

// 设置属性值
element->SetValue(UIElement::WidthProperty(), 200.0);
element->SetValue(UIElement::HeightProperty(), 100.0);

// 获取属性值
double width = element->GetValue<double>(UIElement::WidthProperty());

// 清除本地值
element->ClearValue(UIElement::WidthProperty());

// 检查值来源
auto source = element->GetValueSource(UIElement::WidthProperty());
```

### 数据绑定

```cpp
#include "fk/binding/Binding.h"
#include "fk/binding/ObservableObject.h"

// 创建 ViewModel
class MyViewModel : public fk::ObservableObject {
public:
    fk::ObservableProperty<std::string, MyViewModel> username{this, "Username"};
};

auto viewModel = std::make_shared<MyViewModel>();
viewModel->username = "张三";

// 设置数据上下文
window->SetDataContext(viewModel);

// 创建绑定
fk::Binding binding;
binding.Path("Username").Mode(fk::BindingMode::TwoWay);
textBox->SetBinding(TextBox::TextProperty(), binding);

// ViewModel 变更 → UI 自动更新
viewModel->username = "李四";
```

### 逻辑树操作

```cpp
// 建立父子关系
parent->AddLogicalChild(child);

// 遍历子元素
for (auto* c : parent->GetLogicalChildren()) {
    std::cout << c->GetElementName() << std::endl;
}

// 向上遍历
auto* current = element->GetLogicalParent();
while (current) {
    // 处理祖先
    current = current->GetLogicalParent();
}

// 按名称查找元素
slider->SetElementName("volumeSlider");
auto* found = window->FindElementByName("volumeSlider");
```

### 属性变更监听

```cpp
element->PropertyChanged += [](const fk::DependencyProperty& prop,
                               const std::any& oldVal,
                               const std::any& newVal,
                               fk::ValueSource, fk::ValueSource) {
    if (&prop == &UIElement::WidthProperty()) {
        double oldWidth = std::any_cast<double>(oldVal);
        double newWidth = std::any_cast<double>(newVal);
        std::cout << "宽度从 " << oldWidth << " 变为 " << newWidth << std::endl;
    }
};

element->SetValue(UIElement::WidthProperty(), 300.0);
```

---

## 私有成员

| 成员 | 类型 | 描述 |
|------|------|------|
| `bindingContext_` | `BindingContext` | 数据上下文管理 |
| `dataContextChangedConnection_` | `Connection` | DataContext 事件连接 |
| `propertyStore_` | `PropertyStore` | 属性值存储 |
| `logicalParent_` | `DependencyObject*` | 逻辑父级 |
| `logicalChildren_` | `std::vector<DependencyObject*>` | 逻辑子元素列表 |
| `elementName_` | `std::string` | 元素名称 |
| `activeMultiBinding_` | `std::shared_ptr<MultiBindingExpression>` | 活动的多绑定表达式 |

---

## 性能考虑

### 属性访问
- **GetValue**：O(1) - 哈希映射查找
- **SetValue**：O(1) + 通知开销
- **绑定解析**：首次额外开销，后续缓存

### 内存使用
- 基础对象：~200 字节
- 每个设置的属性：~48 字节
- 每个活动绑定：~100 字节

### 最佳实践

1. **避免不必要的 SetValue 调用**
   ```cpp
   // 不推荐 - 触发多次通知
   for (int i = 0; i < 100; i++) {
       element->SetValue(Property, i);
   }
   
   // 推荐 - 只设置最终值
   element->SetValue(Property, 99);
   ```

2. **及时清理绑定**
   ```cpp
   // 不再需要时清除绑定释放资源
   element->ClearBinding(TextProperty());
   ```

---

## 线程安全

`DependencyObject` 不是线程安全的。所有属性访问必须在创建对象的线程上进行（通常是 UI 线程）。

使用 `Dispatcher` 从其他线程访问：

```cpp
dispatcher->InvokeAsync([element]() {
    // 现在在 UI 线程上
    element->SetValue(Property, value);
});
```

---

## 相关类

- [DependencyProperty](DependencyProperty.md) - 依赖属性元数据和注册
- [Binding](Binding.md) - 数据绑定配置
- [BindingExpression](BindingExpression.md) - 活动绑定实例
- [BindingContext](BindingContext.md) - 数据上下文管理
- [PropertyStore](PropertyStore.md) - 属性值存储
- [ObservableObject](ObservableObject.md) - ViewModel 基类

## 另请参阅

- [数据绑定概述](../BINDING_MODULE_OVERVIEW.md)
- [入门指南](../../Guides/GettingStarted.md)
