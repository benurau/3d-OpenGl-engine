#include <iostream>
#include "renderer.h"
#include "soundEngine.h"
#include "Camera.h"
#include "ObjectParams.h"
#include "Colissions.h"
#include "Lights.h"
#include "Model.h"
#include "tinyModel.h"
#include "Mesh.h"


struct MeshObject {
    Mesh mesh;
    ObjectOrientation orientation;
    HitBox hitbox;
};


struct ModelObject {
    tinyModel& model;
    ObjectOrientation orientation;
};

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

    Material testmat = Material(&shaders["gltfModel"]);

    testmat.textureUniforms["uBaseColorTex"] = monsterT;
    testmat.vec4Uniforms["uBaseColorFactor"] = glm::vec4(1.0f);


    TextureLight.textureUniforms["material.texture_diffuse1"] = scarywall;
    silver.textureUniforms["material.texture_diffuse1"] = scarywall;

    Model glock = Model("..\\models\\glock\\scene.gltf");
    Model backpack = Model("..\\models\\backpack\\backpack.gltf");
    Model chair = Model("..\\models\\chair\\scene.gltf");


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

    /*tinyModel pack = tinyModel("..\\models\\backpack\\backpack.gltf");
    pack.materialOffset = renderer.materials.size();
    for (GLTFMaterialGPU mat : pack.gpuMaterials) {
        renderer.materials.push_back(renderer.ConvertGLTFMaterialToMaterial(mat, &shaders["gltfModel"]));
    }*/

    DirLight basicLight;


    Mesh cube(cubeVertices, cubeIndices);

    MeshObject floor{cube, defaultObj, HitBox(cube.vertices, cube.indices, defaultObj.modelMatrix)};
    MeshObject cubeObject{ cube, defaultObj, HitBox(cube.vertices, cube.indices, defaultObj.modelMatrix) };
    MeshObject lightingCube{ cube, defaultObj, HitBox(cube.vertices, cube.indices, defaultObj.modelMatrix) };
    MeshObject textureLightingObject{ cube, defaultObj, HitBox(cube.vertices, cube.indices, defaultObj.modelMatrix) };
    MeshObject basicLightingObject{ cube, defaultObj, HitBox(cube.vertices, cube.indices, defaultObj.modelMatrix) };
    MeshObject materialLightingObject{ cube, defaultObj, HitBox(cube.vertices, cube.indices, defaultObj.modelMatrix) };

    MeshObject testobject{ cube, defaultObj, HitBox(cube.vertices, cube.indices, defaultObj.modelMatrix) };
    testobject.orientation.movePos(glm::vec3(1.0f, -3.0f, 2.0f));
    testobject.hitbox.updateModelMatrix(basicLightingObject.orientation.modelMatrix);

    /*ModelObject minaObject{mina, defaultObj, };*/
    ModelObject pack = { packgltf, defaultObj };
    ModelObject mina = { minaglft, defaultObj };
    pack.orientation.movePos(glm::vec3(3.0f, -3.5f, 2.0f));
    mina.orientation.movePos(glm::vec3(3.0f, -4.9f, 2.0f));
    mina.orientation.rotate(glm::vec3(90.0f, 3.5f, 2.0f));

    Light pointLight;
    pointLight.ambient = glm::vec3(0.2f, 0.2f, 0.2f);
    pointLight.diffuse = glm::vec3(0.8f, 0.8f, 0.8f);
    pointLight.specular = glm::vec3(1.0f, 1.0f, 1.0f);

    std::vector<MeshObject*> objects;
    objects.emplace_back(&cubeObject);
    objects.emplace_back(&floor);
    objects.emplace_back(&lightingCube);
    objects.emplace_back(&textureLightingObject);

    floor.orientation.changeSize(glm::vec3(100.0f, 0.0f, 100.0f));
    floor.orientation.movePos(glm::vec3(-1.0f, -5.0f, -1.0f));
    floor.hitbox.updateModelMatrix(floor.orientation.modelMatrix);
    cubeObject.orientation.movePos(glm::vec3(1.0f, -0.5f, 1.0f));
    cubeObject.hitbox.updateModelMatrix(cubeObject.orientation.modelMatrix);
    lightingCube.orientation.movePos(glm::vec3(2.0f, -0.5f, 1.0f));
    lightingCube.hitbox.updateModelMatrix(lightingCube.orientation.modelMatrix);
    basicLightingObject.orientation.movePos(glm::vec3(1.0f, -0.5f, 6.0f));
    basicLightingObject.hitbox.updateModelMatrix(basicLightingObject.orientation.modelMatrix);
    materialLightingObject.orientation.movePos(glm::vec3(1.0f, -0.5f, 3.0f));
    materialLightingObject.hitbox.updateModelMatrix(materialLightingObject.orientation.modelMatrix);
    textureLightingObject.orientation.movePos(glm::vec3(1.0f, -0.5f, 4.0f));
    textureLightingObject.hitbox.updateModelMatrix(textureLightingObject.orientation.modelMatrix);

    backpack.movePos(glm::vec3(-3.0f, -0.5f, -0.5f));
    chair.changeSize(glm::vec3(2.0f, 2.0f, 2.0f));
    chair.movePos(glm::vec3(3.0f, -0.5f, -0.5f));

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
    int first = 0;
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

        /*pack.orientation.changeView(camera.GetViewMatrix());
        renderer.drawModel(pack.model, pack.orientation);*/

        if (!first){
            mina.model.updateAnimation(deltaTime);
            mina.model.updateNodeTransforms();
            mina.model.updateSkins();
        }
        mina.orientation.changeView(camera.GetViewMatrix());
        first = 0;

        renderer.drawModel(mina.model, mina.orientation);

        testobject.orientation.changeView(camera.GetViewMatrix());
        renderer.draw(testobject.mesh, testobject.orientation, testmat);

        materialLightingObject.orientation.changeView(camera.GetViewMatrix());
        basicLight.SetLightUniforms(shaders["materialLighting"], "light");
        renderer.draw(materialLightingObject.mesh, materialLightingObject.orientation, TextureLight);

        if (basic_AABB_Colission(backpack.coarse_AABB, camera.position, camera.movement)) {
            collided = CheckModelCollision(backpack, camera);
            grounded = checkGroundedOnMeshes(backpack, camera);
        }
        for (MeshObject* object : objects) {
            grounded |= camera.isGrounded(object->hitbox);
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

