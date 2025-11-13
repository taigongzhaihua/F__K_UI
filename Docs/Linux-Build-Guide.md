# Linux 构建指南

## 📋 概述

F__K_UI 框架现在完全支持跨平台构建，包括 Windows、Linux 和 macOS。CMakeLists.txt 已经过优化，可以自动检测平台并应用相应的配置。

## 🔧 跨平台适配改进

### 平台检测

CMake 会自动检测当前平台并输出信息：

```cmake
# ===== 平台检测 =====
if(WIN32)
    message(STATUS "Platform: Windows")
elseif(UNIX AND NOT APPLE)
    message(STATUS "Platform: Linux")
elseif(APPLE)
    message(STATUS "Platform: macOS")
endif()
```

### GLFW 库配置

根据平台使用不同的 GLFW 库来源：

**Windows**:
- 使用 `third_party/lib/` 中的预编译库（MinGW）
- 优先使用 DLL：`glfw3dll.lib` + `glfw3.dll`
- 备选静态库：`libglfw3.a`（Windows MinGW 格式）
- 如使用 DLL 则定义 `GLFW_DLL` 宏

**Linux/Unix**:
- **不使用** `third_party/lib/` 中的库（那些是 Windows MinGW 格式）
- 使用系统安装的 GLFW（通过 pkg-config 或 CMake find_package）
- 需要手动安装：`sudo apt-get install libglfw3-dev`
- 自动链接必要的系统库（X11、pthread、dl）

```cmake
if(WIN32)
    # Windows：使用第三方预编译库
    if(EXISTS "${THIRD_PARTY_DIR}/lib/glfw3dll.lib")
        add_library(glfw3 SHARED IMPORTED)
        set_target_properties(glfw3 PROPERTIES
            IMPORTED_IMPLIB "${THIRD_PARTY_DIR}/lib/glfw3dll.lib"
            IMPORTED_LOCATION "${THIRD_PARTY_DIR}/lib/glfw3.dll"
        )
        target_compile_definitions(fk PUBLIC GLFW_DLL)
    elseif(EXISTS "${THIRD_PARTY_DIR}/lib/libglfw3.a")
        add_library(glfw3 STATIC IMPORTED)
        set_target_properties(glfw3 PROPERTIES
            IMPORTED_LOCATION "${THIRD_PARTY_DIR}/lib/libglfw3.a"
        )
    endif()
else()
    # Linux：使用系统安装的 GLFW
    find_package(PkgConfig)
    if(PKG_CONFIG_FOUND)
        pkg_check_modules(GLFW3 glfw3)
    endif()
    if(NOT GLFW3_FOUND)
        find_package(glfw3 3.3)
    endif()
    if(NOT GLFW3_FOUND)
        message(WARNING "GLFW3 not found. Please install: sudo apt-get install libglfw3-dev")
    endif()
endif()
```

**重要说明**：`third_party/lib/libglfw3.a` 是 Windows MinGW 格式的库文件，**不能**在 Linux 上使用。Linux 必须使用系统安装的 GLFW。

### 无头环境支持

在 CI/CD 或无头服务器环境中，X11 和 OpenGL 可能不可用。CMakeLists.txt 现在将这些设置为可选依赖：

**X11 处理**:
```cmake
find_package(X11)
if(X11_FOUND)
    message(STATUS "Found X11: ${X11_LIBRARIES}")
    set(PLATFORM_LIBS ${X11_LIBRARIES} ${CMAKE_DL_LIBS} pthread)
else()
    message(WARNING "X11 not found. Using headless configuration.")
    set(PLATFORM_LIBS ${CMAKE_DL_LIBS} pthread)
endif()
```

**OpenGL 处理**:
```cmake
find_package(OpenGL)
if(OPENGL_FOUND)
    message(STATUS "Found OpenGL: ${OPENGL_LIBRARIES}")
    set(OPENGL_LIBS OpenGL::GL)
else()
    message(WARNING "OpenGL not found. Building without OpenGL support.")
    add_library(OpenGL_stub INTERFACE)
    set(OPENGL_LIBS OpenGL_stub)
endif()
```

## 🚀 Linux 构建步骤

### 前置要求

**最小要求**（无头环境，仅编译库）:
```bash
sudo apt-get update
sudo apt-get install -y \
    build-essential \
    cmake \
    libfreetype6-dev
```

**完整图形支持**（桌面环境，可运行示例程序）:
```bash
sudo apt-get update
sudo apt-get install -y \
    build-essential \
    cmake \
    libfreetype6-dev \
    libglfw3-dev \
    libx11-dev \
    libxrandr-dev \
    libxinerama-dev \
    libxcursor-dev \
    libxi-dev \
    libgl1-mesa-dev \
    libglu1-mesa-dev
```

**⚠️ 重要**: Linux 环境**必须**安装系统的 GLFW 库 (`libglfw3-dev`)。`third_party/lib/` 中的 `.a` 文件是 Windows MinGW 格式，不适用于 Linux。

### 构建项目

```bash
# 克隆仓库
git clone https://github.com/taigongzhaihua/F__K_UI.git
cd F__K_UI

# 创建构建目录
mkdir build
cd build

# 配置项目
cmake ..

# 构建（使用 4 个并行任务）
cmake --build . -j 4

# 或使用 make
make -j 4
```

