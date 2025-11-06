# MVVM 数据绑定流程 - 快速参考

## 🎯 三步理解绑定

```
1️⃣ 注册阶段（程序启动前）
   宏展开 → 静态初始化 → 注册到 PropertyAccessorRegistry

2️⃣ 绑定阶段（SetBinding 时）
   创建 BindingExpression → 订阅 PropertyChanged → 首次更新 UI

3️⃣ 运行阶段（用户交互时）
   属性变化 → 触发事件 → BindingExpression 更新 UI
```

---

## 📊 数据流向图

### OneWay 绑定（ViewModel → UI）

```
┌─────────────────────────────────────────────────────────────────┐
│                      【注册阶段】                                 │
│  FK_VIEWMODEL_AUTO(CounterViewModel, Count)                     │
│          ↓                                                       │
│  PropertyAccessorRegistry["Count"] = lambda(obj) {               │
│      return static_cast<CounterViewModel&>(obj).GetCount();     │
│  }                                                               │
└─────────────────────────────────────────────────────────────────┘
                              ↓
┌─────────────────────────────────────────────────────────────────┐
│                      【绑定阶段】                                 │
│                                                                  │
│  textBlock->SetBinding(TextProperty(), Binding().Path("Count")) │
│          ↓                                                       │
│  创建 BindingExpression                                          │
│          ↓                                                       │
│  订阅 viewModel->PropertyChanged()                               │
│          ↓                                                       │
│  UpdateTarget(): 获取初始值并显示                                │
│          ↓                                                       │
│  UI 显示 "0"                                                     │
└─────────────────────────────────────────────────────────────────┘
                              ↓
┌─────────────────────────────────────────────────────────────────┐
│                      【运行阶段】                                 │
│                                                                  │
│  用户点击按钮                                                     │
│          ↓                                                       │
│  viewModel->Increment()                                         │
│          ↓                                                       │
│  SetCount(1)                                                    │
│          ↓                                                       │
│  RaisePropertyChanged("Count") 【触发事件】                      │
│          ↓                                                       │
│  BindingExpression 收到通知                                      │
│          ↓                                                       │
│  UpdateTarget(): 重新获取值                                      │
│          ↓                                                       │
│  PropertyAccessorRegistry["Count"]->Get(viewModel) → 返回 1     │
│          ↓                                                       │
│  textBlock->SetValue(TextProperty(), "1")                       │
│          ↓                                                       │
│  InvalidateVisual() → 重绘                                       │
│          ↓                                                       │
│  UI 显示 "1" ✅                                                  │
└─────────────────────────────────────────────────────────────────┘
```

### TwoWay 绑定（ViewModel ⇄ UI）

```
┌──────────────────┐         ┌──────────────────┐
│   ViewModel      │         │   TextBox (UI)    │
├──────────────────┤         ├──────────────────┤
│ string userName_ │ ←──────→│ Text Property    │
│ GetUserName()    │         │                  │
│ SetUserName()    │         │                  │
└──────────────────┘         └──────────────────┘
        ↑                            ↑
        │                            │
        │    BindingExpression       │
        │    (Mode = TwoWay)         │
        │                            │
        └────── PropertyChanged ─────┘
                    事件
```

**双向流程**：

```
ViewModel → UI:
  SetUserName("Alice")
  → RaisePropertyChanged("UserName")
  → BindingExpression::OnSourcePropertyChanged()
  → UpdateTarget()
  → textBox->SetValue(TextProperty(), "Alice")

UI → ViewModel:
  用户输入 "Bob"
  → textBox->SetValue(TextProperty(), "Bob")
  → BindingExpression::OnTargetPropertyChanged()
  → UpdateSource()
  → PropertyAccessorRegistry["UserName"]->Set(viewModel, "Bob")
  → SetUserName("Bob")
```

---

## 🔑 核心组件速查

| 组件 | 职责 | 关键方法 |
|-----|------|---------|
| **ObservableObject** | ViewModel 基类 | `RaisePropertyChanged()` |
| **PropertyAccessorRegistry** | 属性反射系统 | `RegisterPropertyGetter()`, `FindAccessor()` |
| **Binding** | 绑定配置 | `Path()`, `Mode()`, `Source()` |
| **BindingExpression** | 绑定逻辑核心 | `Activate()`, `UpdateTarget()`, `UpdateSource()` |
| **BindingPath** | 路径解析 | `Resolve()` |
| **DependencyObject** | UI 控件基类 | `SetBinding()`, `SetDataContext()` |
| **PropertyStore** | 属性值存储 | `SetValue()`, `GetValue()` |

---

## 🚀 快速实现绑定

### Step 1: 定义 ViewModel

```cpp
#include "fk/binding/ObservableObject.h"
#include "fk/binding/ViewModelMacros.h"

class MyViewModel : public binding::ObservableObject {
public:
    // 使用宏自动生成属性
    FK_PROPERTY_CUSTOM(int, Count, {
        RaisePropertyChanged("CountText");
    })
    
public:
    std::string GetCountText() const {
        return "Count: " + std::to_string(GetCount());
    }
    
    void Increment() { SetCount(GetCount() + 1); }
};

// 自动注册属性
FK_VIEWMODEL_AUTO(MyViewModel, Count, CountText)
```

