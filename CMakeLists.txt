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
    
    # ui 模块（新架构）
    src/ui/Visual.cpp
    src/ui/VisualCollection.cpp
    src/ui/UIElement.cpp
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
    src/ui/Button.cpp
    src/ui/StackPanel.cpp
    src/ui/Grid.cpp
    src/ui/Shape.cpp
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

# ===== 示例可执行文件 (临时测试) =====
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