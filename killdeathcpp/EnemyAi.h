#pragma once
#include "Enemy.h"
#include "MathHelpers.h"
#include <vector>

inline void ChasePlayer(Enemy& e, glm::vec3 targetPosition, float dt) {
    glm::vec3 dir = CalculateDirection(e.object.orientation.position, targetPosition);
    float dist = glm::length(targetPosition - e.object.orientation.position);
    if (dist < e.attackRange) {
        e.state = ATTACK;
    }
    else {
        float desiredYaw = CalculateYawToTarget(e.object.orientation.position, targetPosition);
        e.object.orientation.rotate(glm::vec3(0.0f, desiredYaw - e.object.orientation.rotation.y, 0.0f));
        e.object.orientation.movePos(dir * e.moveSpeed * dt);
    }
}

inline void ChasePlayer(EnemyModel& e, glm::vec3 targetPosition, float dt) {
    glm::vec3 dir = CalculateDirection(e.object.orientation.position, targetPosition);
    float dist = glm::length(targetPosition - e.object.orientation.position);
    if (dist < e.attackRange) {
        e.state = ATTACK;
    }
    else {
        e.object.model.setAnimation(e.chaseAnimation);
        float desiredYaw = CalculateYawToTarget(e.object.orientation.position, targetPosition);
        e.object.orientation.rotate(glm::vec3(0.0f, desiredYaw - e.object.orientation.rotation.y, 0.0f));
        e.object.orientation.movePos(dir * e.moveSpeed * dt);
    }
}

inline void AttackPlayer(Enemy& e, glm::vec3 targetPosition, float dt, std::vector<Projectile>& projectiles) {
    e.shootTimer -= dt;
    glm::vec3 dir = CalculateDirection(e.object.orientation.position, targetPosition);
    float dist = glm::length(targetPosition - e.object.orientation.position);
    if (dist > e.attackRange) {
        e.state = CHASE;
    }
    else if (e.shootTimer <= 0.0f) {
        SpawnProjectile(e.object.orientation.position, dir, e.ptype, projectiles, e.id);
        e.shootTimer = e.shootCooldown;
    }
}

inline void AttackPlayer(EnemyModel& e, glm::vec3 targetPosition, float dt, std::vector<Projectile>& projectiles) {
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
        }
        else {
            float desiredYaw = CalculateYawToTarget(e.object.orientation.position, targetPosition);
            e.object.orientation.rotate(glm::vec3(0.0f, desiredYaw - e.object.orientation.rotation.y, 0.0f));
        }

        e.attack.Update(dt, e.object);
        if (e.attack.timer <= 0.0f) {
            e.attack.StartAttack(e.object, dir, projectiles);
        }
    }
}

inline void UpdateEnemyAI(Enemy& e, glm::vec3 targetPosition, float dt, std::vector<Projectile>& projectiles) {
    switch (e.state) {
    case CHASE:
        ChasePlayer(e, targetPosition, dt);
        break;
    case ATTACK:
        AttackPlayer(e, targetPosition, dt, projectiles);
        break;
    }
}

inline void UpdateEnemyAI(EnemyModel& e, glm::vec3 targetPosition, float dt, std::vector<Projectile>& projectiles) {
    switch (e.state) {
    case CHASE:
        ChasePlayer(e, targetPosition, dt);
        break;
    case ATTACK:
        AttackPlayer(e, targetPosition, dt, projectiles);
        break;
    }
}

inline void UpdateEnemy(Enemy& e, glm::vec3 targetPosition, float dt, std::vector<Projectile>& projectiles) {
    UpdateEnemyAI(e, targetPosition, dt, projectiles);
}

inline void UpdateEnemy(EnemyModel& e, glm::vec3 targetPosition, float dt, std::vector<Projectile>& projectiles) {
    UpdateEnemyAI(e, targetPosition, dt, projectiles);
}
