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
    src/binding/DependencyObject.cpp
    src/binding/PropertyStore.cpp
    src/core/Clock.cpp
    src/core/Dispatcher.cpp
    src/core/Logger.cpp
    src/core/Timer.cpp
    src/ui/DispatcherObject.cpp
    src/ui/DependencyObject.cpp
    src/ui/Control.cpp
    src/ui/ContentControl.cpp
    src/ui/Decorator.cpp
    src/ui/ItemsControl.cpp
    src/ui/FrameworkElement.cpp
    src/ui/Panel.cpp
    src/ui/StackPanel.cpp
    src/ui/UIElement.cpp
    src/ui/Window.cpp
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
