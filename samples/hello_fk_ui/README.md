# Hello F__K_UI - 独立示例项目

这是一个独立的示例项目，演示如何将 F__K_UI 框架作为外部库引用到自己的项目中。

## 项目结构

```
hello_fk_ui/
├── CMakeLists.txt          # CMake 构建配置
├── README.md               # 项目说明
└── src/
    └── main.cpp            # 主程序源代码
```

## 功能展示

本示例展示了 F__K_UI 框架的以下核心功能:

- ✅ **应用程序生命周期管理**: 使用 `app::Application` 类
- ✅ **窗口创建**: 使用工厂函数 `ui::window()`
- ✅ **UI 控件**: Button、TextBlock、StackPanel
- ✅ **事件订阅**: 使用 `+=` 操作符订阅事件
- ✅ **流式 API**: 使用链式调用配置控件属性
- ✅ **布局系统**: StackPanel 垂直布局
- ✅ **交互逻辑**: 按钮点击计数器

## 构建前提

在构建本项目之前，需要先构建 F__K_UI 框架本身:

```powershell
# 进入 F__K_UI 主项目目录
cd ../..

# 创建构建目录并构建
mkdir -p build
cd build
cmake ..
cmake --build .
```

构建完成后会生成:
- `libfk.a` - F__K_UI 静态库
- `libglad.a` - OpenGL 加载器库
- `libfreetype.a` - 字体渲染库

## 构建本项目

```powershell
# 进入本项目目录
cd samples/hello_fk_ui

# 创建构建目录
mkdir -p build
cd build

# 配置 CMake (自动检测 F__K_UI 路径)
cmake ..

# 编译
cmake --build .

# 运行
./hello_fk_ui.exe  # Windows
./hello_fk_ui      # Linux/macOS
```

## 目录说明

本项目通过 `CMakeLists.txt` 中的以下变量引用 F__K_UI:

```cmake
set(FK_UI_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../..")
set(FK_UI_INCLUDE "${FK_UI_ROOT}/include")
set(FK_UI_LIB "${FK_UI_ROOT}/build")
set(FK_UI_THIRD_PARTY "${FK_UI_ROOT}/third_party")
```

如果你的 F__K_UI 安装在其他位置，可以修改 `FK_UI_ROOT` 变量。

## 依赖项

- **F__K_UI**: 主框架库
- **GLFW 3.3+**: 窗口管理
- **OpenGL 3.3+**: 图形渲染
- **FreeType**: 字体渲染
- **glad**: OpenGL 加载器

## 示例代码说明

### 1. 创建应用程序

```cpp
app::Application app;

app.Startup += [] {
    std::cout << "Application started!" << std::endl;
};
```

### 2. 创建窗口和控件

使用工厂函数创建控件（注意：不使用 `::Create()`）:

```cpp
auto mainWindow = ui::window()
    ->Title("Hello F__K_UI!")
    ->Width(600)
    ->Height(400);

auto button = ui::button()
    ->Content("Click Me!")
    ->Background("#3498DB");
```

### 3. 订阅事件

使用 `+=` 操作符订阅事件（注意：不使用 `.Add()`）:

```cpp
button->Click += [](ui::detail::ButtonBase&) {
    std::cout << "Button clicked!" << std::endl;
};
```

### 4. 运行应用程序

```cpp
app.Run(mainWindow);
```

## API 最佳实践

本示例遵循 F__K_UI 框架的最佳实践:

| 功能 | ✅ 推荐写法 | ❌ 不推荐 |
|------|------------|----------|
| 创建控件 | `ui::button()` | `Button::Create()` |
| 事件订阅 | `event += lambda` | `event.Add(lambda)` |
| 流式配置 | `button()->Content("OK")` | `button->SetContent("OK")` |

## 故障排除

### 链接错误: 找不到 libfk.a

确保已经构建了 F__K_UI 主项目，并且 `build/` 目录中存在 `libfk.a`。

### 运行时错误: 找不到 glfw3.dll

CMakeLists.txt 会自动复制 DLL，如果失败，手动复制:

```powershell
copy ../../third_party/lib/glfw3.dll ./build/
```

### 头文件找不到

检查 `FK_UI_ROOT` 路径是否正确指向 F__K_UI 主项目目录。

## 参考资料

- [F__K_UI API 文档](../../Docs/API/)
- [核心模块文档](../../Docs/API/Core.md)
- [UI 模块文档](../../Docs/API/UI.md)
- [应用程序模块文档](../../Docs/API/App.md)

## 许可证

本示例项目遵循 F__K_UI 框架的许可证。
