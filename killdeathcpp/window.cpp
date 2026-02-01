#include <iostream>
#include "renderer.h"
#include "soundEngine.h"
#include "Camera.h"
#include "ObjectParams.h"
#include "Colissions.h"
#include "objects.h"
#include "Lights.h"
#include "tinyModel.h"
#include "Mesh.h"

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
    shaders["gltfModel"] = Shader("..\\shaders\\gltfModel.vs", "..\\shaders\\gltfModel.fs");
    shaders["gltfModelSkinned"] = Shader("..\\shaders\\modelAnimation.vs", "..\\shaders\\modelAnimation.fs");

    ObjectOrientation defaultObj = ObjectOrientation();

    Texture scarywall = { background, "diffusion1", "..\\assets\\background.bmp" };
    Texture monsterT = { monster, "diffusion1", "..\\assets\\monster1.bmp" };

    Material silver = Material(&shaders["textureLighting"]);
    Material TextureLight = Material(&shaders["textureLighting"]);
    Material basic = Material(&shaders["quad3d"]);

    TextureLight.textureUniforms["material.texture_diffuse1"] = scarywall;
    shaders["textureLighting"].use();
    shaders["textureLighting"].setVec3("light.position", glm::vec3(1.0f, 0.5f, 1.0f));
    shaders["textureLighting"].setVec3("light.ambient", glm::vec3(0.2f, 0.2f, 0.2f));
    shaders["textureLighting"].setVec3("light.diffuse", glm::vec3(0.5f, 0.5f, 0.5f));
    shaders["textureLighting"].setVec3("light.specular", glm::vec3(1.0f, 1.0f, 1.0f));
    shaders["textureLighting"].setVec3("material.specular", glm::vec3(0.5f, 0.5f, 0.5f));
    shaders["textureLighting"].setFloat("material.shininess", 32.0f);
    silver.textureUniforms["material.texture_diffuse1"] = scarywall;
    silver.vec3Uniforms["material.ambient"] = glm::vec3(1.0f, 0.5f, 0.31f);
    silver.vec3Uniforms["material.diffuse"] = glm::vec3(1.0f, 0.5f, 0.31f);
    silver.vec3Uniforms["material.specular"] = glm::vec3(0.5f, 0.5f, 0.5f);
    silver.floatUniforms["material.shininess"] = 32.0f;

    tinyModel minaglft = tinyModel("..\\models\\mina\\scene.gltf");
    minaglft.materialOffset = renderer.materials.size();
    for (GLTFMaterialGPU mat : minaglft.gpuMaterials) {
        renderer.materials.push_back(renderer.ConvertGLTFMaterialToMaterial(mat, &shaders["gltfModelSkinned"]));
    }

    tinyModel packgltf = tinyModel("..\\models\\backpack\\scene.gltf");
    packgltf.materialOffset = renderer.materials.size();
    for (GLTFMaterialGPU mat : packgltf.gpuMaterials) {
        renderer.materials.push_back(renderer.ConvertGLTFMaterialToMaterial(mat, &shaders["gltfModel"]));
    }

    DirLight basicLight;
    Mesh cube(cubeVertices, cubeIndices);

    MeshObject floor{cube, defaultObj, VerticeHitBox(cube.vertices, cube.indices, defaultObj.modelMatrix)};
    

    ModelObject pack = { packgltf, defaultObj };
    ModelObject mina = { minaglft, defaultObj };
    pack.orientation.movePos(glm::vec3(0.0f, -3.0f, 2.0f));

    pack.colission.updateModelAABBnodes(pack.model);
    pack.colission.updateWorldAABB(pack.orientation.modelMatrix);

    mina.orientation.movePos(glm::vec3(3.0f, -4.9f, 2.0f));
    mina.orientation.rotate(glm::vec3(90.0f, 3.5f, 2.0f));

    Light pointLight;
    pointLight.ambient = glm::vec3(0.2f, 0.2f, 0.2f);
    pointLight.diffuse = glm::vec3(0.8f, 0.8f, 0.8f);
    pointLight.specular = glm::vec3(1.0f, 1.0f, 1.0f);

    std::vector<MeshObject*> objects;
    objects.emplace_back(&floor);

    floor.orientation.changeSize(glm::vec3(100.0f, 0.0f, 100.0f));
    floor.orientation.movePos(glm::vec3(-1.0f, -5.0f, -1.0f));
    floor.hitbox.updateModelMatrix(floor.orientation.modelMatrix);


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

        pack.orientation.changeView(camera.GetViewMatrix());
        renderer.drawModel(pack.model, pack.orientation);
        //printf("modelspace pack min\n");
        //printVec3(pack.colission.worldAABB.min);
        //printf("modelspace pack max\n");
        //printVec3(pack.colission.worldAABB.max);
        if (AABBPointColission(pack.colission.worldAABB, camera.position + camera.movement)) {
            //printf("gfsdgfdgfds");
        }

        mina.model.updateAnimation(deltaTime);
        mina.model.updateNodeTransforms();
        mina.model.updateSkins();
        mina.colission.updateModelAABBskins(mina.model);
        mina.colission.updateWorldAABB(mina.orientation.modelMatrix);
        mina.orientation.changeView(camera.GetViewMatrix());
        renderer.drawModel(mina.model, mina.orientation);

        if (AABBPointColission(mina.colission.worldAABB, camera.position+camera.movement)) {
            printf("gfsdgfdgfds");
        }


        for (MeshObject* object : objects) {
            grounded |= camera.isGrounded(object->hitbox.aabb);
            object->orientation.changeView(camera.GetViewMatrix());
            renderer.draw(object->mesh, object->orientation, silver);

            if (point_Box_Colission(object->hitbox, camera.position, camera.movement)) {
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

