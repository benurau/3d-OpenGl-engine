#pragma once
#include "misc.h"


class Monster {
public:
	GAMEBITMAP Image;
	Monster(int health);
	void RandPos();
private:
	int PosX;
	int PosY;
	int Health;
};
