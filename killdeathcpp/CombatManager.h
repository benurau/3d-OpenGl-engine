#pragma once
#include "EnemyManager.h"
#include "Attack.h"
#include "Player.h"


struct CombatManager {
    std::vector<Attack*> activeAttacks;


    void CheckProjectileCollision(Player& player, ProjectileManager& pManager)
    {
        for (Projectile& p : pManager.projectiles)
        {
            if (!p.active) continue;
            if (p.ownerId == -1) continue;
            if (AABBvsAABB(p.object.colission.worldAABB, player.object.colission.worldAABB))
            {
                player.health -= p.type.damage;
                p.active = false;
            }
        }
    }

    void CheckProjectileEnemyCollision(ProjectileManager& pManager, EnemyManager& enemyManager)
    {
        for (Projectile& p : pManager.projectiles)
        {
            if (!p.active) continue;
            for (Enemy& e : enemyManager.enemies)
            {
                if (e.id == p.ownerId) continue;
                if (AABBvsAABB(p.object.colission.worldAABB, e.object.colission.worldAABB))
                {
                    e.health -= p.type.damage;
                    p.active = false;
                    break;
                }
            }
            if (!p.active) continue;
            for (EnemyModel& e : enemyManager.modelEnemies)
            {
                if (e.id == p.ownerId) continue;
                if (AABBvsAABB(p.object.colission.worldAABB, e.object.colission.worldAABB))
                {
                    e.health -= p.type.damage;
                    p.active = false;
                    break;
                }
            }
        }
    }

    template<typename T>
    void CheckEnemyHitMeele(T& e, Attack& attack)
    {
        if (!e.alive || e.health <= 0 || e.id == attack.ownerId)
            return;

        if (std::find(attack.meele.hitEnemyIds.begin(), attack.meele.hitEnemyIds.end(), e.id) != attack.meele.hitEnemyIds.end()) return;

        ShapeContact contact = capsuleVsAABB(attack.meele.capsule, e.object.colission.worldAABB);

        if (!contact.isColliding)
            return;
        printf("checkenemy hit triggered colliding hitbox \n");

        e.health -= attack.damage;
        e.velocity += contact.normal * contact.penetrationDepth * 5.0f;

        attack.meele.hitEnemyIds.push_back(e.id);
    }


    void CollectActiveAttacks(WeaponManager& weaponManager, EnemyManager& enemyManager) {
        activeAttacks.clear();
        for (int i = 0; i < weaponManager.weaponCount; i++)
        {
            weapon* w = weaponManager.weapons[i];
            if (w && w->attack.meele.meeleState == MeeleAttack::MeeleState::Swinging)
                activeAttacks.push_back(&w->attack);
        }
        /*for (auto& e : enemyManager.enemies)
            if (e.meeleAttack.meeleState == MeeleAttack::MeeleState::Swinging)
                activeAttacks.push_back(&e.meeleAttack);*/
    }

    void CheckMeeleSweeps(EnemyManager& enemyManager, Player& player)
    {
        for (Attack* attack : activeAttacks)
        {
            for (Enemy& e : enemyManager.enemies)
                CheckEnemyHitMeele(e, *attack);

            for (EnemyModel& e : enemyManager.modelEnemies)
                CheckEnemyHitMeele(e, *attack);

            //CheckEnemyHitMeele(player, *attack);
        }
    }

    

};