cmake_minimum_required(VERSION 3.10)
project(FK_UI_Minimal VERSION 0.1 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(THIRD_PARTY_DIR "${CMAKE_SOURCE_DIR}/third_party")
set(FREETYPE_SOURCE_DIR "${THIRD_PARTY_DIR}/src/freetype")
set(FREETYPE_BINARY_DIR "${CMAKE_BINARY_DIR}/third_party/src/freetype")

set(CMAKE_POLICY_VERSION_MINIMUM 3.5)

add_library(glad STATIC
    ${THIRD_PARTY_DIR}/src/glad/glad.c
)
target_include_directories(glad PUBLIC ${THIRD_PARTY_DIR}/include)

add_library(glfw3 SHARED IMPORTED)
set_target_properties(glfw3 PROPERTIES
    IMPORTED_IMPLIB "${THIRD_PARTY_DIR}/lib/glfw3dll.lib"
    IMPORTED_LOCATION "${THIRD_PARTY_DIR}/lib/glfw3.dll"
    INTERFACE_INCLUDE_DIRECTORIES "${THIRD_PARTY_DIR}/include"
)

set(WITH_ZLIB OFF CACHE BOOL "Disable zlib dependency for FreeType" FORCE)
set(WITH_BZIP2 OFF CACHE BOOL "Disable bzip2 dependency for FreeType" FORCE)
set(WITH_PNG OFF CACHE BOOL "Disable libpng dependency for FreeType" FORCE)
set(WITH_HarfBuzz OFF CACHE BOOL "Disable HarfBuzz dependency for FreeType" FORCE)
set(SKIP_INSTALL_ALL ON CACHE BOOL "Skip FreeType install targets" FORCE)

add_subdirectory(${FREETYPE_SOURCE_DIR})

find_package(OpenGL REQUIRED)

add_library(fk STATIC
    src/app/Application.cpp
    src/binding/DependencyProperty.cpp
    src/binding/Binding.cpp
    src/binding/BindingExpression.cpp
    src/binding/BindingPath.cpp
    src/binding/BindingContext.cpp
    src/binding/DependencyObject.cpp
    src/binding/ValueConverters.cpp
    src/binding/PropertyStore.cpp
    src/core/Clock.cpp
    src/core/Dispatcher.cpp
    src/core/Logger.cpp
    src/core/Timer.cpp
    src/ui/DispatcherObject.cpp
    src/ui/DependencyObject.cpp
    src/ui/Button.cpp
    src/ui/Control.cpp
    src/ui/ContentControl.cpp
    src/ui/Decorator.cpp
    src/ui/ItemsControl.cpp
    src/ui/ScrollBar.cpp
    src/ui/ScrollViewer.cpp
    src/ui/FrameworkElement.cpp
    src/ui/Panel.cpp
    src/ui/StackPanel.cpp
    src/ui/Grid.cpp
    src/ui/TextBlock.cpp
    src/ui/TextBox.cpp
    src/ui/UIElement.cpp
    src/ui/Window.cpp
    src/ui/WindowInteropHelper.cpp
    src/ui/ThreadingConfig.cpp
    src/render/RenderHost.cpp
    src/render/RenderScene.cpp
    src/render/RenderTreeBuilder.cpp
    src/render/GlRenderer.cpp
    src/render/ColorUtils.cpp
    src/render/TextRenderer.cpp
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
    OpenGL::GL
)

target_compile_definitions(fk PUBLIC GLFW_DLL)

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