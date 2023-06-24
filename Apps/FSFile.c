#include <DiskDrm.h>

EFI_STATUS
EFIAPI
FOpen(
        IN CHAR16 *FileName,
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
                EFI_FILE_SYSTEM);
        if (DBfile != NULL){
            return Status;
        }
    }

    return EFI_NOT_FOUND;
}

EFI_STATUS
EFIAPI
FWriteAtPosition(
        IN EFI_FILE_PROTOCOL *File,
        IN UINT64 Position,
        IN UINTN *BufferSize,
        IN VOID *Buffer
) {
    EFI_STATUS Status = EFI_SUCCESS;
    UINT64 StoredPosition;

    // save position
    Status = File->GetPosition(File, &StoredPosition);
    if (EFI_ERROR (Status)) {
        return Status;
    }

    // set position to the last in order to update the DB
    Status = File->SetPosition(File, Position);
    if (EFI_ERROR (Status)) {
        Print(L"Error when Setting DB's cursor position: %r\n", Status);
        return EFI_ABORTED;
    }

    Status = File->Write(File, BufferSize, Buffer);
    if (EFI_ERROR (Status)) {
        Print(L"Error when writing on DB: %r\n", Status);
        return EFI_ABORTED;
    }

    // set position to the last in order to update the DB
    Status = File->SetPosition(File, StoredPosition);
    if (EFI_ERROR (Status)) {
        Print(L"Error when Setting DB's cursor position: %r\n", Status);
        return EFI_ABORTED;
    }

    Status = File->Flush(File);
    if (EFI_ERROR (Status)) {
        Print(L"Error when Flushing DB's file: %r\n", Status);
        return EFI_ABORTED;
    }

    return Status;
}

EFI_STATUS
EFIAPI
FWriteAtEnd(
        IN EFI_FILE_PROTOCOL *File,
        IN UINTN *BufferSize,
        IN VOID *Buffer
) {
    UINTN DB_Info_Length = 0;
    EFI_FILE_INFO *DB_Info;
    UINT64 FSize;
    EFI_STATUS Status = EFI_SUCCESS;

    // get file size
    File->GetInfo(File, &gEfiFileInfoGuid, &DB_Info_Length, NULL);
    DB_Info = AllocateZeroPool(DB_Info_Length);
    Status = File->GetInfo(File, &gEfiFileInfoGuid, &DB_Info_Length, (VOID *) DB_Info);
    if (EFI_ERROR (Status)) {
        Print(L"Error when getting info of DB file: %r\n", Status);
        return EFI_ABORTED;
    }
    FSize = DB_Info->FileSize;
    SafeFreePool((VOID **) &DB_Info);

    // save position
    Status = FWriteAtPosition(File, FSize, BufferSize, Buffer);
    if (EFI_ERROR (Status)) {
        Print(L"Error when Writing in DB's file At Position %d: %r\n", FSize, Status);
        return EFI_ABORTED;
    }

    return Status;
}

EFI_STATUS
EFIAPI
FFind(
        IN EFI_FILE_PROTOCOL *File,
        IN UINTN KeywordSize,
        IN VOID *Keyword,
        OUT UINT64 *FindPosition
) {
    EFI_STATUS SearchStatus = EFI_NOT_FOUND;
    VOID *Buffer;
    UINT64 Position = 0;
    UINTN DB_Info_Length = 0;
    EFI_FILE_INFO *DB_Info;
    UINT64 FSize;
    UINTN ReadSize = KeywordSize;
    EFI_STATUS Status = EFI_SUCCESS;

    Buffer = AllocateZeroPool(KeywordSize);
    if (Buffer == NULL) {
        Print(L"Error when Allocating memory for reading file.\n");
        return EFI_ABORTED;
    }

    // get file size
    File->GetInfo(File, &gEfiFileInfoGuid, &DB_Info_Length, NULL);
    DB_Info = AllocateZeroPool(DB_Info_Length);
    Status = File->GetInfo(File, &gEfiFileInfoGuid, &DB_Info_Length, (VOID *) DB_Info);
    if (EFI_ERROR (Status)) {
        Print(L"Error when getting info of DB file: %r\n", Status);
        return EFI_ABORTED;
    }
    FSize = DB_Info->FileSize;
    SafeFreePool((VOID **) &DB_Info);

    while (Position + KeywordSize <= FSize) {
        Status = File->SetPosition(File, Position);
        if (EFI_ERROR (Status)) {
            Print(L"Error when Setting DB's cursor position: %r\n", Status);
            break;
        }
        Status = File->Read(File, &ReadSize, Buffer);
        if (EFI_ERROR (Status)) {
            Print(L"Error when Reading DB's file: %r\n", Status);
            break;
        }
        if (ReadSize == KeywordSize && BufferNCmp(Keyword, Buffer, ReadSize) == 0) {
            SearchStatus = EFI_SUCCESS;
            break;
        }
        Position++;
    }

    SafeFreePool((VOID **) &Buffer);
    *FindPosition = Position;
    return SearchStatus;
}