#include "fk/core/Logger.h"

#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <utility>

namespace fk::core {

namespace {
constexpr const char* kResetColor = "\x1b[0m";

std::string CurrentTimeString() {
    using Clock = std::chrono::system_clock;
    const auto now = Clock::now();
    const auto tt = Clock::to_time_t(now);
    std::tm tmResult{};
#if defined(_WIN32)
    localtime_s(&tmResult, &tt);
#else
    localtime_r(&tt, &tmResult);
#endif
    std::ostringstream oss;
    oss << std::put_time(&tmResult, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}
}

ConsoleLogger::ConsoleLogger(bool enableColor)
    : enableColor_(enableColor) {}

void ConsoleLogger::Log(Level level, std::string_view message) {
    std::lock_guard lock(mutex_);

    const auto timestamp = CurrentTimeString();
    const char* levelStr = LevelToString(level);

    if (enableColor_) {
        std::cout << LevelToColor(level) << '[' << levelStr << "] "
                  << timestamp << " - " << message << kResetColor << std::endl;
    } else {
        std::cout << '[' << levelStr << "] " << timestamp << " - " << message << std::endl;
    }
}

const char* ConsoleLogger::LevelToString(Level level) const {
    switch (level) {
    case Level::Trace: return "TRACE";
    case Level::Debug: return "DEBUG";
    case Level::Info:  return "INFO";
    case Level::Warn:  return "WARN";
    case Level::Error: return "ERROR";
    case Level::Fatal: return "FATAL";
    default: return "UNKNOWN";
    }
}

const char* ConsoleLogger::LevelToColor(Level level) const {
    if (!enableColor_) {
        return "";
    }
    switch (level) {
    case Level::Trace: return "\x1b[37m";
    case Level::Debug: return "\x1b[36m";
    case Level::Info:  return "\x1b[32m";
    case Level::Warn:  return "\x1b[33m";
    case Level::Error: return "\x1b[31m";
    case Level::Fatal: return "\x1b[35m";
    default: return "";
    }
}

void NullLogger::Log(Level, std::string_view) {}

void LoggerManager::SetLogger(std::shared_ptr<Logger> logger) {
    Instance() = std::move(logger);
}

std::shared_ptr<Logger> LoggerManager::GetLogger() {
    return Instance();
}

void LoggerManager::Log(Logger::Level level, std::string_view message) {
    if (auto logger = Instance()) {
        logger->Log(level, message);
    }
}

std::shared_ptr<Logger>& LoggerManager::Instance() {
    static std::shared_ptr<Logger> logger = std::make_shared<ConsoleLogger>();
    return logger;
}

} // namespace fk::core
