#pragma once
#include "Colissions.h"
#include "EnemyManager.h"
#include "ProjectileManager.h"
#include "Player.h"
#include "SceneManager.h"
#include "CollisionResponse.h"
#include <cstdio>

struct Collider {
    ShapeContact contact;
};

class ColissionManager {
public:
    std::vector<ShapeContact> contacts;

    AABB predictPlayerAABB(const Player& player) const {
        AABB next = player.object.colission.worldAABB;
        next.min += player.movement;
        next.max += player.movement;
        return next;
    }

    void resolvePlayerVertexCollision(Player& player, ObjectCollision& col)
    {
        if (!col.hasVertices) return;
        AABB nextPlayerAABB = predictPlayerAABB(player);


        if (AABBvsAABB(col.worldAABB, nextPlayerAABB))
        {
            ShapeContact contact = pointVertBoxCollision(col.getVerticeHitBox(), player.object.orientation.position+player.movement);
            if (contact.isColliding) {
                player.grounded |= isGrounded(contact, player.object.colission.worldAABB.min.y);
                glm::vec3 movement = ResolveColissionPushBack(player.movement, contact);
                player.movement = movement;
            }
        }
    }

    void resolvePlayerCapsuleColission(Player& player, ObjectCollision& col, const glm::vec3& cameraPos)
    {
        if (!col.hasCapsules) return;
        glm::vec3 checkPos = player.object.orientation.position + player.movement;
        if (AABBPointColission(col.worldAABB, checkPos))
        {
            for (const CapsuleHitBoxWorld& box : col.getCapsuleLocs())
            {
                ShapeContact contact = pointInCapsule(cameraPos + player.movement, box.worldLoc);
                if (contact.isColliding) {
                    player.grounded |= isGrounded(contact, player.object.colission.worldAABB.min.y);
                    player.movement = ResolveColissionPushBack(player.movement, contact);
                }
            }
        }
    }

    void CheckProjectileCollision(Player& player, ProjectileManager& pManager)
    {
        for (Projectile& p : pManager.projectiles)
        {
            if (!p.active) continue;
            if (AABBvsAABB(p.object.colission.worldAABB, player.object.colission.worldAABB))
            {
                printf("player hit\n");
                p.active = false;
            }
        }
    }

    void CheckSceneCollision(Player& player, SceneManager& scene, const glm::vec3& cameraPos)
    {
        for (MeshObject* m : scene.meshes) {
            resolvePlayerVertexCollision(player, m->colission);
        }
        for (ModelObject* m : scene.models)
        {
            resolvePlayerVertexCollision(player, m->colission);
            resolvePlayerCapsuleColission(player, m->colission, cameraPos);
        }
    }

    void CheckEnemyCollision(Player& player, EnemyManager& enemyManager, const glm::vec3& cameraPos)
    {
        for (Enemy& e : enemyManager.enemies)
            resolvePlayerVertexCollision(player, e.object.colission);
        for (EnemyModel& e : enemyManager.modelEnemies)
        {
            resolvePlayerVertexCollision(player, e.object.colission);
            resolvePlayerCapsuleColission(player, e.object.colission, cameraPos);
        }
    }
};
