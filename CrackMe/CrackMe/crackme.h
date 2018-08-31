#pragma once
#include "debug.h"

#define DECOMPRESSED_SIZE 20000
#define DIGIT(x,t) (x-'0')*t  

#pragma function(memset)
void * __cdecl memset(void *pTarget, int value, size_t cbTarget) {
    char *p = reinterpret_cast<char *>(pTarget);
    while (cbTarget-- > 0) {
        *p++ = static_cast<char>(value);
    }
    return pTarget;
}

#pragma function(strlen)
size_t __cdecl strlen(const char* str)
{
    if (!str) return 0;
    size_t len = 0;
    while (str[len++] != 0);
    return len;
}

