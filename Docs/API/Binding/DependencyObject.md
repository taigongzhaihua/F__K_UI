# DependencyObject

## 概览

**目的**：支持依赖属性、数据绑定和逻辑树关系的对象基类。

**命名空间**：`fk::binding`

**继承**：无（根基类）

**头文件**：`fk/binding/DependencyObject.h`

## 描述

`DependencyObject` 是 F__K_UI 属性系统的基础基类。它为以下内容提供基础设施：

- **依赖属性**：具有元数据、变更通知和值强制转换的属性
- **数据绑定**：属性和数据源之间的自动同步
- **逻辑树**：用于元素名称解析和 DataContext 继承的父子关系
- **值来源**：跟踪属性值来源（本地、绑定、默认值等）

所有 UI 元素和许多框架类都派生自 `DependencyObject`。

## 公共接口

### 属性值管理

#### GetValue
```cpp
const std::any& GetValue(const DependencyProperty& property) const;

template<typename T>
T GetValue(const DependencyProperty& property) const;
```
获取依赖属性的当前有效值。

**参数**：
- `property`：要查询的依赖属性

**返回**：属性值（类型化或作为 `std::any`）

**示例**：
```cpp
double width = element->GetValue<double>(UIElement::WidthProperty());
```

#### SetValue
```cpp
void SetValue(const DependencyProperty& property, std::any value);

template<typename T>
void SetValue(const DependencyProperty& property, T&& value);

void SetValue(const DependencyProperty& property, Binding binding);
```
设置依赖属性的本地值。

**参数**：
- `property`：要设置的依赖属性
- `value`：新值（或用于数据绑定的 Binding）

**示例**：
```cpp
element->SetValue(UIElement::WidthProperty(), 200.0);
// 或使用绑定：
element->SetValue(UIElement::WidthProperty(), Binding("Width"));
```

#### ClearValue
```cpp
void ClearValue(const DependencyProperty& property);
```
清除本地值，恢复为默认值或继承值。

#### GetValueSource
```cpp
ValueSource GetValueSource(const DependencyProperty& property) const;
```
返回当前值的来源（Local、Default、Binding 等）。

### 逻辑树

#### SetLogicalParent / GetLogicalParent
```cpp
void SetLogicalParent(DependencyObject* parent);
DependencyObject* GetLogicalParent() const;
```
管理逻辑父子关系。

#### AddLogicalChild / RemoveLogicalChild
```cpp
void AddLogicalChild(DependencyObject* child);
void RemoveLogicalChild(DependencyObject* child);
```
管理逻辑子元素。

### DataContext

#### DataContext
```cpp
static const DependencyProperty& DataContextProperty();
```
数据绑定上下文属性。沿逻辑树继承。

**示例**：
```cpp
// 设置数据上下文
window->SetValue(FrameworkElement::DataContextProperty(), viewModel);

// 子元素自动继承
textBlock->SetValue(TextBlock::TextProperty(), Binding("Username"));
```

### 属性变更通知

#### PropertyChanged
```cpp
core::Event<const DependencyPropertyChangedEventArgs&> PropertyChanged;
```
任何依赖属性变更时触发的事件。

**示例**：
```cpp
obj->PropertyChanged += [](const DependencyPropertyChangedEventArgs& e) {
    std::cout << "属性 " << e.Property.Name() << " 变更" << std::endl;
};
```

## 值优先级

属性值按优先级确定：

1. **动画值**（尚未实现）
2. **本地值** - 通过 SetValue() 设置
3. **模板父值**（尚未实现）
4. **样式值**（部分实现）
5. **继承值** - 从父元素继承
6. **默认值** - 在属性元数据中定义

## 使用示例

### 基本属性操作

```cpp
// 创建对象
auto element = std::make_shared<UIElement>();

// 设置值
element->SetValue(UIElement::WidthProperty(), 200.0);
element->SetValue(UIElement::HeightProperty(), 100.0);

// 获取值
double width = element->GetValue<double>(UIElement::WidthProperty());

// 清除值
element->ClearValue(UIElement::WidthProperty());

// 检查值来源
auto source = element->GetValueSource(UIElement::WidthProperty());
if (source == ValueSource::Local) {
    std::cout << "本地设置的值" << std::endl;
}
```

### 数据绑定

```cpp
// 创建 ViewModel
class MyViewModel : public ObservableObject {
    PROPERTY(std::string, Username, "")
};

auto viewModel = std::make_shared<MyViewModel>();
viewModel->SetUsername("John");

// 设置 DataContext
element->SetValue(FrameworkElement::DataContextProperty(), viewModel);

// 创建绑定
element->SetValue(
    TextBlock::TextProperty(),
    Binding("Username").Mode(BindingMode::TwoWay)
);

// 更新 ViewModel - UI 自动更新
viewModel->SetUsername("Jane");
```

### 属性变更通知

```cpp
element->PropertyChanged += [](const DependencyPropertyChangedEventArgs& e) {
    std::cout << "属性变更：" << e.Property.Name() << std::endl;
    std::cout << "旧值：" << std::any_cast<double>(e.OldValue) << std::endl;
    std::cout << "新值：" << std::any_cast<double>(e.NewValue) << std::endl;
};

element->SetValue(UIElement::WidthProperty(), 300.0);
```

### 逻辑树

```cpp
// 设置父子关系
parent->AddLogicalChild(child.get());
child->SetLogicalParent(parent.get());

// 遍历逻辑子元素
for (auto& child : parent->GetLogicalChildren()) {
    // 处理子元素
}

// 向上遍历逻辑树
auto current = element->GetLogicalParent();
while (current) {
    // 处理祖先
    current = current->GetLogicalParent();
}
```

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
   // 不好 - 不必要地触发通知
   for (int i = 0; i < 100; i++) {
       element->SetValue(Property, i);
   }
   
   // 好 - 只设置最终值
   element->SetValue(Property, 99);
   ```

2. **批量更新使用 BeginInit/EndInit**
   ```cpp
   element->BeginInit();
   element->SetValue(WidthProperty(), 200);
   element->SetValue(HeightProperty(), 100);
   element->SetValue(BackgroundProperty(), color);
   element->EndInit();  // 触发单次布局更新
   ```

3. **清理不需要的绑定**
   ```cpp
   // 明确清除绑定以释放资源
   element->ClearValue(TextProperty());
   ```

## 线程安全

`DependencyObject` 不是线程安全的。所有属性访问必须在创建对象的线程上进行（通常是 UI 线程）。

使用 `Dispatcher` 从其他线程访问：

```cpp
dispatcher->InvokeAsync([element]() {
    // 现在在 UI 线程上
    element->SetValue(Property, value);
});
```

## 相关类

- [DependencyProperty](DependencyProperty.md) - 属性元数据
- [Binding](Binding.md) - 数据绑定配置
- [BindingExpression](BindingExpression.md) - 活动绑定
- [ObservableObject](ObservableObject.md) - ViewModel 基类
- [PropertyStore](PropertyStore.md) - 内部值存储

## 另请参阅

- [设计文档](../../Design/Binding/DependencyObject.md)
- [API 索引](../README.md)
- [架构概览](../../Architecture.md)
