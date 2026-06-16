#pragma once
#include "Enemy.h"
#include "ProjectileManager.h"
#include "Renderer.h"
#include "Camera.h"
#include "Material.h"

class EnemyManager {
public:
    std::vector<Enemy> enemies;
    std::vector<EnemyModel> modelEnemies;

    void AddEnemy(const Enemy& enemy)
    {
        enemies.push_back(enemy);
    }

    void AddEnemy(const EnemyModel& enemy)
    {
        modelEnemies.push_back(enemy);
    }

    void Update(float dt, const glm::vec3& playerPos, ProjectileManager& projectileManager)
    {
        for (Enemy& e : enemies)
        {
            std::cout << e.health<<"enemy healht currently; mrrreeesh \n";
            if (e.health < 0) { continue; }
            UpdateEnemy(e, playerPos, dt, projectileManager.projectiles);

            e.object.colission.updateWorldAABBV(e.object.orientation.modelMatrix);
        }

        for (EnemyModel& e : modelEnemies)
        {
            std::cout << e.health << "enemy healht currently modeeel;\n";
            if (e.health < 0) { continue; }
            UpdateEnemy(e, playerPos, dt, projectileManager.projectiles);

            e.object.model.updateAnimation(dt);
            e.object.model.updateNodeTransforms();
            e.object.model.updateSkins();

            e.object.colission.updateModelAABBskins(e.object.model);
            e.object.colission.updateWorldAABB(e.object.orientation.modelMatrix);
            e.object.colission.updateCapsuleLocs(e.object.model, e.object.orientation);
        }
    }

    void Render(Renderer& renderer, Material& material, Camera& camera)
    {
        for (Enemy& e : enemies)
        {
            if (e.health < 0) { continue; }
            e.object.orientation.changeView(camera.GetViewMatrix());
            renderer.draw(e.object.mesh, e.object.orientation, material);
        }

        for (EnemyModel& e : modelEnemies)
        {
            if (e.health < 0) { continue; }
            e.object.orientation.changeView(camera.GetViewMatrix());
            renderer.drawModel(e.object.model, e.object.orientation);
        }
    }


};