#include "InventorySystem.h"
#include "AnimationSystem.h"
#include "InventoryComponent.h"

void InventorySystem::init(){};

void InventorySystem::update() {}

void InventorySystem::pickUpWeapon(const Entity player, const Entity weapon)
{
    auto& inventory = gCoordinator.getComponent<InventoryComponent>(player);
    inventory.weapons.push_back(weapon);
}
