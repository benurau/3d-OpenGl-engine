#pragma once
#include "objects.h"
#include "Projectile.h"
#include "MathHelpers.h"
#include "Attack.h"


enum EnemyState
{
    IDLE,
    PATROL,
    CHASE,
    ATTACK,
    HIT,
    DEAD
};

struct Enemy {
    MeshObject object;

    glm::vec3 velocity;
    float moveSpeed = 1.0f;

    int health = 100;
    int damage;

    float attackRange;

    EnemyState state;
    float stateTimer;

    ProjectileType ptype;
    float shootCooldown = 2.0f;
    float shootTimer = 5.0f;

    bool alive = true;
    static inline int nextId = 0;
    int id = nextId++;
};

struct EnemyModel {
    ModelObject object;

    glm::vec3 velocity;
    float moveSpeed = 1.0f;

    int health = 100;
    int damage;

    float attackRange;
    float attackLength;

    int chaseAnimation;

    EnemyState state;
    float stateTimer;

    Attack attack;

    bool alive = true;
    static inline int nextId = 0;
    int id = nextId++;
};

void UpdateEnemy(Enemy& e, glm::vec3 targetPosition, float dt, std::vector<Projectile>& projectiles) {
    switch (e.state)
    {
    case CHASE:
    {
        glm::vec3 dir = CalculateDirection(e.object.orientation.position, targetPosition);
        float dist = glm::length(targetPosition - e.object.orientation.position);
        if (dist < e.attackRange)
            e.state = ATTACK;
        else {
            float desiredYaw = CalculateYawToTarget(e.object.orientation.position, targetPosition);
            e.object.orientation.rotate(glm::vec3(0.0f, desiredYaw - e.object.orientation.rotation.y, 0.0f));
            e.object.orientation.movePos(dir * e.moveSpeed * dt);
        }
        break;
    }
    case ATTACK:
    {
        e.shootTimer -= dt;
        glm::vec3 dir = CalculateDirection(e.object.orientation.position, targetPosition);
        float dist = glm::length(targetPosition - e.object.orientation.position);
        if (dist > e.attackRange) {
            e.state = CHASE;
        }
        else if (e.shootTimer <= 0.0f)
        {
            SpawnProjectile(e.object.orientation.position, dir, e.ptype, projectiles, e.id);
            e.shootTimer = e.shootCooldown;
        }
        break;
    }
    }
}

void UpdateEnemy(EnemyModel& e, glm::vec3 targetPosition, float dt, std::vector<Projectile>& projectiles) {
    switch (e.state)
    {
    case CHASE:
    {
        glm::vec3 dir = CalculateDirection(e.object.orientation.position, targetPosition);
        float dist = glm::length(targetPosition - e.object.orientation.position);
        if (dist < e.attackRange)
            e.state = ATTACK;
        else {
            e.object.model.setAnimation(e.chaseAnimation);
            float desiredYaw = CalculateYawToTarget(e.object.orientation.position, targetPosition);
            e.object.orientation.rotate(glm::vec3(0.0f, desiredYaw - e.object.orientation.rotation.y, 0.0f));
            e.object.orientation.movePos(dir * e.moveSpeed * dt);
        }
        break;
    }
    case ATTACK:
    {
        glm::vec3 dir = CalculateDirection(e.object.orientation.position, targetPosition);
        float dist = glm::length(targetPosition - e.object.orientation.position);

        if (dist > e.attackRange) {
            e.state = CHASE;
        }
        else {
            if (e.attack.type == AttackType::Meele) {
                float tipAngle = glm::degrees(atan2(e.attack.meele.localMeshTip.x, e.attack.meele.localMeshTip.z));
                float desiredYaw = CalculateYawToTarget(e.object.orientation.position, targetPosition) - tipAngle;
                e.object.orientation.rotate(glm::vec3(0.0f, desiredYaw - e.object.orientation.rotation.y, 0.0f));

                float meleeReach = e.attack.meele.localMeshTip.length() + e.attack.meele.meeleRadius;
                printf("[EnemyModel] ATTACK melee: dist=%.3f meleeReach=%.3f tipLen=%.3f radius=%.3f pos=(%.2f,%.2f,%.2f) target=(%.2f,%.2f,%.2f)\n",
                    dist, meleeReach, e.attack.meele.localMeshTip.length(), e.attack.meele.meeleRadius,
                    e.object.orientation.position.x, e.object.orientation.position.y, e.object.orientation.position.z,
                    targetPosition.x, targetPosition.y, targetPosition.z);
                if (dist > meleeReach) {
                    e.object.orientation.movePos(dir * e.moveSpeed * dt);
                }
            } else {
                float desiredYaw = CalculateYawToTarget(e.object.orientation.position, targetPosition);
                e.object.orientation.rotate(glm::vec3(0.0f, desiredYaw - e.object.orientation.rotation.y, 0.0f));
            }

            e.attack.Update(dt, e.object);
            if (e.attack.timer <= 0.0f)
            {
                e.attack.StartAttack(e.object, dir, projectiles);
            }
        }
        break;
    }
    }
}