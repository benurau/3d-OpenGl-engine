#pragma once
#include "Player.h"
#include "EnemyManager.h"

struct GameState {
    struct PlayerData {
        glm::vec3 position;
        float health;
        glm::vec3 movement;
        float verticalVelocity;
        bool grounded;
    } playerData;

    struct EnemyData {
        glm::vec3 position;
        glm::vec3 velocity;
        int health;
        EnemyState state;
        float stateTimer;
        float shootTimer;
        bool alive;
    };

    struct EnemyModelData {
        glm::vec3 position;
        glm::vec3 velocity;
        int health;
        EnemyState state;
        float stateTimer;
        float shootTimer;
        bool alive;
    };

    std::vector<EnemyData> enemies;
    std::vector<EnemyModelData> modelEnemies;

    void Store(Player& player, EnemyManager& mgr)
    {
        playerData.position = player.object.orientation.position;
        playerData.health = player.health;
        playerData.movement = player.movement;
        playerData.verticalVelocity = player.verticalVelocity;
        playerData.grounded = player.grounded;

        enemies.clear();
        for (auto& e : mgr.enemies)
            enemies.push_back({ e.object.orientation.position, e.velocity, e.health, e.state, e.stateTimer, e.shootTimer, e.alive });

        modelEnemies.clear();
        for (auto& e : mgr.modelEnemies)
            modelEnemies.push_back({ e.object.orientation.position, e.velocity, e.health, e.state, e.stateTimer, e.shootTimer, e.alive });
    }

    void Restore(Player& player, EnemyManager& mgr)
    {
        player.object.orientation.setPos(playerData.position);
        player.health = playerData.health;
        player.movement = playerData.movement;
        player.verticalVelocity = playerData.verticalVelocity;
        player.grounded = playerData.grounded;

        for (size_t i = 0; i < enemies.size() && i < mgr.enemies.size(); ++i)
        {
            mgr.enemies[i].object.orientation.setPos(enemies[i].position);
            mgr.enemies[i].velocity = enemies[i].velocity;
            mgr.enemies[i].health = enemies[i].health;
            mgr.enemies[i].state = enemies[i].state;
            mgr.enemies[i].stateTimer = enemies[i].stateTimer;
            mgr.enemies[i].shootTimer = enemies[i].shootTimer;
            mgr.enemies[i].alive = enemies[i].alive;
        }

        for (size_t i = 0; i < modelEnemies.size() && i < mgr.modelEnemies.size(); ++i)
        {
            mgr.modelEnemies[i].object.orientation.setPos(modelEnemies[i].position);
            mgr.modelEnemies[i].velocity = modelEnemies[i].velocity;
            mgr.modelEnemies[i].health = modelEnemies[i].health;
            mgr.modelEnemies[i].state = modelEnemies[i].state;
            mgr.modelEnemies[i].stateTimer = modelEnemies[i].stateTimer;
            mgr.modelEnemies[i].shootTimer = modelEnemies[i].shootTimer;
            mgr.modelEnemies[i].alive = modelEnemies[i].alive;
        }
    }
};
