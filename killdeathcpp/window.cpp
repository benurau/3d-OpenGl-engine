#include <iostream>
#include <functional>
#include "renderer.h"
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
#include "CollisionResponse.h"
#include "ColisionManager.h"
#include "Weapon.h"
#include "WeaponManager.h"
#include "GameState.h"
#include "TextRenderer.h"
#include "UIManager.h"
#include "MainMenuScreen.h"
#include "OptionsScreen.h"
#include "PauseScreen.h"
#include "CombatManager.h"
#include "attack.h"

enum class Game {
    START_SCREEN,
    GAME_SCREEN,
    PAUSE_SCREEN,
    DEATH_SCREEN
};

float deltaTime = 0.0f;
float lastFrame = 0.0f;
bool firstMouse = true;
float lastX = C_RES_WIDTH / 2.0;
float lastY = C_RES_HEIGHT / 2.0;
float mouseX = 0.0f, mouseY = 0.0f;
bool leftMousePressed = false;
bool rightMousePressed = false;
bool escapePressed = false;
Game game = Game::GAME_SCREEN;
Camera camera;

void errorCallback(int error, const char* description) {
    std::cerr << "Error: " << description << std::endl;
}

void processKeyboard(GLFWwindow* window, Player& player, WeaponManager& weaponManager);
void processMouse(GLFWwindow* window, UIManager& uiManager, WeaponManager& weaponManager, std::vector<Projectile>& projectiles);
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
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    Renderer renderer(window);
    TextRenderer textRenderer("C:/Windows/Fonts/arial.ttf");

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

    tinyModel gungltf = tinyModel("..\\models\\debug_fps_gun\\scene.gltf");
    gungltf.materialOffset = renderer.materials.size();
    for (GLTFMaterialGPU mat : gungltf.gpuMaterials) {
        renderer.materials.push_back(renderer.ConvertGLTFMaterialToMaterial(mat, &shaders["gltfModel"]));
    }

    tinyModel swordgltf = tinyModel("..\\models\\debug_sword\\scene.gltf");
    swordgltf.materialOffset = renderer.materials.size();
    for (GLTFMaterialGPU mat : swordgltf.gpuMaterials) {
        renderer.materials.push_back(renderer.ConvertGLTFMaterialToMaterial(mat, &shaders["gltfModel"]));
    }

    tinyModel meele_enemygltf = tinyModel("..\\models\\simple_meele_enemy\\scene.gltf");
    meele_enemygltf.materialOffset = renderer.materials.size();
    for (GLTFMaterialGPU mat : meele_enemygltf.gpuMaterials) {
        renderer.materials.push_back(renderer.ConvertGLTFMaterialToMaterial(mat, &shaders["gltfModel"]));
    }

    DirLight basicLight;

    Mesh cube(cubeVertices, cubeIndices);
    VerticeHitBox CubeVertHitbox;
    CubeVertHitbox.buildFromMesh(cubeVertices, cubeIndices);
    ObjectCollision defaultVertCollision;
    defaultVertCollision.setVerticeHitBox(CubeVertHitbox);
    defaultVertCollision.modelSpaceAABB = CubeVertHitbox.localAABB;


    MeshObject floor{cube, defaultObj, defaultVertCollision};

    MeshObject objectCube{ cube, defaultObj, defaultVertCollision };

    ModelObject pack = { packgltf, defaultObj };
    ModelObject mina = { minaglft, defaultObj };
    ModelObject skeleton = { skeletongltf, defaultObj };
    ModelObject gun = { gungltf, defaultObj };
    ModelObject sword = { swordgltf, defaultObj };
    ModelObject meeleEnemyObject = { meele_enemygltf, defaultObj };

    VerticeHitBox packvhb;
    packvhb.buildFromModel(pack.model.glMeshes, pack.model.nodes);
    pack.colission.setVerticeHitBox(packvhb);
    pack.colission.modelSpaceAABB = packvhb.localAABB;
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

    weapon gun_weapon{gun};
    weapon sword_weapon{ sword };

    WeaponManager weaponManager;
    weaponManager.addWeapon(gun_weapon);
    weaponManager.addWeapon(sword_weapon);

    Enemy basicEnemy{objectCube};
    basicEnemy.object.orientation.movePos(glm::vec3(-3.0f, -4.0f, 2.0f));


    ProjectileType basicProjectileType{ cube, 10.0f , 5.0f, 10.0f};
    basicEnemy.ptype = basicProjectileType;
    basicEnemy.attackRange = 2.0f;
    basicEnemy.state = CHASE;

    EnemyModel skeletonEnemy{ skeleton };
    skeletonEnemy.attackRange = 2.0f;
    skeletonEnemy.state = CHASE;
    skeletonEnemy.chaseAnimation = 0;
    skeletonEnemy.attackAnimation = 0;

    EnemyModel meeleEnemy{ meeleEnemyObject };
    meeleEnemy.object.orientation.movePos(glm::vec3(4.0f, -4.0f, 5.0f));
    meeleEnemy.attackRange = 2.0f;
    meeleEnemy.state = CHASE;
    meeleEnemy.chaseAnimation = 0;
    meeleEnemy.attackAnimation = 0;

    Projectile basicProjectile{ objectCube, basicProjectileType };
    basicProjectile.object.orientation.changeSize(glm::vec3(-0.9f));
    basicProjectile.object.colission.updateWorldAABB(basicProjectile.object.orientation.modelMatrix);

    Attack skeletonAttack{ skeletonEnemy.object, skeletonEnemy.id };
    skeletonAttack.type = AttackType::Projectile;
    skeletonAttack.animationIndex = 0;
    skeletonAttack.projectile = basicProjectile;
    skeletonEnemy.attack = skeletonAttack;

    Attack meleeEnemyAttack{ meeleEnemy.object, meeleEnemy.id };
    meleeEnemyAttack.type = AttackType::Meele;
    meleeEnemyAttack.damage = 20.0f;
    meleeEnemyAttack.animationIndex = 0;
    meleeEnemyAttack.meele = MeeleAttack{ meeleEnemy.object, meleeEnemyAttack.ownerId };
    meeleEnemy.attack = meleeEnemyAttack;

    Attack gunWeaponAttack = Attack{ gun_weapon.weaponObject, gun_weapon.ownerId };
    gunWeaponAttack.projectile = basicProjectile;
    gunWeaponAttack.type = AttackType::Projectile;
    gunWeaponAttack.animationIndex = 0;
    gun_weapon.attack = gunWeaponAttack;

    Attack meeleWeaponAttack = Attack{ sword_weapon.weaponObject, sword_weapon.ownerId };
    meeleWeaponAttack.damage = 20.0f;
    meeleWeaponAttack.type = AttackType::Meele;
    meeleWeaponAttack.animationIndex = 0;
    meeleWeaponAttack.meele = MeeleAttack{ sword_weapon.weaponObject , meeleWeaponAttack.ownerId};

    ProjectileManager projectileManager;
    projectileManager.AddProjectile(basicProjectile, 100);

    EnemyManager enemyManager;
    enemyManager.AddEnemy(basicEnemy);
    enemyManager.AddEnemy(meeleEnemy);
    //enemyManager.AddEnemy(skeletonEnemy);

    mina.orientation.movePos(glm::vec3(3.0f, -4.9f, 2.0f));
    mina.orientation.rotate(glm::vec3(90.0f, 3.5f, 2.0f));

    ColissionManager colMgr;

    CombatManager cmbMgr;

    SceneManager sceneManager;
    sceneManager.Add(floor);
    sceneManager.Add(pack);
    sceneManager.Add(mina);

    Light pointLight;
    pointLight.ambient = glm::vec3(0.2f, 0.2f, 0.2f);
    pointLight.diffuse = glm::vec3(0.8f, 0.8f, 0.8f);
    pointLight.specular = glm::vec3(1.0f, 1.0f, 1.0f);

    floor.orientation.changeSize(glm::vec3(100.0f, 0.0f, 100.0f));
    floor.orientation.movePos(glm::vec3(-1.0f, -5.0f, -1.0f));
    floor.colission.updateWorldAABBV(floor.orientation.modelMatrix);

    UIManager uiManager;

    std::function<void()> showMainMenu, showPause;

    showMainMenu = [&]() {
        game = Game::START_SCREEN;
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        uiManager.SetScreen(new MainMenuScreen(
            [&]() { game = Game::GAME_SCREEN; glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); },
            [&]() { uiManager.SetScreen(new OptionsScreen([&]() { showMainMenu(); })); },
            [&]() { glfwSetWindowShouldClose(window, true); }
        ));
    };

    showPause = [&]() {
        game = Game::PAUSE_SCREEN;
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        uiManager.SetScreen(new PauseScreen(
            [&]() { game = Game::GAME_SCREEN; glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); },
            [&]() { uiManager.SetScreen(new OptionsScreen([&]() { showPause(); })); },
            [&]() { showMainMenu(); }
        ));
    };

    //showMainMenu();
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    GameState gameState;
    gameState.Store(player, enemyManager);

    float deathTimer = 5.0f;

    glEnable(GL_DEPTH_TEST);
    while (!glfwWindowShouldClose(window)) {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        processMouse(window, uiManager, weaponManager, projectileManager.projectiles);
        processKeyboard(window, player, weaponManager);
        player.grounded = false;

        switch (game) {
        case Game::GAME_SCREEN: {
            bool escDown = glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS;
            if (escDown && !escapePressed) showPause();
            escapePressed = escDown;

            glm::vec3 originalMovement = player.movement;

            sceneManager.Update(deltaTime);

            enemyManager.Update(deltaTime, player.object.orientation.position, projectileManager);
            projectileManager.Update(deltaTime);

            weaponManager.Update(camera, renderer, colMgr, enemyManager, deltaTime);

            colMgr.CheckSceneCollision(player, sceneManager, camera.position);
            colMgr.CheckEnemyCollision(player, enemyManager, camera.position);
            cmbMgr.CollectActiveAttacks(weaponManager, enemyManager);
            cmbMgr.CheckMeeleSweeps(enemyManager, player);
            cmbMgr.CheckProjectileCollision(player, projectileManager);
            cmbMgr.CheckProjectileEnemyCollision(projectileManager, enemyManager);

            if (player.health <= 0.0f) {
                deathTimer = 5.0f;
                game = Game::DEATH_SCREEN;
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                gameState.Restore(player, enemyManager);
            }

            sceneManager.Render(renderer, silver, camera);

            enemyManager.Render(renderer, silver, camera);

            projectileManager.Render(renderer, silver, camera);

            updatePlayer(player, originalMovement, deltaTime);
            camera.position = player.object.orientation.position + glm::vec3(0, player.cameraHeight, 0);

            textRenderer.RenderText("HP: " + std::to_string(player.health), 20.0f, 60.0f, 1.0f, glm::vec3(1.0f, 0.2f, 0.2f));
            break;
        }

        case Game::START_SCREEN:
        case Game::PAUSE_SCREEN:
            uiManager.Update(deltaTime);
            uiManager.Render(textRenderer);
            break;

        case Game::DEATH_SCREEN:
            if (deathTimer <= 0) showMainMenu();
            textRenderer.RenderText("YOU DIED PUSSY", 400.0f, 400.0f, 1.5f, glm::vec3(0.0f, 1.0f, 0.0f));
            deathTimer -= deltaTime;
            break;
        }

        glfwPollEvents();
        glfwSwapBuffers(window);         
    }
    glfwTerminate();

    return 0;
}

