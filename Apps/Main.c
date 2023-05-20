#include <DiskDrm.h>

#include <Library/HandleParsingLib.h> //BUGBUG

UINT8 Verbose_Level = 1;

VOID Usage(VOID) {
    Print(L"Usage: \n"
          L"DISKDRM [arg1 [arg2 ...]] \n"
          L"   arg: \n"
          L"       INFO - The basic infomation from every detected DISK\n"
          L"       ADD [#Number] - Add the hash of selected drive to DB\n"
          L"       REMOVE [#Number] - Remove the hash of selected drive from DB\n"
          L"       HELP - This help message\n"
          L"\n"
          L"Open Source:\n"
          L"   https://github.com/mohammadrezaabdi/DISKDRM.efi\n"
          L"\n"
          L"\n"
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
            DiskInfo();
        } else if (StriCmp(argv[1], L"ADD") == 0 || StriCmp(argv[1], L"REMOVE") == 0) {
            if (argc > 2) {
                return DBUpdate(argv[2], (StriCmp(argv[1], L"REMOVE") == 0));
            } else {
                Print(L"%s needs the DISK's number provided by INFO command.\n", argv[1]);
                Usage();
                return 1;
            }
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
