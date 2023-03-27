#pragma once

#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <Library/PrintLib.h>
#include <Library/ShellLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DevicePathLib.h>
#include <Library/ShellCEntryLib.h>
#include <Library/ShellCommandLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Protocol/ServiceBinding.h>

// #include <Include/Protocol/DiskIo.h>
// #include <Include/Protocol/DiskInfo.h>
// #include <Include/Protocol/DriverBinding.h>
// #include <Include/Protocol/ComponentName2.h>

// #include <IndustryStandard/Atapi.h>
// #include <IndustryStandard/Scsi.h>
#include <IndustryStandard/Nvme.h>

#include <Protocol/BlockIo.h>
// #include <Protocol/DevicePath.h>
// #include <Protocol/EraseBlock.h>
// #include <Protocol/IdeControllerInit.h>
#include <Protocol/NvmExpressPassthru.h>

#include <Protocol/Hash2.h>

// Accourding to NVM Express Documentation
#define NVME_PRODUCT_MODEL_NUMBER_SIZE 40
#define NVME_PRODUCT_SERIAL_NUMBER_SIZE 20
#define NVME_DESCRIPTION_SIZE (NVME_PRODUCT_MODEL_NUMBER_SIZE+NVME_PRODUCT_SERIAL_NUMBER_SIZE+2)


extern UINT8 Verbose_Level;

// UefiBootManagerLib | MdeModulePkg/Library/UefiBootManagerLib/BmBootDescription.c
CHAR16 *
BmGetNvmeDescription(
        IN EFI_HANDLE Handle
);

// UefiBootManagerLib | MdeModulePkg/Library/UefiBootManagerLib/BmBootDescription.c
CHAR16 *
BmGetBootDescription(
        IN EFI_HANDLE Handle
);

VOID SafeFreePool(void **h);

VOID SafeShellCloseFile(SHELL_FILE_HANDLE *sfh);

int isBlockEmpty(VOID *data, UINT64 size);

CHAR16 *
EFIAPI
StrUprS(
        OUT CHAR16 *Destination,
        IN  UINTN DestMax,
        IN  CONST CHAR16 *Source,
        OUT UINT8 *Truncated
);

INTN
EFIAPI
StriCmp(
        IN CONST CHAR16 *FirstString,
        IN CONST CHAR16 *SecondString
);

INTN
EFIAPI
StrinCmp(
        IN CONST CHAR16 *FirstString,
        IN CONST CHAR16 *SecondString,
        IN UINTN Length
);

CHAR16 *StrStrip(CHAR16 *src);

VOID
EFIAPI
BufferNCpy(
        IN UINT8 *Dst,
        IN CONST UINT8 *Src,
        IN CONST UINTN Length
);

INTN
EFIAPI
BufferNCmp(
        IN VOID *FirstBuffer,
        IN VOID *SecondBuffer,
        IN UINTN Length
);

INT64 NvmeInfo(VOID);

EFI_STATUS DBUpdate(CHAR16 *SSD_SN, BOOLEAN IsRemove);

#define BlkIo_Terminate -1
#define BlkIo_Continue -2
#define BlkIo_Error -3

INT64 BlkIo_Iterator(INT64 index, EFI_HANDLE *Handle, EFI_BLOCK_IO_PROTOCOL **BlkIo);

INT64 NVME_Iterator(INT64 index, EFI_BLOCK_IO_PROTOCOL **BlkIo, CHAR16 *Desc_Buffer, INT16 Desc_Max);

CHAR16 *DescToMnSn(CHAR16 *Desc, CHAR16 **Sn);

#define MAX_HASH_CTX_SIZE 64

EFI_STATUS
EFIAPI
HashSetup(
        OUT EFI_HASH2_PROTOCOL **CryptoProtocol
);

EFI_STATUS
EFIAPI
HashInfo(
        IN CONST CHAR16 *Info,
        IN CONST EFI_HASH2_PROTOCOL *CryptoProtocol,
        IN CONST EFI_GUID *HashAlgorithm,
        OUT UINT8 *Hash,
        OUT UINTN *HashCtxSize
);

VOID PrintHash(UINT8 *Hash, UINTN HashCtxSize);

#define DB_FILE_NAME L"\\EFI\\BOOT\\db"

EFI_STATUS
EFIAPI
FOpen(
        IN CHAR16 *FileName,
        OUT EFI_FILE_PROTOCOL **DBfile
);

EFI_STATUS
EFIAPI
FWriteAtPosition(
        IN EFI_FILE_PROTOCOL *File,
        IN UINT64 Position,
        IN UINTN *BufferSize,
        IN VOID *Buffer
);

EFI_STATUS
EFIAPI
FWriteAtEnd(
        IN EFI_FILE_PROTOCOL *File,
        IN UINTN *BufferSize,
        IN VOID *Buffer
);

EFI_STATUS
EFIAPI
FFind(
        IN EFI_FILE_PROTOCOL *File,
        IN UINTN KeywordSize,
        IN VOID *Keyword,
        OUT UINT64 *FindPosition
);