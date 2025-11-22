# libtess2 - Tesselation library
# https://github.com/memononen/libtess2

cmake_minimum_required(VERSION 3.10)

# libtess2 源文件
set(LIBTESS2_SOURCES
    Source/bucketalloc.c
    Source/dict.c
    Source/geom.c
    Source/mesh.c
    Source/priorityq.c
    Source/sweep.c
    Source/tess.c
)

# 创建静态库
add_library(libtess2 STATIC ${LIBTESS2_SOURCES})

# 设置头文件目录（使用生成器表达式支持导出）
target_include_directories(libtess2 PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Include>
    $<INSTALL_INTERFACE:include>
)

# 设置编译选项
if(MSVC)
    target_compile_options(libtess2 PRIVATE /W3)
else()
    target_compile_options(libtess2 PRIVATE -Wall -Wextra)
endif()

# 导出头文件目录给父项目
set(LIBTESS2_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Include PARENT_SCOPE)
