#pragma once
#include "misc.h"


#define C_BPP         32
#define C_AREA_MEMORY_SIZE (C_RES_WIDTH * C_RES_HEIGHT * (C_BPP/8))
#define C_AREA_PIXELS 512*384
#define C_RES_MIDDLE (C_RES_WIDTH / 2)


class BBuffer {

public:
	BBuffer();
	GAMEBITMAP bBuffer;
	void BitmapToBuffer(GAMEBITMAP* BitMap, unsigned int x, unsigned int y);
	void SwordToBuffer(int MousePosX, int MousePosY);
	void DrawLine(int x, int y, int startX, int startY, int length,  PIXEL32 pixel);
	void RenderFrameGraphics(HWND window);
};
