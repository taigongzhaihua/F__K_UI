# F__K_UI MVVM 数据绑定工作流程详解

## 📋 目录

1. [绑定系统架构概览](#绑定系统架构概览)
2. [核心组件职责](#核心组件职责)
3. [完整工作流程](#完整工作流程)
4. [属性注册机制](#属性注册机制)
5. [绑定创建流程](#绑定创建流程)
6. [数据更新流程](#数据更新流程)
7. [生命周期管理](#生命周期管理)
8. [时序图](#时序图)

---

## 绑定系统架构概览

```
┌─────────────────────────────────────────────────────────────────┐
│                         用户代码层                               │
├─────────────────────────────────────────────────────────────────┤
│  ViewModel (ObservableObject)  ←→  UI Control (DependencyObject)│
└─────────────────────────────────────────────────────────────────┘
                    ↓                           ↓
┌─────────────────────────────────────────────────────────────────┐
│                        绑定基础设施                               │
├─────────────────────────────────────────────────────────────────┤
│  PropertyAccessorRegistry  ←  宏自动注册 (FK_VIEWMODEL_AUTO)    │
│  BindingExpression        ←  绑定逻辑核心                        │
│  BindingPath              ←  属性路径解析                        │
│  BindingContext           ←  DataContext 管理                   │
│  PropertyStore            ←  属性值存储                          │
└─────────────────────────────────────────────────────────────────┘
                    ↓
┌─────────────────────────────────────────────────────────────────┐
│                        事件系统                                  │
├─────────────────────────────────────────────────────────────────┤
│  PropertyChanged Event    ←  ViewModel → BindingExpression      │
│  DependencyProperty Event ←  UI → BindingExpression             │
└─────────────────────────────────────────────────────────────────┘
```

---

## 核心组件职责

### 1. **ViewModel 层**

#### `ObservableObject` (ViewModel 基类)
```cpp
class ObservableObject : public INotifyPropertyChanged {
protected:
    void RaisePropertyChanged(std::string_view propertyName);
private:
    PropertyChangedEvent propertyChanged_;
};
```

**职责**：
- 提供 `PropertyChanged` 事件
- 当属性值变化时触发事件
- 通知所有订阅者（BindingExpression）

**使用示例**：
```cpp
void SetCount(int value) {
    if (count_ != value) {
        count_ = value;
        RaisePropertyChanged("Count");  // 👈 触发通知
    }
}
```

---

### 2. **属性注册系统**

#### `PropertyAccessorRegistry` (全局单例)
```cpp
class PropertyAccessorRegistry {
public:
    template<typename TOwner>
    static void RegisterPropertyGetter(const std::string& name, GetterFunc getter);
    
    template<typename TOwner>
    static std::shared_ptr<Accessor> FindAccessor(const std::string& name);
};
```

**职责**：
- 维护 `类型 → 属性名 → 访问器` 的映射
- 提供属性 getter/setter 的反射能力
- 支持在运行时通过字符串访问属性

**数据结构**：
```cpp
std::unordered_map<
    std::type_index,  // 类型（如 INotifyPropertyChanged）
    std::unordered_map<
        std::string,   // 属性名（如 "Count"）
        Accessor       // 访问器（lambda 包装器）
    >
>
```

**注册示例**（通过宏自动完成）：
```cpp
FK_VIEWMODEL_BEGIN(CounterViewModel)
    FK_VIEWMODEL_PROPERTY("Count", &CounterViewModel::GetCount)
FK_VIEWMODEL_END()

// 展开为：
PropertyAccessorRegistry::RegisterPropertyGetter<INotifyPropertyChanged>(
    "Count",
    [](const INotifyPropertyChanged& obj) -> int {
        return static_cast<const CounterViewModel&>(obj).GetCount();
    }
);
```

---

### 3. **绑定定义**

#### `Binding` (绑定配置)
```cpp
class Binding {
public:
    Binding& Path(std::string path);
    Binding& Source(std::any source);
    Binding& Mode(BindingMode mode);
    Binding& Converter(std::shared_ptr<IValueConverter> converter);
};
```

**职责**：
- 存储绑定配置（路径、模式、源等）
- 使用 Fluent API 风格构建绑定
- 创建 `BindingExpression` 实例

**使用示例**：
```cpp
textBlock->SetBinding(
    TextBlock::TextProperty(),
    binding::Binding()
        .Path("Count")
        .Mode(BindingMode::OneWay)
);
```

---

### 4. **绑定表达式**

#### `BindingExpression` (绑定逻辑核心)
```cpp
class BindingExpression {
public:
    void Activate();           // 激活绑定
    void Detach();            // 分离绑定
    void UpdateTarget();      // 更新目标（UI）
    void UpdateSource();      // 更新源（ViewModel）
};
```

**职责**：
- 管理绑定的生命周期
- 订阅 ViewModel 的 `PropertyChanged` 事件
- 解析属性路径并获取值
- 将值应用到目标属性
- 处理双向绑定的反向更新

**关键成员**：
```cpp
private:
    Binding definition_;                    // 绑定定义
    BindingPath path_;                     // 属性路径
    DependencyObject* target_;             // 目标对象（UI）
    const DependencyProperty* property_;   // 目标属性
    std::any currentSource_;               // 当前数据源
    bool isActive_{false};                 // 是否激活
    EventConnection sourcePropertyChangedConnection_;  // 事件连接
```

---

### 5. **路径解析**

#### `BindingPath` (属性路径解析器)
```cpp
class BindingPath {
public:
    bool Resolve(const std::any& source, std::any& outValue) const;
    bool IsEmpty() const;
};
```

**职责**：
- 解析属性路径字符串（如 `"FirstName"`, `"Person.Age"`）
- 通过 `PropertyAccessorRegistry` 查找访问器
- 调用 getter 获取属性值
- 支持嵌套路径和索引访问

**解析流程**：
```
"Count" → PropertyAccessorRegistry::FindAccessor("Count")
        → accessor->Get(source)
        → 返回值
```

---

### 6. **UI 层**

#### `DependencyObject` (UI 控件基类)
```cpp
class DependencyObject {
public:
    void SetValue(const DependencyProperty& property, std::any value);
    const std::any& GetValue(const DependencyProperty& property) const;
    void SetBinding(const DependencyProperty& property, Binding binding);
    void SetDataContext(std::any value);
};
```

**职责**：
- 管理依赖属性值
- 维护 DataContext
- 管理属性绑定
- 触发属性变化通知

**内部组件**：
```cpp
private:
    PropertyStore propertyStore_;        // 存储属性值
    BindingContext bindingContext_;      // 管理 DataContext
    std::unordered_map<...> bindings_;   // 属性 → BindingExpression
```

---

## 完整工作流程

### 阶段 1：初始化（程序启动时）

```
1. 编译期：宏展开
   FK_VIEWMODEL_BEGIN(CounterViewModel)
       FK_VIEWMODEL_PROPERTY("Count", &CounterViewModel::GetCount)
   FK_VIEWMODEL_END()
   ↓
   生成静态初始化代码

2. 运行期：全局对象初始化
   static Registrar instance;
   ↓
   构造函数执行
   ↓
   PropertyAccessorRegistry::RegisterPropertyGetter<INotifyPropertyChanged>(
       "Count",
       [](const Base& obj) { return static_cast<const CounterViewModel&>(obj).GetCount(); }
   )
   ↓
   注册完成！现在可以通过字符串 "Count" 访问属性
```

**关键点**：
- 注册发生在 `main()` 函数之前
- 使用静态初始化确保注册顺序
- 注册到基类 `INotifyPropertyChanged`（不是派生类）

---

### 阶段 2：创建绑定

#### 用户代码

```cpp
// 1. 创建 ViewModel
auto viewModel = std::make_shared<CounterViewModel>();

// 2. 创建 UI 控件
auto textBlock = ui::textBlock();

// 3. 设置 DataContext
window->SetDataContext(
    std::static_pointer_cast<INotifyPropertyChanged>(viewModel)
);

// 4. 创建绑定
textBlock->SetBinding(
    TextBlock::TextProperty(),
    binding::Binding().Path("Count")
);
```

#### 内部流程

```
Step 1: window->SetDataContext(viewModel)
   ↓
   DependencyObject::SetDataContext()
   ↓
   bindingContext_.SetLocalDataContext(viewModel)
   ↓
   触发 DataContextChanged 事件
   ↓
   子控件继承 DataContext（如果没有本地值）

Step 2: textBlock->SetBinding(TextBlock::TextProperty(), binding)
   ↓
   DependencyObject::SetBinding()
   ↓
   binding.CreateExpression(this, property)
   ↓
   创建 BindingExpression(binding, target=textBlock, property=TextProperty)
   ↓
   propertyStore_.SetBinding(property, expression)
   ↓
   expression->Activate()
```

---

### 阶段 3：激活绑定

#### `BindingExpression::Activate()`

```cpp
void BindingExpression::Activate() {
    if (isActive_) return;
    
    isActive_ = true;
    
    // 1. 订阅 PropertyChanged 事件
    Subscribe();
    
    // 2. 首次更新目标
    UpdateTarget();
}
```

#### `Subscribe()` - 订阅事件

```cpp
void BindingExpression::Subscribe() {
    // 1. 解析数据源（从 DataContext 获取）
    std::any sourceRoot = ResolveSourceRoot();
    
    // 2. 尝试转换为 INotifyPropertyChanged
    auto notifier = TryGetNotifier(sourceRoot);
    
    if (!notifier) return;
    
    // 3. 订阅 PropertyChanged 事件
    sourcePropertyChangedConnection_ = notifier->PropertyChanged().Connect(
        [this](std::string_view propertyName) {
            OnSourcePropertyChanged(propertyName);  // 👈 事件处理
        }
    );
}
```

#### `ResolveSourceRoot()` - 获取数据源

```cpp
std::any BindingExpression::ResolveSourceRoot() {
    // 1. 如果有显式指定的 Source
    if (definition_.HasExplicitSource()) {
        return definition_.GetSource();
    }
    
    // 2. 否则从 DataContext 获取
    return target_->GetDataContext();
}
```

---

### 阶段 4：初始更新

#### `BindingExpression::UpdateTarget()`

```cpp
void BindingExpression::UpdateTarget() {
    // 1. 获取数据源
    std::any sourceRoot = ResolveSourceRoot();
    
    // 2. 解析属性路径
    std::any resolvedValue;
    bool resolved = path_.Resolve(sourceRoot, resolvedValue);
    
    if (!resolved) {
        ApplyTargetValue(std::any{});  // 应用默认值
        return;
    }
    
    // 3. 应用值转换器（如果有）
    std::any value = resolvedValue;
    if (auto converter = definition_.GetConverter()) {
        value = converter->Convert(value, property_->PropertyType(), ...);
    }
    
    // 4. 应用到目标属性
    ApplyTargetValue(value);
}
```

#### `BindingPath::Resolve()` - 路径解析

```cpp
bool BindingPath::Resolve(const std::any& source, std::any& outValue) const {
    // 1. 尝试转换为 shared_ptr<INotifyPropertyChanged>
    auto sourcePtr = std::any_cast<std::shared_ptr<INotifyPropertyChanged>>(&source);
    if (!sourcePtr || !*sourcePtr) {
        return false;
    }
    
    // 2. 从 PropertyAccessorRegistry 查找访问器
    auto accessor = PropertyAccessorRegistry::FindAccessor<INotifyPropertyChanged>(
        "Count"  // path_
    );
    
    if (!accessor) {
        return false;  // 属性未注册！
    }
    
    // 3. 调用 getter
    outValue = accessor->Get(**sourcePtr);
    
    return true;
}
```

#### `ApplyTargetValue()` - 应用值到 UI

```cpp
void BindingExpression::ApplyTargetValue(std::any value) {
    // 1. 设置标志，防止循环更新
    ScopedFlag guard(isUpdatingTarget_);
    
    // 2. 调用 DependencyObject::ApplyBindingValue()
    target_->ApplyBindingValue(*property_, std::move(value));
}
```

#### `DependencyObject::ApplyBindingValue()`

```cpp
void DependencyObject::ApplyBindingValue(
    const DependencyProperty& property,
    std::any value
) {
    // 1. 存储到 PropertyStore（标记为 Binding 源）
    propertyStore_.SetValue(property, std::move(value), ValueSource::Binding);
    
    // 2. 触发属性变化回调
    OnPropertyChanged(property, oldValue, newValue);
}
```

#### 控件属性变化回调

```cpp
// TextBlock 示例
void TextBlock::OnTextChanged(const std::any& oldValue, const std::any& newValue) {
    // 1. 提取新值
    std::string newText = std::any_cast<std::string>(newValue);
    
    // 2. 更新内部状态
    text_ = newText;
    
    // 3. 触发重绘
    InvalidateVisual();  // 标记需要重新渲染
}
```

**至此，UI 已经显示了 ViewModel 的初始值！**

---

### 阶段 5：响应属性变化

#### 用户操作

```cpp
// 用户点击按钮
incrementBtn->Click += [viewModel](ButtonBase&) {
    viewModel->Increment();  // 调用 ViewModel 方法
};

// ViewModel 方法
void CounterViewModel::Increment() {
    SetCount(count_ + 1);  // 修改属性
}

void CounterViewModel::SetCount(int value) {
    if (count_ != value) {
        count_ = value;
        RaisePropertyChanged("Count");  // 👈 触发事件
    }
}
```

#### 事件传播流程

```
1. RaisePropertyChanged("Count")
   ↓
   propertyChanged_("Count")
   ↓
   所有订阅者收到通知

2. BindingExpression::OnSourcePropertyChanged("Count")
   ↓
   检查：propertyName == "Count"？
   ↓
   是！调用 UpdateTarget()

3. UpdateTarget()
   ↓
   path_.Resolve(sourceRoot, resolvedValue)  // 重新获取值
   ↓
   ApplyTargetValue(resolvedValue)
   ↓
   propertyStore_.SetValue(property, value, ValueSource::Binding)

4. TextBlock::OnTextChanged()
   ↓
   text_ = newValue
   ↓
   InvalidateVisual()

5. 渲染循环
   ↓
   检测到 isVisualDirty_ = true
   ↓
   重新渲染文本
   ↓
   UI 更新完成！✅
```

---

### 阶段 6：双向绑定（可选）

#### 用户输入

```cpp
// TextBox 示例（支持双向绑定）
textBox->SetBinding(
    TextBox::TextProperty(),
    binding::Binding().Path("UserName").Mode(BindingMode::TwoWay)
);

// 用户在文本框输入 "Alice"
// ↓
// TextBox::OnTextChanged()
void TextBox::OnTextInput(const std::string& newText) {
    // 1. 更新本地值
    SetValue(TextProperty(), newText);
    
    // 2. 触发绑定更新源
    // （PropertyStore 检测到 Local 值变化，通知 BindingExpression）
}
```

#### 反向更新流程

```
1. PropertyStore::SetValue(property, newText, ValueSource::Local)
   ↓
   检测到该属性有绑定
   ↓
   通知 BindingExpression

2. BindingExpression::OnTargetPropertyChanged()
   ↓
   检查：effectiveMode_ 支持 TwoWay？
   ↓
   是！调用 UpdateSource()

3. UpdateSource()
   ↓
   获取目标值：target_->GetValue(*property_)
   ↓
   通过 PropertyAccessorRegistry 查找 setter
   ↓
   accessor->Set(source, value)

4. ViewModel::SetUserName("Alice")
   ↓
   userName_ = "Alice"
   ↓
   RaisePropertyChanged("UserName")
   ↓
   （可能触发其他依赖属性更新）
```

---

## 属性注册机制详解

### 为什么需要注册？

C++ 没有原生反射，无法在运行时通过字符串 `"Count"` 访问对象的 `GetCount()` 方法。

**解决方案**：手动建立映射表。

### 注册流程

#### 1. 宏展开

```cpp
FK_VIEWMODEL_BEGIN(CounterViewModel)
    FK_VIEWMODEL_PROPERTY("Count", &CounterViewModel::GetCount)
FK_VIEWMODEL_END()
```

**展开为**：

```cpp
namespace {
    struct ViewModelRegistrar_98 {  // 98 = __LINE__
        ViewModelRegistrar_98() {
            using VM = CounterViewModel;
            using Base = ::fk::binding::INotifyPropertyChanged;
            
            // 注册 "Count" 属性
            ::fk::binding::PropertyAccessorRegistry::RegisterPropertyGetter<Base>(
                "Count",
                [](const Base& obj) -> decltype((std::declval<VM>().*(&VM::GetCount))()) {
                    return (static_cast<const VM&>(obj).*(&VM::GetCount))();
                }
            );
        }
    };
    
    // 创建静态实例，触发构造函数
    static ViewModelRegistrar_98 gViewModelRegistrarInstance_98;
}
```

#### 2. 注册函数

```cpp
template<typename TOwner>
void PropertyAccessorRegistry::RegisterPropertyGetter(
    const std::string& name,
    GetterFunc getter
) {
    // 1. 获取全局存储
    auto& storage = GetStorage();  // 静态单例
    
    // 2. 加锁（线程安全）
    std::lock_guard lock(storage.mutex);
    
    // 3. 按类型和名称存储
    auto& ownerMap = storage.accessors[typeid(TOwner)];
    ownerMap[name] = Accessor{getter, nullptr};
}
```

#### 3. 查找访问器

```cpp
template<typename TOwner>
std::shared_ptr<Accessor> PropertyAccessorRegistry::FindAccessor(
    const std::string& name
) {
    auto& storage = GetStorage();
    std::lock_guard lock(storage.mutex);
    
    // 1. 查找类型映射
    auto typeIt = storage.accessors.find(typeid(TOwner));
    if (typeIt == storage.accessors.end()) {
        return nullptr;  // 类型未注册
    }
    
    // 2. 查找属性映射
    auto& ownerMap = typeIt->second;
    auto propIt = ownerMap.find(name);
    if (propIt == ownerMap.end()) {
        return nullptr;  // 属性未注册
    }
    
    // 3. 返回访问器
    return std::make_shared<Accessor>(propIt->second);
}
```

### 关键设计决策

#### 为什么注册到基类？

```cpp
// ❌ 错误：注册到派生类
RegisterPropertyGetter<CounterViewModel>("Count", ...);

// ✅ 正确：注册到基类
RegisterPropertyGetter<INotifyPropertyChanged>("Count", ...);
```

**原因**：

1. **DataContext 存储的是基类指针**：
   ```cpp
   std::any dataContext = std::static_pointer_cast<INotifyPropertyChanged>(viewModel);
   ```

2. **`std::any_cast` 不支持多态转换**：
   ```cpp
   // DataContext 中存储的类型是 shared_ptr<INotifyPropertyChanged>
   auto ptr = std::any_cast<std::shared_ptr<CounterViewModel>>(&dataContext);
   // ❌ 失败！类型不匹配
   ```

3. **查找访问器时使用的是运行时类型**：
   ```cpp
   auto accessor = FindAccessor<INotifyPropertyChanged>("Count");
   // 必须与存储时的类型一致
   ```

#### Lambda 包装器的作用

```cpp
[](const INotifyPropertyChanged& obj) -> int {
    return static_cast<const CounterViewModel&>(obj).GetCount();
}
```

**作用**：
1. **类型桥接**：`INotifyPropertyChanged` → `CounterViewModel`
2. **调用具体方法**：`GetCount()`
3. **类型安全**：返回值类型自动推导

---

## 生命周期管理

### 绑定生命周期

```
创建 → 激活 → 运行 → 分离 → 销毁
  ↓      ↓      ↓      ↓      ↓
Create Activate Run  Detach Destroy
```

#### 1. 创建

```cpp
auto expression = binding.CreateExpression(target, property);
// BindingExpression 对象创建，但未激活
```

#### 2. 激活

```cpp
expression->Activate();
// - 订阅 PropertyChanged 事件
// - 执行首次 UpdateTarget()
```

#### 3. 运行

```cpp
// 响应事件
OnSourcePropertyChanged() → UpdateTarget()
OnTargetPropertyChanged() → UpdateSource()
```

#### 4. 分离

```cpp
expression->Detach();
// - 取消事件订阅
// - 停止更新
```

#### 5. 销毁

```cpp
// BindingExpression 智能指针引用计数归零
~BindingExpression()
```

### 自动分离时机

```cpp
// 1. 控件被移除
window->RemoveChild(textBlock);
// ↓ textBlock 的所有绑定自动分离

// 2. 重新设置绑定
textBlock->SetBinding(TextProperty(), newBinding);
// ↓ 旧绑定自动分离，新绑定激活

// 3. 清除绑定
textBlock->ClearBinding(TextProperty());
// ↓ 绑定分离

// 4. 控件销毁
textBlock.reset();
// ↓ 析构函数自动分离所有绑定
```

---

## 时序图

### 完整绑定流程时序图

```
用户代码        DependencyObject    BindingExpression    PropertyAccessorRegistry    ViewModel
   |                   |                    |                      |                       |
   |-- SetBinding() -->|                    |                      |                       |
   |                   |-- CreateExpression() -->                  |                       |
   |                   |                    |                      |                       |
   |                   |-- Activate() ----->|                      |                       |
   |                   |                    |-- ResolveSourceRoot() ->                      |
   |                   |                    |  (获取 DataContext)  |                       |
   |                   |                    |<---------------------                        |
   |                   |                    |                      |                       |
   |                   |                    |-- Subscribe() -------|-------- 订阅 -------->|
   |                   |                    |                      |   PropertyChanged     |
   |                   |                    |                      |                       |
   |                   |                    |-- UpdateTarget() --->|                       |
   |                   |                    |                      |                       |
   |                   |                    |-- path_.Resolve() -->|                       |
   |                   |                    |                      |-- FindAccessor() ---->|
   |                   |                    |                      |<----------------------|
   |                   |                    |                      |                       |
   |                   |                    |                      |-- accessor->Get() --->|
   |                   |                    |                      |                   GetCount()
   |                   |                    |                      |<--- 返回值 -----------|
   |                   |                    |<--- 返回值 ----------|                       |
   |                   |                    |                      |                       |
   |                   |<-- ApplyBindingValue() --|                |                       |
   |                   |-- OnPropertyChanged() ---|                |                       |
   |                   |                    |                      |                       |
   |<--- UI 更新 ------|                    |                      |                       |
   |                   |                    |                      |                       |
   
   【用户点击按钮】
   |                   |                    |                      |                       |
   |-- Increment() ----|--------------------|--------------------- | --------------------->|
   |                   |                    |                      |                  SetCount()
   |                   |                    |                      |            RaisePropertyChanged()
   |                   |                    |<----- PropertyChanged("Count") --------------|
   |                   |                    |-- OnSourcePropertyChanged() ->               |
   |                   |                    |                      |                       |
   |                   |                    |-- UpdateTarget() --->|                       |
   |                   |                    |  (重复上述流程)      |                       |
   |                   |                    |                      |                       |
   |<--- UI 更新 ------|<-------------------|                      |                       |
```

---

## 常见问题排查

### 问题 1：绑定不工作（UI 不更新）

**检查清单**：

1. ✅ **属性是否已注册？**
   ```cpp
   FK_VIEWMODEL_AUTO(MyViewModel, PropertyName)
   ```

2. ✅ **DataContext 是否正确设置？**
   ```cpp
   window->SetDataContext(std::static_pointer_cast<INotifyPropertyChanged>(viewModel));
   ```

3. ✅ **绑定路径是否正确？**
   ```cpp
   .Path("Count")  // 必须与注册时的名称一致
   ```

4. ✅ **是否调用了 RaisePropertyChanged？**
   ```cpp
   void SetCount(int value) {
       count_ = value;
       RaisePropertyChanged("Count");  // 必须！
   }
   ```

5. ✅ **getter 方法是否 public？**
   ```cpp
   public:  // ✅
       int GetCount() const { return count_; }
   ```

### 问题 2：循环更新/栈溢出

**原因**：属性相互依赖形成循环。

```cpp
// ❌ 错误
FK_PROPERTY_CUSTOM(int, A, {
    RaisePropertyChanged("B");
})
FK_PROPERTY_CUSTOM(int, B, {
    RaisePropertyChanged("A");  // 循环！
})
```

**解决**：使用单向依赖图。

### 问题 3：计算属性不更新

**原因**：忘记通知依赖属性。

```cpp
// ❌ 错误
void SetFirstName(const std::string& value) {
    firstName_ = value;
    RaisePropertyChanged("FirstName");
    // FullName 也变了，但没有通知！
}

// ✅ 正确
FK_PROPERTY_CUSTOM(std::string, FirstName, {
    RaisePropertyChanged("FullName");  // 通知依赖属性
})
```

---

## 性能优化建议

### 1. 避免不必要的通知

```cpp
void SetCount(int value) {
    if (count_ != value) {  // ✅ 先检查
        count_ = value;
        RaisePropertyChanged("Count");
    }
}
```

### 2. 批量更新

```cpp
void UpdateMultipleProperties() {
    BeginUpdate();  // 暂停通知
    SetFirstName("John");
    SetLastName("Doe");
    SetAge(30);
    EndUpdate();    // 一次性通知
}
```

### 3. 使用弱引用

```cpp
// 避免循环引用导致内存泄漏
std::weak_ptr<ViewModel> weakVM = viewModel;
button->Click += [weakVM]() {
    if (auto vm = weakVM.lock()) {
        vm->Increment();
    }
};
```

---

## 总结

F__K_UI 的数据绑定系统核心流程：

1. **编译期**：宏展开 → 生成注册代码
2. **启动期**：静态初始化 → 注册属性访问器
3. **运行期**：
   - 创建绑定 → 激活 → 订阅事件
   - 属性变化 → 触发事件 → 更新 UI
   - UI 输入 → 反向更新 → 更新 ViewModel

**关键特性**：
- ✅ 声明式绑定（Fluent API）
- ✅ 自动属性注册（宏系统）
- ✅ 事件驱动更新
- ✅ 支持双向绑定
- ✅ 类型安全的反射
- ✅ 自动生命周期管理

**性能**：
- 注册开销：编译期 + 启动期一次性
- 运行时开销：事件分发 + 哈希查找 + 函数调用
- 零运行时反射开销（预注册）

现在您已经完全理解了 F__K_UI 的 MVVM 绑定机制！🎉
