# TextBox - 文本输入框

## 概述

`TextBox` 是单行文本输入控件,支持文本编辑、选择、光标定位和占位符文本。

**命名空间**: `fk::ui`  
**头文件**: `fk/ui/TextBox.h`  
**继承**: `Control` → `FrameworkElement` → `UIElement`

## 核心功能

- **文本输入**: 键盘文本输入
- **光标控制**: 光标定位、闪烁动画
- **文本选择**: 鼠标拖拽选择、键盘选择
- **占位符**: 空文本时显示提示
- **自动滚动**: 光标超出视口时自动滚动
- **只读模式**: 禁用编辑功能
- **样式定制**: 前景色、背景色、边框

## 依赖属性

### 文本内容

```cpp
FK_DEPENDENCY_PROPERTY(Text, std::string)
```
当前文本内容。

```cpp
FK_DEPENDENCY_PROPERTY(PlaceholderText, std::string)
```
占位符文本(文本为空时显示)。

### 光标和选择

```cpp
FK_DEPENDENCY_PROPERTY(CaretIndex, int)
```
光标位置(字符索引)。

```cpp
FK_DEPENDENCY_PROPERTY(SelectionStart, int)
```
选择起始位置。

```cpp
FK_DEPENDENCY_PROPERTY(SelectionLength, int)
```
选择长度。

### 状态

```cpp
FK_DEPENDENCY_PROPERTY(IsReadOnly, bool)
```
是否只读模式。

### 样式

```cpp
FK_DEPENDENCY_PROPERTY(Foreground, std::string)
```
前景色(文本颜色)。

```cpp
FK_DEPENDENCY_PROPERTY(Background, std::string)
```
背景色。

```cpp
FK_DEPENDENCY_PROPERTY(BorderBrush, std::string)
```
边框颜色。

```cpp
FK_DEPENDENCY_PROPERTY(BorderThickness, float)
```
边框厚度。

## 事件

```cpp
core::Event<TextBoxBase&, const std::string&, const std::string&> TextChanged;
```
文本改变事件。
- **参数**: 
  - `TextBoxBase&` - TextBox 实例
  - `const std::string&` - 旧文本
  - `const std::string&` - 新文本

## 使用示例

### 1. 创建基本 TextBox

```cpp
#include <fk/ui/TextBox.h>

// 创建 TextBox
auto textBox = fk::ui::TextBox<>::Create();
textBox->Width(200)->Height(30);
textBox->PlaceholderText("请输入文本...");

// 或使用工厂函数
auto textBox2 = fk::ui::textBox()
    ->Width(200)
    ->Height(30)
    ->PlaceholderText("请输入文本...");
```

### 2. 获取和设置文本

```cpp
#include <fk/ui/TextBox.h>

auto textBox = fk::ui::textBox();

// 设置文本
textBox->Text("Hello World");

// 获取文本
std::string text = textBox->Text();
std::cout << "当前文本: " << text << "\n";

// 清空文本
textBox->Text("");
```

### 3. 监听文本改变

```cpp
#include <fk/ui/TextBox.h>

auto textBox = fk::ui::textBox()
    ->PlaceholderText("输入您的名字")
    ->OnTextChanged([](auto& sender, const auto& oldText, const auto& newText) {
        std::cout << "文本改变:\n";
        std::cout << "  旧文本: " << oldText << "\n";
        std::cout << "  新文本: " << newText << "\n";
    });
```

### 4. 占位符文本

```cpp
#include <fk/ui/TextBox.h>

auto usernameBox = fk::ui::textBox()
    ->PlaceholderText("用户名")
    ->Width(200)
    ->Height(30);

auto passwordBox = fk::ui::textBox()
    ->PlaceholderText("密码")
    ->Width(200)
    ->Height(30);

// 占位符在文本为空时显示,获得焦点时保留
```

### 5. 只读模式

```cpp
#include <fk/ui/TextBox.h>

auto readOnlyBox = fk::ui::textBox()
    ->Text("只读内容")
    ->IsReadOnly(true)  // 禁止编辑
    ->Width(300)
    ->Height(30);

// 用户可以选择和复制文本,但不能修改
```

### 6. 样式定制

```cpp
#include <fk/ui/TextBox.h>

auto styledBox = fk::ui::textBox()
    ->Width(250)
    ->Height(35)
    ->Foreground("#FFFFFF")       // 白色文字
    ->Background("#1E1E1E")       // 深色背景
    ->BorderBrush("#007ACC")      // 蓝色边框
    ->BorderThickness(2.0f)       // 2px 边框
    ->PlaceholderText("输入内容...");
```

