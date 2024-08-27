//
// Created by dominik-szot on 28.07.24.
//

#include "EquipWeaponSystem.h"

#include "AnimationSystem.h"
#include "EquippedWeaponComponent.h"

void EquipWeaponSystem::equipWeapon(const Entity player, const Entity weapon)
{
    auto& equipped = gCoordinator.getComponent<EquippedWeaponComponent>(player);
    gCoordinator.destroyEntity(equipped.currentWeapon);
    equipped.currentWeapon = weapon;
}
