#include <iostream>
#include "renderer.h"
#include "soundEngine.h"
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

    std::vector<glm::vec3> cubePos = {
        {0.0f, -0.25f, 0.5f}, {0.5f, -0.25f, 0.5f}, {0.5f, 0.25f,  0.5f}, {0.0f, 0.25f, 0.5f},
        {0.0f, -0.25f, 0.0f}, {0.5f, -0.25f, 0.0f}, {0.5f, 0.25f, 0.0f}, {0.0f, 0.25f, 0.0f}
    };

    std::vector<glm::vec2> texCoords = {
        {0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f},
        {0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f}
    };

    std::vector<glm::vec3> normals(8, { 0.0f, 0.0f, 0.0f });

    std::vector<unsigned int> cubeIndices = {
        0, 1, 2, 2, 3, 0,
        4, 5, 6, 6, 7, 4,
        4, 0, 3, 3, 7, 4,
        1, 5, 6, 6, 2, 1,
        3, 2, 6, 6, 7, 3,
        4, 5, 1, 1, 0, 4
    };

    std::vector<Vertex> cubeVertices;

    for (size_t i = 0; i < cubePos.size(); ++i) {
        Vertex vertex;
        vertex.position = cubePos[i];
        vertex.normal = normals[i];
        vertex.texCoords = texCoords[i];
        cubeVertices.push_back(vertex);
    }

    Vertex vertex3;
    vertex3.position = glm::vec3(1.0f, -1.0f, 0.0f);
    vertex3.normal = glm::vec3(1.0f, 0.0f, 0.0f);
    vertex3.texCoords = glm::vec2(0.0f, 0.0f);
    Vertex vertex32;
    vertex32.position = glm::vec3(1.0f, 1.0f, 0.8f);
    vertex32.normal = glm::vec3(0.0f, 1.0f, 0.0f);
    vertex32.texCoords = glm::vec2(0.0f, 0.0f);
    Vertex vertex33;
    vertex33.position = glm::vec3(-1.0f, 1.0f, 0.0f);
    vertex33.normal = glm::vec3(0.0f, 0.0f, 1.0f);
    vertex33.texCoords = glm::vec2(0.0f, 0.0f);
    Vertex vertex34;
    vertex34.position = glm::vec3(-1.0f, -1.0f, 0.0f);
    vertex34.normal = glm::vec3(1.0f, 1.0f, 0.0f);
    vertex34.texCoords = glm::vec2(0.0f, 0.0f);
    std::vector<Vertex> guadVertices;
    guadVertices.push_back(vertex3);
    guadVertices.push_back(vertex32);
    guadVertices.push_back(vertex33);
    guadVertices.push_back(vertex34);

    std::vector<GLuint> guadIndices{
        0, 1, 2,
        0, 2, 3
    };

    Vertex vertex1;
    vertex1.position = glm::vec3(0.0f, 0.5f, 0.0f);
    vertex1.normal = glm::vec3(1.0f, 0.0f, 0.0f);
    vertex1.texCoords = glm::vec2(0.0f, 0.0f);
    Vertex vertex12;
    vertex12.position = glm::vec3(0.5f, -0.5f, 0.8f);
    vertex12.normal = glm::vec3(0.0f, 1.0f, 0.0f);
    vertex12.texCoords = glm::vec2(0.0f, 0.0f);
    Vertex vertex13;
    vertex13.position = glm::vec3(-0.5f, -0.5f, 0.0f);
    vertex13.normal = glm::vec3(0.0f, 0.0f, 1.0f);
    vertex13.texCoords = glm::vec2(0.0f, 0.0f);
    std::vector<Vertex> triangleVertices;
    triangleVertices.push_back(vertex1);
    triangleVertices.push_back(vertex12);
    triangleVertices.push_back(vertex13);


    //Object triangleObject(triangleVertices, 3, renderer.shaders["triangle"], 0);
    Object guadObject(guadVertices, 6, renderer.shaders["guad"], 0, guadIndices);
    Object background2dObject(guadVertices, 6, renderer.shaders["guadtexture"], backgroundTexture, guadIndices);
    Object cubeObject(cubeVertices, 36, renderer.shaders["guad3d"], backgroundTexture, cubeIndices);
    std::vector<Object> objects;
    objects.push_back(guadObject);
    objects.push_back(background2dObject);
    objects.push_back(cubeObject);

    glEnable(GL_DEPTH_TEST);
    while (!glfwWindowShouldClose(window)) {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        processKeyboard(window);
        cubeObject.make3DSquare();
        cubeObject.changeView(camera.GetViewMatrix());
        renderer.drawObject(cubeObject);
        if (cubeObject.hitbox.CheckCameraCollision(camera.position, camera.movement)) {
            camera.position -= camera.movement - cubeObject.hitbox.correctingMovement;
        }
        //printf("return of collission %d \n",cubeObject.hitbox.CheckCameraCollision(camera.position, camera.movement));     
        //std::cout<<cubeObject.hitbox.checkAllPointsInTriangles(camera.position);
        glfwPollEvents();
        glfwSwapBuffers(window);
    }

    for (Object i : objects) {
        i.Destroy();
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
        //std::cout << "forward";
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        //std::cout << "backward";
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        //std::cout << "left";
        camera.ProcessKeyboard(LEFT, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        //std::cout << "right";
        camera.ProcessKeyboard(RIGHT, deltaTime);
    }
}

