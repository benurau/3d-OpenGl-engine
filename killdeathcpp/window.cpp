#include <iostream>
#include "renderer.h"
#include "soundEngine.h"
#include "SDL2/SDL.h"
#include "sword.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Object.h"
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

    GLfloat triangleVerticles[] = {
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

    int guadIndices[]{
        0, 1, 2,
        0, 2, 3
    };
    //Object::Object(GLfloat verticles[], int verticlesByteSize, int totalVerticles, Shader shader, int vboCount, int textureID)
    Object triangleObject(triangleVerticles, sizeof(triangleVerticles), 3, renderer.shaders["triangle"], 2, 0);
    Object guadObject(guad, sizeof(guad), 6, renderer.shaders["guad"], 2, 0, guadIndices, sizeof(guadIndices));
    Object Background2dObject(background, sizeof(background), 6, renderer.shaders["guadtexture"], 3, backgroundTexture, guadIndices, sizeof(guadIndices));
    Object cubeObject(box, sizeof(box), 36, renderer.shaders["guad3d"], 4, backgroundTexture);

    glEnable(GL_DEPTH_TEST);
    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        //renderer.changeSize(renderer.shaders["guadtexture"], glm::vec3(0.5, 0.5, 0.5));
        cubeObject.make3DSquare();
        renderer.drawObject(cubeObject);
        glfwPollEvents();
        glfwSwapBuffers(window);
    }

    sEngine.audioCleanup(huh, aDevice);
    glfwTerminate();
    return 0;
}