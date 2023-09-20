#include <stdio.h>
#include <windows.h>
#include <windowsx.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include <emmintrin.h>
#include "main.h"
#include <vector>
#include<iostream>
#include<cstdlib>
using namespace std;



#include <stdio.h>
#pragma warning(push, 3)
#include <windows.h>
#include <windowsx.h>
#pragma warning(pop)
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include <emmintrin.h>
#include <xaudio2.h>
#include "main.h"




HWND gGameWindow;
BOOL gGameIsRunning;
GAMEBITMAP gBBuffer;
PERFORMANCEDATA gPData;
SWORD gSwordData;
BOOL gWindowFocus;
MONSTER gMonster1;
GAMESTATE gGameState = GAMESCREEN;
MONSTER gMonsterList[5];
GAMEBITMAP gBackground;






int WinMain(HINSTANCE Instance, HINSTANCE PrevInstance, PSTR lpCmdLine, INT CmdShow)
{


    UNREFERENCED_PARAMETER(PrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    UNREFERENCED_PARAMETER(CmdShow);
    long long FrameStart = 0;
    long long FrameEnd = 0;
    long long ElapsedMSecondsPerFrame = 0;
    long long ElapsedMSecondsPerFrameCalc = 0;
    MSG Message = { 0 };
    int Frame = 0;

    if (GameIsAlreadyRunning() == TRUE) {
        MessageBoxA(NULL, "Program is already running!", "Error!", MB_ICONEXCLAMATION | MB_OK);

        goto Exit;
    }

    if (CreateMainGameWindow(Instance) != ERROR_SUCCESS) {
        goto Exit;
    }


    QueryPerformanceFrequency(&gPData.PerfFrequency);

    gBBuffer.BitmapInfo.bmiHeader.biSize = sizeof(gBBuffer.BitmapInfo.bmiHeader);
    gBBuffer.BitmapInfo.bmiHeader.biWidth = C_RES_WIDTH;
    gBBuffer.BitmapInfo.bmiHeader.biHeight = C_RES_HEIGHT;
    gBBuffer.BitmapInfo.bmiHeader.biBitCount = C_BPP;
    gBBuffer.BitmapInfo.bmiHeader.biCompression = BI_RGB;
    gBBuffer.BitmapInfo.bmiHeader.biPlanes = 1;
    gBBuffer.Memory = VirtualAlloc(NULL, C_AREA_MEMORY_SIZE, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);


    if (gBBuffer.Memory == NULL) {
        MessageBoxA(NULL, "MEMORY ALLOCATION FOR C_AREA FAILED!", "Error!", MB_ICONEXCLAMATION | MB_OK);
        goto Exit;
    }

    memset(gBBuffer.Memory, 220, C_AREA_MEMORY_SIZE);

    if (InitializeBitMaps() != ERROR_SUCCESS) {

        goto Exit;
    }



    gGameIsRunning = TRUE;

    while (gGameIsRunning == TRUE) {
        QueryPerformanceCounter(&FrameStart);

        while (PeekMessageA(&Message, gGameWindow, 0, 0, PM_REMOVE)) {
            DispatchMessageA(&Message);
        }

        ProcessPlayerInput();

        RenderFrameGraphics();

        QueryPerformanceCounter(&FrameEnd);
        ElapsedMSecondsPerFrame = FrameEnd - FrameStart;
        ElapsedMSecondsPerFrame *= 1000000;
        ElapsedMSecondsPerFrame /= gPData.PerfFrequency;


        /*while (ElapsedMSecondsPerFrame < 2500) {
            Sleep(0);

            ElapsedMSecondsPerFrame = FrameEnd - FrameStart;
            ElapsedMSecondsPerFrame *= 10000000;
            ElapsedMSecondsPerFrame /= gPData.PerfFrequency;
            QueryPerformanceCounter(&FrameEnd);
        }*/


        gPData.CurrentFrame++;
        gPData.Total_Frames_Rendered++;
        ElapsedMSecondsPerFrameCalc += ElapsedMSecondsPerFrame;



        if (gPData.Total_Frames_Rendered % CAL_AVGFPS == 0) {
            gPData.RawFramesPerSec = 1.0f / ((ElapsedMSecondsPerFrameCalc) * 0.00000001f);
            ElapsedMSecondsPerFrameCalc = 0;
            gPData.CurrentFrame = 0;
        }





    }



Exit:

    return 0;
}

LRESULT CALLBACK MainWndProc(_In_ HWND WindowHandle, _In_ UINT Message, _In_ WPARAM wParam, _In_ LPARAM lParam)
{
    LRESULT Result = 0;

    switch (Message)
    {

    case WM_MOUSEMOVE:
    {
        OnMouseMove(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
        return 0;

    }

    case WM_LBUTTONDOWN:
    {
        return 0;
    }

    case WM_CLOSE:
    {
        gGameIsRunning = FALSE;

        PostQuitMessage(0);
        break;
    }

    case WM_ACTIVATE:
    {
        if (wParam == 0) {
            gWindowFocus = FALSE;
        }
        else {
            gWindowFocus = TRUE;
        }
        ShowCursor(FALSE);
        break;
    }


    default:
    {
        Result = DefWindowProcA(WindowHandle, Message, wParam, lParam);
    }
    }

    return Result;
}

DWORD CreateMainGameWindow(_In_ HANDLE Instance)
{
    DWORD Result = ERROR_SUCCESS;

    WNDCLASSEX WindowClass = { 0 };


    WindowClass.cbSize = sizeof(WNDCLASSEX);
    WindowClass.style = 0;
    WindowClass.lpfnWndProc = MainWndProc;
    WindowClass.cbClsExtra = 0;
    WindowClass.cbWndExtra = 0;
    WindowClass.hInstance = Instance;
    WindowClass.hIcon = LoadIconA(NULL, IDI_APPLICATION);
    WindowClass.hCursor = LoadCursorA(NULL, IDC_ARROW);
    WindowClass.hIconSm = LoadIconA(NULL, IDI_APPLICATION);
    WindowClass.hbrBackground = CreateSolidBrush(RGB(255, 0, 255));
    WindowClass.lpszMenuName = NULL;
    WindowClass.lpszClassName = GAME_NAME;




    if (!RegisterClassExA(&WindowClass))
    {
        Result = GetLastError();
        MessageBoxA(NULL, "Window Registration Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);

        goto Exit;
    }

    gGameWindow = CreateWindowExA(0, WindowClass.lpszClassName, "KILLGUY", WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, 512, 384, NULL, NULL, Instance, NULL);

    if (gGameWindow == NULL)
    {
        Result = GetLastError();
        MessageBoxA(NULL, "Window Creation Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);

        goto Exit;
    }

    gPData.MonitorInfo.cbSize = sizeof(MONITORINFO);

    if (GetMonitorInfoA(MonitorFromWindow(gGameWindow, MONITOR_DEFAULTTOPRIMARY), &gPData.MonitorInfo) == 0) {
        Result = ERROR_MONITOR_NO_DESCRIPTOR;

        goto Exit;
    }

    gPData.MonitorWidth = gPData.MonitorInfo.rcMonitor.right - gPData.MonitorInfo.rcMonitor.left;
    gPData.MonitorHeight = gPData.MonitorInfo.rcMonitor.bottom - gPData.MonitorInfo.rcMonitor.top;

    if (SetWindowLongPtrA(gGameWindow, GWL_STYLE, WS_VISIBLE) == 0) {
        Result = GetLastError();
        goto Exit;
    }

    if (SetWindowPos(gGameWindow, HWND_TOPMOST, gPData.MonitorInfo.rcMonitor.left, gPData.MonitorInfo.rcMonitor.top,
        512, 384, SWP_FRAMECHANGED) == 0) {

        Result = GetLastError();
        goto Exit;
    }


Exit:

    return Result;
}

BOOL GameIsAlreadyRunning(void) {
    HANDLE Mutex = NULL;

    Mutex = CreateMutexA(NULL, FALSE, GAME_NAME);

    if (GetLastError() == ERROR_ALREADY_EXISTS) {
        return(TRUE);
    }
    else {
        return(FALSE);
    }
}

void OnMouseMove(int PosX, int PosY) {
    if (gWindowFocus == 0) {
        return;
    }
    static float scale = { 0 };
    float dpi = GetDpiForWindow(gGameWindow);
    scale = dpi / 96.0f;

    gSwordData.SwordPosY = PosY / scale;
    gSwordData.SwordPosX = PosX / scale;
}

void ProcessPlayerInput(void) {
    if (gWindowFocus == 0) {
        return;
    }
    short EscapeKeyDown = GetAsyncKeyState(VK_ESCAPE);
    short F2KeyDown = GetAsyncKeyState(VK_F2);


    static short F2KeyWasDown;


    if (EscapeKeyDown) {
        SendMessageA(gGameWindow, WM_CLOSE, 0, 0);
    }

    if (F2KeyDown && !F2KeyWasDown) {
        gPData.DisplayFps = !gPData.DisplayFps;
    }
    F2KeyWasDown = F2KeyDown;
}

DWORD Load32ppBitMapFile(_In_ char* FileName, _In_ GAMEBITMAP* BitMap) {
    DWORD Error = ERROR_SUCCESS;

    HANDLE FileHandle = INVALID_HANDLE_VALUE;

    WORD BitmapHeader = 0;

    DWORD PixelDataOffset = 0;

    DWORD NumberOfBytesRead = 2;

    if ((FileHandle = CreateFileA(FileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL)) == INVALID_HANDLE_VALUE) {
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

DWORD InitializeBitMaps(void) {
    DWORD Error = ERROR_SUCCESS;

    if ((Error = Load32ppBitMapFile("..\\assets\\monster1.bmp", &gMonster1.Image)) != ERROR_SUCCESS) {
        MessageBoxA(NULL, "load bitmap failed", "Error!", MB_ICONEXCLAMATION | MB_OK);

        goto Exit;
    }

    if ((Error = Load32ppBitMapFile("..\\assets\\background.bmp", &gBackground)) != ERROR_SUCCESS) {
        MessageBoxA(NULL, "load bitmap failed", "Error!", MB_ICONEXCLAMATION | MB_OK);

        goto Exit;
    }



Exit:

    return(Error);
}

void RenderFrameGraphics(void) {
#ifdef SIMD
    __m128i QuadPixel = { 0x7f, 0x00, 0x00, 0xff, 0x7f, 0x00, 0x00, 0xff, 0x7f, 0x00, 0x00, 0xff, 0x7f, 0x00, 0x00, 0xff };
    ClearScreen(QuadPixel);
#else
    PIXEL32 Pixel = { 0xff, 0x00, 0x00, 0xff };
    ClearScreen(&Pixel);
#endif
    switch (gGameState) {
    case TITLESCREEN: {
        DrawTitleScreen();
        break;
    }
    case GAMESCREEN: {
        DrawGameScreen();
        break;
    }
    }





    HDC DeviceContext = GetDC(gGameWindow);

    StretchDIBits(DeviceContext, 0, 0, 512, 384, 0, 0, C_RES_WIDTH, C_RES_HEIGHT, gBBuffer.Memory, &gBBuffer.BitmapInfo, DIB_RGB_COLORS, SRCCOPY);

    char TextBuffer[64] = { 0 };
    SelectObject(DeviceContext, (HFONT)GetStockObject(ANSI_FIXED_FONT));
    if (gPData.DisplayFps) {
        sprintf_s(TextBuffer, _countof(TextBuffer), "FPS: %0.1f", gPData.RawFramesPerSec);
        TextOutA(DeviceContext, 0, 0, TextBuffer, (int)strlen(TextBuffer));
    }


    ReleaseDC(gGameWindow, DeviceContext);
}

#ifdef SIMD
void ClearScreen(_In_ __m128i Color) {
    for (int x = 0; x < (C_RES_WIDTH * C_RES_HEIGHT); x += 4) {
        _mm_store_si128((PIXEL32*)gBBuffer.Memory + x, Color);
    }
}
#else
void ClearScreen(_In_ PIXEL32* Pixel) {
    for (int x = 0; x < (C_RES_WIDTH * C_RES_HEIGHT); x++) {
        memcpy((PIXEL32*)gBBuffer.Memory + x, Pixel, sizeof(PIXEL32));
    }
}
#endif


void BitmapToBuffer(_In_ GAMEBITMAP* BitMap, _In_ unsigned int x, _In_ unsigned int y) {
    long StartingPixel = ((C_RES_WIDTH * C_RES_HEIGHT) - C_RES_WIDTH) - (C_RES_WIDTH * y) + x;
    long StartingBitmapPixel = ((BitMap->BitmapInfo.bmiHeader.biWidth * BitMap->BitmapInfo.bmiHeader.biHeight) - \
        BitMap->BitmapInfo.bmiHeader.biWidth) - BitMap->BitmapInfo.bmiHeader.biWidth;
    long MemoryOffset = 0;
    long BitmapOffset = 0;

    PIXEL32 BitmapPixel = { 0 };


    for (int YPixel = 0; YPixel < BitMap->BitmapInfo.bmiHeader.biHeight; YPixel++) {
        for (int XPixel = 0; XPixel < BitMap->BitmapInfo.bmiHeader.biWidth; XPixel++) {
            MemoryOffset = StartingPixel + XPixel - (C_RES_WIDTH * YPixel);
            BitmapOffset = StartingBitmapPixel + XPixel - (BitMap->BitmapInfo.bmiHeader.biWidth * YPixel);

            memcpy_s(&BitmapPixel, sizeof(PIXEL32), (PIXEL32*)BitMap->Memory + BitmapOffset, sizeof(PIXEL32));
            if (BitmapPixel.Alpha == 255) {
                memcpy_s((PIXEL32*)gBBuffer.Memory + MemoryOffset, sizeof(PIXEL32), &BitmapPixel, sizeof(PIXEL32));
            }
        }
    }
}


void RenderSword(_In_ int x, _In_ int y, _In_ int prevX, _In_ int prevY) {


    PIXEL32 BladePixel = { 95, 95, 95, 255 };
    PIXEL32 ShinePixel = { 255, 255, 255, 255 };
    PIXEL32 MousePixel = { 0, 0 , 255, 255 };


    int StartingPixel = 0;
    int StartTipPixel = 0;




    if (y > (C_RES_HEIGHT / 1.2)) {
        y = (C_RES_HEIGHT / 1.2);
    }

    if (y < (C_RES_HEIGHT / 20)) {
        y = (C_RES_HEIGHT / 20);
    }

    if (x < (C_RES_WIDTH / 20)) {
        x = (C_RES_WIDTH / 20);
    }

    if (x > (C_RES_WIDTH - (C_RES_WIDTH / 20))) {
        x = (C_RES_WIDTH - (C_RES_WIDTH / 20));
    }




    if (y > 0) {
        gSwordData.Angle = (float)(C_RES_MIDDLE - x) / (float)(C_RES_HEIGHT - y);
    }
    if (fabs(gSwordData.Angle) < 1) {
        gSwordData.Angle = floor((fabs(gSwordData.Angle) * 10));
    }


    for (int AOffset = 0; AOffset < (abs(gSwordData.Angle) + 1); AOffset++) {
        for (int posY = y; posY < C_RES_HEIGHT; posY++) {

            StartingPixel = ((C_RES_WIDTH * C_RES_HEIGHT) - C_RES_WIDTH) - (posY * C_RES_WIDTH) + (C_RES_MIDDLE - 4) - ((gSwordData.Angle * (C_RES_HEIGHT - posY))) + AOffset;


            memcpy_s((PIXEL32*)gBBuffer.Memory + StartingPixel, sizeof(PIXEL32), &ShinePixel, sizeof(PIXEL32));

            for (int BladeWidth = 0; BladeWidth < 14; BladeWidth++) {
                if (BladeWidth < 10) {
                    memcpy_s((PIXEL32*)gBBuffer.Memory + StartingPixel + BladeWidth, sizeof(PIXEL32), &ShinePixel, sizeof(PIXEL32));
                }
                if (BladeWidth > 2 & BladeWidth < 10) {
                    memcpy_s((PIXEL32*)gBBuffer.Memory + StartingPixel + BladeWidth, sizeof(PIXEL32), &BladePixel, sizeof(PIXEL32));
                }
                else if (BladeWidth > 9) {
                    memcpy_s((PIXEL32*)gBBuffer.Memory + StartingPixel + BladeWidth, sizeof(PIXEL32), &ShinePixel, sizeof(PIXEL32));
                }
            }

            if (posY == y) {
                memcpy_s((PIXEL32*)gBBuffer.Memory + StartingPixel, sizeof(PIXEL32), &MousePixel, sizeof(PIXEL32));
            }

        }
    }

    int TipOffset = 7;
    int TipWidthCurrent = 1;

    for (int Tip = (y - 7); Tip < y; Tip++) {
        StartTipPixel = StartTipPixel = ((C_RES_WIDTH * C_RES_HEIGHT) - C_RES_WIDTH) - (Tip * C_RES_WIDTH) + (C_RES_MIDDLE - 4) - ((gSwordData.Angle * (C_RES_HEIGHT - Tip)));
        for (int TipWidth = 0; TipWidth < TipWidthCurrent; TipWidth++) {
            memcpy_s((PIXEL32*)gBBuffer.Memory + StartTipPixel + TipOffset + TipWidth, sizeof(PIXEL32), &ShinePixel, sizeof(PIXEL32));
        }
        TipWidthCurrent += 2;
        TipOffset--;
    }

}

short FirstRun = 1;

void MonsterSpawn(_In_ MONSTER Monster) {

    time_t t;
    srand((unsigned)time(&t));

    int RandPosX = rand() % (C_RES_WIDTH - Monster.Image.BitmapInfo.bmiHeader.biWidth - (C_RES_HEIGHT / 5));
    int RandPosY = rand() % (C_RES_HEIGHT - Monster.Image.BitmapInfo.bmiHeader.biHeight - (C_RES_WIDTH / 10));

    int WeakPointY1 = rand() % Monster.Image.BitmapInfo.bmiHeader.biHeight;
    int WeakPointY2 = rand() % Monster.Image.BitmapInfo.bmiHeader.biHeight;

    MONSTER NewMonster = { Monster.Image, 200, RandPosX, RandPosY, WeakPointY1 ,WeakPointY2, 0 };

    if (FirstRun != 0) {
        for (int i = 0; i < 4; i++) {
            gMonsterList[i] = NewMonster;
            gMonsterList[i].Health = -1;
            *gMonsterList[i].HitPixels = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(int) * 2000);
        }
    }



    for (int i = 0; i < 4; i++) {
        if (gMonsterList[i].Health <= 0) {
            gMonsterList[i] = NewMonster;
            break;
        }
    }
    FirstRun = 0;
}



void DrawGameScreen() {


    BitmapToBuffer(&gBackground, 0, 0);

    if (gPData.Total_Frames_Rendered % 10 == 0) {
        MonsterSpawn(gMonster1);
    }



    for (int i = 0; i < 4; i++) {
        if (gMonsterList[i].Health > 0) {
            BitmapToBuffer(&gMonsterList[i].Image, gMonsterList[i].PosX, gMonsterList[i].PosY);
            DrawMonsterHitBox(&gMonsterList[i]);
        }
    }

    RenderSword(gSwordData.SwordPosX, gSwordData.SwordPosY, gSwordData.SwordPrevPosX, gSwordData.SwordPrevPosY);

}

void DrawTitleScreen() {
    static long long LocalFrameCounter;
    static long long LastFrameScene;

    memset(gBBuffer.Memory, 0, C_AREA_MEMORY_SIZE);


}

void DrawMonsterHitBox(_In_ MONSTER* Monster) {


    PIXEL32 WeakSpotPixel = { 50, 50, 155, 255 };
    PIXEL32 HitWeakSpotPixel = { 0, 0, 60, 255 };
    int YOffset = Monster->Y1 - Monster->Y2;
    int StartingPixel = 0;
    float HitBoxAngle = 0;

    HitBoxAngle = (float)(YOffset) / (float)(Monster->Image.BitmapInfo.bmiHeader.biWidth);

    for (int XPixel = 0; XPixel < Monster->Image.BitmapInfo.bmiHeader.biWidth; XPixel++) {

        StartingPixel = ((C_RES_WIDTH * C_RES_HEIGHT) - C_RES_WIDTH) + Monster->PosX - (Monster->PosY * C_RES_WIDTH) - (C_RES_WIDTH * Monster->Y1) + XPixel + (C_RES_WIDTH * (int)(XPixel * HitBoxAngle));

        for (int length = 0; length < 5; length++) {

            if (CursorHitBox(StartingPixel + (length * C_RES_WIDTH)) == 1 && IfInArray(StartingPixel + (length * C_RES_WIDTH), &Monster->HitPixels) == 0) {
                Monster->Health -= 1;
                memcpy_s((PIXEL32*)gBBuffer.Memory + StartingPixel + (length * C_RES_WIDTH), sizeof(PIXEL32), &HitWeakSpotPixel, sizeof(PIXEL32));
                AddToArray(StartingPixel + (length * C_RES_WIDTH), &Monster->HitPixels);
            }

            else if (IfInArray(StartingPixel + (length * C_RES_WIDTH), &Monster->HitPixels) == 1) {

                for (int length = 0; length < 5; length++) {
                    memcpy_s((PIXEL32*)gBBuffer.Memory + StartingPixel + (length * C_RES_WIDTH), sizeof(PIXEL32), &HitWeakSpotPixel, sizeof(PIXEL32));
                }
                break;
            }

            else {
                memcpy_s((PIXEL32*)gBBuffer.Memory + StartingPixel + (length * C_RES_WIDTH), sizeof(PIXEL32), &WeakSpotPixel, sizeof(PIXEL32));
            }

        }
    }

}

int IfInArray(_Inout_ int Number, _In_ int* Array) {
    for (int i = 0; i < 2000; i++) {
        if (Array[i] == Number) {
            return 1;
        }
    }
    return 0;
}

void AddToArray(_In_ int Number, _In_ int* Array) {

    int i = 0;
    while (Array[i] != 0) {
        i++;
    }
    if (Number > 0) {
        Array[i] = Number;
    }
}

int CursorHitBox(_In_ int Pixel) {
    int MousePos = ((C_RES_WIDTH * C_RES_HEIGHT) - C_RES_WIDTH) + gSwordData.SwordPosX - (gSwordData.SwordPosY * C_RES_WIDTH);

    if (Pixel >= MousePos && Pixel <= (MousePos + 3)) {
        return 1;
    }
    if (Pixel >= (MousePos + C_RES_WIDTH) && Pixel <= (MousePos + 3 + C_RES_WIDTH)) {
        return 1;
    }
    if (Pixel >= (MousePos + (2 * C_RES_WIDTH)) && Pixel <= (MousePos + 3 + (2 * C_RES_WIDTH))) {
        return 1;
    }


    return 0;
}






class Monster {
public:
    GAMEBITMAP Image;
    int MonsterHealth;
    int PosX;
    int PosY;
    

    Monster(GAMEBITMAP Image, int MHealth) {
        srand((unsigned)time(NULL));
        this->Image = Image;
        this->MonsterHealth = MHealth;
        this->PosX = rand() % (C_RES_WIDTH - this->Image.BitmapInfo.bmiHeader.biWidth);
        this->PosY = rand() % (C_RES_HEIGHT - this->Image.BitmapInfo.bmiHeader.biHeight);
    };
  
};

class HitBox {
public:
    int PosY1;
    int PosY2;
    int HitBoxHealth;
    vector<int> HitPixels;

    HitBox(GAMEBITMAP Image, int Health) {
        this->PosY1 = rand() % Image.BitmapInfo.bmiHeader.biHeight;
        this->PosY2 = rand() % Image.BitmapInfo.bmiHeader.biHeight;
        this->HitBoxHealth = Health;
    };

    void AddHit(int pos) {
        if (std::find(this->HitPixels.begin(), this->HitPixels.end(), pos) == this->HitPixels.end()) {
            this->HitBoxHealth--;
            this->HitPixels.push_back(pos);
        };
    };

    int CheckHHealth() {
        return this->HitBoxHealth;
    }
};



int main(int argc, char const* argv[])
{
    /* code */
    return 0;
}

