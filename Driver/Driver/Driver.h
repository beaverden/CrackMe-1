#pragma once
#ifndef DRIVER_H
#define DRIVER_H

#include <ntifs.h>
#include <ntddk.h>
#include <wdm.h>

//#define DEBUG

#ifdef DEBUG
#define DEBUG_PRINT(fmt, ...) { DbgPrint(fmt, __VA_ARGS__); }
#else
#define DEBUG_PRINT(fmt, ...)
#endif

#define REF(ptr, type) (*(type*)(ptr))
#define HashGetaddrinfo 0x7C84CDCC

typedef struct _DEVICE_EXTENSION
{
    // Callback data
    BOOLEAN IsCallbackActive;
    PMDL SystemWide;
} DEVICE_EXTENSION, *PDEVICE_EXTENSION;


typedef NTSTATUS(*QUERY_INFO_PROCESS) (
    HANDLE ProcessHandle,
    PROCESSINFOCLASS ProcessInformationClass,
    PVOID ProcessInformation,
    ULONG ProcessInformationLength,
    PULONG ReturnLength
    );

typedef enum _KAPC_ENVIRONMENT
{
    OriginalApcEnvironment,
    AttachedApcEnvironment,
    CurrentApcEnvironment,
    InsertApcEnvironment
}KAPC_ENVIRONMENT, *PKAPC_ENVIRONMENT;

typedef VOID(NTAPI *PKNORMAL_ROUTINE)(
    PVOID NormalContext,
    PVOID SystemArgument1,
    PVOID SystemArgument2
);

typedef VOID(NTAPI *PKKERNEL_ROUTINE)(
    PRKAPC Apc,
    PKNORMAL_ROUTINE *NormalRoutine,
    PVOID *NormalContext,
    PVOID *SystemArgument1,
    PVOID *SystemArgument2
);

typedef VOID(NTAPI *PKRUNDOWN_ROUTINE)(
    PRKAPC Apc
);

VOID KeInitializeApc(
    PRKAPC Apc,
    PRKTHREAD Thread,
    KAPC_ENVIRONMENT Environment,
    PKKERNEL_ROUTINE KernelRoutine,
    PKRUNDOWN_ROUTINE RundownRoutine,
    PKNORMAL_ROUTINE NormalRoutine,
    KPROCESSOR_MODE ProcessorMode,
    PVOID NormalContext
);

BOOLEAN KeInsertQueueApc(
    PRKAPC Apc,
    PVOID SystemArgument1,
    PVOID SystemArgument2,
    KPRIORITY Increment
);


NTSTATUS ZwQuerySystemInformation(
    ULONG InfoClass, 
    PVOID Buffer, 
    ULONG Length, 
    PULONG ReturnLength
);

typedef struct _APC_CONTEXT {
    HANDLE  Pid;
    ULONG   ImageBase;
} APC_CONTEXT, *PAPC_CONTEXT;

#endif // DRIVER_H