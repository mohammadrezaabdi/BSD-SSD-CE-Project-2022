
#include <SsdDrm.h>

INT64 NvmeInfo(VOID) {
    INT64 NVME_Count = 1, i = 0;
    EFI_HASH2_PROTOCOL *mCryptoProtocol;
    UINTN HashCtxSize;
    EFI_STATUS Status;

    Status = HashSetup(&mCryptoProtocol, &HashCtxSize);
    if (EFI_ERROR (Status)) {
        Print(L"Error when initializing Hash2 protocol: %r\n", Status);
        return EFI_ABORTED;
    }

    Print(L"NVME Storage\n");
    for (i = 0;; i++) {
        EFI_BLOCK_IO_PROTOCOL *BlkIo;
        CHAR16 Desc[NVME_DESCRIPTION_SIZE + 1], *Mn, *Sn;
        UINT8 *HashedInfo;

        if ((i = NVME_Iterator(i, &BlkIo, Desc, NVME_DESCRIPTION_SIZE)) < 0) {
            break;
        }

        HashedInfo = AllocatePool(HashCtxSize);
        Status = HashInfo(Desc, NVME_DESCRIPTION_SIZE, mCryptoProtocol, HashCtxSize, HashedInfo);
        if (EFI_ERROR (Status)) {
            Print(L"Error when hashing description of SSD #%d: %r\n", NVME_Count, Status);
            return EFI_ABORTED;
        }

        Print(L"  #%d:\n", NVME_Count);
        Mn = DescToMnSn(Desc, &Sn);
        Print(L"  Model  : %s\n", Mn);
        Print(L"  Serial : %s\n", Sn);
        Print(L"  %d blocks = %ld bytes\n", BlkIo->Media->LastBlock + 1,
              (BlkIo->Media->LastBlock + 1) * BlkIo->Media->BlockSize);
        Print(L"  SHA256 : ");
        PrintHash(HashedInfo, HashCtxSize);
        SafeFreePool((VOID **) &HashedInfo);

        NVME_Count++;
    }
    if (!NVME_Count) {
        Print(L"  Not found.\n");
    }
    return NVME_Count;
}
