#include <iostream>
#include "renderer.h"
#include "soundEngine.h"
#include "SDL2/SDL.h"
#include "sword.h"

float mousePosX = 0.0f;
float mousePosY = 0.0f;

void errorCallback(int error, const char* description) {
    std::cerr << "Error: " << description << std::endl;
}

void cursorPositionCallback(GLFWwindow* window, double xpos, double ypos) {
    mousePosX = xpos;
    mousePosY = ypos;
}

int main(int argc, char* argv[]){
    glfwSetErrorCallback(errorCallback);
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW\n";
        return -1;
    }
    GLFWwindow* window = glfwCreateWindow(C_RES_WIDTH, C_RES_HEIGHT, "GameWindow", NULL, NULL);
    if (!window) {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD\n";
        return -1;
    }
    glfwSetCursorPosCallback(window, cursorPositionCallback);
    Renderer renderer(window);

    soundEngine sEngine;
    sEngine.initialize();

    WAV huh = sEngine.loadWavFile("..\\assets\\ahem_x.wav", "ahem");
    SDL_AudioDeviceID aDevice = sEngine.openAudioDevice(huh);
    //sEngine.playAudio(huh, aDevice);

    GLuint monsterTexture = renderer.create2DBitMapTexture("..\\assets\\monster1.bmp");
    GLuint backgroundTexture = renderer.create2DBitMapTexture("..\\assets\\background.bmp");

    while (!glfwWindowShouldClose(window)) {
        renderer.clear();
        renderer.drawLine(0.1, 0.5, 0.2, 0.9);
        renderer.drawBackgroundTexture(backgroundTexture);
        //renderer.draw2DBitMap(monsterTexture);
        drawSword(renderer, mousePosX, mousePosY);
        renderer.swapBuffers();
        glfwPollEvents(); 
    }

    sEngine.audioCleanup(huh, aDevice);
    glfwTerminate();
    return 0;
}