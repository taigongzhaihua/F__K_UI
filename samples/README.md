# F__K_UI 示例项目集合

本目录包含多个独立的示例项目，展示如何将 F__K_UI 框架作为外部库引用到自己的项目中。

## 项目列表

### 1. [hello_fk_ui](./hello_fk_ui/) - 入门示例 ⭐

**难度**: 初级  
**功能**: 基础窗口、按钮、文本、事件处理  

最简单的入门示例，展示:
- 应用程序创建和生命周期
- 窗口和基本控件（Button、TextBlock）
- 事件订阅和处理
- StackPanel 布局

**适合**: 新手入门，了解框架基本用法

---

### 2. [mvvm_binding_demo](./mvvm_binding_demo/) - MVVM 数据绑定示例 ⭐⭐

**难度**: 中级  
**功能**: MVVM 模式、数据绑定、ObservableObject  

演示 MVVM 架构和数据绑定，展示:
- ObservableObject 实现 ViewModel
- 属性绑定（OneWay）
- 计算属性和依赖通知
- PropertyChanged 事件
- DataContext 使用

**适合**: 学习 MVVM 模式，构建数据驱动应用

---

## 通用构建说明

### 前提条件

所有示例项目都依赖 F__K_UI 框架，需要先构建主框架:

```powershell
# 在 F__K_UI 主目录
cd ..
mkdir -p build
cd build
cmake ..
cmake --build .
```

### 构建单个示例

```powershell
# 进入示例项目目录
cd samples/<项目名>

# 创建构建目录
mkdir -p build
cd build

# 配置和编译
cmake ..
cmake --build .

# 运行
./<项目名>.exe  # Windows
./<项目名>      # Linux/macOS
```

### 项目结构

每个示例项目都采用相同的结构:

```
<项目名>/
├── CMakeLists.txt          # CMake 构建配置
├── README.md               # 项目说明和教程
└── src/
    └── main.cpp            # 主程序源代码
```

## 依赖项说明

所有示例项目共享以下依赖:

| 依赖库 | 版本要求 | 用途 |
|--------|----------|------|
| **F__K_UI** | latest | 主框架库 |
| **GLFW** | 3.3+ | 窗口管理和输入处理 |
| **OpenGL** | 3.3+ | 图形渲染 |
| **FreeType** | 2.x | 字体渲染 |
| **glad** | - | OpenGL 函数加载器 |

## CMake 配置说明

每个项目的 `CMakeLists.txt` 都通过以下变量引用 F__K_UI:

```cmake
set(FK_UI_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../..")
set(FK_UI_INCLUDE "${FK_UI_ROOT}/include")
set(FK_UI_LIB "${FK_UI_ROOT}/build")
set(FK_UI_THIRD_PARTY "${FK_UI_ROOT}/third_party")
```

如果你的 F__K_UI 安装在其他位置，可以通过命令行指定:

```powershell
cmake -DFK_UI_ROOT=/path/to/f__k_ui ..
```

## API 使用规范

所有示例都遵循 F__K_UI 的 API 最佳实践:

### ✅ 推荐的写法

```cpp
// 1. 使用工厂函数创建控件
auto window = ui::window()->Title("My Window");
auto button = ui::button()->Content("Click");

// 2. 使用 += 订阅事件
button->Click += [](ui::detail::ButtonBase&) {
    std::cout << "Clicked!" << std::endl;
};

// 3. 使用流式 API 配置属性
auto text = ui::textBlock()
    ->Text("Hello")
    ->FontSize(18.0f)
    ->Foreground("#333333");
```

### ❌ 不推荐的写法

```cpp
// 1. 不要使用 ::Create()
auto button = Button::Create();  // ❌ 过时

// 2. 不要使用 .Add() 订阅事件
button->Click.Add([](auto&) { });  // ❌ 不推荐

// 3. 不要分散的 Set 方法
auto text = ui::textBlock();
text->SetText("Hello");           // ❌ 不够简洁
text->SetFontSize(18.0f);
```

## 学习路径建议

如果你是 F__K_UI 新手，建议按以下顺序学习示例:

1. **hello_fk_ui** - 从这里开始，了解基本概念
2. *(未来示例)* **button_demo** - 深入了解按钮控件
3. *(未来示例)* **layout_demo** - 学习布局系统
4. *(未来示例)* **databinding_demo** - 掌握数据绑定
5. *(未来示例)* **custom_control_demo** - 创建自定义控件

## 故障排除

### 问题: 链接错误，找不到 libfk.a

**解决方案**: 确保已经构建了 F__K_UI 主项目:

```powershell
cd ../..
cd build
cmake --build .
```

### 问题: 运行时找不到 glfw3.dll

**解决方案**: CMakeLists.txt 会自动复制，如失败可手动复制:

```powershell
copy ../../third_party/lib/glfw3.dll ./build/
```

### 问题: 头文件找不到

**解决方案**: 检查 `FK_UI_ROOT` 路径是否正确，或使用绝对路径:

```powershell
cmake -DFK_UI_ROOT=G:/path/to/f__k_ui ..
```

## 参考资源

- [F__K_UI API 文档](../Docs/API/)
- [核心模块](../Docs/API/Core.md) - Dispatcher, Event, Timer
- [UI 模块](../Docs/API/UI.md) - Window, Button, TextBlock, Panel
- [绑定模块](../Docs/API/Binding.md) - DependencyProperty, Binding
- [应用模块](../Docs/API/App.md) - Application
- [渲染模块](../Docs/API/Render.md) - IRenderer, GlRenderer

## 贡献新示例

如果你想贡献新的示例项目，请遵循以下规范:

1. **目录命名**: 使用小写加下划线 (如 `custom_control_demo`)
2. **结构统一**: 遵循标准的项目结构
3. **文档完整**: 包含详细的 README.md
4. **代码规范**: 使用最佳实践 API
5. **可构建**: 确保 CMake 配置正确
6. **有注释**: 关键代码添加中文注释

## 许可证

所有示例项目遵循 F__K_UI 框架的许可证。
