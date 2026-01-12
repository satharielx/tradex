#pragma once
#include <string>

namespace tradex {

    class WinHttpClient {
    public:
        static std::string Get(const std::string& urlUtf8, unsigned long timeoutMs = 5000);
    };

} 
