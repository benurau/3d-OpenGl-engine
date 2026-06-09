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
#include "Player.h"
#include "EnemyManager.h"
#include "ProjectileManager.h"
#include "SceneManager.h"

float deltaTime = 0.0f;
float lastFrame = 0.0f;
bool firstMouse = true;
float lastX = C_RES_WIDTH / 2.0;
float lastY = C_RES_HEIGHT / 2.0;
Camera camera;


void errorCallback(int error, const char* description) {
    std::cerr << "Error: " << description << std::endl;
}

void processKeyboard(GLFWwindow* window, Player& player);
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
    shaders["debugshader"] = Shader("..\\shaders\\debugShader.vs", "..\\shaders\\debugShader.fs");

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

    tinyModel skeletongltf = tinyModel("..\\models\\skeleton\\scene.gltf");
    skeletongltf.materialOffset = renderer.materials.size();
    for (GLTFMaterialGPU mat : skeletongltf.gpuMaterials) {
        renderer.materials.push_back(renderer.ConvertGLTFMaterialToMaterial(mat, &shaders["gltfModelSkinned"]));
    }

    tinyModel packgltf = tinyModel("..\\models\\backpack\\scene.gltf");
    packgltf.materialOffset = renderer.materials.size();
    for (GLTFMaterialGPU mat : packgltf.gpuMaterials) {
        renderer.materials.push_back(renderer.ConvertGLTFMaterialToMaterial(mat, &shaders["gltfModel"]));
    }

    DirLight basicLight;

    Mesh cube(cubeVertices, cubeIndices);
    VerticeHitBox CubeVertHitbox;
    CubeVertHitbox.buildFromMesh(cubeVertices, cubeIndices);
    ObjectCollision defaultVertCollision;
    defaultVertCollision.vHitbox = CubeVertHitbox;

    MeshObject floor{cube, defaultObj, defaultVertCollision};

    MeshObject objectCube{ cube, defaultObj, defaultVertCollision };

    ModelObject pack = { packgltf, defaultObj };
    ModelObject mina = { minaglft, defaultObj };
    ModelObject skeleton = { skeletongltf, defaultObj };

    VerticeHitBox packvhb;
    packvhb.buildFromModel(pack.model.glMeshes, pack.model.nodes);
    pack.colission.vHitbox = packvhb;
    pack.orientation.movePos(glm::vec3(0.0f, -3.0f, 2.0f));
    pack.colission.updateWorldAABBV(pack.orientation.modelMatrix);
    

    Player player;
    MeshObject playerObject = objectCube;
    playerObject.orientation.changeSize(glm::vec3(0.5f, 1.0f, 0.5f));
    playerObject.colission.updateWorldAABB(playerObject.orientation.modelMatrix);
    player.object = playerObject;

    skeleton.orientation.rotate(glm::vec3(-90.0f, 0.0f, 2.0f));
    skeleton.orientation.movePos(glm::vec3(6.0f, -4.0f, 2.0f));
    skeleton.orientation.changeSize(glm::vec3(-0.99f, -0.99f, -0.99f));

    mina.model.setAnimation(0);


    Enemy basicEnemy{objectCube};

    ProjectileType basicProjectileType{ cube, 10.0f , 3.0f};
    basicEnemy.ptype = basicProjectileType;
    basicEnemy.attackRange = 2.0f;
    basicEnemy.state = CHASE;

    EnemyModel skeletonEnemy{ skeleton };
    skeletonEnemy.attackRange = 2.0f;
    skeletonEnemy.state = CHASE;
    skeletonEnemy.attackAnimation = 0;
    skeletonEnemy.chaseAnimation = -1;

    Projectile basicProjectile{ objectCube, basicProjectileType };
    basicProjectile.object.orientation.changeSize(glm::vec3(-0.9f));
    basicProjectile.object.colission.updateWorldAABB(basicProjectile.object.orientation.modelMatrix);

    ProjectileManager projectileManager;
    projectileManager.AddProjectile(basicProjectile, 100);

    EnemyManager enemyManager;
    enemyManager.AddEnemy(basicEnemy);
    enemyManager.AddEnemy(skeletonEnemy);

    mina.orientation.movePos(glm::vec3(3.0f, -4.9f, 2.0f));
    mina.orientation.rotate(glm::vec3(90.0f, 3.5f, 2.0f));

    SceneManager sceneManager;
    sceneManager.Add(floor);
    sceneManager.Add(pack);
    sceneManager.Add(mina);



    Light pointLight;
    pointLight.ambient = glm::vec3(0.2f, 0.2f, 0.2f);
    pointLight.diffuse = glm::vec3(0.8f, 0.8f, 0.8f);
    pointLight.specular = glm::vec3(1.0f, 1.0f, 1.0f);

    std::vector<MeshObject*> objects;
    objects.emplace_back(&floor);

    floor.orientation.changeSize(glm::vec3(100.0f, 0.0f, 100.0f));
    floor.orientation.movePos(glm::vec3(-1.0f, -5.0f, -1.0f));
    floor.colission.updateWorldAABBV(floor.orientation.modelMatrix);

    glEnable(GL_DEPTH_TEST);
    while (!glfwWindowShouldClose(window)) {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        processKeyboard(window, player); 
        bool grounded = false;      
        bool collided = false;
        glm::vec3 originalMovement = player.movement;    
      
        renderer.drawAABB(pack.colission.worldAABB, pack.orientation.proj * pack.orientation.view, glm::vec3(1.0f, 1.0f, 0.0f), shaders["debugshader"]);
        if (AABBPointColission(pack.colission.worldAABB, player.object.orientation.position + player.movement)) {
            ShapeContact contanct = pointVertBoxCollision(pack.colission.vHitbox, player.object.orientation.position + player.movement);
            if (contanct.isColliding) {
                player.movement += contanct.normal * contanct.penetrationDepth;
            }
        }

        if (AABBPointColission(mina.colission.worldAABB, player.object.orientation.position + player.movement)) {
            for (CapsuleHitBoxWorld& box : mina.colission.capsuleLocs) {
                ShapeContact cContact = pointInCapsule(camera.position + player.movement, box.worldLoc);
                if (cContact.isColliding) {
                    glm::vec3 offsetVec = cContact.penetrationDepth * cContact.normal;
                    player.movement += offsetVec;
                    break;
                }
            }
        }

        sceneManager.Update(deltaTime);
        sceneManager.Render(renderer, silver, camera);

        enemyManager.Update(deltaTime, player.object.orientation.position, projectileManager);
        enemyManager.Render(renderer, silver, camera);

        projectileManager.Update(deltaTime);
        projectileManager.Render(renderer, silver, camera);
        projectileManager.CheckPlayerCollision(player);


        for (MeshObject* object : objects) {           
            ShapeContact tempContact = pointVertBoxCollision(object->colission.vHitbox, player.object.orientation.position + player.movement);
            if (tempContact.isColliding) {
                collided = true;
                player.movement += tempContact.normal * tempContact.penetrationDepth;
                grounded |= isGrounded(tempContact, player.object.colission.worldAABB.min.y);
            }
        }
        updatePlayer(collided, grounded, player, originalMovement, deltaTime);
        camera.position = player.object.orientation.position + glm::vec3(0, player.cameraHeight, 0);
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

void processKeyboard(GLFWwindow* window, Player& player) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        ProcessViewControls(player, FORWARD, camera, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        ProcessViewControls(player, BACKWARD, camera, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        ProcessViewControls(player, LEFT, camera, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        ProcessViewControls(player, RIGHT, camera, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        ProcessViewControls(player, UP, camera, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
        ProcessViewControls(player, DOWN, camera, deltaTime);
    }
}

