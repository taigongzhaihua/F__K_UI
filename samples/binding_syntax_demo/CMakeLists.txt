cmake_minimum_required(VERSION 3.10)
project(BindingSyntaxDemo VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# F__K_UI 框架路径
set(FK_UI_ROOT "${CMAKE_SOURCE_DIR}/../..")
set(FK_UI_INCLUDE "${FK_UI_ROOT}/include")
set(FK_UI_LIB_DIR "${FK_UI_ROOT}/build")
set(FK_UI_THIRD_PARTY "${FK_UI_ROOT}/third_party")

# 查找 OpenGL
find_package(OpenGL REQUIRED)

# 添加可执行文件
add_executable(binding_syntax_demo
    src/main.cpp
)

# 包含头文件目录
target_include_directories(binding_syntax_demo PRIVATE
    ${FK_UI_INCLUDE}
    ${FK_UI_THIRD_PARTY}/include
)

# 链接库
target_link_libraries(binding_syntax_demo PRIVATE
    ${FK_UI_LIB_DIR}/libfk.a
    ${FK_UI_LIB_DIR}/third_party/src/freetype/libfreetype.a
    ${FK_UI_LIB_DIR}/libglad.a
    ${FK_UI_THIRD_PARTY}/lib/glfw3dll.lib
    OpenGL::GL
)

# 复制运行时依赖
add_custom_command(TARGET binding_syntax_demo POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${FK_UI_THIRD_PARTY}/lib/glfw3.dll"
        $<TARGET_FILE_DIR:binding_syntax_demo>
)
