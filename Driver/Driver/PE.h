#pragma once
#include "ntifs.h"

#define MZ 0x5A4D
#define PE 0x4550
#define MAX_DLL_PARSE 50
#define MAX_IMP_PARSE 200

/*

  _____  ______   _    _ ______          _____  ______ _____   _____ 
 |  __ \|  ____| | |  | |  ____|   /\   |  __ \|  ____|  __ \ / ____|
 | |__) | |__    | |__| | |__     /  \  | |  | | |__  | |__) | (___  
 |  ___/|  __|   |  __  |  __|   / /\ \ | |  | |  __| |  _  / \___ \ 
 | |    | |____  | |  | | |____ / ____ \| |__| | |____| | \ \ ____) |
 |_|    |______| |_|  |_|______/_/    \_\_____/|______|_|  \_\_____/ 
                                                                     
*/
typedef struct _IMAGE_DOS_HEADER {      // DOS .EXE header
    SHORT   e_magic;                     // Magic number
    SHORT   e_cblp;                      // Bytes on last page of file
    SHORT   e_cp;                        // Pages in file
    SHORT   e_crlc;                      // Relocations
    SHORT   e_cparhdr;                   // Size of header in paragraphs
    SHORT   e_minalloc;                  // Minimum extra paragraphs needed
    SHORT   e_maxalloc;                  // Maximum extra paragraphs needed
    SHORT   e_ss;                        // Initial (relative) SS value
    SHORT   e_sp;                        // Initial SP value
    SHORT   e_csum;                      // Checksum
    SHORT   e_ip;                        // Initial IP value
    SHORT   e_cs;                        // Initial (relative) CS value
    SHORT   e_lfarlc;                    // File address of relocation table
    SHORT   e_ovno;                      // Overlay number
    SHORT   e_res[4];                    // Reserved words
    SHORT   e_oemid;                     // OEM identifier (for e_oeminfo)
    SHORT   e_oeminfo;                   // OEM information; e_oemid specific
    SHORT   e_res2[10];                  // Reserved words
    LONG   e_lfanew;                    // File address of new exe header
  } IMAGE_DOS_HEADER, *PIMAGE_DOS_HEADER;

typedef struct _IMAGE_FILE_HEADER {
    SHORT    Machine;
    SHORT    NumberOfSections;
    INT   TimeDateStamp;
    INT   PointerToSymbolTable;
    INT   NumberOfSymbols;
    SHORT    SizeOfOptionalHeader;
    SHORT    Characteristics;
} IMAGE_FILE_HEADER, *PIMAGE_FILE_HEADER;

typedef struct _IMAGE_DATA_DIRECTORY {
    INT   VirtualAddress;
    INT   Size;
} IMAGE_DATA_DIRECTORY, *PIMAGE_DATA_DIRECTORY;

#define IMAGE_NUMBEROF_DIRECTORY_ENTRIES    16

//
// Optional header format.
//

typedef struct _IMAGE_OPTIONAL_HEADER {
    //
    // Standard fields.
    //

    SHORT    Magic;
    CHAR    MajorLinkerVersion;
    CHAR    MinorLinkerVersion;
    INT   SizeOfCode;
    INT   SizeOfInitializedData;
    INT   SizeOfUninitializedData;
    INT   AddressOfEntryPoint;
    INT   BaseOfCode;
    INT   BaseOfData;

    //
    // NT additional fields.
    //

    INT   ImageBase;
    INT   SectionAlignment;
    INT   FileAlignment;
    SHORT    MajorOperatingSystemVersion;
    SHORT    MinorOperatingSystemVersion;
    SHORT    MajorImageVersion;
    SHORT    MinorImageVersion;
    SHORT    MajorSubsystemVersion;
    SHORT    MinorSubsystemVersion;
    INT   Win32VersionValue;
    INT   SizeOfImage;
    INT   SizeOfHeaders;
    INT   CheckSum;
    SHORT    Subsystem;
    SHORT    DllCharacteristics;
    INT   SizeOfStackReserve;
    INT   SizeOfStackCommit;
    INT   SizeOfHeapReserve;
    INT   SizeOfHeapCommit;
    INT   LoaderFlags;
    INT   NumberOfRvaAndSizes;
    IMAGE_DATA_DIRECTORY DataDirectory[IMAGE_NUMBEROF_DIRECTORY_ENTRIES];
} IMAGE_OPTIONAL_HEADER32, *PIMAGE_OPTIONAL_HEADER32;


