# ViewModel 属性宏 - 快速开始

## 🎯 为什么需要属性宏？

在传统的 C++ MVVM 开发中，每个属性需要编写大量样板代码：

```cpp
// ❌ 传统方式：每个属性需要 ~15 行代码
class ViewModel : public ObservableObject {
public:
    const std::string& GetName() const { return name_; }
    void SetName(const std::string& value) {
        if (name_ != value) {
            name_ = value;
            RaisePropertyChanged(std::string_view("Name"));
            RaisePropertyChanged(std::string_view("DisplayText"));
        }
    }
private:
    std::string name_;
};

// 还需要手动注册...
FK_VIEWMODEL_BEGIN(ViewModel)
    FK_VIEWMODEL_PROPERTY("Name", &ViewModel::GetName)
FK_VIEWMODEL_END()
```

使用属性宏后，同样的功能只需 **3 行代码**！

```cpp
// ✅ 使用宏：只需 3 行
class ViewModel : public ObservableObject {
public:
    FK_PROPERTY_CUSTOM(std::string, Name, {
        RaisePropertyChanged(std::string_view("DisplayText"));
    })
};

FK_VIEWMODEL_AUTO(ViewModel, Name, DisplayText)
```

---

## 🚀 三种属性宏

### 1. `FK_PROPERTY` - 普通属性

**最常用**，自动生成 getter、setter 和成员变量。

```cpp
class MyViewModel : public binding::ObservableObject {
public:
    FK_PROPERTY(std::string, UserName)
    FK_PROPERTY(int, Age)
    FK_PROPERTY(bool, IsActive)
};

FK_VIEWMODEL_AUTO(MyViewModel, UserName, Age, IsActive)
```

**自动生成**：
- `private: std::string propertyName_UserName{};`
- `public: const std::string& GetUserName() const;`
- `public: void SetUserName(const std::string& value);` （含 PropertyChanged 通知）

**使用场景**：简单数据属性，不影响其他属性。

---

### 2. `FK_PROPERTY_CUSTOM` - 带依赖通知的属性

当一个属性变化会影响其他属性时使用。

```cpp
class PersonViewModel : public binding::ObservableObject {
public:
    // FirstName 变化时，通知 FullName 也变了
    FK_PROPERTY_CUSTOM(std::string, FirstName, {
        RaisePropertyChanged(std::string_view("FullName"));
    })
    
    FK_PROPERTY_CUSTOM(std::string, LastName, {
        RaisePropertyChanged(std::string_view("FullName"));
    })
    
public:
    // 计算属性（手动定义）
    std::string GetFullName() const {
        return GetFirstName() + " " + GetLastName();
    }
};

FK_VIEWMODEL_AUTO(PersonViewModel, FirstName, LastName, FullName)
```

**使用场景**：有依赖关系的属性、需要副作用的属性。

---

### 3. `FK_PROPERTY_READONLY` - 只读属性

不生成 setter，只能在类内部修改。

```cpp
class ProductViewModel : public binding::ObservableObject {
public:
    FK_PROPERTY_READONLY(std::string, ProductId)
    FK_PROPERTY(std::string, Name)
    
public:
    void Initialize(const std::string& id) {
        propertyName_ProductId = id;  // 直接访问成员变量
        RaisePropertyChanged(std::string_view("ProductId"));
    }
};

FK_VIEWMODEL_AUTO(ProductViewModel, ProductId, Name)
```

**使用场景**：系统生成的 ID、时间戳、不允许外部修改的值。

---

## 📝 完整示例

### 计数器 ViewModel

```cpp
#include "fk/binding/ObservableObject.h"
#include "fk/binding/ViewModelMacros.h"
#include "fk/binding/Binding.h"

class CounterViewModel : public binding::ObservableObject {
public:
    // Count 属性（带依赖通知）
    FK_PROPERTY_CUSTOM(int, Count, {
        RaisePropertyChanged(std::string_view("CountText"));
        RaisePropertyChanged(std::string_view("IsEven"));
    })
    
public:
    // 计算属性
    std::string GetCountText() const {
        return "当前计数: " + std::to_string(GetCount());
    }
    
    bool GetIsEven() const {
        return GetCount() % 2 == 0;
    }
    
    // 命令方法
    void Increment() { SetCount(GetCount() + 1); }
    void Decrement() { SetCount(GetCount() - 1); }
};

// 自动注册所有属性
FK_VIEWMODEL_AUTO(CounterViewModel, Count, CountText, IsEven)
```

### 在 UI 中使用

```cpp
int main() {
    // 创建 ViewModel
    auto viewModel = std::make_shared<CounterViewModel>();
    
    // 创建窗口
    auto window = ui::window()->Title("计数器");
    window->SetDataContext(
        std::static_pointer_cast<binding::INotifyPropertyChanged>(viewModel)
    );
    
    // 创建文本控件并绑定
    auto counterText = ui::textBlock();
    counterText->SetBinding(
        ui::detail::TextBlockBase::TextProperty(),
        binding::Binding().Path("CountText")  // 绑定到 CountText 属性
    );
    
    // 创建按钮
    auto incrementBtn = ui::button()->Content("增加");
    incrementBtn->Click += [viewModel](ui::detail::ButtonBase&) {
        viewModel->Increment();  // 点击时调用 ViewModel 方法
    };
    
    // ... 布局代码
    
    return app.Run(window);
}
```

