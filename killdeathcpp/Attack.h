#pragma once

#include "meele.h"
#include "Projectile.h"

enum class AttackType
{
    Meele,
    Projectile
};

struct Attack
{
    AttackType type = AttackType::Meele;

    float cooldown = 0.5f;
    float timer = 0.0f;
    int damage = 10.0f;

    MeeleAttack meele;
    Projectile projectile;

    int ownerId = -1;

    Attack() : ownerId(-1) {}
    Attack(ModelObject& object, int _ownerId)
        : meele(object, _ownerId), ownerId(_ownerId)
    {
    }

    void Update(float dt, ModelObject& object)
    {
        if (timer > 0.0f)
            timer -= dt;

        switch (type)
        {
        case AttackType::Meele:
            meele.updateMeele(dt, object, timer);
            break;

        case AttackType::Projectile:
            break;
        }
    }

    void StartAttack(ModelObject& object, glm::vec3 direction, std::vector<Projectile>& projectilePool)
    {
        if (timer > 0.0f)
            return;

        switch (type)
        {
        case AttackType::Meele:
            meele.startMeele(object);
            break;

        case AttackType::Projectile:
        {
            object.model.setAnimation(0, true);
            SpawnProjectile( object.orientation.position, direction, projectile.type, projectilePool, ownerId);
            break;
        }
        }

        timer = cooldown;
    }

    bool IsAttacking() const
    {
        return meele.meeleState == MeeleAttack::MeeleState::Swinging;
    }

    
};