### 构建输出

成功构建后，你会看到：

```
-- Platform: Linux
-- X11 not found. Using headless configuration.
-- OpenGL not found. Building without OpenGL support.
-- F__K_UI library build configured successfully!
--   - Library: libfk.a
--   - Include: /path/to/F__K_UI/include
--   - Third-party: /path/to/F__K_UI/third_party
```

### 运行示例

```bash
# 在 build 目录中
./phase2_demo
./style_system_demo
./binding_modes_demo
# ... 其他示例程序
```

## 📊 平台差异对照表

| 特性 | Windows | Linux | macOS |
|------|---------|-------|-------|
| GLFW 来源 | third_party 预编译 | 系统安装 (apt) | 系统安装 (brew) |
| GLFW 格式 | DLL 或 MinGW .a | ELF 共享库/静态库 | Mach-O 动态库 |
| GLFW_DLL 宏 | ✅ 定义（DLL模式） | ❌ 不定义 | ❌ 不定义 |
| X11 依赖 | ❌ 不需要 | ⚠️ 可选 | ❌ 不需要 |
| OpenGL | ✅ 内置 | ⚠️ 可选 | ✅ 内置 |
| 平台库 | 无 | pthread, dl | pthread, dl |
| third_party/lib/ | ✅ 可用 | ❌ 不可用（MinGW格式） | ❌ 可能不可用 |

## ⚠️ 已知问题

### 1. 无头环境限制

在无 X11 和 OpenGL 的环境中：
- ✅ 可以成功配置和构建库
- ⚠️ 无法运行需要图形界面的示例程序
- ✅ 可以用于 CI/CD 构建测试

### 2. OpenGL 依赖

某些渲染功能需要 OpenGL：
- GlRenderer - OpenGL 后端渲染器
- 图形显示和窗口创建

如果没有 OpenGL：
- 库可以编译
- 但运行时会缺少图形输出

### 3. GLFW 库来源混淆

**问题**: `third_party/lib/libglfw3.a` 是什么？

**答案**: 这是 Windows MinGW 编译的静态库，格式为 PE (Portable Executable)。虽然扩展名是 `.a`，但它**不是** Linux 原生格式。

验证方法：
```bash
$ file third_party/lib/libglfw3.a
third_party/lib/libglfw3.a: current ar archive

$ objdump -a third_party/lib/libglfw3.a | head
context.c.obj:     file format pe-x86-64  # ← Windows 格式！
```

Linux 系统需要 ELF 格式的库，必须从系统包管理器安装：
```bash
sudo apt-get install libglfw3-dev
```

## 🔧 故障排除

### CMake 配置失败

**问题**: `Could NOT find X11`
```
解决方案：
1. 安装 X11 开发库：sudo apt-get install libx11-dev
2. 或者接受警告，使用无头配置
```

**问题**: `Could NOT find OpenGL`
```
解决方案：
1. 安装 OpenGL 库：sudo apt-get install libgl1-mesa-dev
2. 或者接受警告，构建无图形支持的版本
```

### 编译错误

**问题**: `fatal error: GL/gl.h: No such file or directory`
```
解决方案：
安装 OpenGL 头文件：
sudo apt-get install mesa-common-dev libgl1-mesa-dev
```

**问题**: `undefined reference to pthread_create`
```
解决方案：
CMakeLists.txt 已自动链接 pthread，如仍有问题：
- 检查 CMake 版本 >= 3.10
- 重新配置：rm -rf build && mkdir build && cd build && cmake ..
```

### 运行时错误

**问题**: `error while loading shared libraries: libglfw.so.3`
```
解决方案：
使用静态库版本（已配置），或设置 LD_LIBRARY_PATH：
export LD_LIBRARY_PATH=/path/to/third_party/lib:$LD_LIBRARY_PATH
```

## 📝 CI/CD 配置示例

### GitHub Actions

```yaml
name: Linux Build

on: [push, pull_request]

jobs:
  build:
    runs-on: ubuntu-latest
    
    steps:
    - uses: actions/checkout@v2
    
    - name: Install dependencies
      run: |
        sudo apt-get update
        sudo apt-get install -y build-essential cmake libfreetype6-dev
    
    - name: Configure
      run: |
        mkdir build
        cd build
        cmake ..
    
    - name: Build
      run: |
        cd build
        cmake --build . -j 4
    
    - name: Test
      run: |
        cd build
        ctest --output-on-failure
```

### Docker 构建

```dockerfile
FROM ubuntu:22.04

RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    libfreetype6-dev \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /workspace
COPY . .

RUN mkdir build && cd build && \
    cmake .. && \
    cmake --build . -j 4

CMD ["/bin/bash"]
```

## 🎯 最佳实践

1. **开发环境**: 安装完整的图形库支持
2. **CI/CD**: 使用最小依赖，接受无头配置
3. **生产部署**: 根据目标环境选择依赖
4. **跨平台开发**: 在所有目标平台上测试

## 📚 相关文档

- [Phase2-Features.md](Phase2-Features.md) - Phase 2 功能说明
- [DevelopmentPlan.md](DevelopmentPlan.md) - 完整开发计划
- [README.md](../README.md) - 项目概述

---

**文档版本**: 1.0  
**最后更新**: 2025-11-13  
**作者**: GitHub Copilot
