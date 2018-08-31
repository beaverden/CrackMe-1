#pragma once
#include "PE.h"
#pragma warning( disable : 4731 )
#pragma warning( disable : 4740 )

#define HashCreateFileA         0xEB96C5FA
#define HashWriteFile           0x663CECB0
#define HashCloseHandle         0x3870CA07
#define HashOutputDebugStringA  0x79729F95
#define HashVirtualAlloc        0x382C0F97
#define HashVirtualFree         0x668FCF2E
#define CREATE_NEW              1

ULONG KernelResolver(ULONG Hash);
INT HookFunc(PCSTR pNodeName, PCSTR pServiceName, PVOID pHints, PVOID *ppResult);