#pragma once

#include <memory>
#include <mutex>
#include <string_view>

namespace fk::core {

class Logger {
public:
    enum class Level {
        Trace,
        Debug,
        Info,
        Warn,
        Error,
        Fatal
    };

    virtual ~Logger() = default;
    virtual void Log(Level level, std::string_view message) = 0;
};

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

class NullLogger : public Logger {
public:
    void Log(Level level, std::string_view message) override;
};

class LoggerManager {
public:
    static void SetLogger(std::shared_ptr<Logger> logger);
    static std::shared_ptr<Logger> GetLogger();

    static void Log(Logger::Level level, std::string_view message);

private:
    static std::shared_ptr<Logger>& Instance();
};

} // namespace fk::core
