# ViewModel 属性宏完全指南

## 概述

F__K_UI 提供了三层级的 ViewModel 属性宏，从手动到全自动：

1. **手动模式**：手写属性 + 使用 `FK_VIEWMODEL_BEGIN/END` 注册
2. **半自动模式**：手写属性 + 使用 `FK_VIEWMODEL_AUTO` 自动注册
3. **全自动模式**：使用 `FK_PROPERTY` 系列宏自动生成属性 + 自动注册

## 快速对比

### 传统方式（~15 行代码）

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
private:
    std::string firstName_;
};

FK_VIEWMODEL_BEGIN(PersonViewModel)
    FK_VIEWMODEL_PROPERTY("FirstName", &PersonViewModel::GetFirstName)
FK_VIEWMODEL_END()
```

### 全自动方式（~3 行代码！）

```cpp
class PersonViewModel : public binding::ObservableObject {
public:
    FK_PROPERTY_CUSTOM(std::string, FirstName, {
        RaisePropertyChanged(std::string_view("FullName"));
    })
};

FK_VIEWMODEL_AUTO(PersonViewModel, FirstName, FullName)
```

**减少 80% 的代码量！**

---

## 属性声明宏

### `FK_PROPERTY_READONLY` - 只读属性

自动生成：
- `private: Type propertyName_PropertyName{};`
- `public: const Type& GetPropertyName() const;`

**用法**：

```cpp
class MyViewModel : public binding::ObservableObject {
public:
    FK_PROPERTY_READONLY(int, Age)
    FK_PROPERTY_READONLY(std::string, UserId)
};
```

**生成的代码**：

```cpp
private:
    int propertyName_Age{};
public:
    const int& GetAge() const { return propertyName_Age; }

private:
    std::string propertyName_UserId{};
public:
    const std::string& GetUserId() const { return propertyName_UserId; }
```

**适用场景**：
- 不需要外部修改的属性
- 系统生成的 ID、时间戳等
- 需要在类内部修改，但不希望暴露 setter

**访问内部成员变量**：

```cpp
void InternalUpdate() {
    propertyName_Age = 30;  // 直接访问
    RaisePropertyChanged(std::string_view("Age"));
}
```

---

### `FK_PROPERTY` - 可读写属性

自动生成：
- `private: Type propertyName_PropertyName{};`
- `public: const Type& GetPropertyName() const;`
- `public: void SetPropertyName(const Type& value);` （自动调用 `RaisePropertyChanged`）

**用法**：

```cpp
class MyViewModel : public binding::ObservableObject {
public:
    FK_PROPERTY(std::string, UserName)
    FK_PROPERTY(std::string, Email)
    FK_PROPERTY(bool, IsActive)
};
```

**生成的代码**：

```cpp
private:
    std::string propertyName_UserName{};
public:
    const std::string& GetUserName() const { return propertyName_UserName; }
    void SetUserName(const std::string& value) {
        if (propertyName_UserName != value) {
            propertyName_UserName = value;
            RaisePropertyChanged(std::string_view("UserName"));
        }
    }
```

**特性**：
- ✅ 自动值比较（避免无效通知）
- ✅ 自动调用 `RaisePropertyChanged`
- ✅ 线程安全（如果基类 `RaisePropertyChanged` 是线程安全的）

**适用场景**：
- 简单的数据属性
- 不影响其他属性的独立属性
- 大部分常规属性

---

### `FK_PROPERTY_CUSTOM` - 自定义 Setter

自动生成：
- `private: Type propertyName_PropertyName{};`
- `public: const Type& GetPropertyName() const;`
- `public: void SetPropertyName(const Type& value);` （带自定义代码）

**用法**：

```cpp
class MyViewModel : public binding::ObservableObject {
public:
    FK_PROPERTY_CUSTOM(std::string, FirstName, {
        RaisePropertyChanged(std::string_view("FullName"));
        RaisePropertyChanged(std::string_view("DisplayText"));
    })
    
    FK_PROPERTY_CUSTOM(int, Count, {
        if (GetCount() > 10) {
            RaisePropertyChanged(std::string_view("IsOverLimit"));
        }
    })
    
    // 计算属性（手动定义）
    std::string GetFullName() const {
        return GetFirstName() + " " + GetLastName();
    }
    
    bool GetIsOverLimit() const {
        return GetCount() > 10;
    }
};
```

**生成的代码**：

```cpp
void SetFirstName(const std::string& value) {
    if (propertyName_FirstName != value) {
        propertyName_FirstName = value;
        RaisePropertyChanged(std::string_view("FirstName"));  // 自动
        // 用户自定义代码：
        RaisePropertyChanged(std::string_view("FullName"));
        RaisePropertyChanged(std::string_view("DisplayText"));
    }
}
```

**适用场景**：
- 有依赖属性的属性（如 FirstName → FullName）
- 需要触发副作用的属性
- 需要额外验证或日志的属性

**注意**：
- 自定义代码在 `RaisePropertyChanged` **之后**执行
- 可以访问更新后的值（通过 `GetPropertyName()`）
- 可以调用其他方法或触发事件

---

## 自动注册宏

### `FK_VIEWMODEL_AUTO` - 自动注册所有属性

根据命名约定（`Get##PropertyName`）自动注册属性访问器。

