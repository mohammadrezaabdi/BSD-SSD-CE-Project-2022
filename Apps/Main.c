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
         L"SSDDRM [arg1 [arg2 ...]] \n"
         L"   arg: \n"
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
