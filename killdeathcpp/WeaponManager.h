#pragma once
#include "Weapon.h"

class ColissionManager;
class EnemyManager;

// SUGGESTION (#23): Replace raw weapon* array with std::array<std::reference_wrapper<weapon>, MAX_WEAPONS>
// or std::vector<std::reference_wrapper<weapon>> for type-safe, bounds-checked access.
// Current raw pointer array works but has no ownership semantics and no bounds safety beyond manual checks.
struct WeaponManager {
    static const int MAX_WEAPONS = 3;
    weapon* weapons[MAX_WEAPONS] = {};
    int weaponCount = 0;
    int activeIndex = 0;

    bool addWeapon(weapon& w) {
        if (weaponCount >= MAX_WEAPONS) return false;
        weapons[weaponCount++] = &w;
        return true;
    }

    weapon* getActiveWeapon() {
        return weaponCount > 0 ? weapons[activeIndex] : nullptr;
    }

    void setActiveWeapon(int index) {
        if (index >= 0 && index < weaponCount)
            activeIndex = index;
    }

    void Update(Camera& camera, Renderer& renderer, ColissionManager& colMgr, EnemyManager& enemyManager, float deltaTime) {
        weapon* w = getActiveWeapon();
        if (!w) return;
        w->Update(camera, renderer, deltaTime);
    }
};