**用法**：

```cpp
class MyViewModel : public binding::ObservableObject {
public:
    FK_PROPERTY(std::string, UserName)
    FK_PROPERTY(int, Age)
    
    std::string GetDisplayText() const {
        return GetUserName() + " (" + std::to_string(GetAge()) + ")";
    }
};

// 自动注册（假设 getter 为 GetUserName, GetAge, GetDisplayText）
FK_VIEWMODEL_AUTO(MyViewModel, UserName, Age, DisplayText)
```

**展开为**：

```cpp
FK_VIEWMODEL_BEGIN(MyViewModel)
    FK_VIEWMODEL_PROPERTY("UserName", &MyViewModel::GetUserName)
    FK_VIEWMODEL_PROPERTY("Age", &MyViewModel::GetAge)
    FK_VIEWMODEL_PROPERTY("DisplayText", &MyViewModel::GetDisplayText)
FK_VIEWMODEL_END()
```

**限制**：
- 最多支持 8 个属性
- 必须遵循 `Get##PropertyName` 命名约定
- 如果需要更多属性，可以多次调用或使用手动注册

---

## 完整示例

### 示例 1：简单计数器

```cpp
#include "fk/binding/ObservableObject.h"
#include "fk/binding/ViewModelMacros.h"

class CounterViewModel : public binding::ObservableObject {
public:
    // 可读写属性（带依赖通知）
    FK_PROPERTY_CUSTOM(int, Count, {
        RaisePropertyChanged(std::string_view("CountText"));
        RaisePropertyChanged(std::string_view("IsEven"));
    })
    
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

// 自动注册
FK_VIEWMODEL_AUTO(CounterViewModel, Count, CountText, IsEven)
```

**代码量对比**：
- 传统方式：~40 行
- 使用宏：~15 行
- **减少 62.5%**

---

### 示例 2：用户资料

```cpp
class UserProfileViewModel : public binding::ObservableObject {
public:
    // 基本属性
    FK_PROPERTY(std::string, UserName)
    FK_PROPERTY(std::string, Email)
    
    // 带依赖的属性
    FK_PROPERTY_CUSTOM(std::string, FirstName, {
        RaisePropertyChanged(std::string_view("FullName"));
    })
    
    FK_PROPERTY_CUSTOM(std::string, LastName, {
        RaisePropertyChanged(std::string_view("FullName"));
    })
    
    FK_PROPERTY_CUSTOM(bool, IsOnline, {
        RaisePropertyChanged(std::string_view("StatusText"));
    })
    
    // 计算属性
    std::string GetFullName() const {
        return GetFirstName() + " " + GetLastName();
    }
    
    std::string GetStatusText() const {
        return GetUserName() + " - " + 
               (GetIsOnline() ? "在线" : "离线");
    }
};

// 自动注册
FK_VIEWMODEL_AUTO(UserProfileViewModel, 
    UserName, Email, FirstName, LastName, IsOnline, 
    FullName, StatusText)
```

---

### 示例 3：混合使用不同类型的属性

```cpp
class ProductViewModel : public binding::ObservableObject {
public:
    // 只读属性（系统生成）
    FK_PROPERTY_READONLY(std::string, ProductId)
    
    // 可读写属性
    FK_PROPERTY(std::string, Name)
    FK_PROPERTY(double, Price)
    
    // 带依赖的属性
    FK_PROPERTY_CUSTOM(int, Quantity, {
        RaisePropertyChanged(std::string_view("TotalPrice"));
        RaisePropertyChanged(std::string_view("IsInStock"));
    })
    
    FK_PROPERTY_CUSTOM(double, Discount, {
        RaisePropertyChanged(std::string_view("FinalPrice"));
    })
    
    // 计算属性
    double GetTotalPrice() const {
        return GetPrice() * GetQuantity();
    }
    
    double GetFinalPrice() const {
        return GetTotalPrice() * (1.0 - GetDiscount());
    }
    
    bool GetIsInStock() const {
        return GetQuantity() > 0;
    }
    
    // 方法：初始化只读属性
    void Initialize(const std::string& id) {
        propertyName_ProductId = id;
        RaisePropertyChanged(std::string_view("ProductId"));
    }
};

FK_VIEWMODEL_AUTO(ProductViewModel, 
    ProductId, Name, Price, Quantity, Discount,
    TotalPrice, FinalPrice, IsInStock)
```

---

## 最佳实践

### 1. 选择合适的宏

| 场景 | 推荐宏 | 理由 |
|------|--------|------|
| 简单数据属性 | `FK_PROPERTY` | 自动处理通知 |
| 系统生成的值 | `FK_PROPERTY_READONLY` | 防止外部修改 |
| 影响其他属性 | `FK_PROPERTY_CUSTOM` | 级联通知 |
| 计算属性 | 手动 getter | 不需要存储值 |

