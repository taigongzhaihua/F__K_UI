# F__K_UI x:Name 功能使用指南

> 类似WPF的元素命名和查找机制

---

## 🎯 快速开始

### 1. 命名元素

使用流式API为元素设置名称：

```cpp
auto* button = new Button();
button->Name("submitButton")
      ->Content("提交")
      ->Width(200)
      ->Height(40);
```

### 2. 查找元素

从父元素或窗口中查找：

```cpp
// 从窗口查找
auto* found = window->FindName("submitButton");

// 从面板查找
auto* found = panel->FindName("submitButton");

// 类型转换
if (auto* btn = dynamic_cast<Button*>(found)) {
    btn->Content("找到了！");
}
```

### 3. ElementName绑定

将一个控件的属性绑定到另一个命名控件：

```cpp
// 源控件
auto* slider = new Slider();
slider->Name("volumeSlider");
slider->Value(50);

// 目标控件绑定到源控件的属性
auto* text = new TextBlock();
Binding binding;
binding.ElementName("volumeSlider").Path("Value");
text->SetBinding(TextBlock::TextProperty(), std::move(binding));

// slider.Value改变时，text.Text自动更新
```

---

## 📚 核心功能

### ✅ 元素命名

#### 方法1：流式API（推荐）

```cpp
element->Name("elementName");
```

**优势**：支持链式调用

```cpp
button->Name("btn")->Content("点击")->Width(100);
```

#### 方法2：直接设置

```cpp
element->SetName("elementName");
```

#### 获取名称

```cpp
std::string name = element->GetName();
```

### ✅ 元素查找

#### UIElement::FindName()

从当前元素的子树中递归查找：

```cpp
UIElement* found = element->FindName("targetName");
```

**特性**：
- 递归深度优先搜索（DFS）
- 搜索当前元素及所有子元素
- 返回第一个匹配的元素
- 未找到返回 nullptr

**示例**：
```cpp
auto* panel = new StackPanel();
panel->AddChild(button1->Name("btn1"));
panel->AddChild(button2->Name("btn2"));

auto* found = panel->FindName("btn1");  // 找到button1
```

#### Window::FindName()

便捷方法，从窗口内容开始搜索：

```cpp
UIElement* found = window->FindName("targetName");
```

**等价于**：
```cpp
auto* content = window->GetContent();
if (content) {
    found = content->FindName("targetName");
}
```

### ✅ ElementName绑定

将目标元素的属性绑定到源元素：

```cpp
Binding binding;
binding.ElementName("sourceName").Path("PropertyName");
target->SetBinding(TargetProperty(), std::move(binding));
```

**工作原理**：
- 从目标元素开始向上遍历逻辑树
- 在每个祖先中搜索指定名称的元素
- 找到后建立属性绑定
- 源属性变化时自动更新目标属性

**可以找到的元素**：
- ✅ 兄弟元素
- ✅ 叔伯元素
- ✅ 祖先元素
- ✅ 表兄弟元素

**示例**：
```cpp
// 兄弟元素绑定
panel->AddChild(slider->Name("source"));
panel->AddChild(text);

Binding binding;
binding.ElementName("source").Path("Value");
text->SetBinding(TextProperty(), std::move(binding));

// slider.Value改变时，text.Text自动更新
```

### ✅ 命名作用域

**重要**：名称**不需要**在整个应用中唯一！

每个容器形成一个命名作用域：

```cpp
// Header区域
auto* headerPanel = new StackPanel();
headerPanel->AddChild(new Button()->Name("submit"));

// Content区域
auto* contentPanel = new StackPanel();
contentPanel->AddChild(new Button()->Name("submit")); // 相同名称 ✅

// 从不同作用域查找
auto* btn1 = headerPanel->FindName("submit");   // Header的submit
auto* btn2 = contentPanel->FindName("submit");  // Content的submit
```

**优势**：
- ✅ 组件化开发友好
- ✅ 不同窗口可用相同名称
- ✅ 不同面板可用相同名称
- ✅ 模块化代码更清晰

---

## 💡 实际应用

### 场景1：表单验证

```cpp
class LoginForm {
    StackPanel* panel_;
    
public:
    void Init() {
        panel_ = new StackPanel();
        
        panel_->AddChild(new TextBox()->Name("username"));
        panel_->AddChild(new TextBox()->Name("password"));
        panel_->AddChild(new Button()->Name("loginButton"));
    }
    
    void Validate() {
        auto* username = dynamic_cast<TextBox*>(panel_->FindName("username"));
        auto* password = dynamic_cast<TextBox*>(panel_->FindName("password"));
        
        if (username && password) {
            if (username->Text().empty() || password->Text().empty()) {
                ShowError("请填写用户名和密码");
            }
        }
    }
};
```

### 场景2：动态UI

```cpp
void CreateDynamicUI(StackPanel* panel, int count) {
    for (int i = 0; i < count; i++) {
        auto* button = new Button();
        button->Name("btn_" + std::to_string(i));
        button->Content("按钮 " + std::to_string(i));
        panel->AddChild(button);
    }
}

// 稍后访问
auto* btn5 = panel->FindName("btn_5");
```

