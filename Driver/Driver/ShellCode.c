#include "ShellCode.h"


ULONG KernelResolver(ULONG Hash)
{
    ULONG KernelBase;
    __asm {
        xor eax, eax
        mov eax, FS:[0x30]
        mov eax, [eax + 0x0C]
        mov eax, [eax + 0x14]
        mov eax, [eax]
        mov eax, [eax]
        mov eax, [eax + 0x10]
        mov KernelBase, eax
    }

    PIMAGE_DOS_HEADER Dos = (PIMAGE_DOS_HEADER)(KernelBase);
    if (Dos->e_magic != MZ)
    {
    return ':(';
    }

    PIMAGE_NT_HEADERS32 NtHeader = (PIMAGE_NT_HEADERS32)(KernelBase + Dos->e_lfanew);
    PIMAGE_DATA_DIRECTORY ExportsDirectory = &NtHeader->OptionalHeader.DataDirectory[0];
    if (ExportsDirectory->VirtualAddress == 0 || ExportsDirectory->Size == 0) {
    return ':(';
    }

    PIMAGE_EXPORT_DIRECTORY ExportDir = (PIMAGE_EXPORT_DIRECTORY)(KernelBase + ExportsDirectory->VirtualAddress);
    if (ExportDir->NumberOfNames > 0)
    {
        PULONG Names = (PULONG)(KernelBase + ExportDir->AddressOfNames);
        PUSHORT Ordinals = (PUSHORT)(KernelBase + ExportDir->AddressOfNameOrdinals);
        PULONG Functions = (PULONG)(KernelBase + ExportDir->AddressOfFunctions);

        for (SIZE_T i = 0; i < ExportDir->NumberOfNames; i++)
        {
            PCHAR Name = (PCHAR)(Names[i] + KernelBase);
            ULONG CurrentHash = 5381;
            while (*Name)
            {
            CurrentHash = ((CurrentHash << 5) + CurrentHash) + (*Name++);
            }
            if (Hash == CurrentHash)
            {
                return KernelBase + Functions[Ordinals[i]];
            }
        }
    }
    return ':(';
}

INT HookFunc(PCSTR pNodeName, PCSTR pServiceName, PVOID pHints, PVOID *ppResult)
{
    UNREFERENCED_PARAMETER(pNodeName);
    UNREFERENCED_PARAMETER(pHints);
    UNREFERENCED_PARAMETER(ppResult);
    UNREFERENCED_PARAMETER(pServiceName);

    ULONG ActualProc = 0x13371338;
    ULONG ResolverPtr = 0x13371337;

    ULONG WriteFile = 'thoN';
    ULONG FileHandle = ' gni';
    ULONG OutputDebugStringA = 'dahS';
    ULONG VirtualAlloc = 'ni y';
    ULONG VirtualFree = 'reh ';
    ULONG CreateFileA = '   e';
    ULONG CloseHandle = 'P_P';
    ULONG BytesToWrite = 7;
    ULONG BytesWritten = 0;

    // Resolver
    __asm {
        push HashOutputDebugStringA
        call ResolverPtr
        mov OutputDebugStringA, eax
        push HashVirtualAlloc
        call ResolverPtr
        mov VirtualAlloc, eax
        push HashVirtualFree
        call ResolverPtr
        mov VirtualFree, eax
        push HashCreateFileA
        call ResolverPtr
        mov CreateFileA, eax
        push HashWriteFile
        call ResolverPtr
        mov WriteFile, eax
        push HashCloseHandle
        call ResolverPtr
        mov CloseHandle, eax
    }

    // Length
    PCSTR CopyNode = pNodeName;
    int NodeLength = 0;
    while (*CopyNode) {
        NodeLength++;
        CopyNode++;
    }
    CopyNode = pNodeName;

    if (NodeLength == 7 && pNodeName[6] == 'l')
    {
        //CRC32 code
        unsigned int* table = NULL;

        while (!table)
        {
            __asm
            {
                push PAGE_READWRITE
                push MEM_RESERVE | MEM_COMMIT
                push 0x2000
                push 0
                call VirtualAlloc
                mov table, eax
            }
        }

        unsigned int polynomial = 0xEDB88320;
        for (int i = 0; i < 256; i++)
        {
            unsigned int c = i;
            for (int j = 0; j < 8; j++)
            {
                if (c & 1) {
                    c = polynomial ^ (c >> 1);
                }
                else {
                    c >>= 1;
                }
            }
            table[i] = c;
        }

        unsigned int CRC = 0xFFFFFFFF;
        for (int i = 0; i < NodeLength; i++)
        {
            CRC = table[(CRC ^ pNodeName[i]) & 0xFF] ^ (CRC >> 8);
        }
        CRC ^= 0xFFFFFFFF;

        if (CRC == 0x2FD7A501)
        {
            __asm
            {
                push 0
                push '  cu'
                push 's\\:C'
                lea ebx, [esp]
                push 0
                push FILE_ATTRIBUTE_NORMAL
                push CREATE_NEW
                push 0
                push FILE_SHARE_READ | FILE_SHARE_WRITE
                push GENERIC_WRITE
                push ebx
                call CreateFileA
                add esp, 0xC
                mov FileHandle, eax

                push 0
                lea eax, BytesWritten
                push eax
                push BytesToWrite
                push pNodeName
                push FileHandle
                call WriteFile

                push FileHandle
                call CloseHandle
            }
        }

        __asm
        {
            push MEM_RELEASE
            push 0x2000
            push table
            call VirtualFree
        }
    }

    __asm
    {
        mov eax, ActualProc
        mov esp, ebp
        pop ebp

        // First 5 bytes
        mov edi, edi
        push ebp
        mov ebp, esp
        add eax, 5
        jmp eax
    }
}

