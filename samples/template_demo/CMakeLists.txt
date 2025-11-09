cmake_minimum_required(VERSION 3.10)
project(template_demo)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 设置父项目路径
set(FK_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../..")
set(THIRD_PARTY_DIR "${FK_ROOT}/third_party")

# 假设 fk 已经构建在 FK_ROOT/build
find_library(FK_LIBRARY NAMES fk PATHS "${FK_ROOT}/build" NO_DEFAULT_PATH)
find_library(GLAD_LIBRARY NAMES glad PATHS "${FK_ROOT}/build" NO_DEFAULT_PATH)
find_library(FREETYPE_LIBRARY NAMES freetype PATHS "${FK_ROOT}/build/third_party/src/freetype" NO_DEFAULT_PATH)

find_package(OpenGL REQUIRED)

# 导入 glfw3
add_library(glfw3 SHARED IMPORTED)
set_target_properties(glfw3 PROPERTIES
    IMPORTED_IMPLIB "${THIRD_PARTY_DIR}/lib/glfw3dll.lib"
    IMPORTED_LOCATION "${THIRD_PARTY_DIR}/lib/glfw3.dll"
)

add_executable(template_demo main.cpp)

target_include_directories(template_demo PRIVATE
    "${FK_ROOT}/include"
    "${THIRD_PARTY_DIR}/include"
)

target_link_libraries(template_demo PRIVATE
    ${FK_LIBRARY}
    ${GLAD_LIBRARY}
    ${FREETYPE_LIBRARY}
    glfw3
    OpenGL::GL
)

# 复制 GLFW DLL
add_custom_command(TARGET template_demo POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${THIRD_PARTY_DIR}/lib/glfw3.dll"
        $<TARGET_FILE_DIR:template_demo>
)
