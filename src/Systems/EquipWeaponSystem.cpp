#include "EquipWeaponSystem.h"
#include "AnimationSystem.h"
#include "EquippedWeaponComponent.h"

void EquipWeaponSystem::init() const {}

void EquipWeaponSystem::update() const {}

void EquipWeaponSystem::equipWeapon(const Entity player, const Entity weapon) const
{
    auto& equipped = gCoordinator.getComponent<EquippedWeaponComponent>(player);
    equipped.currentWeapon = weapon;
}
