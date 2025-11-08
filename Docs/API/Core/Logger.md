# Logger - 日志系统

## 概述

F__K UI 框架的日志系统提供灵活的日志记录功能，支持多种日志级别、自定义日志器实现，以及彩色输出等特性。核心设计遵循策略模式，允许应用程序根据需求选择或实现不同的日志记录后端。

## 核心 API

### 日志级别枚举

```cpp
enum class Level {
    Trace,   // 最详细的跟踪信息
    Debug,   // 调试信息
    Info,    // 一般信息
    Warn,    // 警告信息
    Error,   // 错误信息
    Fatal    // 致命错误
};
```

日志级别从低到高，用于过滤和归类日志消息。

### ILogger 接口

```cpp
class Logger {
public:
    virtual ~Logger() = default;
    virtual void Log(Level level, std::string_view message) = 0;
};
```

基础日志器接口，所有具体的日志实现都需要继承此接口。

### ConsoleLogger 实现

```cpp
class ConsoleLogger : public Logger {
public:
    explicit ConsoleLogger(bool enableColor = true);
    void Log(Level level, std::string_view message) override;
    
private:
    const char* LevelToString(Level level) const;
    const char* LevelToColor(Level level) const;
    bool enableColor_{true};
    std::mutex mutex_;
};
```

向控制台输出日志的实现，支持彩色输出。

**特点：**
- 支持 ANSI 颜色代码
- 线程安全（使用 mutex）
- 可选的禁用彩色输出
- 自动转换日志级别为字符串

**示例：**
```cpp
// 创建彩色日志器
auto logger = std::make_shared<fk::core::ConsoleLogger>(true);
fk::core::LoggerManager::SetLogger(logger);

// 创建无色日志器
auto plainLogger = std::make_shared<fk::core::ConsoleLogger>(false);
fk::core::LoggerManager::SetLogger(plainLogger);
```

### NullLogger 实现

```cpp
class NullLogger : public Logger {
public:
    void Log(Level level, std::string_view message) override;
};
```

空日志器实现，所有日志消息都被忽略。用于生产环境禁用日志输出或测试环境。

**示例：**
```cpp
// 禁用所有日志输出
fk::core::LoggerManager::SetLogger(std::make_shared<fk::core::NullLogger>());
```

### LoggerManager 日志管理器

```cpp
class LoggerManager {
public:
    static void SetLogger(std::shared_ptr<Logger> logger);
    static std::shared_ptr<Logger> GetLogger();
    static void Log(Logger::Level level, std::string_view message);
    
private:
    static std::shared_ptr<Logger>& Instance();
};
```

全局日志管理器，提供一致的日志访问接口。

#### SetLogger - 设置全局日志器

```cpp
static void SetLogger(std::shared_ptr<Logger> logger);
```

设置应用程序使用的日志器实现。

**示例：**
```cpp
#include "fk/core/Logger.h"

void Initialize() {
    // 开发环境：彩色控制台日志
    if (IsDebugBuild()) {
        fk::core::LoggerManager::SetLogger(
            std::make_shared<fk::core::ConsoleLogger>(true)
        );
    }
    // 生产环境：禁用日志
    else {
        fk::core::LoggerManager::SetLogger(
            std::make_shared<fk::core::NullLogger>()
        );
    }
}
```

#### GetLogger - 获取当前日志器

```cpp
static std::shared_ptr<Logger> GetLogger();
```

获取当前配置的日志器实现。

#### Log - 记录日志

```cpp
static void Log(Logger::Level level, std::string_view message);
```

通过全局日志器记录一条日志消息。

**示例：**
```cpp
fk::core::LoggerManager::Log(fk::core::Logger::Level::Info, "应用程序启动");
fk::core::LoggerManager::Log(fk::core::Logger::Level::Error, "加载资源失败");
```

## 常见模式

### 1. 初始化日志系统

