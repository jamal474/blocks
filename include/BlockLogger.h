#pragma once
#include <string>
#include <mutex>
#include <cstdarg>

enum class LogLevel {
    Debug,
    Info,
    Warn,
    Error,
    Off
};

class BlockLogger {
public:
    static BlockLogger& getInstance();

    void logInternal(LogLevel level, const char* file, int line, const char* format, ...);

    /// Messages below this level are dropped before any formatting happens.
    /// Defaults to Info, or the level named by the BLOCKS_LOG_LEVEL environment
    /// variable (debug|info|warn|error|off).
    void     setMinLevel(LogLevel level) { minLevel = level; }
    LogLevel getMinLevel() const         { return minLevel; }
    bool     enabled(LogLevel level) const { return level >= minLevel; }

private:
    BlockLogger();
    ~BlockLogger() = default;
    BlockLogger(const BlockLogger&) = delete;
    BlockLogger& operator=(const BlockLogger&) = delete;

    std::string getFileName(const char* filepath);
    std::string getCurrentTime();

    std::mutex logMutex;
    LogLevel   minLevel = LogLevel::Info;
};

// The level test happens before the arguments are evaluated, so a disabled log
// line costs one comparison - safe to leave calls in hot paths.
#define LOG_AT(lvl, fmt, ...)                                                  \
    do {                                                                       \
        if (BlockLogger::getInstance().enabled(lvl))                           \
            BlockLogger::getInstance().logInternal(lvl, __FILE__, __LINE__,    \
                                                   fmt, ##__VA_ARGS__);        \
    } while (0)

#define LOG_DEBUG(fmt, ...) LOG_AT(LogLevel::Debug, fmt, ##__VA_ARGS__)
#define LOG_INFO(fmt, ...)  LOG_AT(LogLevel::Info,  fmt, ##__VA_ARGS__)
#define LOG_WARN(fmt, ...)  LOG_AT(LogLevel::Warn,  fmt, ##__VA_ARGS__)
#define LOG_ERROR(fmt, ...) LOG_AT(LogLevel::Error, fmt, ##__VA_ARGS__)
