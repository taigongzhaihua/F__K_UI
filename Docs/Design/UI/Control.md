# Control - 设计文档

## 概览

**目的**：为可模板化控件提供基础

## 设计目标

1. **模板支持** - ControlTemplate自定义外观
2. **样式系统** - 通过Style设置属性
3. **默认外观** - 提供合理的默认渲染
4. **扩展性** - 易于创建自定义控件

## 模板应用流程

```
1. Control创建
   ↓
2. 检查Template属性
   ↓
3. Template.LoadContent()
   ↓
4. 生成视觉树
   ↓
5. 应用TemplateBindings
   ↓
6. 添加到Control的视觉树
```

## 样式应用

**优先级**：
1. 本地值
2. Style设置器
3. 默认值

```cpp
void Control::ApplyStyle(Style* style) {
    for (auto& setter : style->Setters()) {
        // 只设置没有本地值的属性
        if (!HasLocalValue(setter->Property())) {
            SetValue(setter->Property(), setter->Value());
        }
    }
}
```

## 默认外观

派生类可以重写OnRender提供默认外观：

```cpp
void Button::OnRender(const RenderContext& context) {
    if (!HasTemplate()) {
        // 默认渲染
        RenderDefaultButton(context);
    } else {
        // 使用模板
        Control::OnRender(context);
    }
}
```

## 另请参阅

- [API文档](../../API/UI/Control.md)
- [FrameworkElement设计](FrameworkElement.md)
