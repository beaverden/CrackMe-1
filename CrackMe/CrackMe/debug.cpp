#include "debug.h"

void WINAPIV DebugOut(const char *fmt, ...) {
    char s[1025];
    va_list args;
    ZeroMemory(s, 1025 * sizeof(s[0]));
    va_start(args, fmt);
    wvsprintf(s, fmt, args);
    va_end(args);
    s[1024] = 0;
    OutputDebugString(s);
}