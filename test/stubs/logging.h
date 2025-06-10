#pragma once
#include <string>

namespace Log {
    inline void add(const std::string&) {}
    inline void add(const char*) {}
    inline std::string get() { return ""; }
    inline void clearLogFile() {}
}

#define TRACE_STACK()
