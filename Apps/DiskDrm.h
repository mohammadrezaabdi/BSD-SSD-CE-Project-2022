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

#include <Protocol/DiskIo.h>
#include <Protocol/DiskInfo.h>
#include <Protocol/DriverBinding.h>
#include <Protocol/ComponentName2.h>

#include <IndustryStandard/Atapi.h>
#include <IndustryStandard/Scsi.h>
#include <IndustryStandard/Nvme.h>

#include <Protocol/BlockIo.h>
#include <Protocol/DevicePath.h>
#include <Protocol/EraseBlock.h>
#include <Protocol/IdeControllerInit.h>
#include <Protocol/NvmExpressPassthru.h>

#include <Protocol/Hash2.h>
#include <Library/UefiBootManagerLib/InternalBm.h>

#define DISK_PRODUCT_MODEL_NUMBER_SIZE 40
#define DISK_PRODUCT_SERIAL_NUMBER_SIZE 20
#define DISK_DESCRIPTION_SIZE (DISK_PRODUCT_MODEL_NUMBER_SIZE + DISK_PRODUCT_SERIAL_NUMBER_SIZE + 2)

CHAR16 *
BmGetBootDescriptionMinimal (
  IN EFI_HANDLE Handle
);

extern UINT8 Verbose_Level;

VOID SafeFreePool(void **h);

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

INT64 DiskInfo(VOID);

EFI_STATUS DBUpdate(CHAR16 *DiskN, BOOLEAN IsRemove);

#define BlkIo_Terminate -1
#define BlkIo_Continue -2
#define BlkIo_Error -3

INT64 BlkIo_Iterator(INT64 index, EFI_HANDLE *Handle, EFI_BLOCK_IO_PROTOCOL **BlkIo);

INT64 DISK_Iterator(INT64 index, EFI_BLOCK_IO_PROTOCOL **BlkIo, CHAR16 *Desc_Buffer, INT16 Desc_Max);

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

#define DB_FILE_NAME L"\\db.bin"

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