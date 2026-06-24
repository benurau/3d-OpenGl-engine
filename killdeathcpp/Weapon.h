#pragma once
#include "objects.h"
#include "Projectile.h"
#include "Renderer.h"
#include "Colissions.h"

struct weapon {
    ModelObject& weaponObject;
    ProjectileType projectileT;
    float shootTimer = 0.0f;
    float shootCooldown = 0.25f;

    static inline int nextId = 0;
    int id = nextId++;

    enum class WeaponType { Melee, Range, Both };
    WeaponType type = WeaponType::Range;

    enum class MeleeState { Idle, Swinging, Cooldown };
    MeleeState meleeState = MeleeState::Idle;
    float swingTimer = 0.0f;
    float swingDuration = 0.35f;
    float meleeCooldownTimer = 0.0f;
    float meleeCooldown = 0.5f;
    float meleeDamage = 30.0f;
    float meleeRadius = 0.3f;
    int meleeSwingAnimIndex = 0;

    glm::vec3 localMeshTip{ 0.0f };
    glm::vec3 prevWorldTip{ 0.0f };
    bool hasPrevTip = false;
    std::vector<int> hitEnemyIds;

    weapon(ModelObject& obj) : weaponObject(obj) {
        findTip();
    }

    void findTip() {
        tinyModel& model = weaponObject.model;
        float maxDist = 0.0f;
        for (Node& node : model.nodes) {
            if (node.glMeshIndex < 0) continue;
            Mesh& mesh = model.glMeshes[node.glMeshIndex];
            for (Vertex& v : mesh.vertices) {
                glm::vec3 p = glm::vec3(node.globalMatrix * glm::vec4(v.position, 1.0f));
                float dist = glm::length(p);
                if (dist > maxDist) {
                    maxDist = dist;
                    localMeshTip = v.position;
                }
            }
        }
    }

    glm::vec3 getCurrentTipWorld() {
        tinyModel& model = weaponObject.model;
        glm::vec3 modelLocal = localMeshTip;
        for (Node& node : model.nodes) {
            if (node.glMeshIndex >= 0) {
                modelLocal = glm::vec3(node.globalMatrix * glm::vec4(localMeshTip, 1.0f));
                break;
            }
        }
        return glm::vec3(weaponObject.orientation.modelMatrix * glm::vec4(modelLocal, 1.0f));
    }

    void startMelee() {
        if (meleeState != MeleeState::Idle) return;
        meleeState = MeleeState::Swinging;
        swingTimer = 0.0f;
        hasPrevTip = false;
        hitEnemyIds.clear();
        weaponObject.model.setAnimation(meleeSwingAnimIndex, true);
    }

    void Update(Camera& camera, Renderer& renderer, float deltaTime)
    {
        shootTimer -= deltaTime;

        glm::vec3 pos = camera.position + camera.Right * 0.15f + camera.Up * -0.15f + camera.Front * 0.25f;
        glm::mat4 rot(1.0f);
        rot[0] = glm::vec4(camera.Front, 0.0f);
        rot[1] = glm::vec4(camera.Up, 0.0f);
        rot[2] = glm::vec4(camera.Right, 0.0f);

        weaponObject.orientation.modelMatrix = glm::translate(glm::mat4(1.0f), pos) * rot;
        weaponObject.orientation.position = pos;

        weaponObject.model.updateAnimation(deltaTime, false);
        weaponObject.model.updateNodeTransforms();
        weaponObject.orientation.changeView(camera.GetViewMatrix());
        renderer.drawModel(weaponObject.model, weaponObject.orientation);

        updateMelee(deltaTime);
    }

    void updateMelee(float deltaTime) {
        switch (meleeState) {
        case MeleeState::Swinging:
            swingTimer += deltaTime;
            if (swingTimer >= swingDuration) {
                meleeState = MeleeState::Cooldown;
                meleeCooldownTimer = meleeCooldown;
                hasPrevTip = false;
                hitEnemyIds.clear();
            }
            break;
        case MeleeState::Cooldown:
            meleeCooldownTimer -= deltaTime;
            if (meleeCooldownTimer <= 0.0f)
                meleeState = MeleeState::Idle;
            break;
        }
    }

    bool updateSweepCapsule() {
        if (meleeState != MeleeState::Swinging) return false;
        glm::vec3 currentTip = getCurrentTipWorld();
        if (!hasPrevTip) {
            prevWorldTip = currentTip;
            hasPrevTip = true;
            return false;
        }
        CapsuleWorldLoc cap;
        cap.p0 = prevWorldTip;
        cap.p1 = currentTip;
        cap.radius = meleeRadius;
        weaponObject.colission.setSweepCapsule(cap);
        prevWorldTip = currentTip;
        return true;
    }

    void fire(std::vector<Projectile>& projectiles)
    {
        if (shootTimer > 0.0f) return;
        weaponObject.model.setAnimation(0, true);
        glm::vec3 forward = glm::normalize(glm::vec3(weaponObject.orientation.modelMatrix[0]));
        SpawnProjectile(weaponObject.orientation.position, forward, projectileT, projectiles);
        shootTimer = shootCooldown;
    }
};
