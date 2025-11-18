# CheckGitStatus.cmake
# 检测Git仓库中是否有未提交的更改

# 查找git可执行文件
find_package(Git QUIET)

if(GIT_FOUND AND EXISTS "${CMAKE_SOURCE_DIR}/.git")
    # 检查是否有未提交的更改
    execute_process(
        COMMAND ${GIT_EXECUTABLE} status --porcelain
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE GIT_STATUS_OUTPUT
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET
    )
    
    # 检查是否有未跟踪或已修改的文件
    if(NOT "${GIT_STATUS_OUTPUT}" STREQUAL "")
        message(WARNING "====================================")
        message(WARNING "检测到未提交的更改！")
        message(WARNING "Detected uncommitted changes!")
        message(WARNING "====================================")
        message(WARNING "以下文件有未提交的更改：")
        message(WARNING "The following files have uncommitted changes:")
        message(WARNING "${GIT_STATUS_OUTPUT}")
        message(WARNING "====================================")
        message(WARNING "建议在构建前提交更改")
        message(WARNING "Recommend committing changes before building")
        message(WARNING "====================================")
        
        # 设置变量以便其他部分可以查询状态
        set(GIT_HAS_UNCOMMITTED_CHANGES TRUE PARENT_SCOPE)
    else()
        message(STATUS "Git仓库状态：干净 (Git repository: clean)")
        set(GIT_HAS_UNCOMMITTED_CHANGES FALSE PARENT_SCOPE)
    endif()
else()
    if(NOT GIT_FOUND)
        message(STATUS "未找到Git，跳过状态检查 (Git not found, skipping status check)")
    elseif(NOT EXISTS "${CMAKE_SOURCE_DIR}/.git")
        message(STATUS "不是Git仓库，跳过状态检查 (Not a Git repository, skipping status check)")
    endif()
    set(GIT_HAS_UNCOMMITTED_CHANGES FALSE PARENT_SCOPE)
endif()
