# ViewModel 自动注册宏使用指南

## 概述

F__K_UI 框架提供了便捷的宏系统，用于自动注册 ViewModel 属性访问器。这大大简化了 MVVM 模式的使用。

## 为什么需要注册？

MVVM 数据绑定系统使用 `PropertyAccessorRegistry` 来解析属性路径（如 `"UserName"`）。由于 C++ 缺乏反射机制，我们需要手动注册每个属性的访问器。

传统的注册方式需要写大量重复代码：

```cpp
// ❌ 繁琐的手动注册
binding::PropertyAccessorRegistry::RegisterPropertyGetter<binding::INotifyPropertyChanged>(
    "UserName", 
    [](const binding::INotifyPropertyChanged& obj) -> const std::string& {
        return static_cast<const MyViewModel&>(obj).GetUserName();
    }
);
```

使用宏后，只需一行：

```cpp
// ✅ 简洁的宏注册
FK_VIEWMODEL_PROPERTY("UserName", &MyViewModel::GetUserName)
```

## 基础用法

### 1. 包含必要的头文件

```cpp
#include "fk/binding/ObservableObject.h"
#include "fk/binding/ViewModelMacros.h"
```

### 2. 定义 ViewModel 类

```cpp
class MyViewModel : public binding::ObservableObject {
public:
    // Getter 方法
    const std::string& GetUserName() const { return userName_; }
    int GetAge() const { return age_; }
    
    // Setter 方法（需要调用 RaisePropertyChanged）
    void SetUserName(const std::string& value) {
        if (userName_ != value) {
            userName_ = value;
            RaisePropertyChanged("UserName");
            // 如果有依赖属性，也要通知
            RaisePropertyChanged("DisplayName");
        }
    }
    
    // 计算属性
    std::string GetDisplayName() const {
        return "用户: " + userName_ + " (年龄: " + std::to_string(age_) + ")";
    }
    
private:
    std::string userName_;
    int age_{0};
};
```

### 3. 使用宏注册属性

#### 方式 1: 标准注册（推荐）

```cpp
FK_VIEWMODEL_BEGIN(MyViewModel)
    FK_VIEWMODEL_PROPERTY("UserName", &MyViewModel::GetUserName)
    FK_VIEWMODEL_PROPERTY("Age", &MyViewModel::GetAge)
    FK_VIEWMODEL_PROPERTY("DisplayName", &MyViewModel::GetDisplayName)
FK_VIEWMODEL_END()
```

#### 方式 2: 自动注册（适合快速原型）

如果您的 getter 方法遵循 `Get##PropertyName` 命名规范，可以使用更简洁的宏：

```cpp
// 自动推断 getter 为 GetUserName(), GetAge(), GetDisplayName()
FK_VIEWMODEL_AUTO(MyViewModel, UserName, Age, DisplayName)
```

**注意**：`FK_VIEWMODEL_AUTO` 最多支持 8 个属性。如果需要更多，请使用标准方式或多次调用。

### 4. 在 UI 中使用绑定

```cpp
int main() {
    // 创建 ViewModel
    auto viewModel = std::make_shared<MyViewModel>();
    
    // 创建窗口
    auto window = ui::window()->Title("MVVM 示例");
    
    // 设置 DataContext（注意转换为基类指针）
    window->SetDataContext(
        std::static_pointer_cast<binding::INotifyPropertyChanged>(viewModel)
    );
    
    // 创建绑定
    auto nameText = ui::textBlock();
    nameText->SetBinding(
        ui::detail::TextBlockBase::TextProperty(),
        binding::Binding().Path("UserName")
    );
    
    // ... 其他 UI 代码
}
```

## 高级用法

### 可读写属性

如果属性需要双向绑定（TwoWay），使用 `FK_VIEWMODEL_PROPERTY_RW`：

```cpp
FK_VIEWMODEL_BEGIN(MyViewModel)
    // 只读属性
    FK_VIEWMODEL_PROPERTY("DisplayName", &MyViewModel::GetDisplayName)
    
    // 可读写属性
    FK_VIEWMODEL_PROPERTY_RW("UserName", &MyViewModel::GetUserName, &MyViewModel::SetUserName)
FK_VIEWMODEL_END()
```

### 混合使用

您可以在同一个 ViewModel 中混合使用不同的注册方式：

```cpp
FK_VIEWMODEL_BEGIN(MyViewModel)
    FK_VIEWMODEL_PROPERTY("ReadOnlyProp", &MyViewModel::GetReadOnlyProp)
    FK_VIEWMODEL_PROPERTY_RW("EditableProp", &MyViewModel::GetEditableProp, &MyViewModel::SetEditableProp)
FK_VIEWMODEL_END()
```

## 完整示例