**效果**：点击按钮 → `Count` 变化 → 自动通知 `CountText` 和 `IsEven` → UI 自动更新！

---

## 🎨 对比：传统 vs 宏

### 定义 3 个属性（FirstName, LastName, FullName）

#### 传统方式：~50 行

```cpp
class PersonViewModel : public binding::ObservableObject {
public:
    const std::string& GetFirstName() const { return firstName_; }
    void SetFirstName(const std::string& value) {
        if (firstName_ != value) {
            firstName_ = value;
            RaisePropertyChanged(std::string_view("FirstName"));
            RaisePropertyChanged(std::string_view("FullName"));
        }
    }
    
    const std::string& GetLastName() const { return lastName_; }
    void SetLastName(const std::string& value) {
        if (lastName_ != value) {
            lastName_ = value;
            RaisePropertyChanged(std::string_view("LastName"));
            RaisePropertyChanged(std::string_view("FullName"));
        }
    }
    
    std::string GetFullName() const {
        return firstName_ + " " + lastName_;
    }
    
private:
    std::string firstName_;
    std::string lastName_;
};

FK_VIEWMODEL_BEGIN(PersonViewModel)
    FK_VIEWMODEL_PROPERTY("FirstName", &PersonViewModel::GetFirstName)
    FK_VIEWMODEL_PROPERTY("LastName", &PersonViewModel::GetLastName)
    FK_VIEWMODEL_PROPERTY("FullName", &PersonViewModel::GetFullName)
FK_VIEWMODEL_END()
```

#### 使用宏：~10 行（减少 80%！）

```cpp
class PersonViewModel : public binding::ObservableObject {
public:
    FK_PROPERTY_CUSTOM(std::string, FirstName, {
        RaisePropertyChanged(std::string_view("FullName"));
    })
    FK_PROPERTY_CUSTOM(std::string, LastName, {
        RaisePropertyChanged(std::string_view("FullName"));
    })
    
public:
    std::string GetFullName() const {
        return GetFirstName() + " " + GetLastName();
    }
};

FK_VIEWMODEL_AUTO(PersonViewModel, FirstName, LastName, FullName)
```

---

## ⚠️ 注意事项

### 1. public 区域

宏必须在 `public:` 区域使用：

```cpp
class MyViewModel : public binding::ObservableObject {
public:  // ✅ 正确
    FK_PROPERTY(int, Count)
};
```

### 2. 计算属性必须 public

`FK_VIEWMODEL_AUTO` 需要访问 getter 方法：

```cpp
class MyViewModel : public binding::ObservableObject {
public:
    FK_PROPERTY(std::string, FirstName)
    
public:  // ✅ 必须 public
    std::string GetDisplayName() const {
        return "User: " + GetFirstName();
    }
};

FK_VIEWMODEL_AUTO(MyViewModel, FirstName, DisplayName)
```

### 3. 属性命名约定

- 属性名：PascalCase（`FirstName`, `UserAge`）
- Getter：`Get##PropertyName` （`GetFirstName()`, `GetUserAge()`）
- 成员变量：自动生成为 `propertyName_##PropertyName`

### 4. 访问内部成员变量

对于只读属性或需要内部修改的场景：

```cpp
FK_PROPERTY_READONLY(int, Id)

void InternalUpdate() {
    propertyName_Id = 123;  // 直接访问
    RaisePropertyChanged(std::string_view("Id"));
}
```

---

## 🔍 常见问题

### Q: 为什么要通知依赖属性？

**A**: C++ 没有自动依赖追踪。当 `FirstName` 变化时，`FullName` 的值也变了，但 UI 不知道，需要手动通知。

```cpp
FK_PROPERTY_CUSTOM(std::string, FirstName, {
    RaisePropertyChanged(std::string_view("FullName"));  // 必须！
})
```

### Q: 可以有循环依赖吗？

**A**: 不能！会导致无限循环。

```cpp
// ❌ 错误：A → B → A
FK_PROPERTY_CUSTOM(int, A, {
    RaisePropertyChanged(std::string_view("B"));
})
FK_PROPERTY_CUSTOM(int, B, {
    RaisePropertyChanged(std::string_view("A"));  // 循环！
})
```

### Q: 如何绑定到 UI？

**A**: 使用 `Binding().Path("PropertyName")`：

```cpp
textBlock->SetBinding(
    ui::detail::TextBlockBase::TextProperty(),
    binding::Binding().Path("FirstName")
);
```

### Q: 性能如何？

**A**: 宏在编译时展开，**零运行时开销**！生成的代码与手写完全相同。

---

## 📚 更多资源

- **详细文档**: `Docs/ViewModelPropertyMacros-Guide.md`
- **完整示例**: `examples/simple_property_macro_test.cpp`
- **MVVM 示例**: `samples/mvvm_binding_demo/src/main.cpp`

---

## 🎉 总结

使用 ViewModel 属性宏可以：

✅ **减少 60-80% 的样板代码**  
✅ **自动处理 PropertyChanged 通知**  
✅ **自动注册属性访问器**  
✅ **避免拼写错误和重复代码**  
✅ **提高开发效率 3-5 倍**  

现在开始使用吧！🚀
