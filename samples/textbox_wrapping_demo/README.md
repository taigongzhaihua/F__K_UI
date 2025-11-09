# TextBox 换行功能演示

此示例演示了 TextBox 控件的两个新增换行功能：

## 功能特性

### 1. 自动换行（TextWrapping）
- **TextWrapping::NoWrap** - 默认模式，不换行，超出宽度自动滚动
- **TextWrapping::Wrap** - 自动换行模式，文本超出宽度时自动折行显示

### 2. 多行输入（AcceptsReturn）
- **AcceptsReturn = false** - 默认模式，按 Enter 键不插入换行符
- **AcceptsReturn = true** - 多行模式，按 Enter 键插入换行符（\n）

## 示例内容

该示例创建了三个 TextBox：

1. **单行输入框** - 默认行为，无换行
2. **自动换行输入框** - 启用 TextWrapping::Wrap
3. **多行输入框** - 同时启用 AcceptsReturn 和 TextWrapping::Wrap

## 构建步骤

```bash
# 1. 确保主库已构建
cd ../../build
cmake ..
cmake --build .

# 2. 构建此示例
cd ../samples/textbox_wrapping_demo
mkdir build
cd build
cmake ..
cmake --build .

# 3. 运行
./textbox_wrapping_demo.exe  # Windows
./textbox_wrapping_demo       # Linux/macOS
```

## 使用说明

1. 在第一个输入框中输入长文本，观察自动滚动行为
2. 在第二个输入框中输入长文本，观察自动换行效果
3. 在第三个输入框中按 Enter 键，观察多行输入效果
4. 点击"显示所有输入框的内容"按钮，在控制台查看实际文本内容（包括换行符）

## 技术实现

### TextWrapping 实现
- TextBox 内部使用 TextBlock 作为文本渲染器
- 将 TextWrapping 属性传递给 TextBlock
- TextBlock 在 MeasureOverride 中计算换行位置

### AcceptsReturn 实现
- 在 OnKeyDown 处理 GLFW_KEY_ENTER 和 GLFW_KEY_KP_ENTER
- 检查 AcceptsReturn 属性，若为 true 则插入 '\n' 字符
- 文本渲染支持多行显示

## 代码示例

```cpp
// 自动换行
auto textBox = ui::textBox()
    ->Width(400.0f)
    ->Height(80.0f)
    ->TextWrapping(TextWrapping::Wrap);  // 启用自动换行

// 多行输入
auto multilineBox = ui::textBox()
    ->Width(400.0f)
    ->Height(150.0f)
    ->AcceptsReturn(true)                // 允许 Enter 换行
    ->TextWrapping(TextWrapping::Wrap);  // 同时启用自动换行
```
