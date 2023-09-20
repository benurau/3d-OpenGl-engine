#include "32ppBitMap.h"


DWORD Load32ppBitMapFile(const wchar_t* FileName, GAMEBITMAP* BitMap)
{
    DWORD Error = ERROR_SUCCESS;

    HANDLE FileHandle = INVALID_HANDLE_VALUE;

    WORD BitmapHeader = 0;

    DWORD PixelDataOffset = 0;

    DWORD NumberOfBytesRead = 2;

    if ((FileHandle = CreateFileW(FileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL)) == INVALID_HANDLE_VALUE) {
        Error = GetLastError();
        goto Exit;
    }

    if (ReadFile(FileHandle, &BitmapHeader, 2, &NumberOfBytesRead, NULL) == 0) {
        Error = GetLastError();
        goto Exit;
    }

    if (BitmapHeader != 0x4d42) {
        Error = ERROR_FILE_INVALID;
        goto Exit;
    }

    if (SetFilePointer(FileHandle, 0xA, NULL, FILE_BEGIN) == 0) {
        Error = GetLastError();
        goto Exit;
    }

    if (ReadFile(FileHandle, &PixelDataOffset, sizeof(DWORD), &NumberOfBytesRead, NULL) == 0) {
        Error = GetLastError();
        goto Exit;
    }

    if (SetFilePointer(FileHandle, 0xE, NULL, FILE_BEGIN) == 0) {
        Error = GetLastError();
        goto Exit;
    }



    if (ReadFile(FileHandle, &BitMap->BitmapInfo.bmiHeader, sizeof(BITMAPINFOHEADER), &NumberOfBytesRead, NULL) == 0) {
        Error = GetLastError();
        goto Exit;
    }

    if ((BitMap->Memory = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, BitMap->BitmapInfo.bmiHeader.biSizeImage)) == NULL) {
        Error = ERROR_NOT_ENOUGH_MEMORY;
        goto Exit;
    }

    if (SetFilePointer(FileHandle, PixelDataOffset, NULL, FILE_BEGIN) == 0) {
        Error = GetLastError();
        goto Exit;
    }

    if (ReadFile(FileHandle, BitMap->Memory, BitMap->BitmapInfo.bmiHeader.biSizeImage, &NumberOfBytesRead, NULL) == 0) {
        Error = GetLastError();
        goto Exit;
    }





Exit:
    if (FileHandle != INVALID_HANDLE_VALUE) {
        CloseHandle(FileHandle);
    }

    return(Error);
}


