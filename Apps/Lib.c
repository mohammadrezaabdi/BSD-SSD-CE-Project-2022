
#include <DiskDrm.h>

VOID SafeFreePool(void **h) {
    if (*h) {
        gBS->FreePool(*h);
        *h = NULL;
    }
}

INTN
EFIAPI
StriCmp(
        IN CONST CHAR16 *FirstString,
        IN CONST CHAR16 *SecondString
) {
    INTN sub = 0;

    do {
        sub = CharToUpper(*FirstString) - CharToUpper(*SecondString);
        if (sub != 0) {
            break;
        }
        FirstString++;
        SecondString++;
    } while (*FirstString && *SecondString);

    return sub;
}

INTN
EFIAPI
StrinCmp(
        IN CONST CHAR16 *FirstString,
        IN CONST CHAR16 *SecondString,
        IN UINTN Length
) {
    INTN sub = 0;

    if (!Length) {
        return 0;
    }
    do {
        sub = CharToUpper(*FirstString) - CharToUpper(*SecondString);
        if (sub != 0) {
            break;
        }
        FirstString++;
        SecondString++;
        Length--;
    } while (*FirstString && *SecondString && Length);

    return Length ? (CharToUpper(*FirstString) - CharToUpper(*SecondString)) : sub;
}


CHAR16 *StrStrip(CHAR16 *src) {
    CHAR16 * src_end = src + StrLen(src) - 1;

    while (*src == L' ') {
        src++;
    }
    while (src_end >= src && *src_end == L' ') {
        *src_end = L'\0';
        src_end--;
    }
    return src;
}


VOID
EFIAPI
BufferNCpy(
        IN UINT8 *Dst,
        IN CONST UINT8 *Src,
        IN CONST UINTN Length
) {
    INTN i;

    for (i = 0; i < Length; i++) {
        Dst[i] = Src[i];
    }
}


INTN
EFIAPI
BufferNCmp(
        IN VOID *FirstBuffer,
        IN VOID *SecondBuffer,
        IN UINTN Length
) {
    INTN sub = 0;

    while (Length){
        sub = *(INT8 *) FirstBuffer - *(INT8 *) SecondBuffer;
        if (sub != 0) {
            break;
        }
        FirstBuffer++;
        SecondBuffer++;
        Length--;
    }

    return sub;
}