### 7. 光标控制

```cpp
#include <fk/ui/TextBox.h>

auto textBox = fk::ui::textBox()->Text("Hello World");

// 设置光标位置
textBox->CaretIndex(5);  // 光标在 'H' 和 'e' 之间

// 移动到末尾
textBox->CaretIndex(textBox->Text().length());

// 移动到开头
textBox->CaretIndex(0);
```

### 8. 文本选择

```cpp
#include <fk/ui/TextBox.h>

auto textBox = fk::ui::textBox()->Text("Hello World");

// 选择 "World"
textBox->SelectionStart(6);
textBox->SelectionLength(5);

// 选择全部
textBox->SelectionStart(0);
textBox->SelectionLength(textBox->Text().length());

// 清除选择
textBox->SelectionLength(0);
```

### 9. 表单验证

```cpp
#include <fk/ui/TextBox.h>
#include <fk/ui/TextBlock.h>

auto emailBox = fk::ui::textBox()
    ->PlaceholderText("电子邮件")
    ->Width(300)
    ->Height(30);

auto errorText = fk::ui::textBlock()
    ->Foreground("#FF0000")
    ->Visibility(fk::ui::Visibility::Collapsed);

emailBox->OnTextChanged([errorText](auto&, auto&, const auto& newText) {
    // 简单的邮箱验证
    bool isValid = newText.find('@') != std::string::npos;
    
    if (newText.empty() || isValid) {
        errorText->Visibility(fk::ui::Visibility::Collapsed);
    } else {
        errorText->Text("请输入有效的邮箱地址");
        errorText->Visibility(fk::ui::Visibility::Visible);
    }
});
```

### 10. 登录表单示例

```cpp
#include <fk/ui/TextBox.h>
#include <fk/ui/Button.h>
#include <fk/ui/StackPanel.h>

auto CreateLoginForm() {
    auto panel = fk::ui::stackPanel()
        ->Orientation(fk::ui::Orientation::Vertical)
        ->Width(300);
    
    // 用户名输入
    auto usernameBox = fk::ui::textBox()
        ->PlaceholderText("用户名")
        ->Height(35);
    
    // 密码输入(TODO: 需要 PasswordBox 控件)
    auto passwordBox = fk::ui::textBox()
        ->PlaceholderText("密码")
        ->Height(35);
    
    // 登录按钮
    auto loginButton = fk::ui::button()
        ->Content("登录")
        ->Height(40);
    
    loginButton->Click.Subscribe([usernameBox, passwordBox]() {
        std::string username = usernameBox->Text();
        std::string password = passwordBox->Text();
        
        if (username.empty() || password.empty()) {
            std::cout << "请填写用户名和密码\n";
            return;
        }
        
        std::cout << "登录: " << username << "\n";
        // 执行登录逻辑...
    });
    
    panel->Children()
        .Add(usernameBox)
        .Add(passwordBox)
        .Add(loginButton);
    
    return panel;
}
```

### 11. 实时搜索

```cpp
#include <fk/ui/TextBox.h>
#include <vector>
#include <algorithm>

auto searchBox = fk::ui::textBox()
    ->PlaceholderText("搜索...")
    ->Width(300)
    ->Height(30);

std::vector<std::string> items = {"Apple", "Banana", "Cherry", "Date"};

searchBox->OnTextChanged([&items](auto&, auto&, const auto& query) {
    // 实时搜索
    std::vector<std::string> results;
    
    for (const auto& item : items) {
        if (item.find(query) != std::string::npos) {
            results.push_back(item);
        }
    }
    
    std::cout << "搜索结果(" << results.size() << "):\n";
    for (const auto& result : results) {
        std::cout << "  - " << result << "\n";
    }
});
```

### 12. 字符限制

```cpp
#include <fk/ui/TextBox.h>

auto limitedBox = fk::ui::textBox()
    ->PlaceholderText("最多10个字符")
    ->Width(200)
    ->Height(30);

const int maxLength = 10;

limitedBox->OnTextChanged([limitedBox, maxLength](auto&, auto&, auto& newText) {
    if (newText.length() > maxLength) {
        // 截断文本
        limitedBox->Text(newText.substr(0, maxLength));
    }
});
```

## 最佳实践

### 1. 占位符文本
```cpp
// 推荐:使用清晰的占位符
textBox->PlaceholderText("请输入您的姓名");

// 避免:空占位符或无意义文本
textBox->PlaceholderText("");
```

