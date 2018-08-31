#include <Windows.h>
#include "crackme.h"
#include "driver.h"
#include "payload.h"
#include "resource.h"
#include "bitmap.h"
#include "rc4.h"

const BYTE decode[] = { 0x03,0xF3,0x0D,0x0A,0x5F,0x87,0x88,0x5B,0x63,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x05,0x00,0x00,0x00,0x40,0x00,0x00,0x00,0x73,0xAD,0x00,0x00,0x00,0x64,0x00,0x00,0x84,0x00,0x00,0x5A,0x00,0x00,0x65,0x01,0x00,0x65,0x00,0x00,0x64,0x01,0x00,0x83,0x01,0x00,0x83,0x01,0x00,0x5A,0x02,0x00,0x65,0x01,0x00,0x65,0x00,0x00,0x64,0x02,0x00,0x83,0x01,0x00,0x83,0x01,0x00,0x5A,0x03,0x00,0x65,0x04,0x00,0x65,0x04,0x00,0x65,0x03,0x00,0x65,0x00,0x00,0x64,0x03,0x00,0x83,0x01,0x00,0x83,0x02,0x00,0x65,0x00,0x00,0x64,0x04,0x00,0x83,0x01,0x00,0x83,0x02,0x00,0x5A,0x05,0x00,0x65,0x04,0x00,0x65,0x04,0x00,0x65,0x03,0x00,0x65,0x00,0x00,0x64,0x05,0x00,0x83,0x01,0x00,0x83,0x02,0x00,0x65,0x00,0x00,0x64,0x06,0x00,0x83,0x01,0x00,0x83,0x02,0x00,0x83,0x00,0x00,0x5A,0x06,0x00,0x65,0x05,0x00,0x65,0x04,0x00,0x65,0x02,0x00,0x65,0x00,0x00,0x64,0x07,0x00,0x83,0x01,0x00,0x83,0x02,0x00,0x65,0x06,0x00,0x64,0x08,0x00,0x1F,0x83,0x01,0x00,0x65,0x07,0x00,0x65,0x06,0x00,0x64,0x08,0x00,0x20,0x83,0x01,0x00,0x20,0x83,0x01,0x00,0x01,0x64,0x09,0x00,0x53,0x28,0x0A,0x00,0x00,0x00,0x63,0x01,0x00,0x00,0x00,0x02,0x00,0x00,0x00,0x06,0x00,0x00,0x00,0x43,0x00,0x00,0x00,0x73,0x30,0x00,0x00,0x00,0x64,0x01,0x00,0x6A,0x00,0x00,0x67,0x00,0x00,0x7C,0x00,0x00,0x44,0x5D,0x1C,0x00,0x7D,0x01,0x00,0x74,0x01,0x00,0x74,0x02,0x00,0x7C,0x01,0x00,0x83,0x01,0x00,0x64,0x02,0x00,0x41,0x83,0x01,0x00,0x5E,0x02,0x00,0x71,0x0D,0x00,0x83,0x01,0x00,0x53,0x28,0x03,0x00,0x00,0x00,0x4E,0x74,0x00,0x00,0x00,0x00,0x69,0x50,0x00,0x00,0x00,0x28,0x03,0x00,0x00,0x00,0x74,0x04,0x00,0x00,0x00,0x6A,0x6F,0x69,0x6E,0x74,0x03,0x00,0x00,0x00,0x63,0x68,0x72,0x74,0x03,0x00,0x00,0x00,0x6F,0x72,0x64,0x28,0x02,0x00,0x00,0x00,0x74,0x01,0x00,0x00,0x00,0x73,0x74,0x01,0x00,0x00,0x00,0x78,0x28,0x00,0x00,0x00,0x00,0x28,0x00,0x00,0x00,0x00,0x73,0x06,0x00,0x00,0x00,0x73,0x75,0x63,0x2E,0x70,0x79,0x74,0x02,0x00,0x00,0x00,0x63,0x63,0x01,0x00,0x00,0x00,0x73,0x02,0x00,0x00,0x00,0x00,0x01,0x73,0x03,0x00,0x00,0x00,0x32,0x2A,0x62,0x73,0x03,0x00,0x00,0x00,0x23,0x29,0x23,0x73,0x06,0x00,0x00,0x00,0x23,0x24,0x34,0x3F,0x25,0x24,0x73,0x05,0x00,0x00,0x00,0x27,0x22,0x39,0x24,0x35,0x73,0x05,0x00,0x00,0x00,0x23,0x24,0x34,0x39,0x3E,0x73,0x04,0x00,0x00,0x00,0x22,0x35,0x31,0x34,0x73,0x0A,0x00,0x00,0x00,0x34,0x35,0x33,0x3F,0x3D,0x20,0x22,0x35,0x23,0x23,0x69,0x06,0x00,0x00,0x00,0x4E,0x28,0x08,0x00,0x00,0x00,0x52,0x06,0x00,0x00,0x00,0x74,0x0A,0x00,0x00,0x00,0x5F,0x5F,0x69,0x6D,0x70,0x6F,0x72,0x74,0x5F,0x5F,0x74,0x04,0x00,0x00,0x00,0x66,0x75,0x63,0x6B,0x74,0x04,0x00,0x00,0x00,0x79,0x6F,0x75,0x72,0x74,0x07,0x00,0x00,0x00,0x67,0x65,0x74,0x61,0x74,0x74,0x72,0x74,0x07,0x00,0x00,0x00,0x73,0x74,0x72,0x69,0x6E,0x67,0x73,0x74,0x04,0x00,0x00,0x00,0x64,0x75,0x64,0x65,0x74,0x03,0x00,0x00,0x00,0x69,0x6E,0x74,0x28,0x00,0x00,0x00,0x00,0x28,0x00,0x00,0x00,0x00,0x28,0x00,0x00,0x00,0x00,0x73,0x06,0x00,0x00,0x00,0x73,0x75,0x63,0x2E,0x70,0x79,0x74,0x08,0x00,0x00,0x00,0x3C,0x6D,0x6F,0x64,0x75,0x6C,0x65,0x3E,0x01,0x00,0x00,0x00,0x73,0x0A,0x00,0x00,0x00,0x09,0x03,0x12,0x01,0x12,0x01,0x24,0x01,0x27,0x01 };