```cpp
#include "fk/core/Logger.h"

class Application {
public:
    void Initialize() {
        // 根据构建配置选择日志器
        #ifdef _DEBUG
            auto logger = std::make_shared<fk::core::ConsoleLogger>(true);
        #else
            auto logger = std::make_shared<fk::core::NullLogger>();
        #endif
        
        fk::core::LoggerManager::SetLogger(logger);
    }
};
```

### 2. 自定义日志器实现

创建自己的日志器以满足特定需求（如文件日志、网络日志等）：

```cpp
#include "fk/core/Logger.h"
#include <fstream>
#include <sstream>
#include <chrono>

class FileLogger : public fk::core::Logger {
public:
    explicit FileLogger(const std::string& filename)
        : file_(filename, std::ios::app) {}
    
    void Log(fk::core::Logger::Level level, std::string_view message) override {
        std::lock_guard<std::mutex> lock(mutex_);
        
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        
        file_ << "[" << LevelToString(level) << "] "
              << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S")
              << " " << message << "\n";
        file_.flush();
    }
    
private:
    const char* LevelToString(fk::core::Logger::Level level) {
        switch (level) {
            case fk::core::Logger::Level::Trace: return "TRACE";
            case fk::core::Logger::Level::Debug: return "DEBUG";
            case fk::core::Logger::Level::Info:  return "INFO";
            case fk::core::Logger::Level::Warn:  return "WARN";
            case fk::core::Logger::Level::Error: return "ERROR";
            case fk::core::Logger::Level::Fatal: return "FATAL";
        }
        return "UNKNOWN";
    }
    
    std::ofstream file_;
    std::mutex mutex_;
};

// 使用文件日志器
void SetupFileLogging() {
    auto logger = std::make_shared<FileLogger>("app.log");
    fk::core::LoggerManager::SetLogger(logger);
}
```

### 3. 多路日志输出

创建一个日志器，同时输出到控制台和文件：

```cpp
class MultiLogger : public fk::core::Logger {
public:
    void AddLogger(std::shared_ptr<Logger> logger) {
        loggers_.push_back(logger);
    }
    
    void Log(Level level, std::string_view message) override {
        for (auto& logger : loggers_) {
            logger->Log(level, message);
        }
    }
    
private:
    std::vector<std::shared_ptr<Logger>> loggers_;
};

// 使用
auto multi = std::make_shared<MultiLogger>();
multi->AddLogger(std::make_shared<fk::core::ConsoleLogger>(true));
multi->AddLogger(std::make_shared<FileLogger>("app.log"));
fk::core::LoggerManager::SetLogger(multi);
```

### 4. 条件日志记录

```cpp
#define FK_LOG(level, message) \
    do { \
        auto logger = fk::core::LoggerManager::GetLogger(); \
        if (logger) logger->Log(level, message); \
    } while (0)

#define FK_LOG_INFO(msg) FK_LOG(fk::core::Logger::Level::Info, msg)
#define FK_LOG_ERROR(msg) FK_LOG(fk::core::Logger::Level::Error, msg)
#define FK_LOG_DEBUG(msg) FK_LOG(fk::core::Logger::Level::Debug, msg)

// 使用
FK_LOG_INFO("应用程序初始化完成");
FK_LOG_ERROR("发生了一个错误");
```

### 5. 格式化日志输出

```cpp
#include <sstream>
#include <format>

void LogFormatted(const std::string& format, const std::vector<std::string>& args) {
    std::stringstream ss;
    ss << format;
    // 简单的占位符替换示例
    for (const auto& arg : args) {
        // 实现替换逻辑
    }
    fk::core::LoggerManager::Log(
        fk::core::Logger::Level::Info,
        ss.str()
    );
}

// 使用 C++20 std::format（如果可用）
void LogMessage(const std::string& name, int code) {
    auto message = std::format("错误 [{}]: 代码 {}", name, code);
    fk::core::LoggerManager::Log(fk::core::Logger::Level::Error, message);
}
```

