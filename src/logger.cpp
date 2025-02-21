#include "logger.hpp"
#include <fstream>
#include <ctime>
#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

Logger::Logger() : m_consoleOutput(false) {
    initializeSyslog();
    setLogFile("/var/log/jailautomation.log");
}

Logger::~Logger() {
    closelog();
}

void Logger::initializeSyslog() {
    openlog("jailautomation", LOG_PID | LOG_NDELAY, LOG_LOCAL0);
}

void Logger::setLogFile(const std::string& path) {
    m_logFile = path;
    openLogFile();
}

void Logger::openLogFile() {
    // Ensure log file exists with proper permissions
    int fd = open(m_logFile.c_str(), O_WRONLY | O_CREAT | O_APPEND, 0640);
    if (fd != -1) {
        close(fd);
    }
}

void Logger::log(Level level, const std::string& message) {
    // Log to syslog
    syslog(static_cast<int>(level), "%s", message.c_str());
    
    // Log to file
    logToFile(message);
    
    // Console output if enabled
    if (m_consoleOutput) {
        std::cout << message << std::endl;
    }
}

void Logger::logToFile(const std::string& message) {
    std::ofstream logFile(m_logFile, std::ios::app);
    if (logFile) {
        time_t now = time(nullptr);
        char timestamp[64];
        strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", localtime(&now));
        
        logFile << timestamp << " [" << getpid() << "]: " << message << std::endl;
    }
}

void Logger::enableConsoleOutput(bool enable) {
    m_consoleOutput = enable;
}