### 2. 命名约定

```cpp
// ✅ 推荐
FK_PROPERTY(std::string, UserName)     // PascalCase
FK_PROPERTY(bool, IsActive)            // Is/Has/Can 前缀
FK_PROPERTY(int, MessageCount)         // 描述性名称

// ❌ 避免
FK_PROPERTY(std::string, username)     // 不要用 camelCase
FK_PROPERTY(bool, active)              // bool 应有前缀
FK_PROPERTY(int, n)                    // 名称太短
```

### 3. 依赖关系管理

```cpp
class ViewModel : public binding::ObservableObject {
public:
    // 基础属性 → 影响计算属性
    FK_PROPERTY_CUSTOM(double, Width, {
        RaisePropertyChanged(std::string_view("Area"));
    })
    
    FK_PROPERTY_CUSTOM(double, Height, {
        RaisePropertyChanged(std::string_view("Area"));
    })
    
    // 计算属性
    double GetArea() const {
        return GetWidth() * GetHeight();
    }
};
```

**规则**：
- 基础属性变化 → 通知所有依赖的计算属性
- 避免循环依赖（A → B → A）
- 使用有向无环图（DAG）组织依赖

### 4. 性能优化

```cpp
// ✅ 好：使用 const 引用
FK_PROPERTY(std::string, LongText)

// ❌ 不好：返回值拷贝（适用于小对象）
int GetCount() const { return count_; }  // int 可以直接返回

// ✅ 好：大对象返回引用
const Type& GetPropertyName() const { return propertyName_PropertyName; }
```

### 5. 初始化只读属性

```cpp
class ViewModel : public binding::ObservableObject {
public:
    FK_PROPERTY_READONLY(std::string, UserId)
    
    // 构造函数或初始化方法
    void Initialize(const std::string& id) {
        propertyName_UserId = id;  // 直接访问成员变量
        RaisePropertyChanged(std::string_view("UserId"));
    }
};
```

---

## 故障排除

### 问题 1：编译错误 `propertyName_XXX` 未定义

**原因**：在类外部或错误的访问级别使用。

**解决**：

```cpp
// ❌ 错误
class MyVM : public binding::ObservableObject {
    FK_PROPERTY(int, Count)  // private 区域
};

// ✅ 正确
class MyVM : public binding::ObservableObject {
public:
    FK_PROPERTY(int, Count)  // public 区域
};
```

### 问题 2：属性不更新

**检查清单**：
1. ✅ 是否使用了 `FK_VIEWMODEL_AUTO` 或手动注册？
2. ✅ 属性名称是否与绑定路径一致？
3. ✅ 是否正确设置了 `DataContext`？

```cpp
// 确保注册
FK_VIEWMODEL_AUTO(MyViewModel, PropertyName)

// 确保 DataContext
window->SetDataContext(
    std::static_pointer_cast<binding::INotifyPropertyChanged>(viewModel)
);
```

### 问题 3：依赖属性不更新

**检查**：是否在 `FK_PROPERTY_CUSTOM` 中通知了依赖属性？

```cpp
// ❌ 错误：没有通知 FullName
FK_PROPERTY(std::string, FirstName)

// ✅ 正确：通知依赖属性
FK_PROPERTY_CUSTOM(std::string, FirstName, {
    RaisePropertyChanged(std::string_view("FullName"));
})
```

---

## 高级技巧

### 1. 组合多个属性变化

```cpp
FK_PROPERTY_CUSTOM(bool, IsAdmin, {
    RaisePropertyChanged(std::string_view("CanEdit"));
    RaisePropertyChanged(std::string_view("CanDelete"));
    RaisePropertyChanged(std::string_view("PermissionText"));
})
```

### 2. 条件通知

```cpp
FK_PROPERTY_CUSTOM(int, Score, {
    if (GetScore() >= 100) {
        RaisePropertyChanged(std::string_view("IsMaxScore"));
    }
    if (GetScore() % 10 == 0) {
        RaisePropertyChanged(std::string_view("Milestone"));
    }
})
```

### 3. 副作用和日志

```cpp
FK_PROPERTY_CUSTOM(std::string, UserName, {
    std::cout << "[VM] UserName changed to: " << GetUserName() << std::endl;
    LogUserAction("username_changed");
    RaisePropertyChanged(std::string_view("DisplayName"));
})
```

---

## 总结

使用 ViewModel 属性宏可以：

✅ **大幅减少代码量**（60-80%）  
✅ **自动处理 PropertyChanged 通知**  
✅ **避免重复和拼写错误**  
✅ **提高代码可读性**  
✅ **加快开发速度**  

**推荐使用顺序**：
1. 尝试 `FK_PROPERTY` 或 `FK_PROPERTY_CUSTOM`
2. 如果需要更多控制，使用手动 getter/setter
3. 始终使用 `FK_VIEWMODEL_AUTO` 自动注册

现在您可以专注于业务逻辑，让宏处理繁琐的样板代码！🚀
