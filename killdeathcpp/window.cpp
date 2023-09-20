#include <tchar.h>
#include "window.h"
#include <iostream>

BOOL gWindowFocus;


LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg)
    {
    case WM_CLOSE:
        DestroyWindow(hWnd);
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
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
    case WM_MOUSEMOVE:
    {
        
        return 0;
    }
    }

    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

BOOL GameIsAlreadyRunning(void) {
    HANDLE Mutex = NULL;

    Mutex = CreateMutexW(NULL, FALSE, L"KILLDEATH");

    if (GetLastError() == ERROR_ALREADY_EXISTS) {
        return(TRUE);
    }
    else {
        return(FALSE);
    }
}


Window::Window()
    : hInstance(GetModuleHandle(nullptr)), windowMutex(GameIsAlreadyRunning())
{
    const wchar_t* CLASS_NAME = L"GameWC";
    

    WNDCLASSW WindowClass = { 0 };
    WindowClass.style = 0;
    WindowClass.lpfnWndProc = WindowProc;
    WindowClass.cbClsExtra = 0;
    WindowClass.cbWndExtra = 0;
    WindowClass.hInstance = hInstance;
    WindowClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    WindowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    WindowClass.hbrBackground = CreateSolidBrush(RGB(255, 0, 255));
    WindowClass.lpszMenuName = NULL;
    WindowClass.lpszClassName = CLASS_NAME;

    if (!RegisterClassW(&WindowClass)) {
        MessageBoxW(NULL, L"window class failed", L"Error!", MB_ICONEXCLAMATION | MB_OK);
    }

    DWORD style = WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU;

    hWnd = CreateWindowExW(
        0,
        CLASS_NAME,
        L"KILLDEATH",
        WS_VISIBLE,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        C_RES_WIDTH,
        C_RES_HEIGHT,
        NULL,
        NULL,
        hInstance,
        NULL
    );

    if (hWnd == NULL) {
        MessageBoxW(NULL, L"window creation failed", L"Error!", MB_ICONEXCLAMATION);
    }

    

    PData.MonitorInfo.cbSize = sizeof(MONITORINFO);

    GetMonitorInfoW(MonitorFromWindow(hWnd, MONITOR_DEFAULTTOPRIMARY), &PData.MonitorInfo);

    PData.MonitorWidth = PData.MonitorInfo.rcMonitor.right - PData.MonitorInfo.rcMonitor.left;
    PData.MonitorHeight = PData.MonitorInfo.rcMonitor.bottom - PData.MonitorInfo.rcMonitor.top;

    SetWindowLongPtrW(hWnd, GWL_STYLE, WS_VISIBLE);

    SetWindowPos(hWnd, HWND_TOPMOST, PData.MonitorInfo.rcMonitor.left, PData.MonitorInfo.rcMonitor.top,
        C_RES_WIDTH, C_RES_HEIGHT, SWP_FRAMECHANGED);

    ShowWindow(hWnd, SW_SHOW);

}




Window::~Window() {
    const wchar_t* CLASS_NAME = L"GameWC";

    UnregisterClassW(CLASS_NAME, hInstance);

}

bool Window::ProcessMessage()
{
    MSG msg = { 0 };

    while (PeekMessage(&msg, nullptr, 0u, 0u, PM_REMOVE)) {

        if (msg.message == WM_QUIT) {
            return false;
        }

        TranslateMessage(&msg);
        DispatchMessage(&msg);

    }

    return true;
}




