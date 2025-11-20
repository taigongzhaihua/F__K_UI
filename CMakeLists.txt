cmake_minimum_required(VERSION 3.10)
project(FK_UI_Minimal VERSION 0.1 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ===== 平台检测 =====
if(WIN32)
    message(STATUS "Platform: Windows")
elseif(UNIX AND NOT APPLE)
    message(STATUS "Platform: Linux")
elseif(APPLE)
    message(STATUS "Platform: macOS")
else()
    message(STATUS "Platform: Unknown")
endif()

set(THIRD_PARTY_DIR "${CMAKE_SOURCE_DIR}/third_party")
set(FREETYPE_SOURCE_DIR "${THIRD_PARTY_DIR}/src/freetype")
set(FREETYPE_BINARY_DIR "${CMAKE_BINARY_DIR}/third_party/src/freetype")

set(CMAKE_POLICY_VERSION_MINIMUM 3.5)

add_library(glad STATIC
    ${THIRD_PARTY_DIR}/src/glad/glad.c
)
target_include_directories(glad PUBLIC ${THIRD_PARTY_DIR}/include)

# ===== GLFW 跨平台配置 =====
set(GLFW_AVAILABLE FALSE)

if(WIN32)
    # Windows 平台：使用第三方库（MinGW 预编译）
    # 优先使用静态库，避免DLL依赖问题
    if(EXISTS "${THIRD_PARTY_DIR}/lib/libglfw3.a")
        # 使用静态库版本（MinGW）
        add_library(glfw3 STATIC IMPORTED)
        set_target_properties(glfw3 PROPERTIES
            IMPORTED_LOCATION "${THIRD_PARTY_DIR}/lib/libglfw3.a"
            INTERFACE_INCLUDE_DIRECTORIES "${THIRD_PARTY_DIR}/include"
        )
        message(STATUS "Using Windows GLFW static library from third_party")
        set(GLFW_AVAILABLE TRUE)
        set(GLFW_IS_DLL FALSE)
    elseif(EXISTS "${THIRD_PARTY_DIR}/lib/glfw3dll.lib")
        # 备用：使用 DLL 版本
        add_library(glfw3 SHARED IMPORTED)
        set_target_properties(glfw3 PROPERTIES
            IMPORTED_IMPLIB "${THIRD_PARTY_DIR}/lib/glfw3dll.lib"
            IMPORTED_LOCATION "${THIRD_PARTY_DIR}/lib/glfw3.dll"
            INTERFACE_INCLUDE_DIRECTORIES "${THIRD_PARTY_DIR}/include"
        )
        message(STATUS "Using Windows GLFW DLL from third_party")
        set(GLFW_AVAILABLE TRUE)
        set(GLFW_IS_DLL TRUE)
    else()
        message(FATAL_ERROR "GLFW library not found in third_party/lib/")
    endif()
    set(PLATFORM_LIBS "")
else()
    # Linux/Unix 平台：使用系统安装的 GLFW
    # 第三方目录中的 .a 文件是 Windows MinGW 库，不适用于 Linux
    message(STATUS "Linux platform: attempting to use system-installed GLFW")
    
    # 优先使用 pkg-config 查找 GLFW
    set(GLFW_AVAILABLE FALSE)
    find_package(PkgConfig)
    if(PKG_CONFIG_FOUND)
        pkg_check_modules(GLFW3 glfw3)
        if(GLFW3_FOUND)
            message(STATUS "Found GLFW3 via pkg-config: ${GLFW3_VERSION}")
            add_library(glfw3 INTERFACE IMPORTED)
            set_target_properties(glfw3 PROPERTIES
                INTERFACE_INCLUDE_DIRECTORIES "${GLFW3_INCLUDE_DIRS}"
                INTERFACE_LINK_LIBRARIES "${GLFW3_LIBRARIES}"
                INTERFACE_LINK_DIRECTORIES "${GLFW3_LIBRARY_DIRS}"
            )
            set(GLFW_AVAILABLE TRUE)
        endif()
    endif()
    
    # 如果 pkg-config 未找到，尝试使用 CMake 的 find_package
    if(NOT GLFW3_FOUND)
        find_package(glfw3 3.3)
        if(glfw3_FOUND)
            message(STATUS "Found GLFW3 via CMake: ${glfw3_VERSION}")
            # glfw3 目标已由 find_package 创建
            set(GLFW_AVAILABLE TRUE)
        else()
            message(WARNING "GLFW3 not found on system. Please install: sudo apt-get install libglfw3-dev")
            message(WARNING "Building without GLFW - examples may not work")
            # 创建一个空目标以避免链接错误
            add_library(glfw3 INTERFACE)
        endif()
    endif()
    
    # Linux 系统库
    find_package(X11)
    if(X11_FOUND)
        message(STATUS "Found X11: ${X11_LIBRARIES}")
        set(PLATFORM_LIBS ${X11_LIBRARIES} ${CMAKE_DL_LIBS} pthread)
    else()
        message(WARNING "X11 not found. Using headless configuration.")
        set(PLATFORM_LIBS ${CMAKE_DL_LIBS} pthread)
    endif()
endif()

set(WITH_ZLIB OFF CACHE BOOL "Disable zlib dependency for FreeType" FORCE)
set(WITH_BZIP2 OFF CACHE BOOL "Disable bzip2 dependency for FreeType" FORCE)
set(WITH_PNG OFF CACHE BOOL "Disable libpng dependency for FreeType" FORCE)
set(WITH_HarfBuzz OFF CACHE BOOL "Disable HarfBuzz dependency for FreeType" FORCE)
set(SKIP_INSTALL_ALL ON CACHE BOOL "Skip FreeType install targets" FORCE)

add_subdirectory(${FREETYPE_SOURCE_DIR})

# ===== OpenGL 配置 =====
# 在无头环境中 OpenGL 可能不可用，设为可选
find_package(OpenGL)
if(OPENGL_FOUND)
    message(STATUS "Found OpenGL: ${OPENGL_LIBRARIES}")
    set(OPENGL_LIBS OpenGL::GL)
else()
    message(WARNING "OpenGL not found. Building without OpenGL support.")
    # 创建一个空的 OpenGL 目标以避免链接错误
    add_library(OpenGL_stub INTERFACE)
    set(OPENGL_LIBS OpenGL_stub)
endif()

add_library(fk STATIC
    # app 模块
    src/app/Application.cpp
    
    # binding 模块
    src/binding/DependencyProperty.cpp
    src/binding/Binding.cpp
    src/binding/BindingExpression.cpp
    src/binding/BindingPath.cpp
    src/binding/BindingContext.cpp
    src/binding/DependencyObject.cpp
    src/binding/ValueConverters.cpp
    src/binding/PropertyStore.cpp
    src/binding/MultiBinding.cpp
    src/binding/MultiBindingExpression.cpp
    
    # core 模块
    src/core/Clock.cpp
    src/core/Dispatcher.cpp
    src/core/Logger.cpp
    src/core/Timer.cpp
    
    # animation 模块 (Phase 4)
    src/animation/Timeline.cpp
    src/animation/AnimationClock.cpp
    src/animation/AnimationManager.cpp
    src/animation/DoubleAnimation.cpp
    src/animation/ColorAnimation.cpp
    src/animation/PointAnimation.cpp
    src/animation/ThicknessAnimation.cpp
    src/animation/Storyboard.cpp
    src/animation/VisualStateManager.cpp
    src/animation/VisualStateBuilder.cpp
    
    # resources 模块 (Phase 4.3)
    src/resources/ThemeManager.cpp
    
    # performance 模块 (Phase 4.4)
    src/performance/GeometryCache.cpp
    src/performance/MaterialPool.cpp
    src/performance/RenderBatcher.cpp
    
    # ui 模块（新架构）
    src/ui/Visual.cpp
    src/ui/VisualCollection.cpp
    src/ui/UIElement.cpp
    src/ui/NameScope.cpp
    src/ui/FrameworkElement.cpp
    src/ui/Control.cpp
    src/ui/ContentControl.cpp
    src/ui/FrameworkTemplate.cpp
    src/ui/ControlTemplate.cpp
    src/ui/DataTemplate.cpp
    src/ui/Setter.cpp
    # src/ui/Style.cpp  # 头文件实现，不需要 cpp 文件
    src/ui/Window.cpp
    src/ui/TextBlock.cpp
    src/ui/Border.cpp
    src/ui/Image.cpp
    src/ui/Transform.cpp
    src/ui/InputManager.cpp
    src/ui/FocusManager.cpp
    src/ui/Panel.cpp
    src/ui/ItemsControl.cpp
    src/ui/ItemContainerGenerator.cpp
    src/ui/Button.cpp
    src/ui/StackPanel.cpp
    src/ui/Grid.cpp
    src/ui/GridCellAttacher.cpp
    src/ui/Brush.cpp
    src/ui/ContentPresenter.cpp
    src/ui/Shape.cpp
    src/ui/ScrollBar.cpp
    src/ui/ScrollViewer.cpp
    src/ui/ListBox.cpp
    src/ui/Popup.cpp
    src/ui/ComboBox.cpp
    src/ui/DrawCommand.cpp
    src/ui/Renderer.cpp
    src/ui/RenderBackend.cpp
    
    # render 模块（旧框架，暂时注释，新设计中 DrawCommand 在 ui 模块）
    # src/render/RenderHost.cpp
    # src/render/RenderScene.cpp
    # src/render/RenderTreeBuilder.cpp
    src/render/GlRenderer.cpp
    src/render/ColorUtils.cpp
    src/render/TextRenderer.cpp
    src/render/RenderContext.cpp  # Phase 5.0.1
    src/render/RenderList.cpp     # Phase 5.0.2
)

target_include_directories(fk PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)

target_include_directories(fk PUBLIC
    ${CMAKE_SOURCE_DIR}/include
    ${THIRD_PARTY_DIR}/include
    ${FREETYPE_SOURCE_DIR}/include
    ${FREETYPE_BINARY_DIR}/include
)

target_link_libraries(fk PUBLIC
    glad
    glfw3
    freetype
    ${OPENGL_LIBS}     # OpenGL 库（如果可用）
    ${PLATFORM_LIBS}   # 平台特定的库
)

# 添加 GLFW 和 OpenGL 可用性编译定义
if(GLFW_AVAILABLE)
    target_compile_definitions(fk PUBLIC FK_HAS_GLFW)
    message(STATUS "GLFW is available - defining FK_HAS_GLFW")
    if(GLFW_IS_DLL)
        target_compile_definitions(fk PUBLIC GLFW_DLL)
        message(STATUS "GLFW is DLL - defining GLFW_DLL")
    endif()
endif()

if(OPENGL_FOUND)
    target_compile_definitions(fk PUBLIC FK_HAS_OPENGL)
    message(STATUS "OpenGL is available - defining FK_HAS_OPENGL")
endif()

# ===== 示例可执行文件 =====
add_executable(example_app examples/main.cpp)
target_link_libraries(example_app PRIVATE fk)

# 其余示例程序已移除，仅保留主示例。

# ===== F__K_UI 库构建完成 =====
# 主项目专注于构建 libfk.a 静态库
# 
# 示例项目现在作为独立项目存在于 samples/ 目录中
# 要构建示例项目，请进入 samples/<示例名>/build 目录并运行:
#   cmake ..
#   cmake --build .
#
# 详见: samples/README.md

message(STATUS "F__K_UI library build configured successfully!")
message(STATUS "  - Library: libfk.a")
message(STATUS "  - Include: ${CMAKE_SOURCE_DIR}/include")
message(STATUS "  - Third-party: ${THIRD_PARTY_DIR}")
message(STATUS "  - Example target: example_app (examples/main.cpp)")

