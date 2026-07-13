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