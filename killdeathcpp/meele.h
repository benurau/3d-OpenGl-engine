#pragma once
#include "objects.h"



struct MeeleAttack {
    enum class MeeleState { Idle, Swinging, Cooldown };
    MeeleState meeleState = MeeleState::Idle;
    float swingTimer = 0.0f;
    float swingDuration = 0.35f;
    float meeleRadius = 0.3f;

    int ownerId;

    glm::vec3 localMeshTip{ 0.0f };
    glm::vec3 prevWorldTip{ 0.0f };
    bool hasPrevTip = false;

    CapsuleWorldLoc capsule;
    CapsuleWorldLoc hiltCapsule;

    std::vector<int> hitEnemyIds;
    bool hitPlayer = false;



    MeeleAttack() : ownerId(-1) {}
    MeeleAttack(ModelObject& attackObject, int id) {
        findTip(attackObject), ownerId= id;
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

    void startMeele(ModelObject& attackObject) {
        if (meeleState != MeeleState::Idle) return;
        meeleState = MeeleState::Swinging;
        swingTimer = 0.0f;
        hasPrevTip = false;
        hitEnemyIds.clear();
        hitPlayer = false;
    }

    void updateMeele(float deltaTime, ModelObject& attackObject, float coolDown) {
        switch (meeleState) {
        case MeeleState::Swinging:
            swingTimer += deltaTime;
            updateSweepCapsule(attackObject);
            if (swingTimer >= swingDuration) {
                meeleState = MeeleState::Cooldown;
                hasPrevTip = false;
                hitEnemyIds.clear();
            }
            break;
        case MeeleState::Cooldown:
            if (coolDown <= 0.0f)
                meeleState = MeeleState::Idle;
            break;
        }
    }

    bool updateSweepCapsule(ModelObject& attackObject) {
        if (meeleState != MeeleState::Swinging) return false;
        glm::vec3 currentTip = getCurrentTipWorld(attackObject);
        glm::vec3 hiltWorld = glm::vec3(attackObject.orientation.modelMatrix * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
        if (!hasPrevTip) {
            prevWorldTip = currentTip;
            capsule.p0 = currentTip;
            capsule.p1 = currentTip;
            capsule.radius = meeleRadius;
            hiltCapsule.p0 = hiltWorld;
            hiltCapsule.p1 = currentTip;
            hiltCapsule.radius = meeleRadius;  
            hasPrevTip = true;
            return false;
        }
        capsule.p0 = prevWorldTip;
        capsule.p1 = currentTip;
        capsule.radius = meeleRadius;
        hiltCapsule.p0 = hiltWorld;
        hiltCapsule.p1 = currentTip;
        hiltCapsule.radius = meeleRadius;
        prevWorldTip = currentTip;
        return true;
    }
};