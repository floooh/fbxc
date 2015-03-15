#pragma once
//------------------------------------------------------------------------------
/**
    @class FBXC::Log
    @brief static logging functions
*/
#include <cstdio>
#include <cassert>

namespace FBXC {

class Log {
public:
    /// print normal info
    static void Info(const char* str, ...) {
        va_list args;
        va_start(args, str);
        std::vprintf(str, args);
        va_end(args);
    };
    /// print warning (to stderr)
    static void Warn(const char* str, ...) {
        va_list args;
        va_start(args, str);
        std::fprintf(stderr, "[warn] ");
        std::vfprintf(stderr, str, args);
        va_end(args);
    };
    /// display an error message and terminate the program
    static void Fatal(const char* str, ...) {
        va_list args;
        va_start(args, str);
        std::fprintf(stderr, "[error] ");
        std::vfprintf(stderr, str, args);
        va_end(args);
        std::fflush(stdout);
        std::fflush(stderr);
        exit(10);
    };
};

}