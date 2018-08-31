#pragma once
#include <Windows.h>

#pragma pack(push, 1)
typedef struct _HEADER {
    WORD    wType;                          /* Magic identifier            */
    DWORD   dwSize;                         /* File size in bytes          */
    DWORD   dwReserverd;
    DWORD   dwOffset;                       /* Offset to image data, bytes */
} HEADER, *PHEADER;
#pragma pack(pop)