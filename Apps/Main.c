#include <SsdDrm.h>

#include <Library/HandleParsingLib.h> //BUGBUG

CHAR16 NVME_SERIAL_NUMBER[NVME_PRODUCT_SERIAL_NUMBER_SIZE] = {0};
CHAR16 NVME_PRODUCT_MODEL_NUMBER[NVME_PRODUCT_MODEL_NUMBER_SIZE] = {0};


UINT8 Verbose_Level = 1;


VOID Usage(VOID) {
    Print(L"Usage: \n"
          L"SSDDRM [arg1 [arg2 ...]] \n"
          L"   arg: \n"
          L"       INFO - The basic infomation from every detected NVME SSD\n"
          L"       ADD [sn] - Add the hash of selected drive from DB\n"
          L"       REMOVE [sn] - Remove the hash of selected drive\n"
          L"       REMOVEALL - Clear DB\n"
          L"       HELP - This help message\n"
          L"\n"
          L"Open Source:\n"
          L"   https://github.com/mohammadrezaabdi/SSDDRM.efi\n"
    );

}

INTN
EFIAPI
ShellAppMain(
        IN UINTN argc,
        IN CHAR16 **argv
) {
    UINTN i;

    if (Verbose_Level > 1) {
        for (i = 0; i < argc; i++) {
            Print(L"argv[%d]: %s\n", i, argv[i]);
        }
    }
    if (argc > 1) {
        if (StriCmp(argv[1], L"INFO") == 0) {
            NvmeInfo();
        } else if (StriCmp(argv[1], L"ADD") == 0) {
            if (argc > 2) {
                return DBAddNvme(argv[2]);
            } else {
                Print(L"%s needs the NVME's serial number assigned.\n", argv[1]);
                Usage();
                return 1;
            }
        } else if (StriCmp(argv[1], L"REMOVE") == 0) {
            //remove hash of ssd
        } else if (StriCmp(argv[1], L"REMOVEALL") == 0) {
            //remove all non existed ssds from OS
        } else {
            Usage();
            return 0;
        }
    } else {
        Usage();
        return 0;
    }
    return 0;
}
