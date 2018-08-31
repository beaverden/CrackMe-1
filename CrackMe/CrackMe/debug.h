#pragma once
#ifndef __DEBUG__
#define __DEBUG__

#include <Windows.h>
//#define DEBUG

#ifdef DEBUG
    void WINAPIV DebugOut(const char *fmt, ...);
    #define DebugOutput(fmt, ...) DebugOut(fmt, __VA_ARGS__)
#else
    #define DebugOutput(fmt, ...)
#endif

#endif // __DEBUG__