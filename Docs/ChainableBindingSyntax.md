# 链式绑定语法实现完成

## 实现内容

已成功实现 `textBlock()->Text(bind("PropertyName"))` 这种链式绑定语法。

## 改动文件

### 1. `include/fk/binding/Binding.h`

添加了全局辅助函数 `bind()`：

```cpp
namespace fk {
    // Helper function for convenient binding creation
    inline Binding bind(std::string path) {
        Binding b;
        b.Path(std::move(path));
        return b;
    }
}
```

**注意**：最初尝试使用 `binding()` 作为函数名，但与命名空间 `fk::binding` 冲突，因此改为 `bind()`。

### 2. `include/fk/ui/TextBlock.h`

为 `TextBlock` 模板类添加了 `Text(Binding)` 重载：

```cpp
// Fluent API: Text with Binding
Ptr Text(binding::Binding binding) {
    SetBinding(TextProperty(), std::move(binding));
    return Self();
}
```

## 使用方法

### ⚠️ 关键要点

**必须将 ViewModel 转换为基类指针才能使绑定工作！**

```cpp
auto viewModel = std::make_shared<PersonViewModel>();

// ✅ 正确：转换为 INotifyPropertyChanged 基类指针
std::shared_ptr<binding::INotifyPropertyChanged> baseViewModel = viewModel;
window->SetDataContext(baseViewModel);

// ❌ 错误：直接使用派生类指针会导致绑定解析失败
// window->SetDataContext(viewModel);
```

### 基本语法

```cpp
auto textBlock = ui::textBlock()->Text(bind("PropertyName"));
```

### 链式调用

```cpp
auto textBlock = ui::textBlock()
    ->Text(bind("UserName"))
    ->FontSize(16.0f)
    ->Foreground("#FF0078D4");
```

### 完整示例

```cpp
// 1. 创建 ViewModel
class PersonViewModel : public fk::ObservableObject {
public:
    FK_PROPERTY(std::string, Name)
    FK_PROPERTY(std::string, City)
};
FK_VIEWMODEL_AUTO(PersonViewModel, Name, City)

// 2. 在 main() 中
auto viewModel = std::make_shared<PersonViewModel>();
auto window = ui::window();
auto panel = ui::stackPanel();

auto nameText = ui::textBlock()->Text(bind("Name"));
auto cityText = ui::textBlock()->Text(bind("City"));

panel->AddChild(nameText);
panel->AddChild(cityText);
window->SetContent(panel);

// 3. ⚠️ 关键：转换为基类指针
std::shared_ptr<binding::INotifyPropertyChanged> baseViewModel = viewModel;
window->SetDataContext(baseViewModel);
```

### 与传统语法对比

**传统语法**：
```cpp
Binding binding;
binding.Path("PropertyName");
textBlock->SetBinding(TextProperty(), binding);
```

**新语法**：
```cpp
textBlock()->Text(bind("PropertyName"))  // ✨ 简洁！
```

## 示例项目

创建了完整的示例项目：`samples/binding_syntax_demo/`

- **位置**: `g:\Documents\Visual Studio Code\F__K_UI\samples\binding_syntax_demo\`
- **文件结构**:
  ```
  binding_syntax_demo/
  ├── CMakeLists.txt
  ├── README.md
  ├── src/
  │   └── main.cpp
  └── build/
      └── binding_syntax_demo.exe
  ```

## 编译和测试

### 编译库

```bash
cd g:\Documents\Visual Studio Code\F__K_UI\build
cmake --build . --target fk
```

✅ 编译成功，无错误

### 编译示例

```bash
cd samples/binding_syntax_demo/build
cmake -G "MinGW Makefiles" ..
cmake --build .
```

✅ 编译成功，生成 `binding_syntax_demo.exe`

### 运行示例

```bash
.\binding_syntax_demo.exe
```

✅ 运行成功，窗口正常显示

## 技术细节

### 实现原理

1. **`bind()` 函数**：创建 `Binding` 对象并设置 `Path`
2. **`Text(Binding)` 重载**：接受 `Binding` 对象，调用 `SetBinding()` 并返回 `this` 指针以支持链式调用
3. **内部实现**：与传统语法完全相同，没有额外开销

### 为什么需要基类指针转换？

绑定系统使用 `std::type_index` 查找属性访问器：

```cpp
// BindingPath.cpp - Resolve() 方法
const auto type = std::type_index(current.type());
const auto* accessor = PropertyAccessorRegistry::FindAccessor(type, segment.name);
```

**问题原因：**
- 属性通过 `FK_VIEWMODEL_BEGIN/END` 宏注册在 `INotifyPropertyChanged` 类型上
- 如果 DataContext 存储 `std::shared_ptr<PersonViewModel>`
  - `current.type()` 返回 `typeid(std::shared_ptr<PersonViewModel>)`
  - 但属性访问器注册在 `typeid(std::shared_ptr<INotifyPropertyChanged>)` 上
  - 类型索引不匹配 → `FindAccessor()` 返回 `nullptr` → 绑定失败

**解决方法：**
```cpp
// 转换为基类指针
std::shared_ptr<binding::INotifyPropertyChanged> baseViewModel = viewModel;
window->SetDataContext(baseViewModel);

// 现在：
// - DataContext 类型 = std::shared_ptr<INotifyPropertyChanged>
// - 属性注册类型 = std::shared_ptr<INotifyPropertyChanged>
// - 类型匹配 ✅ → 绑定成功！
```

### 属性注册机制

`FK_VIEWMODEL_BEGIN` 宏展开后：

```cpp
FK_VIEWMODEL_BEGIN(PersonViewModel)
    FK_VIEWMODEL_PROPERTY("Name", &PersonViewModel::GetName)
FK_VIEWMODEL_END(PersonViewModel)

