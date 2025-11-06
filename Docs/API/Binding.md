# Binding 模块 API 文档

数据绑定与依赖属性系统模块，实现 MVVM 模式支持�?

命名空间: `fk::binding`

---

## 目录

- [DependencyProperty](#dependencyproperty) - 依赖属�?
- [DependencyObject](#dependencyobject) - 依赖对象
- [Binding](#binding) - 数据绑定
- [BindingExpression](#bindingexpression) - 绑定表达�?
- [ObservableObject](#observableobject) - 可观察对�?
- [INotifyPropertyChanged](#inotifypropertychanged) - 属性变更通知接口
- [PropertyStore](#propertystore) - 属性存�?
- [ValueConverter](#valueconverter) - 值转换器

---

## DependencyProperty

**依赖属�?* - 支持数据绑定、默认值、属性变更回调和验证的属性系统�?

### 类定�?

```cpp
class DependencyProperty {
public:
    static const DependencyProperty& Register(
        std::string name,
        std::type_index propertyType,
        std::type_index ownerType,
        PropertyMetadata metadata = {}
    );
    
    static const DependencyProperty& RegisterAttached(
        std::string name,
        std::type_index propertyType,
        std::type_index ownerType,
        PropertyMetadata metadata = {}
    );
};
```

### 枚举

#### BindingMode

数据绑定模式

| �?| 说明 |
|---|---|
| `OneTime` | 一次性绑定，仅在初始化时设置 |
| `OneWay` | 单向绑定，源到目�?|
| `TwoWay` | 双向绑定，源和目标相互同�?|
| `OneWayToSource` | 反向单向绑定，目标到�?|

#### UpdateSourceTrigger

源更新触发器

| �?| 说明 |
|---|---|
| `Default` | 使用属性的默认设置 |
| `PropertyChanged` | 属性改变时立即更新 |
| `LostFocus` | 失去焦点时更�?|
| `Explicit` | 手动调用 UpdateSource() 时更�?|

### PropertyMetadata 结构

```cpp
struct PropertyMetadata {
    std::any defaultValue{};
    PropertyChangedCallback propertyChangedCallback{};
    ValidateValueCallback validateCallback{};
    BindingOptions bindingOptions{};
};

struct BindingOptions {
    BindingMode defaultMode{BindingMode::OneWay};
    UpdateSourceTrigger updateSourceTrigger{UpdateSourceTrigger::PropertyChanged};
    bool inheritsDataContext{false};
};
```

### 核心方法

#### Register()

注册普通依赖属�?

```cpp
static const DependencyProperty& Register(
    std::string name,
    std::type_index propertyType,
    std::type_index ownerType,
    PropertyMetadata metadata = {}
);
```

**参数:**
- `name` - 属性名�?
- `propertyType` - 属性类型（使用 typeid�?
- `ownerType` - 所有者类�?
- `metadata` - 属性元数据

**返回:** 注册的依赖属性引�?

**线程安全:** 是（应在静态初始化时调用）

**示例:**
```cpp
class MyControl : public DependencyObject {
public:
    static const DependencyProperty& WidthProperty() {
        static const auto& prop = DependencyProperty::Register(
            "Width",
            typeid(float),
            typeid(MyControl),
            PropertyMetadata{
                .defaultValue = 100.0f,
                .propertyChangedCallback = [](DependencyObject& obj, 
                    const DependencyProperty& prop,
                    const std::any& oldValue, 
                    const std::any& newValue) {
                    auto& control = static_cast<MyControl&>(obj);
                    control.OnWidthChanged(
                        std::any_cast<float>(oldValue),
                        std::any_cast<float>(newValue)
                    );
                },
                .validateCallback = [](const std::any& value) {
                    float v = std::any_cast<float>(value);
                    return v >= 0.0f;  // 宽度不能为负
                }
            }
        );
        return prop;
    }
    
    void SetWidth(float value) {
        SetValue(WidthProperty(), value);
    }
    
    float GetWidth() const {
        return GetValue<float>(WidthProperty());
    }
};
```

#### RegisterAttached()

注册附加属性（可附加到任何 DependencyObject�?

```cpp
static const DependencyProperty& RegisterAttached(
    std::string name,
    std::type_index propertyType,
    std::type_index ownerType,
    PropertyMetadata metadata = {}
);
```

**参数:** �?Register()

**返回:** 注册的附加属性引�?

**示例:**
```cpp
class Grid : public Panel {
public:
    // Grid.Row 附加属�?
    static const DependencyProperty& RowProperty() {
        static const auto& prop = DependencyProperty::RegisterAttached(
            "Row",
            typeid(int),
            typeid(Grid),
            PropertyMetadata{.defaultValue = 0}
        );
        return prop;
    }
    
    static void SetRow(UIElement* element, int row) {
        element->SetValue(RowProperty(), row);
    }
    
    static int GetRow(UIElement* element) {
        return element->GetValue<int>(RowProperty());
    }
};

// 使用
auto btn = std::make_shared<Button>();
Grid::SetRow(btn.get(), 1);  // 设置按钮在第 1 �?
```

### 属性访问器

#### Name()

获取属性名�?

```cpp
const std::string& Name() const noexcept;
```

#### PropertyType()

获取属性类�?

```cpp
std::type_index PropertyType() const noexcept;
```

#### OwnerType()

获取所有者类�?

```cpp
std::type_index OwnerType() const noexcept;
```

#### IsAttached()

检查是否为附加属�?

```cpp
bool IsAttached() const noexcept;
```

---

## DependencyObject

**依赖对象** - 支持依赖属性、数据绑定和逻辑树的基类�?

### 类定�?

```cpp
class DependencyObject {
public:
    virtual ~DependencyObject();
    
    // 事件
    PropertyChangedEvent PropertyChanged;
    BindingChangedEvent BindingChanged;
    DataContextChangedEvent DataContextChanged;
};
```

### 核心方法

#### GetValue()

获取依赖属性的�?

```cpp
const std::any& GetValue(const DependencyProperty& property) const;

template<typename T>
T GetValue(const DependencyProperty& property) const;
```

**参数:**
- `property` - 依赖属�?

**返回:** 属性�?

**线程安全:** 否（必须�?UI 线程�?

**示例:**
```cpp
float width = control->GetValue<float>(MyControl::WidthProperty());

// �?
const std::any& value = control->GetValue(MyControl::WidthProperty());
```

#### SetValue()

设置依赖属性的�?

```cpp
void SetValue(const DependencyProperty& property, std::any value);

template<typename T>
void SetValue(const DependencyProperty& property, T&& value);

void SetValue(const DependencyProperty& property, Binding binding);
```

**参数:**
- `property` - 依赖属�?
- `value` - 新值（可以是具体值或 Binding 对象�?

**线程安全:** 否（必须�?UI 线程�?

**示例:**
```cpp
// 设置具体�?
control->SetValue(MyControl::WidthProperty(), 200.0f);

// 设置绑定
control->SetValue(MyControl::WidthProperty(), 
    Binding()
        .Path("Width")
        .Mode(BindingMode::TwoWay)
);
```

#### ClearValue()

清除属性值，恢复为默认�?

```cpp
void ClearValue(const DependencyProperty& property);
```

**参数:**
- `property` - 依赖属�?

**线程安全:** �?

**示例:**
```cpp
control->ClearValue(MyControl::WidthProperty());
// 现在 Width 回到默认�?100.0f
```

#### GetValueSource()

获取属性值的来源

```cpp
ValueSource GetValueSource(const DependencyProperty& property) const;
```

**返回:** 值来源枚�?

**ValueSource 枚举:**
| �?| 说明 |
|---|---|
| `Default` | 使用默认�?|
| `Local` | 本地设置的�?|
| `DataBinding` | 数据绑定的�?|

**示例:**
```cpp
auto source = control->GetValueSource(MyControl::WidthProperty());
if (source == ValueSource::DataBinding) {
    std::cout << "Width is data-bound" << std::endl;
}
```

### 数据绑定方法

#### SetBinding()

为属性设置数据绑�?

```cpp
void SetBinding(const DependencyProperty& property, Binding binding);
```

**参数:**
- `property` - 目标依赖属�?
- `binding` - 绑定配置

**线程安全:** �?

**示例:**
```cpp
control->SetBinding(
    ui::detail::TextBlockBase::TextProperty(),
    Binding()
        .Path("UserName")
        .Mode(BindingMode::OneWay)
);
```

#### ClearBinding()

清除属性的数据绑定

```cpp
void ClearBinding(const DependencyProperty& property);
```

**参数:**
- `property` - 依赖属�?

**线程安全:** �?

#### GetBinding()

获取属性的绑定表达�?

```cpp
std::shared_ptr<BindingExpression> GetBinding(const DependencyProperty& property) const;
```

**返回:** 绑定表达式（如果没有绑定则为 nullptr�?

**线程安全:** �?

### DataContext 方法

#### SetDataContext()

设置数据上下�?

```cpp
void SetDataContext(std::any value);

template<typename T>
void SetDataContext(T&& value);
```

**参数:**
- `value` - 数据上下文对�?

**线程安全:** �?

**注意:** DataContext 会自动继承给子元�?

**示例:**
```cpp
class ViewModel {
public:
    std::string userName = "John";
    int age = 30;
};

auto viewModel = std::make_shared<ViewModel>();
window->SetDataContext(viewModel);

// 子元素可以绑定到 DataContext
textBlock->SetBinding(
    ui::detail::TextBlockBase::TextProperty(),
    Binding().Path("userName")
);
```

#### GetDataContext()

获取数据上下�?

```cpp
const std::any& GetDataContext() const noexcept;
```

**返回:** 数据上下�?

**线程安全:** �?

#### HasDataContext()

检查是否有数据上下�?

```cpp
bool HasDataContext() const noexcept;
```

**返回:** 如果�?DataContext 返回 true

**线程安全:** �?

#### ClearDataContext()

清除数据上下�?

```cpp
void ClearDataContext();
```

**线程安全:** �?

### 逻辑树方�?

#### SetLogicalParent()

设置逻辑父元�?

```cpp
void SetLogicalParent(DependencyObject* parent);
```

**参数:**
- `parent` - 父元素指�?

**线程安全:** �?

#### GetLogicalParent()

获取逻辑父元�?

```cpp
DependencyObject* GetLogicalParent() const noexcept;
```

**返回:** 父元素指�?

**线程安全:** �?

#### AddLogicalChild()

添加逻辑子元�?

```cpp
void AddLogicalChild(DependencyObject* child);
```

**参数:**
- `child` - 子元素指�?

**线程安全:** �?

#### RemoveLogicalChild()

移除逻辑子元�?

```cpp
void RemoveLogicalChild(DependencyObject* child);
```

**参数:**
- `child` - 子元素指�?

**线程安全:** �?

#### GetLogicalChildren()

获取所有逻辑子元�?

```cpp
const std::vector<DependencyObject*>& GetLogicalChildren() const noexcept;
```

**返回:** 子元素列�?

**线程安全:** �?

### 元素名称方法

#### SetElementName()

设置元素名称（用�?ElementName 绑定�?

```cpp
void SetElementName(std::string name);
```

**参数:**
- `name` - 元素名称

**线程安全:** �?

**示例:**
```cpp
auto textBox = std::make_shared<TextBox>();
textBox->SetElementName("nameInput");

// 其他元素可以通过名称绑定
auto label = std::make_shared<TextBlock>();
label->SetBinding(
    ui::detail::TextBlockBase::TextProperty(),
    Binding().ElementName("nameInput").Path("Text")
);
```

#### GetElementName()

获取元素名称

```cpp
const std::string& GetElementName() const noexcept;
```

**返回:** 元素名称

**线程安全:** �?

#### FindElementByName()

在逻辑树中查找指定名称的元�?

```cpp
DependencyObject* FindElementByName(std::string_view name);
const DependencyObject* FindElementByName(std::string_view name) const;
```

**参数:**
- `name` - 要查找的元素名称

**返回:** 找到的元素指针，未找到返�?nullptr

**线程安全:** �?

### 事件

#### PropertyChanged

属性值改变事�?

```cpp
Event<const DependencyProperty&, 
      const std::any&,      // oldValue
      const std::any&,      // newValue
      ValueSource,          // oldSource
      ValueSource> PropertyChanged;  // newSource
```

**示例:**
```cpp
control->PropertyChanged += [](const DependencyProperty& prop,
                                const std::any& oldValue,
                                const std::any& newValue,
                                ValueSource oldSource,
                                ValueSource newSource) {
    std::cout << "Property " << prop.Name() << " changed" << std::endl;
};
```

#### BindingChanged

绑定改变事件

```cpp
Event<const DependencyProperty&,
      const std::shared_ptr<BindingExpression>&,  // oldBinding
      const std::shared_ptr<BindingExpression>&> BindingChanged;  // newBinding
```

#### DataContextChanged

数据上下文改变事�?

```cpp
Event<const std::any&,  // oldValue
      const std::any&> DataContextChanged;  // newValue
```

---

## Binding

**数据绑定配置** - 使用流式 API 配置数据绑定�?

### 类定�?

```cpp
class Binding {
public:
    Binding() = default;
    
    Binding& Path(std::string path);
    Binding& Source(std::any source);
    Binding& Mode(BindingMode mode);
    Binding& ElementName(std::string name);
    Binding& Converter(std::shared_ptr<IValueConverter> converter);
};
```

### 核心方法

#### Path()

设置绑定路径

```cpp
Binding& Path(std::string path);
```

**参数:**
- `path` - 属性路径（支持嵌套，如 "Address.City"�?

**返回:** 自身引用（链式调用）

**示例:**
```cpp
Binding()
    .Path("UserName")
    .Mode(BindingMode::TwoWay)
```

#### Source()

设置绑定源对�?

```cpp
Binding& Source(std::any source);
```

**参数:**
- `source` - 源对�?

**返回:** 自身引用

**注意:** 如果不设�?Source，将使用 DataContext

**示例:**
```cpp
auto viewModel = std::make_shared<ViewModel>();
Binding()
    .Source(viewModel)
    .Path("UserName")
```

#### Mode()

设置绑定模式

```cpp
Binding& Mode(BindingMode mode);
```

**参数:**
- `mode` - 绑定模式

**返回:** 自身引用

#### ElementName()

通过元素名称绑定

```cpp
Binding& ElementName(std::string name);
```

**参数:**
- `name` - 目标元素名称

**返回:** 自身引用

**示例:**
```cpp
// 绑定到另一个元素的属�?
Binding()
    .ElementName("slider1")
    .Path("Value")
```

#### SetRelativeSource()

设置相对�?

```cpp
Binding& SetRelativeSource(RelativeSource relativeSource);
```

**参数:**
- `relativeSource` - 相对源配�?

**返回:** 自身引用

**示例:**
```cpp
// 绑定到自己的属�?
Binding()
    .SetRelativeSource(RelativeSource::Self())
    .Path("Width")

// 绑定到父元素
Binding()
    .SetRelativeSource(
        RelativeSource::FindAncestor(typeid(Panel), 1)
    )
    .Path("Background")
```

#### Converter()

设置值转换器

```cpp
Binding& Converter(std::shared_ptr<IValueConverter> converter);
```

**参数:**
- `converter` - 值转换器

**返回:** 自身引用

**示例:**
```cpp
class BoolToVisibilityConverter : public IValueConverter {
public:
    std::any Convert(const std::any& value, 
                    std::type_index targetType,
                    const std::any* parameter) const override {
        bool isVisible = std::any_cast<bool>(value);
        return isVisible ? Visibility::Visible : Visibility::Collapsed;
    }
    
    std::any ConvertBack(const std::any& value,
                        std::type_index sourceType,
                        const std::any* parameter) const override {
        auto vis = std::any_cast<Visibility>(value);
        return vis == Visibility::Visible;
    }
};

// 使用
Binding()
    .Path("IsEnabled")
    .Converter(std::make_shared<BoolToVisibilityConverter>())
```

#### SetUpdateSourceTrigger()

设置源更新触发器

```cpp
Binding& SetUpdateSourceTrigger(UpdateSourceTrigger trigger);
```

**参数:**
- `trigger` - 更新触发�?

**返回:** 自身引用

### RelativeSource

相对源配�?

```cpp
class RelativeSource {
public:
    static RelativeSource Self();
    static RelativeSource FindAncestor(std::type_index type, int level = 1);
};
```

**方法:**

- `Self()` - 绑定到元素自�?
- `FindAncestor(type, level)` - 查找指定类型的祖先元�?
  - `type` - 祖先类型
  - `level` - 层级�? 表示父元素，2 表示祖父元素，以此类推）

---

## ObservableObject

**可观察对�?* - MVVM 模式中的 ViewModel 基类�?

### 类定�?

```cpp
class ObservableObject : public INotifyPropertyChanged {
public:
    core::Event<std::string_view> PropertyChanged;
    
protected:
    template<typename T>
    bool SetProperty(T& field, T value, std::string_view propertyName);
    
    void OnPropertyChanged(std::string_view propertyName);
};
```

### 核心方法

#### SetProperty()

设置属性并触发通知

```cpp
template<typename T>
bool SetProperty(T& field, T value, std::string_view propertyName);
```

**参数:**
- `field` - 字段引用
- `value` - 新�?
- `propertyName` - 属性名�?

**返回:** 如果值改变返�?true

**线程安全:** �?

**示例:**
```cpp
class UserViewModel : public ObservableObject {
public:
    void SetUserName(const std::string& value) {
        if (SetProperty(userName_, value, "UserName")) {
            // 值已改变，PropertyChanged 事件已触�?
        }
    }
    
    const std::string& GetUserName() const { return userName_; }
    
private:
    std::string userName_;
};
```

#### OnPropertyChanged()

手动触发属性变更通知

```cpp
void OnPropertyChanged(std::string_view propertyName);
```

**参数:**
- `propertyName` - 变更的属性名�?

**线程安全:** �?

**示例:**
```cpp
void UpdateCalculatedProperty() {
    // 更新计算属性后通知
    OnPropertyChanged("FullName");
}
```

### 使用示例

```cpp
class PersonViewModel : public ObservableObject {
public:
    // 属�?Setter/Getter
    void SetFirstName(const std::string& value) {
        if (SetProperty(firstName_, value, "FirstName")) {
            OnPropertyChanged("FullName");  // 通知依赖属�?
        }
    }
    
    const std::string& GetFirstName() const { return firstName_; }
    
    void SetLastName(const std::string& value) {
        if (SetProperty(lastName_, value, "LastName")) {
            OnPropertyChanged("FullName");
        }
    }
    
    const std::string& GetLastName() const { return lastName_; }
    
    std::string GetFullName() const {
        return firstName_ + " " + lastName_;
    }
    
private:
    std::string firstName_;
    std::string lastName_;
};

// 使用
auto person = std::make_shared<PersonViewModel>();
window->SetDataContext(person);

// 绑定
textBlock->SetBinding(
    ui::detail::TextBlockBase::TextProperty(),
    Binding().Path("FullName")
);

// 修改数据，UI 自动更新
person->SetFirstName("John");
person->SetLastName("Doe");
```

---

## INotifyPropertyChanged

**属性变更通知接口** - 用于 MVVM 模式�?

### 接口定义

```cpp
class INotifyPropertyChanged {
public:
    virtual ~INotifyPropertyChanged() = default;
    
    core::Event<std::string_view> PropertyChanged;
};
```

### 实现示例

```cpp
class MyModel : public INotifyPropertyChanged {
public:
    void SetValue(int value) {
        if (value_ != value) {
            value_ = value;
            PropertyChanged.Invoke("Value");
        }
    }
    
    int GetValue() const { return value_; }
    
private:
    int value_{0};
};
```

---

## IValueConverter

**值转换器接口** - 在绑定中转换值类型�?

### 接口定义

```cpp
class IValueConverter {
public:
    virtual ~IValueConverter() = default;
    
    virtual std::any Convert(
        const std::any& value,
        std::type_index targetType,
        const std::any* parameter
    ) const = 0;
    
    virtual std::any ConvertBack(
        const std::any& value,
        std::type_index sourceType,
        const std::any* parameter
    ) const = 0;
};
```

### 实现示例

```cpp
// 数字到字符串转换�?
class NumberToStringConverter : public IValueConverter {
public:
    std::any Convert(const std::any& value,
                    std::type_index targetType,
                    const std::any* parameter) const override {
        if (value.type() == typeid(int)) {
            return std::to_string(std::any_cast<int>(value));
        } else if (value.type() == typeid(float)) {
            return std::to_string(std::any_cast<float>(value));
        }
        return std::string{};
    }
    
    std::any ConvertBack(const std::any& value,
                        std::type_index sourceType,
                        const std::any* parameter) const override {
        std::string str = std::any_cast<std::string>(value);
        if (sourceType == typeid(int)) {
            return std::stoi(str);
        } else if (sourceType == typeid(float)) {
            return std::stof(str);
        }
        return std::any{};
    }
};

// 使用
control->SetBinding(
    ui::detail::TextBlockBase::TextProperty(),
    Binding()
        .Path("Age")
        .Converter(std::make_shared<NumberToStringConverter>())
);
```

---

## 完整示例

### MVVM 模式示例

```cpp
// ViewModel
class TodoViewModel : public ObservableObject {
public:
    void SetTask(const std::string& value) {
        SetProperty(task_, value, "Task");
    }
    
    const std::string& GetTask() const { return task_; }
    
    void SetIsCompleted(bool value) {
        SetProperty(isCompleted_, value, "IsCompleted");
    }
    
    bool IsCompleted() const { return isCompleted_; }
    
private:
    std::string task_;
    bool isCompleted_{false};
};

// View 设置
auto viewModel = std::make_shared<TodoViewModel>();
window->SetDataContext(viewModel);

auto textBox = std::make_shared<TextBox>();
textBox->SetBinding(
    TextBox::TextProperty(),
    Binding()
        .Path("Task")
        .Mode(BindingMode::TwoWay)
);

auto checkbox = std::make_shared<CheckBox>();
checkbox->SetBinding(
    CheckBox::IsCheckedProperty(),
    Binding()
        .Path("IsCompleted")
        .Mode(BindingMode::TwoWay)
);
```

---

## 线程安全说明

| �?| 线程安全�?|
|---|---|
| DependencyProperty | Register/RegisterAttached 线程安全 |
| DependencyObject | 所有方法必须在 UI 线程调用 |
| Binding | 配置方法线程安全，但应在设置前完成配�?|
| ObservableObject | 必须在创建它的线程访�?|

---

## 最佳实�?

### 依赖属�?

1. **使用静态方法返回依赖属�?*
   ```cpp
   static const DependencyProperty& MyProperty() {
       static const auto& prop = DependencyProperty::Register(...);
       return prop;
   }
   ```

2. **提供类型安全的包装方�?*
   ```cpp
   void SetMyValue(int value) {
       SetValue(MyProperty(), value);
   }
   
   int GetMyValue() const {
       return GetValue<int>(MyProperty());
   }
   ```

3. **验证属性�?*
   ```cpp
   PropertyMetadata{
       .validateCallback = [](const std::any& value) {
           float v = std::any_cast<float>(value);
           return v >= 0.0f && v <= 1.0f;
       }
   }
   ```

### 数据绑定

1. **使用 TwoWay 绑定用于输入控件**
   ```cpp
   textBox->SetBinding(
       TextBox::TextProperty(),
       Binding().Path("UserName").Mode(BindingMode::TwoWay)
   );
   ```

2. **使用转换器转换数据类�?*
   ```cpp
   Binding()
       .Path("IsEnabled")
       .Converter(std::make_shared<BoolToVisibilityConverter>())
   ```

3. **使用 ElementName 绑定控件间关�?*
   ```cpp
   label->SetBinding(
       ui::detail::TextBlockBase::TextProperty(),
       Binding().ElementName("inputBox").Path("Text")
   );
   ```

### MVVM 模式

1. **ViewModel 继承�?ObservableObject**
   ```cpp
   class MyViewModel : public ObservableObject {
       // 使用 SetProperty 自动触发通知
   };
   ```

2. **�?Window 或根元素设置 DataContext**
   ```cpp
   window->SetDataContext(std::make_shared<MainViewModel>());
   ```

3. **保持 ViewModel 独立�?View**
   - ViewModel 不应引用 UI 元素
   - 使用 Command 模式处理用户操作
