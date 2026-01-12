#pragma once
#include <string>

namespace tradex {

    class Logger {
    public:
        static void Log(const std::string& message);
    };

} 
