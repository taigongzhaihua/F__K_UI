# DependencyObject - 设计文档

## 概览

**目的**：为依赖属性系统、数据绑定和逻辑树关系提供基础基类。

**命名空间**：`fk::binding`

**头文件**：`fk/binding/DependencyObject.h`

## 架构

### 核心职责

1. **属性存储** - 通过 PropertyStore 管理依赖属性值
2. **绑定管理** - 通过 BindingContext 管理数据绑定
3. **变更通知** - 属性变更时触发事件
4. **值优先级** - 实现属性值优先级系统
5. **逻辑树** - 管理父子关系

### 类设计

```cpp
class DependencyObject : public std::enable_shared_from_this<DependencyObject> {
public:
    // 属性访问
    template<typename T>
    T GetValue(const DependencyProperty& property) const;
    
    void SetValue(const DependencyProperty& property, std::any value);
    void ClearValue(const DependencyProperty& property);
    
    // 绑定支持
    void SetBinding(const DependencyProperty& property, const Binding& binding);
    
    // 逻辑树
    void SetLogicalParent(DependencyObject* parent);
    DependencyObject* GetLogicalParent() const;
    void AddLogicalChild(DependencyObject* child);
    
    // 事件
    core::Event<const DependencyPropertyChangedEventArgs&> PropertyChanged;
    
private:
    std::unique_ptr<PropertyStore> propertyStore_;
    std::unique_ptr<BindingContext> bindingContext_;
    DependencyObject* logicalParent_;
    std::vector<DependencyObject*> logicalChildren_;
};
```

## 设计决策

### 1. 值存储

**决策**：使用 PropertyStore 作为单独的组件

**理由**：
- 关注点分离 - DependencyObject 专注于 API
- 可测试性 - PropertyStore 可以独立测试
- 灵活性 - 可以更改存储实现

**替代方案**：
- 直接在 DependencyObject 中存储 - 更简单但耦合度更高
- 静态存储 - 节省内存但限制灵活性

### 2. 绑定管理

**决策**：使用 BindingContext 管理绑定

**理由**：
- 关注点分离 - 绑定逻辑与属性逻辑分离
- 生命周期管理 - BindingContext 拥有 BindingExpressions
- 清理 - 销毁时自动断开绑定

### 3. 值优先级

**决策**：在 PropertyStore 中实现优先级

**理由**：
- 封装 - 隐藏 DependencyObject 的复杂性
- 性能 - O(1) 查找当前有效值
- 可扩展性 - 可以轻松添加新的优先级层

**优先级顺序**（从高到低）：
1. 动画值（未来）
2. 本地值
3. 模板父值（未来）
4. 样式值
5. 继承值
6. 默认值

### 4. 逻辑树 vs. 视觉树

**决策**：在 DependencyObject 中维护逻辑树

**理由**：
- DataContext 继承需要逻辑树
- 元素名称查找使用逻辑树
- 独立于可视化 - 非视觉对象可以参与

**逻辑树**：
- 内容关系（ContentControl.Content）
- 子元素（Panel.Children）
- 模板化父关系

**视觉树**（在 Visual 中）：
- 实际的渲染层次结构
- 包括模板元素
- 用于命中测试和渲染

### 5. 弱引用 vs. 强引用

**决策**：树关系使用原始指针（弱）

**理由**：
- 避免循环引用 - 父和子不互相拥有
- 公共 API 使用 shared_ptr - 用户控制生命周期
- 清理 - 销毁时自动从树中移除

## 实现详情

### 属性值解析

```cpp
template<typename T>
T DependencyObject::GetValue(const DependencyProperty& property) const {
    // 1. 检查本地值
    if (auto localValue = propertyStore_->GetLocalValue(property)) {
        return std::any_cast<T>(*localValue);
    }
    
    // 2. 检查绑定值
    if (auto bindingExpr = bindingContext_->GetBinding(property)) {
        return std::any_cast<T>(bindingExpr->GetValue());
    }
    
    // 3. 检查样式值
    if (auto styleValue = GetStyleValue(property)) {
        return std::any_cast<T>(*styleValue);
    }
    
    // 4. 检查继承值
    if (property.Metadata()->Inherits) {
        if (auto inherited = GetInheritedValue(property)) {
            return std::any_cast<T>(*inherited);
        }
    }
    
    // 5. 返回默认值
    return std::any_cast<T>(property.Metadata()->DefaultValue);
}
```

### 属性变更传播

```cpp
void DependencyObject::SetValue(const DependencyProperty& property, std::any value) {
    // 获取旧值
    auto oldValue = GetValue(property);
    
    // 强制值（如果已注册强制回调）
    if (auto coerceCallback = property.Metadata()->CoerceValueCallback) {
        value = coerceCallback(this, value);
    }
    
    // 存储新值
    propertyStore_->SetLocalValue(property, value);
    
    // 触发变更回调
    if (auto changedCallback = property.Metadata()->PropertyChangedCallback) {
        DependencyPropertyChangedEventArgs args(property, oldValue, value);
        changedCallback(this, args);
    }
    
    // 触发 PropertyChanged 事件
    PropertyChanged.Invoke(DependencyPropertyChangedEventArgs(property, oldValue, value));
    
    // 使继承该属性的子元素失效
    InvalidateInheritingChildren(property);
}
```

