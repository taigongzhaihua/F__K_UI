# 集成 libtess2 说明

## 下载 libtess2 源码

请按照以下步骤下载 libtess2 的源文件：

### 方法一：使用 Git（推荐）

```bash
cd third_party/src/libtess2
git clone https://github.com/memononen/libtess2.git temp
cp temp/Source/* ./Source/
cp temp/Include/* ./Include/
rm -rf temp
```

### 方法二：手动下载

1. 访问 https://github.com/memononen/libtess2
2. 下载以下文件到 `third_party/src/libtess2/Source/` 目录：
   - bucketalloc.c
   - bucketalloc.h
   - dict.c
   - dict.h
   - geom.c
   - geom.h
   - mesh.c
   - mesh.h
   - priorityq.c
   - priorityq.h
   - sweep.c
   - sweep.h
   - tess.c
   - tess.h

3. 下载以下文件到 `third_party/src/libtess2/Include/` 目录：
   - tesselator.h（已提供）

## 需要的源文件列表

Source 目录下需要这些文件：
- bucketalloc.c
- bucketalloc.h
- dict.c
- dict.h
- geom.c
- geom.h
- mesh.c
- mesh.h
- priorityq.c
- priorityq.h
- sweep.c
- sweep.h
- tess.c
- tess.h

## 验证安装

文件结构应该如下：

```
third_party/src/libtess2/
├── CMakeLists.txt
├── Include/
│   └── tesselator.h
└── Source/
    ├── bucketalloc.c
    ├── bucketalloc.h
    ├── dict.c
    ├── dict.h
    ├── geom.c
    ├── geom.h
    ├── mesh.c
    ├── mesh.h
    ├── priorityq.c
    ├── priorityq.h
    ├── sweep.c
    ├── sweep.h
    ├── tess.c
    └── tess.h
```

## PowerShell 一键下载脚本

```powershell
# 在项目根目录运行此脚本
$baseUrl = "https://raw.githubusercontent.com/memononen/libtess2/master"
$sourceDir = "third_party/src/libtess2/Source"
$includeDir = "third_party/src/libtess2/Include"

# 创建目录
New-Item -ItemType Directory -Force -Path $sourceDir
New-Item -ItemType Directory -Force -Path $includeDir

# 下载 Source 文件
$sourceFiles = @(
    "bucketalloc.c", "bucketalloc.h",
    "dict.c", "dict.h",
    "geom.c", "geom.h",
    "mesh.c", "mesh.h",
    "priorityq.c", "priorityq.h",
    "sweep.c", "sweep.h",
    "tess.c", "tess.h"
)

foreach ($file in $sourceFiles) {
    $url = "$baseUrl/Source/$file"
    $output = "$sourceDir/$file"
    Write-Host "Downloading $file..."
    Invoke-WebRequest -Uri $url -OutFile $output
}

Write-Host "libtess2 源码下载完成！"
```