### 2. 表单验证
```cpp
// 推荐:实时反馈
textBox->OnTextChanged([](auto&, auto&, const auto& text) {
    bool isValid = ValidateInput(text);
    UpdateValidationUI(isValid);
});

// 避免:仅在提交时验证
```

### 3. 初始值
```cpp
// 推荐:设置默认文本
textBox->Text("默认值");

// 注意:占位符仅在文本为空时显示
```

### 4. 只读展示
```cpp
// 推荐:使用 IsReadOnly 展示不可编辑内容
textBox->Text("订单号: #12345")
       ->IsReadOnly(true);

// 或使用 TextBlock
auto label = fk::ui::textBlock()->Text("订单号: #12345");
```

### 5. 样式一致性
```cpp
// 推荐:统一的样式
const std::string inputBorder = "#CCCCCC";
const float inputHeight = 35.0f;

auto CreateStyledTextBox(const std::string& placeholder) {
    return fk::ui::textBox()
        ->PlaceholderText(placeholder)
        ->Height(inputHeight)
        ->BorderBrush(inputBorder)
        ->BorderThickness(1.0f);
}
```

## 常见问题

### Q1: 如何实现密码输入框?
当前版本 TextBox 不支持密码模式。可以使用 `IsReadOnly` 属性或等待 PasswordBox 控件。

### Q2: 如何限制输入类型(如仅数字)?
```cpp
textBox->OnTextChanged([textBox](auto&, auto& oldText, auto& newText) {
    // 仅允许数字
    if (!std::all_of(newText.begin(), newText.end(), ::isdigit)) {
        textBox->Text(oldText);  // 恢复旧文本
    }
});
```

### Q3: 如何实现多行文本?
当前 TextBox 是单行控件。多行编辑需要使用 TextBox 的多行版本或专门的 TextArea 控件。

### Q4: 文本改变事件触发时机?
每次文本改变时立即触发,包括:
- 用户键盘输入
- 粘贴
- 程序代码设置 `Text` 属性

### Q5: 如何处理 Enter 键?
```cpp
// 监听键盘事件(需要重写 OnKeyDown)
// 或在 TextChanged 中检测换行符
```

## 性能考虑

### 1. 文本改变频率
```cpp
// 避免:在 TextChanged 中执行耗时操作
textBox->OnTextChanged([](auto&, auto&, const auto& text) {
    ExpensiveOperation(text);  // 每个字符都触发!
});

// 推荐:使用防抖
textBox->OnTextChanged([](auto&, auto&, const auto& text) {
    debouncer.Debounce(500ms, [text]() {
        ExpensiveOperation(text);
    });
});
```

### 2. 长文本性能
单行 TextBox 不适合大量文本。对于长文本,考虑:
- 限制文本长度
- 使用多行编辑控件
- 分页显示

### 3. 频繁更新
```cpp
// 避免:循环中频繁更新
for (int i = 0; i < 1000; ++i) {
    textBox->Text(textBox->Text() + "a");  // 每次重绘!
}

// 推荐:批量更新
std::string newText = textBox->Text();
for (int i = 0; i < 1000; ++i) {
    newText += "a";
}
textBox->Text(newText);  // 一次更新
```

## 键盘快捷键

TextBox 支持的快捷键:
- **Backspace**: 删除光标前字符
- **Delete**: 删除光标后字符
- **Left/Right**: 移动光标
- **Home/End**: 跳转到开头/末尾
- **Ctrl+A**: 全选(TODO)
- **Ctrl+C**: 复制(TODO)
- **Ctrl+V**: 粘贴(TODO)
- **Ctrl+X**: 剪切(TODO)

## 内部机制

### 光标闪烁
- 周期: 0.8 秒
- 自动重置: 文本改变或光标移动时重置

### 自动滚动
当光标超出视口时,TextBox 自动调整水平滚动偏移以保持光标可见。

### 文本选择
- 鼠标拖拽选择
- 键盘选择(Shift + 方向键, TODO)
- 选择高亮渲染

## 相关控件

- **TextBlock**: 只读文本显示
- **PasswordBox**: 密码输入(TODO)
- **TextArea**: 多行文本编辑(TODO)
- **RichTextBox**: 富文本编辑(TODO)

## 参考

- [Control.md](Control.md) - 控件基类
- [TextBlock.md](TextBlock.md) - 文本显示
- [Button.md](Button.md) - 按钮控件
