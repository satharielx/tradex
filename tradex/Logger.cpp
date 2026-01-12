#include "Logger.h"

#include <iostream>
#include <mutex>
#include <chrono>
#include <iomanip>
#include <ctime>

namespace tradex {

    static std::mutex g_consoleMutex;

    void Logger::Log(const std::string& message) {
        std::lock_guard<std::mutex> lock(g_consoleMutex);

        auto now = std::chrono::system_clock::now();
        std::time_t t = std::chrono::system_clock::to_time_t(now);

        std::tm tm{};
        localtime_s(&tm, &t);

        std::cout << "[" << std::put_time(&tm, "%H:%M:%S") << "] " << message << "\n";
    }

} 
