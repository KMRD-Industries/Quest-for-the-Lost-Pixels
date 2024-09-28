#include "WeaponsSystem.h"
#include <RenderSystem.h>
#include <cmath>
#include "AnimationSystem.h"
#include "RenderComponent.h"
#include "WeaponComponent.h"

#define M_PI 3.14159265358979323846

void WeaponSystem::init() {}

void WeaponSystem::update()
{
    for (const auto entity : m_entities)
    {
        updateStartingAngle(entity);
        updateWeaponAngle(entity);
    }
}

inline void WeaponSystem::updateWeaponAngle(const Entity entity)
{
    auto& weaponComponent = gCoordinator.getComponent<WeaponComponent>(entity);

    if (!weaponComponent.isAttacking) return;
    rotateWeapon(entity, weaponComponent.isSwingingForward);
}


inline void WeaponSystem::rotateWeapon(const Entity entity, bool forward)
{
    auto& weaponComponent = gCoordinator.getComponent<WeaponComponent>(entity);

    weaponComponent.remainingDistance -= weaponComponent.rotationSpeed;
    const float direction = weaponComponent.isFacingRight ? 1.f : -1.f;
    const float isMovingForward = forward ? 1.f : -1.f;

    if (weaponComponent.remainingDistance > 0)
    {
        weaponComponent.currentAngle += weaponComponent.rotationSpeed * direction * isMovingForward;
    }
    else
    {
        weaponComponent.currentAngle -= weaponComponent.remainingDistance * direction * isMovingForward;
        weaponComponent.remainingDistance = weaponComponent.swingDistance;
        weaponComponent.isSwingingForward = !forward;

        if (!forward)
        {
            if (weaponComponent.queuedAttack == true)
            {
                weaponComponent.isAttacking = true;
                weaponComponent.queuedAttack = false;

                setAngle(entity);
            }
            else
                weaponComponent.isAttacking = false;
        }
    }
}

inline void WeaponSystem::setAngle(const Entity entity)
{
    auto& weaponComponent = gCoordinator.getComponent<WeaponComponent>(entity);
    auto& renderComponent = gCoordinator.getComponent<RenderComponent>(entity);

    // Reset the remaining distance when not in animating state.
    weaponComponent.remainingDistance = weaponComponent.swingDistance;

    // Get the position of player.
    const auto& origin = renderComponent.sprite.getPosition();

    // Calculate the vector from the player position to the mouse position.
    const sf::Vector2f mouseOffset = weaponComponent.pivotPoint - origin;

    // Update the target point and determine the facing direction.
    weaponComponent.targetPoint = mouseOffset;
    weaponComponent.isFacingRight = mouseOffset.x >= 0;
    const double angleRad = std::atan2(mouseOffset.y, mouseOffset.x);

    constexpr float radToDeg = -180.0f / M_PI;

    // Convert radians into an angle.
    const auto angleInDegrees = static_cast<float>(angleRad * radToDeg);
    weaponComponent.targetAngleDegrees = angleInDegrees;

    // Adjust the current angle based on the facing direction.
    const float adjustedAngle = 90.f - angleInDegrees;
    if (weaponComponent.isFacingRight)
        weaponComponent.currentAngle = adjustedAngle - weaponComponent.initialAngle;
    else
        weaponComponent.currentAngle = adjustedAngle + weaponComponent.initialAngle;
}

inline void WeaponSystem::updateStartingAngle(const Entity entity)
{
    auto& weaponComponent = gCoordinator.getComponent<WeaponComponent>(entity);

    if (weaponComponent.queuedAttack && !weaponComponent.isAttacking)
    {
        weaponComponent.isAttacking = true;
        weaponComponent.queuedAttack = false;
        return;
    }

    if (weaponComponent.queuedAttack || weaponComponent.isAttacking) return;

    setAngle(entity);
}