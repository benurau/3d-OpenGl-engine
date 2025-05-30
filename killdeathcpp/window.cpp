#include <iostream>
#include "renderer.h"
#include "soundEngine.h"
#include "Object.h"
#include "Camera.h"
#include "ObjectParams.h"
#include "Colissions.h"

float deltaTime = 0.0f;
float lastFrame = 0.0f;
bool firstMouse = true;
float lastX = C_RES_WIDTH / 2.0;
float lastY = C_RES_HEIGHT / 2.0;
float bounceSmoothener = 0.01f;
Camera camera;

void errorCallback(int error, const char* description) {
    std::cerr << "Error: " << description << std::endl;
}


void processKeyboard(GLFWwindow* window);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouseCallback(GLFWwindow* window, double xpos, double ypos);

std::ostream& operator<<(std::ostream& os, const glm::vec3& v) {
    os << "(" << v.x << ", " << v.y << ", " << v.z << ")";
    return os;
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
    glfwSetCursorPosCallback(window, mouseCallback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    Renderer renderer(window);
    //soundEngine sEngine;
    //sEngine.initialize();
    //WAV huh = sEngine.loadWavFile("..\\assets\\ahem_x.wav", "ahem");
    //SDL_AudioDeviceID aDevice = sEngine.openAudioDevice(huh);
    GLuint monsterTexture = renderer.create2DBitMapTexture("..\\assets\\monster1.bmp");
    GLuint backgroundTexture = renderer.create2DBitMapTexture("..\\assets\\background.bmp");
    renderer.loadAllShaders();
    

    std::vector<Vertex> cubeVertices;
    cubeVertices.reserve(cubePos.size());
    for (size_t i = 0; i < cubePos.size(); ++i) {
        cubeVertices.emplace_back(cubePos[i], normals[i], texCoords[i]);
    }

    std::vector<Vertex> quadVertices;
    quadVertices.reserve(quadPos.size());
    for (size_t i = 0; i < quadPos.size(); ++i) {
        quadVertices.emplace_back(quadPos[i], quadNormals[i], quadTexCoords[i]);
    }
    glEnable(GL_DEPTH_TEST);
    //Object triangleObject(triangleVertices, 3, renderer.shaders["triangle"], 0);
    Object floor(cubeVertices, 36, renderer.shaders["quad3d"], backgroundTexture, cubeIndices);
    Object cubeObject(cubeVertices, 36, renderer.shaders["quad3d"], backgroundTexture, cubeIndices);
    std::vector<Object> objects;
    objects.push_back(cubeObject);
    objects.push_back(floor);
    objects[1].changeSize(glm::vec3(100.0f, 0.0f, 100.0f));
    objects[1].movePos(glm::vec3(-1.0f, -1.0f, -1.0f));
    objects[0].movePos(glm::vec3(1.0f, -0.5f, 1.0f));

    while (!glfwWindowShouldClose(window)) {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        processKeyboard(window); 
        bool grounded = false;      
        bool collided = false;
        glm::vec3 originalMovement = camera.movement;

        for (Object obj : objects) {
            grounded |= camera.isGrounded(obj.hitbox);
            obj.changeView(camera.GetViewMatrix());
            renderer.drawObject(obj);              
            if (Point_Box_Colission(obj.hitbox, camera.position, camera.movement)) {
                collided = true;
            }
        }
        if (collided) {
            camera.position += camera.movement;
        }
        else {
            camera.position += originalMovement;
        }

        if (grounded) {
            camera.movement = glm::vec3(0.0f);
        }


        printf("\n");
        camera.movement = glm::vec3(0.0f);
        camera.airborne = !grounded;
        camera.applyGravity(deltaTime);
        //printf("return of collission %d \n",cubeObject.hitbox.CheckCameraCollision(camera.position, camera.movement));     
        //std::cout<<cubeObject.hitbox.checkAllPointsInTriangles(camera.position);
        glfwPollEvents();
        glfwSwapBuffers(window);
    }

    for (Object i : objects) {
        i.Destroy();
    }
    //sEngine.audioCleanup(huh, aDevice);
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
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        //std::cout << "up";
        camera.ProcessKeyboard(UP, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
        //std::cout << "up";
        camera.ProcessKeyboard(DOWN, deltaTime);
    }
}

