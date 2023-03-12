//TODO: write documentation
#include <SsdDrm.h>

EFI_STATUS
EFIAPI
HashSetup(
        OUT EFI_HASH2_PROTOCOL **CryptoProtocol
) {
    EFI_SERVICE_BINDING_PROTOCOL *BindingProtocol = NULL;
    EFI_STATUS Status = EFI_SUCCESS;
    EFI_HANDLE * ChildHandle = NULL;

    // check if there is hash2 protocol before
    Status = gBS->LocateProtocol(
            &gEfiHash2ProtocolGuid,
            NULL,
            (VOID **) CryptoProtocol
    );
    if (Status == EFI_SUCCESS && *CryptoProtocol != NULL) {
        return Status;
    }

    Status = gBS->LocateProtocol(
            &gEfiHash2ServiceBindingProtocolGuid,
            NULL,
            (VOID **) &BindingProtocol
    );
    if (EFI_ERROR (Status) || BindingProtocol == NULL) {
        Print(L"Error when locating Hash2 binding protocol: %r\n", Status);
        return EFI_ABORTED;
    }

    Status = BindingProtocol->CreateChild(BindingProtocol, (VOID **) &ChildHandle);
    if (EFI_ERROR (Status) || ChildHandle == NULL) {
        Print(L"Error when building Hash2 child: %r\n", Status);
        return EFI_ABORTED;
    }

    Status = gBS->HandleProtocol(
            ChildHandle,
            &gEfiHash2ProtocolGuid,
            (VOID **) CryptoProtocol
    );
    if (EFI_ERROR (Status) || *CryptoProtocol == NULL) {
        Print(L"Error when handling Hash2 protocol: %r\n", Status);
        return EFI_ABORTED;
    }

    return Status;
}

//TODO: check If Hash is correct
EFI_STATUS
EFIAPI
HashInfo(
        IN CONST CHAR16 *Info,
        IN CONST EFI_HASH2_PROTOCOL *CryptoProtocol,
        IN CONST EFI_GUID *HashAlgorithm,
        OUT UINT8 *Hash,
        OUT UINTN *HashCtxSize
) {
    EFI_STATUS Status = EFI_SUCCESS;
    EFI_HASH2_OUTPUT *HashCtx;
    UINTN DataSize;

    Status = CryptoProtocol->GetHashSize(CryptoProtocol, HashAlgorithm, HashCtxSize);
    if (EFI_ERROR (Status) || CryptoProtocol == NULL) {
        Print(L"Error when finding sha256 context size : %r\n", Status);
        return EFI_ABORTED;
    }
    HashCtx = AllocatePool(*HashCtxSize);

    Status = CryptoProtocol->HashInit(CryptoProtocol, HashAlgorithm);
    if (EFI_ERROR (Status)) {
        Print(L"Hash Init Failed: %r\n", Status);
        return EFI_ABORTED;
    }

    DataSize = StrLen(Info);
    if (DataSize <= 0) {
        Print(L"Hash Info Is Empty.\n");
        return EFI_ABORTED;
    }

    Status = CryptoProtocol->HashUpdate(CryptoProtocol, (UINT8 *) Info, DataSize);
    if (EFI_ERROR (Status)) {
        Print(L"Hash Update Failed: %r\n", Status);
        return EFI_ABORTED;
    }

    Status = CryptoProtocol->HashFinal(CryptoProtocol, HashCtx);
    if (EFI_ERROR (Status)) {
        Print(L"Hash Finalize Failed: %r\n", Status);
        return EFI_ABORTED;
    }

    BufferNCpy(Hash, HashCtx->Sha256Hash, *HashCtxSize);

    SafeFreePool((VOID **) &HashCtx);
    return Status;
}

VOID PrintHash(UINT8 *Hash, UINTN HashCtxSize) {
    UINTN Index = 0;
    if (Hash == NULL) {
        return;
    }
    for (Index = 0; Index < HashCtxSize; Index++) {
        Print(L"%2X", Hash[Index]);
    }
    Print(L"\n");
}