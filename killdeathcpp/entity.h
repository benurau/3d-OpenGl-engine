#pragma once
#include <GLFW/glfw3.h>

class entity{
public:
	entity(int health, GLuint texture, float x, float y, double size);

	void renderEntity();
	void killEntity();


private:
	int health;
	GLuint texture;
	float x;
	float y;
	double size;
};