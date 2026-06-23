#pragma once
#include "objects.h"
#include "Projectile.h"
#include "MathHelpers.h"


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

    int attackAnimation;
    int chaseAnimation;

    EnemyState state;
    float stateTimer;

    ProjectileType ptype;
    float shootCooldown = 2.0f;
    float shootTimer = 1.0f;

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
            e.object.model.setAnimation(e.attackAnimation);
            SpawnProjectile(e.object.orientation.position, dir, e.ptype, projectiles, e.id);
            e.shootTimer = e.shootCooldown;
        }
        break;
    }
    }
}