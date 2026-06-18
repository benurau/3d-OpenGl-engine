#pragma once
#include "objects.h"


struct ProjectileType {
    Mesh mesh;
    float speed;
    float lifetime;
    float damage;
};

struct Projectile
{
    MeshObject object;
    ProjectileType type;
    glm::vec3 velocity;
    bool active;
    int ownerId = -1;
};

void SpawnProjectile(glm::vec3& position, glm::vec3& direction, ProjectileType& type, std::vector<Projectile>& projectiles, int ownerId = -1) {
    if (projectiles.empty()) printf("projectiles list empty in attack spawnprojectile function undfefined behaviour!!");
    for (Projectile& p : projectiles)
    {
        if (!p.active)
        {
            p.type.mesh = type.mesh;
            p.object.orientation.setPos(position);
            p.object.colission.updateWorldAABB(p.object.orientation.modelMatrix);
            p.velocity = direction * type.speed;
            p.type.lifetime = type.lifetime;
            p.active = true;
            p.ownerId = ownerId;
            break;
        }
    }
}

void UpdateProjectile(Projectile& p, float& dt)
{
    p.object.orientation.movePos(p.velocity * dt);
    p.type.lifetime -= dt;

    if (p.type.lifetime <= 0.0f)
    {
        p.active = false;
    }
}