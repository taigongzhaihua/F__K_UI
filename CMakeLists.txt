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

# ===== 示例可执行文件 (临时测试) =====
add_executable(example_app examples/main.cpp)
target_link_libraries(example_app PRIVATE fk)

add_executable(style_api_test examples/style_api_test.cpp)
target_link_libraries(style_api_test PRIVATE fk)

add_executable(style_template_demo examples/style_template_demo.cpp)
target_link_libraries(style_template_demo PRIVATE fk)

add_executable(binding_modes_demo examples/binding_modes_demo.cpp)
target_link_libraries(binding_modes_demo PRIVATE fk)

add_executable(async_binding_demo examples/async_binding_demo.cpp)
target_link_libraries(async_binding_demo PRIVATE fk)

add_executable(multibinding_demo examples/multibinding_demo.cpp)
target_link_libraries(multibinding_demo PRIVATE fk)

add_executable(multibinding_auto_update_demo examples/multibinding_auto_update_demo.cpp)
target_link_libraries(multibinding_auto_update_demo PRIVATE fk)

add_executable(validation_demo examples/validation_demo.cpp)
target_link_libraries(validation_demo PRIVATE fk)

add_executable(binding_validation_demo examples/binding_validation_demo.cpp)
target_link_libraries(binding_validation_demo PRIVATE fk)

add_executable(style_system_demo examples/style_system_demo.cpp)
target_link_libraries(style_system_demo PRIVATE fk)

add_executable(template_system_demo examples/template_system_demo.cpp)
target_link_libraries(template_system_demo PRIVATE fk)

add_executable(p2_features_demo examples/p2_features_demo.cpp)
target_link_libraries(p2_features_demo PRIVATE fk)

# phase2_demo 已移除（有 API 兼容性问题）
# 使用 brush_system_demo 和 contentpresenter_demo 代替
# add_executable(phase2_demo examples/phase2_demo.cpp)
# target_link_libraries(phase2_demo PRIVATE fk)

add_executable(brush_system_demo examples/brush_system_demo.cpp)
target_link_libraries(brush_system_demo PRIVATE fk)

add_executable(contentpresenter_demo examples/contentpresenter_demo.cpp)
target_link_libraries(contentpresenter_demo PRIVATE fk)

# Phase 4 - Animation System Demo
add_executable(animation_demo examples/animation_demo.cpp)
target_link_libraries(animation_demo PRIVATE fk)

add_executable(keyframe_animation_demo examples/keyframe_animation_demo.cpp)
target_link_libraries(keyframe_animation_demo PRIVATE fk)

add_executable(visual_state_demo examples/visual_state_demo.cpp)
target_link_libraries(visual_state_demo PRIVATE fk)

add_executable(button_custom_visual_states_demo examples/button_custom_visual_states_demo.cpp)
target_link_libraries(button_custom_visual_states_demo PRIVATE fk)

add_executable(button_visual_states_simple_test examples/button_visual_states_simple_test.cpp)
target_link_libraries(button_visual_states_simple_test PRIVATE fk)

add_executable(window_test examples/window_test.cpp)
target_link_libraries(window_test PRIVATE fk)

add_executable(theme_manager_demo examples/theme_manager_demo.cpp)
target_link_libraries(theme_manager_demo PRIVATE fk)

add_executable(performance_demo examples/performance_demo.cpp)
target_link_libraries(performance_demo PRIVATE fk)

# Phase 3 演示程序
add_executable(gradient_brush_demo examples/gradient_brush_demo.cpp)
target_link_libraries(gradient_brush_demo PRIVATE fk)

add_executable(scrollviewer_demo examples/scrollviewer_demo.cpp)
target_link_libraries(scrollviewer_demo PRIVATE fk)

add_executable(listbox_demo examples/listbox_demo.cpp)
target_link_libraries(listbox_demo PRIVATE fk)

add_executable(combobox_demo examples/combobox_demo.cpp)
target_link_libraries(combobox_demo PRIVATE fk)

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
message(STATUS "")
message(STATUS "To build sample projects, navigate to samples/ directory")
message(STATUS "  Example: cd samples/hello_fk_ui && mkdir build && cd build && cmake .. && cmake --build .")

# Fluent API Demo
add_executable(fluent_api_demo examples/fluent_api_demo.cpp)
target_link_libraries(fluent_api_demo PRIVATE fk)

# FindName Demo
add_executable(findname_demo examples/findname_demo.cpp)
target_link_libraries(findname_demo PRIVATE fk)

# ElementName Binding Demo
add_executable(elementname_binding_demo examples/elementname_binding_demo.cpp)
target_link_libraries(elementname_binding_demo PRIVATE fk)

# Duplicate Names Test
add_executable(duplicate_names_test examples/duplicate_names_test.cpp)
target_link_libraries(duplicate_names_test PRIVATE fk)

# Name Scope Demo
add_executable(name_scope_demo examples/name_scope_demo.cpp)
target_link_libraries(name_scope_demo PRIVATE fk)

