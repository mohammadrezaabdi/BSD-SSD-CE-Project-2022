#include <DiskDrm.h>

EFI_STATUS DBUpdate(CHAR16 *DiskN, BOOLEAN IsRemove) {
    INT64 i = 0;
    UINTN NLength = 0;
    UINTN DiskCount = 1;
    INT16 DiskNMatched = 0;
    EFI_HASH2_PROTOCOL *mCryptoProtocol;
    EFI_FILE_PROTOCOL * DBFile;
    EFI_STATUS Status;

    Status = HashSetup(&mCryptoProtocol);
    if (EFI_ERROR (Status)) {
        Print(L"Error when initializing Hash2 protocol: %r\n", Status);
        return EFI_ABORTED;
    }

    DiskN = StrStrip(DiskN);
    NLength = StrLen(DiskN);
    if (NLength <= 0) {
        Print(L"Invalid Option.\n");
        return EFI_ABORTED;
    }

    Status = FOpen(DB_FILE_NAME, &DBFile);
    if (EFI_ERROR (Status)) {
        Print(L"Error when opening DB file: %r\n", Status);
        return EFI_ABORTED;
    }

    for (i = 0;; i++) {
        EFI_BLOCK_IO_PROTOCOL *BlkIo;
        CHAR16 Desc[DISK_DESCRIPTION_SIZE + 1];
        UINT8 HashedInfo[MAX_HASH_CTX_SIZE + 1];
        UINTN HashCtxSize;

        if ((i = DISK_Iterator(i, &BlkIo, Desc, DISK_DESCRIPTION_SIZE)) < 0) {
            break;
        }

        Status = HashInfo(Desc, mCryptoProtocol, &gEfiHashAlgorithmSha256Guid, HashedInfo, &HashCtxSize);
        if (EFI_ERROR (Status)) {
            Print(L"Error when hashing description of DISK #%d: %r\n", DiskCount, Status);
            break;
        }

        CHAR16 * DISKCountStr = AllocatePool(NLength + sizeof(CHAR16) + 1);
        ASSERT (DISKCountStr != NULL);
        UnicodeSPrint(DISKCountStr, NLength + sizeof(CHAR16) + 1, L"%d", DiskCount);
        if (StrinCmp(DiskN, DISKCountStr, NLength) == 0) {
            Print(L"    Disk #%s Selected.\n", DiskN);
            DiskNMatched++;

            UINT64 Position = 0;
            Status = FFind(DBFile, HashCtxSize, HashedInfo, &Position);
            if (!IsRemove && Status == EFI_SUCCESS) {
                Print(L"DISK has been already added to Database.\n");
                break;
            } else if (IsRemove && Status == EFI_NOT_FOUND) {
                Print(L"DISK not Found in Database.\n");
                break;
            }

            if (IsRemove) {
                VOID *Buffer;
                Buffer = AllocateZeroPool(HashCtxSize);
                Status = FWriteAtPosition(DBFile, Position, &HashCtxSize, Buffer);
                if (EFI_ERROR (Status)) {
                    Print(L"Error when updating Database File: %r\n", Status);
                    break;
                }
                SafeFreePool((VOID **) &Buffer);
                Print(L"DISK removed from Database Successfully.\n");
            } else {
                Status = FWriteAtEnd(DBFile, &HashCtxSize, HashedInfo);
                if (EFI_ERROR (Status)) {
                    Print(L"Error when updating Database File: %r\n", Status);
                    break;
                }
                Print(L"DISK added to Database Successfully.\n");
            }

            break;
        }

        SafeFreePool((VOID **) &DISKCountStr);
        DiskCount++;
    }

    Status = DBFile->Close(DBFile);
    if (EFI_ERROR (Status)) {
        Print(L"Error when closing DB file: %r\n", Status);
        return EFI_ABORTED;
    }

    if (!DiskNMatched) {
        Print(L"  invalid option: %s.\n", DiskN);
    }
    return DiskNMatched ? 0 : EFI_NOT_FOUND;
}