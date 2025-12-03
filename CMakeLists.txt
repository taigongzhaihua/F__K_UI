cmake_minimum_required(VERSION 3.10)
project(FK_UI_Minimal VERSION 0.1 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 包含 GNUInstallDirs 以便早期定义安装目录
include(GNUInstallDirs)
set(FK_UI_INSTALL_INCLUDEDIR ${CMAKE_INSTALL_INCLUDEDIR})

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

# glad 将直接编译到 fk 库中

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

# ===== libtess2 配置 =====
set(LIBTESS2_SOURCE_DIR "${THIRD_PARTY_DIR}/src/libtess2")
add_subdirectory(${LIBTESS2_SOURCE_DIR})

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
    # 第三方依赖（直接编译到库中以简化安装）
    ${THIRD_PARTY_DIR}/src/glad/glad.c
    
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
    
    # ========== ui 模块（重组后的新架构）==========
    
    # ui/base - 基础架构
    src/ui/base/Visual.cpp
    src/ui/base/VisualCollection.cpp
    src/ui/base/UIElement.cpp
    src/ui/base/FrameworkElement.cpp
    
    # ui/controls - 控件基类和通用控件
    src/ui/controls/Control.cpp
    src/ui/controls/ContentControl.cpp
    src/ui/controls/ContentPresenter.cpp
    src/ui/controls/ItemsControl.cpp
    src/ui/controls/ItemContainerGenerator.cpp
    src/ui/controls/Border.cpp
    src/ui/controls/Image.cpp
    src/ui/controls/Popup.cpp
    
    # ui/buttons - 按钮系控件
    src/ui/buttons/ButtonBase.cpp
    src/ui/buttons/Button.cpp
    src/ui/buttons/ToggleButton.cpp
    src/ui/buttons/CheckBox.cpp
    src/ui/buttons/RadioButton.cpp
    src/ui/buttons/RepeatButton.cpp
    
    # ui/text - 文本控件
    src/ui/text/TextBlock.cpp
    # src/ui/text/TextBox.cpp       # 不完整的实现，暂时禁用
    # src/ui/text/TextBoxBase.cpp   # 不完整的实现，暂时禁用
    
    # ui/lists - 列表控件
    src/ui/lists/ListBox.cpp
    src/ui/lists/ComboBox.cpp
    
    # ui/scrolling - 滚动相关
    src/ui/scrolling/Thumb.cpp
    src/ui/scrolling/Track.cpp
    src/ui/scrolling/ScrollBar.cpp
    src/ui/scrolling/ScrollContentPresenter.cpp
    src/ui/scrolling/ScrollViewer.cpp
    
    # ui/layouts - 布局容器
    src/ui/layouts/Panel.cpp
    src/ui/layouts/StackPanel.cpp
    src/ui/layouts/Grid.cpp
    src/ui/layouts/GridCellAttacher.cpp
    
    # ui/graphics - 图形
    src/ui/graphics/Shape.cpp
    src/ui/graphics/Brush.cpp
    src/ui/graphics/Transform.cpp
    
    # ui/styling - 样式和模板
    src/ui/styling/Setter.cpp
    src/ui/styling/FrameworkTemplate.cpp
    src/ui/styling/ControlTemplate.cpp
    src/ui/styling/DataTemplate.cpp
    # src/ui/styling/Style.cpp  # 头文件实现，不需要 cpp 文件
    
    # ui/input - 输入管理
    src/ui/input/InputManager.cpp
    src/ui/input/FocusManager.cpp
    src/ui/input/NameScope.cpp
    
    # ui - 窗口（特殊，放根目录）
    src/ui/Window.cpp
    
    # ========== ui 模块结束 ==========
    
    # render 模块（完整的生产级渲染系统）
    src/render/DrawCommand.cpp
    src/render/GlRenderer.cpp
    src/render/ColorUtils.cpp
    src/render/TextRenderer.cpp
    src/render/RenderContext.cpp  # Phase 5.0.1
    src/render/RenderList.cpp     # Phase 5.0.2
)

target_include_directories(fk PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)

