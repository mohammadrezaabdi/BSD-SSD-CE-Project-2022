#include <SsdDrm.h>

EFI_STATUS
EFIAPI
FOpen(
        IN CHAR16 * FileName,
        OUT EFI_FILE_PROTOCOL **DBfile
) {
    EFI_HANDLE * handles = NULL;
    UINTN handleCount = 0;
    EFI_STATUS Status = EFI_SUCCESS;
    UINT64 index = 0;

    Status = gBS->LocateHandleBuffer(ByProtocol,
                                     &gEfiSimpleFileSystemProtocolGuid,
                                     NULL,
                                     &handleCount,
                                     &handles);
    if (EFI_ERROR(Status)) {
        Print(L"Error when locating Simple File System Protocol: %r\n", Status);
        return BlkIo_Error;
    }
    for (index = 0; index < handleCount; ++index) {
        EFI_SIMPLE_FILE_SYSTEM_PROTOCOL * fs = NULL;
        EFI_FILE_PROTOCOL * root = NULL;

        Status = gBS->HandleProtocol(
                handles[index],
                &gEfiSimpleFileSystemProtocolGuid,
                (VOID **) &fs);
        if (EFI_ERROR(Status)) {
            Print(L"Error when handling Simple File System Protocol: %r\n", Status);
            return BlkIo_Error;
        }

        Status = fs->OpenVolume(fs, &root);
        if (EFI_ERROR(Status)) {
            Print(L"Error when opening Root volume: %r\n", Status);
            return BlkIo_Error;
        }

        Status = root->Open(
                root,
                DBfile,
                FileName,
                EFI_FILE_MODE_CREATE | EFI_FILE_MODE_WRITE | EFI_FILE_MODE_READ,
                0);
        if (EFI_ERROR(Status)) {
            Print(L"Error when opening DB file: %r\n", Status);
            return BlkIo_Error;
        }
    }

    return Status;
}