### 绑定集成

```cpp
void DependencyObject::SetBinding(const DependencyProperty& property, const Binding& binding) {
    // 创建绑定表达式
    auto expr = std::make_shared<BindingExpression>(binding, this, property);
    
    // 解析绑定源
    expr->ResolveSource();
    
    // 存储在绑定上下文中
    bindingContext_->SetBinding(property, expr);
    
    // 订阅源变更
    expr->Attach();
    
    // 初始更新
    expr->UpdateTarget();
}
```

## 内存管理

### 所有权模型

1. **PropertyStore** - 由 DependencyObject 拥有（unique_ptr）
2. **BindingContext** - 由 DependencyObject 拥有（unique_ptr）
3. **BindingExpressions** - 由 BindingContext 拥有（shared_ptr）
4. **逻辑父/子** - 弱引用（原始指针）

### 生命周期

```
创建 DependencyObject
    ↓
设置属性和绑定
    ↓
添加到逻辑树
    ↓
属性变更和更新
    ↓
从逻辑树中移除
    ↓
清理绑定（BindingContext 析构）
    ↓
销毁 DependencyObject
```

### 循环引用预防

- 树关系不持有强引用
- 绑定在销毁时自动断开
- 事件连接使用弱引用

## 线程模型

### 线程关联

所有 DependencyObject 操作必须在创建线程上进行：

```cpp
void DependencyObject::VerifyAccess() const {
    if (std::this_thread::get_id() != creationThreadId_) {
        throw std::runtime_error("跨线程访问不允许");
    }
}
```

### 线程安全访问

使用 Dispatcher 进行跨线程访问：

```cpp
dispatcher->InvokeAsync([obj, value]() {
    obj->SetValue(Property, value);
});
```

## 性能优化

### 属性访问

- **哈希映射查找**：O(1) 平均情况
- **值缓存**：当前有效值已缓存
- **延迟绑定解析**：首次访问时解析

### 内存占用

典型的 DependencyObject：
- 基础开销：~200 字节
- 每个设置的属性：~48 字节
- 每个活动绑定：~100 字节

### 优化技术

1. **小对象优化** - 在 PropertyStore 中内联小值
2. **写时复制** - 继承值不复制
3. **绑定延迟** - 延迟绑定源解析
4. **批量失效** - 将多个变更合并为单次更新

## 测试策略

### 单元测试

```cpp
TEST(DependencyObjectTest, SetGetValue) {
    auto obj = std::make_shared<TestObject>();
    obj->SetValue(TestProperty, 42);
    EXPECT_EQ(obj->GetValue<int>(TestProperty), 42);
}

TEST(DependencyObjectTest, PropertyChanged) {
    auto obj = std::make_shared<TestObject>();
    bool changed = false;
    obj->PropertyChanged += [&](const auto& e) {
        changed = true;
    };
    obj->SetValue(TestProperty, 42);
    EXPECT_TRUE(changed);
}

TEST(DependencyObjectTest, ValuePriority) {
    auto obj = std::make_shared<TestObject>();
    
    // 设置默认值
    EXPECT_EQ(obj->GetValue<int>(TestProperty), 0);
    
    // 设置本地值 - 覆盖默认值
    obj->SetValue(TestProperty, 42);
    EXPECT_EQ(obj->GetValue<int>(TestProperty), 42);
    
    // 清除 - 恢复默认值
    obj->ClearValue(TestProperty);
    EXPECT_EQ(obj->GetValue<int>(TestProperty), 0);
}
```

### 集成测试

```cpp
TEST(DependencyObjectTest, DataBinding) {
    auto viewModel = std::make_shared<ViewModel>();
    auto element = std::make_shared<UIElement>();
    
    element->SetBinding(UIElement::WidthProperty(),
                       Binding("Width").Source(viewModel));
    
    viewModel->SetWidth(200);
    EXPECT_EQ(element->GetValue<double>(UIElement::WidthProperty()), 200);
}
```

## 未来增强

1. **动画支持** - 添加动画值优先级
2. **值强制** - 完善强制回调系统
3. **附加属性** - 支持附加的依赖属性
4. **属性触发器** - 基于属性值的触发器
5. **性能分析** - 添加性能计数器和分析

## 另请参阅

- [API 文档](../../API/Binding/DependencyObject.md)
- [DependencyProperty 设计](DependencyProperty.md)
- [Binding 设计](Binding.md)
- [PropertyStore 设计](PropertyStore.md)
