#include <SsdDrm.h>

EFI_STATUS DBAddNvme(CHAR16 *SSD_SN) {
    INT64 i = 0;
    UINTN SN_Length = 0;
    UINT16 NVME_Count = 1;
    INT16 NVME_SN_MATCHED = 0;
    EFI_HASH2_PROTOCOL *mCryptoProtocol;
    EFI_FILE_PROTOCOL * DBFile;
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

    for (i = 0;; i++) {
        EFI_BLOCK_IO_PROTOCOL *BlkIo;
        CHAR16 Desc[NVME_DESCRIPTION_SIZE + 1], *Sn, Sn_Shorthand[NVME_PRODUCT_SERIAL_NUMBER_SIZE + 1];

        if ((i = NVME_Iterator(i, &BlkIo, Desc, NVME_DESCRIPTION_SIZE)) < 0) {
            break;
        }

        DescToMnSn(Desc, &Sn);
        StrnCpy(Sn_Shorthand, Sn, SN_Length);
        Print(L"Storage %2d - Serial Number : %s \n", NVME_Count, Sn);
        Print(L"  Serial Number Shorthand  : %s \n", Sn_Shorthand);

        if (StrinCmp(SSD_SN, Sn_Shorthand, SN_Length) == 0) {
            Print(L"  Matched Serial Number : %s as %s\n", SSD_SN, Sn);
            NVME_SN_MATCHED++;
            UINT8 HashedInfo[MAX_HASH_CTX_SIZE + 1];
            UINTN HashCtxSize;

            Status = HashInfo(Desc, mCryptoProtocol, &gEfiHashAlgorithmSha256Guid, HashedInfo, &HashCtxSize);
            if (EFI_ERROR (Status)) {
                Print(L"Error when hashing description of SSD #%d: %r\n", NVME_Count, Status);
                break;
            }

            //TODO: fix file updating corruption

            Status = FFind(DBFile, &HashCtxSize, HashedInfo);
            if (Status == EFI_SUCCESS) {
                Print(L"SSD has been already added to Database.\n");
                break;
            }

            Status = FWriteAtEnd(DBFile, &HashCtxSize, HashedInfo);
            if (EFI_ERROR (Status)) {
                Print(L"Error when updating Database File: %r\n", Status);
                break;
            }
            Print(L"SSD added to Database Successfully.\n");

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