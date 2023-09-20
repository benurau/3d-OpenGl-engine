#include "Monster.h"
#include <time.h>
#include <stdlib.h>

Monster::Monster(int health)
    :Health(health), Image({ 0 }), PosX(0), PosY(0)
{
	

}

void Monster::RandPos() {
    srand(time(NULL));

    PosX = rand() % (C_RES_WIDTH - Image.BitmapInfo.bmiHeader.biWidth);
    PosY = rand() % (C_RES_HEIGHT - Image.BitmapInfo.bmiHeader.biHeight);
}
