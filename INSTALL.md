# F__K_UI 库安装和使用指南

## 构建和安装库

### 1. 构建库

```bash
cd F__K_UI
mkdir build && cd build
cmake ..
cmake --build . -j8
```

### 2. 安装库

**安装到默认位置：**
```bash
# Linux/macOS: /usr/local
# Windows: C:\Program Files\FK_UI
cmake --install .
```

**安装到自定义位置：**
```bash
cmake --install . --prefix /path/to/install
```

### 3. 安装内容

安装后的目录结构：
```
<install_prefix>/
├── include/
│   └── fk/
│       ├── FK_UI.h              # 主头文件
│       ├── animation/           # 动画系统
│       ├── app/                 # 应用程序框架
│       ├── binding/             # 数据绑定
│       ├── core/                # 核心工具
│       ├── performance/         # 性能优化
│       ├── render/              # 渲染系统
│       ├── resources/           # 资源管理
│       └── ui/                  # UI 控件
├── lib/
│   ├── libfk.a                  # 静态库
│   └── cmake/
│       └── FK_UI/
│           ├── FK_UIConfig.cmake
│           ├── FK_UIConfigVersion.cmake
│           └── FK_UITargets.cmake
```

## 在项目中使用 F__K_UI

### 方法 1: 使用 CMake find_package（推荐）

**CMakeLists.txt:**
```cmake
cmake_minimum_required(VERSION 3.10)
project(MyApp)

set(CMAKE_CXX_STANDARD 23)

# 查找 FK_UI 库
find_package(FK_UI REQUIRED)

# 创建可执行文件
add_executable(myapp main.cpp)

# 链接 FK_UI 库
target_link_libraries(myapp PRIVATE FK_UI::fk)
```

**main.cpp:**
```cpp
#include <fk/FK_UI.h>

int main(int argc, char** argv) {
    fk::Application app;
    
    auto window = std::make_shared<fk::ui::Window>();
    window->Title("My Application")
          ->Width(800)
          ->Height(600)
          ->Background(new fk::ui::SolidColorBrush(240, 245, 255))
          ->Content(
              (new fk::ui::StackPanel())
                  ->Children({
                      (new fk::ui::TextBlock())
                          ->Text("Hello, F__K_UI!")
                          ->FontSize(24)
                  })
          );
    
    return app.Run(window);
}
```

**构建项目:**
```bash
mkdir build && cd build
cmake ..
cmake --build .
```

**如果安装到自定义位置:**
```bash
cmake -DCMAKE_PREFIX_PATH=/path/to/install ..
```

### 方法 2: 手动指定头文件和库

**CMakeLists.txt:**
```cmake
cmake_minimum_required(VERSION 3.10)
project(MyApp)

set(CMAKE_CXX_STANDARD 23)

# 手动指定包含目录
include_directories(/path/to/install/include)

# 创建可执行文件
add_executable(myapp main.cpp)

# 手动链接库
target_link_libraries(myapp PRIVATE 
    /path/to/install/lib/libfk.a
    # 还需要链接依赖
    OpenGL::GL
    glfw3
    freetype
)
```

### 方法 3: 使用系统包管理器（未来）

```bash
# Ubuntu/Debian
sudo apt install libfk-ui-dev

# Fedora/RHEL
sudo dnf install fk-ui-devel

# macOS
brew install fk-ui

# Windows (vcpkg)
vcpkg install fk-ui
```

## 完整示例

### 创建简单窗口

```cpp
#include <fk/FK_UI.h>

int main() {
    fk::Application app;
    
    auto window = std::make_shared<fk::ui::Window>();
    window->Title("Simple Window")
          ->Width(800)
          ->Height(600);
    
    return app.Run(window);
}
```

### 使用布局和控件

```cpp
#include <fk/FK_UI.h>

int main() {
    fk::Application app;
    
    auto window = std::make_shared<fk::ui::Window>();
    window->Title("Layout Example")
          ->Width(800)
          ->Height(600)
          ->Content(
              (new fk::ui::Grid())
                  ->Rows("Auto, *")
                  ->Children({
                      // 标题栏
                      (new fk::ui::Border())
                          ->Background(new fk::ui::SolidColorBrush(60, 80, 120))
                          ->Child(
                              (new fk::ui::TextBlock())
                                  ->Text("My App")
                                  ->FontSize(20)
                                  ->Foreground(fk::ui::Brushes::White())
                          ) | fk::ui::cell(0, 0),
                      
                      // 内容区域
                      (new fk::ui::StackPanel())
                          ->Children({
                              (new fk::ui::Button())
                                  ->Content("Click Me")
                                  ->Width(100)
                                  ->Height(30)
                          }) | fk::ui::cell(1, 0)
                  })
          );
    
    return app.Run(window);
}
```

### 使用数据绑定

```cpp
#include <fk/FK_UI.h>

class ViewModel : public fk::binding::ObservableObject {
public:
    OBSERVABLE_PROPERTY(std::string, Title, "Hello")
    OBSERVABLE_PROPERTY(int, Counter, 0)
};

int main() {
    fk::Application app;
    auto viewModel = std::make_shared<ViewModel>();
    
    auto window = std::make_shared<fk::ui::Window>();
    window->SetDataContext(viewModel)
          ->Content(
              (new fk::ui::StackPanel())
                  ->Children({
                      (new fk::ui::TextBlock())
                          ->Binding("Text", "Title"),
                      (new fk::ui::Button())
                          ->Content("Increment")
                          ->Click([viewModel](auto, auto) {
                              viewModel->SetCounter(viewModel->GetCounter() + 1);
                          })
                  })
          );
    
    return app.Run(window);
}
```

## 依赖项

F__K_UI 需要以下依赖：

- **OpenGL 3.3+** - 图形渲染
- **GLFW 3.x** - 窗口管理和输入
- **FreeType 2.x** - 文本渲染
- **C++23** 编译器

这些依赖会在安装时自动包含头文件，但运行时可能需要：
- Linux: `sudo apt install libglfw3 libfreetype6 libgl1`
- Windows: DLL 文件应在安装目录的 `bin/` 中
- macOS: `brew install glfw freetype`

## 故障排除

### find_package 找不到 FK_UI

1. 确认已正确安装：`cmake --install . --prefix <path>`
2. 设置 CMAKE_PREFIX_PATH：`cmake -DCMAKE_PREFIX_PATH=<install_path> ..`
3. 或设置环境变量：`export CMAKE_PREFIX_PATH=<install_path>`

### 链接错误

确保链接了所有依赖库：
```cmake
find_package(OpenGL REQUIRED)
target_link_libraries(myapp PRIVATE FK_UI::fk OpenGL::GL)
```

### 运行时缺少 DLL (Windows)

将 `<install_path>/bin/` 添加到 PATH，或将 DLL 复制到可执行文件目录。

## 更多资源

- [API 文档](https://github.com/taigongzhaihua/F__K_UI/tree/main/Docs/API)
- [示例代码](https://github.com/taigongzhaihua/F__K_UI/tree/main/examples)
- [开发指南](https://github.com/taigongzhaihua/F__K_UI/blob/main/Docs/Development.md)
