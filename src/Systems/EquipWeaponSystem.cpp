#include "EquipWeaponSystem.h"
#include "AnimationSystem.h"
#include "EquippedWeaponComponent.h"

void EquipWeaponSystem::init() {}

void EquipWeaponSystem::update() {}

void EquipWeaponSystem::equipWeapon(const Entity player, const Entity weapon)
{
    auto& equipped = gCoordinator.getComponent<EquippedWeaponComponent>(player);
    equipped.currentWeapon = weapon;
}
