#pragma once
#include "objects.h"

struct Player {
    MeshObject object;
    float movementSpeed = 2.5f;
    float cameraHeight = 0.5;
    float smashSpeed = -4.0f;
    glm::vec3 movement{ 0.0f };
    float jumpHeight = 1.0f;
    float verticalVelocity = 0.0;
    bool grounded = false;

    float health = 100;
    bool alive = true;
    int id = -1;
    glm::vec3 velocity{ 0.0f };

    float dashSpeed = 8.0f;
    float dashDuration = 0.2f;
    float dashCooldown = 1.5f;
    float dashTimer = 0.0f;
    float dashCooldownTimer = 0.0f;
    bool isDashing = false;
    bool smashUsed = false;
};

void ProcessViewControls(Player& player, Camera_Movement direction, Camera& camera, float deltaTime)
{
    float speed = player.movementSpeed * deltaTime;
    glm::vec3 flatFront = glm::normalize(glm::vec3(camera.Front.x, 0.0f, camera.Front.z));

    if (direction == FORWARD)
    {
        player.movement += flatFront * speed;
    }
    if (direction == BACKWARD)
    {
        player.movement += -flatFront * speed;
    }
    if (direction == LEFT)
    {
        player.movement += -camera.Right * speed;
    }
    if (direction == RIGHT)
    {
        player.movement += camera.Right * speed;
    }
    if (direction == UP && player.grounded)
    {
        player.verticalVelocity = player.jumpHeight;
    }
    if (direction == DOWN && !player.grounded)
    {
        player.verticalVelocity = player.smashSpeed;
        player.smashUsed = true;
    }
}

void PlayerDash(Player& player, Camera& camera, GLFWwindow* window, float deltaTime) {
    if (player.dashCooldownTimer > 0.0f || player.isDashing) return;

    bool w = glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS;
    bool a = glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS;
    bool s = glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS;
    bool d = glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS;

    glm::vec3 flatFront = glm::normalize(glm::vec3(camera.Front.x, 0.0f, camera.Front.z));
    glm::vec3 dir(0.0f);

    if (w) dir += flatFront;
    if (s) dir -= flatFront;
    if (d) dir += camera.Right;
    if (a) dir -= camera.Right;

    if (glm::length(dir) < 0.01f) dir = flatFront;
    else dir = glm::normalize(dir);

    player.movement += dir * player.dashSpeed * deltaTime;
    player.movementSpeed = player.dashSpeed;
    player.isDashing = true;
    player.dashTimer = player.dashDuration;
    player.dashCooldownTimer = player.dashCooldown;
}

void applyGravity(Player& player, float deltaTime) {
    if (player.isDashing) {
        if (player.smashUsed) {
            player.movement.y = player.verticalVelocity * deltaTime;
        } else {
            player.movement.y = 0.0f;
        }
        return;
    }
    if (!player.grounded) player.verticalVelocity += GRAVITY * deltaTime;
    player.movement.y = player.verticalVelocity * deltaTime;
}

void updatePlayer(Player& player, glm::vec3& originalMovement, float deltaTime) {
    if (player.isDashing) {
        player.dashTimer -= deltaTime;
        if (player.dashTimer <= 0.0f) {
            player.isDashing = false;
            player.movementSpeed = 2.5f;
        }
    }
    if (player.dashCooldownTimer > 0.0f) {
        player.dashCooldownTimer -= deltaTime;
    }
    player.smashUsed = false;

    player.object.orientation.movePos(player.movement);
    player.movement = glm::vec3(0.0f);
    applyGravity(player, deltaTime);
    player.object.colission.updateWorldAABB(player.object.orientation.modelMatrix);
}

