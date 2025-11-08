#ifndef MISC_H
#define MISC_H


#include <windows.h>



		

#define C_RES_WIDTH	  1024
#define C_RES_HEIGHT  768

#define C_RES_MIDDLE (C_RES_WIDTH / 2)

#define SWORD_WIDTH (C_RES_WIDTH / 50)
#define SWORD_LENGTH (C_RES_HEIGHT * 0.8)
#define RADIUS 0.15f

typedef struct PIXEL32 {
	unsigned char Blue;
	unsigned char Green;
	unsigned char Red;
	unsigned char Alpha;
} PIXEL32;



#endif