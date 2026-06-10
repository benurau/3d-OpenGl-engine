#pragma once
#include "Colissions.h"
#include "EnemyManager.h"
#include "ProjectileManager.h"
#include "Player.h"
#include "SceneManager.h"

struct Collider {
    ShapeContact contact;

};

class ColissionManager {
public:
    std::vector<ShapeContact> contacts;


    void CheckProjectileCollision(Player& player, ProjectileManager pManager)
    {
        for (Projectile& p : pManager.projectiles)
        {
            if (!p.active)
                continue;
            if (AABBvsAABB(p.object.colission.worldAABB, player.object.colission.worldAABB))
            {
                printf("player hit\n");
            }
        }
    }

    /*void CheckSceneCollision(Player& player, SceneManager sManager)
    {
        for (auto& object : sManager.meshes)
        {
            ShapeContact tempContact = pointVertBoxCollision(object->colission.vHitbox, player.object.orientation.position + player.movement);
            if (tempContact.isColliding) {
                collided = true;
                player.movement += tempContact.normal * tempContact.penetrationDepth;
                grounded |= isGrounded(tempContact, player.object.colission.worldAABB.min.y);
            }
        }
    }*/



};