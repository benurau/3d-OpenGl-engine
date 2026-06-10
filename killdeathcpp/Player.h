#pragma once
#include "objects.h"

struct Player {
    MeshObject object;
    float movementSpeed = 2.5f;
    float cameraHeight = 0.5;
    float smashSpeed = -4.0f;
    glm::vec3 movement{ 0.0f };
    float jumpHeight = 1.0f;
    float verticalVelocity;
    bool grounded = false;
};

void ProcessViewControls(Player& player, Camera_Movement direction, Camera& camera, float deltaTime)
{
    float velocity = player.movementSpeed * deltaTime;
    glm::vec3 flatFront = glm::normalize(glm::vec3(camera.Front.x, 0.0f, camera.Front.z));

    if (direction == FORWARD)
    {
        player.movement += flatFront * velocity;
    }
    if (direction == BACKWARD)
    {
        player.movement += -flatFront * velocity;
    }
    if (direction == LEFT)
    {
        player.movement += -camera.Right * velocity;
    }
    if (direction == RIGHT)
    {
        player.movement += camera.Right * velocity;
    }
    if (direction == UP && player.grounded)
    {
        player.verticalVelocity = player.jumpHeight;
    }
    if (direction == DOWN && !player.grounded)
    {
        player.verticalVelocity = player.smashSpeed;
    }
}

void applyGravity(Player& player, float deltaTime) {
    if (!player.grounded) player.verticalVelocity += GRAVITY * deltaTime;
    player.movement.y = player.verticalVelocity * deltaTime;
}

void updatePlayer(Player& player, glm::vec3& originalMovement, float deltaTime) {
    player.object.orientation.movePos(player.movement);
    player.movement = glm::vec3(0.0f);
    applyGravity(player, deltaTime);
    player.object.colission.updateWorldAABB(player.object.orientation.modelMatrix);
}

