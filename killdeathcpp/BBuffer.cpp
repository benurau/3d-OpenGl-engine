#include <windows.h>
#include "BBuffer.h"
#include <iostream>


BBuffer::BBuffer()
    : bBuffer({0})
{
    
    bBuffer.BitmapInfo.bmiHeader.biSize = sizeof(bBuffer.BitmapInfo.bmiHeader);
    bBuffer.BitmapInfo.bmiHeader.biWidth = C_RES_WIDTH;
    bBuffer.BitmapInfo.bmiHeader.biHeight = C_RES_HEIGHT;
    bBuffer.BitmapInfo.bmiHeader.biBitCount = C_BPP;
    bBuffer.BitmapInfo.bmiHeader.biCompression = BI_RGB;
    bBuffer.BitmapInfo.bmiHeader.biPlanes = 1;
    bBuffer.Memory = VirtualAlloc(NULL, C_AREA_MEMORY_SIZE, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

    
}







void BBuffer::BitmapToBuffer(_In_ GAMEBITMAP* BitMap, _In_ unsigned int x, _In_ unsigned int y) {
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
            if (BitmapPixel.Alpha == 255 && MemoryOffset > 1 && MemoryOffset <= C_AREA_PIXELS) {
                memcpy_s((PIXEL32*)bBuffer.Memory + MemoryOffset, sizeof(PIXEL32), &BitmapPixel, sizeof(PIXEL32));
            }
        }
    }
}



void BBuffer::SwordToBuffer(int MousePosX, int MousePosY)
{
    PIXEL32 ShinePixel = { 255, 255, 255, 255 };

    //if (abs(angle) < 1) {

    //}
  
    DrawLine(MousePosX, MousePosY, C_RES_MIDDLE, C_RES_HEIGHT,  SWORD_LENGTH, ShinePixel);
        
}


void BBuffer::DrawLine(int x, int y, int startX, int startY, int length, PIXEL32 pixel){

    int StartingPixel = 0;


    float lLength = sqrt(pow((x - startX), 2) + pow((y - startY), 2));

    float vecX = (x - startX) / lLength;
    float vecY = (y - startY) / lLength;

    float angle = vecX / vecY;

    int vecEndX = 256 + length * vecX;
    int vecEndY = 384 + length * vecY;



    if ((startY - vecEndY) * angle >= (C_RES_WIDTH - startX)) {
        int overLengthX = abs((C_RES_WIDTH - startX) - abs((startY - vecEndY) * angle));
        vecEndY += overLengthX / angle;
        printf_s("%f   ", overLengthX*(vecY/vecX));
    }


    if ((startY - vecEndY) * angle <= -(C_RES_WIDTH - startX)) {
        int overLengthX = abs((C_RES_WIDTH - startX) - abs((startY - vecEndY) * angle));
        vecEndY -= overLengthX / angle;
        printf_s("%f   ", overLengthX * (vecY / vecX));
    }
    

    for (int AOffset = 0; AOffset < abs(vecX/ vecY); AOffset++) {
        for (int posY = vecEndY; posY < startY; posY++) {

            StartingPixel = ((C_RES_WIDTH * C_RES_HEIGHT) - C_RES_WIDTH) - (posY * C_RES_WIDTH) + startX - ((angle * (C_RES_HEIGHT - posY))) + AOffset;

            if (StartingPixel > 1 && StartingPixel <= C_AREA_PIXELS) {
                memcpy_s((PIXEL32*)bBuffer.Memory + StartingPixel, sizeof(PIXEL32), &pixel, sizeof(PIXEL32));
            }

        }


    }

}




void BBuffer::RenderFrameGraphics(HWND window){
    HDC DeviceContext = GetDC(window);

    StretchDIBits(DeviceContext, 0, 0, C_RES_WIDTH, C_RES_HEIGHT, 0, 0, C_RES_WIDTH, C_RES_HEIGHT, bBuffer.Memory, &bBuffer.BitmapInfo, DIB_RGB_COLORS, SRCCOPY);
}




