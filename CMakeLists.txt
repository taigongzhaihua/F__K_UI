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
    src/ui/TextBlock.cpp
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

add_executable(fk_example
    examples/main.cpp
)

target_link_libraries(fk_example PRIVATE fk)

add_custom_command(TARGET fk_example POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${THIRD_PARTY_DIR}/lib/glfw3.dll"
        $<TARGET_FILE_DIR:fk_example>
)

# Phase 2 Demo
add_executable(phase2_demo
    examples/phase2_demo.cpp
)

target_link_libraries(phase2_demo PRIVATE fk)

add_custom_command(TARGET phase2_demo POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${THIRD_PARTY_DIR}/lib/glfw3.dll"
        $<TARGET_FILE_DIR:phase2_demo>
)

# Phase 3 Demo
add_executable(phase3_demo
    examples/phase3_demo.cpp
)

target_link_libraries(phase3_demo PRIVATE fk)

add_custom_command(TARGET phase3_demo POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${THIRD_PARTY_DIR}/lib/glfw3.dll"
        $<TARGET_FILE_DIR:phase3_demo>
)

# Test Getter Overloads
add_executable(test_getter
    examples/test_getter.cpp
)

target_link_libraries(test_getter PRIVATE fk)

add_custom_command(TARGET test_getter POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${THIRD_PARTY_DIR}/lib/glfw3.dll"
        $<TARGET_FILE_DIR:test_getter>
)

# Test Window Width Resolution
add_executable(test_window_width
    examples/test_window_width.cpp
)

target_link_libraries(test_window_width PRIVATE fk)

add_custom_command(TARGET test_window_width POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${THIRD_PARTY_DIR}/lib/glfw3.dll"
        $<TARGET_FILE_DIR:test_window_width>
)

# Test Panel Children Dependency Property
add_executable(test_panel_children
    examples/test_panel_children.cpp
)

target_link_libraries(test_panel_children PRIVATE fk)

add_custom_command(TARGET test_panel_children POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${THIRD_PARTY_DIR}/lib/glfw3.dll"
        $<TARGET_FILE_DIR:test_panel_children>
)

# Children API Demo
add_executable(children_api_demo
    examples/children_api_demo.cpp
)

target_link_libraries(children_api_demo PRIVATE fk)

add_custom_command(TARGET children_api_demo POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${THIRD_PARTY_DIR}/lib/glfw3.dll"
        $<TARGET_FILE_DIR:children_api_demo>
)

# Test Panel CRTP
add_executable(test_panel_crtp
    examples/test_panel_crtp.cpp
)

target_link_libraries(test_panel_crtp PRIVATE fk)

add_custom_command(TARGET test_panel_crtp POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${THIRD_PARTY_DIR}/lib/glfw3.dll"
        $<TARGET_FILE_DIR:test_panel_crtp>
)

# test_items_control_children 示例
add_executable(test_items_control_children
    examples/test_items_control_children.cpp
)

target_link_libraries(test_items_control_children PRIVATE fk)

add_custom_command(TARGET test_items_control_children POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${THIRD_PARTY_DIR}/lib/glfw3.dll"
        $<TARGET_FILE_DIR:test_items_control_children>
)

# test_items_control_improvements 示例
add_executable(test_items_control_improvements
    examples/test_items_control_improvements.cpp
)

target_link_libraries(test_items_control_improvements PRIVATE fk)

add_custom_command(TARGET test_items_control_improvements POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${THIRD_PARTY_DIR}/lib/glfw3.dll"
        $<TARGET_FILE_DIR:test_items_control_improvements>
)

# button_example 示例
add_executable(button_example
    examples/button_example.cpp
)

target_link_libraries(button_example PRIVATE fk)

add_custom_command(TARGET button_example POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${THIRD_PARTY_DIR}/lib/glfw3.dll"
        $<TARGET_FILE_DIR:button_example>
)

# Button Render Test
add_executable(button_render_test
    examples/button_render_test.cpp
)

target_link_libraries(button_render_test PRIVATE fk)

add_custom_command(TARGET button_render_test POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${THIRD_PARTY_DIR}/lib/glfw3.dll"
        $<TARGET_FILE_DIR:button_render_test>
)

# Button Direct Render Demo
add_executable(button_direct_render
    examples/button_direct_render.cpp
)

target_link_libraries(button_direct_render PRIVATE fk)

add_custom_command(TARGET button_direct_render POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${THIRD_PARTY_DIR}/lib/glfw3.dll"
        $<TARGET_FILE_DIR:button_direct_render>
)

# Simple Render Test
add_executable(simple_render_test
    examples/simple_render_test.cpp
)

target_link_libraries(simple_render_test PRIVATE fk)

add_custom_command(TARGET simple_render_test POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${THIRD_PARTY_DIR}/lib/glfw3.dll"
        $<TARGET_FILE_DIR:simple_render_test>
)

add_executable(minesweeper
    minesweeper.cpp
)
target_include_directories(minesweeper PUBLIC
    ${CMAKE_SOURCE_DIR}/include
    ${THIRD_PARTY_DIR}/include
    ${FREETYPE_SOURCE_DIR}/include
    ${FREETYPE_BINARY_DIR}/include
)
target_link_libraries(minesweeper PUBLIC
    glad
    glfw3
    freetype
)