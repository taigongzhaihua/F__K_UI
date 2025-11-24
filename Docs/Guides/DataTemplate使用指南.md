# DataTemplate 使用指南

## 目录

1. [简介](#简介)
2. [为什么需要DataTemplate](#为什么需要datatemplate)
3. [WPF与FK_UI对比](#wpf与fk_ui对比)
4. [基本用法](#基本用法)
5. [进阶用法](#进阶用法)
6. [实战示例](#实战示例)
7. [常见问题](#常见问题)

## 简介

DataTemplate（数据模板）是一种在UI框架中定义数据对象如何显示的机制。它允许开发者将数据的表示逻辑从数据本身分离出来，实现真正的数据驱动UI。

**核心理念**：数据只需要存储和管理业务逻辑，而如何显示数据则完全由DataTemplate决定。

## 为什么需要DataTemplate

### 问题场景

假设你有一个用户列表，需要在UI中显示。传统方法可能是：

```cpp
// 不好的做法：数据和视图耦合
class User {
public:
    std::string name;
    int age;
    
    // 数据类不应该知道如何渲染自己！
    UIElement* CreateUI() {
        auto panel = new StackPanel();
        auto nameLabel = new TextBlock();
        nameLabel->SetText("Name: " + name);
        panel->AddChild(nameLabel);
        // ...
        return panel;
    }
};
```

这种方法的问题：
- ❌ 数据类和UI耦合
- ❌ 难以在不同场景使用不同显示方式
- ❌ 违反单一职责原则
- ❌ 难以测试和维护

### 使用DataTemplate的方法

```cpp
// 好的做法：数据和视图分离
class User {
public:
    std::string name;
    int age;
    // 只关注数据，不关心如何显示
};

// UI显示逻辑由DataTemplate定义
auto template = CreateUserCardTemplate();  // 卡片样式
// 或
auto template = CreateUserListTemplate();   // 列表样式
// 或
auto template = CreateUserTileTemplate();   // 磁贴样式
```

优点：
- ✅ 数据类只关注数据
- ✅ 可以为同一数据创建多种显示方式
- ✅ 易于复用和维护
- ✅ 符合MVVM模式

## WPF与FK_UI对比

### WPF中的DataTemplate

在WPF中，DataTemplate通常使用XAML定义：

```xml
<DataTemplate x:Key="UserTemplate">
    <Border Background="LightBlue" Padding="10" Margin="5">
        <StackPanel>
            <TextBlock Text="{Binding Name}" FontSize="18" FontWeight="Bold"/>
            <TextBlock Text="{Binding Age}" FontSize="14"/>
            <TextBlock Text="{Binding Email}" FontSize="14" Foreground="Gray"/>
        </StackPanel>
    </Border>
</DataTemplate>

<ListBox ItemsSource="{Binding Users}" ItemTemplate="{StaticResource UserTemplate}"/>
```

### FK_UI中的等效实现

在FK_UI中，使用C++代码实现：

```cpp
auto CreateUserTemplate() {
    auto dataTemplate = std::make_shared<DataTemplate>();
    
    dataTemplate->SetFactory([](const std::any& dataContext) -> UIElement* {
        // 提取数据
        auto user = std::any_cast<std::shared_ptr<User>>(dataContext);
        
        // 创建视觉树
        auto border = new Border();
        border->SetBackground(new SolidColorBrush(173, 216, 230));
        border->SetPadding(Thickness(10));
        border->SetMargin(Thickness(5));
        
        auto panel = new StackPanel();
        panel->SetOrientation(Orientation::Vertical);
        
        auto nameText = new TextBlock();
        nameText->SetText(user->name);
        nameText->SetFontSize(18);
        panel->AddChild(nameText);
        
        auto ageText = new TextBlock();
        ageText->SetText(std::to_string(user->age));
        ageText->SetFontSize(14);
        panel->AddChild(ageText);
        
        auto emailText = new TextBlock();
        emailText->SetText(user->email);
        emailText->SetFontSize(14);
        emailText->SetForeground(new SolidColorBrush(128, 128, 128));
        panel->AddChild(emailText);
        
        border->SetChild(panel);
        return border;
    });
    
    return dataTemplate;
}

// 使用
auto listBox = new ListBox();
listBox->SetItemTemplate(CreateUserTemplate().get());
```

### 对比表

| 方面 | WPF | FK_UI |
|------|-----|-------|
| 定义方式 | XAML（声明式） | C++代码（命令式） |
| 数据绑定 | {Binding Path} | 手动设置或使用Binding类 |
| 类型系统 | CLR类型 | std::any + any_cast |
| 工厂函数 | 隐式（由框架处理） | 显式SetFactory |
| 编译时检查 | 较弱（XAML） | 强类型检查 |
| 智能提示 | 一般 | 完整的IDE支持 |
| 学习曲线 | 需要学习XAML | 只需C++知识 |

## 基本用法

### 1. 创建DataTemplate

```cpp
auto dataTemplate = std::make_shared<DataTemplate>();
```

### 2. 设置Factory函数

Factory函数是DataTemplate的核心，它定义了如何将数据转换为UI元素：

```cpp
dataTemplate->SetFactory([](const std::any& dataContext) -> UIElement* {
    // 步骤1：提取数据
    auto data = std::any_cast<MyDataType>(dataContext);
    
    // 步骤2：创建UI元素
    auto element = new TextBlock();
    
    // 步骤3：用数据填充UI
    element->SetText(data.GetDisplayText());
    
    // 步骤4：返回UI元素
    return element;
});
```

### 3. 使用DataTemplate

#### 在ItemsControl中使用

```cpp
auto listBox = new ListBox();
listBox->SetItemTemplate(dataTemplate.get());

// 添加数据
listBox->GetItems().Add(myData1);
listBox->GetItems().Add(myData2);
```

#### 在ContentControl中使用

```cpp
auto contentControl = new ContentControl();
contentControl->SetContentTemplate(dataTemplate.get());
contentControl->SetContent(myData);
```

#### 手动实例化

```cpp
UIElement* visualElement = dataTemplate->Instantiate(myData);
parentPanel->AddChild(visualElement);
```

### 4. 类型安全处理

始终使用try-catch处理类型转换：

```cpp
dataTemplate->SetFactory([](const std::any& dataContext) -> UIElement* {
    try {
        auto data = std::any_cast<std::shared_ptr<MyData>>(dataContext);
        // 正常处理...
        return CreateUIForData(data);
    } catch (const std::bad_any_cast& e) {
        // 类型不匹配，返回错误提示
        auto errorText = new TextBlock();
        errorText->SetText("数据类型错误");
        errorText->SetForeground(new SolidColorBrush(255, 0, 0));
        return errorText;
    }
});
```

## 进阶用法

### 1. 复杂视觉树

创建复杂的UI结构：

```cpp
auto CreateComplexTemplate() {
    auto dataTemplate = std::make_shared<DataTemplate>();
    
    dataTemplate->SetFactory([](const std::any& dataContext) -> UIElement* {
        auto product = std::any_cast<std::shared_ptr<Product>>(dataContext);
        
        // 根容器
        auto border = new Border();
        border->SetBackground(new SolidColorBrush(255, 255, 255));
        border->SetBorderBrush(new SolidColorBrush(200, 200, 200));
        border->SetBorderThickness(Thickness(1));
        border->SetCornerRadius(CornerRadius(5));
        border->SetPadding(Thickness(15));
        
        // 主布局：水平排列
        auto mainPanel = new StackPanel();
        mainPanel->SetOrientation(Orientation::Horizontal);
        mainPanel->SetSpacing(15);
        
        // 左侧：图片
        auto image = new Image();
        image->SetSource(product->imagePath);
        image->SetWidth(80);
        image->SetHeight(80);
        mainPanel->AddChild(image);
        
        // 右侧：信息面板
        auto infoPanel = new StackPanel();
        infoPanel->SetOrientation(Orientation::Vertical);
        infoPanel->SetSpacing(5);
        
        // 产品名称
        auto nameText = new TextBlock();
        nameText->SetText(product->name);
        nameText->SetFontSize(18);
        nameText->SetFontWeight(FontWeight::Bold);
        infoPanel->AddChild(nameText);
        
        // 价格
        auto pricePanel = new StackPanel();
        pricePanel->SetOrientation(Orientation::Horizontal);
        pricePanel->SetSpacing(10);
        
        auto priceLabel = new TextBlock();
        priceLabel->SetText("价格:");
        pricePanel->AddChild(priceLabel);
        
        auto priceValue = new TextBlock();
        priceValue->SetText("¥" + std::to_string(product->price));
        priceValue->SetForeground(new SolidColorBrush(255, 0, 0));
        priceValue->SetFontSize(20);
        pricePanel->AddChild(priceValue);
        
        infoPanel->AddChild(pricePanel);
        
        // 描述
        auto descText = new TextBlock();
        descText->SetText(product->description);
        descText->SetForeground(new SolidColorBrush(128, 128, 128));
        descText->SetTextWrapping(TextWrapping::Wrap);
        descText->SetMaxWidth(300);
        infoPanel->AddChild(descText);
        
        mainPanel->AddChild(infoPanel);
        border->SetChild(mainPanel);
        
        return border;
    });
    
    return dataTemplate;
}
```

### 2. 数据绑定集成

结合FK_UI的数据绑定系统：

```cpp
dataTemplate->SetFactory([](const std::any& dataContext) -> UIElement* {
    auto viewModel = std::any_cast<std::shared_ptr<ViewModel>>(dataContext);
    
    auto textBlock = new TextBlock();
    
    // 创建绑定
    auto binding = std::make_shared<Binding>();
    binding->Path("Title")->Source(viewModel);
    
    // 应用绑定
    textBlock->SetBinding(TextBlock::TextProperty(), binding);
    
    return textBlock;
});
```

### 3. 模板选择器

根据数据类型动态选择模板：

```cpp
class DataTemplateSelector {
public:
    DataTemplateSelector() {
        userTemplate_ = CreateUserTemplate();
        productTemplate_ = CreateProductTemplate();
        defaultTemplate_ = CreateDefaultTemplate();
    }
    
    DataTemplate* SelectTemplate(const std::any& data) {
        if (data.type() == typeid(std::shared_ptr<User>)) {
            return userTemplate_.get();
        } else if (data.type() == typeid(std::shared_ptr<Product>)) {
            return productTemplate_.get();
        }
        return defaultTemplate_.get();
    }
    
private:
    std::shared_ptr<DataTemplate> userTemplate_;
    std::shared_ptr<DataTemplate> productTemplate_;
    std::shared_ptr<DataTemplate> defaultTemplate_;
};

// 使用
DataTemplateSelector selector;
for (const auto& item : mixedDataList) {
    auto* template = selector.SelectTemplate(item);
    auto* visual = template->Instantiate(item);
    panel->AddChild(visual);
}
```

### 4. 模板库

创建可复用的模板库：

```cpp
namespace UITemplates {
    // 卡片样式模板
    std::shared_ptr<DataTemplate> CreateCardTemplate(
        const std::function<std::string(const std::any&)>& titleGetter,
        const std::function<std::string(const std::any&)>& descGetter
    ) {
        auto dataTemplate = std::make_shared<DataTemplate>();
        
        dataTemplate->SetFactory([titleGetter, descGetter](const std::any& data) -> UIElement* {
            auto border = new Border();
            border->SetBackground(new SolidColorBrush(250, 250, 250));
            border->SetBorderBrush(new SolidColorBrush(200, 200, 200));
            border->SetBorderThickness(Thickness(1));
            border->SetCornerRadius(CornerRadius(8));
            border->SetPadding(Thickness(15));
            border->SetMargin(Thickness(5));
            
            auto panel = new StackPanel();
            panel->SetOrientation(Orientation::Vertical);
            panel->SetSpacing(8);
            
            auto title = new TextBlock();
            title->SetText(titleGetter(data));
            title->SetFontSize(16);
            title->SetFontWeight(FontWeight::Bold);
            panel->AddChild(title);
            
            auto desc = new TextBlock();
            desc->SetText(descGetter(data));
            desc->SetFontSize(14);
            desc->SetForeground(new SolidColorBrush(100, 100, 100));
            panel->AddChild(desc);
            
            border->SetChild(panel);
            return border;
        });
        
        return dataTemplate;
    }
    
    // 列表项样式模板
    std::shared_ptr<DataTemplate> CreateListItemTemplate(/* ... */) {
        // ...
    }
    
    // 磁贴样式模板
    std::shared_ptr<DataTemplate> CreateTileTemplate(/* ... */) {
        // ...
    }
}

// 使用模板库
auto cardTemplate = UITemplates::CreateCardTemplate(
    [](const std::any& data) { 
        auto user = std::any_cast<std::shared_ptr<User>>(data);
        return user->name; 
    },
    [](const std::any& data) { 
        auto user = std::any_cast<std::shared_ptr<User>>(data);
        return user->email; 
    }
);
```

## 实战示例

### 示例1：通讯录应用

```cpp
class Contact {
public:
    std::string name;
    std::string phone;
    std::string avatar;  // 头像路径
    std::string group;   // 分组
};

auto CreateContactTemplate() {
    auto dataTemplate = std::make_shared<DataTemplate>();
    
    dataTemplate->SetFactory([](const std::any& dataContext) -> UIElement* {
        auto contact = std::any_cast<std::shared_ptr<Contact>>(dataContext);
        
        auto border = new Border();
        border->SetPadding(Thickness(10));
        
        auto panel = new StackPanel();
        panel->SetOrientation(Orientation::Horizontal);
        panel->SetSpacing(10);
        
        // 头像
        auto avatarBorder = new Border();
        avatarBorder->SetWidth(50);
        avatarBorder->SetHeight(50);
        avatarBorder->SetCornerRadius(CornerRadius(25));  // 圆形
        avatarBorder->SetBackground(new SolidColorBrush(200, 200, 200));
        // TODO: 添加实际头像图片
        panel->AddChild(avatarBorder);
        
        // 信息
        auto infoPanel = new StackPanel();
        infoPanel->SetOrientation(Orientation::Vertical);
        
        auto nameText = new TextBlock();
        nameText->SetText(contact->name);
        nameText->SetFontSize(16);
        infoPanel->AddChild(nameText);
        
        auto phoneText = new TextBlock();
        phoneText->SetText(contact->phone);
        phoneText->SetFontSize(14);
        phoneText->SetForeground(new SolidColorBrush(128, 128, 128));
        infoPanel->AddChild(phoneText);
        
        panel->AddChild(infoPanel);
        border->SetChild(panel);
        
        return border;
    });
    
    return dataTemplate;
}
```

### 示例2：消息列表

```cpp
class Message {
public:
    std::string sender;
    std::string content;
    std::string time;
    bool isRead;
    bool isMine;  // 是否是自己发的
};

auto CreateMessageTemplate() {
    auto dataTemplate = std::make_shared<DataTemplate>();
    
    dataTemplate->SetFactory([](const std::any& dataContext) -> UIElement* {
        auto message = std::any_cast<std::shared_ptr<Message>>(dataContext);
        
        auto panel = new StackPanel();
        panel->SetOrientation(Orientation::Vertical);
        panel->SetMargin(Thickness(10));
        
        // 发送者和时间
        auto headerPanel = new StackPanel();
        headerPanel->SetOrientation(Orientation::Horizontal);
        headerPanel->SetSpacing(10);
        
        auto senderText = new TextBlock();
        senderText->SetText(message->sender);
        senderText->SetFontWeight(FontWeight::Bold);
        headerPanel->AddChild(senderText);
        
        auto timeText = new TextBlock();
        timeText->SetText(message->time);
        timeText->SetFontSize(12);
        timeText->SetForeground(new SolidColorBrush(150, 150, 150));
        headerPanel->AddChild(timeText);
        
        panel->AddChild(headerPanel);
        
        // 消息内容
        auto contentBorder = new Border();
        contentBorder->SetPadding(Thickness(10));
        contentBorder->SetCornerRadius(CornerRadius(5));
        
        // 根据是否是自己的消息设置不同样式
        if (message->isMine) {
            contentBorder->SetBackground(new SolidColorBrush(173, 216, 230)); // 浅蓝色
            panel->SetHorizontalAlignment(HorizontalAlignment::Right);
        } else {
            contentBorder->SetBackground(new SolidColorBrush(240, 240, 240)); // 浅灰色
            panel->SetHorizontalAlignment(HorizontalAlignment::Left);
        }
        
        auto contentText = new TextBlock();
        contentText->SetText(message->content);
        contentText->SetTextWrapping(TextWrapping::Wrap);
        contentBorder->SetChild(contentText);
        
        panel->AddChild(contentBorder);
        
        return panel;
    });
    
    return dataTemplate;
}
```

### 示例3：商品展示

```cpp
class Product {
public:
    std::string name;
    double price;
    double originalPrice;  // 原价
    std::string imageUrl;
    int sales;  // 销量
    double rating;  // 评分
};

auto CreateProductTemplate() {
    auto dataTemplate = std::make_shared<DataTemplate>();
    
    dataTemplate->SetFactory([](const std::any& dataContext) -> UIElement* {
        auto product = std::any_cast<std::shared_ptr<Product>>(dataContext);
        
        auto border = new Border();
        border->SetWidth(200);
        border->SetBackground(new SolidColorBrush(255, 255, 255));
        border->SetBorderBrush(new SolidColorBrush(230, 230, 230));
        border->SetBorderThickness(Thickness(1));
        border->SetCornerRadius(CornerRadius(8));
        border->SetPadding(Thickness(10));
        
        auto panel = new StackPanel();
        panel->SetOrientation(Orientation::Vertical);
        panel->SetSpacing(8);
        
        // 商品图片
        auto image = new Image();
        image->SetSource(product->imageUrl);
        image->SetWidth(180);
        image->SetHeight(180);
        image->SetStretch(Stretch::UniformToFill);
        panel->AddChild(image);
        
        // 商品名称
        auto nameText = new TextBlock();
        nameText->SetText(product->name);
        nameText->SetFontSize(14);
        nameText->SetTextWrapping(TextWrapping::Wrap);
        nameText->SetMaxLines(2);
        panel->AddChild(nameText);
        
        // 价格区域
        auto pricePanel = new StackPanel();
        pricePanel->SetOrientation(Orientation::Horizontal);
        pricePanel->SetSpacing(5);
        
        auto priceText = new TextBlock();
        priceText->SetText("¥" + std::to_string(product->price));
        priceText->SetFontSize(18);
        priceText->SetForeground(new SolidColorBrush(255, 0, 0));
        priceText->SetFontWeight(FontWeight::Bold);
        pricePanel->AddChild(priceText);
        
        if (product->originalPrice > product->price) {
            auto originalPriceText = new TextBlock();
            originalPriceText->SetText("¥" + std::to_string(product->originalPrice));
            originalPriceText->SetFontSize(12);
            originalPriceText->SetForeground(new SolidColorBrush(150, 150, 150));
            originalPriceText->SetTextDecorations(TextDecorations::Strikethrough);
            pricePanel->AddChild(originalPriceText);
        }
        
        panel->AddChild(pricePanel);
        
        // 销量和评分
        auto infoPanel = new StackPanel();
        infoPanel->SetOrientation(Orientation::Horizontal);
        infoPanel->SetSpacing(10);
        
        auto salesText = new TextBlock();
        salesText->SetText("销量: " + std::to_string(product->sales));
        salesText->SetFontSize(12);
        salesText->SetForeground(new SolidColorBrush(128, 128, 128));
        infoPanel->AddChild(salesText);
        
        auto ratingText = new TextBlock();
        ratingText->SetText("评分: " + std::to_string(product->rating));
        ratingText->SetFontSize(12);
        ratingText->SetForeground(new SolidColorBrush(255, 165, 0)); // 橙色
        infoPanel->AddChild(ratingText);
        
        panel->AddChild(infoPanel);
        
        border->SetChild(panel);
        return border;
    });
    
    return dataTemplate;
}
```

## 常见问题

### Q1: DataTemplate和ControlTemplate有什么区别？

**答：**
- **DataTemplate**：定义数据对象的可视化表示。用于ItemsControl的ItemTemplate或ContentControl的ContentTemplate。
- **ControlTemplate**：定义控件本身的外观结构。用于重新定义控件的视觉树（如Button的外观）。

简单记忆：
- DataTemplate：数据→视图（数据如何显示）
- ControlTemplate：控件→外观（控件如何呈现）

### Q2: 为什么要使用std::any？

**答：** std::any提供了类型安全的泛型容器，允许DataTemplate处理任意类型的数据。这类似于WPF中的object类型。

### Q3: 如何处理不同类型的数据？

**答：** 使用模板选择器或在Factory函数中进行类型检查：

```cpp
dataTemplate->SetFactory([](const std::any& data) -> UIElement* {
    if (data.type() == typeid(std::shared_ptr<User>)) {
        return CreateUserUI(std::any_cast<std::shared_ptr<User>>(data));
    } else if (data.type() == typeid(std::shared_ptr<Product>)) {
        return CreateProductUI(std::any_cast<std::shared_ptr<Product>>(data));
    }
    return CreateDefaultUI();
});
```

### Q4: DataTemplate会造成内存泄漏吗？

**答：** 只要正确管理UIElement的生命周期，不会。建议：
- DataTemplate使用智能指针（shared_ptr）
- Factory返回的UIElement由父容器管理（通过TakeOwnership）
- 避免循环引用

### Q5: 可以在DataTemplate中响应事件吗？

**答：** 可以。在Factory函数中为创建的UI元素添加事件处理：

```cpp
dataTemplate->SetFactory([](const std::any& data) -> UIElement* {
    auto button = new Button();
    button->SetContent("点击");
    
    // 添加事件处理
    button->Click += [data](const auto& sender, const auto& args) {
        // 处理点击事件
        auto user = std::any_cast<std::shared_ptr<User>>(data);
        std::cout << "点击了: " << user->name << std::endl;
    };
    
    return button;
});
```

### Q6: 如何更新DataTemplate创建的UI？

**答：** 有两种方式：
1. **数据绑定**：如果使用了数据绑定，修改数据会自动更新UI
2. **重新实例化**：调用Instantiate重新创建UI元素

### Q7: 性能如何优化？

**答：**
- 使用虚拟化（Virtualization）处理大量数据
- 缓存常用的模板实例
- 避免在Factory中进行耗时操作
- 使用ItemContainerGenerator的回收机制

### Q8: 是否支持嵌套DataTemplate？

**答：** 支持。可以在一个DataTemplate中使用另一个DataTemplate：

```cpp
auto innerTemplate = CreateInnerTemplate();

auto outerTemplate = std::make_shared<DataTemplate>();
outerTemplate->SetFactory([innerTemplate](const std::any& data) -> UIElement* {
    auto outerData = std::any_cast<OuterData>(data);
    
    auto panel = new StackPanel();
    // ...
    
    // 使用内部模板
    auto innerElement = innerTemplate->Instantiate(outerData.innerData);
    panel->AddChild(innerElement);
    
    return panel;
});
```

## 总结

DataTemplate是FK_UI框架中实现数据驱动UI的核心机制。通过DataTemplate，你可以：

✅ **实现数据与视图的完全分离**  
✅ **创建可复用的UI模板**  
✅ **支持多种数据类型的统一展示**  
✅ **构建灵活且可维护的应用程序**

**答案是肯定的：在C++中完全可以实现类似WPF中DataTemplate的效果！** FK_UI框架已经提供了完整的基础设施，让你能够在C++中享受数据模板化带来的便利和强大功能。

## 相关资源

- [DataTemplate API文档](/Docs/API/UI/DataTemplate.md)
- [ItemsControl API文档](/Docs/API/UI/ItemsControl.md)
- [ContentPresenter API文档](/Docs/API/UI/ContentPresenter.md)
- [示例代码](/examples/datatemplate/)
