/** @file

Clean the starting sectors of an NVME disk.
Ref. https://en.wikipedia.org/wiki/GUID_Partition_Table


NvmeGpt command [data]

command/data
  1. zap $Sn
  2. info
  3. TODO: backup
  4. TODO: restore


**/

#include <SsdDrm.h>

#include <Library/HandleParsingLib.h> //BUGBUG

CHAR16 NVME_SERIAL_NUMBER[NVME_PRODUCT_SERIAL_NUMBER_SIZE] = {0};
CHAR16 NVME_PRODUCT_MODEL_NUMBER[NVME_PRODUCT_MODEL_NUMBER_SIZE] = {0};


UINT8 Verbose_Level = 1;


VOID Usage(VOID)
{
  Print (L"Usage: \n"
         L"NvmeGpt [arg1 [arg2 ...]] \n"
         L"   arg: \n"  
         L"       ZAP Serial_Number - Wipe out the NVME SSD's GPT\n"
//TODO         L"       STORE\  n"
//TODO         L"       RESTORE\n"
         L"       INFO - The basic infomation from every detected NVME SSD\n"
         L"       HELP - This help message\n"
         L"\n"
         L"Open Source:\n"
         L"   https://github.com/mohammadrezaabdi/SSDDRM.efi\n"
         );

}

INTN
EFIAPI
ShellAppMain (
    IN UINTN argc,
    IN CHAR16 **argv
)
{
  // Only for Test Purpose 
  // Message string for digest validation
  // message = www.lab-z.com, len = 13
  // UINT8 HashData[] = {0x77, 0x77, 0x77, 0x2E, 0x6C, 0x61, 0x62, 0x2D, 0x7A, 0x2E, 0x63, 0x6F, 0x6D};
  CHAR8 *HashData = "www.lab-z.com";
  EFI_HASH2_PROTOCOL  *mCryptProtocol = NULL;
  EFI_SERVICE_BINDING_PROTOCOL *bindingProtocol = NULL;
  EFI_STATUS                  Status;
  EFI_HANDLE *ChildHandle = NULL;

  Status = gBS->LocateProtocol(
          &gEfiHash2ServiceBindingProtocolGuid,
          NULL,
          (VOID **) &bindingProtocol
  );
  if (EFI_ERROR (Status) || bindingProtocol == NULL) {
      Print(L"Error when locating Hash2 binding protocol: %r\n", Status);
      return EFI_ABORTED;
  }

  Status = bindingProtocol->CreateChild(bindingProtocol, (VOID **) &ChildHandle);
  if (EFI_ERROR (Status) || ChildHandle == NULL) {
      Print(L"Error when building Hash2 child: %r\n", Status);
      return EFI_ABORTED;
  }

  Status = gBS->HandleProtocol (
          ChildHandle,
          &gEfiHash2ProtocolGuid,
          (VOID **) &mCryptProtocol
  );
  if (EFI_ERROR (Status) || mCryptProtocol == NULL) {
      Print(L"Error when handling Hash2 protocol: %r\n", Status);
      return EFI_ABORTED;
  }

  Print(L"- SHA256: \n");
  UINTN    CtxSize;
  EFI_HASH2_OUTPUT    *HashCtx;
  UINTN    DataSize;
  UINTN    Index;
  Status = mCryptProtocol->GetHashSize(mCryptProtocol, &gEfiHashAlgorithmSha256Guid, &CtxSize);
  if (EFI_ERROR (Status) || mCryptProtocol == NULL) {
      Print(L"Error when finding sha256 context size : %r\n", Status);
      return EFI_ABORTED;
  }
  Print(L"sha256 size: %d\n", CtxSize);
  HashCtx = AllocatePool (CtxSize);
  DataSize = AsciiStrLen(HashData);
  Print(L"data size: %d\n", DataSize);

  Print (L"Init... \n");
  Status  = mCryptProtocol->HashInit(mCryptProtocol, &gEfiHashAlgorithmSha256Guid);
  if (EFI_ERROR (Status)) {
      Print (L"[Fail]: %r\n", Status);
      return EFI_ABORTED;
  }

  Print (L"Update... \n");
  Status  = mCryptProtocol->HashUpdate(mCryptProtocol, (UINT8*)HashData, DataSize);
  if (EFI_ERROR (Status)) {
      Print (L"[Fail]: %r\n", Status);
      return EFI_ABORTED;
  }

  Print (L"Finalize... \n");
  Status  = mCryptProtocol->HashFinal(mCryptProtocol, HashCtx);
  if (EFI_ERROR (Status)) {
      Print (L"[Fail]: %r\n", Status);
      return EFI_ABORTED;
  }

  for (Index=0;Index<CtxSize;Index++) {
      Print (L"%2X  ",HashCtx->Sha256Hash[Index]);
  }
  Print (L"\n");
  FreePool (HashCtx);

  UINTN i;

  if (Verbose_Level > 1) {
    for (i=0; i<argc; i++) {
      Print (L"argv[%d]: %s\n", i, argv[i]);
    }
  }
  if (argc > 1) {
    if (StriCmp(argv[1], L"INFO") == 0) {
      NvmeInfo();
      //TODO: show ssd hash as key
    }
    else if (StriCmp(argv[1], L"SAVE") == 0) {
      //save ssd hash key to the database file
    }
    else if (StriCmp(argv[1], L"CHECK") == 0) {
      //check the ssd if it exists in database
    }
    else if (StriCmp(argv[1], L"REMOVEALL") == 0) {
      //remove all non existed ssds from OS
    }
    else {
      Usage();
      return 0;
    }
  }
  else {
    Usage();
    return 0;
  }
  return 0;
}
