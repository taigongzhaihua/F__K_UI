# Logger

## 概览

**目的**：日志记录接口

**命名空间**：`fk::core`

**头文件**：`fk/core/Logger.h`

## 描述

`Logger` 提供日志记录功能的接口。

## 日志级别

- `Trace` - 详细跟踪信息
- `Debug` - 调试信息
- `Info` - 一般信息
- `Warning` - 警告
- `Error` - 错误
- `Fatal` - 致命错误

## 公共接口

### 日志方法

```cpp
virtual void Log(LogLevel level, const std::string& message) = 0;

void Trace(const std::string& message);
void Debug(const std::string& message);
void Info(const std::string& message);
void Warning(const std::string& message);
void Error(const std::string& message);
void Fatal(const std::string& message);
```

## 使用示例

### 设置日志记录器
```cpp
#include "fk/core/Logger.h"

// 使用控制台日志
LoggerManager::Instance().SetLogger(
    std::make_shared<ConsoleLogger>()
);
```

### 记录日志
```cpp
auto logger = LoggerManager::Instance().GetLogger();

logger->Info("应用程序启动");
logger->Debug("调试信息");
logger->Warning("警告信息");
logger->Error("错误信息");
```

### 自定义日志记录器
```cpp
class FileLogger : public Logger {
public:
    void Log(LogLevel level, const std::string& message) override {
        // 写入文件
        file_ << FormatLog(level, message) << std::endl;
    }
    
private:
    std::ofstream file_;
};
```

## 相关类

- [LoggerManager](#)
- [ConsoleLogger](#)