void mouseCallback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);
    mouseX = xpos;
    mouseY = ypos;
    if (game == Game::GAME_SCREEN)
    {
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
    else
    {
        firstMouse = true;
    }
}

void processMouse(GLFWwindow* window, UIManager& uiManager, WeaponManager& weaponManager, std::vector<Projectile>& projectiles) {
    bool leftClick = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
    if (leftClick && !leftMousePressed) {
        uiManager.OnMouseClick(mouseX, mouseY);
    }
    leftMousePressed = leftClick;
    uiManager.OnMouseMove(mouseX, mouseY);
    weapon* w = weaponManager.getActiveWeapon();
    if (w && game == Game::GAME_SCREEN && leftClick) {
        w->fire(projectiles);
    }
    //bool rightClick = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS;
    //if (rightClick && !rightMousePressed && game == Game::GAME_SCREEN) {
    //    if (w && (w->type == weapon::WeaponType::Meele || w->type == weapon::WeaponType::Both))
    //        w->meeleAttack.startMeele(w->weaponObject);
    //}
    //rightMousePressed = rightClick;
}

void processKeyboard(GLFWwindow* window, Player& player, WeaponManager& weaponManager) {
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
        PlayerDash(player, camera, window, deltaTime);
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
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
        weaponManager.setActiveWeapon(0);
    }
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
        weaponManager.setActiveWeapon(1);
    }
    if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) {
        weaponManager.setActiveWeapon(2);
    }
}
