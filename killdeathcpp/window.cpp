#include <iostream>
#include "renderer.h"
#include "soundEngine.h"
#include "Camera.h"
#include "ObjectParams.h"
#include "Colissions.h"
#include "Lights.h"
#include "model.h"




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

    GLuint monster = create2DBitMapTexture("..\\assets\\monster1.bmp");
    GLuint background = create2DBitMapTexture("..\\assets\\background.bmp");

    std::unordered_map <std::string, Shader> shaders;
    shaders["quad3d"] = Shader("..\\shaders\\quad3d.vs", "..\\shaders\\quad3d.fs");
    shaders["lightcube"] = Shader("..\\shaders\\lightcube.vs", "..\\shaders\\lightcube.fs");
    shaders["basiclighting"] = Shader("..\\shaders\\basiclighting.vs", "..\\shaders\\basiclighting.fs");
    shaders["materialLighting"] = Shader("..\\shaders\\basiclighting.vs", "..\\shaders\\materialLighting.fs");
    shaders["textureLighting"] = Shader("..\\shaders\\lightingMap.vs", "..\\shaders\\lightingMap.fs");
    shaders["model_Load"] = Shader("..\\shaders\\model_load.vs", "..\\shaders\\model_load.fs");
    shaders["animation"] = Shader("..\\shaders\\animation.vs", "..\\shaders\\animation.fs");

    Texture scarywall = { background, "diffusion1", "..\\assets\\background.bmp" };

    Material silver = Material(&shaders["materialLighting"]);
    Material TextureLight = Material(&shaders["textureLighting"]);
    Material basic = Material(&shaders["quad3d"]);
    TextureLight.textureUniforms["material.texture_diffuse1"] = scarywall;

    Model glock = Model("..\\models\\glock\\scene.gltf");
    Model backpack = Model("..\\models\\backpack\\backpack.gltf");
    Model chair = Model("..\\models\\chair\\scene.gltf");
    Model mina = Model("..\\models\\mina\\scene.gltf");

    DirLight basicLight;

    Mesh floor(cubeVertices, 36, TextureLight, cubeIndices);
    Mesh cubeObject(cubeVertices, 36, silver, cubeIndices);
    Mesh lightingCube(cubeVertices, 36, silver, cubeIndices);
    Mesh basicLightingObject(cubeVertices, 36, silver, cubeIndices);
    Mesh materialLightingObject(cubeVertices, 36, silver, cubeIndices);
    Mesh textureLightingObject(cubeVertices, 36, silver, cubeIndices);

    Light pointLight;
    pointLight.ambient = glm::vec3(0.2f, 0.2f, 0.2f);
    pointLight.diffuse = glm::vec3(0.8f, 0.8f, 0.8f);
    pointLight.specular = glm::vec3(1.0f, 1.0f, 1.0f);

    std::vector<Mesh*> meshes;
    meshes.emplace_back(&cubeObject);
    meshes.emplace_back(&floor);
    meshes.emplace_back(&lightingCube);
    meshes.emplace_back(&textureLightingObject);

    floor.changeSize(glm::vec3(100.0f, 0.0f, 100.0f));
    floor.movePos(glm::vec3(-1.0f, -1.0f, -1.0f));
    cubeObject.movePos(glm::vec3(1.0f, -0.5f, 1.0f));
    lightingCube.movePos(glm::vec3(2.0f, -0.5f, 1.0f));
    basicLightingObject.movePos(glm::vec3(1.0f, -0.5f, 6.0f));
    materialLightingObject.movePos(glm::vec3(1.0f, -0.5f, 3.0f));
    textureLightingObject.movePos(glm::vec3(1.0f, -0.5f, 4.0f));

    backpack.movePos(glm::vec3(-3.0f, -0.5f, -0.5f));
    chair.changeSize(glm::vec3(2.0f, 2.0f, 2.0f));
    chair.movePos(glm::vec3(3.0f, -0.5f, -0.5f));
    mina.movePos(glm::vec3(5.0f, -0.5f, -0.5f));

    shaders["textureLighting"].use();
    shaders["textureLighting"].setVec3("light.position", glm::vec3(1.0f, 0.5f, 1.0f));
    shaders["textureLighting"].setVec3("light.ambient", glm::vec3(0.2f, 0.2f, 0.2f));
    shaders["textureLighting"].setVec3("light.diffuse", glm::vec3(0.5f, 0.5f, 0.5f));
    shaders["textureLighting"].setVec3("light.specular", glm::vec3(1.0f, 1.0f, 1.0f));
    shaders["textureLighting"].setVec3("material.specular", glm::vec3(0.5f, 0.5f, 0.5f));
    shaders["textureLighting"].setFloat("material.shininess", 32.0f);

    glm::vec3 lightColor = glm::vec3(0.6f, 1.2f, 2.2f);
    glm::vec3 diffuseColor = lightColor * glm::vec3(0.5f);
    glm::vec3 ambientColor = diffuseColor * glm::vec3(0.2f);

    basicLight.direction = lightingCube.orientation.position;
    basicLight.ambient = ambientColor;
    basicLight.diffuse = diffuseColor;
    basicLight.specular = glm::vec3(1.0f, 1.0f, 1.0f);
    


    silver.vec3Uniforms["material.ambient"]  = glm::vec3(1.0f, 0.5f, 0.31f);
    silver.vec3Uniforms["material.diffuse"]  = glm::vec3(1.0f, 0.5f, 0.31f);
    silver.vec3Uniforms["material.specular"] = glm::vec3(0.5f, 0.5f, 0.5f);
    silver.floatUniforms["material.shininess"] = 32.0f;
    glEnable(GL_DEPTH_TEST);
    while (!glfwWindowShouldClose(window)) {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        processKeyboard(window); 
        bool grounded = false;      
        bool collided = false;
        glm::vec3 originalMovement = camera.movement;    

        backpack.orientation.changeView(camera.GetViewMatrix());
        backpack.Draw(shaders["model_Load"]);
        mina.orientation.changeView(camera.GetViewMatrix());

        mina.Draw(shaders["animation"]);
		//glock.orientation.changeView(camera.GetViewMatrix());
		//glock.Draw(shaders["animation"]);

        materialLightingObject.orientation.changeView(camera.GetViewMatrix());
        basicLight.SetLightUniforms(shaders["materialLighting"], "light");
        renderer.draw(materialLightingObject, silver);

        if (basic_AABB_Colission(backpack.coarse_AABB, camera.position, camera.movement)) {
            collided = CheckModelCollision(backpack, camera);
            grounded = checkGroundedOnMeshes(backpack, camera);
        }
        for (Mesh* mesh : meshes) {
            grounded |= camera.isGrounded(mesh->hitbox);
            mesh->orientation.changeView(camera.GetViewMatrix());
            renderer.draw(*mesh, mesh->material);

            if (point_Box_Colission(mesh->hitbox, camera.position, camera.movement)) {
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

        camera.movement = glm::vec3(0.0f);
        camera.airborne = !grounded;
        camera.applyGravity(deltaTime);
        glfwPollEvents();
        glfwSwapBuffers(window);
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

