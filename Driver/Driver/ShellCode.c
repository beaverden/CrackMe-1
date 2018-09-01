#include "ShellCode.h"


ULONG KernelResolver(ULONG Hash)
{
    UNREFERENCED_PARAMETER(Hash);
    ULONG Val = ':(';
    __asm {
        xor eax, eax
        mov eax, FS:[0x30]
        mov eax, [eax + 0x0C]
        mov eax, [eax + 0x14]
        mov eax, [eax]
        mov eax, [eax]
        mov eax, [eax + 0x10]
        mov edi, eax
        add eax, [eax + 0x3C] // PE
        mov eax, [eax + 0x78] // Exports
        cmp eax, 0
        add eax, edi
        je _end
        mov ecx, 0
        mov ebx, [eax + 0x20] // Address of names
        add ebx, edi
        _loop_names :
        cmp ecx, [eax + 0x18] // Number of names
        je _end
        mov esi, [ebx]
        add esi, edi
        mov edx, 5381
        push ecx
        push edi
        _loop_name :
        cmp byte ptr[esi], 0
        je _end_name
        mov edi, edx
        shl edi, 5
        add edi, edx
        movzx ecx, byte ptr[esi]
        add edi, ecx
        mov edx, edi
        inc esi
        jmp _loop_name
        _end_name :
        pop edi
        pop ecx
        cmp edx, [ebp+8]
        jne _not_eq
        mov esi, [eax + 0x24] // Ordinals
        add esi, edi
        movzx esi, word ptr[esi + 2 * ecx]
        mov edx, [eax + 0x1C] // Functions
        add edx, edi
        mov eax, [edx + 4 * esi]
        add eax, edi
        mov Val, eax
        jmp _end
        _not_eq :
        add ebx, 4
        inc ecx
        jmp _loop_names
        _end :
        retn 4
    }
    return Val;
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
                push ' cus'
                push '\\72n'
                push 'ohty'
                push 'P\\:C'
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