// Directory Entries
#define IMAGE_DIRECTORY_ENTRY_EXPORT          0   // Export Directory
#define IMAGE_DIRECTORY_ENTRY_IMPORT          1   // Import Directory
#define IMAGE_DIRECTORY_ENTRY_RESOURCE        2   // Resource Directory
#define IMAGE_DIRECTORY_ENTRY_EXCEPTION       3   // Exception Directory
#define IMAGE_DIRECTORY_ENTRY_SECURITY        4   // Security Directory
#define IMAGE_DIRECTORY_ENTRY_BASERELOC       5   // Base Relocation Table
#define IMAGE_DIRECTORY_ENTRY_DEBUG           6   // Debug Directory
//      IMAGE_DIRECTORY_ENTRY_COPYRIGHT       7   // (X86 usage)
#define IMAGE_DIRECTORY_ENTRY_ARCHITECTURE    7   // Architecture Specific Data
#define IMAGE_DIRECTORY_ENTRY_GLOBALPTR       8   // RVA of GP
#define IMAGE_DIRECTORY_ENTRY_TLS             9   // TLS Directory
#define IMAGE_DIRECTORY_ENTRY_LOAD_CONFIG    10   // Load Configuration Directory
#define IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT   11   // Bound Import Directory in headers
#define IMAGE_DIRECTORY_ENTRY_IAT            12   // Import Address Table
#define IMAGE_DIRECTORY_ENTRY_DELAY_IMPORT   13   // Delay Load Import Descriptors
#define IMAGE_DIRECTORY_ENTRY_COM_DESCRIPTOR 14   // COM Runtime descriptor

typedef IMAGE_OPTIONAL_HEADER32             IMAGE_OPTIONAL_HEADER;
typedef PIMAGE_OPTIONAL_HEADER32            PIMAGE_OPTIONAL_HEADER;
#define IMAGE_NT_OPTIONAL_HDR32_MAGIC      0x10b


typedef struct _IMAGE_NT_HEADERS {
    INT Signature;
    IMAGE_FILE_HEADER FileHeader;
    IMAGE_OPTIONAL_HEADER32 OptionalHeader;
} IMAGE_NT_HEADERS32, *PIMAGE_NT_HEADERS32;

typedef IMAGE_NT_HEADERS32                  IMAGE_NT_HEADERS;
typedef PIMAGE_NT_HEADERS32                 PIMAGE_NT_HEADERS;


/*


________   _______   ____  _____ _______ _____
|  ____\ \ / /  __ \ / __ \|  __ \__   __/ ____|
| |__   \ V /| |__) | |  | | |__) | | | | (___
|  __|   > < |  ___/| |  | |  _  /  | |  \___ \
| |____ / . \| |    | |__| | | \ \  | |  ____) |
|______/_/ \_\_|     \____/|_|  \_\ |_| |_____/


*/
typedef struct _IMAGE_EXPORT_DIRECTORY {
    ULONG   Characteristics;
    ULONG   TimeDateStamp;
    SHORT   MajorVersion;
    SHORT   MinorVersion;
    ULONG   Name;
    ULONG   Base;
    ULONG   NumberOfFunctions;
    ULONG   NumberOfNames;
    ULONG   AddressOfFunctions;     // RVA from base of image
    ULONG   AddressOfNames;         // RVA from base of image
    ULONG   AddressOfNameOrdinals;  // RVA from base of image
} IMAGE_EXPORT_DIRECTORY, *PIMAGE_EXPORT_DIRECTORY;


