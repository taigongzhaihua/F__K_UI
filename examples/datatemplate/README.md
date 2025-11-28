# DataTemplate 使用指南

## 概述

本目录包含关于如何在FK_UI框架中使用DataTemplate的示例代码，展示了如何在C++中实现类似WPF中DataTemplate的效果。

## 什么是DataTemplate？

DataTemplate（数据模板）是一种定义数据对象可视化表示的机制。在WPF中，DataTemplate允许你自定义任何数据对象在UI中的显示方式，而不需要让数据对象本身知道如何渲染。

### WPF中的DataTemplate示例

在WPF中，你可以这样定义DataTemplate：

```xml
<ListBox ItemsSource="{Binding Users}">
    <ListBox.ItemTemplate>
        <DataTemplate>
            <Border Background="LightBlue" Padding="10">
                <StackPanel>
                    <TextBlock Text="{Binding Name}" FontSize="18"/>
                    <TextBlock Text="{Binding Age}" FontSize="14"/>
                    <TextBlock Text="{Binding Email}" FontSize="14"/>
                </StackPanel>
            </Border>
        </DataTemplate>
    </ListBox.ItemTemplate>
</ListBox>
```

### FK_UI中的等效实现

在FK_UI中，我们使用C++代码实现相同的效果：

```cpp
auto dataTemplate = std::make_shared<DataTemplate>();

dataTemplate->SetFactory([](const std::any& dataContext) -> UIElement* {
    auto userData = std::any_cast<std::shared_ptr<UserData>>(dataContext);
    
    auto border = new Border();
    border->SetBackground(new SolidColorBrush(173, 216, 230)); // LightBlue
    border->SetPadding(Thickness(10));
    
    auto panel = new StackPanel();
    panel->SetOrientation(Orientation::Vertical);
    
    auto nameText = new TextBlock();
    nameText->SetText(userData->GetName());
    nameText->SetFontSize(18);
    panel->AddChild(nameText);
    
    auto ageText = new TextBlock();
    ageText->SetText(std::to_string(userData->GetAge()));
    ageText->SetFontSize(14);
    panel->AddChild(ageText);
    
    auto emailText = new TextBlock();
    emailText->SetText(userData->GetEmail());
    emailText->SetFontSize(14);
    panel->AddChild(emailText);
    
    border->SetChild(panel);
    return border;
});

// 在ListBox中使用
listBox->SetItemTemplate(dataTemplate.get());
```

## 核心概念

### 1. 数据与视图分离

DataTemplate实现了数据与视图的分离：
- **数据模型**：定义数据结构（如UserData类）
- **视图模板**：定义如何显示数据（DataTemplate）
- **控件**：使用模板显示数据（如ListBox）

### 2. Factory函数

DataTemplate的核心是Factory函数，它接收一个数据对象（通过std::any传递），并返回一个UIElement*：

```cpp
dataTemplate->SetFactory([](const std::any& dataContext) -> UIElement* {
    // 1. 从dataContext提取数据
    // 2. 创建UI元素
    // 3. 用数据填充UI元素
    // 4. 返回根UI元素
});
```

### 3. 类型安全

使用std::any_cast安全地提取数据：

```cpp
try {
    auto data = std::any_cast<MyDataType>(dataContext);
    // 使用data...
} catch (const std::bad_any_cast&) {
    // 处理类型不匹配
    return CreateErrorElement();
}
```

## 示例程序说明

### datatemplate_demo.cpp

这个示例包含：

1. **UserData类**：一个简单的数据模型，包含姓名、年龄和邮箱
2. **CreateUserDataTemplate()**：创建一个复杂的DataTemplate，显示用户信息
3. **CreateSimpleTextTemplate()**：创建一个简单的DataTemplate，只显示文本
4. **主程序**：演示如何使用DataTemplate实例化视觉树

### 运行示例

```bash
# 编译项目
cd build
make datatemplate_demo

# 运行示例
./datatemplate_demo
```

## 与WPF的对比

| 特性 | WPF | FK_UI |
|------|-----|-------|
| 定义方式 | XAML声明式 | C++代码 |
| 工厂函数 | 隐式 | 显式SetFactory |
| 数据绑定 | {Binding Path} | 手动在Factory中设置 |
| 类型系统 | .NET类型 | std::any + any_cast |
| 可视化树 | 自动创建 | Factory函数返回 |

## 实际应用场景

