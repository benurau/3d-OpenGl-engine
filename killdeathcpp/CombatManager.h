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
    void CheckHitTarget(T& target, Attack& attack)
    {
        if (!target.alive || target.health <= 0 || target.id == attack.ownerId)
            return;

        if (target.id == -1) {
            if (attack.meele.hitPlayer) return;
        } else {
            if (std::find(attack.meele.hitEnemyIds.begin(), attack.meele.hitEnemyIds.end(), target.id) != attack.meele.hitEnemyIds.end()) return;
        }

        ShapeContact contact = capsuleVsAABB(attack.meele.capsule, target.object.colission.worldAABB);

        if (!contact.isColliding)
            return;

        target.health -= attack.damage;
        target.velocity += contact.normal * contact.penetrationDepth * 5.0f;

        if (target.id == -1) {
            attack.meele.hitPlayer = true;
        } else {
            attack.meele.hitEnemyIds.push_back(target.id);
        }
    }


    void CollectActiveAttacks(WeaponManager& weaponManager, EnemyManager& enemyManager) {
        activeAttacks.clear();
        for (int i = 0; i < weaponManager.weaponCount; i++)
        {
            weapon* w = weaponManager.weapons[i];
            if (w && w->attack.meele.meeleState == MeeleAttack::MeeleState::Swinging)
                activeAttacks.push_back(&w->attack);
        }
        for (EnemyModel& e : enemyManager.modelEnemies)
        {
            if (e.attack.type == AttackType::Meele && 
                e.attack.meele.meeleState == MeeleAttack::MeeleState::Swinging)
                activeAttacks.push_back(&e.attack);
        }
    }

    void CheckMeeleSweeps(EnemyManager& enemyManager, Player& player)
    {
        for (Attack* attack : activeAttacks)
        {
            for (Enemy& e : enemyManager.enemies)
                CheckHitTarget(e, *attack);

            for (EnemyModel& e : enemyManager.modelEnemies)
                CheckHitTarget(e, *attack);

            CheckHitTarget(player, *attack);
        }
    }

};