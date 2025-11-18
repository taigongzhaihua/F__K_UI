# Git Status Check / Git状态检查

## Overview / 概述

This feature automatically checks for uncommitted changes in the Git repository during the CMake configuration phase. It helps developers ensure they haven't forgotten to commit important changes before building.

此功能在CMake配置阶段自动检查Git仓库中的未提交更改。它帮助开发者确保在构建之前没有忘记提交重要的更改。

## How It Works / 工作原理

When you run `cmake` to configure the project, the build system will:

当你运行`cmake`配置项目时，构建系统会：

1. Check if Git is installed and the project is in a Git repository
   检查Git是否已安装以及项目是否在Git仓库中
2. Run `git status --porcelain` to detect uncommitted changes
   运行`git status --porcelain`来检测未提交的更改
3. Display a warning message if uncommitted changes are detected
   如果检测到未提交的更改，显示警告消息
4. List all files with uncommitted changes
   列出所有有未提交更改的文件

## Warning Message Example / 警告消息示例

```
CMake Warning:
====================================
检测到未提交的更改！
Detected uncommitted changes!
====================================
以下文件有未提交的更改：
The following files have uncommitted changes:
 M CMakeLists.txt
 M src/ui/Button.cpp
?? new_file.txt
====================================
建议在构建前提交更改
Recommend committing changes before building
====================================
```

## Disabling the Check / 禁用检查

If you want to disable this check (e.g., during development when frequent uncommitted changes are expected), you can:

如果你想禁用此检查（例如，在开发过程中经常有未提交的更改），你可以：

### Method 1: CMake Option / 方法1：CMake选项

```bash
cmake -DCHECK_GIT_STATUS=OFF ..
```

### Method 2: In CMakeLists.txt / 方法2：在CMakeLists.txt中

Modify the option in `CMakeLists.txt`:

在`CMakeLists.txt`中修改选项：

```cmake
option(CHECK_GIT_STATUS "..." OFF)  # Change ON to OFF
```

### Method 3: CMake Cache / 方法3：CMake缓存

```bash
cmake -DCHECK_GIT_STATUS=OFF .
# or use ccmake to interactively toggle the option
```

## Why This Is Useful / 为什么这很有用

- **Prevents accidental commits**: Reminds you to commit changes before creating a release build
  **防止意外提交**：提醒你在创建发布版本之前提交更改

- **Development workflow**: Helps maintain a clean git history
  **开发工作流**：帮助维护干净的git历史

- **Team collaboration**: Ensures all team members are aware of uncommitted changes
  **团队协作**：确保所有团队成员都了解未提交的更改

- **Build reproducibility**: Helps track exactly what code was built
  **构建可重现性**：帮助准确跟踪构建了哪些代码

## Implementation / 实现

The feature is implemented in two files:

此功能在两个文件中实现：

1. **cmake/CheckGitStatus.cmake**: The main check logic
   **cmake/CheckGitStatus.cmake**：主要的检查逻辑

2. **CMakeLists.txt**: Integration point that includes the check
   **CMakeLists.txt**：包含检查的集成点

## Notes / 注意事项

- This is a **warning only** - the build will not fail
  这只是一个**警告** - 构建不会失败

- The check runs during CMake configuration, not during every build
  检查在CMake配置期间运行，而不是在每次构建时运行

- If Git is not installed or the directory is not a Git repository, the check is silently skipped
  如果Git未安装或目录不是Git仓库，检查会被静默跳过

- The check includes both modified tracked files and untracked files
  检查包括已修改的跟踪文件和未跟踪的文件
