#include "renderer.h"
#include <iostream>
#include <cmath>
#include <algorithm>
#include <fstream>
#include <sstream>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "misc.h"



Renderer::Renderer(GLFWwindow* window) : window(window){}
void Renderer::clear() {
    glClear(GL_COLOR_BUFFER_BIT);
}

void Renderer::swapBuffers() {
    glfwSwapBuffers(window);
}










