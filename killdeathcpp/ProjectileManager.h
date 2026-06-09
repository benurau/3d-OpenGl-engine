#pragma once
#include "Projectile.h"
#include "Renderer.h"


class ProjectileManager {
public:
    std::vector<Projectile> projectiles;

    void AddProjectile(const Projectile& prototype, int count)
    {
        for (int i = 0; i < count; i++)
            projectiles.push_back(prototype);
    }

    void Update(float dt)
    {
        for (Projectile& p : projectiles)
        {
            if (!p.active)
                continue;
            UpdateProjectile(p, dt);
            p.object.colission.updateWorldAABBV(p.object.orientation.modelMatrix);
        }
    }

    void Render(Renderer& renderer, Material& material, Camera& camera)
    {
        for (Projectile& p : projectiles)
        {
            if (!p.active)
                continue;
            p.object.orientation.changeView(camera.GetViewMatrix());
            renderer.draw(p.object.mesh, p.object.orientation, material);
        }
    }

    void CheckPlayerCollision(Player& player)
    {
        for (Projectile& p : projectiles)
        {
            if (!p.active)
                continue;

            if (AABBvsAABB(p.object.colission.worldAABB, player.object.colission.worldAABB))
            {
                printf("player hit\n");
            }
        }
    }

};