## 日志级别说明

| 级别 | 用途 | 示例 |
|------|------|------|
| **Trace** | 最详细的跟踪信息 | 函数入口/出口、变量值变化 |
| **Debug** | 调试信息 | 内存分配、资源加载细节 |
| **Info** | 一般信息 | 应用启动、配置加载完成 |
| **Warn** | 警告信息 | 废弃 API 使用、性能问题 |
| **Error** | 错误信息 | 操作失败、异常捕获 |
| **Fatal** | 致命错误 | 应用即将崩溃、无法恢复的错误 |

## 线程安全性

- `ConsoleLogger` 和自定义日志器应使用 `std::mutex` 保护共享状态
- `LoggerManager::SetLogger()` 不是线程安全的，应在程序启动时设置
- `LoggerManager::Log()` 委托给具体日志器的 `Log()` 方法

## 最佳实践

### 1. 在应用初始化时配置日志器

```cpp
int main() {
    // 第一步：设置日志系统
    fk::core::LoggerManager::SetLogger(
        std::make_shared<fk::core::ConsoleLogger>()
    );
    
    // 后续代码可以使用日志
    fk::core::LoggerManager::Log(
        fk::core::Logger::Level::Info,
        "应用程序启动"
    );
    
    // ... 其他初始化代码
}
```

### 2. 为不同的构建配置使用不同的日志器

```cpp
#ifdef _DEBUG
    // 开发环境：详细的彩色日志
    const auto logLevel = fk::core::Logger::Level::Debug;
    auto logger = std::make_shared<fk::core::ConsoleLogger>(true);
#else
    // 生产环境：无日志或文件日志
    auto logger = std::make_shared<fk::core::NullLogger>();
#endif

fk::core::LoggerManager::SetLogger(logger);
```

### 3. 为日志消息提供上下文

```cpp
void LoadResource(const std::string& name) {
    try {
        // 加载资源的逻辑
        fk::core::LoggerManager::Log(
            fk::core::Logger::Level::Debug,
            "加载资源: " + name
        );
    } catch (const std::exception& e) {
        fk::core::LoggerManager::Log(
            fk::core::Logger::Level::Error,
            "加载资源失败: " + name + " - " + e.what()
        );
    }
}
```

### 4. 避免过度日志

- 不要在性能关键路径上记录太多日志
- 使用日志级别过滤而不是条件编译
- 考虑使用 NullLogger 在性能敏感的场景

### 5. 在库代码中提供日志钩子

```cpp
class UIElement {
public:
    void ProcessEvent(const Event& event) {
        auto logger = fk::core::LoggerManager::GetLogger();
        if (logger) {
            logger->Log(
                fk::core::Logger::Level::Debug,
                "处理事件: " + event.Type()
            );
        }
        // ... 处理事件
    }
};
```

## 常见问题

### Q: 如何在发布版本中完全禁用日志？
**A:** 使用 `NullLogger`：
```cpp
fk::core::LoggerManager::SetLogger(std::make_shared<fk::core::NullLogger>());
```

### Q: 如何添加日志前缀或标签？
**A:** 创建自定义日志器或在调用时添加前缀：
```cpp
auto msg = "[UIModule] " + std::string(originalMessage);
fk::core::LoggerManager::Log(level, msg);
```

### Q: ConsoleLogger 的彩色输出在 Windows 上工作吗？
**A:** Windows 10+ 支持 ANSI 颜色代码。对于较旧的 Windows 版本，建议禁用彩色输出。

### Q: 我可以在不同的线程中调用 Log 吗？
**A:** 可以，但应确保日志器实现是线程安全的（使用 mutex）。

## 相关文档

- [Clock.md](./Clock.md) - 时钟系统
- [Dispatcher.md](./Dispatcher.md) - 调度器
- [Event.md](./Event.md) - 事件系统