// 展开为：
PropertyAccessorRegistry::RegisterPropertyGetter<INotifyPropertyChanged>(
    "Name",
    [](const INotifyPropertyChanged& obj) -> std::string {
        return static_cast<const PersonViewModel&>(obj).GetName();
    }
);
```

`RegisterPropertyGetter` 会为多种指针类型注册访问器：

```cpp
template<typename Owner>
void RegisterVariants(const std::string& name, const Accessor& accessor) {
    RegisterAccessor(TypeIndex<Owner>(), name, accessor);
    RegisterAccessor(TypeIndex<const Owner>(), name, accessor);
    RegisterAccessor(TypeIndex<Owner*>(), name, accessor);
    RegisterAccessor(TypeIndex<const Owner*>(), name, accessor);
    RegisterAccessor(TypeIndex<std::shared_ptr<Owner>>(), name, accessor);  // ← 关键！
    RegisterAccessor(TypeIndex<std::shared_ptr<const Owner>>(), name, accessor);
    // ... 其他类型
}
```

这里的 `Owner` 是 `INotifyPropertyChanged`，所以注册的是：
- `std::shared_ptr<INotifyPropertyChanged>` ✅
- **不是** `std::shared_ptr<PersonViewModel>` ❌

### 类型安全

- 函数参数类型明确：`Binding` 对象
- 编译时检查，避免运行时错误
- 返回类型 `Ptr` (即 `std::shared_ptr<Derived>`) 支持链式调用

### 性能

- 无额外开销
- `bind()` 函数内联 (inline)
- `std::move` 语义避免不必要的拷贝
- 基类指针转换在编译时完成，零运行时开销

## 扩展性

这种模式可以应用到其他控件和属性：

### 未来可扩展的控件

```cpp
// Button
auto btn = ui::button()
    ->Content(bind("ButtonText"))
    ->IsEnabled(bind("CanExecute"));

// TextBox
auto input = ui::textBox()
    ->Text(bind("InputValue"))
    ->IsReadOnly(bind("IsLocked"));

// CheckBox
auto checkbox = ui::checkBox()
    ->IsChecked(bind("IsSelected"))
    ->Content(bind("CheckboxLabel"));
```

### 实现步骤

对于每个需要支持绑定的属性：

1. 找到对应的 DependencyProperty（如 `ContentProperty()`, `IsEnabledProperty()` 等）
2. 在对应的 CRTP 模板类中添加重载：
   ```cpp
   Ptr PropertyName(binding::Binding binding) {
       SetBinding(PropertyNameProperty(), std::move(binding));
       return Self();
   }
   ```

## 优点总结

1. ✅ **更简洁**：一行代码完成绑定
2. ✅ **可读性强**：代码意图清晰
3. ✅ **链式调用**：与其他方法无缝衔接
4. ✅ **类型安全**：编译时检查
5. ✅ **向后兼容**：不影响现有代码
6. ✅ **零性能开销**：内联函数 + move 语义
7. ✅ **易于扩展**：模式可复用

## 向后兼容性

- ✅ 传统语法仍然有效
- ✅ 现有代码无需修改
- ✅ 两种语法可以混用
- ✅ 内部实现相同

## 文档

- 示例 README：`samples/binding_syntax_demo/README.md`
- 本文档：记录实现细节和使用方法

## 常见问题

### Q: 为什么绑定不显示数据？

**A:** 最常见的原因是忘记将 ViewModel 转换为基类指针：

```cpp
// ❌ 错误
auto viewModel = std::make_shared<PersonViewModel>();
window->SetDataContext(viewModel);

// ✅ 正确
auto viewModel = std::make_shared<PersonViewModel>();
std::shared_ptr<binding::INotifyPropertyChanged> baseViewModel = viewModel;
window->SetDataContext(baseViewModel);
```

### Q: 可以直接使用 `auto` 吗？

**A:** 不可以，必须显式指定基类指针类型：

```cpp
// ❌ 错误：auto 推断为 std::shared_ptr<PersonViewModel>
auto baseViewModel = viewModel;

// ✅ 正确：显式指定类型强制转换
std::shared_ptr<binding::INotifyPropertyChanged> baseViewModel = viewModel;
```

### Q: 忘记注册属性会怎样？

**A:** 编译可以通过，但绑定在运行时静默失败（不显示数据）：

```cpp
class MyViewModel : public fk::ObservableObject {
    FK_PROPERTY(std::string, Name)
};
// ❌ 忘记添加 FK_VIEWMODEL_AUTO(MyViewModel, Name)
// 结果：绑定创建成功，但 Text 显示为空字符串
```

**解决方法**：始终在 ViewModel 类定义后添加 `FK_VIEWMODEL_AUTO`。

## 调试技巧

### 检查 DataContext 类型

```cpp
if (element->HasDataContext()) {
    auto& dc = element->GetDataContext();
    std::cout << "DataContext 类型: " << dc.type().name() << std::endl;
    // 期望输出包含 "INotifyPropertyChanged"，不是 "PersonViewModel"
}
```

### 检查绑定是否创建

```cpp
textBlock->Text(bind("Name"));
std::cout << "绑定后的文本: '" << textBlock->GetText() << "'" << std::endl;
// 如果输出空字符串，说明绑定解析失败
```

### 手动验证 ViewModel

```cpp
auto vm = std::make_shared<PersonViewModel>();
std::cout << "ViewModel 中的值: " << vm->GetName() << std::endl;
// 如果能输出正确值，说明 ViewModel 本身没问题
```

## 状态

✅ **实现完成**  
✅ **编译通过**  
✅ **测试通过**  
✅ **文档完成**  
✅ **调试问题解决**  

---

**实现日期**：2024年  
**关键发现**：必须使用基类指针才能使绑定系统正确识别类型