# 使用生成器表达式支持构建树和安装树
target_include_directories(fk PUBLIC
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${THIRD_PARTY_DIR}/include>
    $<BUILD_INTERFACE:${FREETYPE_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${FREETYPE_BINARY_DIR}/include>
    $<INSTALL_INTERFACE:${FK_UI_INSTALL_INCLUDEDIR}>
)

# freetype 作为 PRIVATE 依赖，用户不需要直接链接
target_link_libraries(fk 
    PRIVATE freetype libtess2
    PUBLIC glfw3 ${OPENGL_LIBS} ${PLATFORM_LIBS}
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

# Demo Examples
add_executable(example_app examples/demos/complex_layout_demo.cpp)
target_link_libraries(example_app PRIVATE fk)

# Basic Examples

add_executable(radiobutton_demo examples/basic/radiobutton_demo.cpp)
target_link_libraries(radiobutton_demo PRIVATE fk)

add_executable(repeatbutton_demo examples/basic/repeatbutton_demo.cpp)
target_link_libraries(repeatbutton_demo PRIVATE fk)

add_executable(scrollviewer_demo examples/basic/scrollviewer_demo.cpp)
target_link_libraries(scrollviewer_demo PRIVATE fk)

add_executable(border_test examples/basic/border_test.cpp)
target_link_libraries(border_test PRIVATE fk)

add_executable(clip_test examples/basic/clip_test.cpp)
target_link_libraries(clip_test PRIVATE fk)

add_executable(constraint_test examples/basic/constraint_test.cpp)
target_link_libraries(constraint_test PRIVATE fk)

# Shape Examples
add_executable(shape_demo examples/shapes/shape_demo.cpp)
target_link_libraries(shape_demo PRIVATE fk)

add_executable(shape_test examples/shapes/shape_test.cpp)
target_link_libraries(shape_test PRIVATE fk)

add_executable(star_test examples/shapes/star_test.cpp)
target_link_libraries(star_test PRIVATE fk)

# Path Examples
add_executable(path_test examples/path/path_test.cpp)
target_link_libraries(path_test PRIVATE fk)

add_executable(path_line_test examples/path/path_line_test.cpp)
target_link_libraries(path_line_test PRIVATE fk)

add_executable(arc_demo examples/path/arc_demo.cpp)
target_link_libraries(arc_demo PRIVATE fk)

add_executable(simple_arc_test examples/path/simple_arc_test.cpp)
target_link_libraries(simple_arc_test PRIVATE fk)

add_executable(sweep_test examples/path/sweep_test.cpp)
target_link_libraries(sweep_test PRIVATE fk)

add_executable(multicolor_simple_test examples/path/multicolor_simple_test.cpp)
target_link_libraries(multicolor_simple_test PRIVATE fk)

add_executable(multicolor_path_test examples/path/multicolor_path_test.cpp)
target_link_libraries(multicolor_path_test PRIVATE fk)

add_executable(subpath_fill_test examples/path/subpath_fill_test.cpp)
target_link_libraries(subpath_fill_test PRIVATE fk)

# DataTemplate Examples
add_executable(datatemplate_demo examples/datatemplate/datatemplate_demo.cpp)
target_link_libraries(datatemplate_demo PRIVATE fk)

# ===== F__K_UI 库构建完成 =====
# 主项目专注于构建 libfk.a 静态库
# 
# 示例项目现在作为独立项目存在于 samples/ 目录中
# 要构建示例项目，请进入 samples/<示例名>/build 目录并运行:
#   cmake ..
#   cmake --build .
#
# 详见: samples/README.md

# ===== 库安装配置 =====
# 设置安装目录
set(FK_UI_INSTALL_LIBDIR ${CMAKE_INSTALL_LIBDIR})
set(FK_UI_INSTALL_BINDIR ${CMAKE_INSTALL_BINDIR})
set(FK_UI_INSTALL_CMAKEDIR ${CMAKE_INSTALL_LIBDIR}/cmake/FK_UI)

# 安装库文件（包括 freetype 作为依赖）
install(TARGETS fk freetype
    EXPORT FK_UI_Targets
    LIBRARY DESTINATION ${FK_UI_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${FK_UI_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${FK_UI_INSTALL_BINDIR}
    INCLUDES DESTINATION ${FK_UI_INSTALL_INCLUDEDIR}
)

# 安装头文件（包括模块头文件，不带扩展名）
install(DIRECTORY ${CMAKE_SOURCE_DIR}/include/fk
    DESTINATION ${FK_UI_INSTALL_INCLUDEDIR}
    FILES_MATCHING 
        PATTERN "*.h"
        PATTERN "Core"
        PATTERN "Application"
        PATTERN "Controls"
        PATTERN "Layouts"
        PATTERN "Graphics"
        PATTERN "Styling"
        PATTERN "DataBinding"
        PATTERN "Animation"
        PATTERN "Collections"
        PATTERN "Input"
        PATTERN "Rendering"
)

# 安装第三方依赖的头文件（如果需要暴露给用户）
install(DIRECTORY ${THIRD_PARTY_DIR}/include/
    DESTINATION ${FK_UI_INSTALL_INCLUDEDIR}
    FILES_MATCHING 
        PATTERN "*.h"
        PATTERN "*.hpp"
    PATTERN "glm/detail" EXCLUDE
)

# 安装 libtess2 头文件
install(DIRECTORY ${CMAKE_SOURCE_DIR}/third_party/src/libtess2/Include/
    DESTINATION ${FK_UI_INSTALL_INCLUDEDIR}
    FILES_MATCHING PATTERN "*.h"
)

# 将 libtess2 添加到导出集（作为 fk 的依赖）
install(TARGETS libtess2
    EXPORT FK_UI_Targets
    ARCHIVE DESTINATION ${FK_UI_INSTALL_LIBDIR}
)

# 生成并安装 CMake 配置文件
install(EXPORT FK_UI_Targets
    FILE FK_UITargets.cmake
    NAMESPACE FK_UI::
    DESTINATION ${FK_UI_INSTALL_CMAKEDIR}
)

# 创建版本配置文件
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/FK_UIConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

# 创建配置文件
configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/FK_UIConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/FK_UIConfig.cmake"
    INSTALL_DESTINATION ${FK_UI_INSTALL_CMAKEDIR}
)

# 安装配置文件
install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/FK_UIConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/FK_UIConfigVersion.cmake"
    DESTINATION ${FK_UI_INSTALL_CMAKEDIR}
)

# 导出构建树（可选，用于直接从构建目录使用）
export(EXPORT FK_UI_Targets
    FILE "${CMAKE_CURRENT_BINARY_DIR}/FK_UITargets.cmake"
    NAMESPACE FK_UI::
)

message(STATUS "F__K_UI library build configured successfully!")
message(STATUS "  - Library: libfk.a")
message(STATUS "  - Include: ${CMAKE_SOURCE_DIR}/include")
message(STATUS "  - Third-party: ${THIRD_PARTY_DIR}")
message(STATUS "  - Example target: example_app (examples/demos/complex_layout_demo.cpp)")
message(STATUS "  - Install prefix: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "  - Install command: cmake --install . [--prefix <path>]")