### 1. ListBox项目模板

```cpp
auto listBox = new ListBox();
listBox->SetItemTemplate(CreateUserDataTemplate().get());
listBox->GetItems().Add(std::make_shared<UserData>("张三", 28, "email"));
```

### 2. ContentControl内容模板

```cpp
auto contentControl = new ContentControl();
contentControl->SetContentTemplate(CreateUserDataTemplate().get());
contentControl->SetContent(userData);
```

### 3. 自定义数据展示

```cpp
// 为不同类型的数据创建不同的模板
auto imageTemplate = CreateImageDataTemplate();
auto textTemplate = CreateTextDataTemplate();
auto videoTemplate = CreateVideoDataTemplate();

// 根据数据类型选择合适的模板
control->SetTemplate(SelectTemplateByType(data));
```

## 高级技巧

### 1. 模板选择器

创建一个根据数据类型选择不同模板的选择器：

```cpp
class DataTemplateSelector {
public:
    virtual DataTemplate* SelectTemplate(const std::any& data) {
        if (data.type() == typeid(std::shared_ptr<UserData>)) {
            return userTemplate_.get();
        } else if (data.type() == typeid(std::string)) {
            return textTemplate_.get();
        }
        return defaultTemplate_.get();
    }
private:
    std::shared_ptr<DataTemplate> userTemplate_;
    std::shared_ptr<DataTemplate> textTemplate_;
    std::shared_ptr<DataTemplate> defaultTemplate_;
};
```

### 2. 模板复用

将常用的模板定义为可复用的函数：

```cpp
namespace Templates {
    std::shared_ptr<DataTemplate> CreateCardTemplate();
    std::shared_ptr<DataTemplate> CreateListItemTemplate();
    std::shared_ptr<DataTemplate> CreateTileTemplate();
}
```

### 3. 动态数据绑定

虽然FK_UI支持数据绑定系统，但在DataTemplate中，你也可以手动设置绑定：

```cpp
// 在Factory函数中
auto textBlock = new TextBlock();
auto binding = std::make_shared<Binding>();
binding->Path("Name")->Source(userData);
textBlock->SetBinding(TextBlock::TextProperty(), binding);
```

## 最佳实践

1. **保持Factory函数简洁**：将复杂的UI创建逻辑提取到独立函数中
2. **错误处理**：始终处理any_cast可能的失败情况
3. **内存管理**：注意UIElement的所有权，避免内存泄漏
4. **性能优化**：对于大量数据，考虑虚拟化和模板缓存
5. **类型安全**：使用强类型的包装类而不是裸指针

## 常见问题

### Q: DataTemplate和ControlTemplate有什么区别？

- **DataTemplate**：定义数据对象的可视化表示（数据→视图）
- **ControlTemplate**：定义控件本身的视觉结构（控件→外观）

### Q: 如何在DataTemplate中访问原始数据？

通过dataContext参数，使用std::any_cast提取数据：
```cpp
auto data = std::any_cast<MyType>(dataContext);
```

### Q: DataTemplate会自动更新吗？

如果你的数据类继承自ObservableObject并正确实现了属性变更通知，DataTemplate创建的UI元素可以响应数据变化（通过数据绑定）。

### Q: 可以在运行时动态切换DataTemplate吗？

可以。只需调用SetItemTemplate或SetContentTemplate设置新的模板，然后触发重新生成容器即可。

## 相关文档

- [API文档 - DataTemplate](../../Docs/API/UI/DataTemplate.md)
- [API文档 - ItemsControl](../../Docs/API/UI/ItemsControl.md)
- [API文档 - ContentPresenter](../../Docs/API/UI/ContentPresenter.md)
- [设计文档 - DataTemplate](../../Docs/Design/UI/DataTemplate.md)

## 总结

DataTemplate是FK_UI框架中实现数据驱动UI的关键机制。虽然它使用C++代码而不是声明式标记语言，但它提供了与WPF中DataTemplate相同的功能和灵活性。通过合理使用DataTemplate，你可以：

- 实现数据与视图的完全分离
- 创建可复用的UI模板
- 支持不同类型数据的多态显示
- 构建灵活且可维护的应用程序

**答案：是的，在C++中完全可以实现类似WPF中DataTemplate的效果！** FK_UI框架已经提供了DataTemplate类和完整的基础设施，让你能够在C++中享受数据模板化带来的便利。
