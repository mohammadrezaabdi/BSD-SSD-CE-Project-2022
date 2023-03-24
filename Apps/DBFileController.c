#include <SsdDrm.h>

EFI_STATUS DBAddNvme(CHAR16 *SSD_SN) {
    INT64 i = 0;
    UINTN SN_Length = 0;
    UINT16 NVME_Count = 1;
    INT16 NVME_SN_MATCHED = 0;
    EFI_HASH2_PROTOCOL *mCryptoProtocol;
    EFI_FILE_PROTOCOL * DBFile;
    UINTN DB_Info_Length = 0;
    EFI_FILE_INFO *DB_Info;
    UINT64 DBSize;
    EFI_STATUS Status;

    Status = HashSetup(&mCryptoProtocol);
    if (EFI_ERROR (Status)) {
        Print(L"Error when initializing Hash2 protocol: %r\n", Status);
        return EFI_ABORTED;
    }

    SN_Length = StrLen(SSD_SN);
    if (SN_Length <= 0) {
        Print(L"SSD serial number is Empty.\n");
        return EFI_ABORTED;
    }

    Status = FOpen(DB_FILE_NAME, &DBFile);
    if (EFI_ERROR (Status)) {
        Print(L"Error when opening DB file: %r\n", Status);
        return EFI_ABORTED;
    }

    DBFile->GetInfo(DBFile, &gEfiFileInfoGuid, &DB_Info_Length, NULL);
    DB_Info = AllocateZeroPool(DB_Info_Length);
    Status = DBFile->GetInfo(DBFile, &gEfiFileInfoGuid, &DB_Info_Length, (VOID *) DB_Info);
    if (EFI_ERROR (Status)) {
        Print(L"Error when getting info of DB file: %r\n", Status);
        return EFI_ABORTED;
    }
    DBSize = DB_Info->FileSize;
    Print(L"DB file size is %d.\n", DBSize);
    SafeFreePool((VOID **) DB_Info);

    Print(L"Saving NVME GPT\n");
    for (i = 0;; i++) {
        EFI_BLOCK_IO_PROTOCOL *BlkIo;
        CHAR16 Desc[NVME_DESCRIPTION_SIZE + 1], *Sn, Sn_Shorthand[NVME_PRODUCT_SERIAL_NUMBER_SIZE + 1];

        if ((i = NVME_Iterator(i, &BlkIo, Desc, NVME_DESCRIPTION_SIZE)) < 0) {
            break;
        }

        DescToMnSn(Desc, &Sn);
        StrnCpy(Sn_Shorthand, Sn, SN_Length);
        Print(L"  Storage %2d - Serial Number : %s \n", NVME_Count, Sn);
        Print(L"    Serial Number Shorthand  : %s \n", Sn_Shorthand);

        if (StrinCmp(SSD_SN, Sn_Shorthand, SN_Length) == 0) {
            Print(L"    Matched Serial Number : %s as %s\n", SSD_SN, Sn);
            NVME_SN_MATCHED++;
            UINT8 HashedInfo[MAX_HASH_CTX_SIZE + 1];
            UINTN HashCtxSize;

            Status = HashInfo(Desc, mCryptoProtocol, &gEfiHashAlgorithmSha256Guid, HashedInfo, &HashCtxSize);
            if (EFI_ERROR (Status)) {
                Print(L"Error when hashing description of SSD #%d: %r\n", NVME_Count, Status);
                return EFI_ABORTED;
            }

            //TODO: do not add duplicate NVME SSD
            //TODO: fix file updating corruption
            
            // Save position
            UINT64 StoredPosition;
            Status = DBFile->GetPosition(DBFile, &StoredPosition);
            if (EFI_ERROR (Status)) {
                return Status;
            }

            // set position to the last in order to update the DB
            Status = DBFile->SetPosition(DBFile, DBSize - 1);
            if (EFI_ERROR (Status)) {
                Print(L"Error when Setting DB's cursor position: %r\n", Status);
                return EFI_ABORTED;
            }

            Status = DBFile->Write(DBFile, &HashCtxSize, (CHAR8 *) HashedInfo);
            if (EFI_ERROR (Status)) {
                Print(L"Error when writing on DB: %r\n", Status);
                return EFI_ABORTED;
            }
            Print(L"Written %d bytes in to DB file.\n", HashCtxSize);

            // set position to the last in order to update the DB
            Status = DBFile->SetPosition(DBFile, StoredPosition);
            if (EFI_ERROR (Status)) {
                Print(L"Error when Setting DB's cursor position: %r\n", Status);
                return EFI_ABORTED;
            }

            Status = DBFile->Flush(DBFile);
            if (EFI_ERROR (Status)) {
                Print(L"Error when Flushing DB's file: %r\n", Status);
                return EFI_ABORTED;
            }

            break;
        }

        NVME_Count++;
    }

    Status = DBFile->Close(DBFile);
    if (EFI_ERROR (Status)) {
        Print(L"Error when closing DB file: %r\n", Status);
        return EFI_ABORTED;
    }

    if (!NVME_SN_MATCHED) {
        Print(L"  No (partial) match for NVME storage with serial number: %s.\n", SSD_SN);
    }
    return NVME_SN_MATCHED ? 0 : EFI_NOT_FOUND;
}