### Step 2: 创建 UI 和绑定

```cpp
int main() {
    // 1. 创建 ViewModel
    auto viewModel = std::make_shared<MyViewModel>();
    
    // 2. 创建窗口
    auto window = ui::window()->Title("MVVM Demo");
    
    // 3. 设置 DataContext
    window->SetDataContext(
        std::static_pointer_cast<binding::INotifyPropertyChanged>(viewModel)
    );
    
    // 4. 创建控件并绑定
    auto textBlock = ui::textBlock();
    textBlock->SetBinding(
        ui::detail::TextBlockBase::TextProperty(),
        binding::Binding().Path("CountText")
    );
    
    // 5. 创建按钮
    auto button = ui::button()->Content("Increment");
    button->Click += [viewModel](ui::detail::ButtonBase&) {
        viewModel->Increment();  // UI 自动更新！
    };
    
    // 6. 布局和运行
    // ...
    return app.Run(window);
}
```

---

## 🐛 常见问题一秒诊断

| 症状 | 可能原因 | 解决方法 |
|-----|---------|---------|
| UI 不更新 | 未调用 `RaisePropertyChanged` | 在 setter 中添加通知 |
| 编译错误：属性未注册 | 忘记 `FK_VIEWMODEL_AUTO` | 添加注册宏 |
| 运行时崩溃 | DataContext 类型错误 | 转换为 `shared_ptr<INotifyPropertyChanged>` |
| 计算属性不更新 | 未通知依赖属性 | 使用 `FK_PROPERTY_CUSTOM` 通知依赖 |
| 循环更新 | 属性相互依赖 | 检查依赖图，避免循环 |

---

## 📝 关键代码模板

### 模板 1：简单属性

```cpp
class MyViewModel : public binding::ObservableObject {
public:
    FK_PROPERTY(std::string, Name)
    FK_PROPERTY(int, Age)
};

FK_VIEWMODEL_AUTO(MyViewModel, Name, Age)
```

### 模板 2：带依赖的属性

```cpp
class PersonViewModel : public binding::ObservableObject {
public:
    FK_PROPERTY_CUSTOM(std::string, FirstName, {
        RaisePropertyChanged("FullName");
    })
    FK_PROPERTY_CUSTOM(std::string, LastName, {
        RaisePropertyChanged("FullName");
    })
    
public:
    std::string GetFullName() const {
        return GetFirstName() + " " + GetLastName();
    }
};

FK_VIEWMODEL_AUTO(PersonViewModel, FirstName, LastName, FullName)
```

### 模板 3：集合绑定（TODO）

```cpp
class ItemsViewModel : public binding::ObservableObject {
public:
    FK_PROPERTY(std::vector<std::string>, Items)
};

FK_VIEWMODEL_AUTO(ItemsViewModel, Items)

// UI
listBox->SetBinding(
    ListBox::ItemsSourceProperty(),
    binding::Binding().Path("Items")
);
```

---

## 🎓 学习路径

1. **入门** (5 分钟)
   - 阅读 `ViewModelMacros-QuickStart.md`
   - 运行 `simple_property_macro_test.cpp`

2. **理解原理** (15 分钟)
   - 阅读本文档的「数据流向图」
   - 理解注册 → 绑定 → 运行三阶段

3. **深入细节** (30 分钟)
   - 阅读 `MVVM-Binding-Workflow.md` 完整版
   - 查看源码：`BindingExpression.cpp`, `BindingPath.cpp`

4. **实战应用** (1 小时)
   - 构建自己的 ViewModel
   - 实现复杂的计算属性
   - 处理集合和验证

---

## 🔗 相关文档

- **宏使用指南**: `ViewModelMacros-QuickStart.md`
- **详细流程**: `MVVM-Binding-Workflow.md`
- **属性宏完整指南**: `ViewModelPropertyMacros-Guide.md`
- **示例代码**: `samples/mvvm_binding_demo/`

---

## 💡 设计哲学

F__K_UI 的绑定系统遵循以下原则：

1. **声明式优于命令式**
   ```cpp
   // ✅ 声明式绑定
   textBlock->SetBinding(TextProperty(), Binding().Path("Count"));
   
   // ❌ 命令式更新
   button->Click += [textBlock, vm]() {
       textBlock->SetText(std::to_string(vm->GetCount()));
   };
   ```

2. **自动化优于手动**
   - 宏自动生成属性
   - 自动注册访问器
   - 自动订阅事件

3. **类型安全优于灵活性**
   - 编译期类型检查
   - Lambda 包装器确保类型转换安全
   - `std::any` + RTTI 最小化运行时错误

4. **性能优先**
   - 预注册（零运行时反射开销）
   - 延迟绑定（按需激活）
   - 智能更新（值比较，避免无效通知）

---

现在您已经掌握了 F__K_UI 绑定系统的精髓！🎉

**记住三个关键步骤**：
1. 定义 ViewModel + 使用宏
2. 设置 DataContext
3. 创建 Binding

其他的都是自动的！✨