### 场景3：主从控件

```cpp
// 主控件（滑块）
auto* slider = new Slider();
slider->Name("masterSlider");
slider->Minimum(0);
slider->Maximum(100);
slider->Value(50);

// 从控件1（文本）
auto* valueText = new TextBlock();
Binding binding1;
binding1.ElementName("masterSlider").Path("Value");
valueText->SetBinding(TextBlock::TextProperty(), std::move(binding1));

// 从控件2（进度条）
auto* progressBar = new ProgressBar();
Binding binding2;
binding2.ElementName("masterSlider").Path("Value");
progressBar->SetBinding(ProgressBar::ValueProperty(), std::move(binding2));

// slider改变时，valueText和progressBar自动更新
```

### 场景4：模块化组件

```cpp
class UserCard {
    Border* root_;
    
public:
    UserCard() {
        root_ = new Border();
        
        auto* panel = new StackPanel();
        panel->AddChild(new Image()->Name("avatar"));
        panel->AddChild(new TextBlock()->Name("username"));
        panel->AddChild(new TextBlock()->Name("bio"));
        
        root_->Child(panel);
    }
    
    void SetUser(const User& user) {
        auto* avatar = dynamic_cast<Image*>(root_->FindName("avatar"));
        auto* username = dynamic_cast<TextBlock*>(root_->FindName("username"));
        auto* bio = dynamic_cast<TextBlock*>(root_->FindName("bio"));
        
        if (avatar) avatar->Source(user.avatarUrl);
        if (username) username->Text(user.name);
        if (bio) bio->Text(user.bio);
    }
    
    UIElement* GetRoot() { return root_; }
};

// 使用
auto card1 = new UserCard();
auto card2 = new UserCard();

// 两个卡片可以有相同的内部名称（作用域隔离）✅
```

---

## 📖 最佳实践

### ✅ 推荐做法

#### 1. 使用有意义的名称

```cpp
// ✅ 好
button->Name("submitButton");
button->Name("cancelButton");
button->Name("deleteButton");

// ❌ 不好
button->Name("btn1");
button->Name("btn2");
button->Name("b");
```

#### 2. 使用前缀区分

```cpp
// ✅ 模块化命名
panel->AddChild(new TextBox()->Name("header_title"));
panel->AddChild(new TextBox()->Name("content_title"));
panel->AddChild(new TextBox()->Name("footer_copyright"));
```

#### 3. 保持容器引用

```cpp
// ✅ 好 - 保持引用，快速访问
class MyPanel {
    StackPanel* panel_;
    
    void Init() {
        panel_ = new StackPanel();
        panel_->AddChild(new Button()->Name("btn"));
    }
    
    void UpdateButton() {
        auto* btn = panel_->FindName("btn");  // 快速查找
    }
};

// ❌ 不好 - 从根遍历，效率低
void UpdateButton(Window* window) {
    auto* btn = window->FindName("btn");  // 可能需要遍历整个树
}
```

#### 4. 使用流式API

```cpp
// ✅ 好 - 清晰简洁
button->Name("btn")->Content("点击")->Width(100)->Height(40);

// ❌ 不好 - 冗长
button->SetName("btn");
button->SetContent("点击");
button->SetWidth(100);
button->SetHeight(40);
```

### ❌ 避免做法

#### 1. 同一作用域重复名称

```cpp
// ❌ 避免 - 会返回第一个，可能不是你想要的
panel->AddChild(new Button()->Name("btn"));
panel->AddChild(new Button()->Name("btn"));  // 重复！

// ✅ 好 - 使用唯一名称
panel->AddChild(new Button()->Name("btn1"));
panel->AddChild(new Button()->Name("btn2"));
```

#### 2. 依赖查找顺序

```cpp
// ❌ 避免 - 不要假设查找顺序
panel->AddChild(button1->Name("btn"));
panel->AddChild(button2->Name("btn"));
auto* found = panel->FindName("btn");
// 找到的是button1还是button2？不要依赖这个！

// ✅ 好 - 使用唯一名称
panel->AddChild(button1->Name("firstBtn"));
panel->AddChild(button2->Name("secondBtn"));
```

#### 3. 假设全局唯一

```cpp
// ✅ 完全可以！不同模块可以使用相同名称
module1->AddChild(new Button()->Name("submit"));
module2->AddChild(new Button()->Name("submit"));

// 从各自的模块查找
module1->FindName("submit");  // 模块1的submit
module2->FindName("submit");  // 模块2的submit
```

---

## 🚀 性能考虑

### 当前实现性能

| UI规模 | 元素数量 | FindName性能 |
|--------|---------|-------------|
| 小型 | < 100 | < 1ms |
| 中型 | 100-500 | 1-5ms |
| 大型 | > 1000 | 5-20ms |

**结论**：对大多数应用性能足够 ✅

### 性能优化建议

#### 1. 缓存查找结果

