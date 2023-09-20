#pragma once
#include "misc.h"


typedef struct PERFORMANCEDATA{
    MONITORINFO MonitorInfo;
    long MonitorWidth;
    long MonitorHeight;
}PERFORMANCEDATA;


LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

BOOL GameIsAlreadyRunning(void);


class Window {
public:
    HWND hWnd;
    BOOL windowMutex;
    Window();
    Window(const Window&) = delete;
    Window& operator =(const Window&) = delete;
    ~Window();
    bool ProcessMessage();

    
    

private:
    HINSTANCE hInstance;
    PERFORMANCEDATA PData;
};





