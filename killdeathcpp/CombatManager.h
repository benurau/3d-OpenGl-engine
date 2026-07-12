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
                printf("[CombatMgr] Projectile(id=%d) HIT Player(id=%d) dmg=%.0f\n", p.ownerId, player.id, p.type.damage);
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
                    printf("[CombatMgr] Projectile(owner=%d) HIT Enemy(id=%d) dmg=%.0f hp=%d->%d\n", p.ownerId, e.id, p.type.damage, e.health, e.health - (int)p.type.damage);
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
                    printf("[CombatMgr] Projectile(owner=%d) HIT ModelEnemy(id=%d) dmg=%.0f hp=%d->%d\n", p.ownerId, e.id, p.type.damage, e.health, e.health - (int)p.type.damage);
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
            contact = capsuleVsAABB(attack.meele.hiltCapsule, target.object.colission.worldAABB);
        if (!contact.isColliding)
            return;

        glm::vec3 capMid = (attack.meele.capsule.p0 + attack.meele.capsule.p1) * 0.5f;
        glm::vec3 targetPos = (target.object.colission.worldAABB.min + target.object.colission.worldAABB.max) * 0.5f;
        float dist = glm::length(capMid - targetPos);
        printf("[CombatMgr] Melee owner(id=%d) HIT target(id=%d) dmg=%d hp=%d->%d\n", attack.ownerId, target.id, attack.damage, target.health, target.health - attack.damage);
        printf("  capsule p0=(%.2f,%.2f,%.2f) p1=(%.2f,%.2f,%.2f) r=%.2f\n", attack.meele.capsule.p0.x, attack.meele.capsule.p0.y, attack.meele.capsule.p0.z, attack.meele.capsule.p1.x, attack.meele.capsule.p1.y, attack.meele.capsule.p1.z, attack.meele.capsule.radius);
        printf("  target AABB min=(%.2f,%.2f,%.2f) max=(%.2f,%.2f,%.2f) center=(%.2f,%.2f,%.2f)\n", target.object.colission.worldAABB.min.x, target.object.colission.worldAABB.min.y, target.object.colission.worldAABB.min.z, target.object.colission.worldAABB.max.x, target.object.colission.worldAABB.max.y, target.object.colission.worldAABB.max.z, targetPos.x, targetPos.y, targetPos.z);
        printf("  contact normal=(%.2f,%.2f,%.2f) pen=%.2f dist=%.2f\n", contact.normal.x, contact.normal.y, contact.normal.z, contact.penetrationDepth, dist);
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
            {
                printf("[CombatMgr] CollectActiveAttacks: weapon[%d] owner(id=%d) swinging\n", i, w->attack.ownerId);
                activeAttacks.push_back(&w->attack);
            }
        }
        for (EnemyModel& e : enemyManager.modelEnemies)
        {
            if (e.attack.type == AttackType::Meele && 
                e.attack.meele.meeleState == MeeleAttack::MeeleState::Swinging)
            {
                printf("[CombatMgr] CollectActiveAttacks: EnemyModel(id=%d) swinging\n", e.id);
                activeAttacks.push_back(&e.attack);
            }
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