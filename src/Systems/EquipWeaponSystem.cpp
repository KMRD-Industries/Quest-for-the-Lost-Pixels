#include "EquipWeaponSystem.h"
#include "AnimationSystem.h"
#include "EquippedHelmetComponent.h"
#include "EquippedWeaponComponent.h"
#include "HelmetComponent.h"
#include "WeaponComponent.h"

void EquipWeaponSystem::init() const {}

void EquipWeaponSystem::update() const {}

void EquipWeaponSystem::equipWeapon(const Entity player, const Entity weapon) const
{
    auto& equipped = gCoordinator.getComponent<EquippedWeaponComponent>(player);
    equipped.currentWeapon = weapon;
    gCoordinator.getComponent<WeaponComponent>(weapon).equipped = true;
}

void EquipWeaponSystem::equipHelmet(const Entity player, const Entity helmet) const
{
    auto& equipped = gCoordinator.getComponent<EquippedHelmetComponent>(player);
    equipped.currentHelmet = helmet;
    gCoordinator.getComponent<HelmetComponent>(helmet).equipped = true;
}
