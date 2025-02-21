#pragma once
#include <string>
#include <syslog.h>
#include <memory>

class Logger {
public:
    enum class Level {
        DEBUG = LOG_DEBUG,
        INFO = LOG_INFO,
        NOTICE = LOG_NOTICE,
        WARNING = LOG_WARNING,
        ERROR = LOG_ERR,
        CRITICAL = LOG_CRIT
    };

    static Logger& getInstance() {
        static Logger instance;
        return instance;
    }

    void log(Level level, const std::string& message);
    void logToFile(const std::string& message);
    
    void setLogFile(const std::string& path);
    void enableConsoleOutput(bool enable);

private:
    Logger();
    ~Logger();
    
    std::string m_logFile;
    bool m_consoleOutput;
    
    void initializeSyslog();
    void openLogFile();
};

// Convenience macros for logging
#define LOG_DEBUG(msg) Logger::getInstance().log(Logger::Level::DEBUG, msg)
#define LOG_INFO(msg) Logger::getInstance().log(Logger::Level::INFO, msg)
#define LOG_NOTICE(msg) Logger::getInstance().log(Logger::Level::NOTICE, msg)
#define LOG_WARNING(msg) Logger::getInstance().log(Logger::Level::WARNING, msg)
#define LOG_ERROR(msg) Logger::getInstance().log(Logger::Level::ERROR, msg)
#define LOG_CRITICAL(msg) Logger::getInstance().log(Logger::Level::CRITICAL, msg)