# ElementName Scope Demo
add_executable(elementname_scope_demo examples/elementname_scope_demo.cpp)
target_link_libraries(elementname_scope_demo PRIVATE fk)

# NameScope Performance Demo (混合优化方案)
add_executable(namescope_performance_demo examples/namescope_performance_demo.cpp)
target_link_libraries(namescope_performance_demo PRIVATE fk)

# Template Binding Test
add_executable(template_binding_test examples/template_binding_test.cpp)
target_link_libraries(template_binding_test PRIVATE fk)

# Binding Debug Test
add_executable(binding_debug_test examples/binding_debug_test.cpp)
target_link_libraries(binding_debug_test PRIVATE fk)

# Button Render Test
add_executable(test_button_render test_button_render.cpp)
target_link_libraries(test_button_render PRIVATE fk)

# ContentControl Comprehensive Test
add_executable(test_contentcontrol_comprehensive test_contentcontrol_comprehensive.cpp)
target_link_libraries(test_contentcontrol_comprehensive PRIVATE fk)

# Button Template Test
add_executable(test_button_template test_button_template.cpp)
target_link_libraries(test_button_template PRIVATE fk)

# ContentControl OnRender Test
add_executable(test_contentcontrol_onrender test_contentcontrol_onrender.cpp)
target_link_libraries(test_contentcontrol_onrender PRIVATE fk)

# Render Trace Test
add_executable(test_render_trace test_render_trace.cpp)
target_link_libraries(test_render_trace PRIVATE fk)

# Simple Render Test
add_executable(test_simple_render test_simple_render.cpp)
target_link_libraries(test_simple_render PRIVATE fk)

# Button Comprehensive Render Test
add_executable(test_button_comprehensive_render test_button_comprehensive_render.cpp)
target_link_libraries(test_button_comprehensive_render PRIVATE fk)

# Main Example Issue Test
add_executable(test_main_example_issue test_main_example_issue.cpp)
target_link_libraries(test_main_example_issue PRIVATE fk)

# Button Background Test
add_executable(test_button_background test_button_background.cpp)
target_link_libraries(test_button_background PRIVATE fk)

# Button Click Event Test
add_executable(test_button_click_event test_button_click_event.cpp)
target_link_libraries(test_button_click_event PRIVATE fk)

# Example Main Simulation Test
add_executable(test_example_main_simulation test_example_main_simulation.cpp)
target_link_libraries(test_example_main_simulation PRIVATE fk)

# Coordinate Issue Test
add_executable(test_coordinate_issue test_coordinate_issue.cpp)
target_link_libraries(test_coordinate_issue PRIVATE fk)

# Simulate Renderer Test
add_executable(test_simulate_renderer test_simulate_renderer.cpp)
target_link_libraries(test_simulate_renderer PRIVATE fk)

# Fixed Renderer Test
add_executable(test_fixed_renderer test_fixed_renderer.cpp)
target_link_libraries(test_fixed_renderer PRIVATE fk)

# Global Coordinates Test
add_executable(test_global_coordinates test_global_coordinates.cpp)
target_link_libraries(test_global_coordinates PRIVATE fk)

# Before Fix Analysis Test
add_executable(test_before_fix_analysis test_before_fix_analysis.cpp)
target_link_libraries(test_before_fix_analysis PRIVATE fk)

# TextBlock Position Test
add_executable(test_textblock_position test_textblock_position.cpp)
target_link_libraries(test_textblock_position PRIVATE fk)

# TextBlock Visual Check Test
add_executable(test_textblock_visual_check test_textblock_visual_check.cpp)
target_link_libraries(test_textblock_visual_check PRIVATE fk)

# Transform Bug Test
add_executable(test_transform_bug test_transform_bug.cpp)
target_link_libraries(test_transform_bug PRIVATE fk)

# GlRenderer Fix Verification Test
add_executable(test_glrenderer_fix test_glrenderer_fix.cpp)
target_link_libraries(test_glrenderer_fix PRIVATE fk)

# Button New Features Demo
add_executable(button_new_features_demo examples/button_new_features_demo.cpp)
target_link_libraries(button_new_features_demo PRIVATE fk)

# Fluent Binding Demo
add_executable(fluent_binding_demo examples/fluent_binding_demo.cpp)
target_link_libraries(fluent_binding_demo PRIVATE fk)

# Property Macros Demo
add_executable(property_macros_demo examples/property_macros_demo.cpp)
target_link_libraries(property_macros_demo PRIVATE fk)

add_executable(test_vsb_minimal test_vsb_minimal.cpp)
target_link_libraries(test_vsb_minimal PRIVATE fk)

# Button State Transition Test
add_executable(test_button_state_transition test_button_state_transition.cpp)
target_link_libraries(test_button_state_transition PRIVATE fk)


# 边框零厚度测试
add_executable(test_border_zero_thickness ../test_border_zero_thickness.cpp)
target_link_libraries(test_border_zero_thickness PRIVATE fk glad freetype)
target_compile_features(test_border_zero_thickness PRIVATE cxx_std_23)
