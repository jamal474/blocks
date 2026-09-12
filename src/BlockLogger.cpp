#include "BlockLogger.h"
#include <iostream>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <vector>
#include <cstdio>
#include <cstdlib>
#include <cstring>

const std::string RESET_COLOR =     "\033[0m";
const std::string RED_COLOR =       "\033[31m";
const std::string YELLOW_COLOR =    "\033[33m";
const std::string GREEN_COLOR =     "\033[32m";
const std::string CYAN_COLOR =      "\033[36m";

BlockLogger::BlockLogger() {
    if (const char* env = std::getenv("BLOCKS_LOG_LEVEL")) {
        if      (std::strcmp(env, "debug") == 0) minLevel = LogLevel::Debug;
        else if (std::strcmp(env, "info")  == 0) minLevel = LogLevel::Info;
        else if (std::strcmp(env, "warn")  == 0) minLevel = LogLevel::Warn;
        else if (std::strcmp(env, "error") == 0) minLevel = LogLevel::Error;
        else if (std::strcmp(env, "off")   == 0) minLevel = LogLevel::Off;
    }
}

BlockLogger& BlockLogger::getInstance() {
    static BlockLogger instance;
    return instance;
}

std::string BlockLogger::getFileName(const char* filepath) {
    std::string path(filepath);
    size_t lastSeparator = path.find_last_of("/\\");
    if (lastSeparator != std::string::npos) {
        return path.substr(lastSeparator + 1);
    }
    return path;
}

std::string BlockLogger::getCurrentTime() {
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;

    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %H:%M:%S");
    ss << '.' << std::setfill('0') << std::setw(3) << ms.count();
    return ss.str();
}

void BlockLogger::logInternal(LogLevel level, const char* file, int line, const char* format, ...) {
    if (level < minLevel) return;

    std::lock_guard<std::mutex> lock(logMutex);

    int size = 0;
    std::string message;

    va_list args;
    va_start(args, format);
    size = std::vsnprintf(nullptr, 0, format, args);
    va_end(args);

    if (size < 0) {
        message = "Error encoding log message";
    } else {
        std::vector<char> buffer(size + 1);

        va_start(args, format);
        std::vsnprintf(buffer.data(), size + 1, format, args);
        va_end(args);

        message = std::string(buffer.data(), size);
    }

    std::string levelStr;
    std::string color;

    switch (level) {
        case LogLevel::Debug: levelStr = "[DEBUG]"; color = CYAN_COLOR; break;
        case LogLevel::Info:  levelStr = "[INFO] "; color = GREEN_COLOR; break;
        case LogLevel::Warn:  levelStr = "[WARN] "; color = YELLOW_COLOR; break;
        case LogLevel::Error: levelStr = "[ERROR]"; color = RED_COLOR; break;
        default: levelStr = "[UNKNOWN]"; color = RESET_COLOR; break;
    }

    std::cout << color << "[" << getCurrentTime() << "] "
              << levelStr << " "
              << "[" << getFileName(file) << ":" << line << "] "
              << message
              << RESET_COLOR << std::endl;
}
