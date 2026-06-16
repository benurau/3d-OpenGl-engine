#pragma once
#include "objects.h"
#include "Projectile.h"
#include "Renderer.h"

struct weapon {
    ModelObject& weaponObject;
    ProjectileType projectileT;
    float shootTimer = 0.0f;
    float shootCooldown = 0.25f;

    weapon(ModelObject& obj) : weaponObject(obj) {}

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
