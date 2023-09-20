#include <iostream>
#include <windows.h>
#include "window.h"
#include "game.h"
#include "BBuffer.h"
#include "32ppBitMap.h"
#include "Monster.h"
#include "Wav.h"


int main() {
    Window* gWindow = new Window();
    BBuffer* bBuffer = new BBuffer();
    Monster* bugla = new Monster(200);
    GAMEBITMAP* background = new GAMEBITMAP;
    GAMESOUND* ahem = new GAMESOUND;

    RECT windowRect;
    POINT mousePos;
    
    Load32ppBitMapFile(L"..\\assets\\background.bmp", background);

    LoadWavFromFile("..\\assets\\ahem_x.wav", ahem);
   
    bool running = true;

   
    

    while (running) {

        bBuffer->BitmapToBuffer(background, 0, 0);

        GetCursorPos(&mousePos);
        ScreenToClient(gWindow->hWnd, &mousePos);

        GetClientRect(gWindow->hWnd, &windowRect);

        if (PtInRect(&windowRect, mousePos))
        {
            bBuffer->SwordToBuffer(mousePos.x, mousePos.y);
            
        }

        

        bBuffer->RenderFrameGraphics(gWindow->hWnd);

        if (!gWindow->ProcessMessage()) {
            running = false;
        }

        Sleep(10);
    }

    delete gWindow;

    return 0;
}