/*

  _____ __  __ _____   ____  _____ _______ _____ 
 |_   _|  \/  |  __ \ / __ \|  __ \__   __/ ____|
   | | | \  / | |__) | |  | | |__) | | | | (___  
   | | | |\/| |  ___/| |  | |  _  /  | |  \___ \ 
  _| |_| |  | | |    | |__| | | \ \  | |  ____) |
 |_____|_|  |_|_|     \____/|_|  \_\ |_| |_____/ 
                                                 
                                    
*/
typedef struct _IMAGE_THUNK_DATA {
    union {
        ULONG ForwarderString;      // PBYTE 
        ULONG Function;             // PULONG
        ULONG Ordinal;
        ULONG AddressOfData;        // PIMAGE_IMPORT_BY_NAME
    } u1;
} IMAGE_THUNK_DATA;
typedef IMAGE_THUNK_DATA * PIMAGE_THUNK_DATA;
#define IMAGE_ORDINAL_FLAG32 0x80000000

typedef struct _IMAGE_IMPORT_DESCRIPTOR {
    union {
        ULONG   Characteristics;            // 0 for terminating null import descriptor
        ULONG   OriginalFirstThunk;         // RVA to original unbound IAT (PIMAGE_THUNK_DATA)
    } d DUMMYUNIONNAME;
    ULONG   TimeDateStamp;                  // 0 if not bound,
                                            // -1 if bound, and real date\time stamp
                                            //     in IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT (new BIND)
                                            // O.W. date/time stamp of DLL bound to (Old BIND)

    ULONG   ForwarderChain;                 // -1 if no forwarders
    ULONG   Name;
    ULONG   FirstThunk;                     // RVA to IAT (if bound this IAT has actual addresses)
} IMAGE_IMPORT_DESCRIPTOR;
typedef IMAGE_IMPORT_DESCRIPTOR UNALIGNED *PIMAGE_IMPORT_DESCRIPTOR;


#define IMAGE_DIRECTORY_ENTRY_DELAY_IMPORT   13   // Delay Load Import Descriptors
typedef struct _IMAGE_DELAYLOAD_DESCRIPTOR {
    union {
        ULONG AllAttributes;
        ULONG RvaBased;
    } Attributes;

    ULONG DllNameRVA;                       // RVA to the name of the target library (NULL-terminate ASCII string)
    ULONG ModuleHandleRVA;                  // RVA to the HMODULE caching location (PHMODULE)
    ULONG ImportAddressTableRVA;            // RVA to the start of the IAT (PIMAGE_THUNK_DATA)
    ULONG ImportNameTableRVA;               // RVA to the start of the name table (PIMAGE_THUNK_DATA::AddressOfData)
    ULONG BoundImportAddressTableRVA;       // RVA to an optional bound IAT
    ULONG UnloadInformationTableRVA;        // RVA to an optional unload info table
    ULONG TimeDateStamp;                    // 0 if not bound,
                                            // Otherwise, date/time of the target DLL

} IMAGE_DELAYLOAD_DESCRIPTOR, *PIMAGE_DELAYLOAD_DESCRIPTOR;


/*

   _____ ______ _____ _______ _____ ____  _   _ 
  / ____|  ____/ ____|__   __|_   _/ __ \| \ | |
 | (___ | |__ | |       | |    | || |  | |  \| |
  \___ \|  __|| |       | |    | || |  | | . ` |
  ____) | |___| |____   | |   _| || |__| | |\  |
 |_____/|______\_____|  |_|  |_____\____/|_| \_|
                                                
                                                
*/

#define IMAGE_SIZEOF_SHORT_NAME              8
typedef struct _IMAGE_SECTION_HEADER {
    CHAR    Name[IMAGE_SIZEOF_SHORT_NAME];
    union {
        ULONG   PhysicalAddress;
        ULONG   VirtualSize;
    } Misc;
    ULONG   VirtualAddress;
    ULONG   SizeOfRawData;
    ULONG   PointerToRawData;
    ULONG   PointerToRelocations;
    ULONG   PointerToLinenumbers;
    SHORT   NumberOfRelocations;
    SHORT   NumberOfLinenumbers;
    ULONG   Characteristics;
} IMAGE_SECTION_HEADER, *PIMAGE_SECTION_HEADER;
#define IMAGE_SIZEOF_SECTION_HEADER          40