BYTE* imagePointer;
DWORD imageSize;
DWORD jmpBack;

void DecryptImage(BYTE* key, SIZE_T keyLength)
{
    PHEADER pHeader = (PHEADER)(imagePointer);
    PBITMAPINFOHEADER pInfoHeader = (PBITMAPINFOHEADER)(imagePointer + sizeof(HEADER));
    DWORD old = 0;
    if (!VirtualProtect(imagePointer, imageSize, PAGE_READWRITE, &old))
    {
        DebugOutput("Failed to make image writeable (%d)", GetLastError());
    }

    BYTE* pImageData = imagePointer + pHeader->dwOffset;
    DWORD off = 0;
    RC4(key, keyLength, pImageData, pInfoHeader->biWidth * pInfoHeader->biHeight);
    DebugOutput("Done decrypting");
}

__forceinline VOID Leave()
{
    __asm
    {
        mov eax, jmpBack
        jmp eax
    }
}


void DecodePayload()
{ 
    CHAR fileName[256];
    memset(fileName, 0, sizeof(fileName));
    HMODULE hModule = GetModuleHandle(NULL);
    GetModuleFileName(hModule, fileName, 1024);
    int len = strlen(fileName);
    while (len > 0 && fileName[--len] != '\\');
    fileName[len] = 0;
    strcat(fileName, "\\snake");
    DebugOutput("Path = %s", fileName);

    HANDLE hFile;
    if ((hFile = CreateFile(fileName, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL)), hFile == INVALID_HANDLE_VALUE)
    {
        DebugOutput("%X", hFile);
        DebugOutput("Failed to open payload to write (%d)", GetLastError());
        Leave();
    }
    
    DWORD bytesWritten = 0;
    if (!WriteFile(hFile, decode, sizeof(decode), &bytesWritten, NULL) || bytesWritten != sizeof(decode))
    {
        DebugOutput("Failed to write to payload");
        Leave();
    }
    CloseHandle(hFile);

    HANDLE hOutPipeR;
    HANDLE hOutPipeW;
    HANDLE hInPipeR;
    HANDLE hInPipeW;
    SECURITY_ATTRIBUTES sa = { 0 };
    sa.nLength = sizeof(sa);
    sa.bInheritHandle = TRUE;
    sa.lpSecurityDescriptor = NULL;
    if (!CreatePipe(&hOutPipeR, &hOutPipeW, &sa, DECOMPRESSED_SIZE))
    {
        DebugOutput("Unable to create pipes out");
        Leave();
    }
    SetHandleInformation(hOutPipeR, HANDLE_FLAG_INHERIT, 0);
    if (!CreatePipe(&hInPipeR, &hInPipeW, &sa, DECOMPRESSED_SIZE))
    {
        DebugOutput("Unable to create pipe in");
        Leave();
    }
    SetHandleInformation(hInPipeW, HANDLE_FLAG_INHERIT, 0);
    
    STARTUPINFOA startupInfo;
    memset(&startupInfo, 0, sizeof(startupInfo));
    startupInfo.cb = sizeof(startupInfo);
    startupInfo.dwFlags |= STARTF_USESTDHANDLES;
    startupInfo.hStdOutput = hOutPipeW;
    startupInfo.hStdInput = hInPipeR;
    startupInfo.hStdError = hOutPipeW;

    PROCESS_INFORMATION processInfo;
    if (CreateProcess(
        "C:\\Python27\\python.exe",
        (LPSTR)" -u snake",
        NULL,
        NULL,
        TRUE,
        NORMAL_PRIORITY_CLASS,
        NULL,
        NULL,
        &startupInfo,
        &processInfo) == FALSE
    )
    {
        DebugOutput("Unable to create process! (%d)", GetLastError());
        Leave();
    }

    CloseHandle(processInfo.hProcess);
    CloseHandle(processInfo.hThread);
    
    if (!WriteFile(hInPipeW, payload, sizeof(payload), &bytesWritten, NULL) || bytesWritten != sizeof(payload))
    {
        DebugOutput("Unable to write payload to process");
        Leave();
    }
    CloseHandle(hInPipeW);

    CloseHandle(hOutPipeW);
    CloseHandle(hInPipeR);
   
    BYTE* decompressedPayload = (BYTE*)VirtualAlloc(NULL, DECOMPRESSED_SIZE, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (!ReadFile(hOutPipeR, decompressedPayload, DECOMPRESSED_SIZE, &bytesWritten, NULL))
    {
        DebugOutput("Readfile failed on pipe (%d)", GetLastError());
        Leave();
    }
    CloseHandle(hOutPipeR);
    CloseHandle(hInPipeW);
    DeleteFile(fileName);

    DebugOutput("Got decompressed payload! %02X %02X", decompressedPayload[0], decompressedPayload[1]);
    memset(fileName, 0, sizeof(fileName));
    GetTempPath(sizeof(fileName), fileName);
    strcat(fileName, "\\sunca");

    HANDLE hSys = CreateFile(fileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hSys == INVALID_HANDLE_VALUE)
    {
        DebugOutput("Create file failed for driver (%d)", GetLastError());
        Leave();
    }
    
    DWORD dwBytesToWrite = 
        DIGIT(payload[0], 100000) 
        + DIGIT(payload[1], 10000) 
        + DIGIT(payload[2], 1000) 
        + DIGIT(payload[3], 100) 
        + DIGIT(payload[4], 10) 
        + DIGIT(payload[5], 1);
    DWORD dwBytes = 0;
    DebugOutput("Actual size: %d", dwBytesToWrite);

    if (!WriteFile(hSys, decompressedPayload, dwBytesToWrite, &dwBytes, NULL))
    {
        DebugOutput("Can't write to sys file (%d)", GetLastError());
        Leave();
    }

    CloseHandle(hSys);

    memset(decompressedPayload, 0, DECOMPRESSED_SIZE);
    VirtualFree(decompressedPayload, DECOMPRESSED_SIZE, MEM_FREE);
   
    UninstallService();
    if (!InstallService(fileName))
    {
        Leave();
    }

    Sleep(30000);

    HANDLE hSuc = CreateFile("C:\\suc", GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    DWORD dwFileSize = GetFileSize(hSuc, NULL);
    if (hSuc == INVALID_HANDLE_VALUE || dwFileSize == 0)
    {
        DebugOutput("No suc (%d)", GetLastError());
        Leave();
    }

    BYTE* sucContent = (BYTE*)VirtualAlloc(NULL, dwFileSize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    if (!ReadFile(hSuc, sucContent, dwFileSize, &dwBytes, NULL) || dwBytes != dwFileSize)
    {
        DebugOutput("Error reading suc (%d)", GetLastError());
    }
    CloseHandle(hSuc);
    DecryptImage(sucContent, dwFileSize);
    UninstallService();

    Leave();
}


void Decrypt()
{
    //DebugOutput("Decode?");
    
    __asm {
    mov eax, 0x13371337
    call $+5
    pop eax
    and eax, 0xFFFF0000
    add eax, 0x1000
    dec eax
    _loop:
        inc eax
        mov ebx, dword ptr [eax]
        cmp ebx, 0x13371337
        je _endloop

        mov bl, byte ptr [eax]
        rol bl, 5
        mov byte ptr [eax], bl
        jne _loop     
    _endloop:
    }

    DecodePayload();
    ExitProcess(0);
}


DWORD called = FALSE;
void ExceptionHandler()
{
    const char* msg = "Unhandled exception occurred";
    if (called) return;
    called = TRUE;
    __asm
    {
        call $ + 5;
        pop eax
        add eax, 0x100
        _loop:
        add eax, 1
        mov ebx, dword ptr [eax]
        cmp ebx, ':un:'
        jnz _loop
        add eax, 4
        mov jmpBack, eax
    }
    OutputDebugString(msg);
    __asm
    {
        push Decrypt
        ret
        push 0
        call ExitProcess
    }
}


INT CALLBACK main(INT argc, CHAR* argv[])
{ 
    __asm
    {
        push offset ExceptionHandler
        push fs:[0x0]
        mov fs:[0x0], esp
    }

    HRSRC hSrc;
    HGLOBAL hResHandle;
    DWORD dwResourceId = IDR_DEFAULT1;
    DWORD dwSize = 0;
    BYTE* resPtr = NULL;
    
    hSrc = FindResource(
        NULL,
        MAKEINTRESOURCE(dwResourceId),
        MAKEINTRESOURCE(RESOURCE_TYPE)
    );
    if (hSrc == NULL)
    {
        DebugOutput("Unable to find resource of id: %d", dwResourceId);
        return NULL;
    }
    if ((dwSize = SizeofResource(NULL, hSrc)) == 0)
    {
        DebugOutput("Invalid size of resource id: %d", dwResourceId);
        return NULL;
    }
    
    if ((hResHandle = LoadResource(NULL, hSrc)) == NULL)
    {
        DebugOutput("Unable to obtain resource handle of resource id: %d", dwResourceId);
        return NULL;
    }
    if ((resPtr = (BYTE*)LockResource(hResHandle)) == NULL)
    {
        DebugOutput("Unable to obtain lock of resource id: %d", dwResourceId);
        return NULL;
    }
    
    imagePointer = resPtr;
    imageSize = dwSize;
    DebugOutput("Image: %08X, Size: %08X", imagePointer, imageSize);
    *(int*)(0) = ':un:';

    DebugOutput("Dropping bitmap!");
    HANDLE hImage = CreateFile("seful labani.bmp", GENERIC_WRITE, NULL, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    DWORD dwBytes = 0;
    if (hImage == INVALID_HANDLE_VALUE)
    {
        DebugOutput("Unable to open file (%d)", GetLastError());
        return 0;
    }
    if (!WriteFile(hImage, imagePointer, imageSize, &dwBytes, NULL) || dwBytes != imageSize)
    {
        DebugOutput("Unable to write image to disk (%d)", GetLastError());
        return 0;
    }

    __asm 
    {
        push 'N'
        push 'O'
        push 'T'
        push 'H'
        push 'I'
        push 'N'
        push 'G'
        push ' '
        push 'S'
        push 'H'
        push 'A'
        push 'D'
        push 'Y'
        push ' '
        push 'I'
        push 'N'
        push ' '
        push 'H'
        push 'E'
        push 'R'
        push 'E'
    }
    CloseHandle(hImage);
    ExitProcess(0);
    return 0;
}
