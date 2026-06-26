#pragma once
#include "objects.h"



struct MeeleAttack {
    enum class MeleeState { Idle, Swinging, Cooldown };
    MeleeState meleeState = MeleeState::Idle;
    float swingTimer = 0.0f;
    float swingDuration = 0.35f;
    float meleeCooldownTimer = 0.0f;
    float meleeCooldown = 0.5f;
    float meleeDamage = 30.0f;
    float meleeRadius = 0.3f;
    int meleeSwingAnimIndex = 0;

    int id = 0;

    glm::vec3 localMeshTip{ 0.0f };
    glm::vec3 prevWorldTip{ 0.0f };
    bool hasPrevTip = false;

    CapsuleWorldLoc capsule;

    std::vector<int> hitEnemyIds;



    MeeleAttack(ModelObject& attackObject, int ownerId) {
        findTip(attackObject); id = ownerId;
    }

    void findTip(ModelObject& attackObject) {
        tinyModel& model = attackObject.model;
        float maxDist = 0.0f;
        for (Node& node : model.nodes) {
            if (node.glMeshIndex < 0) continue;
            Mesh& mesh = model.glMeshes[node.glMeshIndex];
            for (Vertex& v : mesh.vertices) {
                glm::vec3 p = glm::vec3(node.globalMatrix * glm::vec4(v.position, 1.0f));
                float dist = glm::length(p);
                if (dist > maxDist) {
                    maxDist = dist;
                    localMeshTip = p;
                }
            }
        }
    }

    glm::vec3 getCurrentTipWorld(ModelObject& attackObject)
    {
        return glm::vec3(attackObject.orientation.modelMatrix * glm::vec4(localMeshTip, 1.0f));
    }

    void startMelee(ModelObject& attackObject) {
        if (meleeState != MeleeState::Idle) return;
        meleeState = MeleeState::Swinging;
        swingTimer = 0.0f;
        hasPrevTip = false;
        hitEnemyIds.clear();
        attackObject.model.setAnimation(meleeSwingAnimIndex, true);
    }

    void updateMelee(float deltaTime, ModelObject& attackObject) {
        switch (meleeState) {
        case MeleeState::Swinging:
            swingTimer += deltaTime;
            updateSweepCapsule(attackObject);
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

    bool updateSweepCapsule(ModelObject& attackObject) {
        if (meleeState != MeleeState::Swinging) return false;
        glm::vec3 currentTip = getCurrentTipWorld(attackObject);
        if (!hasPrevTip) {
            prevWorldTip = currentTip;
            capsule.p0 = currentTip;
            capsule.p1 = currentTip;
            capsule.radius = meleeRadius;
            hasPrevTip = true;
            return false;
        }
        capsule.p0 = prevWorldTip;
        capsule.p1 = currentTip;
        capsule.radius = meleeRadius;
        prevWorldTip = currentTip;
        return true;
    }
};