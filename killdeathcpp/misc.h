#pragma once
#include <windows.h>
#include <vector>
#include <xaudio2.h>

		

#define C_RES_WIDTH	  512
#define C_RES_HEIGHT  384

#define C_RES_MIDDLE (C_RES_WIDTH / 2)

#define SWORD_WIDTH (C_RES_WIDTH / 50)
#define SWORD_LENGTH (C_RES_HEIGHT * 0.8)


typedef struct GAMEBITMAP {
	BITMAPINFO BitmapInfo;
	void* Memory;
} GAMEBITMAP;


typedef struct PIXEL32 {
	unsigned char Blue;
	unsigned char Green;
	unsigned char Red;
	unsigned char Alpha;
} PIXEL32;


