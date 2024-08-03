#include <iostream>
#include "renderer.h"
#include "soundEngine.h"
#include "SDL2/SDL.h"
#include "sword.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
float mousePosX = 0.0f;
float mousePosY = 0.0f;

void errorCallback(int error, const char* description) {
    std::cerr << "Error: " << description << std::endl;
}

void cursorPositionCallback(GLFWwindow* window, double xpos, double ypos) {
    mousePosX = xpos/C_RES_WIDTH/2-1;
    mousePosY = ypos / C_RES_HEIGHT / 2 - 1;
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

    renderer.loadAllShaders();
    float line[]{
        -0.3f, -0.3f,  0.0f,
        0.3f, 0.3f,  0.0f
    };

    float box[] = {
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
    };

    GLfloat triangle[] = {
       0.0f,  0.5f,  0.0f,  1.0f, 0.0f, 0.0f,
       0.5f, -0.5f,  0.0f,  0.0f, 1.0f, 0.0f,
      -0.5f, -0.5f,  0.0f,  0.0f, 0.0f, 1.0f,
    };

    GLfloat guad[] =
    {
        -0.9f, -0.9f, 0.0f,  1.0f, 0.0f, 0.0f,
         0.9f, -0.9f, 0.0f,  0.0f, 1.0f, 0.0f,
         0.9f,  0.9f, 0.0f,  0.0f, 0.0f, 1.0f,
        -0.9f,  0.9f, 0.0f,  1.0f, 1.0f, 0.0f,
    };

    float background[] = {
         1.0f,  -1.0f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,
         1.0f,   1.0f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,
        -1.0f,   1.0f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,
        -1.0f,  -1.0f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f 
    };


    glEnable(GL_DEPTH_TEST);
    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        renderer.make3DSquare(renderer.shaders["guad3d"]);    
        renderer.drawCube(renderer.vaos["guadtexture"], renderer.shaders["guad3d"], box, sizeof(box), backgroundTexture);
        //renderer.changeSize(renderer.shaders["guadtexture"], glm::vec3(0.5, 0.5, 0.5));
        //renderer.draw2DBitMap(renderer.vaos["guadtexture"], renderer.shaders["guadtexture"], background, sizeof(background), backgroundTexture);
        //renderer.drawTriange(renderer.vaos["triangle"], renderer.shaders["triangle"], triangle, sizeof(triangle));
        //renderer.drawRectangle(renderer.vaos["guad"], renderer.shaders["guad"], guad, sizeof(guad));
        //renderer.drawFixedLine(renderer.vaos["line"], renderer.shaders["line"], line, sizeof(line), 1.8);
        glfwPollEvents();
        glfwSwapBuffers(window);
    }

    sEngine.audioCleanup(huh, aDevice);
    glfwTerminate();
    return 0;
}