cmake_minimum_required(VERSION 3.10)
project(textbox_wrapping_demo)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 设置 F__K_UI 库路径
set(FK_UI_ROOT "${CMAKE_SOURCE_DIR}/../..")
set(FK_UI_INCLUDE "${FK_UI_ROOT}/include")
set(FK_UI_LIB "${FK_UI_ROOT}/build")
set(THIRD_PARTY_DIR "${FK_UI_ROOT}/third_party")

# 查找 OpenGL
find_package(OpenGL REQUIRED)

# 添加可执行文件
add_executable(textbox_wrapping_demo main.cpp)

# 包含目录
target_include_directories(textbox_wrapping_demo PRIVATE
    ${FK_UI_INCLUDE}
    ${THIRD_PARTY_DIR}/include
    ${THIRD_PARTY_DIR}/src/freetype/include
    ${FK_UI_LIB}/third_party/src/freetype/include
)

# 链接库
target_link_libraries(textbox_wrapping_demo PRIVATE
    ${FK_UI_LIB}/libfk.a
    ${FK_UI_LIB}/third_party/src/freetype/libfreetype.a
    ${FK_UI_LIB}/libglad.a
    ${THIRD_PARTY_DIR}/lib/glfw3dll.lib
    OpenGL::GL
)

# 编译定义
target_compile_definitions(textbox_wrapping_demo PRIVATE GLFW_DLL)

# 复制 DLL 到输出目录
add_custom_command(TARGET textbox_wrapping_demo POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${THIRD_PARTY_DIR}/lib/glfw3.dll"
    $<TARGET_FILE_DIR:textbox_wrapping_demo>
)

message(STATUS "TextBox Wrapping Demo configured successfully!")
