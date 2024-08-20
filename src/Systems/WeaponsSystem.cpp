#include "WeaponsSystem.h"

#include <RenderSystem.h>
#include <cmath>
#include "AnimationSystem.h"
#include "RenderComponent.h"
#include "WeaponComponent.h"

void WeaponSystem::update() const
{
    for (const auto entity : m_entities)
    {
        updateStartingAngle(entity);
        updateWeaponAngle(entity);
    }
}

void WeaponSystem::updateWeaponAngle(const Entity entity)
{
    const auto& weaponComponent = gCoordinator.getComponent<WeaponComponent>(entity);
    if (!weaponComponent.isAttacking) return;

    (weaponComponent.swingingForward) ? rotateForward(entity) : rotateBackward(entity);
}

void WeaponSystem::rotateBackward(const Entity entity)
{
    auto& weaponComponent = gCoordinator.getComponent<WeaponComponent>(entity);

    // Adjust angle based on facing direction
    const float angleAdjustment =
        weaponComponent.isFacingLeftToRight ? -weaponComponent.rotationSpeed : weaponComponent.rotationSpeed;

    weaponComponent.angle = weaponComponent.isFacingLeftToRight
        ? std::max(weaponComponent.angle + angleAdjustment, weaponComponent.startingAngle)
        : std::min(weaponComponent.angle + angleAdjustment, MAX_LEFT_FACING_ANGLE - weaponComponent.startingAngle);

    // Stop swing and reset attacking state
    if (weaponComponent.angle == weaponComponent.startingAngle ||
        weaponComponent.angle == 420.f - weaponComponent.startingAngle)
    {
        weaponComponent.isAttacking = false;
        weaponComponent.swingingForward = true;
    }
}

void WeaponSystem::rotateForward(const Entity entity)
{
    auto& weaponComponent = gCoordinator.getComponent<WeaponComponent>(entity);

    const float angleAdjustment =
        weaponComponent.isFacingLeftToRight ? weaponComponent.rotationSpeed : -weaponComponent.rotationSpeed;

    // Adjust angle based on facing direction
    weaponComponent.angle = weaponComponent.isFacingLeftToRight
        ? std::min(weaponComponent.angle + angleAdjustment, weaponComponent.maxAngle)
        : std::max(weaponComponent.angle + angleAdjustment,
                   weaponComponent.maxAngle + (180 - weaponComponent.maxAngle) * 2);

    // Stop swing at bottom and force swinging backward
    if (weaponComponent.angle == weaponComponent.maxAngle ||
        weaponComponent.angle == weaponComponent.maxAngle + (180 - weaponComponent.maxAngle) * 2)
    {
        weaponComponent.swingingForward = false;
    }
}

void WeaponSystem::updateStartingAngle(const Entity entity)
{
    auto& weaponComponent = gCoordinator.getComponent<WeaponComponent>(entity);
    const auto& renderComponent = gCoordinator.getComponent<RenderComponent>(entity);

    if (weaponComponent.isAttacking) return;

    const auto& origin = renderComponent.sprite.getPosition();

    weaponComponent.atan = {weaponComponent.pivot.x - static_cast<int>(origin.x),
                            weaponComponent.pivot.y - static_cast<int>(origin.y)};

    weaponComponent.isFacingLeftToRight = (weaponComponent.atan.x >= 0);

    const float angle = std::atan2(weaponComponent.atan.y, weaponComponent.atan.x);
    auto angleInDegrees = static_cast<float>(angle * (180.0f / M_PI));

    // Adjust the angle to the mirrored starting position if the weapon is facing left to right
    weaponComponent.angle = (weaponComponent.isFacingLeftToRight)
        ? (weaponComponent.startingAngle + angleInDegrees + 30)
        : (angleInDegrees + 130.f);
}
