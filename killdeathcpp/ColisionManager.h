#pragma once
#include "Colissions.h"
#include "EnemyManager.h"
#include "ProjectileManager.h"
#include "Player.h"
#include "SceneManager.h"
#include "CollisionResponse.h"
#include "Weapon.h"
#include <cstdio>

struct Collider {
    ShapeContact contact;
};

class ColissionManager {
public:
    std::vector<ShapeContact> contacts;


    void resolvePlayerAABBCollision(Player& player, ObjectCollision& col)
    {

        ShapeContact contact = AABBvsAABBContact(col.worldAABB, player.object.colission.worldAABB);
        {
            if (contact.isColliding) {
                player.grounded |= isGrounded(contact, player.object.colission.worldAABB.min.y);
                glm::vec3 movement = ResolveColissionPushBack(player.movement, contact);
                player.movement = movement;
            }
        }
    }

    void resolvePlayerVertexCollision(Player& player, ObjectCollision& col)
    {
        if (!col.hasVertices) return;

        if (AABBvsAABB(col.worldAABB, player.object.colission.worldAABB))
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
        for (Enemy& e : enemyManager.enemies) {
            if (e.health <= 0)continue;
            resolvePlayerAABBCollision(player, e.object.colission);
        }
        for (EnemyModel& e : enemyManager.modelEnemies)
        {
            if (e.health <= 0)continue;
            resolvePlayerVertexCollision(player, e.object.colission);
            resolvePlayerCapsuleColission(player, e.object.colission, cameraPos);
        }
    }
};