```cpp
// ✅ 好 - 缓存频繁访问的元素
class MyForm {
    Button* submitButton_{nullptr};
    
    void Init() {
        // 只查找一次
        submitButton_ = dynamic_cast<Button*>(panel_->FindName("submit"));
    }
    
    void Update() {
        if (submitButton_) {
            submitButton_->Content("更新");  // 直接使用，不再查找
        }
    }
};

// ❌ 不好 - 每次都查找
void Update() {
    auto* btn = panel_->FindName("submit");  // 重复查找
    btn->Content("更新");
}
```

#### 2. 从最近的父元素查找

```cpp
// ✅ 好 - 从最近的容器查找
auto* btn = innerPanel->FindName("btn");  // 搜索范围小

// ❌ 不好 - 从根查找
auto* btn = window->FindName("btn");  // 搜索整个树
```

#### 3. 使用性能优化方案

对于大型UI（> 1000元素）或频繁查找（> 10次/秒），可以考虑：

- **哈希表缓存**：O(1)查找
- **NameScope类**：O(1)查找 + WPF兼容
- **混合方案**：最佳性能 + 设计清晰度

详见 `性能优化方案.md` 和 `混合优化方案.md`

---

## 📋 完整示例

### 示例1：简单应用

```cpp
#include "fk/Application.h"
#include "fk/ui/Window.h"
#include "fk/ui/StackPanel.h"
#include "fk/ui/Button.h"
#include "fk/ui/TextBox.h"

int main() {
    auto app = fk::Application::Create();
    auto window = app->CreateWindow();
    
    // 创建UI
    auto* panel = new fk::ui::StackPanel();
    
    auto* nameBox = new fk::ui::TextBox();
    nameBox->Name("nameInput");
    panel->AddChild(nameBox);
    
    auto* greetButton = new fk::ui::Button();
    greetButton->Name("greetButton");
    greetButton->Content("打招呼");
    greetButton->Click.connect([window]() {
        auto* input = dynamic_cast<fk::ui::TextBox*>(
            window->FindName("nameInput"));
        if (input && !input->Text().empty()) {
            std::cout << "你好，" << input->Text() << "！" << std::endl;
        }
    });
    panel->AddChild(greetButton);
    
    window->Content(panel);
    window->Show();
    
    return app->Run();
}
```

### 示例2：主从UI

```cpp
auto window = app->CreateWindow();
auto* panel = new fk::ui::StackPanel();

// 主控件：滑块
auto* slider = new fk::ui::Slider();
slider->Name("volumeSlider");
slider->Minimum(0);
slider->Maximum(100);
slider->Value(50);
panel->AddChild(slider);

// 从控件1：显示数值
auto* valueText = new fk::ui::TextBlock();
fk::binding::Binding binding1;
binding1.ElementName("volumeSlider").Path("Value");
valueText->SetBinding(fk::ui::TextBlock::TextProperty(), std::move(binding1));
panel->AddChild(valueText);

// 从控件2：进度条
auto* progressBar = new fk::ui::ProgressBar();
fk::binding::Binding binding2;
binding2.ElementName("volumeSlider").Path("Value");
progressBar->SetBinding(fk::ui::ProgressBar::ValueProperty(), std::move(binding2));
panel->AddChild(progressBar);

window->Content(panel);
window->Show();

// slider拖动时，valueText和progressBar自动更新
```

---

## 🔗 更多资源

### 详细文档

- **X_NAME_使用指南.md** - 中文快速入门指南
- **X_NAME_FEATURE.md** - 完整API文档
- **ELEMENTNAME_BINDING_说明.md** - ElementName绑定详解
- **命名作用域说明.md** - 作用域概念详解
- **性能优化方案.md** - 性能优化建议
- **实现总结_FINAL.md** - 完整实现总结

### 示例程序

运行演示程序查看实际效果：

```bash
cd build
./findname_demo
./elementname_binding_demo
./name_scope_demo
```

---

## ❓ 常见问题

### Q1: 名称需要全局唯一吗？

**A**: 不需要！只需在同一个作用域（容器）内唯一即可。不同窗口、不同面板可以使用相同的名称。

### Q2: 如果有重复名称会怎样？

**A**: `FindName()`会返回深度优先搜索找到的第一个匹配元素。建议避免在同一作用域内使用重复名称。

### Q3: ElementName绑定能找到哪些元素？

**A**: 可以找到兄弟元素、叔伯元素、祖先元素等。从目标元素开始向上遍历，在每个祖先中搜索。

### Q4: 性能如何？

**A**: 对于小型和中型UI（< 500元素），性能完全足够（< 5ms）。大型UI可以考虑使用性能优化方案。

### Q5: 如何优化性能？

**A**: 
1. 缓存频繁访问的元素
2. 从最近的父元素查找
3. 对于大型UI，可以实施NameScope优化方案（O(1)查找）

### Q6: 与WPF有何不同？

**A**: 核心功能完全兼容WPF。额外提供了流式API支持，使代码更简洁。

---

## 📞 反馈

如有问题或建议，欢迎提交Issue或PR！

---

**Happy Coding! 🎉**