```cpp
#include "fk/binding/ObservableObject.h"
#include "fk/binding/ViewModelMacros.h"
#include "fk/binding/Binding.h"
#include "fk/ui/Window.h"
#include "fk/ui/TextBlock.h"
#include "fk/ui/Button.h"

using namespace fk;

// ============================================================================
// ViewModel 定义
// ============================================================================

class CounterViewModel : public binding::ObservableObject {
public:
    int GetCount() const { return count_; }
    
    void SetCount(int value) {
        if (count_ != value) {
            count_ = value;
            RaisePropertyChanged("Count");
            RaisePropertyChanged("CountText");  // 通知计算属性
        }
    }
    
    std::string GetCountText() const {
        return "当前计数: " + std::to_string(count_);
    }
    
    void Increment() {
        SetCount(count_ + 1);
    }
    
private:
    int count_{0};
};

// ============================================================================
// 自动注册属性（只需这几行！）
// ============================================================================

FK_VIEWMODEL_BEGIN(CounterViewModel)
    FK_VIEWMODEL_PROPERTY("Count", &CounterViewModel::GetCount)
    FK_VIEWMODEL_PROPERTY("CountText", &CounterViewModel::GetCountText)
FK_VIEWMODEL_END()

// ============================================================================
// UI 代码
// ============================================================================

int main() {
    Application app;
    
    // 创建 ViewModel
    auto viewModel = std::make_shared<CounterViewModel>();
    
    // 创建窗口和控件
    auto window = ui::window()->Title("计数器")->Width(400)->Height(300);
    auto counterText = ui::textBlock()->FontSize(32.0f);
    auto incrementBtn = ui::button()->Content("增加");
    
    // 设置 DataContext
    window->SetDataContext(
        std::static_pointer_cast<binding::INotifyPropertyChanged>(viewModel)
    );
    
    // 绑定文本
    counterText->SetBinding(
        ui::detail::TextBlockBase::TextProperty(),
        binding::Binding().Path("CountText")
    );
    
    // 绑定按钮点击事件
    incrementBtn->Click += [viewModel](ui::detail::ButtonBase&) {
        viewModel->Increment();
    };
    
    // ... 布局和其他代码
    
    return app.Run(window);
}
```

## 宏参考

### `FK_VIEWMODEL_BEGIN(ViewModelType)`
开始 ViewModel 属性注册块。

**参数**：
- `ViewModelType`: ViewModel 类名

### `FK_VIEWMODEL_PROPERTY(PropertyName, Getter)`
注册只读属性。

**参数**：
- `PropertyName`: 属性名称（字符串字面量）
- `Getter`: getter 方法指针（如 `&MyViewModel::GetName`）

### `FK_VIEWMODEL_PROPERTY_RW(PropertyName, Getter, Setter)`
注册可读写属性。

**参数**：
- `PropertyName`: 属性名称（字符串字面量）
- `Getter`: getter 方法指针
- `Setter`: setter 方法指针

### `FK_VIEWMODEL_END()`
结束 ViewModel 属性注册块。

### `FK_VIEWMODEL_AUTO(ViewModelType, Prop1, Prop2, ...)`
自动注册属性（假设 getter 为 `Get##PropertyName`）。

**参数**：
- `ViewModelType`: ViewModel 类名
- `Prop1, Prop2, ...`: 属性名称列表（最多 8 个）

## 注意事项

1. **注册位置**：宏必须在类定义之后使用，通常在同一个文件中。

2. **命名约定**：
   - 属性名称建议使用 PascalCase（如 `"UserName"`）
   - Getter 方法建议使用 `Get` 前缀（如 `GetUserName()`）
   - Setter 方法建议使用 `Set` 前缀（如 `SetUserName()`）

3. **DataContext 类型**：设置 DataContext 时必须转换为 `std::shared_ptr<INotifyPropertyChanged>`：
   ```cpp
   window->SetDataContext(std::static_pointer_cast<binding::INotifyPropertyChanged>(viewModel));
   ```

4. **属性变化通知**：在 setter 中必须调用 `RaisePropertyChanged()`：
   ```cpp
   void SetName(const std::string& value) {
       if (name_ != value) {
           name_ = value;
           RaisePropertyChanged("Name");  // 必须！
       }
   }
   ```

5. **计算属性**：当依赖的属性变化时，需要手动通知计算属性：
   ```cpp
   void SetFirstName(const std::string& value) {
       if (firstName_ != value) {
           firstName_ = value;
           RaisePropertyChanged("FirstName");
           RaisePropertyChanged("FullName");  // 通知依赖的计算属性
       }
   }
   ```

## 故障排除

### 编译错误：找不到 getter 方法

**问题**：
```
error: cannot call member function '...' without object
```

**解决方案**：
- 确保 getter 方法是 `const` 的
- 确保方法指针正确（使用 `&ClassName::MethodName`）

### 运行时错误：属性值不更新

**问题**：点击按钮后 UI 没有更新。

**检查清单**：
1. ✅ 是否调用了 `RaisePropertyChanged()`？
2. ✅ 是否正确设置了 `DataContext`？
3. ✅ 是否转换为 `std::shared_ptr<INotifyPropertyChanged>`？
4. ✅ 属性名称是否与绑定路径一致？
5. ✅ 是否注册了所有需要的属性？

### 属性名称拼写错误

使用宏可以避免在多处重复属性名称字符串，但仍需注意：

```cpp
// ❌ 错误：属性名称不一致
FK_VIEWMODEL_PROPERTY("Username", &MyViewModel::GetUserName)  // 注意大小写

// ✅ 正确
FK_VIEWMODEL_PROPERTY("UserName", &MyViewModel::GetUserName)
```

**建议**：将属性名称定义为常量：

```cpp
class MyViewModel : public binding::ObservableObject {
public:
    static constexpr const char* PROP_USER_NAME = "UserName";
    // ...
};

FK_VIEWMODEL_BEGIN(MyViewModel)
    FK_VIEWMODEL_PROPERTY(MyViewModel::PROP_USER_NAME, &MyViewModel::GetUserName)
FK_VIEWMODEL_END()
```

## 总结

使用 ViewModel 宏可以：
- ✅ **大幅减少样板代码**（每个属性从 10 行减少到 1 行）
- ✅ **提高代码可读性**
- ✅ **避免重复和错误**
- ✅ **加快开发速度**

现在您可以专注于业务逻辑，而不是繁琐的注册代码！🎉
