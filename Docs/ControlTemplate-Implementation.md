# ControlTemplate 系统实现

## 概述

已成功实现 ControlTemplate 系统的核心功能，遵循 WinUI/WPF 的设计理念。

## 实现的功能

### 1. Template 基类 (`include/fk/ui/Template.h`)

```cpp
class Template {
public:
    virtual std::shared_ptr<FrameworkElement> LoadContent() = 0;
};
```

- 定义了模板的基本接口
- 负责创建可视化树

### 2. ControlTemplate (`include/fk/ui/Template.h`)

```cpp
class ControlTemplate : public Template {
public:
    void SetVisualTreeFactory(std::function<std::shared_ptr<FrameworkElement>()> factory);
    std::shared_ptr<FrameworkElement> LoadContent() override;
    
    const std::type_info* GetTargetType() const;
    void SetTargetType(const std::type_info* type);
};
```

**功能：**
- 允许通过 `SetVisualTreeFactory` 设置可视化树工厂函数
- `LoadContent()` 从工厂函数创建实际的可视化树
- 支持目标类型检查（用于样式系统）

### 3. DataTemplate (`include/fk/ui/Template.h`)

```cpp
class DataTemplate : public Template {
public:
    void SetVisualTreeFactory(std::function<std::shared_ptr<FrameworkElement>()> factory);
    std::shared_ptr<FrameworkElement> LoadContent() override;
};
```

**用途：** 用于 ItemsControl 等控件的数据可视化

### 4. Control 中的 Template 支持 (`include/fk/ui/Control.h`)

**新增方法：**

```cpp
class ControlBase : public FrameworkElement {
public:
    // 设置控件模板
    void SetTemplate(std::shared_ptr<ControlTemplate> controlTemplate);
    
    // 获取当前模板
    std::shared_ptr<ControlTemplate> GetTemplate() const;
    
    // 应用模板（从模板创建可视化树）
    bool ApplyTemplate();
    
    // 模板应用后的回调（子类重写以获取命名部件）
    virtual void OnApplyTemplate();
    
private:
    std::shared_ptr<ControlTemplate> template_;
    bool templateApplied_{false};
};
```

**实现逻辑：**
1. `SetTemplate()` - 设置模板并自动调用 `ApplyTemplate()`
2. `ApplyTemplate()` - 从模板创建可视化树，设置为 Content
3. `OnApplyTemplate()` - 回调通知子类模板已应用

## 使用示例

### 基本用法

```cpp
// 创建自定义按钮模板
auto buttonTemplate = std::make_shared<ControlTemplate>();
buttonTemplate->SetVisualTreeFactory([]() -> std::shared_ptr<FrameworkElement> {
    auto panel = ui::stackPanel();
    panel->SetBackground("#4CAF50");
    panel->SetPadding(fk::Thickness{16.0f, 8.0f});
    
    auto text = ui::textBlock();
    text->SetText("自定义按钮");
    text->SetForeground("#FFFFFF");
    
    panel->AddChild(text);
    return panel;
});

// 应用到控件
auto button = std::make_shared<Button>();
button->SetTemplate(buttonTemplate);
// ApplyTemplate() 会自动调用，创建可视化树
```

### 子类自定义模板处理

```cpp
class CustomButton : public Button {
protected:
    void OnApplyTemplate() override {
        Button::OnApplyTemplate();
        
        // 在这里可以获取模板中的命名部件
        // 例如：textBlock_ = FindTemplateChild<TextBlock>("PART_Text");
    }
};
```

## 设计优势

1. **关注点分离**：控件逻辑与视觉表现完全分离
2. **高度可定制**：可以完全替换控件的视觉结构
3. **符合 MVVM 模式**：支持数据驱动的UI设计
4. **扩展性强**：为未来的样式系统（Style）和主题系统（Theme）奠定基础

## 下一步计划

基于已实现的 ControlTemplate，可以进一步实现：

1. **命名部件系统** - `FindTemplateChild<T>(name)` 查找模板中的元素
2. **TemplateBinding** - 将模板内部元素属性绑定到控件属性
3. **Style 系统** - 定义可重用的属性设置和模板集合
4. **主题系统** - 应用级别的视觉风格管理
5. **触发器（Triggers）** - 根据属性值自动改变外观

## TextBox 使用 ControlTemplate 重构

有了 ControlTemplate 系统，TextBox 可以这样重构：

```cpp
// TextBox 的默认模板
auto textBoxTemplate = std::make_shared<ControlTemplate>();
textBoxTemplate->SetVisualTreeFactory([]() {
    // Border 作为外框
    auto border = ui::border();
    border->SetBorderBrush("#CCCCCC");
    border->SetBorderThickness(1.0f);
    border->SetBackground("#FFFFFF");
    
    // ScrollViewer 提供滚动功能
    auto scrollViewer = ui::scrollViewer();
    scrollViewer->SetVerticalScrollBarVisibility(ScrollBarVisibility::Auto);
    scrollViewer->SetHorizontalScrollBarVisibility(ScrollBarVisibility::Disabled);
    
    // TextBlock 显示实际文本
    auto textPresenter = ui::textBlock();
    // TemplateBinding: textPresenter->Text 绑定到 TextBox::Text
    
    scrollViewer->SetContent(textPresenter);
    border->SetChild(scrollViewer);
    
    return border;
});

textBox->SetTemplate(textBoxTemplate);
```

这种方式下：
- 滚动由 ScrollViewer 处理（无需手动实现）
- 边框由 Border 处理
- TextBox 只需关注文本编辑逻辑（光标、选择、输入）
- 完全符合"组合优于继承"的设计原则

## 总结

ControlTemplate 系统已成功实现并集成到 Control 基类中。这为构建高度可定制、易于维护的 UI 框架奠定了坚实基础，使框架更接近 WinUI/WPF 的成熟设计模式。
