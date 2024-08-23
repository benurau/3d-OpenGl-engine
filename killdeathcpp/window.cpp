#include <iostream>
#include "renderer.h"
#include "soundEngine.h"
#include "SDL2/SDL.h"
#include "sword.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Object.h"
#include "Camera.h"


float deltaTime = 0.0f;
float lastFrame = 0.0f;
bool firstMouse = true;
float lastX = C_RES_WIDTH / 2.0;
float lastY = C_RES_HEIGHT / 2.0;
Camera camera;

void errorCallback(int error, const char* description) {
    std::cerr << "Error: " << description << std::endl;
}


void processKeyboard(GLFWwindow* window);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouseCallback(GLFWwindow* window, double xpos, double ypos);

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
    glfwSetCursorPosCallback(window, mouseCallback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);


    Renderer renderer(window);
    soundEngine sEngine;
    sEngine.initialize();
    WAV huh = sEngine.loadWavFile("..\\assets\\ahem_x.wav", "ahem");
    SDL_AudioDeviceID aDevice = sEngine.openAudioDevice(huh);
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

   
    Object triangleObject(triangleVerticles, sizeof(triangleVerticles), 3, renderer.shaders["triangle"], PC, 0);
    Object guadObject(guad, sizeof(guad), 6, renderer.shaders["guad"], PC, 0, guadIndices, sizeof(guadIndices));
    Object Background2dObject(background, sizeof(background), 6, renderer.shaders["guadtexture"], PCT, backgroundTexture, guadIndices, sizeof(guadIndices));
    Object cubeObject(box, sizeof(box), 36, renderer.shaders["guad3d"], PT, backgroundTexture);

    glEnable(GL_DEPTH_TEST);
    while (!glfwWindowShouldClose(window)) {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        processKeyboard(window);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        cubeObject.make3DSquare();
        cubeObject.changeView(camera.GetViewMatrix());
        renderer.drawObject(cubeObject);
        glfwPollEvents();
        glfwSwapBuffers(window);
    }

    sEngine.audioCleanup(huh, aDevice);
    glfwTerminate();
    return 0;
}

void mouseCallback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }
    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;
    camera.ProcessMouseMovement(xoffset, yoffset);
}

void processKeyboard(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        camera.ProcessKeyboard(FORWARD, deltaTime);
        std::cout << "forward";
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        std::cout << "backward";
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        std::cout << "left";
        camera.ProcessKeyboard(LEFT, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        std::cout << "right";
        camera.ProcessKeyboard(RIGHT, deltaTime);
    }
}

