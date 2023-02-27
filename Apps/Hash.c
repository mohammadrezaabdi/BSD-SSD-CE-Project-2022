//TODO: write documentation
#include <SsdDrm.h>

//TODO: make Hash Algorithm optional
EFI_STATUS
EFIAPI
HashSetup(
        OUT EFI_HASH2_PROTOCOL **CryptoProtocol,
        OUT UINTN *HashCtxSize
) {
    EFI_SERVICE_BINDING_PROTOCOL *bindingProtocol = NULL;
    EFI_STATUS Status = EFI_SUCCESS;
    EFI_HANDLE * ChildHandle = NULL;

    Status = gBS->LocateProtocol(
            &gEfiHash2ServiceBindingProtocolGuid,
            NULL,
            (VOID **) &bindingProtocol
    );
    if (EFI_ERROR (Status) || bindingProtocol == NULL) {
        Print(L"Error when locating Hash2 binding protocol: %r\n", Status);
        return EFI_ABORTED;
    }

    //TODO: create child only once

    Status = bindingProtocol->CreateChild(bindingProtocol, (VOID **) &ChildHandle);
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

    Status = (*CryptoProtocol)->GetHashSize(*CryptoProtocol, &gEfiHashAlgorithmSha256Guid, HashCtxSize);
    if (EFI_ERROR (Status) || CryptoProtocol == NULL) {
        Print(L"Error when finding sha256 context size : %r\n", Status);
        return EFI_ABORTED;
    }

    return Status;
}

//TODO: check Info size by ascii strlen
//TODO: check If Hash is correct
EFI_STATUS
EFIAPI
HashInfo(
        IN CHAR16 *Info,
        IN UINTN DataSize,
        IN EFI_HASH2_PROTOCOL *CryptoProtocol,
        IN UINTN HashCtxSize,
        IN OUT UINT8 *Hash
) {
    EFI_STATUS Status = EFI_SUCCESS;
    EFI_HASH2_OUTPUT *HashCtx;
    HashCtx = AllocatePool(HashCtxSize);

    Status = CryptoProtocol->HashInit(CryptoProtocol, &gEfiHashAlgorithmSha256Guid);
    if (EFI_ERROR (Status)) {
        Print(L"Hash Init Failed: %r\n", Status);
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

    BufferNCpy(Hash, HashCtx->Sha256Hash, HashCtxSize);

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