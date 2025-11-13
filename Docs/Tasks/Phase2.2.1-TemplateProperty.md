# Phase 2.2.1: Template DependencyProperty 集成

## 目标

将 Control 类中的 Template 属性从简单成员变量转换为 DependencyProperty，实现数据绑定支持并与属性系统正确集成。

## 背景

当前 Template 是一个普通指针成员 `ControlTemplate* template_`。需要将其转换为 DependencyProperty 以支持：
- 数据绑定
- 样式设置器
- 属性变更通知
- 值继承

## 实现计划

### 1. 添加 TemplateProperty

```cpp
class Control {
public:
    static const DependencyProperty& TemplateProperty();
    
protected:
    virtual void OnTemplateChanged(
        const DependencyPropertyChangedEventArgs& e);
};
```

### 2. 属性元数据

```cpp
DependencyProperty::Register<Control, std::shared_ptr<ControlTemplate>>(
    "Template",
    PropertyMetadata::Create(
        nullptr,
        &Control::OnTemplateChanged
    )
);
```

### 3. 自动模板应用

在 `OnTemplateChanged` 中：
1. 撤销旧模板
2. 应用新模板
3. 触发重新渲染

## 成功标准

- [ ] Template 是 DependencyProperty
- [ ] 支持数据绑定
- [ ] 自动应用/撤销
- [ ] 所有测试通过

## 状态

**已完成** ✅
